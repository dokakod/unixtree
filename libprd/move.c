/*------------------------------------------------------------------------
 * process the "move" cmd
 */
#include "libprd.h"

struct move_info
{
	char	pattern[MAX_FILELEN];
	char	old_name[MAX_PATHLEN];
	char	new_name[MAX_PATHLEN];
	char	dest_dir[MAX_PATHLEN];
	char	new_filename[MAX_FILELEN];
	int		do_ask;
};
typedef struct move_info MOVE_INFO;

static int filemove (MOVE_INFO *mvi, char *old_name, char *new_name)
{
	int i;
	TREE *t;
	FBLK *f;
	BLIST *b;
	DBLK *d;

	if (os_file_rename(old_name, new_name))
	{
		i = errsys(ER_CRF);
		return (i);
	}
	t = pathname_to_dirtree(mvi->dest_dir);
	if (t)
	{
		if (t == gbl(scr_cur)->cur_dir_tree)
		{
			d = (DBLK *)tid(t);
			for (b=d->flist; b; b=bnext(b))
			{
				f = (FBLK *)bid(b);
				if (strcmp(mvi->new_filename, FULLNAME(f)) == 0)
				{
					remove_file(f, TRUE);
					break;
				}
			}
			fblk_change_name(gbl(scr_cur)->cur_file, mvi->new_filename);
			update_dir(d);
			hilite_file(ON);		/* already on but gotta disp new name */
			i = 1;
		}
		else
		{
			i = remove_file(gbl(scr_cur)->cur_file, FALSE);
			add_file_to_dir(mvi->new_filename, mvi->dest_dir, t, FALSE);
			if (gbl(scr_cur)->numfiles)
				hilite_file(ON);
		}
	}
	else
	{
		i = remove_file(gbl(scr_cur)->cur_file, FALSE);
		if (gbl(scr_cur)->numfiles)
			hilite_file(ON);
	}
	dblk_to_dirname((gbl(scr_cur)->cur_file)->dir, gbl(scr_cur)->path_name);
	disp_path_line();
	disp_cur_file();
	return (i);
}

static int move_file (void *data)
{
	MOVE_INFO *	mvi = (MOVE_INFO *)data;
	struct stat stbuf;
	int i;
	char *p, *q;

	make_file_name(mvi->pattern, FULLNAME(gbl(scr_cur)->cur_file),
		mvi->new_filename);
	strcpy(mvi->new_name, mvi->dest_dir);
	fn_append_filename_to_dir(mvi->new_name, mvi->new_filename);
	strcpy(mvi->old_name, gbl(scr_cur)->path_name);
	fn_append_filename_to_dir(mvi->old_name, FULLNAME(gbl(scr_cur)->cur_file));
	wmove(gbl(win_commands), 0, 0);
	wclrtoeol(gbl(win_commands));
	xaddstr(gbl(win_commands), msgs(m_move_mov));
	zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_file));
	waddstr(gbl(win_commands), msgs(m_move_as2));
	waddstr(gbl(win_commands), mvi->new_filename);
	wrefresh(gbl(win_commands));
	if (strcmp(mvi->old_name, mvi->new_name) == 0)
	{
		i = errmsg(ER_CMFTI, "", ERR_ANY);
		return (i);
	}
	if (os_stat(mvi->new_name, &stbuf) == 0)
	{
		if (mvi->do_ask)
		{
			p = msgs(m_move_mov);
			q = msgs(m_move_as1);
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
			show_file_info(mvi->new_filename,
			               stbuf.st_size,
			               stbuf.st_mtime,
			               stbuf.st_mode,
			               stbuf.st_uid,
			               stbuf.st_gid);
			i = errmsg(ER_FER, "", ERR_YESNO);
			if (i)
				return (i);
		}
		if (os_file_delete(mvi->new_name))
		{
			i = errsys(ER_CU);
			return (i);

		}
	}
	untag_file(gbl(scr_cur)->cur_file);
	i = filemove(mvi, mvi->old_name, mvi->new_name);
	disp_dir_stats();
	return (i);
}

void do_move (void)
{
	MOVE_INFO	move_info;
	MOVE_INFO *	mvi = &move_info;
	char input_str[MAX_PATHLEN];
	int i;
	int rc;

	bang(msgs(m_move_entfil));
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_move_movfil));
	zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_file));
	waddstr(gbl(win_commands), msgs(m_move_as3));
	wrefresh(gbl(win_commands));
	strcpy(mvi->pattern, FULLNAME(gbl(scr_cur)->cur_file));
	i = xgetstr(gbl(win_commands), mvi->pattern, XGL_PATTERN, MAX_FILELEN, 0,
		XG_FILENAME);
	if (i < 0)
		return;
	else if (i == 0)
	{
		strcpy(mvi->pattern, FULLNAME(gbl(scr_cur)->cur_file));
		zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_file));
		wrefresh(gbl(win_commands));
	}

	bang(msgs(m_move_entdesn));
	waddstr(gbl(win_message), "  ");
	fk_msg(gbl(win_message), CMDS_DEST_DIR, msgs(m_move_f2dir));
	wrefresh(gbl(win_message));

	wmove(gbl(win_commands), 1, 0);
	waddstr(gbl(win_commands), msgs(m_move_to));
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
		return;

	fn_resolve_pathname(input_str);
	fn_get_abs_dir(gbl(scr_cur)->path_name, input_str, mvi->dest_dir);
	rc = chk_dir_and_make(mvi->dest_dir);
	if (rc < 0)
		return;
	mvi->do_ask = 1;
	move_file(mvi);
	if (rc > 0)
		disp_new_tree_if_needed();
}

void do_move_files (void)
{
	MOVE_INFO	move_info;
	MOVE_INFO *	mvi = &move_info;
	char input_str[MAX_PATHLEN];
	int i;
	int rc;

	if (check_tag_count())
		return;

	bang(msgs(m_move_entfil));

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_move_movall));
	wrefresh(gbl(win_commands));
	strcpy(mvi->pattern, fn_all());
	i = xgetstr(gbl(win_commands), mvi->pattern, XGL_PATTERN, MAX_FILELEN, 0,
		XG_FILENAME);
	if (i < 0)
		return;
	else if (i == 0)
	{
		strcpy(mvi->pattern, fn_all());
		waddstr(gbl(win_commands), mvi->pattern);
		wrefresh(gbl(win_commands));
	}

	bang(msgs(m_move_entdesp));
	waddstr(gbl(win_message), "  ");
	fk_msg(gbl(win_message), CMDS_DEST_DIR, msgs(m_move_f2dir));
	wrefresh(gbl(win_message));

	wmove(gbl(win_commands), 1, 0);
	waddstr(gbl(win_commands), msgs(m_move_to));
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
		return;

	fn_resolve_pathname(input_str);
	fn_get_abs_dir(gbl(scr_cur)->path_name, input_str, mvi->dest_dir);
	rc = chk_dir_and_make(mvi->dest_dir);
	if (rc < 0)
		return;
	mvi->do_ask = FALSE;
	if (rc == 0)
	{
		mvi->do_ask = yesno_msg(msgs(m_move_repexi));
		if (mvi->do_ask < 0)
			return;
	}
	traverse(move_file, mvi);
	if (rc > 0)
		disp_new_tree_if_needed();
}
