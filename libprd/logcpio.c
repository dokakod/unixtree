/*------------------------------------------------------------------------
 * log a "cpio" file
 */
#include "libprd.h"

static TREE *log_cpio_file (NBLK *n, struct stat *stbuf)
{
	char fullpath[MAX_PATHLEN];
	char *name;
	TREE *t;
	DBLK *d;
	FBLK *f;
	char *p;
	int i;
	int m;
	int newdir;
	char dir_name[MAX_PATHLEN];

	p = cpio_filename();
	if (fn_is_path_absolute(p))
	{
		strcpy(fullpath, p);
	}
	else
	{
		fn_rootname(p, fullpath);
		fn_append_dirname_to_dir(fullpath, ARCH_REL_DIR_NAME);
		fn_append_filename_to_dir(fullpath, p);
	}
	fn_cleanup_path(fullpath);
	name = fullpath;

	if (S_ISDIR(stbuf->st_mode))
	{
		t = add_phony_dblk_to_node(name, n, &newdir);
		d = (DBLK *)tid(t);
		make_xstat(stbuf, &d->stbuf);
	}
	else
	{
		t = add_phony_dblk_to_node(fn_dirname(name, dir_name), n, &newdir);
		if (!t)
			return (t);
		d = (DBLK *)tid(t);

		f = make_phony_fblk(fn_basename(name));
		make_xstat(stbuf, &f->stbuf);
		f->archive_loc = ai->cpio_buf_addr + ai->cpio_buf_pos -
			ai->cpio_hdr_len;
		f->dir = d;

		n->node_total_count++;
		if (!check_for_dup_inode(d->flist, f, 0, 0))
		{
			d->dir_size += f->stbuf.st_size;
			if (!check_for_dup_inode(n->showall_flist, f, 0, 0))
				n->node_total_bytes += f->stbuf.st_size;
		}

		d->flist = put_file_in_list(f, d->flist);
		n->showall_flist = put_file_in_list(f, n->showall_flist);

		for (i=0; i<gbl(scr_cur_count); i++)
		{
			m = TRUE;
			if (gbl(scr_stat)[i].file_spec_pres)
			{
				if (match(FULLNAME(f), gbl(scr_stat)[i].file_spec))
				{
					d->mlist[i] = put_file_in_list(f, d->mlist[i]);
					n->showall_mlist[i] =
						put_file_in_list(f, n->showall_mlist[i]);
				}
				else
					m = FALSE;
			}
			else
			{
				d->mlist[i] = d->flist;
				n->showall_mlist[i] = n->showall_flist;
			}
			if (m)
			{
				n->node_match_count++;
				if (!check_for_dup_inode(d->mlist[i], f, 1, 0))
					n->node_match_bytes += f->stbuf.st_size;
			}
		}
	}
	return (newdir? t : 0);
}

static int read_cpio_node (NBLK *n)
{
	int c;
	struct stat stbuf;
	ABLK *a;
	char *p;
	int save_arch_blkfactor;
	TREE *t;

	a = (ABLK *)n->node_sub_blk;
	disp_file_box();
	disp_drive_info(0, 0);
	disp_node_stats(n);
	if (! opt(wide_screen) && !gbl(scr_is_split))
	{
		werase(gbl(win_current));
		wrefresh(gbl(win_current));
	}

	save_arch_blkfactor = ai->a_blkfactor;
	if (ai->a_dev_type != A_DEV_NOSEEK)
		ai->a_blkfactor = 1;

	ai->a_buffer = (char *)MALLOC(ai->a_blkfactor*ai->a_blksize);
	if (!ai->a_buffer)
	{
		errmsg(ER_IM, "", ERR_ANY);
		return (-1);
	}

	ai->a_open_flags = O_RDONLY;
	if (cpio_open())
	{
		FREE(ai->a_buffer);
		return (-1);
	}

	ai->cpio_buffer = (char *)MALLOC(ai->cpio_bufsize);
	if (!ai->cpio_buffer)
	{
		cpio_close(TRUE);
		FREE(ai->a_buffer);
		errmsg(ER_IM, "", ERR_ANY);
		return (-1);
	}

	while (TRUE)
	{
		if (xchk_kbd() == KEY_ESCAPE)
			break;
		c = cpio_rd_hdr();
		if (c == -1)
			break;
		p = cpio_filename();
		if (strcmp(p, CPIO_TRAILER) == 0)
			break;
		cpio_get_stbuf(&stbuf);
		t = log_cpio_file(n, &stbuf);
		if (t && opt(scroll_during_logging))
			disp_arch_dirs(t);
		cpio_skip_file();
		a->arch_size = (ai->a_volsize * (ai->a_volno-1)) +
			(ai->cpio_buf_addr + ai->cpio_buf_pos) / ai->a_blksize;

		if (!gbl(scr_is_split) && ! opt(wide_screen))
		{
			if (n->node_total_count % opt(arch_log_interval) == 0)
			{
				char dbuf[12];

				disp_drive_info(0, 0);
				wmove(gbl(win_stats), 2, 8);
				wstandout(gbl(win_stats));
				waddstr(gbl(win_stats), xform(dbuf, n->node_total_count));
				wstandend(gbl(win_stats));
				wrefresh(gbl(win_stats));
			}
		}
	}
	cpio_close(TRUE);
	a->arch_numvols = ai->a_volno;
	FREE(ai->a_buffer);
	FREE(ai->cpio_buffer);
	ai->a_blkfactor = save_arch_blkfactor;

	disp_drive_info(0, 0);
	disp_node_stats(n);
	return (0);
}

static int init_cpio_node (void)
{
	NBLK *n;
	ABLK *a;
	TREE *t;
	DBLK *d;
	BLIST *b;
	int rc;
	BLIST *save_cur_node;
	NBLK *save_cur_root;

	n = nblk_make();
	if (n == 0)
	{
		return (-1);
	}

	fn_rootname(NULL, n->root_name);
	n->node_type = N_ARCH;

	a = ablk_make();
	if (a == 0)
	{
		nblk_free(n);
		return (-1);
	}

	n->node_sub_blk = (void *)a;
	strcpy(a->arch_name, ai->a_name);
	strcpy(a->arch_devname, ai->a_device);
	*a->arch_tmpname = 0;
	strcpy(a->arch_typename, "cpio");
	a->arch_type      = A_CPIO;
	a->arch_dev_type  = ai->a_dev_type;
	a->arch_blkfactor = ai->a_blkfactor;
	a->arch_volsize   = ai->a_volsize;
	a->arch_size      = 0;

	d = make_phony_dblk(n->root_name);
	if (d == 0)
	{
		ablk_free(a);
		nblk_free(n);
		return (-1);
	}

	t = TNEW(d);
	if (t == 0)
	{
		errmsg(ER_IM, "", ERR_ANY);
		dblk_free(d);
		ablk_free(a);
		nblk_free(n);
		return (-1);
	}
	d->dir_tree = t;

	n->root = t;

	b = BNEW(n);
	if (b == 0)
	{
		TFREE(t, FALSE);
		dblk_free(d);
		ablk_free(a);
		nblk_free(n);
		errmsg(ER_IM, "", ERR_ANY);
		return (-1);
	}

	gbl(nodes_list) = bappend(gbl(nodes_list), b);
	save_cur_node = gbl(scr_cur)->cur_node;
	save_cur_root = gbl(scr_cur)->cur_root;
	gbl(scr_cur)->cur_node = b;
	gbl(scr_cur)->cur_root = n;

	rc = read_cpio_node(n);
	if (rc != 0 || n->node_total_count == 0)
	{
		TFREE(t, FALSE);
		dblk_free(d);
		ablk_free(a);
		nblk_free(n);
		gbl(nodes_list) = BDELETE(gbl(nodes_list), b, FALSE);
		gbl(scr_cur)->cur_node = save_cur_node;
		gbl(scr_cur)->cur_root = save_cur_root;
		rc = -1;
	}
	else
	{
		fix_dir_list(n);
	}
	return (rc);
}

int log_cpio (void)
{
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_logcpio_lcf));
	waddstr(gbl(win_commands), msgs(m_logcpio_frodev));
	waddstr(gbl(win_commands), ai->a_device);
	wrefresh(gbl(win_commands));

	werase(gbl(win_message));
	esc_msg();
	wrefresh(gbl(win_message));

	werase(gbl(scr_cur)->cur_path_line);
	wrefresh(gbl(scr_cur)->cur_path_line);
	gbl(scr_cur)->cur_file_win = gbl(scr_cur)->small_file_win;
	gbl(scr_cur)->command_mode = m_dir;
	if (! opt(keep_file_spec))
	{
		rm_file_spec();
		strcpy(gbl(scr_cur)->file_spec, fn_all());
		disp_file_box();
	}
	file_scroll_bar_remove();
	dir_scroll_bar_remove();
	werase(gbl(scr_cur)->cur_dir_win);
	if (! opt(scroll_during_logging) && opt(display_headline))
		headline();
	wrefresh(gbl(scr_cur)->cur_dir_win);
	small_border(ON);
	disp_credits();

	return (init_cpio_node());
}
