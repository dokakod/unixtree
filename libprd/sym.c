/*------------------------------------------------------------------------
 * symlink routines
 */
#include "libprd.h"

int resolve_symlink (void)
{
	FBLK *f;
	TREE *t;
	DBLK *d;
	char dir_name[MAX_PATHLEN];
	char full_path[MAX_PATHLEN];
	struct stat stbuf;

	f = gbl(scr_cur)->cur_file;
	fblk_to_dirname(f, dir_name);
	strcpy(full_path, dir_name);
	fn_append_filename_to_dir(full_path, FULLNAME(f));
	if (os_stat(full_path, &stbuf))
	{
		errsys(ER_CRSYM);
		return (-1);
	}

	t = f->dir->dir_tree;
	if (! S_ISDIR(stbuf.st_mode))
	{
		add_file_to_dir(FULLNAME(f), dir_name, t, TRUE);
	}
	else
	{
		t = add_dir(FULLNAME(f), full_path, t);
		if (!t)
			return (-1);
		d = (DBLK *)tid(t);
		d->flags = D_NOT_LOGGED;
		remove_file(f, FALSE);
		gbl(scr_cur)->cur_dir_tree = t;
		gbl(scr_cur)->cur_dir = d;
		set_top_dir();
		gbl(scr_cur)->in_small_window = FALSE;
		gbl(scr_cur)->cmd_sub_mode = m_reg;
		return (1);
	}
	return (0);
}

int check_symlink (const char *path)
{
	char new_path[MAX_PATHLEN];
	char old_path[MAX_PATHLEN];
	struct stat stbuf;
	int i;
	int count;

	strcpy(old_path, path);
	for (count=0; count<16; count++)
	{
		i = os_readlink(old_path, new_path, sizeof(new_path));
		if (i < 0)
			return (i);
		new_path[i] = 0;

		if (fn_is_path_absolute(new_path))
			strcpy(old_path, new_path);
		else
			strcpy(fn_basename(old_path), new_path);
		fn_cleanup_path(old_path);

		i = os_lstat(old_path, &stbuf);
		if (i)
			return (i);

		if (! S_ISLNK(stbuf.st_mode))
		{
			if (S_ISDIR(stbuf.st_mode))
			{
				if (strncmp(path, old_path, strlen(old_path)) == 0)
					return (-1);
			}
			return (0);
		}
	}
	return (-1);
}

void do_alt_link (void)
{
	FBLK *f = gbl(scr_cur)->cur_file;
	char linkpath[MAX_PATHLEN];
	char fullpath[MAX_PATHLEN];
	char input_str[MAX_PATHLEN];
	struct stat stbuf;
	int make_sym_abs = TRUE;
	char *p;
	int rc;
	int c;

	if (! S_ISLNK(f->stbuf.st_mode) || f->sym_name == 0)
		return;

	bang(msgs(m_sym_changenm));
	waddstr(gbl(win_message), "  ");
	fk_msg(gbl(win_message), CMDS_DEST_DIR, msgs(m_copy_f2dir));
	wrefresh(gbl(win_message));

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_sym_changefr));
	zaddstr(gbl(win_commands), f->sym_name);
	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_sym_changeto));

	*input_str = 0;
	if (gbl(scr_is_split))
	{
		int i;

		i = next_scr_no(gbl(scr_cur_no));
		strcpy(input_str, gbl(scr_stat)[i].path_name);
		if (gbl(scr_stat)[i].command_mode != m_dir)
		{
			fn_append_filename_to_dir(input_str,
				FULLNAME(gbl(scr_stat)[i].cur_file));
		}
	}

	c = xgetstr(gbl(win_commands), input_str, XGL_SYM_NAMES, MAX_PATHLEN, 0,
		XG_STRING_DS);

	if (c <= 0)
		return;

	p = fn_basename(input_str);
	if (strcmp(p, "*") == 0)
	{
		strcpy(p, fn_basename(f->sym_name));
	}

	fn_resolve_pathname(input_str);
	fn_get_abs_dir(gbl(scr_cur)->path_name, input_str, linkpath);

	rc = os_lstat(linkpath, &stbuf);
	if (rc)
	{
		c = errmsg(ER_FDNEC, "", ERR_YESNO);
		if (c != 0)
			return;

		stbuf.st_mode = 0;
	}
	else
	{
		if (S_ISDIR(stbuf.st_mode))
		{
			fn_append_dirname_to_dir(linkpath, fn_cwdname());
		}
	}

	werase(gbl(win_message));
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
		make_sym_abs = FALSE;

	strcpy(fullpath, gbl(scr_cur)->path_name);
	fn_append_filename_to_dir(fullpath, FULLNAME(gbl(scr_cur)->cur_file));

	rc = os_file_delete(fullpath);
	if (rc)
	{
		errsys(ER_CDF);
		return;
	}

	rc = make_sym_link(linkpath, fullpath, make_sym_abs);
	if (rc)
	{
		errsys(ER_CLF);
		return;
	}

	check_the_file(fullpath);

	hilite_file(ON);
}

void fblk_sym (FBLK *f)
{
	char dir_name[MAX_PATHLEN];
	char sym_name[MAX_PATHLEN];
	char fullpath[MAX_PATHLEN];
	struct stat stbuf;
	int rc;

	if (f->sym_name != 0)
		FREE(f->sym_name);

	f->sym_name = 0;
	f->sym_mode = 0;

	if (! S_ISLNK(f->stbuf.st_mode))
		return;

	dblk_to_dirname(f->dir, dir_name);
	strcpy(fullpath, dir_name);
	fn_append_filename_to_dir(fullpath, FULLNAME(f));

	rc = os_readlink(fullpath, sym_name, sizeof(sym_name));
	if (rc <= 0)
		return;

	sym_name[rc] = 0;
	f->sym_name = STRDUP(sym_name);

	if (fn_is_path_absolute(sym_name))
	{
		strcpy(fullpath, sym_name);
	}
	else
	{
		char sym_dir[MAX_PATHLEN];
		char *sym_base;

		fn_dirname(sym_name, sym_dir);
		sym_base = fn_basename(sym_name);

		strcpy(fullpath, dir_name);
		fn_append_dirname_to_dir(fullpath, sym_dir);
		fn_append_filename_to_dir(fullpath, sym_base);
	}
	rc = os_lstat(fullpath, &stbuf);
	if (rc == 0)
		f->sym_mode = stbuf.st_mode;
}

int make_sym_link (const char *old_path, const char *new_path, int abs)
{
	char *old_base = fn_basename(old_path);
	char fixed_path[MAX_PATHLEN];
	int rc;

	strcpy(fixed_path, old_path);

	if (! abs)
	{
		char old_dir[MAX_PATHLEN];
		char new_dir[MAX_PATHLEN];

		fn_dirname(old_path, old_dir);
		fn_dirname(new_path, new_dir);

		if (strcmp(old_dir, new_dir) == 0)
		{
			strcpy(fixed_path, old_base);
		}
		else
		{
			int num_old_subdirs = fn_num_subdirs(old_dir);
			int num_new_subdirs = fn_num_subdirs(new_dir);
			char old_part[MAX_FILELEN];
			char new_part[MAX_FILELEN];
			int i;
			int j;

			for (i=0; ; i++)
			{
				if (i >= num_old_subdirs || i >= num_new_subdirs)
					break;

				fn_get_nth_dir(old_dir, i, old_part);
				fn_get_nth_dir(new_dir, i, new_part);
				if (strcmp(old_part, new_part) != 0)
					break;
			}

			if (i > 0)
			{
				*fixed_path = 0;

				for (j=i; j<num_new_subdirs; j++)
				{
					fn_append_dirname_to_dir(fixed_path, fn_parent());
				}

				for (j=i; j<num_old_subdirs; j++)
				{
					fn_get_nth_dir(old_dir, j, old_part);
					fn_append_dirname_to_dir(fixed_path, old_part);
				}
			}

			strcpy(fixed_path, fixed_path+1);

			if (strcmp(old_base, fn_cwdname()) != 0)
			{
				fn_append_filename_to_dir(fixed_path, old_base);
			}
		}
	}
	else
	{
		if (strcmp(old_base, fn_cwdname()) == 0)
		{
			fn_dirname(old_path, fixed_path);
		}
	}

	rc = os_symlink(fixed_path, new_path);

	return (rc);
}
