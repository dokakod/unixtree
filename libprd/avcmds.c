/*------------------------------------------------------------------------
 * autoview cmd routines
 */
#include "libprd.h"

static int do_mode_cmd (int c)
{
	if (c == cmds(CMDS_WIN_ASCII))
	{
		gbl(scr_cur_cmd) = CMDS_WIN_ASCII;
		view_command(gbl(scr_cur)->vfcb, V_CMD_ASCII);
		disp_av_mode();
		return (0);
	}

	if (c == cmds(CMDS_WIN_DUMP))
	{
		gbl(scr_cur_cmd) = CMDS_WIN_DUMP;
		view_command(gbl(scr_cur)->vfcb, V_CMD_DUMP);
		disp_av_mode();
		return (0);
	}

	if (c == cmds(CMDS_WIN_HEX))
	{
		gbl(scr_cur_cmd) = CMDS_WIN_HEX;
		view_command(gbl(scr_cur)->vfcb, V_CMD_HEX);
		disp_av_mode();
		return (0);
	}

	if (c == cmds(CMDS_WIN_KEEP))
	{
		gbl(scr_cur_cmd) = CMDS_WIN_KEEP;
		view_command(gbl(scr_cur)->vfcb, V_KEEP_MODE);
		disp_av_mode();
		return (0);
	}

	if (c == cmds(CMDS_WIN_MASK))
	{
		gbl(scr_cur_cmd) = CMDS_WIN_MASK;
		view_command(gbl(scr_cur)->vfcb, V_CMD_TOGGLE_MASK);
		disp_av_mode();
		return (0);
	}

	if (c == cmds(CMDS_WIN_WRAP))
	{
		gbl(scr_cur_cmd) = CMDS_WIN_WRAP;
		view_command(gbl(scr_cur)->vfcb, V_CMD_WRAP);
		disp_av_mode();
		return (0);
	}

	return (c);
}

int do_av_cmd (int c)
{
	if (c == KEY_UP || c == KEY_CTRL_UP)
	{
		view_command(gbl(scr_cur)->vfcb, V_CMD_UP);
		disp_av_pct(TRUE);
		return (0);
	}

	if (c == KEY_DOWN || c == KEY_CTRL_DOWN)
	{
		view_command(gbl(scr_cur)->vfcb, V_CMD_DOWN);
		disp_av_pct(TRUE);
		return (0);
	}

	if (c == KEY_LEFT || c == KEY_CTRL_LEFT)
	{
		view_command(gbl(scr_cur)->vfcb, V_CMD_LEFT);
		return (0);
	}

	if (c == KEY_RIGHT || c == KEY_CTRL_RIGHT)
	{
		view_command(gbl(scr_cur)->vfcb, V_CMD_RIGHT);
		return (0);
	}

	if (c == KEY_HOME || c == KEY_CTRL_HOME)
	{
		view_command(gbl(scr_cur)->vfcb, V_CMD_HOME);
		disp_av_pct(TRUE);
		return (0);
	}

	if (c == KEY_END || c == KEY_CTRL_END)
	{
		view_command(gbl(scr_cur)->vfcb, V_CMD_END);
		disp_av_pct(TRUE);
		return (0);
	}

	if (c == KEY_PGUP || c == KEY_CTRL_PGUP)
	{
		view_command(gbl(scr_cur)->vfcb, V_CMD_PGUP);
		disp_av_pct(TRUE);
		return (0);
	}

	if (c == KEY_PGDN || c == KEY_CTRL_PGDN)
	{
		view_command(gbl(scr_cur)->vfcb, V_CMD_PGDN);
		disp_av_pct(TRUE);
		return (0);
	}

	else
	{
		return (c);
	}
	/*NOTREACHED*/
}

int do_reg_av_cmd (int c)
{
	VFCB *v;
	int i;
	int m;

	if (c == cmds(CMDS_VIEW_WINDOW))
	{
		c = get_disp_mode();
		if (c == KEY_ESCAPE || c == KEY_BS)
			return (0);
		c = do_mode_cmd(c);
		if (!c)
			return (0);
	}

	v = gbl(scr_cur)->vfcb;

	if (c == KEY_MOUSE)
	{
		m = mouse_get_event(v->v_win);
		if (m == MOUSE_RBDBL)
			c = cmds(CMDS_COMMON_VIEW);
	}

	if (c == KEY_ESCAPE || c == KEY_BS || c == cmds(CMDS_AUTOVIEW))
	{
		end_av();
		setup_display();
		disp_cmds();
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_ATTRS))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_ATTRS;
		if (check_read_only(msgs(m_filecmd2_att)))
			return (0);
		do_file_attr();
		disp_cmds();
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_COPY))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_COPY;
		if (check_read_only(msgs(m_filecmd2_cop)))
			return (0);
		do_copy();
		disp_cmds();
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_DELETE))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_DELETE;
		if (check_read_only(msgs(m_filecmd2_del)))
			return (0);
		i = do_delete();
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

	else if (c == cmds(CMDS_COMMON_EDIT))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_EDIT;
		gbl(scr_av_focus) = FALSE;
		do_editor(gbl(scr_cur)->cur_file);
		close_av_file();
		open_av_file(gbl(scr_cur)->cur_file);
		display_av();
		return (0);
	}

	else if (c == cmds(CMDS_VIEW_HEXEDIT))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_EDIT;
		gbl(scr_av_focus) = FALSE;
		close_av_file();
		do_hexedit(gbl(scr_cur)->cur_file);
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_DIFF))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_DIFF;
		diff();
		disp_cmds();
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_FILESPEC))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_FILESPEC;
		gbl(scr_av_focus) = FALSE;
		do_file_spec();
		if (!gbl(scr_cur)->first_file)
			return (KEY_ESCAPE);
		disp_cmds();
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_GOTO1))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_GOTO1;
		gbl(scr_av_focus) = FALSE;
		goto_file();
		if (gbl(scr_cur)->cur_file != gbl(scr_cur)->av_file_displayed)
		{
			close_av_file();
			open_av_file(gbl(scr_cur)->cur_file);
			display_av();
		}
		disp_cmds();
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_INVERT))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_INVERT;
		gbl(scr_av_focus) = FALSE;
		do_cmn_reg_file_invert();
		return (' ');
	}

	else if (c == cmds(CMDS_REG_FILE_LINK))
	{
		gbl(scr_cur_cmd) = CMDS_REG_FILE_LINK;
		if (check_read_only(msgs(m_filecmd2_lin)))
			return (0);
		do_link_file();
		disp_cmds();
		return (0);
	}

	else if (c == cmds(CMDS_REG_FILE_MOVE))
	{
		gbl(scr_cur_cmd) = CMDS_REG_FILE_MOVE;
		if (check_read_only(msgs(m_filecmd2_mov)))
			return (0);
		do_move();
		if (gbl(scr_cur)->numfiles)
		{
			disp_cmds();
			return (0);
		}
	}

	else if (c == cmds(CMDS_COMMON_PRINT))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_PRINT;
		print_file();
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_QUIT))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_QUIT;
		gbl(scr_av_focus) = FALSE;
		if (quit())
		{
			for (i=0; i<gbl(scr_cur_count); i++)
			{
				if (gbl(scr_stat)[i].av_file_displayed)
				{
					view_command(gbl(scr_stat)[i].vfcb, V_CMD_CLOSE);
					gbl(scr_stat)[i].av_file_displayed = 0;
				}
			}
			all_done();
		}
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_RENAME))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_RENAME;
		if (check_read_only(msgs(m_filecmd2_ren)))
			return (0);
		do_rename();
		return (0);
	}

	else if (c == cmds(CMDS_VIEW_SRCH_FWD) ||
		c == cmds(CMDS_VIEW_SRCH_BCK))
	{
		if (c == cmds(CMDS_VIEW_SRCH_BCK))
			gbl(scr_cur_cmd) = CMDS_VIEW_SRCH_BCK;
		else
			gbl(scr_cur_cmd) = CMDS_VIEW_SRCH_FWD;

		if (gbl(srch_re))
		{
			FREE(gbl(srch_re));
			gbl(srch_re) = 0;
		}
		if (!gbl(srch_re))
		{
			gbl(srch_re) = get_search_str(msgs(m_avcmds_sea));
			disp_cmds();
		}
		v->v_re = gbl(srch_re);
		if (c == cmds(CMDS_VIEW_SRCH_BCK))
			view_command(v, V_SRCH_PREV);
		else
			view_command(v, V_SRCH_NEXT);
		disp_av_pct(TRUE);
		return (0);
	}

	else if (c == cmds(CMDS_VIEW_SRCH_NEXT) ||
		c == cmds(CMDS_VIEW_SRCH_NEXT2) ||
		c == cmds(CMDS_VIEW_SRCH_PREV) ||
		c == cmds(CMDS_VIEW_SRCH_PREV2))
	{
		if (c == cmds(CMDS_VIEW_SRCH_NEXT) ||
			c == cmds(CMDS_VIEW_SRCH_NEXT2))
			gbl(scr_cur_cmd) = CMDS_VIEW_SRCH_NEXT;
		else
			gbl(scr_cur_cmd) = CMDS_VIEW_SRCH_PREV;

		if (!gbl(srch_re))
		{
			gbl(srch_re) = get_search_str(msgs(m_avcmds_sea));
			disp_cmds();
			if (!gbl(srch_re))
				return (0);
		}
		v->v_re = gbl(srch_re);
		if (gbl(scr_cur_cmd) == CMDS_VIEW_SRCH_NEXT)
			view_command(v, V_SRCH_NEXT);
		else
			view_command(v, V_SRCH_PREV);
		disp_av_pct(TRUE);
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_TAG))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_TAG;
		gbl(scr_av_focus) = FALSE;
		do_cmn_reg_file_tag();
		return (' ');
	}

	else if (c == cmds(CMDS_COMMON_UNTAG))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_UNTAG;
		gbl(scr_av_focus) = FALSE;
		do_cmn_reg_file_untag();
		return (' ');
	}

	else if (c == cmds(CMDS_COMMON_VIEW))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_VIEW;
		gbl(scr_av_focus) = FALSE;
		do_viewer(gbl(scr_cur)->cur_file);
		return (0);
	}

	else if (c == cmds(CMDS_WIN_TABWIDTH))
	{
		gbl(scr_cur_cmd) = CMDS_WIN_TABWIDTH;
		{
			int i;

			opt(tab_width) = (opt(tab_width) == 8 ? 4 : 8);

			gbl(vfcb_av)->v_tab_width = opt(tab_width);
			gbl(vfcb_fv)->v_tab_width = opt(tab_width);
			for (i=0; i<gbl(scr_cur_count); i++)
			{
				gbl(vfcb_av_split)[i]->v_tab_width = opt(tab_width);
				v_display(gbl(scr_stat)[i].vfcb);
			}
		}
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_EXEC))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_EXEC;
		do_exec();
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

	return (0);
}

int do_tag_av_cmd (int c)
{
	if (c == KEY_ESCAPE || c == KEY_RETURN)
	{
	}

	else if (c == cmds(CMDS_COMMON_ATTRS))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_ATTRS;
		if (!check_read_only(msgs(m_filecmd2_att)))
			do_tag_file_attr();
	}

	else if (c == cmds(CMDS_COMMON_COPY))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_COPY;
		if (!check_read_only(msgs(m_filecmd2_cop)))
			do_tag_copy();
	}

	else if (c == cmds(CMDS_COMMON_DELETE))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_DELETE;
		if (!check_read_only(msgs(m_filecmd2_del)))
		{
			do_del_files();
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
		gbl(scr_av_focus) = FALSE;
		do_cmn_tag_file_invert();
	}

	else if (c == cmds(CMDS_TAG_FILE_MOVE))
	{
		gbl(scr_cur_cmd) = CMDS_TAG_FILE_MOVE;
		if (!check_read_only(msgs(m_filecmd2_mov)))
		{
			do_move_files();
			if (!gbl(scr_cur)->numfiles)
			{
				gbl(scr_cur)->cmd_sub_mode = m_reg;
				return (KEY_RETURN);
			}
		}
	}

	else if (c == cmds(CMDS_COMMON_PRINT))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_PRINT;
		do_tag_print();
	}

	else if (c == cmds(CMDS_COMMON_RENAME))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_RENAME;
		if (!check_read_only(msgs(m_filecmd2_ren)))
			do_tag_rename();
	}

	else if (c == cmds(CMDS_TAG_FILE_SEARCH))
	{
		gbl(scr_cur_cmd) = CMDS_TAG_FILE_SEARCH;
		gbl(scr_av_focus) = FALSE;
		do_search();
	}

	else if (c == cmds(CMDS_COMMON_TAG))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_TAG;
		gbl(scr_av_focus) = FALSE;
		do_cmn_tag_file_tag();
	}

	else if (c == cmds(CMDS_COMMON_UNTAG))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_UNTAG;
		gbl(scr_av_focus) = FALSE;
		do_cmn_tag_file_untag();
	}

	else if (c == cmds(CMDS_COMMON_VIEW))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_VIEW;
		do_tag_viewer();
	}

	else if (c == cmds(CMDS_TAG_MENU_1) || c == cmds(CMDS_TAG_MENU_2))
	{
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
	}
	gbl(scr_cur)->cmd_sub_mode = m_reg;
	disp_cmds();
	return (0);
}

int do_alt_av_cmd (int c)
{
	if (c == KEY_ESCAPE || c == KEY_RETURN)
	{
	}

	if (c == cmds(CMDS_COMMON_INVERT))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_INVERT;
		gbl(scr_av_focus) = FALSE;
		do_cmn_alt_file_invert();
	}

	else if (c == cmds(CMDS_COMMON_QUIT))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_QUIT;
		gbl(scr_av_focus) = FALSE;
		if (alt_quit())
		{
			int i;

			for (i=0; i<gbl(scr_cur_count); i++)
			{
				if (gbl(scr_stat)[i].av_file_displayed)
				{
					view_command(gbl(scr_stat)[i].vfcb, V_CMD_CLOSE);
					gbl(scr_stat)[i].av_file_displayed = 0;
				}
			}
			all_done();
		}
	}

	if (c == cmds(CMDS_COMMON_TAG))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_TAG;
		gbl(scr_av_focus) = FALSE;
		do_cmn_alt_file_tag();
	}

	if (c == cmds(CMDS_COMMON_UNTAG))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_UNTAG;
		gbl(scr_av_focus) = FALSE;
		do_cmn_alt_file_untag();
	}

	if (c == cmds(CMDS_TAG_MENU_1) || c == cmds(CMDS_TAG_MENU_2))
	{
		gbl(scr_cur)->cmd_sub_mode = m_tag;
		disp_cmds();
		return (0);
	}

	if (c == cmds(CMDS_ALT_MENU_1) || c == cmds(CMDS_ALT_MENU_2))
	{
		return (0);
	}

	else
	{
		/* beep? */
	}
	gbl(scr_cur)->cmd_sub_mode = m_reg;
	disp_cmds();
	return (0);
}

int do_reg_fv_cmd (int c)
{
	char edit_buffer[MAX_PATHLEN];
	VFCB *v;
	FBLK *f;

	if (c == cmds(CMDS_VIEW_WINDOW))
	{
		c = get_disp_mode();
		if (c == KEY_ESCAPE || c == KEY_BS)
			return (0);
		c = do_mode_cmd(c);
		if (!c)
			return (0);
	}

	v = gbl(scr_cur)->vfcb;
	if (c == KEY_ESCAPE ||
		c == KEY_BS ||
		c == KEY_RETURN ||
		c == cmds(CMDS_COMMON_QUIT))
	{
	}

	else if (c == cmds(CMDS_COMMON_VIEW))
	{
		if ((gbl(scr_cur)->vfcb)->v_mode & V_SYM)
		{
			view_command(v, V_CMD_CLOSE);
			(gbl(scr_cur)->vfcb)->v_mode &= ~V_SYM;
			view_command(v, V_CMD_OPEN_RO);
			view_command(v, V_CMD_REDISPLAY);
			return (0);
		}
	}

	else if (c == cmds(CMDS_VIEW_NEXTFILE))
	{
		gbl(scr_cur_cmd) = CMDS_VIEW_NEXTFILE;
		if (gbl(view_list))
		{
			f = 0;
			for (gbl(view_list)=bnext(gbl(view_list)); gbl(view_list);
				gbl(view_list)=bnext(gbl(view_list)))
			{
				f = (FBLK *)bid(gbl(view_list));
				if (is_file_tagged(f))
					break;
			}
			if (gbl(view_list))
			{
				view_command(v, V_CMD_CLOSE);
				fblk_to_pathname(f, edit_buffer);
				strcpy(v->v_pathname, edit_buffer);
				v->v_mode &= ~V_SYM;
				if (S_ISLNK(f->stbuf.st_mode))
					v->v_mode |= V_SYM;
				view_command(v, V_CMD_OPEN_RO);
				setup_display();
			}
			else
			{
				goto normal;
			}
		}
		return (0);
	}

	else if (c == cmds(CMDS_VIEW_PREVFILE))
	{
		gbl(scr_cur_cmd) = CMDS_VIEW_PREVFILE;
		if (gbl(view_list))
		{
			f = 0;
			for (gbl(view_list)=bprev(gbl(view_list)); gbl(view_list);
				gbl(view_list)=bprev(gbl(view_list)))
			{
				f = (FBLK *)bid(gbl(view_list));
				if (is_file_tagged(f))
					break;
			}
			if (gbl(view_list))
			{
				view_command(v, V_CMD_CLOSE);
				fblk_to_pathname(f, edit_buffer);
				strcpy(v->v_pathname, edit_buffer);
				v->v_mode &= ~V_SYM;
				if (S_ISLNK(f->stbuf.st_mode))
					v->v_mode |= V_SYM;
				view_command(v, V_CMD_OPEN_RO);
				setup_display();
			}
			else
			{
				goto normal;
			}
		}
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_EDIT))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_EDIT;
		do_editor(gbl(scr_cur)->cur_file);
		close_av_file();
		open_av_file(gbl(scr_cur)->cur_file);
		display_av();
		return (0);
	}

	else if (c == cmds(CMDS_VIEW_HEXEDIT))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_EDIT;
		close_av_file();
		do_hexedit(gbl(scr_cur)->cur_file);
		return (0);
	}

	else if (c == cmds(CMDS_COMMON_PRINT))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_PRINT;
		print_file();
		return (0);
	}

#if 0
	else if (c == cmds(CMDS_COMMON_QUIT))
	{
		gbl(scr_cur_cmd) = CMDS_COMMON_QUIT;
		if (quit())
		{
			close_av_file();
			all_done();
		}
		return (0);
	}
#endif

	else if (c == cmds(CMDS_VIEW_SRCH_FWD) || c == cmds(CMDS_VIEW_SRCH_BCK))
	{
		if (c == cmds(CMDS_VIEW_SRCH_BCK))
			gbl(scr_cur_cmd) = CMDS_VIEW_SRCH_BCK;
		else
			gbl(scr_cur_cmd) = CMDS_VIEW_SRCH_FWD;

		if (gbl(srch_re))
		{
			FREE(gbl(srch_re));
			gbl(srch_re) = 0;
		}
		if (!gbl(srch_re))
		{
			gbl(srch_re) = get_search_str(msgs(m_avcmds_sea));
			disp_cmds();
			if (!gbl(srch_re))
				return (0);
		}
		v->v_re = gbl(srch_re);
		if (c == cmds(CMDS_VIEW_SRCH_BCK))
			view_command(v, V_SRCH_PREV);
		else
			view_command(v, V_SRCH_NEXT);
		disp_av_pct(TRUE);
		return (0);
	}

	else if (c == cmds(CMDS_VIEW_SRCH_NEXT) ||
		c == cmds(CMDS_VIEW_SRCH_NEXT2) ||
		c == cmds(CMDS_VIEW_SRCH_PREV) ||
		c == cmds(CMDS_VIEW_SRCH_PREV2))
	{
		if (c == cmds(CMDS_VIEW_SRCH_NEXT) ||
			c == cmds(CMDS_VIEW_SRCH_NEXT2))
			gbl(scr_cur_cmd) = CMDS_VIEW_SRCH_NEXT;
		else
			gbl(scr_cur_cmd) = CMDS_VIEW_SRCH_PREV;

		if (!gbl(srch_re))
		{
			gbl(srch_re) = get_search_str(msgs(m_avcmds_sea));
			disp_cmds();
		}
		v->v_re = gbl(srch_re);
		if (gbl(scr_cur_cmd) == CMDS_VIEW_SRCH_NEXT)
			view_command(v, V_SRCH_NEXT);
		else
			view_command(v, V_SRCH_PREV);
		disp_av_pct(TRUE);
		return (0);
	}

	else if (c == cmds(CMDS_WIN_TABWIDTH))
	{
		gbl(scr_cur_cmd) = CMDS_WIN_TABWIDTH;
		{
			int i;

			opt(tab_width) = (opt(tab_width) == 8 ? 4 : 8);

			gbl(vfcb_av)->v_tab_width = opt(tab_width);
			gbl(vfcb_fv)->v_tab_width = opt(tab_width);
			for (i=0; i<gbl(scr_cur_count); i++)
			{
				gbl(vfcb_av_split)[i]->v_tab_width = opt(tab_width);
				v_display(gbl(scr_stat)[i].vfcb);
			}
		}
		return (0);
	}

	else
	{
		return (0);
	}

	/* reset back to normal */

normal:
	gbl(view_list) = 0;
	gbl(scr_in_fullview) = FALSE;
	if (gbl(scr_is_split))
	{
		gbl(scr_cur)->vfcb = gbl(vfcb_av_split)[gbl(scr_cur_no)];
		gbl(scr_cur)->cur_av_mode_win =
			gbl(win_av_mode_split)[gbl(scr_cur_no)];
		if (gbl(scr_in_autoview))
		{
			gbl(scr_cur)->cur_path_line =
				gbl(win_av_path_line_split)[gbl(scr_cur_no)];
		}
		else
		{
			gbl(scr_cur)->cur_path_line =
				gbl(win_path_line_split)[gbl(scr_cur_no)];
		}
	}
	else
	{
		gbl(scr_cur)->vfcb = gbl(vfcb_av);
	}
	if (gbl(scr_in_autoview))
	{
		gbl(scr_cur)->vfcb->v_top_pos = v->v_top_pos;
		gbl(scr_cur)->vfcb->v_mode =
			(v->v_mode & ~V_HSCROLLBAR) | V_VSCROLLBAR;
	}
	view_command(v, V_CMD_CLOSE);
	setup_display();
	disp_cmds();
	return (0);
}
