/*------------------------------------------------------------------------
 * extract "cpio" files
 */
#include "libprd.h"

static char *cpio_make_link_name (char *linkpath, struct stat *stbuf)
{
	char tmp_name[MAX_PATHLEN];
	FBLK *f;
	BLIST *l;
	char *p;
	int i;

	f = 0;
	for (l=ai->a_links; l; l=bnext(l))
	{
		f = (FBLK *)bid(l);
		if (f->stbuf.st_dev == stbuf->st_dev &&
			f->stbuf.st_ino == stbuf->st_ino)
			break;
	}
	if (!l)
		return (0);

	if (is_arch_path_rel(f))
	{
		strcpy(linkpath, ai->a_ext_rel_path);
		fblk_to_dirname(f, tmp_name);
		for (i=1; ; i++)
		{
			char dir_part[MAX_FILELEN];

			p = fn_get_nth_dir(tmp_name, i, dir_part);
			if (!p)
				break;
			fn_append_dirname_to_dir(linkpath, p);
		}
		fn_append_filename_to_dir(linkpath, FULLNAME(f));
	}
	else
	{
		fblk_to_pathname(f, linkpath);
	}
	return (linkpath);
}

static int cpio_hard_link (const char *path, struct stat *stbuf)
{
	char linkname[MAX_PATHLEN];
	char *ln;
	int c;

	ln = cpio_make_link_name(linkname, stbuf);
	if (!ln)
		return (-1);
	if (os_link(ln, path))
	{
		c = errsys(ER_CLF);
		return (c);
	}
	check_the_file(path);
	check_the_file(ln);
	return (0);
}

static int cpio_soft_link (const char *path, struct stat *stbuf)
{
	int c;

#if X_IFLNK
	c = cpio_read_rec(ai->cpio_buffer, stbuf->st_size);
	if (c == -1)
		return (-1);
	ai->cpio_buffer[stbuf->st_size] = 0;
	if (os_symlink(ai->cpio_buffer, path))
	{
		c = errsys(ER_CMSL);
		return (c);
	}
	check_the_file(path);
	return (0);
#else
	if (opt(arch_ignore_bad_types))
		return (1);
	c = errmsg(ER_NOSYM, "", ERR_ANY);
	return (c);
#endif
}

static int extract_cpio_file (FBLK *f)
{
	NBLK *n;
	ABLK *a;
	char new_name[MAX_PATHLEN];
	char tmp_name[MAX_PATHLEN];
	char *p;
	int rc;
	int i;

	/* make sure we have a location */

	if (f->archive_vol == -1 || f->archive_loc == -1)
		return (1);

	n = get_root_of_file(f);
	a = (ABLK *)n->node_sub_blk;

	/* put together name to extract to */

	if (ai->a_name_only)
	{
		strcpy(new_name, ai->a_ext_rel_path);
		fn_append_filename_to_dir(new_name, FULLNAME(f));
	}
	else
	{
		if (is_arch_path_rel(f))
		{
			strcpy(new_name, ai->a_ext_rel_path);
			fblk_to_dirname(f, tmp_name);
			for (i=1; ; i++)
			{
				char dir_part[MAX_FILELEN];

				p = fn_get_nth_dir(tmp_name, i, dir_part);
				if (!p)
					break;
				fn_append_dirname_to_dir(new_name, p);
			}
			fn_append_filename_to_dir(new_name, FULLNAME(f));
		}
		else
		{
			fblk_to_pathname(f, new_name);
		}
	}
	fn_cleanup_path(new_name);

	/* tell the people what we're doing */

	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_exttar_extfil2));
	wstandout(gbl(win_commands));
	waddstr(gbl(win_commands), FULLNAME(f));
	wstandend(gbl(win_commands));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_exttar_to));
	wstandout(gbl(win_commands));
	waddstr(gbl(win_commands), new_name);
	wstandend(gbl(win_commands));

	wrefresh(gbl(win_commands));

	werase(gbl(win_message));
	esc_msg();
	wrefresh(gbl(win_message));

	/* position tar_file to file to extract */

	if (cpio_position(a->arch_name, f->archive_vol, f->archive_loc))
		return (1);

	/* read the file */

	rc = cpio_extract_file(new_name);

	return (rc);
}

static int ext_cpio_file (void *data)
{
	int i;

	i = extract_cpio_file(gbl(scr_cur)->cur_file);
	if (i < 0)
		return (i);
	return (1);
}

void do_cpio_extract (void)
{
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_exttar_extfil));
	waddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_file));
	if (ext_ask(is_arch_path_rel(gbl(scr_cur)->cur_file)))
		return;
	if (set_arch_buf())
		return;
	ai->a_open_flags = O_RDONLY;
	ai->cpio_buffer = (char *)MALLOC(ai->cpio_bufsize);
	if (!ai->cpio_buffer)
	{
		FREE(ai->a_buffer);
		errmsg(ER_IM, "", ERR_ANY);
		return;
	}
	extract_cpio_file(gbl(scr_cur)->cur_file);
	cpio_close(TRUE);
	FREE(ai->cpio_buffer);
	FREE(ai->a_buffer);
}

void do_tag_cpio_extract (void)
{
	BLIST *taglist;
	BLIST *first_file;

	if (check_tag_count())
		return;

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_exttar_extall));
	if (ext_ask(TRUE))
		return;
	if (set_arch_buf())
		return;

	/* create list of tagged files */

	taglist = make_ext_list();

	ai->a_open_flags = O_RDONLY;
	ai->cpio_buffer = (char *)MALLOC(ai->cpio_bufsize);
	if (!ai->cpio_buffer)
	{
		FREE(ai->a_buffer);
		errmsg(ER_IM, "", ERR_ANY);
		return;
	}

	first_file = disp_ext_list(taglist);
	traverse(ext_cpio_file, 0);
	restore_saved_first_file(first_file);

	cpio_close(TRUE);
	FREE(ai->a_buffer);
	FREE(ai->cpio_buffer);
	BSCRAP(taglist, FALSE);
	ai->a_links = BSCRAP(ai->a_links, FALSE);
}

int cpio_extract_file (const char *path)
{
	int out_fd;
	int i;
	int c;
	int rc;
	off_t s;
	char *dir;
	NBLK *n;
	TREE *t;
	struct stat old_stbuf;
	struct stat new_stbuf;
	char *p;
	char *q;
	char *fn;
	int keep;
	int num_to_read;
	int num_read_in;
	char dir_name[MAX_PATHLEN];

	if (cpio_rd_hdr())
		return (-1);
	cpio_get_stbuf(&old_stbuf);

	if (os_stat(path, &new_stbuf) == 0)
	{
		i = new_stbuf.st_mode & S_IFMT;

		if (i != (old_stbuf.st_mode & S_IFMT))
		{
			c = errmsg(ER_FTDIFF, "", ERR_ANY);
			return (c);
		}

		if (i == S_IFREG)
		{
			if (!ai->a_do_auto)
			{
				fn = cpio_filename();
				p = msgs(m_exttar_extfil);
				q = msgs(m_exttar_to);
				werase(gbl(win_commands));
				wmove(gbl(win_commands), 0, 0);
				xaddstr(gbl(win_commands), p);
				show_file_info(fn_basename(fn),
					old_stbuf.st_size,
					old_stbuf.st_mtime,
					old_stbuf.st_mode,
					old_stbuf.st_uid,
					old_stbuf.st_gid);
				wmove(gbl(win_commands), 1, display_len(p)-display_len(q));
				xaddstr(gbl(win_commands), q);
				show_file_info(fn_basename(path),
					new_stbuf.st_size,
					new_stbuf.st_mtime,
					new_stbuf.st_mode,
					new_stbuf.st_uid,
					new_stbuf.st_gid);
				c = errmsg(ER_FER, "", ERR_YESNO);
				if (c < 0)
					return (-1);
				if (c)
					return (1);
			}
		}
		else if (i == S_IFDIR)
		{
			return (0);
		}
		else
		{
			c = errmsg(ER_EXISPC, "", ERR_ANY);
			return (c);
		}
	}

	dir = fn_dirname(path, dir_name);
	if (make_dir_path(dir))
	{
		c = errsys(ER_COOF);
		return (c);
	}
	if (chk_dir_in_tree(dir) > 0)
	{
		t = path_to_dt(dir, N_FS);
		if (t)
		{
			n = get_root_of_dir(t);
			fix_dir_list(n);
		}
	}

	i = old_stbuf.st_mode & S_IFMT;
	if (i != S_IFREG)
	{
		if (i == S_IFLNK)
		{
			return (cpio_soft_link(path, &old_stbuf));
		}
		else
		if (i == S_IFCHR || i == S_IFBLK)
		{
			c = os_mknod(path, old_stbuf.st_mode, get_rdev(&old_stbuf));
			if (c == -1)
			{
				if (opt(arch_ignore_bad_types))
					return (1);
				c = errsys(ER_CMSPC);
				return (c);
			}
			check_the_file(path);
			return (0);
		}

		else if (i == S_IFDIR)
		{
			if (os_dir_make(path) == -1)
				return (-1);
			else
				return (0);
		}

		return (-1);
	}

	if (old_stbuf.st_nlink > 1)
	{
		c = cpio_hard_link(path, &old_stbuf);
		if (c == 0)
			return (0);
	}

	/* CHECK FOR ENOUGH DISK SPACE ????? */

	out_fd = os_open(path, O_WRONLY|O_CREAT|O_TRUNC,
		old_stbuf.st_mode & 0777);
	if (out_fd == -1)
	{
		c = errsys(ER_COOF);
		return (c);
	}

	s = old_stbuf.st_size;
	rc = 0;
	keep = TRUE;
	while (s)
	{
		if (xchk_kbd() == KEY_ESCAPE)
		{
			keep = FALSE;
			errmsg(ER_FNE, "", ERR_ANY);
			break;
		}
		num_to_read = ai->cpio_bufsize;
		if (num_to_read > s)
			num_to_read = s;
		num_read_in = cpio_read_rec(ai->cpio_buffer, num_to_read);
		if (num_read_in < 0)
		{
			rc = -1;
			break;
		}
		if (num_read_in == 0)
		{
			errmsg(ER_UEOF, "", ERR_ANY);
			rc = -1;
			break;
		}
		s -= num_read_in;
		if (os_write(out_fd, ai->cpio_buffer, num_read_in) == -1)
		{
			rc = errsys(ER_IOERR);
			break;
		}
	}
	os_close(out_fd);
	if (keep)
	{
		os_set_file_time(path, old_stbuf.st_mtime, old_stbuf.st_mtime);

		/* set owner & group if root */

		if (! gbl(pgm_euid))
		{
			os_chown(path, old_stbuf.st_uid, old_stbuf.st_gid);
		}

		check_the_file(path);
	}
	else
	{
		os_file_delete(path);
		rc = -1;
	}
	return (rc);
}

int cpio_extract_temp (FBLK *f, const char *name)
{
	NBLK *n;
	ABLK *a;

	n = get_root_of_file(f);
	a = (ABLK *)n->node_sub_blk;
	if (set_arch_buf())
		return (-1);
	ai->a_open_flags = O_RDONLY;
	ai->cpio_buffer = (char *)MALLOC(ai->cpio_bufsize);
	if (!ai->cpio_buffer)
	{
		FREE(ai->a_buffer);
		errmsg(ER_IM, "", ERR_ANY);
		return (-1);
	}
	if (cpio_position(a->arch_name, f->archive_vol, f->archive_loc))
		return (-1);
	if (cpio_extract_file(name))
		return (-1);
	cpio_close(TRUE);
	FREE(ai->a_buffer);
	FREE(ai->cpio_buffer);
	return (0);
}
