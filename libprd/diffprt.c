/*------------------------------------------------------------------------
 * "diff" print routines
 */
#include "libprd.h"

static void print_line (void *data, const char *line)
{
	PRINTER *	prt = (PRINTER *)data;

	prt_output_str(prt, line);
}

void diff_print (DIFF_LINE *lines)
{
	int			report_to_file;
	int			report_as_diff;
	PRINTER	*	report_prt;
	char		report_path[MAX_PATHLEN];
	char		msgbuf[256];
	int			c;
	int			rc;

	/*--------------------------------------------------------------------
	 * get report type: "diff" or "side-by-side"
	 */
	werase  (gbl(win_commands));
	wmove   (gbl(win_commands), 0, 0);
	xaddstr (gbl(win_commands), msgs(m_diff_prtask1));
	xcaddstr(gbl(win_commands), CMDS_DIFF_PRINT_SIDE, msgs(m_diff_prtasks));
	xaddstr (gbl(win_commands), msgs(m_diff_prtaskor));
	xcaddstr(gbl(win_commands), CMDS_DIFF_PRINT_DIFF, msgs(m_diff_prtaskd));
	wrefresh(gbl(win_commands));

	werase(gbl(win_message));
	esc_msg();
	wmove   (gbl(win_message), 0, 0);
	xaddstr (gbl(win_message), msgs(m_diff_prompt));
	xcaddstr(gbl(win_message), CMDS_DIFF_PRINT_SIDE, msgs(m_diff_prompts));
	xaddstr (gbl(win_message), msgs(m_diff_prtaskor));
	xcaddstr(gbl(win_message), CMDS_DIFF_PRINT_DIFF, msgs(m_diff_promptd));
	waddch  (gbl(win_message), ' ');
	wrefresh(gbl(win_message));

	while (TRUE)
	{
		c = xgetch(gbl(win_message));
		c = TO_LOWER(c);

		if (c == KEY_ESCAPE ||
			c == cmds(CMDS_DIFF_PRINT_SIDE) ||
			c == cmds(CMDS_DIFF_PRINT_DIFF))
		{
			break;
		}
	}
	bang("");

	if (c == KEY_ESCAPE)
		return;

	report_as_diff = (c == cmds(CMDS_DIFF_PRINT_DIFF));

	/*--------------------------------------------------------------------
	 * get whether to print or write-file
	 */
	werase  (gbl(win_commands));
	esc_msg();
	wmove   (gbl(win_commands), 0, 0);
	xaddstr (gbl(win_commands), msgs(m_diff_prtask2));
	xcaddstr(gbl(win_commands), CMDS_DIFF_PRINT_FILE,    msgs(m_diff_prtaskf));
	xaddstr (gbl(win_commands), msgs(m_diff_prtaskor));
	xcaddstr(gbl(win_commands), CMDS_DIFF_PRINT_PRINTER, msgs(m_diff_prtaskp));
	wrefresh(gbl(win_commands));

	werase(gbl(win_message));
	esc_msg();
	wmove   (gbl(win_message), 0, 0);
	xaddstr (gbl(win_message), msgs(m_diff_prompt));
	xcaddstr(gbl(win_message), CMDS_DIFF_PRINT_FILE,    msgs(m_diff_promptp));
	xaddstr (gbl(win_message), msgs(m_diff_prtaskor));
	xcaddstr(gbl(win_message), CMDS_DIFF_PRINT_PRINTER, msgs(m_diff_promptf));
	waddch  (gbl(win_message), ' ');
	wrefresh(gbl(win_message));

	while (TRUE)
	{
		c = xgetch(gbl(win_message));
		c = TO_LOWER(c);

		if (c == KEY_ESCAPE ||
			c == cmds(CMDS_DIFF_PRINT_FILE) ||
			c == cmds(CMDS_DIFF_PRINT_PRINTER))
		{
			break;
		}
	}
	bang("");

	if (c == KEY_ESCAPE)
		return;

	report_to_file = (c == cmds(CMDS_DIFF_PRINT_FILE));

	/*--------------------------------------------------------------------
	 * if print-to-file, get pathname to create
	 */
	if (report_to_file)
	{
		char filename[MAX_FILELEN];
		char directory[MAX_PATHLEN];

		bang(msgs(m_diff_promptfil));

		werase  (gbl(win_commands));
		wmove   (gbl(win_commands), 0, 0);
		xaddstr (gbl(win_commands), msgs(m_diff_filename));
		wrefresh(gbl(win_commands));

		*filename = 0;
		c = xgetstr(gbl(win_commands), filename, XGL_DIFFRPT, MAX_FILELEN, 0,
			XG_FILENAME);
		if (c <= 0)
			return;

		bang(msgs(m_diff_entdes));
		fk_msg(gbl(win_message), CMDS_DEST_DIR, msgs(m_copy_f2dir));
		wrefresh(gbl(win_message));

		wmove   (gbl(win_commands), 1, 0);
		xaddstr (gbl(win_commands), msgs(m_diff_directory));

		*directory = 0;
		c = xgetstr(gbl(win_commands), directory, XGL_DEST, MAX_PATHLEN, 0,
			XG_PATHNAME);
		if (c <= 0)
			return;

		fn_resolve_pathname(directory);
		fn_get_abs_dir(gbl(scr_cur)->path_name, directory, report_path);
		fn_append_filename_to_dir(report_path, filename);
	}

	/*--------------------------------------------------------------------
	 * now open the print file
	 */
	if (report_to_file)
	{
		report_prt = prt_open(P_FILE, report_path, msgbuf);
	}
	else
	{
		report_prt = printer_open(opt(printer), msgbuf);
	}

	if (report_prt == 0)
	{
		errmsg(ER_COP, msgbuf, ERR_ANY);
		return;
	}

	/*--------------------------------------------------------------------
	 * now do it
	 */
	if (report_as_diff)
	{
		diff_print_diff(print_line, report_prt, lines);
	}
	else
	{
		diff_print_side(print_line, report_prt, lines, FALSE,
			opt(diff_page_width), opt(diff_show_numbers), opt(tab_width));
	}

	/*--------------------------------------------------------------------
	 * close the print file
	 */
	rc = prt_close(report_prt, msgbuf);
	if (rc)
	{
		errmsg(ER_COP, msgbuf, ERR_ANY);
	}
}
