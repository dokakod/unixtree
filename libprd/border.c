/*------------------------------------------------------------------------
 * draw all window borders
 */
#include "libprd.h"

void main_border (void)
{
	int i;

	for (i=1; i<=getmaxy(stdscr)-4; i++)
	{
		wmove(gbl(win_border), i, 0);
		wclrtoeol(gbl(win_border));
	}

	box_around(gbl(scr_cur)->large_file_win, gbl(win_border),
		pgm_const(border_type));
	box_around(gbl(scr_cur)->small_file_win, gbl(win_border),
		pgm_const(border_type));

	if (! opt(wide_screen))
	{
		box_around(gbl(win_file_box),  gbl(win_border),
			pgm_const(border_type));
		box_around(gbl(win_drive_box), gbl(win_border),
			pgm_const(border_type));
		box_around(gbl(win_stats),     gbl(win_border),
			pgm_const(border_type));
		box_around(gbl(win_current),   gbl(win_border),
			pgm_const(border_type));
	}

	touchwin(stdscr);
	position_cursor();
	wrefresh(stdscr);
}

void split_main_border (void)
{
	int i, j;

	for (i=1; i<=getmaxy(stdscr)-4; i++)
	{
		wmove(gbl(win_border), i, 0);
		wclrtoeol(gbl(win_border));
	}
	for (j=0; j<gbl(scr_cur_count); j++)
	{
		box_around(gbl(win_large_file_split)[j], gbl(win_border),
			pgm_const(border_type));
	}

	touchwin(stdscr);
	position_cursor();
	wrefresh(stdscr);
}

void small_border (int on_off)
{
	int i;
	int start;
	int len;

	start = getbegx(gbl(scr_cur)->small_file_win)-1;
	len   = getmaxx(gbl(scr_cur)->small_file_win);
	wmove(gbl(win_border),
		getmaxy(stdscr)-4-getmaxy(gbl(win_small_file_reg))-1, start);
	if (on_off)
	{
		wbox_chr(gbl(win_border), B_ML, pgm_const(border_type));
		for (i=0; i<len; i++)
			wbox_chr(gbl(win_border), B_HO, pgm_const(border_type));
		if (getmaxy(gbl(win_current)) == getmaxy(gbl(win_small_file_reg)))
			wbox_chr(gbl(win_border), B_MC, pgm_const(border_type));
		else
			wbox_chr(gbl(win_border), B_MR, pgm_const(border_type));
	}
	else
	{
		wbox_chr(gbl(win_border), B_VE, pgm_const(border_type));
		for (i=0; i<len; i++)
			waddch(gbl(win_border), ' ');
		if (getmaxy(gbl(win_current)) == getmaxy(gbl(win_small_file_reg)))
			wbox_chr(gbl(win_border), B_ML, pgm_const(border_type));
		else
			wbox_chr(gbl(win_border), B_VE, pgm_const(border_type));
	}
	wrefresh(gbl(win_border));
}

void dest_border (void)
{
	int i;

	if (gbl(scr_in_autoview))
	{
		werase(gbl(scr_cur)->cur_av_view_win);
		wattrset(gbl(scr_cur)->cur_av_view_win, gbl(scr_border_attr));
		wmove(gbl(scr_cur)->cur_av_view_win, 0, 1);
		wbox_chr(gbl(scr_cur)->cur_av_view_win, B_TL, pgm_const(border_type));
		for (i=2; i<getmaxx(gbl(scr_cur)->cur_av_view_win)-2; i++)
		{
			wbox_chr(gbl(scr_cur)->cur_av_view_win, B_HO,
				pgm_const(border_type));
		}
		wbox_chr(gbl(scr_cur)->cur_av_view_win, B_TR, pgm_const(border_type));

		for (i=1; i<getmaxy(gbl(scr_cur)->cur_av_view_win)-1; i++)
		{
			wmove(gbl(scr_cur)->cur_av_view_win, i, 1);
			wbox_chr(gbl(scr_cur)->cur_av_view_win, B_VE,
				pgm_const(border_type));
			wmove(gbl(scr_cur)->cur_av_view_win, i,
				getmaxx(gbl(scr_cur)->cur_av_view_win)-2);
			wbox_chr(gbl(scr_cur)->cur_av_view_win, B_VE,
				pgm_const(border_type));
		}

		wmove(gbl(scr_cur)->cur_av_view_win,
			getmaxy(gbl(scr_cur)->cur_av_view_win)-1, 1);
		wbox_chr(gbl(scr_cur)->cur_av_view_win, B_BL, pgm_const(border_type));
		for (i=2; i<getmaxx(gbl(scr_cur)->cur_av_view_win)-2; i++)
		{
			wbox_chr(gbl(scr_cur)->cur_av_view_win, B_HO,
				pgm_const(border_type));
		}
		wbox_chr(gbl(scr_cur)->cur_av_view_win, B_BR, pgm_const(border_type));
		wrefresh(gbl(scr_cur)->cur_av_view_win);
		wattrset(gbl(scr_cur)->cur_av_view_win, gbl(scr_av_hilite_attr));
	}
	else
	{
		small_border(OFF);
		wattrset(gbl(scr_cur)->large_file_win, gbl(scr_reg_hilite_attr));
		werase(gbl(scr_cur)->large_file_win);
		wattrset(gbl(scr_cur)->large_file_win, gbl(scr_border_attr));

		wmove(gbl(scr_cur)->large_file_win, 0, 1);
		wbox_chr(gbl(scr_cur)->large_file_win, B_TL, pgm_const(border_type));
		for (i=2; i<getmaxx(gbl(scr_cur)->large_file_win)-2; i++)
		{
			wbox_chr(gbl(scr_cur)->large_file_win, B_HO,
				pgm_const(border_type));
		}
		wbox_chr(gbl(scr_cur)->large_file_win, B_TR, pgm_const(border_type));

		for (i=1; i<getmaxy(gbl(scr_cur)->large_file_win)-1; i++)
		{
			wmove(gbl(scr_cur)->large_file_win, i, 1);
			wbox_chr(gbl(scr_cur)->large_file_win, B_VE,
				pgm_const(border_type));
			wmove(gbl(scr_cur)->large_file_win, i,
				getmaxx(gbl(scr_cur)->large_file_win)-2);
			wbox_chr(gbl(scr_cur)->large_file_win, B_VE,
				pgm_const(border_type));
		}

		wmove(gbl(scr_cur)->large_file_win,
			getmaxy(gbl(scr_cur)->large_file_win)-1, 1);
		wbox_chr(gbl(scr_cur)->large_file_win, B_BL, pgm_const(border_type));
		for (i=2; i<getmaxx(gbl(scr_cur)->large_file_win)-2; i++)
		{
			wbox_chr(gbl(scr_cur)->large_file_win, B_HO,
				pgm_const(border_type));
		}
		wbox_chr(gbl(scr_cur)->large_file_win, B_BR, pgm_const(border_type));
		wrefresh(gbl(scr_cur)->large_file_win);
		if (gbl(scr_cur)->command_mode == m_dir ||
			gbl(scr_cur)->command_mode == m_file)
		{
			wattrset(gbl(scr_cur)->large_file_win, gbl(scr_reg_hilite_attr));
		}
		else
		{
			wattrset(gbl(scr_cur)->large_file_win, gbl(scr_sa_hilite_attr));
		}
	}
}

void fv_border (void)
{
	int i;

	/* top line */

	wmove(gbl(win_border), 1, 0);
	for (i=0; i<getmaxx(gbl(win_border)); i++)
		wbox_chr(gbl(win_border), B_HO, pgm_const(border_type));

	/* bottom line */

	wmove(gbl(win_border), getmaxy(stdscr)-4, 0);
	for (i=0; i<getmaxx(gbl(win_border)); i++)
		wbox_chr(gbl(win_border), B_HO, pgm_const(border_type));
}

void av_border (void)
{
	int i;

	for (i=1; i<=getmaxy(stdscr)-4; i++)
	{
		wmove(gbl(win_border), i, 0);
		wclrtoeol(gbl(win_border));
	}

	box_around(gbl(win_av_file_reg), gbl(win_border), pgm_const(border_type));
	box_around(gbl(win_av_view_reg), gbl(win_border), pgm_const(border_type));

	touchwin(stdscr);
	position_cursor();
	wrefresh(stdscr);
}

void split_av_border (void)
{
	int i, j;

	for (i=1; i<=getmaxy(stdscr)-4; i++)
	{
		wmove(gbl(win_border), i, 0);
		wclrtoeol(gbl(win_border));
	}

	for (j=0; j<gbl(scr_cur_count); j++)
	{
		box_around(gbl(win_av_file_split)[j], gbl(win_border),
			pgm_const(border_type));
		box_around(gbl(win_av_view_split)[j], gbl(win_border),
			pgm_const(border_type));
	}

	touchwin(stdscr);
	position_cursor();
	wrefresh(stdscr);
}

void config_border (void)
{
	int i;

	wmove(gbl(win_border), 0, 0);
	wbox_chr(gbl(win_border), B_TL, pgm_const(border_type));
	for (i=1; i<getmaxx(gbl(win_border))-1; i++)
		wbox_chr(gbl(win_border), B_HO, pgm_const(border_type));
	wbox_chr(gbl(win_border), B_TR, pgm_const(border_type));

	/* sides of main box */

	for (i=1; i<getmaxy(gbl(win_config))-1; i++)
	{
		wmove(gbl(win_border), i, 0);
		wbox_chr(gbl(win_border), B_VE, pgm_const(border_type));
		wmove(gbl(win_border), i, getmaxx(gbl(win_border))-1);
		wbox_chr(gbl(win_border), B_VE, pgm_const(border_type));
	}

	wmove(gbl(win_border), 2, 0);
	wbox_chr(gbl(win_border), B_ML, pgm_const(border_type));
	for (i=1; i<getmaxx(gbl(win_border))-1; i++)
		wbox_chr(gbl(win_border), B_HO, pgm_const(border_type));
	wbox_chr(gbl(win_border), B_MR, pgm_const(border_type));

	wmove(gbl(win_border), getmaxy(gbl(win_config))-3, 0);
	wbox_chr(gbl(win_border), B_ML, pgm_const(border_type));
	for (i=1; i<getmaxx(gbl(win_border))-1; i++)
		wbox_chr(gbl(win_border), B_HO, pgm_const(border_type));
	wbox_chr(gbl(win_border), B_MR, pgm_const(border_type));

	wmove(gbl(win_border), getmaxy(gbl(win_config))-1, 0);
	wbox_chr(gbl(win_border), B_BL, pgm_const(border_type));
	for (i=1; i<getmaxx(gbl(win_border))-1; i++)
		wbox_chr(gbl(win_border), B_HO, pgm_const(border_type));
	wbox_chr(gbl(win_border), B_BR, pgm_const(border_type));

	touchwin(stdscr);
	position_cursor();
	wrefresh(stdscr);
}

void diff_border (void)
{
	int i;

	for (i=1; i<=getmaxy(stdscr)-4; i++)
	{
		wmove(gbl(win_border), i, 0);
		wclrtoeol(gbl(win_border));
	}

	box_around(gbl(win_diff_lf), gbl(win_border), pgm_const(border_type));
	box_around(gbl(win_diff_rt), gbl(win_border), pgm_const(border_type));

	touchwin(stdscr);
	position_cursor();
	wrefresh(stdscr);
}

