/*------------------------------------------------------------------------
 * setup terminal display & windows
 */
#include "libprd.h"
#include <signal.h>
#include "pgm.xbm"

#define BOX_WIDTH	20

int win_cols (const void *valp)
{
	int cols = *(const int *)valp;

	if (cols < MIN_WIN_COLS)
	{
		err_message(msgs(m_main_mincols), "", ERR_ANY);
		return (-1);
	}

	return (0);
}

int win_rows (const void *valp)
{
	int rows = *(const int *)valp;

	if (rows < MIN_WIN_ROWS)
	{
		err_message(msgs(m_main_minrows), "", ERR_ANY);
		return (-1);
	}

	return (0);
}

int win_opts (const void *valp)
{
	const char * opts = (const char *)valp;
	XVT_DATA *	xd = gbl(xvt_data);
	int rc;

	rc = xvt_tk_optlist(xd, opts);
	if (rc)
	{
		err_message(xvt_data_get_error_msg(xd), "", ERR_ANY);
		return (-1);
	}

	return (0);
}

int win_arg (const char *argstr)
{
	int	rc;

	rc = xvt_tk_optlist(gbl(xvt_data), argstr);

	return (rc);
}

void clone_term (void)
{
	XVT_DATA *	xd = gbl(xvt_data);
	int			rc;

	/*--------------------------------------------------------------------
	 * create clone of our program
	 */
	rc = xvt_here_clone(xd);
	if (rc)
	{
		err_message(xvt_data_get_error_msg(xd), "", ERR_ANY);
	}

	/*--------------------------------------------------------------------
	 * If successful here, we have two returns: the parent & the child.
	 */
}

static int win_run (void)
{
	XVT_DATA *	xd = gbl(xvt_data);
	int			rc;

	sprintf(gbl(pgm_label), package_name());
	strcat (gbl(pgm_label), msgs(m_term_label));

	xvt_tk_resource(xd);

	if (xvt_data_get_scr_cols(xd) < 0)
		xvt_data_set_scr_cols(xd, opt(columns));

	if (xvt_data_get_scr_rows(xd) < 0)
		xvt_data_set_scr_rows(xd, opt(rows));

	if (xvt_data_get_reg_fname(xd, 0) == 0)
		xvt_data_set_reg_fname(xd, opt(norm_font), 0);

	if (xvt_data_get_bld_fname(xd, 0) == 0)
		xvt_data_set_bld_fname(xd, opt(bold_font), 0);

	if (xvt_data_get_win_label(xd) == 0)
		xvt_data_set_win_label(xd, gbl(pgm_label));

	rc = xvt_here_run(xd, TRUE);
	if (rc != XVT_OK)
	{
		rc = xvt_data_get_error_quit(xd);

		if (rc)
			return (-1);
	}
	else
	{
		gbl(pgm_term) = xvt_data_get_term_type(xd);
	}

	return (0);
}

static void sig_int (int sig, void *data)
{
	int y, x;

	y = getcury(stdscr);
	x = getcurx(stdscr);
	if (errmsg(ER_INT, "", ERR_YESNO) == 0)
		all_done();
	wmove(stdscr, y, x);
	wrefresh(stdscr);
}

static void sig_pipe (int sig, void *data)
{
	errsys(ER_COP);
}

static void sig_term (int sig, void *data)
{
	all_done();
}

static int kbd_press_release (int key, void *data)
{
	switch (key)
	{
	case KEY_SHIFT_PRESS:
		gbl(scr_kbd_shift)	= opt(shift_for_tag);
		return (key);

	case KEY_SHIFT_RELEASE:
		gbl(scr_kbd_shift)	= FALSE;
		return (key);

	case KEY_CTRL_PRESS:
	case KEY_CTRL_RELEASE:
		return (0);

	case KEY_ALT_PRESS:
		gbl(scr_kbd_alt)	= TRUE;
		return (key);

	case KEY_ALT_RELEASE:
		gbl(scr_kbd_alt)	= FALSE;
		return (key);
	}

	return (0);
}

static int trm_did_change (int key, void *data)
{
	gbl(scr_size_changed) = TRUE;
	return (0);
}

static int set_alt_chars_ok (int flag)
{
	set_valid_acs(flag);

	return (0);
}

static void create_windows (void)
{
	int i;
	int j;
	int k;
	int lines;
	int cols;
	int split_window_width;
	int main_box_wd;
	int main_box_ht;
	int num_lines_in_sm_win;

	lines = getmaxy(stdscr);
	cols  = getmaxx(stdscr);

	keypad(stdscr, TRUE);
	leaveok(stdscr, TRUE);
	split_window_width = cols / gbl(scr_num_split_wins);
	main_box_ht = lines - 4;
	main_box_wd = cols - BOX_WIDTH - 3;
	num_lines_in_sm_win = (main_box_ht - 3) / 4;	/* 1/4 for small window */

	gbl(win_border)		=
		derwin(stdscr, 0, 0, 0, 0);

	gbl(win_file_box)	=
		derwin(stdscr, 1, BOX_WIDTH, 2, cols - BOX_WIDTH - 1);

	gbl(win_drive_box)	=
		derwin(stdscr, 2, BOX_WIDTH, 4, cols - BOX_WIDTH - 1);

	gbl(win_stats)		=
		derwin(stdscr, 10, BOX_WIDTH, 7, cols - BOX_WIDTH - 1);

	gbl(win_current)	=
		derwin(stdscr, main_box_ht - 18, BOX_WIDTH, 18, cols - BOX_WIDTH - 1);

	gbl(win_av_path_line_reg) =
		derwin(stdscr, 1, cols - 20, 0, 0);

	gbl(win_av_mode_reg)	=
		derwin(stdscr, 1, 20, 0, cols - 20);

	gbl(win_av_file_reg)	=
		derwin(stdscr, main_box_ht - 2, OUR_MAXFN + 2, 2, 1);

	gbl(win_av_view_reg)	=
		derwin(stdscr, main_box_ht - 2, cols-OUR_MAXFN-5, 2, OUR_MAXFN + 4);

	gbl(win_av_dest_dir_reg) =
		derwin(stdscr, main_box_ht - 4, cols-OUR_MAXFN-9, 3, OUR_MAXFN + 6);

	gbl(win_full_view)	=
		derwin(stdscr, main_box_ht - 2, cols, 2, 0);

	gbl(win_clkdisp)	=
		derwin(stdscr, 1, 21, 0, cols - 21);

	gbl(win_commands)	=
		derwin(stdscr, 2, cols, lines - 3, 0);

	gbl(win_message)	=
		derwin(stdscr, 1, cols, lines - 1, 0);

	gbl(win_error)		=
		newwin(1, cols, lines - 1, 0);

	gbl(win_config)		=
		derwin(stdscr, lines - 1, cols, 0, 0);

	gbl(win_path_line_reg)	=
		derwin(stdscr, 1, cols - getmaxx(gbl(win_clkdisp)), 0, 0);

	gbl(win_dir_reg)	=
		derwin(stdscr, main_box_ht-num_lines_in_sm_win-3, main_box_wd, 2, 1);

	gbl(win_dir_wide)	=
		derwin(stdscr, main_box_ht-num_lines_in_sm_win-3, cols-2, 2, 1);

	gbl(win_dest_dir_reg)	=
		derwin(stdscr, main_box_ht-4, main_box_wd-4, 3, 3);

	gbl(win_dest_dir_wide)	=
		derwin(stdscr, main_box_ht-4, cols-6, 3, 3);

	gbl(win_large_file_reg)	=
		derwin(stdscr, main_box_ht-2, main_box_wd, 2, 1);

	gbl(win_large_file_wide)	=
		derwin(stdscr, main_box_ht-2, cols-2, 2, 1);

	gbl(win_small_file_reg)	=
		derwin(stdscr, num_lines_in_sm_win,
			main_box_wd, main_box_ht-num_lines_in_sm_win, 1);

	gbl(win_small_file_wide)	=
		derwin(stdscr, num_lines_in_sm_win, cols-2,
			main_box_ht-num_lines_in_sm_win, 1);

	gbl(win_diff_lf)	=
		derwin(stdscr, main_box_ht-2, split_window_width-3, 2, 1);

	gbl(win_diff_rt)	=
		derwin(stdscr, main_box_ht-2, split_window_width-3,
			2, split_window_width+2);

	gbl(win_diff_tp)	=
		derwin(stdscr, main_box_ht-2, 2, 2, split_window_width-1);

	k = 0;
	for (i=0; i<gbl(scr_num_split_wins); i++)
	{
		gbl(win_path_line_split)[i]	=
			derwin(stdscr, 1, split_window_width, 0, i*split_window_width);

		gbl(win_dir_split)[i]	=
			derwin(stdscr, main_box_ht-num_lines_in_sm_win-3,
				split_window_width-2, 2, i*split_window_width+1);

		gbl(win_dest_dir_split)[i]	=
			derwin(stdscr, main_box_ht-4, 34, 3, i*split_window_width+3);

		gbl(win_large_file_split)[i]	=
			derwin(stdscr, main_box_ht-2, split_window_width-2,
				2, i*split_window_width+1);

		gbl(win_small_file_split)[i]	=
			derwin(stdscr, num_lines_in_sm_win, split_window_width-2,
				main_box_ht-num_lines_in_sm_win, i*split_window_width+1);

		j =  (main_box_ht+1) / gbl(scr_num_split_wins) +
			((main_box_ht+1) % gbl(scr_num_split_wins) > i);

		gbl(win_av_path_line_split)[i]   =
			derwin(stdscr, 1, cols-20, k, 0);

		gbl(win_av_mode_split)[i]    =
			derwin(stdscr, 1, 20, k, cols-20);

		gbl(win_av_file_split)[i]    =
			derwin(stdscr, j-3, OUR_MAXFN+2, k+2, 1);

		gbl(win_av_view_split)[i]    =
			derwin(stdscr, j-3, cols-OUR_MAXFN-5, k+2, OUR_MAXFN+4);

		gbl(win_av_dest_dir_split)[i] =
			derwin(stdscr, j-5, cols-OUR_MAXFN-9, k+3, OUR_MAXFN+6);

		k += j;
	}

	gbl(win_menu_reg) =
		derwin(stdscr, lines-1, cols, 0, 0);

	gbl(win_menu_sub) =
		derwin(gbl(win_menu_reg),
			getmaxy(gbl(win_menu_reg))-5, getmaxx(gbl(win_menu_reg))-6, 2, 3);

	/*
	 *	The following windows must reposition the cursor after refresh
	 */

	leaveok(stdscr, FALSE);
	leaveok(gbl(win_commands), FALSE);
	leaveok(gbl(win_message), FALSE);
	leaveok(gbl(win_error), FALSE);
	leaveok(gbl(win_config), FALSE);

	setup_window_attributes();
}

int init_wins (void)
{
	XVT_DATA *		xd = gbl(xvt_data);
	SCREEN *		trm_screen;
	const char *	p;
	const char **	defs;
	int				i;

	/*--------------------------------------------------------------------
	 * initialize all variables
	 */
	gbl(scr_size_changed)	= FALSE;
	gbl(scr_inp_fd)			= -1;
	gbl(scr_out_fd)			= -1;
	gbl(scr_is_color)		= FALSE;
	gbl(scr_in_window)		= get_window_env();

	gbl(scr_num_split_wins)	= NUM_SPLIT_WINS;

	if (opt(run_in_window))
	{
		if (win_run())
			return (-1);
	}
	else
	{
		/*----------------------------------------------------------------
		 * open input stream if requested
		 */
		p = cmdopt(trm_inp);
		if (p != 0 && *p != 0)
		{
			gbl(scr_inp_fd) = os_open(p, O_RDWR, 0666);
			if (gbl(scr_inp_fd) == -1)
			{
			}
		}

		/*----------------------------------------------------------------
		 * open output stream if requested
		 */
		p = cmdopt(trm_out);
		if (p != 0 && *p != 0)
		{
			gbl(scr_out_fd) = os_open(p, O_RDWR, 0666);
			if (gbl(scr_out_fd) == -1)
			{
			}
		}
	}

	/*--------------------------------------------------------------------
	 * create SCREEN & check its size
	 */
	defs = xvt_data_get_term_defs(xd);

	set_term_path(gbl(pgm_path));
	trm_screen = newscreen_defs(gbl(pgm_term), defs,
		gbl(scr_out_fd), gbl(scr_inp_fd));
	if (trm_screen == 0)
	{
		fprintf(stderr, "%s: %s\n", gbl(pgm_name), msgs(m_main_cnit));
		return (-1);
	}

	if (get_max_rows() < MIN_WIN_ROWS)
	{
		delscreen(trm_screen);
		fprintf(stderr, "%s: %s\n", gbl(pgm_name), msgs(m_main_minrows));
		return (-1);
	}

	if (get_max_cols() < MIN_WIN_COLS)
	{
		delscreen(trm_screen);
		fprintf(stderr, "%s: %s\n", gbl(pgm_name), msgs(m_main_mincols));
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * register any event routine we might have
	 */
	set_event_rtn(xvt_data_get_event_rtn(xd), xvt_data_get_event_data(xd));

	/*--------------------------------------------------------------------
	 * start keyboard/screen debugging if requested
	 */
	if (cmdopt(debug_keys))
		set_debug_kbd(0, TRUE, cmdopt(debug_keys_text));

	if (cmdopt(debug_scrn))
		set_debug_scr(0, TRUE, cmdopt(debug_scrn_text));

	/*--------------------------------------------------------------------
	 * set icon
	 */
	set_icon(pgm_icon_width, pgm_icon_height, pgm_icon_bits);

	/*--------------------------------------------------------------------
	 * setup screen environment
	 *
	 * Note: these calls are superfluous, since this is the default mode
	 */
	cbreak();
	noecho();
	nonl();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

	/*--------------------------------------------------------------------
	 * set alt char flag
	 */
	set_alt_chars_ok(opt(use_alt_char_sets));

	/*--------------------------------------------------------------------
	 * display title in window (if we can)
	 */
	set_title(package_name());

	/*--------------------------------------------------------------------
	 * setup signal handlers
	 */
	set_signal_rtn(SIGINT,  sig_int,  0);
	set_signal_rtn(SIGTERM, sig_term, 0);
#ifdef SIGPIPE
	set_signal_rtn(SIGPIPE, sig_pipe, 0);
#endif

	/*--------------------------------------------------------------------
	 * now create all our sub-windows
	 */
	create_windows();

	gbl(vfcb_av) = view_init(gbl(win_av_view_reg));
	gbl(vfcb_av)->v_mode |= V_VSCROLLBAR;

	for (i=0; i<gbl(scr_num_split_wins); i++)
	{
		gbl(vfcb_av_split)[i] = view_init(gbl(win_av_view_split)[i]);
		gbl(vfcb_av_split)[i]->v_mode |= V_VSCROLLBAR;
	}

	gbl(vfcb_fv) = view_init(gbl(win_full_view));
	gbl(vfcb_fv)->v_mode |= V_HSCROLLBAR;

	gbl(hxcb) = hexedit_init(gbl(win_full_view));

	set_async_key(KEY_F1,				do_help, 0);
	set_async_key(KEY_HELP,				do_about, 0);
	set_async_key(KEY_WINRESIZE,		trm_did_change, 0);

	set_async_key(KEY_SHIFT_PRESS,		kbd_press_release, 0);
	set_async_key(KEY_SHIFT_RELEASE,	kbd_press_release, 0);
	set_async_key(KEY_CTRL_PRESS,		kbd_press_release, 0);
	set_async_key(KEY_CTRL_RELEASE,		kbd_press_release, 0);
	set_async_key(KEY_ALT_PRESS,		kbd_press_release, 0);
	set_async_key(KEY_ALT_RELEASE,		kbd_press_release, 0);

	return (0);
}

void end_wins (void)
{
	XVT_DATA *	xd = gbl(xvt_data);

	/*--------------------------------------------------------------------
	 * end signal handling
	 */
	set_signal_rtn(SIGINT,  0, 0);
	set_signal_rtn(SIGTERM, 0, 0);
#ifdef SIGPIPE
	set_signal_rtn(SIGPIPE, 0, 0);
#endif

	/*--------------------------------------------------------------------
	 * end curses env
	 */
	endscr(TRUE);

	/*--------------------------------------------------------------------
	 * close event loop if running
	 */
	xvt_here_close(xd);

	/*--------------------------------------------------------------------
	 * close input stream if opened
	 */
	if (gbl(scr_inp_fd) != -1)
	{
		os_close(gbl(scr_inp_fd));
		gbl(scr_inp_fd) = -1;
	}

	/*--------------------------------------------------------------------
	 * close output stream if opened
	 */
	if (gbl(scr_out_fd) != -1)
	{
		os_close(gbl(scr_out_fd));
		gbl(scr_out_fd) = -1;
	}
}

void free_wins (void)
{
	/*--------------------------------------------------------------------
	 * Free entities from curses library
	 */
	end_curses();
}

int trm_use_mouse (const void *valp)
{
	int bf = *(const int *)valp;

	if (bf)
		mouse_show(stdscr);
	else
		mouse_hide(stdscr);

	return (0);
}

int trm_alt_chars_ok (const void *valp)
{
	int bf = *(const int *)valp;

	set_alt_chars_ok(bf);
	cfg_screen(0);

	return (0);
}

int trm_is_changed (int bf)
{
	int changed = gbl(scr_size_changed);

	if (bf)
		gbl(scr_size_changed) = FALSE;

	return (changed);
}

void setup_window_attributes (void)
{
	int i;

	/*--------------------------------------------------------------------
	 * get whether in color or mono
	 */
	switch (opt(screen_type))
	{
	case screen_auto:
		gbl(scr_is_color) = has_colors();
		break;

	case screen_color:
		gbl(scr_is_color) = TRUE;
		break;

	case screen_mono:
		gbl(scr_is_color) = FALSE;
		break;
	}

	/*--------------------------------------------------------------------
	 * set attrs to either the mono or color values
	 */
	if (gbl(scr_is_color))
	{
		gbl(scr_reg_hilite_attr)		= opt(reg_hilite_color);
		gbl(scr_reg_lolite_attr)		= opt(reg_lolite_color);
		gbl(scr_reg_cursor_attr)		= opt(reg_cursor_color);
		gbl(scr_reg_menu_attr)			= opt(reg_menu_color);

		gbl(scr_sa_hilite_attr)			= opt(sa_hilite_color);
		gbl(scr_sa_lolite_attr)			= opt(sa_lolite_color);
		gbl(scr_sa_cursor_attr)			= opt(sa_cursor_color);
		gbl(scr_sa_menu_attr)			= opt(sa_menu_color);

		gbl(scr_av_hilite_attr)			= opt(av_hilite_color);
		gbl(scr_av_lolite_attr)			= opt(av_lolite_color);
		gbl(scr_av_cursor_attr)			= opt(av_cursor_color);
		gbl(scr_av_menu_attr)			= opt(av_menu_color);

		gbl(scr_diff_same_attr)			= opt(diff_same_color);
		gbl(scr_diff_diff_attr)			= opt(diff_diff_color);
		gbl(scr_diff_lfrt_attr)			= opt(diff_lfrt_color);
		gbl(scr_diff_numb_attr)			= opt(diff_numb_color);

		gbl(scr_help_box_lolite_attr)	= opt(help_box_lolite_color);
		gbl(scr_help_box_hilite_attr)	= opt(help_box_hilite_color);
		gbl(scr_help_box_bold_attr)		= opt(help_box_bold_color);
		gbl(scr_help_box_hlink_attr)	= opt(help_box_hlink_color);
		gbl(scr_help_box_glink_attr)	= opt(help_box_glink_color);
		gbl(scr_gloss_box_attr)			= opt(gloss_box_color);

		gbl(scr_menu_lolite_attr)		= opt(menu_lolite_color);
		gbl(scr_menu_hilite_attr)		= opt(menu_hilite_color);
		gbl(scr_menu_cursor_attr)		= opt(menu_cursor_color);
		gbl(scr_menu_border_attr)		= opt(menu_border_color);

		gbl(scr_border_attr)			= opt(border_color);
		gbl(scr_clock_attr)				= opt(clock_color);
		gbl(scr_error_attr)				= opt(error_color);
		gbl(scr_mouse_attr)				= opt(mouse_color);
		gbl(scr_mouse_cmd_attr)			= opt(mouse_cmd_color);
		gbl(scr_escape_attr)			= opt(escape_color);
		gbl(scr_exit_attr)				= opt(exit_color);
	}
	else
	{
		gbl(scr_reg_hilite_attr)		= opt(reg_hilite_mono);
		gbl(scr_reg_lolite_attr)		= opt(reg_lolite_mono);
		gbl(scr_reg_cursor_attr)		= opt(reg_cursor_mono);
		gbl(scr_reg_menu_attr)			= opt(reg_menu_mono);

		gbl(scr_sa_hilite_attr)			= opt(sa_hilite_mono);
		gbl(scr_sa_lolite_attr)			= opt(sa_lolite_mono);
		gbl(scr_sa_cursor_attr)			= opt(sa_cursor_mono);
		gbl(scr_sa_menu_attr)			= opt(sa_menu_mono);

		gbl(scr_av_hilite_attr)			= opt(av_hilite_mono);
		gbl(scr_av_lolite_attr)			= opt(av_lolite_mono);
		gbl(scr_av_cursor_attr)			= opt(av_cursor_mono);
		gbl(scr_av_menu_attr)			= opt(av_menu_mono);

		gbl(scr_diff_same_attr)			= opt(diff_same_mono);
		gbl(scr_diff_diff_attr)			= opt(diff_diff_mono);
		gbl(scr_diff_lfrt_attr)			= opt(diff_lfrt_mono);
		gbl(scr_diff_numb_attr)			= opt(diff_numb_mono);

		gbl(scr_help_box_lolite_attr)	= opt(help_box_lolite_mono);
		gbl(scr_help_box_hilite_attr)	= opt(help_box_hilite_mono);
		gbl(scr_help_box_bold_attr)		= opt(help_box_bold_mono);
		gbl(scr_help_box_hlink_attr)	= opt(help_box_hlink_mono);
		gbl(scr_help_box_glink_attr)	= opt(help_box_glink_mono);
		gbl(scr_gloss_box_attr)			= opt(gloss_box_mono);

		gbl(scr_menu_lolite_attr)		= opt(menu_lolite_mono);
		gbl(scr_menu_hilite_attr)		= opt(menu_hilite_mono);
		gbl(scr_menu_cursor_attr)		= opt(menu_cursor_mono);
		gbl(scr_menu_border_attr)		= opt(menu_border_mono);

		gbl(scr_border_attr)			= opt(border_mono);
		gbl(scr_clock_attr)				= opt(clock_mono);
		gbl(scr_error_attr)				= opt(error_mono);
		gbl(scr_mouse_attr)				= opt(mouse_mono);
		gbl(scr_mouse_cmd_attr)			= opt(mouse_cmd_mono);
		gbl(scr_escape_attr)			= opt(escape_mono);
		gbl(scr_exit_attr)				= opt(exit_mono);
	}

	/*--------------------------------------------------------------------
	 * setup attributes for windows
	 */
	wattrset (stdscr, gbl(scr_reg_lolite_attr));
	wstandset(stdscr, gbl(scr_reg_hilite_attr));

	wattrset (gbl(win_config), gbl(scr_reg_lolite_attr));
	wstandset(gbl(win_config), gbl(scr_reg_hilite_attr));

	wattrset (gbl(win_border), gbl(scr_border_attr));
	wattrset(gbl(win_clkdisp), gbl(scr_clock_attr));

	wattrset (gbl(win_path_line_reg), gbl(scr_reg_lolite_attr));
	wstandset(gbl(win_path_line_reg), gbl(scr_reg_hilite_attr));

	wattrset (gbl(win_commands), gbl(scr_reg_lolite_attr));
	wstandset(gbl(win_commands), gbl(scr_reg_hilite_attr));

	wattrset (gbl(win_message), gbl(scr_reg_lolite_attr));
	wstandset(gbl(win_message), gbl(scr_reg_hilite_attr));

	wattrset (gbl(win_error), gbl(scr_error_attr));
	wstandset(gbl(win_error), gbl(scr_error_attr));

	wattrset (gbl(win_file_box), gbl(scr_reg_lolite_attr));
	wstandset(gbl(win_file_box), gbl(scr_reg_hilite_attr));

	wattrset (gbl(win_drive_box), gbl(scr_reg_lolite_attr));
	wstandset(gbl(win_drive_box), gbl(scr_reg_hilite_attr));

	wattrset (gbl(win_stats), gbl(scr_reg_lolite_attr));
	wstandset(gbl(win_stats), gbl(scr_reg_hilite_attr));

	wattrset (gbl(win_current), gbl(scr_reg_lolite_attr));
	wstandset(gbl(win_current), gbl(scr_reg_hilite_attr));

	wattrset (gbl(win_av_path_line_reg), gbl(scr_av_hilite_attr));
	wstandset(gbl(win_av_path_line_reg), gbl(scr_av_cursor_attr));

	wattrset (gbl(win_av_mode_reg), gbl(scr_av_lolite_attr));
	wstandset(gbl(win_av_mode_reg), gbl(scr_av_hilite_attr));

	wattrset (gbl(win_av_file_reg), gbl(scr_reg_hilite_attr));
	wstandset(gbl(win_av_file_reg), gbl(scr_reg_cursor_attr));

	wattrset (gbl(win_av_view_reg), gbl(scr_av_hilite_attr));
	wstandset(gbl(win_av_view_reg), gbl(scr_av_cursor_attr));

	wattrset (gbl(win_av_dest_dir_reg), gbl(scr_reg_hilite_attr));
	wstandset(gbl(win_av_dest_dir_reg), gbl(scr_reg_cursor_attr));

	wattrset (gbl(win_full_view), gbl(scr_av_hilite_attr));
	wstandset(gbl(win_full_view), gbl(scr_av_cursor_attr));

	wattrset (gbl(win_dir_reg), gbl(scr_reg_hilite_attr));
	wstandset(gbl(win_dir_reg), gbl(scr_reg_cursor_attr));

	wattrset (gbl(win_dir_wide), gbl(scr_reg_hilite_attr));
	wstandset(gbl(win_dir_wide), gbl(scr_reg_cursor_attr));

	wattrset (gbl(win_dest_dir_reg), gbl(scr_reg_hilite_attr));
	wstandset(gbl(win_dest_dir_reg), gbl(scr_reg_cursor_attr));

	wattrset (gbl(win_dest_dir_wide), gbl(scr_reg_hilite_attr));
	wstandset(gbl(win_dest_dir_wide), gbl(scr_reg_cursor_attr));

	wattrset (gbl(win_small_file_reg), gbl(scr_reg_hilite_attr));
	wstandset(gbl(win_small_file_reg), gbl(scr_reg_cursor_attr));

	wattrset (gbl(win_small_file_wide), gbl(scr_reg_hilite_attr));
	wstandset(gbl(win_small_file_wide), gbl(scr_reg_cursor_attr));

	wattrset (gbl(win_large_file_reg), gbl(scr_reg_hilite_attr));
	wstandset(gbl(win_large_file_reg), gbl(scr_reg_cursor_attr));

	wattrset (gbl(win_large_file_wide), gbl(scr_reg_hilite_attr));
	wstandset(gbl(win_large_file_wide), gbl(scr_reg_cursor_attr));

	wattrset (gbl(win_diff_lf), gbl(scr_diff_same_attr));
	wstandset(gbl(win_diff_lf), gbl(scr_diff_diff_attr));

	wattrset (gbl(win_diff_rt), gbl(scr_diff_same_attr));
	wstandset(gbl(win_diff_rt), gbl(scr_diff_diff_attr));

	wattrset (gbl(win_diff_tp), gbl(scr_diff_same_attr));
	wstandset(gbl(win_diff_tp), gbl(scr_diff_diff_attr));

	for (i=0; i<gbl(scr_num_split_wins); i++)
	{
		wattrset (gbl(win_dir_split)[i], gbl(scr_reg_hilite_attr));
		wstandset(gbl(win_dir_split)[i], gbl(scr_reg_cursor_attr));

		wattrset (gbl(win_dest_dir_split)[i], gbl(scr_reg_hilite_attr));
		wstandset(gbl(win_dest_dir_split)[i], gbl(scr_reg_cursor_attr));

		wattrset (gbl(win_small_file_split)[i], gbl(scr_reg_hilite_attr));
		wstandset(gbl(win_small_file_split)[i], gbl(scr_reg_cursor_attr));

		wattrset (gbl(win_large_file_split)[i], gbl(scr_reg_hilite_attr));
		wstandset(gbl(win_large_file_split)[i], gbl(scr_reg_cursor_attr));

		wattrset (gbl(win_av_path_line_split)[i], gbl(scr_av_hilite_attr));
		wstandset(gbl(win_av_path_line_split)[i], gbl(scr_av_cursor_attr));

		wattrset (gbl(win_av_mode_split)[i], gbl(scr_av_lolite_attr));
		wstandset(gbl(win_av_mode_split)[i], gbl(scr_av_hilite_attr));

		wattrset (gbl(win_av_file_split)[i], gbl(scr_reg_hilite_attr));
		wstandset(gbl(win_av_file_split)[i], gbl(scr_reg_cursor_attr));

		wattrset (gbl(win_av_view_split)[i], gbl(scr_av_hilite_attr));
		wstandset(gbl(win_av_view_split)[i], gbl(scr_av_cursor_attr));

		wattrset (gbl(win_av_dest_dir_split)[i], gbl(scr_reg_hilite_attr));
		wstandset(gbl(win_av_dest_dir_split)[i], gbl(scr_reg_cursor_attr));
	}

	wattrset (gbl(win_menu_reg), gbl(scr_menu_lolite_attr));
	wstandset(gbl(win_menu_reg), gbl(scr_menu_hilite_attr));

	wattrset (gbl(win_menu_sub), gbl(scr_menu_lolite_attr));
	wstandset(gbl(win_menu_sub), gbl(scr_menu_hilite_attr));

	mouse_setup(stdscr, gbl(scr_mouse_attr), gbl(scr_mouse_cmd_attr));
	if (opt(use_mouse))
		mouse_show(stdscr);
	else
		mouse_hide(stdscr);
}

void delete_windows (void)
{
	int i;

	delwin(gbl(win_border));
	delwin(gbl(win_file_box));
	delwin(gbl(win_drive_box));
	delwin(gbl(win_stats));
	delwin(gbl(win_current));

	delwin(gbl(win_av_path_line_reg));
	delwin(gbl(win_av_mode_reg));
	delwin(gbl(win_av_file_reg));
	delwin(gbl(win_av_view_reg));
	delwin(gbl(win_av_dest_dir_reg));

	delwin(gbl(win_full_view));

	delwin(gbl(win_clkdisp));
	delwin(gbl(win_commands));
	delwin(gbl(win_message));
	delwin(gbl(win_error));
	delwin(gbl(win_config));

	delwin(gbl(win_path_line_reg));
	delwin(gbl(win_dir_reg));
	delwin(gbl(win_dir_wide));
	delwin(gbl(win_dest_dir_reg));
	delwin(gbl(win_dest_dir_wide));
	delwin(gbl(win_large_file_reg));
	delwin(gbl(win_large_file_wide));
	delwin(gbl(win_small_file_reg));
	delwin(gbl(win_small_file_wide));

	delwin(gbl(win_diff_lf));
	delwin(gbl(win_diff_rt));
	delwin(gbl(win_diff_tp));

	for (i=0; i<gbl(scr_num_split_wins); i++)
	{
		delwin(gbl(win_path_line_split)[i]);
		delwin(gbl(win_dir_split)[i]);
		delwin(gbl(win_dest_dir_split)[i]);
		delwin(gbl(win_large_file_split)[i]);
		delwin(gbl(win_small_file_split)[i]);
		delwin(gbl(win_av_path_line_split)[i]);
		delwin(gbl(win_av_mode_split)[i]);
		delwin(gbl(win_av_file_split)[i]);
		delwin(gbl(win_av_view_split)[i]);
		delwin(gbl(win_av_dest_dir_split)[i]);
	}
	delwin(gbl(win_menu_sub));
	delwin(gbl(win_menu_reg));
}

static void fix_vfcbs (int old_num_split_wins)
{
	int i;
	int j;

	set_view_win(gbl(vfcb_av), gbl(win_av_view_reg));
	set_view_win(gbl(vfcb_fv), gbl(win_full_view));

	if (old_num_split_wins < gbl(scr_num_split_wins))
		j = old_num_split_wins;
	else
		j = gbl(scr_num_split_wins);

	for (i=0; i<j; i++)
		set_view_win(gbl(vfcb_av_split)[i], gbl(win_av_view_split)[i]);
	if (old_num_split_wins < gbl(scr_num_split_wins))
	{
		for (i=j; i<gbl(scr_num_split_wins); i++)
		{
			gbl(vfcb_av_split)[i] = view_init(gbl(win_av_view_split)[i]);
			strcpy(gbl(vfcb_av_split)[i]->v_pathname,
				gbl(vfcb_av_split)[j-1]->v_pathname);
			view_command(gbl(vfcb_av_split)[i], V_CMD_OPEN_RO);
			gbl(vfcb_av_split)[i]->v_top_pos =
				gbl(vfcb_av_split)[j-1]->v_top_pos;
			gbl(vfcb_av_split)[i]->v_mode = gbl(vfcb_av_split)[j-1]->v_mode;
		}
	}
	else if (old_num_split_wins > gbl(scr_num_split_wins))
	{
		for (i=j; i<old_num_split_wins; i++)
			gbl(vfcb_av_split)[i] = view_end(gbl(vfcb_av_split)[i]);
	}

	hexedit_set_win(gbl(hxcb), gbl(win_full_view));
}

void resize_term (void)
{
	int old_num_split_wins;
	int i;

	old_num_split_wins = gbl(scr_num_split_wins);
	delete_windows();
	resize_screen();

	if (get_max_rows() < MIN_WIN_ROWS ||
	    get_max_cols() < MIN_WIN_COLS)
	{
		char row_msg[128];
		char col_msg[128];

		*row_msg = 0;
		if (get_max_rows() < MIN_WIN_ROWS)
		{
			sprintf(row_msg, "%s: %s", gbl(pgm_name), msgs(m_main_minrows));
		}

		*col_msg = 0;
		if (get_max_cols() < MIN_WIN_COLS)
		{
			sprintf(col_msg, "%s: %s", gbl(pgm_name), msgs(m_main_mincols));
		}

		done_processing();

		if (*row_msg != 0)
			fprintf(stderr, "%s\n", row_msg);

		if (*col_msg != 0)
			fprintf(stderr, "%s\n", col_msg);

		pgm_exit(1);
		/*NOTREACHED*/
	}

	clearok(stdscr, FALSE);
	create_windows();
	fix_vfcbs(old_num_split_wins);

	werase(gbl(win_clkdisp));
	win_clock(gbl(win_clkdisp), clock_rtn, 0, pgm_const(time_interval));
	if (gbl(scr_is_split))
	{
		if (gbl(scr_num_split_wins) < old_num_split_wins)
		{
			for (i=gbl(scr_num_split_wins); i<old_num_split_wins; i++)
				kill_screen(i);
		}
		if (gbl(scr_num_split_wins) > old_num_split_wins)
		{
			for (i=old_num_split_wins; i<gbl(scr_num_split_wins); i++)
				clone_screen(old_num_split_wins-1, i);
		}
		gbl(scr_cur_count) = gbl(scr_num_split_wins);
		for (i=0; i<gbl(scr_cur_count); i++)
		{
			if (gbl(scr_in_autoview))
			{
				gbl(scr_stat)[i].cur_path_line    =
					gbl(win_av_path_line_split)[i];
				gbl(scr_stat)[i].cur_dest_dir_win =
					gbl(win_av_dest_dir_split)[i];
				gbl(scr_stat)[i].cur_file_win     =
					gbl(win_av_file_split)[i];
			}
			else
			{
				gbl(scr_stat)[i].cur_path_line    =
					gbl(win_path_line_split)[i];
				gbl(scr_stat)[i].cur_dest_dir_win = gbl(win_dest_dir_split)[i];
				if (gbl(scr_stat)[i].command_mode == m_dir ||
				    gbl(scr_stat)[i].in_small_window)
				{
					gbl(scr_stat)[i].cur_file_win =
						gbl(win_small_file_split)[i];
				}
				else
				{
					gbl(scr_stat)[i].cur_file_win =
						gbl(win_large_file_split)[i];
				}
			}
			gbl(scr_stat)[i].large_file_win   = gbl(win_large_file_split)[i];
			if (gbl(scr_stat)[i].command_mode == m_dir ||
			    gbl(scr_stat)[i].command_mode == m_file)
			{
				wattrset (gbl(scr_stat)[i].large_file_win,
					gbl(scr_reg_hilite_attr));
				wstandset(gbl(scr_stat)[i].large_file_win,
					gbl(scr_reg_cursor_attr));
			}
			else
			{
				wattrset (gbl(scr_stat)[i].large_file_win,
					gbl(scr_sa_hilite_attr));
				wstandset(gbl(scr_stat)[i].large_file_win,
					 gbl(scr_sa_cursor_attr));
			}
			gbl(scr_stat)[i].small_file_win   = gbl(win_small_file_split)[i];
			gbl(scr_stat)[i].cur_dir_win      = gbl(win_dir_split)[i];
			gbl(scr_stat)[i].max_dir_line     =
				getmaxy(gbl(scr_stat)[i].cur_dir_win)-1;
			gbl(scr_stat)[i].max_file_line    =
				getmaxy(gbl(scr_stat)[i].cur_file_win)-1;

			gbl(scr_stat)[i].cur_av_path_line    =
				gbl(win_av_path_line_split)[i];
			gbl(scr_stat)[i].cur_av_mode_win     = gbl(win_av_mode_split)[i];
			gbl(scr_stat)[i].cur_av_file_win     = gbl(win_av_file_split)[i];
			gbl(scr_stat)[i].cur_av_view_win     = gbl(win_av_view_split)[i];
			gbl(scr_stat)[i].cur_av_dest_dir_win =
				gbl(win_av_dest_dir_split)[i];
		}
	}
	else
	{
		gbl(scr_cur)->small_file_win = (opt(wide_screen) ?
			gbl(win_small_file_wide) : gbl(win_small_file_reg));
		gbl(scr_cur)->large_file_win = (opt(wide_screen) ?
			gbl(win_large_file_wide) : gbl(win_large_file_reg));

		if (gbl(scr_cur)->command_mode == m_dir ||
			gbl(scr_cur)->command_mode == m_file)
		{
			wattrset (gbl(scr_cur)->large_file_win, gbl(scr_reg_hilite_attr));
			wstandset(gbl(scr_cur)->large_file_win, gbl(scr_reg_cursor_attr));
		}
		else
		{
			wattrset (gbl(scr_cur)->large_file_win, gbl(scr_sa_hilite_attr));
			wstandset(gbl(scr_cur)->large_file_win, gbl(scr_sa_cursor_attr));
		}

		if (gbl(scr_in_autoview))
		{
			gbl(scr_cur)->cur_path_line    = gbl(win_av_path_line_reg);
			gbl(scr_cur)->cur_dest_dir_win = gbl(win_av_dest_dir_reg);
			gbl(scr_cur)->cur_file_win     = gbl(win_av_file_reg);
		}
		else
		{
			gbl(scr_cur)->cur_path_line    = gbl(win_path_line_reg);
			gbl(scr_cur)->cur_dest_dir_win = gbl(win_dest_dir_reg);
			if (gbl(scr_cur)->command_mode == m_dir ||
				gbl(scr_cur)->in_small_window)
			{
				gbl(scr_cur)->cur_file_win = gbl(scr_cur)->small_file_win;
			}
			else
			{
				gbl(scr_cur)->cur_file_win = gbl(scr_cur)->large_file_win;
			}
		}

		if (opt(wide_screen))
			gbl(scr_cur)->cur_dir_win      = gbl(win_dir_wide);
		else
			gbl(scr_cur)->cur_dir_win      = gbl(win_dir_reg);

		gbl(scr_cur)->max_dir_line     = getmaxy(gbl(scr_cur)->cur_dir_win)-1;
		gbl(scr_cur)->max_dir_level    =
			get_max_dir_level(gbl(scr_cur)->cur_dir_win);
		gbl(scr_cur)->max_file_line    = getmaxy(gbl(scr_cur)->cur_file_win)-1;
		set_file_disp();
		set_dir_disp();

		if (gbl(scr_cur)->dir_scroll)
		{
			while (gbl(scr_cur)->cur_dir_line > gbl(scr_cur)->max_dir_line)
			{
				gbl(scr_cur)->cur_dir_line--;
				gbl(scr_cur)->top_dir = bnext(gbl(scr_cur)->top_dir);
			}
		}

		gbl(scr_cur)->cur_file_col = 0;
		gbl(scr_cur)->cur_file_line = 0;
		gbl(scr_cur)->top_file = gbl(scr_cur)->file_cursor;

		gbl(scr_cur)->cur_av_path_line = gbl(win_av_path_line_reg);
		gbl(scr_cur)->cur_av_mode_win = gbl(win_av_mode_reg);
		gbl(scr_cur)->cur_av_file_win = gbl(win_av_file_reg);
		gbl(scr_cur)->cur_av_view_win = gbl(win_av_view_reg);
		gbl(scr_cur)->cur_av_dest_dir_win = gbl(win_av_dest_dir_reg);
	}
}
