/*------------------------------------------------------------------------
 * ftp copy routines
 */
#include "libprd.h"

struct ftpcopy_info
{
	char	pattern[MAX_PATHLEN];
	char	old_name[MAX_PATHLEN];
	char	new_name[MAX_PATHLEN];
	char	dest_dir[MAX_PATHLEN];
	char	orig_dest_dir[MAX_PATHLEN];
	char	new_filename[MAX_PATHLEN];
	char	rel_base[MAX_PATHLEN];
	int		do_ask;
	int		new_dirs;
};
typedef struct ftpcopy_info FTPCOPY_INFO;

static int ftp_filecopy (char *old_name, char *new_name)
{
	NBLK *n;
	FTP_NODE *fn;

	n = gbl(scr_cur)->cur_root;
	fn = (FTP_NODE *)n->node_sub_blk;

	return (ftp_copy_file_from(fn, new_name, old_name));
}

static int ask_about_copy (FTPCOPY_INFO *fi)
{
	char	bang_msg[91];			/* leave room for _ */
	char input_str[MAX_PATHLEN];
	int i;
	int rc;

	bang(msgs(m_copy_entfil));

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_copy_copfil));
	zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_file));
	waddstr(gbl(win_commands), msgs(m_copy_as2));
	wrefresh(gbl(win_commands));

	strcpy(fi->pattern, FULLNAME(gbl(scr_cur)->cur_file));
	i = xgetstr(gbl(win_commands), fi->pattern, XGL_PATTERN, MAX_PATHLEN, 0,
		XG_FILENAME);
	if (i < 0)
		return (-1);

	else if (i == 0)
	{
		strcpy(fi->pattern, FULLNAME(gbl(scr_cur)->cur_file));
		zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_file));
		wrefresh(gbl(win_commands));
	}

	strcpy(bang_msg, msgs(m_copy_entdes));
	strcat(bang_msg, msgs(m_copy_cop3));
	strcat(bang_msg, msgs(m_copy_node));
	strcat(bang_msg, fk_msg_str(CMDS_DEST_DIR, msgs(m_copy_f2dir), input_str));
	bang(bang_msg);

	wmove(gbl(win_commands), 1, 0);
	waddstr(gbl(win_commands), msgs(m_copy_to3));
	wrefresh(gbl(win_commands));

	*input_str = 0;
	if (gbl(scr_is_split))
	{
		i = next_scr_no(gbl(scr_cur_no));
		if ((gbl(scr_stat)[i].cur_root)->node_type == N_FS )
			strcpy(input_str, gbl(scr_stat)[i].path_name);
	}
	i = xgetstr(gbl(win_commands), input_str, XGL_DEST, MAX_PATHLEN, 0,
		XG_PATHNAME);
	if (i <= 0)
		return (-1);

	fn_resolve_pathname(input_str);
	fn_get_abs_dir(gbl(scr_cur)->path_name, input_str, fi->dest_dir);

	/* check if dest dir exists */

	rc = chk_dir_and_make(fi->dest_dir);
	if (rc < 0)
		return (-1);

	return (0);
}

static int ftp_copy_the_file (FTPCOPY_INFO *fi)
{
	struct stat stbuf;
	int i;
	int c;
	char *p, *q;

	wmove(gbl(win_commands), 0, 0);
	wclrtoeol(gbl(win_commands));
	xaddstr(gbl(win_commands), msgs(m_copy_cop1));
	zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_file));
	waddstr(gbl(win_commands), msgs(m_copy_as1));
	waddstr(gbl(win_commands), fi->new_filename);
	wmove(gbl(win_commands), 1, 0);
	waddstr(gbl(win_commands), msgs(m_copy_to3));
	wclrtoeol(gbl(win_commands));
	waddstr(gbl(win_commands), fi->dest_dir);
	wrefresh(gbl(win_commands));

	if (strcmp(fi->old_name, fi->new_name) == 0)
	{
		c = errmsg(ER_CCFTI, "", ERR_ANY);
		return (c);
	}

	i = os_stat(fi->new_name, &stbuf);

	if (! S_ISREG((gbl(scr_cur)->cur_file)->stbuf.st_mode))
	{
		if (S_ISLNK((gbl(scr_cur)->cur_file)->stbuf.st_mode))
		{
			if (i == 0)
			{
				c = errmsg(ER_CCSLEF, "", ERR_ANY);
				return (c);
			}
		}
		else
		{
			c = errmsg(ER_CCSF, "", ERR_ANY);
			return (c);
		}
	}

	if (fi->do_ask)
	{
		if (i == 0)			/* does file exist? */
		{
			p = msgs(m_copy_cop1);
			q = msgs(m_copy_to1);
			werase(gbl(win_commands));
			wmove(gbl(win_commands), 0, 0);
			waddstr(gbl(win_commands), p);
			show_file_info(FULLNAME(gbl(scr_cur)->cur_file),
				(gbl(scr_cur)->cur_file)->stbuf.st_size,
				(gbl(scr_cur)->cur_file)->stbuf.st_mtime,
				(gbl(scr_cur)->cur_file)->stbuf.st_mode,
				(gbl(scr_cur)->cur_file)->stbuf.st_uid,
				(gbl(scr_cur)->cur_file)->stbuf.st_gid);

			wmove(gbl(win_commands), 1, display_len(p)-display_len(q));
			waddstr(gbl(win_commands), q);
			show_file_info(fi->new_filename,
				stbuf.st_size,
				stbuf.st_mtime,
				stbuf.st_mode,
				stbuf.st_uid,
				stbuf.st_gid);

			werase(gbl(win_message));
			esc_msg();
			i = errmsg(ER_FER, "", ERR_YESNO);
			if (i)
				return (i);
		}
	}

#if 0
	{
		long space_needed = file_size_in_blks(gbl(scr_cur)->cur_file,
			(gbl(scr_cur)->cur_file)->stbuf.st_size);
		if (i == 0)			/* does file exist? */
		{
			if (os_stat(fi->new_name, &stbuf) == 0)
			{
				space_needed -= file_size_in_blks(gbl(scr_cur)->cur_file,
					stbuf.st_size);
			}
		}

		if (os_stat(fi->dest_dir, &stbuf) == 0)
		{
			long blks_needed = (space_needed + 1023) / 1024;
			long space_avail = ml_get_free_by_dev(gbl(mount_list),
				stbuf.st_dev);

			if (space_avail >= 0 && blks_needed > space_avail)
			{
				c = errmsg(ER_NESDD, "", ERR_ANY);
				return (c);
			}
		}
	}
#endif

	i = ftp_filecopy(fi->old_name, fi->new_name);
	if (i)
		return (i);

	if (opt(keep_date_in_copy))
	{
		if (! S_ISLNK((gbl(scr_cur)->cur_file)->stbuf.st_mode))
		{
			i = os_set_file_time(fi->new_name,
				(gbl(scr_cur)->cur_file)->stbuf.st_mtime,
				(gbl(scr_cur)->cur_file)->stbuf.st_mtime);
			if (i)
				errsys(ER_CCD);
		}
	}

	check_for_file(fi->new_filename, fi->dest_dir);
	return (1);						/* for traverse() */
}

static int ftp_copy_file (void *data)
{
	FTPCOPY_INFO *	fi = (FTPCOPY_INFO *)data;
	int i;

	make_file_name(fi->pattern, FULLNAME(gbl(scr_cur)->cur_file),
		fi->new_filename);
	strcpy(fi->new_name, fi->dest_dir);
	fn_append_filename_to_dir(fi->new_name, fi->new_filename);
	strcpy(fi->old_name, gbl(scr_cur)->path_name);
	fn_append_filename_to_dir(fi->old_name, FULLNAME(gbl(scr_cur)->cur_file));

	i = ftp_copy_the_file(fi);
	return (i);
}

static int ftp_alt_copy_file (void *data)
{
	FTPCOPY_INFO *	fi = (FTPCOPY_INFO *)data;
	NBLK *n;
	TREE *t;
	int i;
	char *p;

	if (!fn_is_dir_in_dirname(fi->rel_base, gbl(scr_cur)->path_name))
	{
		return (errmsg(ER_RPBP, "", ERR_ANY));
	}

	strcpy(fi->dest_dir, fi->orig_dest_dir);
	i = fn_num_subdirs(fi->rel_base);
	while (TRUE)
	{
		char dir_part[MAX_FILELEN];

		p = fn_get_nth_dir(gbl(scr_cur)->path_name, i++, dir_part);
		if (!p)
			break;
		fn_append_dirname_to_dir(fi->dest_dir, p);
	}

	i = is_it_a_dir(fi->dest_dir, 0);
	if (i)
	{
		if (i == 1)
		{
			errmsgi(ER_INAD, fi->dest_dir, ERR_ANY);
			return (1);
		}
		else
		{
			if (make_dir_path(fi->dest_dir))
			{
				errmsg(ER_CMD, "", ERR_ANY);
				return (1);
			}
			if (chk_dir_in_tree(fi->dest_dir) > 0)
			{
				t = pathname_to_dirtree(fi->dest_dir);
				n = get_root_of_dir(t);
				fix_dir_list(n);
				set_top_dir();
				fi->new_dirs = TRUE;
			}
		}
	}

	i = ftp_copy_file(fi);
	return (i);
}

void ftp_do_copy (void)
{
	FTPCOPY_INFO	ftpcopy_info;
	FTPCOPY_INFO *	fi = &ftpcopy_info;

	if (ask_about_copy(fi))
		return;

	fi->do_ask = 1;
	ftp_copy_file(fi);
	disp_new_tree_if_needed();
}

void ftp_do_tag_copy (void)
{
	FTPCOPY_INFO	ftpcopy_info;
	FTPCOPY_INFO *	fi = &ftpcopy_info;
	char	bang_msg[91];			/* leave room for _ */
	char input_str[MAX_PATHLEN];
	int i;
	int rc;

	if (check_tag_count())
		return;

	bang(msgs(m_copy_entfil));
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_copy_copall));
	wrefresh(gbl(win_commands));
	*fi->pattern = 0;
	i = xgetstr(gbl(win_commands), fi->pattern, XGL_PATTERN, MAX_PATHLEN, 0,
		XG_FILENAME);
	if (i < 0)
		return;
	else if (i == 0)
	{
		strcpy(fi->pattern, fn_all());
		waddstr(gbl(win_commands), fi->pattern);
		wrefresh(gbl(win_commands));
	}
	strcpy(bang_msg, msgs(m_copy_entdes));
	strcat(bang_msg, msgs(m_copy_cop3));
	strcat(bang_msg, msgs(m_copy_node));
	strcat(bang_msg, fk_msg_str(CMDS_DEST_DIR, msgs(m_copy_f2dir), input_str));
	bang(bang_msg);
	wmove(gbl(win_commands), 1, 0);
	waddstr(gbl(win_commands), msgs(m_copy_to3));
	wrefresh(gbl(win_commands));
	*input_str = 0;
	i = xgetstr(gbl(win_commands), input_str, XGL_DEST, MAX_PATHLEN, 0,
		XG_PATHNAME);
	if (i <= 0)
		return;

	fn_resolve_pathname(input_str);
	fn_get_abs_dir(gbl(scr_cur)->path_name, input_str, fi->dest_dir);
	rc = chk_dir_and_make(fi->dest_dir);
	if (rc < 0)
		return;
	i = 0;
	if (rc == 0)
	{
		i = yesno_msg(msgs(m_copy_repexi));
		if (i < 0)
			return;
	}
	fi->do_ask = i;				/* yesno() returns 0 for yes */
	traverse(ftp_copy_file, fi);
	if (rc > 0)
		disp_new_tree_if_needed();
}

void ftp_do_alt_copy (void)
{
	FTPCOPY_INFO	ftpcopy_info;
	FTPCOPY_INFO *	fi = &ftpcopy_info;
	char	bang_msg[91];			/* leave room for _ */
	char input_str[MAX_PATHLEN];
	char *s;
	int i;
	int rc;

	/* check if any tagged files */

	if (check_tag_count())
		return;

	/* check if in global mode */

	if (gbl(scr_cur)->command_mode == m_global ||
	    gbl(scr_cur)->command_mode == m_tag_global)
	{
		errmsg(ER_ACNA, "", ERR_ANY);
		return;
	}

	bang(msgs(m_copy_entfil));
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	s = msgs(m_copy_dupcop);
	xaddstr(gbl(win_commands), s);
	wrefresh(gbl(win_commands));
	*fi->pattern = 0;
	i = xgetstr(gbl(win_commands), fi->pattern, XGL_PATTERN, MAX_PATHLEN, 0,
		XG_FILENAME);
	if (i < 0)
		return;
	else if (i == 0)
	{
		strcpy(fi->pattern, fn_all());
		waddstr(gbl(win_commands), fi->pattern);
		wrefresh(gbl(win_commands));
	}

	strcpy(bang_msg, msgs(m_copy_entalt));
	strcat(bang_msg, fk_msg_str(CMDS_DEST_DIR, msgs(m_copy_f2dir), input_str));
	bang(bang_msg);
	wmove(gbl(win_commands), 1, 0);
	waddstr(gbl(win_commands), msgs(m_copy_to3));
	wrefresh(gbl(win_commands));
	*input_str = 0;
	i = xgetstr(gbl(win_commands), input_str, XGL_DEST, MAX_PATHLEN, 0,
		XG_PATHNAME);
	if (i <= 0)
		return;

	fn_resolve_pathname(input_str);
	fn_get_abs_dir(gbl(scr_cur)->path_name, input_str, fi->orig_dest_dir);

	rc = chk_dir_and_make(fi->orig_dest_dir);
	if (rc < 0)
		return;
	fi->new_dirs = (rc > 0);

	bang(msgs(m_copy_altbase));
	waddstr(gbl(win_message), "  ");
	fk_msg(gbl(win_message), CMDS_DEST_DIR, msgs(m_copy_f2dir));
	wrefresh(gbl(win_message));
	wmove(gbl(win_commands), 1, 0);
	wclrtoeol(gbl(win_commands));
	xaddstr(gbl(win_commands), msgs(m_copy_basenode));
	wrefresh(gbl(win_commands));
	strcpy(input_str, (gbl(scr_cur)->cur_root)->root_name);
	i = xgetstr(gbl(win_commands), input_str, XGL_REL_BASE, MAX_PATHLEN, 0,
		XG_PATHNAME);
	if (i <= 0)
		return;

	fn_resolve_pathname(input_str);
	fn_get_abs_dir(gbl(scr_cur)->path_name, input_str, fi->rel_base);

	fi->do_ask = FALSE;
	if (rc > 0)
	{
		fi->do_ask = yesno_msg(msgs(m_copy_repexi));
		if (fi->do_ask < 0)
			return;
	}

	traverse(ftp_alt_copy_file, fi);
	if (fi->new_dirs)
		disp_new_tree_if_needed();
}
