/*------------------------------------------------------------------------
 * process the "restore" cmd
 */
#include "libprd.h"

static int read_tar_file (int do_confirm, int *new_dirs)
{
	char new_file[MAX_PATHLEN];
	int i;
	int c;
	char *p;
	char *dp;
	char *fn;
	NBLK *n;
	TREE *t;
	int rc;
	char dir_name[MAX_PATHLEN];

	if (!tar_is_valid_hdr())
		return (-1);

	p = fn = tar_filename();
	if (!match(fn, ai->a_pattern))
	{
		tar_skip_file();
		return (0);
	}
	if (ai->a_name_only)
		fn = fn_basename(fn);
	fn_get_abs_path(ai->a_rel_name, fn, new_file);

	if (do_confirm)
	{
		werase(gbl(win_commands));
		wmove(gbl(win_commands), 0, 0);
		xaddstr(gbl(win_commands), msgs(m_restore_res2));
		zaddstr(gbl(win_commands), p);
		wrefresh(gbl(win_commands));
		c = yesno_msg(msgs(m_restore_resfil));
		if (c < 0)
			return (-1);
		if (c)
		{
			tar_skip_file();
			return (0);
		}
	}

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_restore_resing));
	zaddstr(gbl(win_commands), fn);
	wrefresh(gbl(win_commands));

	werase(gbl(win_message));
	esc_msg();
	wrefresh(gbl(win_message));

	dp = fn_dirname(new_file, dir_name);
	i = is_it_a_dir(dp, 0);
	if (i == 1)
	{
		c = errsys(ER_COOF);
		if (c < 0)
			return (-1);
		tar_skip_file();
		return (0);
	}
	else if (i == -1)
	{
		if (make_dir_path(dp))
		{
			c = errsys(ER_COOF);
			if (c < 0)
				return (-1);
			tar_skip_file();
			return (0);
		}
	}
	i = chk_dir_in_tree(dp);
	if (i == 1)
	{
		t = pathname_to_dirtree(dp);
		if (t)
		{
			n = get_root_of_dir(t);
			fix_dir_list(n);
			*new_dirs = TRUE;
		}
	}

	rc = tar_extract_file(new_file);
	return (rc);
}

static void restore_tar_files (int do_confirm, int *new_dirs)
{
	int c;
	int zero_cnt;

	ai->a_buffer = (char *)MALLOC(ai->a_blkfactor*ai->a_blksize);
	if (!ai->a_buffer)
	{
		errmsg(ER_IM, "", ERR_ANY);
		return;
	}
	strcpy(ai->a_name, msgs(m_restore_tf));
	ai->a_open_flags = O_RDONLY;
	if (tar_open())
	{
		FREE(ai->a_buffer);
#if 0
		errsys(ER_COBF);
#endif
		return;
	}
	zero_cnt = 0;
	while (TRUE)
	{
		c = tar_read_rec();			/* read tar header */
		if (c <= 0)
			break;
		if (!*ai->a_recp)				/* skip empty rec */
		{
			if (zero_cnt)
				break;
			zero_cnt++;
			continue;
		}
		zero_cnt = 0;
		c = read_tar_file(do_confirm, new_dirs);
		if (c)
			break;
	}
	tar_close(TRUE);
	FREE(ai->a_buffer);
}

static int read_bar_file (int do_confirm, int *new_dirs)
{
	char new_file[MAX_PATHLEN];
	int i;
	int c;
	char *p;
	char *dp;
	char *fn;
	NBLK *n;
	TREE *t;
	int rc;
	char dir_name[MAX_PATHLEN];

	if (!bar_is_valid_hdr())
		return (-1);

	p = fn = bar_filename();
	if (!match(fn, ai->a_pattern))
	{
		bar_skip_file();
		return (0);
	}
	if (ai->a_name_only)
		fn = fn_basename(fn);
	fn_get_abs_path(ai->a_rel_name, fn, new_file);

	if (do_confirm)
	{
		werase(gbl(win_commands));
		wmove(gbl(win_commands), 0, 0);
		xaddstr(gbl(win_commands), msgs(m_restore_res2));
		zaddstr(gbl(win_commands), p);
		wrefresh(gbl(win_commands));
		c = yesno_msg(msgs(m_restore_resfil));
		if (c < 0)
			return (-1);
		if (c)
		{
			bar_skip_file();
			return (0);
		}
	}

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_restore_resing));
	zaddstr(gbl(win_commands), fn);
	wrefresh(gbl(win_commands));

	werase(gbl(win_message));
	esc_msg();
	wrefresh(gbl(win_message));

	dp = fn_dirname(new_file, dir_name);
	i = is_it_a_dir(dp, 0);
	if (i == 1)
	{
		c = errsys(ER_COOF);
		if (c < 0)
			return (-1);
		bar_skip_file();
		return (0);
	}
	else if (i == -1)
	{
		if (make_dir_path(dp))
		{
			c = errsys(ER_COOF);
			if (c < 0)
				return (-1);
			bar_skip_file();
			return (0);
		}
	}
	i = chk_dir_in_tree(dp);
	if (i == 1)
	{
		t = pathname_to_dirtree(dp);
		if (t)
		{
			n = get_root_of_dir(t);
			fix_dir_list(n);
			*new_dirs = TRUE;
		}
	}

	rc = bar_extract_file(new_file);
	return (rc);
}

static void restore_bar_files (int do_confirm, int *new_dirs)
{
	int c;
	int zero_cnt;

	ai->a_buffer = (char *)MALLOC(ai->a_blkfactor*ai->a_blksize);
	if (!ai->a_buffer)
	{
		errmsg(ER_IM, "", ERR_ANY);
		return;
	}
	strcpy(ai->a_name, msgs(m_restore_bf));
	ai->a_open_flags = O_RDONLY;
	if (bar_open())
	{
		FREE(ai->a_buffer);
#if 0
		errsys(ER_COBF);
#endif
		return;
	}
	zero_cnt = 0;
	while (TRUE)
	{
		c = bar_read_rec();			/* read bar header */
		if (c <= 0)
			break;
		if (!*ai->a_recp)				/* skip empty rec */
		{
			if (zero_cnt)
				break;
			zero_cnt++;
			continue;
		}
		zero_cnt = 0;
		c = read_bar_file(do_confirm, new_dirs);
		if (c)
			break;
	}
	bar_close(TRUE);
	FREE(ai->a_buffer);
}

static int read_cpio_file (int do_confirm, int *new_dirs)
{
	char new_file[MAX_PATHLEN];
	int i;
	int c;
	char *p;
	char *dp;
	char *fn;
	NBLK *n;
	TREE *t;
	int rc;
	char dir_name[MAX_PATHLEN];

	p = fn = cpio_filename();
	if (!match(fn, ai->a_pattern))
	{
		cpio_skip_file();
		return (0);
	}

	if (ai->a_name_only)
		fn = fn_basename(fn);
	fn_get_abs_path(ai->a_rel_name, fn, new_file);

	if (do_confirm)
	{
		werase(gbl(win_commands));
		wmove(gbl(win_commands), 0, 0);
		xaddstr(gbl(win_commands), msgs(m_restore_res2));
		zaddstr(gbl(win_commands), p);
		wrefresh(gbl(win_commands));
		c = yesno_msg(msgs(m_restore_resfil));
		if (c < 0)
			return (-1);
		if (c)
		{
			cpio_skip_file();
			return (0);
		}
	}

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_restore_resing));
	zaddstr(gbl(win_commands), fn);
	wrefresh(gbl(win_commands));

	werase(gbl(win_message));
	esc_msg();
	wrefresh(gbl(win_message));

	dp = fn_dirname(new_file, dir_name);
	i = is_it_a_dir(dp, 0);
	if (i == 1)
	{
		c = errsys(ER_COOF);
		if (c < 0)
			return (-1);
		cpio_skip_file();
		return (0);
	}
	else if (i == -1)
	{
		if (make_dir_path(dp))
		{
			c = errsys(ER_COOF);
			if (c < 0)
				return (-1);
			cpio_skip_file();
			return (0);
		}
	}
	i = chk_dir_in_tree(dp);
	if (i == 1)
	{
		t = pathname_to_dirtree(dp);
		if (t)
		{
			n = get_root_of_dir(t);
			fix_dir_list(n);
			*new_dirs = TRUE;
		}
	}

	rc = cpio_extract_file(new_file);
	return (rc);
}

static void restore_cpio_files (int do_confirm, int *new_dirs)
{
	int c;
	char *p;

	ai->a_buffer = (char *)MALLOC(ai->a_blkfactor*ai->a_blksize);
	if (!ai->a_buffer)
	{
		errmsg(ER_IM, "", ERR_ANY);
		return;
	}
	strcpy(ai->a_name, msgs(m_restore_cf));
	ai->a_open_flags = O_RDONLY;
	if (cpio_open())
	{
		FREE(ai->a_buffer);
#if 0
		errsys(ER_COBF);
#endif
		return;
	}
	while (TRUE)
	{
		c = cpio_rd_hdr();			/* read cpio header */
		if (c <= 0)
			break;
		p = cpio_filename();
		if (strcmp(p, CPIO_TRAILER) == 0)
			break;
		c = read_cpio_file(do_confirm, new_dirs);
		if (c)
			break;
	}
	cpio_close(TRUE);
	FREE(ai->a_buffer);
}

static int read_ztar_file (int do_confirm, int *new_dirs)
{
	char new_file[MAX_PATHLEN];
	int i;
	int c;
	char *p;
	char *dp;
	char *fn;
	NBLK *n;
	TREE *t;
	int rc;
	char dir_name[MAX_PATHLEN];

	if (!tar_is_valid_hdr())
		return (-1);

	p = fn = tar_filename();
	if (!match(fn, ai->a_pattern))
	{
		tar_skip_file();
		return (0);
	}
	if (ai->a_name_only)
		fn = fn_basename(fn);
	fn_get_abs_path(ai->a_rel_name, fn, new_file);

	if (do_confirm)
	{
		werase(gbl(win_commands));
		wmove(gbl(win_commands), 0, 0);
		xaddstr(gbl(win_commands), msgs(m_restore_res2));
		zaddstr(gbl(win_commands), p);
		wrefresh(gbl(win_commands));
		c = yesno_msg(msgs(m_restore_resfil));
		if (c < 0)
			return (-1);
		if (c)
		{
			tar_skip_file();
			return (0);
		}
	}

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_restore_resing));
	zaddstr(gbl(win_commands), fn);
	wrefresh(gbl(win_commands));

	werase(gbl(win_message));
	esc_msg();
	wrefresh(gbl(win_message));

	dp = fn_dirname(new_file, dir_name);
	i = is_it_a_dir(dp, 0);
	if (i == 1)
	{
		c = errsys(ER_COOF);
		if (c < 0)
			return (-1);
		tar_skip_file();
		return (0);
	}
	else if (i == -1)
	{
		if (make_dir_path(dp))
		{
			c = errsys(ER_COOF);
			if (c < 0)
				return (-1);
			tar_skip_file();
			return (0);
		}
	}
	i = chk_dir_in_tree(dp);
	if (i == 1)
	{
		t = pathname_to_dirtree(dp);
		if (t)
		{
			n = get_root_of_dir(t);
			fix_dir_list(n);
			*new_dirs = TRUE;
		}
	}

	rc = ztar_extract_file(new_file);
	return (rc);
}

static void restore_ztar_files (int do_confirm, int *new_dirs)
{
	int c;
	int zero_cnt;

	ai->a_buffer = (char *)MALLOC(ai->a_blkfactor*ai->a_blksize);
	if (!ai->a_buffer)
	{
		errmsg(ER_IM, "", ERR_ANY);
		return;
	}
	strcpy(ai->a_name, msgs(m_restore_gf));
	ai->a_open_flags = O_RDONLY;
	if (tar_open())
	{
		FREE(ai->a_buffer);
#if 0
		errsys(ER_COBF);
#endif
		return;
	}
	zero_cnt = 0;
	while (TRUE)
	{
		c = tar_read_rec();			/* read tar header */
		if (c <= 0)
			break;
		if (!*ai->a_recp)				/* skip empty rec */
		{
			if (zero_cnt)
				break;
			zero_cnt++;
			continue;
		}
		zero_cnt = 0;
		c = read_ztar_file(do_confirm, new_dirs);
		if (c)
			break;
	}
	tar_close(TRUE);
	FREE(ai->a_buffer);
}

static void restore_files (int do_confirm, int *new_dirs)
{
	switch (ai->a_dev_type)
	{
	case A_TAR:
		restore_tar_files(do_confirm, new_dirs);
		break;

	case A_BAR:
		restore_bar_files(do_confirm, new_dirs);
		break;

	case A_CPIO:
		restore_cpio_files(do_confirm, new_dirs);
		break;

	case A_ZTAR:
		restore_ztar_files(do_confirm, new_dirs);
		break;
	}
}

void do_restore (void)
{
	char input_str[MAX_PATHLEN];
	int do_confirm;
	int new_dirs;
	int c;

	/* get filename pattern */

	bang(msgs(m_restore_entfil));
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_restore_resall));
	wrefresh(gbl(win_commands));
	*ai->a_pattern = 0;
	c = xgetstr(gbl(win_commands), ai->a_pattern, XGL_PATTERN, MAX_PATHLEN, 0,
		XG_FILESPEC);
	if (c < 0)
	{
		return;
	}
	if (c == 0)
	{
		strcpy(ai->a_pattern, fn_all());
		waddstr(gbl(win_commands), ai->a_pattern);
		wrefresh(gbl(win_commands));
	}

	/* get archive device */

	if (get_arch_device(gbl(scr_cur)->path_name, FALSE))
		return;

	/* display "Restore ... from ..." */

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_restore_resall));
	waddstr(gbl(win_commands), ai->a_pattern);
	waddstr(gbl(win_commands), msgs(m_restore_frodev));
	waddstr(gbl(win_commands), ai->a_device);

	/* ask for archive type */

	if (determine_arch_type() && get_arch_type())
		return;

	/* ask relative pathname name */

	bang(msgs(m_restore_entrel));
	waddstr(gbl(win_message), "  ");
	fk_msg(gbl(win_message), CMDS_DEST_DIR, msgs(m_restore_f2pt));
	wrefresh(gbl(win_message));
	wmove(gbl(win_commands), 1, 0);
	wclrtoeol(gbl(win_commands));
	xaddstr(gbl(win_commands), msgs(m_restore_entnod));
	wrefresh(gbl(win_commands));
	*input_str = 0;
	c = xgetstr(gbl(win_commands), input_str, XGL_RELNODE, MAX_PATHLEN, 0,
		XG_PATHNAME);
	if (c < 0)
		return;
	fn_resolve_pathname(input_str);
	fn_get_abs_dir(gbl(scr_cur)->path_name, input_str, ai->a_rel_name);
	if (is_directory(ai->a_rel_name, 0))
		return;

	/* ask about confirming each file */

	c = yesno_msg(msgs(m_restore_cfm));
	if (c < 0)
		return;
	do_confirm = 1-c;

	/* ask about restoring full pathname */

	c = yesno_msg(msgs(m_exttar_extful));
	if (c < 0)
		return;
	ai->a_name_only = (c != 0);

	/* ask about replacing existing files */

	c = yesno_msg(msgs(m_restore_repexi));
	if (c < 0)
		return;
	ai->a_do_auto = 1-c;

	new_dirs = FALSE;
	restore_files(do_confirm, &new_dirs);

	if (new_dirs)
	{
		set_top_dir();
		if (gbl(scr_cur)->in_small_window)
		{
			disp_dir_tree();
			do_dir_scroll_bar();
			hilite_dir(OFF);
			left_arrow(gbl(scr_cur)->cur_dir_win);
			wrefresh(gbl(scr_cur)->cur_dir_win);
		}
	}
}
