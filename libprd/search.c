/*------------------------------------------------------------------------
 * process the "search" cmd
 */
#include "libprd.h"

static void case_opt (void *data, int c)
{
	if (c == cmds(CMDS_SEARCH_IGNORE_CASE))
	{
		int prompt_x = display_len(msgs(m_search_entstr)) + 2;

		opt(ignore_search_case) = ! opt(ignore_search_case);

		wmove(gbl(win_message), 0, prompt_x);
		fk_msg(gbl(win_message), CMDS_SEARCH_IGNORE_CASE,
			opt(ignore_search_case) ?
				msgs(m_search_obscase) : msgs(m_search_igncase));
		wrefresh(gbl(win_message));
	}
}

static int search_file_for_re (const char *pathname, REGEXP *rex)
{
	FILE *fp;
	char line[256];			/* max line in re logic */
	const char *rep;

	rep = 0;
	fp = fopen(pathname, "rb");
	if (fp)
	{
		while (fgets(line, sizeof(line), fp))
		{
			rep = ut_regexec(rex, line, 0);
			if (rep)
				break;
		}
		fclose(fp);
	}

	return (rep != 0);
}

static int search_file (void *data)
{
	int rc;
	char search_path[MAX_PATHLEN];
	char *p, *q;

	werase(gbl(win_commands));
	p = msgs(m_search_seafor);
	q = msgs(m_search_in);
	wmove(gbl(win_commands), 0, 0);
	waddstr(gbl(win_commands), p);
	zaddstr(gbl(win_commands), gbl(srch_str));
	wmove(gbl(win_commands), 1, display_len(p)-display_len(q));
	waddstr(gbl(win_commands), q);
	fblk_to_pathname(gbl(scr_cur)->cur_file, search_path);
	zaddstr(gbl(win_commands), search_path);
	wrefresh(gbl(win_commands));

	rc = search_file_for_re(search_path, gbl(srch_re));
	if (! rc)
	{
		untag_file(gbl(scr_cur)->cur_file);
		disp_dir_stats();
	}
	return (1);
}

void do_search (void)
{
	if (check_tag_count())
		return;

	if (gbl(srch_re))
		FREE(gbl(srch_re));
	gbl(srch_re) = get_search_str(msgs(m_search_satf));
	if (! gbl(srch_re))
		return;

	traverse(search_file, 0);
	disp_dir_stats();
	disp_cur_file();
}

REGEXP *get_search_str (const char *prompt)
{
	int c;
	REGEXP *rep;
	char input_str[MAX_PATHLEN];
	int rc;
	char *e;

	bang(msgs(m_search_entstr));
	waddstr(gbl(win_message), "  ");
	fk_msg(gbl(win_message), CMDS_SEARCH_IGNORE_CASE, opt(ignore_search_case) ?
		msgs(m_search_obscase) : msgs(m_search_igncase));
	wrefresh(gbl(win_message));

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), prompt);
	wrefresh(gbl(win_commands));
	strcpy(input_str, gbl(srch_str));
	c = xgetstr_cb(gbl(win_commands), input_str, XGL_SRCH_STR, MAX_PATHLEN, 0,
		XG_STRING, case_opt, 0);
	if (c <= 0)
		return (0);

	strcpy(gbl(srch_str), input_str);
	rep = ut_regcomp(input_str, opt(ignore_search_case), &rc);
	if (rep == 0)
	{
		e = msgs((int)m_regexp_00 + rc);
		errmsg(ER_IR, e, ERR_ANY);
	}

	return (rep);
}
