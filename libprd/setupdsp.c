/*------------------------------------------------------------------------
 * routines to setup the display
 */
#include "libprd.h"

void setup_display (void)
{
	int i;

	/* application program */

	if (gbl(scr_in_menu))
	{
		Menu_paint_menu();
	}

	/* config program */

	else if (gbl(scr_in_config))
	{
		cfg_redraw();
	}

	/* hexedit display */

	else if (gbl(scr_in_hexedit))
	{
		hexedit_setup_display();
	}

	/* fullview display */

	else if (gbl(scr_in_fullview))
	{
		win_clock_set(FALSE);
		fv_border();
		disp_path_line();
		view_command(gbl(scr_cur)->vfcb, V_CMD_REDISP_NR);
		disp_av_mode();
		touchwin(stdscr);
		wrefresh(stdscr);
		disp_cmds();
	}

	/*	autoview display */

	else if (gbl(scr_in_autoview))
	{
		win_clock_set(FALSE);
		if (gbl(scr_is_split))
			split_av_border();
		else
			av_border();
		for (i=0; i<gbl(scr_cur_count); i++)
		{
			gbl(scr_cur) = &gbl(scr_stat)[i];
			if (gbl(scr_cur)->command_mode == m_dir)
			{
				dptr_to_dirname(gbl(scr_cur)->dir_cursor,
					gbl(scr_cur)->path_name);
			}
			else
			{
				fblk_to_dirname(gbl(scr_cur)->cur_file,
					gbl(scr_cur)->path_name);
			}
			disp_path_line();
			file_scroll_bar_init();
			disp_file_list();
			do_file_scroll_bar();
			if (gbl(scr_cur)->numfiles)
			{
				if (i == gbl(scr_cur_no) && !gbl(scr_av_focus))
					hilite_file(ON);
				else
				{
					hilite_file(OFF);
					waddch(gbl(scr_cur)->cur_file_win,
						pgm_const(selected_char));
					wrefresh(gbl(scr_cur)->cur_file_win);
				}
				display_av();
				disp_av_mode();
			}
			else
			{
				werase(gbl(scr_cur)->cur_av_mode_win);
				wrefresh(gbl(scr_cur)->cur_av_mode_win);
				werase((gbl(scr_cur)->vfcb)->v_win);
				wrefresh((gbl(scr_cur)->vfcb)->v_win);
			}
		}
		gbl(scr_cur) = &gbl(scr_stat)[gbl(scr_cur_no)];
		disp_cmds();
	}

	/*	diffview */

	else if (gbl(scr_in_diffview))
	{
		diff_view_setup();
		disp_cmds();
	}

	/*	normal split screen */

	else if (gbl(scr_is_split))
	{
		disp_splits();
	}

	/*	normal screen */

	else
	{
		disp_path_line();
		if (opt(display_clock))
		{
			win_clock_set(opt(display_clock));
		}
		else
		{
			werase(gbl(win_clkdisp));
			wrefresh(gbl(win_clkdisp));
		}

		main_border();
		if (gbl(scr_cur)->command_mode == m_dir)
		{
			disp_dir_tree();
			dir_scroll_bar_init();
			do_dir_scroll_bar();
			hilite_dir(ON);
			small_border(ON);
			file_scroll_bar_init();
			check_small_window();
		}
		else
		{
			if (gbl(scr_cur)->in_small_window)
			{
				disp_dir_tree();
				dir_scroll_bar_init();
				do_dir_scroll_bar();
				small_border(ON);
				hilite_dir(OFF);
				left_arrow(gbl(scr_cur)->cur_dir_win);
				wrefresh(gbl(scr_cur)->cur_dir_win);
			}
			else
			{
				small_border(OFF);
			}
			disp_file_list();
			file_scroll_bar_init();
			do_file_scroll_bar();
			hilite_file(ON);
		}
		disp_file_box();
		disp_drive_info((gbl(scr_cur)->cur_dir)->stbuf.st_dev, 1);
		if (gbl(scr_cur)->command_mode == m_dir)
		{
			disp_node_stats(gbl(scr_cur)->cur_root);
			disp_cur_dir();
		}
		else
		{
			disp_dir_stats();
			disp_cur_file();
		}
	}
}
