/*------------------------------------------------------------------------
 * main program entry (after opt/arg processing)
 */
#include "libprd.h"

static void do_headline_delay (void)
{
	int save_delay	= getdelay(stdscr);
	int use_delay	= cmdopt(headline_delay);
	int	c;

	if (use_delay > 0)
		use_delay *= 1000;

	flushinp();

	setdelay(stdscr, use_delay);
	while (TRUE)
	{
		c = xgetch(stdscr);
		if (c != KEY_MOUSE && c != ERR)
			break;

		if (mouse_get_event(stdscr) == MOUSE_LBD)
			break;
	}
	setdelay(stdscr, save_delay);
}

void pgm (const char *directory)
{
	int c;

	/*--------------------------------------------------------------------
	 * initialize all our basic globals
	 */
	gbl(owner_list)			= 0;
	gbl(group_list)			= 0;
	gbl(nodes_list)			= 0;
	gbl(mount_list)			= 0;

	gbl(scr_cur_cmd)		= 0;
	gbl(scr_cur_sub_cmd)	= 0;

	gbl(scr_is_split)		= FALSE;
	gbl(scr_av_focus)		= FALSE;
	gbl(scr_info_displayed)	= FALSE;

	gbl(scr_in_autoview)	= FALSE;
	gbl(scr_in_fullview)	= FALSE;
	gbl(scr_in_hexedit)		= FALSE;
	gbl(scr_in_config)		= FALSE;
	gbl(scr_in_menu)		= FALSE;
	gbl(scr_in_diffview)	= FALSE;
	gbl(scr_in_help)		= FALSE;
	gbl(scr_in_about_box)	= FALSE;

	/*--------------------------------------------------------------------
	 * setup current-display structure
	 */
	init_cd();

	/*--------------------------------------------------------------------
	 *	Setup initial display.
	 *	Note that we do this first because ml_get_mounts()
	 *	could take some time and we want to show the
	 *	people back home a picture.
	 */
	werase(stdscr);
	wrefresh(stdscr);

	werase(gbl(win_path_line_reg));
	wrefresh(gbl(win_path_line_reg));

	werase(gbl(win_clkdisp));
	wrefresh(gbl(win_clkdisp));

	main_border();
	headline();
	small_border(ON);
	disp_credits();

	werase(gbl(win_commands));
	wrefresh(gbl(win_commands));

	werase(gbl(win_message));
	esc_msg();
	wrefresh(gbl(win_message));

	/*--------------------------------------------------------------------
	 *	Read in the mount table
	 */
	if (! cmdopt(skip_mnt_table))
		gbl(mount_list) = ml_get_mounts(gbl(mount_list));

	/*--------------------------------------------------------------------
	 *	check if headline delay wanted
	 */
	if (cmdopt(headline_delay) != 0)
	{
		do_headline_delay();
	}

	/*--------------------------------------------------------------------
	 *	setup clock
	 */
	werase(gbl(win_clkdisp));
	win_clock(gbl(win_clkdisp), clock_rtn, 0, pgm_const(time_interval));
	win_clock_set(opt(display_clock));

	/*--------------------------------------------------------------------
	 *	Either read in a chkpoint file or read the directory requested.
	 */
	if (cmdopt(use_ckp_file) && getnode() == 0)
		gbl(scr_cur)->cur_node = gbl(nodes_list);

	if (! gbl(scr_cur)->cur_node)
	{
		if (init_node(directory))
			all_done();

		if (! opt(home_to_node))
		{
			TREE *t = pathname_to_dirtree(gbl(pgm_cwd));

			if (bfind((gbl(scr_cur)->cur_root)->dir_list, t))
				(gbl(scr_cur)->cur_root)->cur_dir_tree = t;
		}
	}

	/*--------------------------------------------------------------------
	 *	Initialize the display
	 */
	init_disp();

	/*--------------------------------------------------------------------
	 *	now process input (never returns)
	 */
	if (opt(enter_app_menu))
		c = cmds(CMDS_REG_DIR_MENU);
	else
		c = 0;

	process_input(c);
	/*NOTREACHED*/
}
