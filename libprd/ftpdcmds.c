/*------------------------------------------------------------------------
 * ftp dir cmds
 */
#include "libprd.h"

int do_reg_dir_ftp (int c)
{
	int m;

	if (c == KEY_MOUSE)
	{
		m = mouse_get_event(gbl(win_commands));
		if (m == MOUSE_RBD)
			c = cmds(CMDS_COMMON_INVERT);
		else if (m == MOUSE_LBDBL)
			c = KEY_RETURN;
	}

	if (c == KEY_RETURN)
	{
		int not_logged = ((gbl(scr_cur)->cur_dir)->flags & D_NOT_LOGGED) != 0;

		if (not_logged && (! ((gbl(scr_cur)->cur_dir)->flags & D_CANT_LOG)))
			relog_ftp_dir(FALSE);
		if (! not_logged)
			setup_file_display(0);

		return (0);
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

	else if (c == cmds(CMDS_COMMON_DELETE))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_DELETE;
		if (check_read_only(msgs(m_dircmd2_del)))
			return (0);
		ftp_do_delete_dir();
		disp_cmds();
		return (RESET);
	}

	else if (c == cmds(CMDS_COMMON_FILESPEC))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_FILESPEC;
		do_file_spec();
		disp_cmds();
		return (0);
	}

	else if (c == cmds(CMDS_REG_DIR_GLOBAL))
	{
		gbl(scr_cur_cmd) = CMDS_REG_DIR_GLOBAL;
		do_cmn_reg_dir_global();
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_INVERT))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_INVERT;
		do_cmn_reg_dir_invert();
		return (' ');
	}

	else if (c == cmds(CMDS_REG_DIR_MKDIR))
	{
		gbl(scr_cur_cmd) = CMDS_REG_DIR_MKDIR;
		if (check_read_only(msgs(m_dircmd2_mkd)))
			return (0);
		ftp_do_make_dir();
		disp_cmds();
		return (0);
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
		do_dir_print();
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

	else if (c == cmds(CMDS_REG_DIR_SHOWALL))
	{
		gbl(scr_cur_cmd) = CMDS_REG_DIR_SHOWALL;
		do_cmn_reg_dir_showall();
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_TAG))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_TAG;
		do_cmn_reg_dir_tag();
		return (' ');
	}

	else if (c == cmds(CMDS_COMMON_UNTAG))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_UNTAG;
		do_cmn_reg_dir_untag();
		return (' ');
	}

	else if (c == cmds(CMDS_COMMON_EXEC))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_EXEC;
		xsystem("", "", FALSE);
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

	else if (c == cmds(CMDS_REG_DIR_RELOG))
	{
		gbl(scr_cur_cmd) = CMDS_REG_DIR_RELOG;
		relog_ftp_dir(FALSE);
		return (0);
	}

	else if (c == cmds(CMDS_REG_DIR_LOG_RECURS))
	{
		gbl(scr_cur_cmd) = CMDS_REG_DIR_LOG_RECURS;
		relog_ftp_dir(TRUE);
		return (0);
	}

	else if (c == cmds(CMDS_REG_DIR_UNLOG))
	{
		gbl(scr_cur_cmd) = CMDS_REG_DIR_UNLOG;
		unlog_dir();
		return (0);
	}

	else if (c == cmds(CMDS_REG_DIR_HIDE))
	{
		gbl(scr_cur_cmd) = CMDS_REG_DIR_HIDE;
		hide_sub_node(ON);
		return (0);
	}

	else if (c == cmds(CMDS_REG_DIR_SHOW))
	{
		gbl(scr_cur_cmd) = CMDS_REG_DIR_SHOW;
		hide_sub_node(OFF);
		return (0);
	}

	return (0);
}

int do_tag_dir_ftp (int c)
{
	if (c == KEY_ESCAPE || c == KEY_RETURN)
	{
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

	else if (c == cmds(CMDS_TAG_DIR_GLOBAL))
	{
		gbl(scr_cur_cmd) = CMDS_TAG_DIR_GLOBAL;
		do_cmn_tag_dir_global();
	}

	else if (c == cmds(CMDS_COMMON_INVERT))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_INVERT;
		do_cmn_tag_dir_invert();
	}

	else if (c == cmds(CMDS_TAG_DIR_SHOWALL))
	{
		gbl(scr_cur_cmd) = CMDS_TAG_DIR_SHOWALL;
		do_cmn_tag_dir_showall();
	}

	else if (c == cmds(CMDS_COMMON_TAG))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_TAG;
		do_cmn_tag_dir_tag();
	}

	else if (c == cmds(CMDS_COMMON_UNTAG))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_UNTAG;
		do_cmn_tag_dir_untag();
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

	else if (c == cmds(CMDS_REG_DIR_RELOG))
	{
		gbl(scr_cur_cmd) = CMDS_REG_DIR_RELOG;
		relog_ftp_dir(FALSE);
	}

	else if (c == cmds(CMDS_REG_DIR_LOG_RECURS))
	{
		gbl(scr_cur_cmd) = CMDS_REG_DIR_LOG_RECURS;
		relog_ftp_dir(TRUE);
	}

	else if (c == cmds(CMDS_REG_DIR_UNLOG))
	{
		gbl(scr_cur_cmd) = CMDS_REG_DIR_UNLOG;
		unlog_dir();
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

int do_alt_dir_ftp (int c)
{
	if (c == KEY_ESCAPE || c == KEY_RETURN)
	{
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

	else if (c == cmds(CMDS_COMMON_DIRDISP))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_DIRDISP;
		do_dir_disp();
		hilite_dir(ON);
	}

	else if (c == cmds(CMDS_COMMON_FILEDISP))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_FILEDISP;
		do_file_disp();
	}

	else if (c == cmds(CMDS_COMMON_INVERT))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_INVERT;
		do_cmn_alt_dir_invert();
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

	else if (c == cmds(CMDS_ALT_DIR_RELEASE))
	{
		gbl(scr_cur_cmd) = CMDS_ALT_DIR_RELEASE;
		do_release();
	}

	else if (c == cmds(CMDS_ALT_DIR_SORT))
	{
		gbl(scr_cur_cmd) = CMDS_ALT_DIR_SORT;
		do_cmn_alt_dir_sort();
	}

	else if (c == cmds(CMDS_COMMON_TAG))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_TAG;
		do_cmn_alt_dir_tag();
	}

	else if (c == cmds(CMDS_COMMON_UNTAG))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_UNTAG;
		do_cmn_alt_dir_untag();
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

	else if (c == cmds(CMDS_REG_DIR_RELOG))
	{
		gbl(scr_cur_cmd) = CMDS_REG_DIR_RELOG;
		relog_ftp_dir(FALSE);
	}

	else if (c == cmds(CMDS_REG_DIR_LOG_RECURS))
	{
		gbl(scr_cur_cmd) = CMDS_REG_DIR_LOG_RECURS;
		relog_ftp_dir(TRUE);
	}

	else if (c == cmds(CMDS_REG_DIR_UNLOG))
	{
		gbl(scr_cur_cmd) = CMDS_REG_DIR_UNLOG;
		unlog_dir();
	}

	else if (c == cmds(CMDS_ALT_DIR_HIDE))
	{
		gbl(scr_cur_cmd) = CMDS_ALT_DIR_HIDE;
		hide_entire_node(ON);
	}

	else if (c == cmds(CMDS_ALT_DIR_SHOW))
	{
		gbl(scr_cur_cmd) = CMDS_ALT_DIR_SHOW;
		hide_entire_node(OFF);
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
