/*------------------------------------------------------------------------
 * Routine to display the prompt line
 * on the bottom of the screen.
 */
#include "libprd.h"

void disp_prompt_line (void)
{
	int reg_fs;
	int ftp_fs;

	reg_fs = ( (gbl(scr_cur)->cur_root)->node_type == N_FS );
	ftp_fs = ( (gbl(scr_cur)->cur_root)->node_type == N_FTP );

	werase(gbl(win_message));
	wmove(gbl(win_message), 0, 0);

	if (gbl(scr_in_hexedit))
	{
		disp_cmds();

		esc_msg();
		wmove(gbl(win_message), 0, 0);
		xaddstr(gbl(win_message), msgs(m_prompt_arrpos));
	}

	else if (gbl(scr_in_fullview))
	{
		esc_msg();
		wmove(gbl(win_message), 0, 0);
#if 0
		xaddstr(gbl(win_message), msgs(m_prompt_arrscr));
		xaddstr(gbl(win_message), msgs(m_prompt_bl2));
#endif
		if (gbl(view_list))
		{
			fk_msg(gbl(win_message), CMDS_VIEW_PREVFILE, msgs(m_prompt_pfile));
			xaddstr(gbl(win_message), msgs(m_prompt_bl2));
			fk_msg(gbl(win_message), CMDS_VIEW_NEXTFILE, msgs(m_prompt_nfile));
			xaddstr(gbl(win_message), msgs(m_prompt_bl2));
		}
		fk_msg(gbl(win_message), CMDS_VIEW_SRCH_PREV, msgs(m_prompt_psrch));
		xaddstr(gbl(win_message), msgs(m_prompt_bl2));
		fk_msg(gbl(win_message), CMDS_VIEW_SRCH_NEXT, msgs(m_prompt_nsrch));
	}

	else if (gbl(scr_in_autoview))
	{
		esc_msg();
		wmove(gbl(win_message), 0, 0);
		switch (gbl(scr_cur)->cmd_sub_mode)
		{
		case m_reg:
			fk_msg(gbl(win_message), CMDS_TAG_MENU_1, msgs(m_prompt_f32));
			xaddstr(gbl(win_message), msgs(m_prompt_bl2));
			fk_msg(gbl(win_message), CMDS_ALT_MENU_1, msgs(m_prompt_f42));
			xaddstr(gbl(win_message), msgs(m_prompt_bl2));
			if (gbl(scr_is_split))
			{
				fk_msg(gbl(win_message), CMDS_SPLIT_SCREEN,
					msgs(m_prompt_f8u));
			}
			else
			{
				fk_msg(gbl(win_message), CMDS_SPLIT_SCREEN,
					msgs(m_prompt_f8s));
			}
			xaddstr(gbl(win_message), msgs(m_prompt_bl2));
			fk_msg(gbl(win_message), CMDS_VIEW_SRCH_PREV,
				msgs(m_prompt_psrch));
			xaddstr(gbl(win_message), msgs(m_prompt_bl2));
			fk_msg(gbl(win_message), CMDS_VIEW_SRCH_NEXT,
				msgs(m_prompt_nsrch));
			break;
		case m_tag:
			fk_msg(gbl(win_message), CMDS_ALT_MENU_1, msgs(m_prompt_f4));
			xaddstr(gbl(win_message), msgs(m_prompt_bl2));
			if (gbl(scr_is_split))
			{
				fk_msg(gbl(win_message), CMDS_SPLIT_SCREEN,
					msgs(m_prompt_f8u));
			}
			else
			{
				fk_msg(gbl(win_message), CMDS_SPLIT_SCREEN,
					msgs(m_prompt_f8s));
			}
			break;
		case m_alt:
			fk_msg(gbl(win_message), CMDS_TAG_MENU_1, msgs(m_prompt_f3));
			xaddstr(gbl(win_message), msgs(m_prompt_bl2));
			if (gbl(scr_is_split))
			{
				fk_msg(gbl(win_message), CMDS_SPLIT_SCREEN,
					msgs(m_prompt_f8u));
			}
			else
			{
				fk_msg(gbl(win_message), CMDS_SPLIT_SCREEN,
					msgs(m_prompt_f8s));
			}
			break;
		}
	}

	else if (gbl(scr_in_diffview))
	{
		esc_msg();
		wmove(gbl(win_message), 0, 0);
		fk_msg(gbl(win_message), CMDS_DIFF_EDIT_BOTH,
			msgs(m_prompt_diffeditb));
		xaddstr(gbl(win_message), msgs(m_prompt_bl2));
		fk_msg(gbl(win_message), CMDS_DIFF_EDIT_LEFT,
			msgs(m_prompt_diffeditl));
		xaddstr(gbl(win_message), msgs(m_prompt_bl2));
		fk_msg(gbl(win_message), CMDS_DIFF_EDIT_RIGHT,
			msgs(m_prompt_diffeditr));
		xaddstr(gbl(win_message), msgs(m_prompt_bl2));
		fk_msg(gbl(win_message), CMDS_DIFF_VIEW_LEFT,
			msgs(m_prompt_diffviewl));
		xaddstr(gbl(win_message), msgs(m_prompt_bl2));
		fk_msg(gbl(win_message), CMDS_DIFF_VIEW_RIGHT,
			msgs(m_prompt_diffviewr));
	}

	else
	{
		if (gbl(scr_cur)->command_mode == m_dir)
		{
			switch (gbl(scr_cur)->cmd_sub_mode)
			{
			case m_reg:
				ret_msg(gbl(win_message));
				xcaddstr(gbl(win_message), CMDS_RETURN, msgs(m_prompt_fil));
				xaddstr(gbl(win_message), msgs(m_prompt_bl2));
				fk_msg(gbl(win_message), CMDS_TAG_MENU_1, msgs(m_prompt_f32));
				xaddstr(gbl(win_message), msgs(m_prompt_bl2));
				fk_msg(gbl(win_message), CMDS_ALT_MENU_1, msgs(m_prompt_f42));
				xaddstr(gbl(win_message), msgs(m_prompt_bl2));
				fk_msg(gbl(win_message), CMDS_REG_DIR_HIDE, msgs(m_prompt_f5));
				xaddstr(gbl(win_message), msgs(m_prompt_bl2));
				fk_msg(gbl(win_message), CMDS_REG_DIR_SHOW,
					msgs(m_prompt_f62));
				if (reg_fs)
				{
					xaddstr(gbl(win_message), msgs(m_prompt_bl2));
					fk_msg(gbl(win_message), CMDS_AUTOVIEW, msgs(m_prompt_f7));
				}
				xaddstr(gbl(win_message), msgs(m_prompt_bl2));
				if (gbl(scr_is_split))
				{
					fk_msg(gbl(win_message), CMDS_SPLIT_SCREEN,
						msgs(m_prompt_f8u));
				}
				else
				{
					fk_msg(gbl(win_message), CMDS_SPLIT_SCREEN,
						msgs(m_prompt_f8s));
				}
				break;
			case m_tag:
				fk_msg(gbl(win_message), CMDS_ALT_MENU_1, msgs(m_prompt_f4));
				xaddstr(gbl(win_message), msgs(m_prompt_bl2));
				if (gbl(scr_is_split))
				{
					fk_msg(gbl(win_message), CMDS_SPLIT_SCREEN,
						msgs(m_prompt_f8u));
				}
				else
				{
					fk_msg(gbl(win_message), CMDS_SPLIT_SCREEN,
						msgs(m_prompt_f8s));
				}
				break;
			case m_alt:
				fk_msg(gbl(win_message), CMDS_TAG_MENU_1, msgs(m_prompt_f3));
				xaddstr(gbl(win_message), msgs(m_prompt_bl2));
				fk_msg(gbl(win_message), CMDS_ALT_DIR_HIDE, msgs(m_prompt_f5));
				xaddstr(gbl(win_message), msgs(m_prompt_bl2));
				fk_msg(gbl(win_message), CMDS_ALT_DIR_SHOW,
					msgs(m_prompt_f62));
				xaddstr(gbl(win_message), msgs(m_prompt_bl2));
				if (gbl(scr_is_split))
				{
					fk_msg(gbl(win_message), CMDS_SPLIT_SCREEN,
						msgs(m_prompt_f8u));
				}
				else
				{
					fk_msg(gbl(win_message), CMDS_SPLIT_SCREEN,
						msgs(m_prompt_f8s));
				}
				if (reg_fs)
				{
					xaddstr(gbl(win_message), msgs(m_prompt_bl2));
					fk_msg(gbl(win_message), CMDS_ALT_DIR_CONFIG,
						msgs(m_prompt_f10));
				}
				break;
			}
		}
		else
		{
			switch (gbl(scr_cur)->cmd_sub_mode)
			{
			case m_reg:
				ret_msg(gbl(win_message));
				if (gbl(scr_cur)->in_small_window)
				{
					xcaddstr(gbl(win_message), CMDS_RETURN,
						msgs(m_prompt_more));
				}
				else
				{
					xcaddstr(gbl(win_message), CMDS_RETURN,
						msgs(m_prompt_tree));
				}
				xaddstr(gbl(win_message), msgs(m_prompt_bl2));
				fk_msg(gbl(win_message), CMDS_TAG_MENU_1, msgs(m_prompt_f3));
				xaddstr(gbl(win_message), msgs(m_prompt_bl2));
				fk_msg(gbl(win_message), CMDS_ALT_MENU_1, msgs(m_prompt_f4));
				if (reg_fs)
				{
					xaddstr(gbl(win_message), msgs(m_prompt_bl2));
					fk_msg(gbl(win_message), CMDS_AUTOVIEW, msgs(m_prompt_f7));
				}
				xaddstr(gbl(win_message), msgs(m_prompt_bl2));
				if (gbl(scr_is_split))
				{
					fk_msg(gbl(win_message), CMDS_SPLIT_SCREEN,
						msgs(m_prompt_f8u));
				}
				else
				{
					fk_msg(gbl(win_message), CMDS_SPLIT_SCREEN,
						msgs(m_prompt_f8s));
				}
				if (bnext(gbl(nodes_list)) || bprev(gbl(nodes_list)))
				{
					waddstr(gbl(win_message), msgs(m_prompt_bl2));
					xcaddstr(gbl(win_message), CMDS_COMMON_NEXT_NODE_1,
						msgs(m_cmds_select));
				}
#if 0
				esc_msg();
#endif
				break;
			case m_tag:
				fk_msg(gbl(win_message), CMDS_ALT_MENU_1, msgs(m_prompt_f4));
				xaddstr(gbl(win_message), msgs(m_prompt_bl2));
				if (gbl(scr_is_split))
				{
					fk_msg(gbl(win_message), CMDS_SPLIT_SCREEN,
						msgs(m_prompt_f8u));
				}
				else
				{
					fk_msg(gbl(win_message), CMDS_SPLIT_SCREEN,
						msgs(m_prompt_f8s));
				}
				break;
			case m_alt:
				if (reg_fs || ftp_fs)
				{
					fk_msg(gbl(win_message), CMDS_ALT_FILE_RELOG,
						msgs(m_prompt_f2));
					xaddstr(gbl(win_message), msgs(m_prompt_bl2));
				}
				fk_msg(gbl(win_message), CMDS_TAG_MENU_1, msgs(m_prompt_f3));
				if (reg_fs)
				{
					xaddstr(gbl(win_message), msgs(m_prompt_bl2));
					fk_msg(gbl(win_message), CMDS_ALT_FILE_COMPRESS,
						msgs(m_prompt_comp));
				}
				if (gbl(scr_is_split))
				{
					xaddstr(gbl(win_message), msgs(m_prompt_bl2));
					fk_msg(gbl(win_message), CMDS_ALT_FILE_COPYTAGS,
						msgs(m_prompt_f61));
				}
				xaddstr(gbl(win_message), msgs(m_prompt_bl2));
				if (gbl(scr_is_split))
				{
					fk_msg(gbl(win_message), CMDS_SPLIT_SCREEN,
						msgs(m_prompt_f8u));
				}
				else
				{
					fk_msg(gbl(win_message), CMDS_SPLIT_SCREEN,
						msgs(m_prompt_f8s));
				}
				if ((reg_fs || ftp_fs) &&
				    S_ISLNK((gbl(scr_cur)->cur_file)->stbuf.st_mode))
				{
					xaddstr(gbl(win_message), msgs(m_prompt_bl2));
					fk_msg(gbl(win_message), CMDS_ALT_FILE_RESOLVE,
						msgs(m_prompt_f10b));
				}
				break;
			}
		}
	}

	if (! gbl(scr_in_diffview))
	{
		xaddstr(gbl(win_message), msgs(m_prompt_bl2));
		fk_msg(gbl(win_message), CMDS_INFO, msgs(m_prompt_qi));
	}

	leaveok(gbl(win_message), TRUE);
	wrefresh(gbl(win_message));
	leaveok(gbl(win_message), FALSE);
}
