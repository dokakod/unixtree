/*------------------------------------------------------------------------
 * autoview routines
 */
#include "libprd.h"

static void init_av (void)
{
	int i;

	for (i=0; i<gbl(scr_cur_count); i++)
	{
		if (i != gbl(scr_cur_no))
			is_screen_kosher(i);

		gbl(scr_cur) = &gbl(scr_stat)[i];
		gbl(scr_cur)->save_cmd_mode     = gbl(scr_cur)->command_mode;
		gbl(scr_cur)->save_file_fmt     = gbl(scr_cur)->file_fmt;

		if (gbl(scr_cur)->command_mode == m_dir)
			gbl(scr_cur)->command_mode  = m_file;
		gbl(scr_cur)->cur_path_line     = gbl(scr_cur)->cur_av_path_line;
		gbl(scr_cur)->cur_file_win      = gbl(scr_cur)->cur_av_file_win;
		gbl(scr_cur)->cur_dest_dir_win  = gbl(scr_cur)->cur_av_dest_dir_win;
		gbl(scr_cur)->cur_av_mode_win   = gbl(scr_cur)->cur_av_mode_win;
		gbl(scr_cur)->file_fmt          = fmt_fname1;
		gbl(scr_cur)->max_file_col      = 0;
		gbl(scr_cur)->file_line_width   = getmaxx(gbl(scr_cur)->cur_file_win);
		gbl(scr_cur)->file_disp_len     = gbl(scr_cur)->file_line_width - 3;
		gbl(scr_cur)->max_file_line     =
			getmaxy(gbl(scr_cur)->cur_file_win)-1;
		gbl(scr_cur)->av_file_displayed = 0;
		if (gbl(scr_cur)->command_mode == m_dir ||
			gbl(scr_cur)->command_mode == m_file)
		{
			wattrset (gbl(scr_cur)->cur_file_win, gbl(scr_reg_hilite_attr));
			wstandset(gbl(scr_cur)->cur_file_win, gbl(scr_reg_cursor_attr));
		}
		else
		{
			wattrset (gbl(scr_cur)->cur_file_win, gbl(scr_sa_hilite_attr));
			wstandset(gbl(scr_cur)->cur_file_win, gbl(scr_sa_cursor_attr));
		}
		locate_cur_file();
		if (gbl(scr_cur)->file_cursor)
			open_av_file(gbl(scr_cur)->cur_file);
	}

	for (i=0; i<gbl(scr_cur_count); i++)
	{
		if (gbl(scr_stat)[gbl(scr_cur_no)].numfiles)
			break;
		gbl(scr_cur_no) = next_scr_no(gbl(scr_cur_no));
	}

	if (i < gbl(scr_cur_count))
	{
		gbl(scr_cur) = &gbl(scr_stat)[gbl(scr_cur_no)];
		gbl(scr_in_autoview) = TRUE;
		gbl(scr_av_focus) = FALSE;
	}
}

void end_av (void)
{
	int i;

	gbl(scr_in_autoview) = FALSE;
	for (i=0; i<gbl(scr_cur_count); i++)
	{
		gbl(scr_cur) = &gbl(scr_stat)[i];
		gbl(scr_cur)->command_mode = gbl(scr_cur)->save_cmd_mode;
		gbl(scr_cur)->cmd_sub_mode = m_reg;
		if (gbl(scr_is_split))
		{
			if (gbl(scr_cur)->command_mode == m_dir ||
				gbl(scr_cur)->in_small_window)
			{
				gbl(scr_cur)->cur_file_win = gbl(win_small_file_split)[i];
			}
			else
			{
				gbl(scr_cur)->cur_file_win = gbl(win_large_file_split)[i];
			}
			gbl(scr_cur)->cur_path_line = gbl(win_path_line_split)[i];
			gbl(scr_cur)->cur_dest_dir_win = gbl(win_dest_dir_split)[i];
		}
		else
		{
			if (gbl(scr_cur)->command_mode == m_dir ||
				gbl(scr_cur)->in_small_window)
			{
				gbl(scr_cur)->cur_file_win = (opt(wide_screen) ?
					gbl(win_small_file_wide): gbl(win_small_file_reg));
			}
			else
			{
				gbl(scr_cur)->cur_file_win = (opt(wide_screen) ?
					gbl(win_large_file_wide): gbl(win_large_file_reg));
			}
			gbl(scr_cur)->cur_path_line = gbl(win_path_line_reg);
			gbl(scr_cur)->cur_dest_dir_win = gbl(win_dest_dir_reg);
		}
		gbl(scr_cur)->max_file_line = getmaxy(gbl(scr_cur)->cur_file_win)-1;
		gbl(scr_cur)->file_fmt = gbl(scr_cur)->save_file_fmt;
		set_file_disp();		/* calculate gbl(scr_cur)->max_file_col */
		locate_cur_file();
		if (gbl(scr_cur)->av_file_displayed)
		{
			gbl(scr_cur)->av_file_displayed = 0;
			close_av_file();
		}
	}
	gbl(scr_cur) = &gbl(scr_stat)[gbl(scr_cur_no)];
}

void autoview (void)
{
	int i;

	/* check if any screens have a file list */

	for (i=0; i<gbl(scr_cur_count); i++)
	{
		if (gbl(scr_stat)[i].numfiles)
			break;
	}
	if (i == gbl(scr_cur_count))
		return;

	init_av();
	setup_display();
	disp_cmds();
}
