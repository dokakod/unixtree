/*------------------------------------------------------------------------
 * misc version & credit routines
 */
#include "libprd.h"

char *package_name (void)
{
	return pgmi(m_pgm_package);
}

char *ver_line (char *line)
{
	strcpy(line, msgs(m_credits_ver));
	strcat(line, " ");
	strcat(line, pgmi(m_pgm_version));
	strcat(line, ".");
	strcat(line, pgmi(m_pgm_release));
	strcat(line, " ");
	strcat(line, msgs(m_credits_bld));
	strcat(line, " ");
	strcat(line, pgmi(m_pgm_build));

	return (line);
}

void headline (void)
{
	if (opt(display_headline) && ! gbl(scr_is_split))
	{
		int num_lines	= getmaxy(gbl(scr_cur)->cur_dir_win);
		int blk_ht		= BLK_HEIGHT;
		int left		= num_lines - ((2 * blk_ht) + 1);
		int i			= (left + 1) / 2;
		char line1[24];
		char line2[24];
		char *s;

		werase(gbl(scr_cur)->cur_dir_win);

		strcpy(line1, pgmi(m_pgm_package));

		for (s=line1+1; *s; s++)
		{
			if (isupper(*s))
				break;
		}

		strcpy(line2, s);
		*s = 0;

		wblk_str(gbl(scr_cur)->cur_dir_win, i,              -1, line1, FALSE);
		wblk_str(gbl(scr_cur)->cur_dir_win, i + blk_ht + 1, -1, line2, FALSE);

		wrefresh(gbl(scr_cur)->cur_dir_win);
	}
}

void disp_credits (void)
{
	char line[128];
	int y;

	y = (getmaxy(gbl(scr_cur)->cur_file_win)-2)/2 - 1;

	werase(gbl(scr_cur)->cur_file_win);
	wcenter(gbl(scr_cur)->cur_file_win, package_name(),         y);
	wcenter(gbl(scr_cur)->cur_file_win, ver_line(line),         y+1);
	wcenter(gbl(scr_cur)->cur_file_win, msgs(m_about_formerly), y+2);
	wcenter(gbl(scr_cur)->cur_file_win, pgmi(m_pgm_slogan),     y+3);
	wrefresh(gbl(scr_cur)->cur_file_win);
}

void disp_credits_below (void)
{
	char line[128];

	werase(gbl(win_commands));
	wcenter(gbl(win_commands), package_name(), 0);
	wcenter(gbl(win_commands), ver_line(line), 1);
	wrefresh(gbl(win_commands));

	werase(gbl(win_message));
	wcenter(gbl(win_message), pgmi(m_pgm_slogan), 0);
	wrefresh(gbl(win_message));

	position_cursor();
	wrefresh(stdscr);

	while (TRUE)
	{
		int c;

		c = xgetch(stdscr);
		if (c != KEY_MOUSE)
			break;

		if (mouse_get_event(gbl(win_commands)) == MOUSE_LBD)
			break;
	}

	disp_cmds();
}

void disp_run_stats (void)
{
	int i;
	int c;
	char *p;
	char malloc_msg[128];

	p =  SYSMEM_STATS(malloc_msg);
	bang("");
	werase(gbl(win_commands));
	i = strlen(p);
	wmove(gbl(win_commands), 1, (getmaxx(gbl(win_commands))-i)/2);
	waddstr(gbl(win_commands), p);
	wrefresh(gbl(win_commands));
	position_cursor();
	wrefresh(stdscr);

	while (TRUE)
	{
		c = xgetch(stdscr);
		if (c != KEY_MOUSE)
			break;

		if (mouse_get_event(gbl(win_commands)) == MOUSE_LBD)
			break;
	}

	bang("");
	disp_cmds();
}
