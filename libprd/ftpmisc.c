/*------------------------------------------------------------------------
 * misc ftp utilities
 */
#include "libprd.h"

struct ftpmisc_info
{
	char		temppath[MAX_PATHLEN];
	BLIST *		links;

	char		foo_path[MAX_PATHLEN];
	struct stat foo_stbuf;

	int			and_perms;
	int			or_perms;
};
typedef struct ftpmisc_info FTPMISC_INFO;

static int ftp_do_symname (FTP_NODE *fn, FTP_STAT *ftp_stat, void *data)
{
	FTPMISC_INFO *fi = (FTPMISC_INFO *)data;

	memcpy(&fi->foo_stbuf, &ftp_stat->stbuf, sizeof(fi->foo_stbuf));
	strcpy(fi->foo_path, ftp_stat->sym_name);

	return (0);
}

static int ftp_get_symname (FTPMISC_INFO *fi, FTP_NODE *fn,
	const char *path, char *sympath)
{
	char dir_name[MAX_PATHLEN];
	int count;

	*fi->foo_path = 0;
	for (count=0; count<16; count++)
	{
		if (ftp_list_file(fn, fn_dirname(path, dir_name), fn_basename(path),
				ftp_do_symname, fi))
			return (-1);

		if (! S_ISLNK(fi->foo_stbuf.st_mode))
		{
			return (0);
		}

		strcpy(sympath, fi->foo_path);
		path = fi->foo_path;
	}

	return (-1);
}

static void ftp_add_file_to_dir (const char *file_name, TREE *dir,
	struct stat *stbuf)
{
	NBLK *n;
	NBLK *r;
	DBLK *d;
	FBLK *f;
	BLIST *b;
	BLIST *l;
	int m[NUM_SPLIT_WINS];
	int i;

	d = (DBLK *)tid(dir);				/* get pointer to dir block */

	n = get_root_of_dir(dir);
	r = gbl(scr_cur)->cur_root;

	/*
	 *  Update all statistics
	 */

	for (i=0; i<gbl(scr_cur_count); i++)
	{
		if (gbl(scr_stat)[i].file_spec_pres)
			m[i] = match(file_name, gbl(scr_stat)[i].file_spec);
		else
			m[i] = 1;
	}

	f = 0;								/* shut up cc */
	for (b=d->flist; b; b=bnext(b))		/* check if file already in it */
	{
		f = (FBLK *)bid(b);
		if (strcmp(FULLNAME(f), file_name) == 0)
			break;
	}

	if (b)
	{
		/* file already in directory */

		if (!check_for_dup_inode(d->flist, f, 0, 0))
			d->dir_size -= f->stbuf.st_size;	/* subtract old size */
		n->node_total_count--;
		if (!check_for_dup_inode(n->showall_flist, f, 0, 0))
			n->node_total_bytes -= f->stbuf.st_size;
		if (m[gbl(scr_cur_no)])
		{
			n->node_match_count--;
			if (! check_for_dup_inode(n->showall_mlist[gbl(scr_cur_no)],
				f, 0, 0))
			{
				n->node_match_bytes -= f->stbuf.st_size;
			}
		}

		if (is_file_tagged(f))
		{
			n->node_tagged_count--;
			if (!check_for_dup_inode(n->showall_flist, f, 0, 1))
			{
				n->node_tagged_bytes -= f->stbuf.st_size;
				n->node_tagged_blocks -=
					file_size_in_blks(f, f->stbuf.st_size);
			}
		}

		if (r->node_type == n->node_type)
		{
			switch (gbl(scr_cur)->command_mode)
			{
			case m_file:
				if (gbl(scr_cur)->cur_dir != d)
					break;
				/*FALLTHROUGH*/
			case m_showall:
			case m_level_showall:
			case m_tag_showall:
			case m_global:
			case m_tag_global:
				gbl(scr_cur)->numfiles--;
				gbl(scr_cur)->dir_total_count--;
				if (!check_for_dup_inode(gbl(scr_cur)->base_file, f, 0, 0))
					gbl(scr_cur)->dir_total_bytes -= f->stbuf.st_size;
				if (m[gbl(scr_cur_no)])
				{
					gbl(scr_cur)->dir_match_count--;
					if (!
						check_for_dup_inode(gbl(scr_cur)->first_file, f, 0, 0))
					{
						gbl(scr_cur)->dir_match_bytes -= f->stbuf.st_size;
					}
				}
				if (is_file_tagged(f))
				{
					gbl(scr_cur)->dir_tagged_count--;
					if (!check_for_dup_inode(gbl(scr_cur)->base_file, f, 0, 1))
					{
						gbl(scr_cur)->dir_tagged_bytes -= f->stbuf.st_size;
						gbl(scr_cur)->dir_tagged_blocks -= file_size_in_blks(f,
							f->stbuf.st_size);
					}
				}
			}
		}

		make_xstat(stbuf, &f->stbuf);
	}
	else
	{
		/* file not in directory */

		f = fs_make_fblk(file_name, stbuf);
		if (!f)
			return;
		f->dir            = d;
		d->flist          = put_file_in_list(f, d->flist);
		n->showall_flist  = put_file_in_list(f, n->showall_flist);
		if (stbuf->st_nlink > 1)
		{
			l = BNEW(f);
			if (l)
			{
				n->link_list = bappend(n->link_list, l);
			}
		}

		for (i=0; i<gbl(scr_cur_count); i++)
		{
			if (gbl(scr_stat)[i].file_spec_pres)
			{
				if (m[i])
				{
					d->mlist[i]          = put_file_in_list(f, d->mlist[i]);
					n->showall_mlist[i]  = put_file_in_list(f,
						n->showall_mlist[i]);
				}
			}
			else
			{
				d->mlist[i]         = d->flist;
				n->showall_mlist[i] = n->showall_flist;
			}

			if (gbl(scr_stat)[i].command_mode == m_global ||
		    	(gbl(scr_stat)[i].command_mode == m_level_showall &&
			    !(d->hidden & gbl(scr_stat)[i].tag_mask)))
			{
				gbl(scr_stat)[i].global_flist =
					put_file_in_list(f, gbl(scr_stat)[i].global_flist);
				if (gbl(scr_stat)[i].file_spec_pres)
				{
					if (m[i])
					{
						gbl(scr_stat)[i].global_mlist =
							put_file_in_list(f, gbl(scr_stat)[i].global_mlist);
					}
				}
				else
				{
					gbl(scr_stat)[i].global_mlist =
						gbl(scr_stat)[i].global_flist;
				}
			}
		}
	}

	if (!check_for_dup_inode(d->flist, f, 0, 0))
		d->dir_size += f->stbuf.st_size;
	n->node_total_count++;
	if (!check_for_dup_inode(n->showall_flist, f, 0, 0))
		n->node_total_bytes += f->stbuf.st_size;
	if (m[gbl(scr_cur_no)])
	{
		n->node_match_count++;
		if (!check_for_dup_inode(n->showall_mlist[gbl(scr_cur_no)], f, 0, 0))
			n->node_match_bytes += f->stbuf.st_size;
	}
	if (is_file_tagged(f))
	{
		n->node_tagged_count++;
		if (!check_for_dup_inode(n->showall_flist, f, 0, 1))
		{
			n->node_tagged_bytes += f->stbuf.st_size;
			n->node_tagged_blocks += file_size_in_blks(f, f->stbuf.st_size);
		}
	}

	if (n->node_type == r->node_type)
	{
		switch (gbl(scr_cur)->command_mode)
		{
			case m_dir:
			case m_file:
				gbl(scr_cur)->base_file  = (gbl(scr_cur)->cur_dir)->flist;
				gbl(scr_cur)->first_file =
					(gbl(scr_cur)->cur_dir)->mlist[gbl(scr_cur_no)];
				break;
			case m_showall:
				gbl(scr_cur)->base_file  =
					(gbl(scr_cur)->cur_root)->showall_flist;
				gbl(scr_cur)->first_file =
					(gbl(scr_cur)->cur_root)->showall_mlist[gbl(scr_cur_no)];
				break;
			case m_level_showall:
			case m_global:
			case m_tag_showall:
			case m_tag_global:
				gbl(scr_cur)->base_file  = gbl(scr_cur)->global_flist;
				gbl(scr_cur)->first_file = gbl(scr_cur)->global_mlist;
				break;
		}
		gbl(scr_cur)->numfiles = bcount(gbl(scr_cur)->first_file);
		if (gbl(scr_cur)->numfiles <= 1)
			gbl(scr_cur)->top_file = gbl(scr_cur)->first_file;

		switch (gbl(scr_cur)->command_mode)
		{
		case m_dir:
			check_small_window();
			disp_node_stats(gbl(scr_cur)->cur_root);
			break;
		case m_file:
			if (gbl(scr_cur)->cur_dir != d)
				break;
			/*FALLTHROUGH*/
		case m_showall:
		case m_level_showall:
		case m_tag_showall:
		case m_global:
		case m_tag_global:
			gbl(scr_cur)->dir_total_count++;
			if (!check_for_dup_inode(gbl(scr_cur)->base_file, f, 0, 0))
				gbl(scr_cur)->dir_total_bytes += stbuf->st_size;
			if (m[gbl(scr_cur_no)])
			{
				gbl(scr_cur)->dir_match_count++;
				if (!check_for_dup_inode(gbl(scr_cur)->first_file, f, 0, 0))
					gbl(scr_cur)->dir_match_bytes += stbuf->st_size;
			}
			disp_dir_stats();
			gbl(scr_cur)->cur_file_no =
				bindex(gbl(scr_cur)->first_file, gbl(scr_cur)->file_cursor);
			i = gbl(scr_cur)->cur_file_no -
				(gbl(scr_cur)->cur_file_line + gbl(scr_cur)->cur_file_col *
				(gbl(scr_cur)->max_file_line+1));
			gbl(scr_cur)->top_file = bnth(gbl(scr_cur)->first_file, i);
			disp_file_list();
			hilite_file(ON);
			disp_cur_file();
			break;
		}
	}
}

int ftp_get_syminfo (FTP_NODE *fn, const char *path, char *sympath,
	struct stat *stbuf)
{
	FTPMISC_INFO	ftpmisc_info;
	FTPMISC_INFO *	fi = &ftpmisc_info;

	if (ftp_get_symname(fi, fn, path, sympath))
		return (-1);

	memcpy(stbuf, &fi->foo_stbuf, sizeof(*stbuf));

	return (0);
}

static int ftp_get_temp (FTPMISC_INFO *fi, char *path)
{
	FTP_NODE *fn;
	int rc;
	int type;

	fn = (FTP_NODE *)(gbl(scr_cur)->cur_root)->node_sub_blk;

	strcpy(fi->temppath, fn->tempname);

	type = x_is_file_compressed(path);

	if (type != -1)
		x_make_compressed_name(fi->temppath, type);

	rc = ftp_copy_file_from(fn, fi->temppath, path);

	return (rc);
}

void ftp_do_print (void)
{
	FTPMISC_INFO	ftpmisc_info;
	FTPMISC_INFO *	fi = &ftpmisc_info;
	FBLK *f;
	char input_str[MAX_PATHLEN];
	char path[MAX_PATHLEN];
	int rc;
	int c;

	f = gbl(scr_cur)->cur_file;

	bang(msgs(m_print_entfil));

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_print_fil2));
	wrefresh(gbl(win_commands));

	strcpy(input_str, FULLNAME(f));
	c = xgetstr(gbl(win_commands), input_str, XGL_PRINT_FILE_NAME, MAX_PATHLEN,
		0, XG_FILEPATH);

	if (c <= 0)
	{
		disp_cmds();
		return;
	}

	fn_get_abs_path(gbl(scr_cur)->path_name, input_str, path);

	rc = ftp_get_temp(fi, path);
	if (rc)
	{
		return;
	}

	print_file_with_hdr(FULLNAME(f), fi->temppath, path);
	os_file_delete(fi->temppath);

	disp_cmds();
}

static int print_ftp_file (void *data)
{
	FTPMISC_INFO *	fi = (FTPMISC_INFO *)data;
	char path[MAX_PATHLEN];
	BLIST *l;
	FBLK *f;
	int i;

	f = (gbl(scr_cur)->cur_file);
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_print_fil1));
	zaddstr(gbl(win_commands), FULLNAME(f));
	wrefresh(gbl(win_commands));

	/* check if link to file already processed */
	if (gbl(scr_cur)->cmd_sub_mode == m_tag &&
	   (gbl(scr_cur)->cur_file)->stbuf.st_nlink>1)
	{
		for (l=fi->links; l; l=bnext(l))
		{
			f = (FBLK *)bid(l);
			if (f->stbuf.st_ino == (gbl(scr_cur)->cur_file)->stbuf.st_ino &&
			    f->stbuf.st_dev == (gbl(scr_cur)->cur_file)->stbuf.st_dev)
				break;
		}
		if (!l)
		{
			l = BNEW(gbl(scr_cur)->cur_file);
			if (l)
			{
				fi->links = bappend(fi->links, l);
			}
		}
		else
		{
			char buf[128];
			char sbuf[128];

			strcpy(buf, msgs(m_print_lin));
			strcat(buf, zstring(FULLNAME((FBLK *)bid(l)), sbuf));
			strcat(buf, msgs(m_print_alrpri));
			i = yesno_msg(buf);
			if (i < 0)
				return (-1);
			if (i)
				return (1);
		}
	}

	fblk_to_pathname((gbl(scr_cur)->cur_file), path);
	if (ftp_get_temp(fi, path))
		print_path(fi->temppath, path);

	return (1);
}

void ftp_do_tag_print (void)
{
	FTPMISC_INFO	ftpmisc_info;
	FTPMISC_INFO *	fi = &ftpmisc_info;
	int c;

	if (check_tag_count())
		return;

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_print_atf));
	wrefresh(gbl(win_commands));

	c = yesno_msg(msgs(m_print_pri));
	if (c)
		return;

	if (print_init())
		return;

	fi->links = 0;
	traverse(print_ftp_file, fi);
	BSCRAP(fi->links, FALSE);

	print_close(FALSE);
	disp_cmds();
}

void ftp_do_view (void)
{
	FTPMISC_INFO	ftpmisc_info;
	FTPMISC_INFO *	fi = &ftpmisc_info;
	char path[MAX_PATHLEN];
	int rc;

	fblk_to_pathname(gbl(scr_cur)->cur_file, path);

	rc = ftp_get_temp(fi, path);
	if (rc)
	{
		return;
	}

	strcpy(gbl(vfcb_fv)->v_dispname, path);
	view_file(fi->temppath);
	*gbl(vfcb_fv)->v_dispname = 0;
	os_file_delete(fi->temppath);
}

static int ftp_set_file_attr (void *data)
{
	FTPMISC_INFO *	fi = (FTPMISC_INFO *)data;
	char filename[MAX_FILELEN];
	BLIST *l;
	NBLK *n;
	FTP_NODE *fn;
	FBLK *f;
	int new_perm;
	int i;
	int frow;
	int fcol;

	n = get_root_of_file(gbl(scr_cur)->cur_file);
	fn = (FTP_NODE *)n->node_sub_blk;

	strcpy(filename, gbl(scr_cur)->path_name);
	fn_append_filename_to_dir(filename, FULLNAME(gbl(scr_cur)->cur_file));

	/* check if link to file already processed */

	if (gbl(scr_cur)->cmd_sub_mode == m_tag &&
		(gbl(scr_cur)->cur_file)->stbuf.st_nlink > 1)
	{
		for (l=fi->links; l; l=bnext(l))
		{
			f = (FBLK *)bid(l);
			if (f->stbuf.st_ino == (gbl(scr_cur)->cur_file)->stbuf.st_ino &&
			    f->stbuf.st_dev == (gbl(scr_cur)->cur_file)->stbuf.st_dev)
				break;
		}
		if (l)							/* if so, return as if done */
			return (1);
		l = BNEW(gbl(scr_cur)->cur_file);	/* if not - add to done list */
		if (l)
		{
			fi->links = bappend(fi->links, l);
		}
	}

	new_perm = (gbl(scr_cur)->cur_file)->stbuf.st_mode & S_PERMS;
	new_perm &= fi->and_perms;
	new_perm |= fi->or_perms;

	i = ftp_chmod_file(fn, filename, new_perm);
	if (i)
	{
		i = errsys(ER_CCP);
		return (i);
	}

	if ((gbl(scr_cur)->cur_file)->stbuf.st_nlink > 1)
	{
		for (l=n->link_list; l; l=bnext(l))
		{
			f = (FBLK *)bid(l);
			if (f != (gbl(scr_cur)->cur_file) &&
			    f->stbuf.st_ino == (gbl(scr_cur)->cur_file)->stbuf.st_ino &&
			    f->stbuf.st_dev == (gbl(scr_cur)->cur_file)->stbuf.st_dev)
			{
				f->stbuf.st_mode &= ~S_PERMS;
				f->stbuf.st_mode |= new_perm;
				if (is_file_displayed(f, &frow, &fcol))
					redisplay_file(f, frow, fcol);
			}
		}
	}

	(gbl(scr_cur)->cur_file)->stbuf.st_mode &= ~S_PERMS;
	(gbl(scr_cur)->cur_file)->stbuf.st_mode |= new_perm;

	return (0);
}

int ftp_do_file_attr (void)
{
	FTPMISC_INFO	ftpmisc_info;
	FTPMISC_INFO *	fi = &ftpmisc_info;
	int i;

	fi->or_perms  = (gbl(scr_cur)->cur_file)->stbuf.st_mode & S_PERMS;
	fi->and_perms = S_PERMS;

	gbl(scr_cur_sub_cmd) = CMDS_ATTR_PERMS;
	i = get_perm_masks(FALSE, &fi->and_perms, &fi->or_perms);
	if (i)
	{
		disp_cmds();
		return (0);
	}

	if (ftp_set_file_attr(fi) == 0)
	{
		hilite_file(ON);
	}

	disp_cmds();
	return (1);					/* for traverse() */
}

void ftp_do_tag_file_attr (void)
{
	FTPMISC_INFO	ftpmisc_info;
	FTPMISC_INFO *	fi = &ftpmisc_info;
	int i;

	if (check_tag_count())
		return;

	gbl(scr_cur_sub_cmd) = CMDS_ATTR_PERMS;
	fi->or_perms  = (gbl(scr_cur)->cur_file)->stbuf.st_mode & S_PERMS;
	fi->and_perms = S_PERMS;
	i = get_perm_masks(FALSE, &fi->and_perms, &fi->or_perms);
	if (i < 0)
	{
		disp_cmds();
		return;
	}

	fi->links = 0;
	traverse(ftp_set_file_attr, fi);
	fi->links = BSCRAP(fi->links, FALSE);

	disp_cmds();
}

int ftp_resolve_symlink (void)
{
	FTPMISC_INFO	ftpmisc_info;
	FTPMISC_INFO *	fi = &ftpmisc_info;
	NBLK *n;
	FTP_NODE *fn;
	FBLK *f;
	TREE *t;
	DBLK *d;
	char dir_name[MAX_PATHLEN];
	char sym_path[MAX_PATHLEN];
	SYM_ENTRY *sd;
	BLIST *b;

	f = gbl(scr_cur)->cur_file;
	t = f->dir->dir_tree;

	if (! S_ISLNK(f->stbuf.st_mode))
		return (-1);

	fblk_to_dirname(f, dir_name);

	n = get_root_of_file(f);
	fn = (FTP_NODE *)n->node_sub_blk;

	strcpy(sym_path, dir_name);
	fn_append_filename_to_dir(sym_path, FULLNAME(f));

	if (ftp_get_symname(fi, fn, sym_path, sym_path))
		return (-1);

	if (! S_ISDIR(fi->foo_stbuf.st_mode))
	{
		ftp_add_file_to_dir(FULLNAME(f), t, &fi->foo_stbuf);
		return (0);
	}
	else
	{
		t = ftp_add_dir(FULLNAME(f), &fi->foo_stbuf, t);
		if (!t)
		{
			return (-1);
		}
		else
		{
			d = (DBLK *)tid(t);
			d->flags = D_NOT_LOGGED;

			sd = (SYM_ENTRY *)MALLOC(sizeof(SYM_ENTRY));
			if (sd != (SYM_ENTRY *)NULL)
			{
				b = BNEW(sd);
				if (b != 0)
				{
					sd->sym_file = (void *)d;
					strcpy(sd->sym_name, sym_path);
					fn->symdirs = bappend(fn->symdirs, b);
				}
				else
					FREE(sd);
			}

			remove_file(f, FALSE);
			gbl(scr_cur)->cur_dir_tree = t;
			gbl(scr_cur)->cur_dir = d;
			set_top_dir();
			gbl(scr_cur)->in_small_window = FALSE;
			gbl(scr_cur)->cmd_sub_mode = m_reg;
			return (1);
		}
	}
}

TREE *ftp_add_dir (const char *filename, struct stat *stbuf, TREE *p)
{
	NBLK *n;
	DBLK *d;
	TREE *t;

	/* check if dir already in tree */

	for (t=tleft(p); t; t=tright(t))
	{
		d = (DBLK *)tid(t);
		if (strcmp(filename, FULLNAME(d)) == 0)
			return (t);
	}

	/* make new DBLK for new directory */

	d = make_ftp_dblk(filename, stbuf);
	if (!d)
	{
		errmsg(ER_IM, "", ERR_ANY);
		return (0);
	}
	d->flags = 0;		/* delete NOT_LOGGED flag */

	/* make tree block for new directory */

	t = TNEW(d);
	if (!t)
	{
		dblk_free(d);
		errmsg(ER_IM, "", ERR_ANY);
		return (0);
	}
	d->dir_tree = t;

	/* hook new tree entry into tree structure */

	add_dir_to_parent(t, p);

	/* update dir lists in node block */

	n = get_root_of_dir(p);
	fix_dir_list(n);

	return (t);
}
