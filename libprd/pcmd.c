/*------------------------------------------------------------------------
 * process inpput commands
 */
#include "libprd.h"

static int process_common_cmd (int c, int m)
{
	gbl(scr_cur_cmd) = c;

	if (c == cmds(CMDS_HELP))
	{
		do_help(0, 0);
		c = 0;
	}

	else if (c == cmds(CMDS_SPLIT_SCREEN) ||
		((c & ~(MASK_SHIFT | MASK_ALT)) | MASK_REG) == cmds(CMDS_SPLIT_SCREEN))
	{
		if (! gbl(scr_in_fullview) &&
			! gbl(scr_in_diffview) &&
			! gbl(scr_in_hexedit))
		{
			gbl(scr_cur_cmd) = CMDS_SPLIT_SCREEN;
			do_split();
			c = 0;
		}
	}

	else if (c == cmds(CMDS_INFO))
	{
		gbl(scr_cur_cmd) = CMDS_INFO;
		if (gbl(scr_in_fullview) ||
			gbl(scr_in_autoview) ||
			gbl(scr_in_hexedit))
		{
			disp_cur_file_info();
		}
		else if (gbl(scr_is_split))
		{
			if (gbl(scr_info_displayed))
				info_win_off();
			else
				disp_info_window();
		}
		else if (! gbl(scr_in_diffview))
		{
			if (gbl(scr_cur)->command_mode == m_dir)
				disp_cur_dir_info();
			else
				disp_cur_file_info();
		}
		c = 0;
	}

	else if (c == cmds(CMDS_TOGGLE_SPLIT))
	{
		gbl(scr_cur_cmd) = CMDS_TOGGLE_SPLIT;
		if (gbl(scr_is_split) &&
		    ! gbl(scr_in_fullview) &&
		    ! gbl(scr_in_diffview) &&
		    ! gbl(scr_in_hexedit))
		{
			int info_was_displayed = gbl(scr_info_displayed);

			toggle_split(-1);
			if (info_was_displayed)
				disp_info_window();
			c = 0;
		}
	}

	else if (c == cmds(CMDS_SHELL_ESCAPE))
	{
		gbl(scr_cur_cmd) = CMDS_SHELL_ESCAPE;
		xsystem("", "", FALSE);
		c = 0;
	}

	else if (c == cmds(CMDS_REFRESH_SCREEN))
	{
		gbl(scr_cur_cmd) = CMDS_REFRESH_SCREEN;
		wrefresh(curscr);
		c = 0;
	}

	else if (c == cmds(CMDS_CREDITS))
	{
		disp_credits_below();
		c = 0;
	}

	else if (c == cmds(CMDS_PRINT_SCREEN))
	{
		gbl(scr_cur_cmd) = CMDS_PRINT_SCREEN;
		print_scrn();
		c = 0;
	}

	else if (c == cmds(CMDS_RUN_STATS))
	{
		disp_run_stats();
		c = 0;
	}

	else if (c == cmds(CMDS_COMMON_TOGGLE_DIRDISP))
	{
		if (gbl(scr_cur)->cur_dir_no >= gbl(scr_cur)->cur_dir_line)
		{
			gbl(scr_cur)->dir_scroll = ! gbl(scr_cur)->dir_scroll;
			if (! gbl(scr_cur)->dir_scroll)
			{
				int i;

				gbl(scr_cur)->top_dir = gbl(scr_cur)->dir_cursor;
				for (i=gbl(scr_cur)->cur_dir_line; i; i--)
					gbl(scr_cur)->top_dir = bprev(gbl(scr_cur)->top_dir);
			}
		}
		c = 0;
	}

	else if (c == cmds(CMDS_CLONE))
	{
		if (opt(run_in_window))
		{
			clone_term();
		}
		c = 0;
	}

	return (c);
}

static void process_hexedit_cmd (int c, int m)
{
	if (c == KEY_MOUSE)
	{
		if (mouse_is_in_win(gbl(win_full_view)))
		{
			if (m == MOUSE_LBD)
				c = KEY_DOWN;
			else if (m == MOUSE_RBD)
				c = KEY_ESCAPE;
		}
	}

	while (c)
	{
		c = do_hexedit_cmd(c);
	}
}

static void process_fullview_cmd (int c, int m)
{
	if (c == KEY_MOUSE)
	{
		if (mouse_is_in_win(gbl(win_full_view)))
		{
			if (m == MOUSE_LBD)
				c = KEY_DOWN;
			else if (m == MOUSE_RBD)
				c = KEY_ESCAPE;
		}
	}

	while (c)
	{
		c = do_av_cmd(c);
		if (c)
			c = do_reg_fv_cmd(c);
	}
}

static void process_autoview_cmd (int c, int m)
{
	while (c)
	{
		if (c == KEY_LEFT)
		{
			if (gbl(scr_av_focus))
				hilite_file(ON);
			gbl(scr_av_focus) = FALSE;
			c = 0;
			disp_cmds();
			break;
		}

		if (c == KEY_RIGHT)
		{
			if (!gbl(scr_av_focus))
			{
				hilite_file(OFF);
				waddch(gbl(scr_cur)->cur_file_win, pgm_const(selected_char));
				wrefresh(gbl(scr_cur)->cur_file_win);
			}
			gbl(scr_av_focus) = TRUE;
			c = 0;
			disp_cmds();
			break;
		}

		if (! gbl(scr_av_focus))
			c = do_file_cmd(c);

		if (c)
			c = do_av_cmd(c);

		if (c)
		{
			switch (gbl(scr_cur)->cmd_sub_mode)
			{
			case m_reg:	c = do_reg_av_cmd(c);	break;
			case m_alt:	c = do_alt_av_cmd(c);	break;
			case m_tag:	c = do_tag_av_cmd(c);	break;
			}
		}
	}
}

static void process_reg_cmd (int c, int m)
{
	switch (gbl(scr_cur)->command_mode)
	{
	case m_file:
	case m_showall:
	case m_level_showall:
	case m_global:
	case m_tag_showall:
	case m_tag_global:
		while (c)
		{
			c = do_file_cmd(c);

			if (c)
			{
				switch ((gbl(scr_cur)->cur_root)->node_type)
				{
				case N_FS:
					switch (gbl(scr_cur)->cmd_sub_mode)
					{
					case m_reg:	c = do_reg_file_cmd(c);	break;
					case m_alt:	c = do_alt_file_cmd(c);	break;
					case m_tag:	c = do_tag_file_cmd(c);	break;
					}
					break;

				case N_ARCH:
					switch (gbl(scr_cur)->cmd_sub_mode)
					{
					case m_reg:	c = do_reg_file_arc(c);	break;
					case m_alt:	c = do_alt_file_arc(c);	break;
					case m_tag:	c = do_tag_file_arc(c);	break;
					}
					break;

				case N_FTP:
					switch (gbl(scr_cur)->cmd_sub_mode)
					{
					case m_reg:	c = do_reg_file_ftp(c);	break;
					case m_alt:	c = do_alt_file_ftp(c);	break;
					case m_tag:	c = do_tag_file_ftp(c);	break;
					}
					break;
				}
			}
		}
		break;

	case m_dir:
		while (c)
		{
			c = do_dir_cmd(c);

			if (c)
			{
				switch ((gbl(scr_cur)->cur_root)->node_type)
				{
				case N_FS:
					switch (gbl(scr_cur)->cmd_sub_mode)
					{
					case m_reg:	c = do_reg_dir_cmd(c);	break;
					case m_alt:	c = do_alt_dir_cmd(c);	break;
					case m_tag:	c = do_tag_dir_cmd(c);	break;
					}
					break;

				case N_ARCH:
					switch (gbl(scr_cur)->cmd_sub_mode)
					{
					case m_reg:	c = do_reg_dir_arc(c);	break;
					case m_alt:	c = do_alt_dir_arc(c);	break;
					case m_tag:	c = do_tag_dir_arc(c);	break;
					}
					break;

				case N_FTP:
					switch (gbl(scr_cur)->cmd_sub_mode)
					{
					case m_reg:	c = do_reg_dir_ftp(c);	break;
					case m_alt:	c = do_alt_dir_ftp(c);	break;
					case m_tag:	c = do_tag_dir_ftp(c);	break;
					}
					break;
				}
			}
		}
		break;
	}
}

void process_cmd (int c)
{
	int save_sub_mode;
	int m = 0;

	/*--------------------------------------------------------------------
	 * check if mouse event
	 */
	if (c == KEY_MOUSE)
	{
		m = mouse_get_event(gbl(win_commands));
		if (m == MOUSE_MOVE || m == MOUSE_LBU || m == MOUSE_RBU)
		{
			return;
		}
	}

	/*--------------------------------------------------------------------
	 * The following cmds are executed at a cmd prompt,
	 * regardless of cmd mode or sub mode.
	 */
	c = process_common_cmd(c, m);
	if (c == 0)
		return;

	/*--------------------------------------------------------------------
	 * If we get here, we have a cmd to pass on.
	 *
	 * If char has alt bit on, treat as temp alt cmd.
	 * If char has shift bit on or is uppercase,
	 * treat as temp tag cmd if shift_for_tag is on.
	 */
	save_sub_mode = gbl(scr_cur)->cmd_sub_mode;
	gbl(scr_temp_sub_mode) = FALSE;

	if (c == KEY_SHIFT_PRESS ||
		c == KEY_SHIFT_RELEASE)
	{
		if (! opt(shift_for_tag))
			return;

		if (gbl(scr_in_autoview) ||
			gbl(scr_in_fullview) ||
			gbl(scr_in_hexedit)  ||
			gbl(scr_in_menu)     ||
			gbl(scr_in_diffview) ||
			gbl(scr_in_help)     ||
			gbl(scr_in_about_box) )
		{
			return;
		}

		if (c == KEY_SHIFT_PRESS)
		{
			switch (gbl(scr_cur)->cmd_sub_mode)
			{
			case m_reg:	c = cmds(CMDS_TAG_MENU_1);	break;
			case m_tag:	return;
			case m_alt:	c = cmds(CMDS_TAG_MENU_1);	break;
			}
		}
		else
		{
			switch (gbl(scr_cur)->cmd_sub_mode)
			{
			case m_reg:	return;
			case m_tag:	c = KEY_RETURN;	break;
			case m_alt:	return;
			}
		}
	}
	else if (c == KEY_CTRL_PRESS ||
			 c == KEY_CTRL_RELEASE)
	{
		return;
	}
	else if (c == KEY_ALT_PRESS ||
			 c == KEY_ALT_RELEASE)
	{
		if (gbl(scr_in_autoview) ||
			gbl(scr_in_fullview) ||
			gbl(scr_in_hexedit)  ||
			gbl(scr_in_menu)     ||
			gbl(scr_in_diffview) ||
			gbl(scr_in_help)     ||
			gbl(scr_in_about_box) )
		{
			return;
		}

		if (c == KEY_ALT_PRESS)
		{
			switch (gbl(scr_cur)->cmd_sub_mode)
			{
			case m_reg:	c = cmds(CMDS_ALT_MENU_1);	break;
			case m_tag:	c = cmds(CMDS_ALT_MENU_1);	break;
			case m_alt:	return;
			}
		}
		else
		{
			switch (gbl(scr_cur)->cmd_sub_mode)
			{
			case m_reg:	return;
			case m_tag:	return;
			case m_alt:	c = KEY_RETURN;	break;
			}
		}
	}

	if (c & MASK_ALT)
	{
		c &= ~MASK_ALT;
		if (c >= K_F1 && c <= K_F16)		/* add in mask if func key */
			c |= MASK_REG;
		if (gbl(scr_cur)->cmd_sub_mode != m_alt)
		{
			gbl(scr_cur)->cmd_sub_mode = m_alt;
			gbl(scr_temp_sub_mode) = TRUE;
		}
	}
	else if (opt(shift_for_tag) &&
			((c & MASK_SHIFT) || (IS_ACHAR(c) && isalpha(c) && isupper(c))))
	{
		c &= ~MASK_SHIFT;
		if (c >= K_F1 && c <= K_F16)		/* add in mask if func key */
			c |= MASK_REG;
		if (gbl(scr_cur)->cmd_sub_mode != m_tag)
		{
			gbl(scr_cur)->cmd_sub_mode = m_tag;
			gbl(scr_temp_sub_mode) = TRUE;
		}
	}

	c = TO_LOWER(c);

	if (c == KEY_MOUSE || c == cmds(CMDS_COMMON_MOUSEPOS))
		c = check_mouse_pos();

	/*--------------------------------------------------------------------
	 * process the cmd
	 */
	if (gbl(scr_in_hexedit))
	{
		process_hexedit_cmd(c, m);
	}
	else if (gbl(scr_in_fullview))
	{
		process_fullview_cmd(c, m);
	}
	else if (gbl(scr_in_autoview))
	{
		process_autoview_cmd(c, m);
	}
	else if (gbl(scr_in_diffview))
	{
		do_diff_cmd(c);
	}
	else
	{
		process_reg_cmd(c, m);
	}

	/*--------------------------------------------------------------------
	 * restore cmd_sub_mode if temp
	 */
	if (gbl(scr_temp_sub_mode))
		gbl(scr_cur)->cmd_sub_mode = save_sub_mode;
}
