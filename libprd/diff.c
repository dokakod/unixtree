/*------------------------------------------------------------------------
 * process the "diff" cmd
 */
#include "libprd.h"

static void do_diff (char *path1, char *path2)
{
	if (*opt(diff_rtn) != 0)
	{
		/* use specified diff program */

		char cmd[BUFSIZ];

		strcpy(cmd, opt(diff_rtn));
		strcat(cmd, " ");
		strcat(cmd, "\"");
		strcat(cmd, path1);
		strcat(cmd, "\"");
		strcat(cmd, " ");
		strcat(cmd, "\"");
		strcat(cmd, path2);
		strcat(cmd, "\"");

		xsystem(cmd, "", TRUE);
	}
	else
	{
		/* use internal diff program */

		DIFF_LINE *dl;
		char msgbuf[128];
		DIFF_OPTS diff_opts;
		int rc;

		memset(&diff_opts, 0, sizeof(diff_opts));
		diff_opts.ignore_case_flag			= opt(diff_ignore_case);
		diff_opts.ignore_space_change_flag	= opt(diff_ignore_ws);
		diff_opts.ignore_some_line_changes	= opt(diff_ignore_ws);

		dl = diff_engine(path1, path2, &diff_opts, &rc, msgbuf);

		switch (rc)
		{
		case DIFF_ERROR:
			bang(msgbuf);
			anykey();
			break;

		case DIFF_SAME_FILE:
			bang(msgs(m_diff_samefile));
			anykey();
			break;

		case DIFF_TXT_SAME:
			bang(msgs(m_diff_same));
			anykey();
			break;

		case DIFF_BIN_SAME:
			bang(msgs(m_diff_binsame));
			anykey();
			break;

		case DIFF_BIN_DIFF:
			bang(msgs(m_diff_bindiff));
			anykey();
			break;

		case DIFF_TXT_DIFF:
			diff_view(path1, path2, dl);
			break;
		}
	}
}

void diff (void)
{
	char path1[MAX_PATHLEN];
	char path2[MAX_PATHLEN];
	char filename[MAX_FILELEN];
	char directory[MAX_PATHLEN];
	int i;

	bang(msgs(m_diff_enter));

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_diff_compare));
	zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_file));
	waddstr(gbl(win_commands), msgs(m_diff_to));
	wrefresh(gbl(win_commands));

	make_file_name(opt(diff_pattern), FULLNAME(gbl(scr_cur)->cur_file),
		filename);
	if (gbl(scr_is_split))
	{
		i = next_scr_no(gbl(scr_cur_no));
		if (gbl(scr_stat)[i].command_mode != m_dir)
		{
			strcpy(filename, FULLNAME(gbl(scr_stat)[i].cur_file));
		}
	}

	i = xgetstr(gbl(win_commands), filename, XGL_DIFFNAMES, MAX_FILELEN, 0,
		XG_FILENAME_NB);
	if (i <= 0)
		return;

	bang(msgs(m_diff_entdes));
	fk_msg(gbl(win_message), CMDS_DEST_DIR, msgs(m_copy_f2dir));
	wrefresh(gbl(win_message));

	wmove(gbl(win_commands), 1, 0);
	waddstr(gbl(win_commands), msgs(m_diff_in));
	wrefresh(gbl(win_commands));

	*directory = 0;
	if (gbl(scr_is_split))
	{
		i = next_scr_no(gbl(scr_cur_no));
		if ((gbl(scr_stat)[i].cur_root)->node_type == N_FS)
			strcpy(directory, gbl(scr_stat)[i].path_name);
	}

	i = xgetstr(gbl(win_commands), directory, XGL_DEST, MAX_PATHLEN, 0,
		XG_PATHNAME);
	if (i < 0)
		return;
	if (i == 0)
		strcpy(directory, gbl(scr_cur)->path_name);

	fn_resolve_pathname(directory);
	fn_get_abs_dir(gbl(scr_cur)->path_name, directory, path2);
	fn_append_filename_to_dir(path2, filename);

	strcpy(path1, gbl(scr_cur)->path_name);
	fn_append_filename_to_dir(path1, FULLNAME(gbl(scr_cur)->cur_file));

	do_diff(path1, path2);
}
