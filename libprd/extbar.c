/*------------------------------------------------------------------------
 * extract "bar" files
 */
#include "libprd.h"

static char *bar_make_link_name (char *link_name)
{
	char *p;

	p = bar_linkname();
	if (ai->a_name_only)
	{
		strcpy(link_name, ai->a_ext_rel_path);
		fn_append_filename_to_dir(link_name, fn_basename(p));
	}
	else
	{
		if (fn_is_path_absolute(p))
		{
			strcpy(link_name, p);
		}
		else
		{
			strcpy(link_name, ai->a_ext_rel_path);
			fn_append_filename_to_dir(link_name, p);
		}
	}
	fn_cleanup_path(link_name);
	return (link_name);
}

static int bar_hard_link (const char *path)
{
	char ln[MAX_PATHLEN];
	int c;

	bar_make_link_name(ln);
	if (os_link(ln, path))
	{
		c = errsys(ER_CLF);
		return (c);
	}
	check_the_file(path);
	check_the_file(ln);
	return (0);
}

static int bar_soft_link (const char *path)
{
	int c;
#if X_IFLNK
	char ln[MAX_PATHLEN];

	bar_make_link_name(ln);
	if (os_symlink(ln, path))
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

static int extract_bar_file (FBLK *f)
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

	if (bar_position(a->arch_name, f->archive_vol, f->archive_loc))
		return (1);

	/* read the file */

	rc = bar_extract_file(new_name);

	return (rc);
}

static int ext_bar_file (void *data)
{
	int i;

	i = extract_bar_file(gbl(scr_cur)->cur_file);
	if (i < 0)
		return (i);
	return (1);
}

void do_bar_extract (void)
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
	extract_bar_file(gbl(scr_cur)->cur_file);
	bar_close(TRUE);
	FREE(ai->a_buffer);
}

void do_tag_bar_extract (void)
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

	first_file = disp_ext_list(taglist);
	traverse(ext_bar_file, 0);
	restore_saved_first_file(first_file);

	bar_close(TRUE);
	FREE(ai->a_buffer);
	BSCRAP(taglist, FALSE);
}

int bar_extract_file (const char *path)
{
	int num_recs;
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
	char zname[MAX_PATHLEN];
	char dir_name[MAX_PATHLEN];

	bar_get_stbuf(&old_stbuf);
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
				fn = bar_filename();
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
			return (bar_soft_link(path));
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

		return (1);
	}

	if (old_stbuf.st_nlink > 1)
		return (bar_hard_link(path));

	/* CHECK FOR ENOUGH DISK SPACE ????? */

	strcpy(zname, path);
	if (ai->a_compressed)
	{
		x_make_compressed_name(zname, comp_lzw);
	}
	out_fd = os_open(zname, O_WRONLY|O_CREAT|O_TRUNC,
		old_stbuf.st_mode & 0777);
	if (out_fd == -1)
	{
		c = errsys(ER_COOF);
		return (c);
	}

	s = old_stbuf.st_size;
	num_recs = (s + ai->a_blksize - 1) / ai->a_blksize;
	rc = 0;
	keep = TRUE;
	for (i=0; i<num_recs; i++)
	{
		if (xchk_kbd() == KEY_ESCAPE)
		{
			keep = FALSE;
			errmsg(ER_FNE, "", ERR_ANY);
			rc = -1;
			break;
		}
		c = bar_read_rec();
		if (c < 0)
		{
			rc = -1;
			break;
		}
		if (c == 0)
		{
			errmsg(ER_UEOF, "", ERR_ANY);
			rc = -1;
			break;
		}
		c = ai->a_blksize;
		if (s < c)
			c = s;
		s -= c;
		if (os_write(out_fd, ai->a_recp, c) == -1)
		{
			rc = errsys(ER_IOERR);
			break;
		}
	}
	os_close(out_fd);

	if (keep)
	{
		if (ai->a_compressed)
		{
#if 0
			werase(gbl(win_commands));
			wmove(gbl(win_commands), 0, 0);
			xaddstr(gbl(win_commands), msgs(m_exttar_uncfile));
			waddstr(gbl(win_commands), fn_basename(path));
			wrefresh(gbl(win_commands));
#endif
			x_decomp(zname, path, comp_lzw);
		}
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
		if (ai->a_compressed)
			os_file_delete(zname);
		else
			os_file_delete(path);
		rc = -1;
	}
	return (rc);
}

int bar_extract_temp (FBLK *f, const char *name)
{
	NBLK *n;
	ABLK *a;

	n = get_root_of_file(f);
	a = (ABLK *)n->node_sub_blk;
	if (set_arch_buf())
		return (-1);
	ai->a_open_flags = O_RDONLY;
	if (bar_position(a->arch_name, f->archive_vol, f->archive_loc))
		return (-1);
	if (bar_extract_file(name))
		return (-1);
	bar_close(TRUE);
	FREE(ai->a_buffer);
	return (0);
}
