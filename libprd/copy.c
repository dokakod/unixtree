/*------------------------------------------------------------------------
 * process the "copy" & "link" cmds
 */
#include "libprd.h"

struct copy_info
{
	char	old_name[MAX_PATHLEN];
	char	new_name[MAX_PATHLEN];
	char	pattern[MAX_PATHLEN];
	char	dest_dir[MAX_PATHLEN];
	char	orig_dest_dir[MAX_PATHLEN];
	char	new_filename[MAX_PATHLEN];
	char	rel_base[MAX_PATHLEN];
	int		do_ask;
	int		copy_mode;					/* 0=link, 1=copy */
	int		file_dir;					/* 0=file, 1=dir */
	int		make_symlink;
	int		make_sym_abs;
	int		new_dirs;
};
typedef struct copy_info COPY_INFO;

static int ask_about_copy_link (COPY_INFO *cpyi)
{
	char bang_msg[91];			/* leave room for _ */
	char input_str[MAX_PATHLEN];
	int rc;
	int i;

	bang(msgs(m_copy_entfil));
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	if (cpyi->file_dir)
	{
		xaddstr(gbl(win_commands), msgs(m_copy_lindir));
		zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_dir));
		strcpy(cpyi->pattern, FULLNAME(gbl(scr_cur)->cur_dir));
	}
	else
	{
		if (cpyi->copy_mode)
			xaddstr(gbl(win_commands), msgs(m_copy_copfil));
		else
			xaddstr(gbl(win_commands), msgs(m_copy_linfil));
		zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_file));
		strcpy(cpyi->pattern, FULLNAME(gbl(scr_cur)->cur_file));
	}
	if (cpyi->copy_mode)
		waddstr(gbl(win_commands), msgs(m_copy_as2));
	else
		waddstr(gbl(win_commands), msgs(m_copy_to2));
	wrefresh(gbl(win_commands));
	i = xgetstr(gbl(win_commands), cpyi->pattern, XGL_PATTERN, MAX_PATHLEN, 0,
		XG_FILENAME);
	if (i < 0)
		return (-1);
	else if (i == 0)
	{
		if (cpyi->file_dir)
		{
			strcpy(cpyi->pattern, FULLNAME(gbl(scr_cur)->cur_dir));
			zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_dir));
		}
		else
		{
			strcpy(cpyi->pattern, FULLNAME(gbl(scr_cur)->cur_file));
			zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_file));
		}
		wrefresh(gbl(win_commands));
	}

#if V_WINDOWS
	if (! cpyi->copy_mode)
	{
		char *ext = fn_ext(cpyi->pattern);

		if (ext == 0 || strcmp(ext, "lnk") != 0)
			strcat(cpyi->pattern, ".lnk");
	}
#endif

	strcpy(bang_msg, msgs(m_copy_entdes));
	if (cpyi->copy_mode)
		strcat(bang_msg, msgs(m_copy_cop3));
	else
		strcat(bang_msg, msgs(m_copy_lin2));
	strcat(bang_msg, msgs(m_copy_node));
	strcat(bang_msg, fk_msg_str(CMDS_DEST_DIR, msgs(m_copy_f2dir), input_str));
	bang(bang_msg);
	wmove(gbl(win_commands), 1, 0);
	if (cpyi->copy_mode)
		waddstr(gbl(win_commands), msgs(m_copy_to3));
	else
		waddstr(gbl(win_commands), msgs(m_copy_in));
	wrefresh(gbl(win_commands));
	*input_str = 0;
	if (gbl(scr_is_split))
	{
		i = next_scr_no(gbl(scr_cur_no));
		if ((gbl(scr_stat)[i].cur_root)->node_type == N_FS)
			strcpy(input_str, gbl(scr_stat)[i].path_name);
	}
	i = xgetstr(gbl(win_commands), input_str, XGL_DEST, MAX_PATHLEN, 0,
		XG_PATHNAME);
	if (i <= 0)
		return (-1);

	fn_resolve_pathname(input_str);
	fn_get_abs_dir(gbl(scr_cur)->path_name, input_str, cpyi->dest_dir);

	/* check if dest dir exists */

	rc = chk_dir_and_make(cpyi->dest_dir);
	if (rc < 0)
		return (-1);

	return (0);
}

static int copysym (const char *old_name, const char *new_name)
{
	char buf[MAX_PATHLEN];
	int i;

	i = os_readlink(old_name, buf, sizeof(buf));
	if (i < 0)
	{
		i = errsys(ER_CRSL);
		return (i);
	}
	buf[i] = 0;
	i = os_symlink(buf, new_name);
	if (i < 0)
	{
		i = errsys(ER_CCSL);
		return (i);
	}
	return (0);
}

static int filelink (COPY_INFO *cpyi, const char *old_name,
	const char *new_name)
{
	int c;

	if (cpyi->make_symlink)
		c = make_sym_link(old_name, new_name, cpyi->make_sym_abs);
	else
		c = os_link(old_name, new_name);

	if (c)
	{
		c = errsys(ER_CLF);
		return (c);
	}
	return (0);
}

static int copy_the_file (COPY_INFO *cpyi)
{
	struct stat stbuf;
	BLIST *l;
	FBLK *f = gbl(scr_cur)->cur_file;
	NBLK *n;
	int i;
	int c;
	char *p, *q;
	int frow;
	int fcol;

	wmove(gbl(win_commands), 0, 0);
	wclrtoeol(gbl(win_commands));
	if (cpyi->copy_mode)
		xaddstr(gbl(win_commands), msgs(m_copy_cop1));
	else
		xaddstr(gbl(win_commands), msgs(m_copy_lin1));
	zaddstr(gbl(win_commands), FULLNAME(f));
	waddstr(gbl(win_commands), msgs(m_copy_as1));
	waddstr(gbl(win_commands), cpyi->new_filename);
	wmove(gbl(win_commands), 1, 0);
	waddstr(gbl(win_commands), msgs(m_copy_to3));
	wclrtoeol(gbl(win_commands));
	waddstr(gbl(win_commands), cpyi->dest_dir);
	wrefresh(gbl(win_commands));

	if (strcmp(cpyi->old_name, cpyi->new_name) == 0)
	{
		if (cpyi->copy_mode)
			c = errmsg(ER_CCFTI, "", ERR_ANY);
		else
			c = errmsg(ER_CLFTI, "", ERR_ANY);
		return (c);
	}

	i = os_stat(cpyi->new_name, &stbuf);

	if (! S_ISREG(f->stbuf.st_mode))
	{
		if (S_ISLNK(f->stbuf.st_mode))
		{
			if (cpyi->copy_mode)
			{
				if (i == 0)
				{
					c = errmsg(ER_CCSLEF, "", ERR_ANY);
					return (c);
				}
			}
			else
			{
				c = errmsg(ER_CLSL, "", ERR_ANY);
				return (c);
			}
		}
		else
		if (cpyi->copy_mode)
		{
			c = errmsg(ER_CCSF, "", ERR_ANY);
			return (c);
		}
	}

	if (!cpyi->copy_mode && !i)		/* link & exists ? */
	{
		c = errmsg(ER_CLEF, "", ERR_ANY);
		return (c);
	}

	if (cpyi->do_ask)
	{
		if (i == 0)			/* does file exist? */
		{
			p = msgs(m_copy_cop1);
			q = msgs(m_copy_to1);
			werase(gbl(win_commands));
			wmove(gbl(win_commands), 0, 0);
			waddstr(gbl(win_commands), p);
			show_file_info(FULLNAME(f),
				f->stbuf.st_size,
				f->stbuf.st_mtime,
				f->stbuf.st_mode,
				f->stbuf.st_uid,
				f->stbuf.st_gid);
			wmove(gbl(win_commands), 1, display_len(p)-display_len(q));
			waddstr(gbl(win_commands), q);
			show_file_info(cpyi->new_filename,
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
	if (cpyi->copy_mode)
	{
		long space_needed = file_size_in_blks(f, f->stbuf.st_size);

		if (i == 0)			/* does file exist? */
		{
			if (os_stat(cpyi->new_name, &stbuf) == 0)
				space_needed -= file_size_in_blks(f, stbuf.st_size);
		}

		if (os_stat(cpyi->dest_dir, &stbuf) == 0)
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

	if (cpyi->copy_mode)
	{
		if (S_ISLNK(f->stbuf.st_mode))
			i = copysym(cpyi->old_name, cpyi->new_name);
		else
			i = filecopy(cpyi->old_name, cpyi->new_name, f->stbuf.st_mode);
	}
	else
	{
		i = filelink(cpyi, cpyi->old_name, cpyi->new_name);
		if (!cpyi->make_symlink && i == 0)
		{
			n = get_root_of_file(f);
			f->stbuf.st_nlink++;
			if (gbl(scr_cur)->file_fmt == fmt_fdate)
				hilite_file(ON);
			if (f->stbuf.st_nlink == fmt_fdate)	/* new link? */
			{
				l = BNEW(f);
				if (l)
				{
					n->link_list = bappend(n->link_list, l);
				}
			}
			for (l=n->link_list; l; l=bnext(l))
			{
				FBLK *ft = (FBLK *)bid(l);

				if (ft != f && ft->stbuf.st_ino == f->stbuf.st_ino &&
					ft->stbuf.st_dev == f->stbuf.st_dev)
				{
					ft->stbuf.st_nlink++;
					if (gbl(scr_cur)->file_fmt == fmt_fdate)
						if (is_file_displayed(ft, &frow, &fcol))
							redisplay_file(ft, frow, fcol);
				}
			}
		}
	}
	if (i)
		return (i);
	if (cpyi->copy_mode && opt(keep_date_in_copy))
	{
		if (! S_ISLNK(f->stbuf.st_mode))
		{
			i = os_set_file_time(cpyi->new_name,
				f->stbuf.st_mtime,
				f->stbuf.st_mtime);
			if (i)
				errsys(ER_CCD);
		}
	}
	check_for_file(cpyi->new_filename, cpyi->dest_dir);
	return (1);						/* for traverse() */
}

static int copy_cur_file (void *data)
{
	COPY_INFO *cpyi = (COPY_INFO *)data;
	int i;

	make_file_name(cpyi->pattern, FULLNAME(gbl(scr_cur)->cur_file),
		cpyi->new_filename);
	strcpy(cpyi->new_name, cpyi->dest_dir);
	fn_append_filename_to_dir(cpyi->new_name, cpyi->new_filename);
	strcpy(cpyi->old_name, gbl(scr_cur)->path_name);
	fn_append_filename_to_dir(cpyi->old_name,
		FULLNAME(gbl(scr_cur)->cur_file));
	i = copy_the_file(cpyi);
	return (i);
}

static int alt_copy_file (void *data)
{
	COPY_INFO *cpyi = (COPY_INFO *)data;
	NBLK *n;
	TREE *t;
	int i;
	char *p;
	char dir_part[MAX_FILELEN];

	if (!fn_is_dir_in_dirname(cpyi->rel_base, gbl(scr_cur)->path_name))
	{
		return (errmsg(ER_RPBP, "", ERR_ANY));
	}

	strcpy(cpyi->dest_dir, cpyi->orig_dest_dir);
	i = fn_num_subdirs(cpyi->rel_base);
	while (TRUE)
	{
		p = fn_get_nth_dir(gbl(scr_cur)->path_name, i++, dir_part);
		if (!p)
			break;
		fn_append_dirname_to_dir(cpyi->dest_dir, p);
	}

	i = is_it_a_dir(cpyi->dest_dir, 0);
	if (i)
	{
		if (i == 1)
		{
			errmsgi(ER_INAD, cpyi->dest_dir, ERR_ANY);
			return (1);
		}
		else
		{
			if (make_dir_path(cpyi->dest_dir))
			{
				errmsg(ER_CMD,  "", ERR_ANY);
				return (1);
			}
			if (chk_dir_in_tree(cpyi->dest_dir) > 0)
			{
				t = pathname_to_dirtree(cpyi->dest_dir);
				n = get_root_of_dir(t);
				fix_dir_list(n);
				set_top_dir();
				cpyi->new_dirs = TRUE;
			}
		}
	}

	i = copy_cur_file(0);
	return (i);
}

static int link_the_dir (COPY_INFO *cpyi)
{
	int i;
	int c;
	struct stat stbuf;

	wmove(gbl(win_commands), 0, 0);
	wclrtoeol(gbl(win_commands));
	xaddstr(gbl(win_commands), msgs(m_copy_lin1));
	zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_dir));
	waddstr(gbl(win_commands), msgs(m_copy_as1));
	waddstr(gbl(win_commands), cpyi->new_filename);
	wmove(gbl(win_commands), 1, 0);
	waddstr(gbl(win_commands), msgs(m_copy_to3));
	wclrtoeol(gbl(win_commands));
	waddstr(gbl(win_commands), cpyi->dest_dir);
	wrefresh(gbl(win_commands));
	if (strcmp(cpyi->old_name, cpyi->new_name) == 0)
	{
		c = errmsg(ER_CLFTI, "", ERR_ANY);
		return (c);
	}
	i = os_stat(cpyi->new_name, &stbuf);
	if (!i)
	{
		c = errmsg(ER_CLEF, "", ERR_ANY);
		return (c);
	}
	i = filelink(cpyi, cpyi->old_name, cpyi->new_name);
	if (i)
		return (i);
	check_for_file(cpyi->new_filename, cpyi->dest_dir);
	return (0);
}

static int copy_it (COPY_INFO *cpyi)
{
	if (ask_about_copy_link(cpyi))
		return (0);

	cpyi->make_symlink = FALSE;
	cpyi->make_sym_abs = TRUE;

	if (! cpyi->copy_mode)
	{
#if V_UNIX
		int c;

		esc_msg();
		bangnc(msgs(m_copy_harsof));
		xcaddstr(gbl(win_message), CMDS_LINK_HARD, msgs(m_copy_hard));
		xaddstr(gbl(win_message), msgs(m_copy_or));
		xcaddstr(gbl(win_message), CMDS_LINK_SOFT, msgs(m_copy_soft));
		xaddstr(gbl(win_message), msgs(m_copy_link));
		wrefresh(gbl(win_message));
		while (TRUE)
		{
			c = xgetch(gbl(win_message));
			c = TO_LOWER(c);
			if (c == KEY_ESCAPE ||
				c == cmds(CMDS_LINK_HARD) ||
				c == cmds(CMDS_LINK_SOFT))
				break;
		}
		bang("");
		if (c == KEY_ESCAPE)
			return (0);
		if (c == cmds(CMDS_LINK_SOFT))
			cpyi->make_symlink = TRUE;

		if (cpyi->make_symlink)
		{
			esc_msg();
			bangnc(msgs(m_copy_harsof));
			xcaddstr(gbl(win_message), CMDS_LINK_ABS, msgs(m_copy_abs));
			xaddstr(gbl(win_message), msgs(m_copy_or));
			xcaddstr(gbl(win_message), CMDS_LINK_REL, msgs(m_copy_rel));
			xaddstr(gbl(win_message), msgs(m_copy_link));
			wrefresh(gbl(win_message));
			while (TRUE)
			{
				c = xgetch(gbl(win_message));
				c = TO_LOWER(c);
				if (c == KEY_ESCAPE ||
					c == cmds(CMDS_LINK_ABS) ||
					c == cmds(CMDS_LINK_REL))
						break;
			}
			bang("");
			if (c == KEY_ESCAPE)
				return (0);
			if (c == cmds(CMDS_LINK_REL))
				cpyi->make_sym_abs = FALSE;
		}
#else
		cpyi->make_symlink = TRUE;
#endif
	}

	cpyi->do_ask = 1;
	copy_cur_file(cpyi);

	if (cpyi->new_dirs)
		disp_new_tree_if_needed();

	return (0);
}

void do_link_dir (void)
{
	COPY_INFO	copy_info;
	COPY_INFO *	cpyi = &copy_info;

	cpyi->copy_mode = 0;
	cpyi->file_dir = 1;
	cpyi->make_symlink = TRUE;
	cpyi->make_sym_abs = TRUE;
	if (ask_about_copy_link(cpyi))
		return;

#if V_UNIX
	{
		int c;

		esc_msg();
		bangnc(msgs(m_copy_harsof));
		xcaddstr(gbl(win_message), CMDS_LINK_ABS, msgs(m_copy_abs));
		xaddstr(gbl(win_message), msgs(m_copy_or));
		xcaddstr(gbl(win_message), CMDS_LINK_REL, msgs(m_copy_rel));
		xaddstr(gbl(win_message), msgs(m_copy_link));
		wrefresh(gbl(win_message));
		while (TRUE)
		{
			c = xgetch(gbl(win_message));
			c = TO_LOWER(c);
			if (c == KEY_ESCAPE ||
				c == cmds(CMDS_LINK_ABS) ||
				c == cmds(CMDS_LINK_REL))
					break;
		}
		bang("");
		if (c == KEY_ESCAPE)
			return;
		if (c == cmds(CMDS_LINK_REL))
			cpyi->make_sym_abs = FALSE;
	}
#endif

	make_file_name(cpyi->pattern, FULLNAME(gbl(scr_cur)->cur_dir),
		cpyi->new_filename);
	strcpy(cpyi->new_name, cpyi->dest_dir);
	fn_append_filename_to_dir(cpyi->new_name, cpyi->new_filename);
	strcpy(cpyi->old_name, gbl(scr_cur)->path_name);
	fn_append_dirname_to_dir(cpyi->old_name, fn_cwdname());
	link_the_dir(cpyi);
	disp_new_tree_if_needed();
}

void do_link_file (void)
{
	COPY_INFO	copy_info;
	COPY_INFO *	cpyi = &copy_info;

	cpyi->copy_mode = 0;
	cpyi->file_dir = 0;

	copy_it(cpyi);
}

void do_copy (void)
{
	COPY_INFO	copy_info;
	COPY_INFO *	cpyi = &copy_info;

	cpyi->copy_mode = 1;
	cpyi->file_dir = 0;

	copy_it(cpyi);
}

int chk_dir_and_make (const char *path)
{
	int new_dirs;
	TREE *t;
	NBLK *n;
	int i;

	new_dirs = FALSE;
	i = is_it_a_dir(path, 0);
	if (i)
	{
		if (i < 0)
		{
			i = errmsg(ER_DNEMNP, "", ERR_YESNO);
			if (i)
				return (-1);
			if (make_dir_path(path))
			{
				errmsg(ER_CMD, "", ERR_ANY);
				return (-1);
			}

			if (chk_dir_in_tree(path) > 0)
			{
				t = pathname_to_dirtree(path);
				if (t)
				{
					n = get_root_of_dir(t);
					fix_dir_list(n);
					set_top_dir();
					new_dirs = TRUE;
				}
			}
		}
		else
		{
			errmsgi(ER_INAD, path, ERR_ANY);
			return (-1);
		}
	}
	return (new_dirs ? 1 : 0);
}

void disp_new_tree_if_needed (void)
{
	if (gbl(scr_cur)->in_small_window)
	{
		disp_dir_tree();
		hilite_dir(OFF);
		left_arrow(gbl(scr_cur)->cur_dir_win);
		wrefresh(gbl(scr_cur)->cur_dir_win);
	}
}

static int tag_copy_it (COPY_INFO *cpyi)
{
	char bang_msg[91];			/* leave room for _ */
	char input_str[MAX_PATHLEN];
	int i;
	int rc;

	if (check_tag_count())
		return (0);

	bang(msgs(m_copy_entfil));
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	if (cpyi->copy_mode)
		xaddstr(gbl(win_commands), msgs(m_copy_copall));
	else
		xaddstr(gbl(win_commands), msgs(m_copy_linall));
	wrefresh(gbl(win_commands));
	strcpy(cpyi->pattern, fn_all());
	i = xgetstr(gbl(win_commands), cpyi->pattern, XGL_PATTERN, MAX_PATHLEN, 0,
		XG_FILENAME);
	if (i < 0)
	{
		return (0);
	}
	else if (i == 0)
	{
		strcpy(cpyi->pattern, fn_all());
		waddstr(gbl(win_commands), cpyi->pattern);
		wrefresh(gbl(win_commands));
	}

	strcpy(bang_msg, msgs(m_copy_entdes));
	if (cpyi->copy_mode)
		strcat(bang_msg, msgs(m_copy_cop3));
	else
		strcat(bang_msg, msgs(m_copy_lin2));
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
		if ((gbl(scr_stat)[i].cur_root)->node_type == N_FS)
			strcpy(input_str, gbl(scr_stat)[i].path_name);
	}
	i = xgetstr(gbl(win_commands), input_str, XGL_DEST, MAX_PATHLEN, 0,
		XG_PATHNAME);
	if (i <= 0)
		return (0);

	fn_resolve_pathname(input_str);
	fn_get_abs_dir(gbl(scr_cur)->path_name, input_str, cpyi->dest_dir);
	rc = chk_dir_and_make(cpyi->dest_dir);
	if (rc < 0)
		return (0);
	i = 0;
	if (rc == 0)
	{
		i = yesno_msg(msgs(m_copy_repexi));
		if (i < 0)
			return (0);
	}
	cpyi->do_ask = i;				/* yesno() returns 0 for yes */
	traverse(copy_cur_file, cpyi);
	if (rc > 0)
		disp_new_tree_if_needed();

	return (0);
}

void do_tag_link (void)
{
	COPY_INFO	copy_info;
	COPY_INFO *	cpyi = &copy_info;

	cpyi->copy_mode = 0;
	tag_copy_it(cpyi);
}

void do_tag_copy (void)
{
	COPY_INFO	copy_info;
	COPY_INFO *	cpyi = &copy_info;

	cpyi->copy_mode = 1;
	tag_copy_it(cpyi);
}

void do_alt_copy (void)
{
	COPY_INFO	copy_info;
	COPY_INFO *	cpyi = &copy_info;
	char bang_msg[91];			/* leave room for _ */
	char input_str[MAX_PATHLEN];
	char *s;
	int i;
	int rc;

	cpyi->copy_mode = 1;

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
	strcpy(cpyi->pattern, fn_all());
	i = xgetstr(gbl(win_commands), cpyi->pattern, XGL_PATTERN, MAX_PATHLEN, 0,
		XG_FILENAME);
	if (i < 0)
		return;
	else if (i == 0)
	{
		strcpy(cpyi->pattern, fn_all());
		waddstr(gbl(win_commands), cpyi->pattern);
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
	fn_get_abs_dir(gbl(scr_cur)->path_name, input_str, cpyi->orig_dest_dir);

	rc = chk_dir_and_make(cpyi->orig_dest_dir);
	if (rc < 0)
		return;
	cpyi->new_dirs = (rc > 0);

#if 0
	{
		TREE *t;

		t = pathname_to_dirtree(cpyi->orig_dest_dir);
		if (t)
		{
			for (; tparent(t); t=tparent(t))
				;
			if ((gbl(scr_cur)->cur_root)->root == t)
			{
				errmsg(ER_CACDCN, "", ERR_ANY);
				return;
			}
		}
	}
#endif

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
	fn_get_abs_dir(gbl(scr_cur)->path_name, input_str, cpyi->rel_base);

	cpyi->do_ask = FALSE;
	if (rc == 0)
	{
		cpyi->do_ask = yesno_msg(msgs(m_copy_repexi));
		if (cpyi->do_ask < 0)
			return;
	}

	traverse(alt_copy_file, cpyi);
	if (cpyi->new_dirs)
		disp_new_tree_if_needed();
}

int filecopy (const char *old_name, const char *new_name, int mode)
{
	int inp;
	int out;
	int size;
	char buffer[BUFSIZ];
	int buflen = sizeof(buffer);
	int c;

	inp = os_open(old_name, O_RDONLY, 0666);
	if (inp == -1)
	{
		c = errsys(ER_COSF);
		return (c);
	}

	out = os_open(new_name, O_WRONLY|O_CREAT|O_TRUNC, mode);
	if (out == -1)
	{
		os_close(inp);
		c = errsys(ER_CODF);
		return (c);
	}

	while (TRUE)
	{
		size = os_read(inp, buffer, buflen);
		if (size == 0)
			break;
		if (size == -1 || os_write(out, buffer, size) == -1)
		{
			os_close(inp);
			os_close(out);
			c = errsys(ER_IOERR);
			return (c);
		}
	}

	os_close(inp);
	os_close(out);

	return (0);
}
