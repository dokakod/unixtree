/*------------------------------------------------------------------------
 * ftp file cmds
 */
#include "libprd.h"

int do_reg_file_ftp (int c)
{
	int i;
	int m;
	BLIST *b;

	if (c == KEY_MOUSE)
	{
		m = mouse_get_event(gbl(win_commands));
		if (m == MOUSE_RBD)
			c = cmds(CMDS_COMMON_INVERT);
		else if (m == MOUSE_RBDBL)
			c = cmds(CMDS_COMMON_VIEW);
	}

	if (c == KEY_ESCAPE || c == KEY_BS || c == KEY_RETURN)
	{
	}

	else if (c == cmds(CMDS_COMMON_NEXT_NODE_1) ||
		     c == cmds(CMDS_COMMON_NEXT_NODE_2))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_NEXT_NODE_1;
		do_cmn_next_node();
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_PREV_NODE_1) ||
		     c == cmds(CMDS_COMMON_PREV_NODE_2))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_PREV_NODE_1;
		do_cmn_prev_node();
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_ATTRS))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_ATTRS;
		if (check_read_only(msgs(m_filecmd2_att)))
			return (0);
		ftp_do_file_attr();
		disp_cmds();
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_COPY))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_COPY;
		if (check_read_only(msgs(m_filecmd2_cop)))
			return (0);
		ftp_do_copy();
		disp_cmds();
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_DELETE))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_DELETE;
		if (check_read_only(msgs(m_filecmd2_del)))
			return (0);
		i = ftp_do_delete();
		if (gbl(scr_cur)->numfiles)
		{
			if (!i)
			{
				hilite_file(ON);
				disp_cur_file();
			}
			disp_cmds();
			return (0);
		}
	}

	else if (c == cmds(CMDS_COMMON_FILESPEC))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_FILESPEC;
		do_file_spec();
		if (gbl(scr_cur)->first_file)
		{
			disp_cmds();
			return (0);
		}
	}

	else if (c == cmds(CMDS_COMMON_GOTO1) || c == cmds(CMDS_COMMON_GOTO2))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_GOTO1;
		goto_file();
		disp_cmds();
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_INVERT))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_INVERT;
		do_cmn_reg_file_invert();
		return (' ');
	}

	else if (c == cmds(CMDS_COMMON_NODE))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_NODE;
		do_log();
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_PRINT))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_PRINT;
		ftp_do_print();
		disp_cmds();
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_QUIT))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_QUIT;
		if (quit())
			all_done();
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_RENAME))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_RENAME;
		if (check_read_only(msgs(m_filecmd2_ren)))
			return (0);
		ftp_do_rename();
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_TAG))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_TAG;
		do_cmn_reg_file_tag();
		return (' ');
	}

	else if (c == cmds(CMDS_COMMON_UNTAG))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_UNTAG;
		do_cmn_reg_file_untag();
		return (' ');
	}

	else if (c == cmds(CMDS_COMMON_VIEW))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_VIEW;
		ftp_do_view();
		return (0);
	}

	else if (c == cmds(CMDS_TAG_MENU_1) || c == cmds(CMDS_TAG_MENU_2))
	{
		gbl(scr_cur)->cmd_sub_mode = m_tag;
		disp_cmds();
		return (0);
	}

	else if (c == cmds(CMDS_ALT_MENU_1) || c == cmds(CMDS_ALT_MENU_2))
	{
		gbl(scr_cur)->cmd_sub_mode = m_alt;
		disp_cmds();
		return (0);
	}

	else
	{
		/* beep? */
		return (0);
	}

	/* reset back to dir display or full file disp */

	if (c == KEY_RETURN &&
		gbl(scr_cur)->numfiles &&
		opt(do_small_window) &&
		gbl(scr_cur)->in_small_window)
	{
		/* small window to big window */

		dir_scroll_bar_remove();
		file_scroll_bar_remove();
		gbl(scr_cur)->in_small_window = 0;
		gbl(scr_cur)->cur_file_line = 0;
		gbl(scr_cur)->cur_file_col  = 0;
		gbl(scr_cur)->cur_file_win  = gbl(scr_cur)->large_file_win;
		wattrset (gbl(scr_cur)->cur_file_win, gbl(scr_reg_hilite_attr));
		wstandset(gbl(scr_cur)->cur_file_win, gbl(scr_reg_cursor_attr));
		gbl(scr_cur)->max_file_line = getmaxy(gbl(scr_cur)->cur_file_win)-1;
		for (b=gbl(scr_cur)->top_file; b; b=bnext(b))
		{
			if (b == gbl(scr_cur)->file_cursor)
				break;
			gbl(scr_cur)->cur_file_line++;
			if (gbl(scr_cur)->cur_file_line > gbl(scr_cur)->max_file_line)
			{
				gbl(scr_cur)->cur_file_line = 0;
				gbl(scr_cur)->cur_file_col++;
			}
		}
		small_border(OFF);
		werase(gbl(scr_cur)->cur_file_win);
		disp_file_list();
		do_file_scroll_bar();
		hilite_file(ON);
		return (0);
	}

	/* big window back to dir window */

	file_scroll_bar_remove();
	gbl(scr_cur)->command_mode = m_dir;
	if (c == KEY_RETURN || !gbl(scr_cur)->in_small_window)
	{
		disp_dir_tree();
		dir_scroll_bar_init();
		do_dir_scroll_bar();
	}
	gbl(scr_cur)->in_small_window = 0;
	check_small_window();
	hilite_dir(ON);
	dptr_to_dirname(gbl(scr_cur)->dir_cursor, gbl(scr_cur)->path_name);
	disp_path_line();
	disp_file_box();
	disp_drive_info(0, 1);
	disp_node_stats(gbl(scr_cur)->cur_root);
	disp_cur_dir();
	disp_cmds();
	return (0);
}

int do_tag_file_ftp (int c)
{
	if (c == KEY_ESCAPE || c == KEY_RETURN)
	{
	}

	else if (c == cmds(CMDS_COMMON_ATTRS))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_ATTRS;
		if (!check_read_only(msgs(m_filecmd2_att)))
			ftp_do_tag_file_attr();
	}

	else if (c == cmds(CMDS_COMMON_COPY))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_COPY;
		if (!check_read_only(msgs(m_filecmd2_cop)))
			ftp_do_tag_copy();
	}

	else if (c == cmds(CMDS_COMMON_DELETE))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_DELETE;
		if (!check_read_only(msgs(m_filecmd2_del)))
		{
			ftp_do_tag_delete();
			if (!gbl(scr_cur)->numfiles)
			{
				gbl(scr_cur)->cmd_sub_mode = m_reg;
				return (KEY_RETURN);
			}
		}
	}

	else if (c == cmds(CMDS_COMMON_INVERT))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_INVERT;
		do_cmn_tag_file_invert();
	}

	else if (c == cmds(CMDS_COMMON_PRINT))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_PRINT;
		ftp_do_tag_print();
	}

	else if (c == cmds(CMDS_COMMON_RENAME))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_RENAME;
		if (!check_read_only(msgs(m_filecmd2_ren)))
			ftp_do_tag_rename();
	}

	else if (c == cmds(CMDS_COMMON_TAG))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_TAG;
		do_cmn_tag_file_tag();
	}

	else if (c == cmds(CMDS_COMMON_UNTAG))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_UNTAG;
		do_cmn_tag_file_untag();
	}

	else if (c == cmds(CMDS_COMMON_NEXT_NODE_1) ||
		     c == cmds(CMDS_COMMON_NEXT_NODE_2))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_NEXT_NODE_1;
		do_cmn_next_node();
	}

	else if (c == cmds(CMDS_COMMON_PREV_NODE_1) ||
		     c == cmds(CMDS_COMMON_PREV_NODE_2))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_PREV_NODE_1;
		do_cmn_prev_node();
	}

	else if (c == cmds(CMDS_TAG_MENU_1) || c == cmds(CMDS_TAG_MENU_2))
	{
		return (0);
	}

	else if (c == cmds(CMDS_ALT_MENU_1) || c == cmds(CMDS_ALT_MENU_2))
	{
		gbl(scr_cur)->cmd_sub_mode = m_alt;
		gbl(scr_temp_sub_mode) = FALSE;
		disp_cmds();
		return (0);
	}

	else
	{
		/* beep? */
	}

	if (! gbl(scr_kbd_shift))
		gbl(scr_cur)->cmd_sub_mode = m_reg;
	disp_cmds();
	return (0);
}

int do_alt_file_ftp (int c)
{
	if (c == KEY_ESCAPE || c == KEY_RETURN)
	{
	}

	else if (c == cmds(CMDS_COMMON_COPY))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_COPY;
		if (!check_read_only(msgs(m_filecmd2_cop)))
			ftp_do_alt_copy();
	}

	else if (c == cmds(CMDS_COMMON_DIRDISP))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_DIRDISP;
		do_dir_disp();
		if (gbl(scr_cur)->in_small_window)
		{
			hilite_dir(OFF);
			left_arrow(gbl(scr_cur)->cur_dir_win);
			wrefresh(gbl(scr_cur)->cur_dir_win);
		}
	}

	else if (c == cmds(CMDS_COMMON_FILEDISP))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_FILEDISP;
		do_file_disp();
		hilite_file(ON);
	}

	else if (c == cmds(CMDS_COMMON_INVERT))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_INVERT;
		do_cmn_alt_file_invert();
	}

	else if (c == cmds(CMDS_COMMON_NODE))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_NODE;
		do_log();
	}

	else if (c == cmds(CMDS_COMMON_QUIT))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_QUIT;
		if (alt_quit())
			all_done();
	}

	else if (c == cmds(CMDS_ALT_FILE_RELEASE))
	{
		gbl(scr_cur_cmd) = CMDS_ALT_FILE_RELEASE;
		do_release();
	}

	else if (c == cmds(CMDS_ALT_FILE_SORT))
	{
		gbl(scr_cur_cmd) = CMDS_ALT_FILE_SORT;
		do_cmn_alt_file_sort();
	}

	else if (c == cmds(CMDS_COMMON_TAG))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_TAG;
		do_cmn_alt_file_tag();
	}

	else if (c == cmds(CMDS_COMMON_UNTAG))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_UNTAG;
		do_cmn_alt_file_untag();
	}

	else if (c == cmds(CMDS_ALT_FILE_RELOG))
	{
		gbl(scr_cur_cmd) = CMDS_ALT_FILE_RELOG;
		relog_ftp_dir(FALSE);
	}

	else if (c == cmds(CMDS_COMMON_NEXT_NODE_1) ||
		     c == cmds(CMDS_COMMON_NEXT_NODE_2))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_NEXT_NODE_1;
		do_cmn_next_node();
	}

	else if (c == cmds(CMDS_COMMON_PREV_NODE_1) ||
		     c == cmds(CMDS_COMMON_PREV_NODE_2))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_PREV_NODE_1;
		do_cmn_prev_node();
	}

	else if (c == cmds(CMDS_TAG_MENU_1) || c == cmds(CMDS_TAG_MENU_2))
	{
		gbl(scr_cur)->cmd_sub_mode = m_tag;
		gbl(scr_temp_sub_mode) = FALSE;
		disp_cmds();
		return (0);
	}

	else if (c == cmds(CMDS_ALT_MENU_1) || c == cmds(CMDS_ALT_MENU_2))
	{
		return (0);
	}

	else if (c == cmds(CMDS_ALT_FILE_COPYTAGS))
	{
		gbl(scr_cur_cmd) = CMDS_ALT_FILE_COPYTAGS;
		do_cmn_alt_file_copytags();
	}

	else if (c == cmds(CMDS_ALT_FILE_RESOLVE))
	{
		gbl(scr_cur_cmd) = CMDS_ALT_FILE_RESOLVE;
		if (ftp_resolve_symlink() > 0)
			return (KEY_RETURN);
	}

	else
	{
		/* beep? */
	}

	if (! gbl(scr_kbd_alt))
		gbl(scr_cur)->cmd_sub_mode = m_reg;
	disp_cmds();
	return (0);
}
