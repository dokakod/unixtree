/*------------------------------------------------------------------------
 * do dest-dir window processing
 */
#include "libprd.h"

struct dest_info
{
	int		dest_ntype;
	DSTAT *	dest_save_cd;
	DSTAT	dest_scr_stat;
};
typedef struct dest_info DEST_INFO;

static void init_dest_dir (DEST_INFO *dsti)
{
	BLIST *b;
	NBLK *n;
	int i;

	if (gbl(scr_in_diffview))
	{
	}
	else if (!gbl(scr_in_autoview))
	{
		if (gbl(scr_is_split))
		{
			wmove(gbl(win_border), 1, getbegx(gbl(scr_cur)->cur_file_win));
			for (i=0; i<getmaxx(gbl(scr_cur)->cur_file_win); i++)
				wbox_chr(gbl(win_border), B_HO, B_DVDH);
			wrefresh(gbl(win_border));
		}
		if (gbl(scr_cur)->command_mode == m_dir ||
			gbl(scr_cur)->in_small_window)
		{
			dir_scroll_bar_remove();
		}
		file_scroll_bar_remove();
	}
	else
	{
		if (gbl(scr_is_split))
		{
			wmove(gbl(win_border), 1, getbegx(gbl(scr_cur)->cur_file_win));
			for (i=0; i<getmaxx(gbl(scr_cur)->cur_file_win); i++)
				wbox_chr(gbl(win_border), B_HO, B_DVDH);
			wrefresh(gbl(win_border));
		}
	}

	dsti->dest_ntype = (gbl(scr_cur)->cur_root)->node_type;
	if (dsti->dest_ntype != N_FS)
	{
		switch (gbl(scr_cur_cmd))
		{
		case CMDS_COMMON_TAG:
		case CMDS_COMMON_UNTAG:
		case CMDS_COMMON_INVERT:
			break;
		default:
			dsti->dest_ntype = N_FS;
			break;
		}
	}

	copy_dstat(gbl(scr_cur), &dsti->dest_scr_stat);
	dsti->dest_save_cd = gbl(scr_cur);
	gbl(scr_cur) = &dsti->dest_scr_stat;
	gbl(scr_cur)->cur_dir_win     = dsti->dest_save_cd->cur_dest_dir_win;
	gbl(scr_cur)->large_file_win  = dsti->dest_save_cd->large_file_win;
	gbl(scr_cur)->small_file_win  = dsti->dest_save_cd->small_file_win;
	gbl(scr_cur)->cur_file_win    = dsti->dest_save_cd->cur_file_win;
	gbl(scr_cur)->cur_path_line   = dsti->dest_save_cd->cur_path_line;
	gbl(scr_cur)->cur_av_view_win = dsti->dest_save_cd->cur_av_view_win;

	gbl(scr_cur)->max_dir_line  = getmaxy(gbl(scr_cur)->cur_dir_win)-1;
	gbl(scr_cur)->max_dir_level = get_max_dir_level(gbl(scr_cur)->cur_dir_win);

	set_file_disp();
	set_dir_disp();
	gbl(scr_cur)->in_dest_dir = TRUE;
	gbl(scr_cur)->command_mode = m_dir;				/* fake it */

	dest_border();

	if (dsti->dest_ntype != (int)(gbl(scr_cur)->cur_root)->node_type)
	{
		for (b=gbl(nodes_list); b; b=bnext(b))
		{
			n = (NBLK *)bid(b);
			if (n->node_type == dsti->dest_ntype)
			{
				gbl(scr_cur)->cur_node = b;
				gbl(scr_cur)->cur_root = n;
				break;
			}
		}
	}
	init_disp();
}

static void end_dest_dir (DEST_INFO *dsti)
{
	if (gbl(scr_cur)->command_mode == m_dir ||
		gbl(scr_cur)->command_mode == m_file)
	{
		wattrset(gbl(scr_cur)->large_file_win, gbl(scr_reg_hilite_attr));
	}
	else
	{
		wattrset(gbl(scr_cur)->large_file_win, gbl(scr_sa_hilite_attr));
	}

	gbl(scr_cur)->max_dir_level++;
	gbl(scr_cur) = dsti->dest_save_cd;
	is_screen_kosher(gbl(scr_cur_no));

	if (! gbl(scr_in_diffview))
	{
		disp_path_line();
		disp_file_box();
	}

	if (gbl(scr_in_autoview))
	{
		display_av();
	}
	else if (gbl(scr_in_diffview))
	{
		diff_view_setup();
	}
	else if (gbl(scr_cur)->command_mode == m_dir)
	{
		disp_dir_tree();
		dir_scroll_bar_init();
		do_dir_scroll_bar();
		small_border(ON);
		werase(gbl(scr_cur)->cur_file_win);
		check_small_window();
		hilite_dir(ON);
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
		werase(gbl(scr_cur)->cur_file_win);
		disp_file_list();
		file_scroll_bar_init();
		do_file_scroll_bar();
		hilite_file(ON);
	}
}

static void dest_bang_line (int show_opt)
{
	werase(gbl(win_message));
	esc_msg();
	wrefresh(gbl(win_message));
	wmove(gbl(win_message), 0, 0);
	xaddstr(gbl(win_message), msgs(m_dest_arrscr));
	if (bprev(gbl(scr_cur)->cur_node) || bnext(gbl(scr_cur)->cur_node))
	{
		waddstr(gbl(win_message), "  ");
		xcaddstr(gbl(win_message), CMDS_COMMON_NEXT_NODE_1,
			msgs(m_dest_select));
	}
	waddstr(gbl(win_message), "  ");
	if (show_opt)
	{
		fk_msg(gbl(win_message), CMDS_DEST_DIR, msgs(m_dest_chodir));
		waddstr(gbl(win_message), "  ");
	}
	xcaddstr(gbl(win_message), CMDS_COMMON_NODE, msgs(m_cmds_node));
	waddstr(gbl(win_message), "  ");
	ret_ok();
	wrefresh(gbl(win_message));
}

int do_dest_dir (char *path, int show_opt, TREE **dtptr)
{
	DEST_INFO	dest_info;
	DEST_INFO *	dsti = &dest_info;
	chtype save_bang_str[128];
	int c;
	int x, y;
	NBLK *n;
	int m;
	TREE *t;

	bang_save(save_bang_str);

	x = getcurx(gbl(win_commands));
	y = getcury(gbl(win_commands));
	init_dest_dir(dsti);
	*dtptr = gbl(scr_cur)->cur_dir_tree;
	dptr_to_dirname(gbl(scr_cur)->dir_cursor, path);
	dest_bang_line(show_opt);

	wmove(gbl(win_commands), y, x);
	wclrtoeol(gbl(win_commands));
	waddstr(gbl(win_commands), path);
	wrefresh(gbl(win_commands));

	while (TRUE)
	{
		position_cursor();
		wrefresh(stdscr);
		c = xgetch(stdscr);

		if (c == KEY_ESCAPE)
		{
			end_dest_dir(dsti);
			bang_restore(save_bang_str);
			return (-1);
		}

		if (c == cmds(CMDS_DEST_DIR))
		{
			*dtptr = gbl(scr_cur)->cur_dir_tree;
			end_dest_dir(dsti);
			bang_restore(save_bang_str);
			return (1);
		}

		m = 0;
		if (c == KEY_MOUSE)
		{
			m = mouse_get_event(gbl(win_commands));
			if (m == MOUSE_LBD || m == MOUSE_DRAG || m == MOUSE_LBDBL)
			{
				t = gbl(scr_cur)->cur_dir_tree;
				c = check_mouse_pos();
				if (c == KEY_MOUSE && t != gbl(scr_cur)->cur_dir_tree)
					c = 0;
			}
		}

		if (c == cmds(CMDS_COMMON_NODE))
		{
			char input_str[MAX_PATHLEN];

			bang(msgs(m_newnode_entlog));
			*input_str = 0;
			c = xgetstr(gbl(win_message), input_str, XGL_NODE, MAX_PATHLEN, 0,
				XG_PATHNAME);
			if (c > 0)
			{
				fn_resolve_pathname(input_str);
				do_new_log(input_str);
			}

			*dtptr = gbl(scr_cur)->cur_dir_tree;
			dptr_to_dirname(gbl(scr_cur)->dir_cursor, path);
			wmove(gbl(win_commands), y, x);
			wclrtoeol(gbl(win_commands));
			waddstr(gbl(win_commands), path);
			wrefresh(gbl(win_commands));
			dest_bang_line(show_opt);

			continue;
		}

		if (c == cmds(CMDS_REG_DIR_RELOG))
		{
			relog_dir(FALSE);
			continue;
		}

		if (c == cmds(CMDS_REG_DIR_LOG_RECURS))
		{
			relog_dir(TRUE);
			continue;
		}

		if (c == cmds(CMDS_REG_DIR_UNLOG))
		{
			unlog_dir();
			continue;
		}

		if (c == cmds(CMDS_REG_DIR_HIDE))
		{
			hide_sub_node(ON);
			continue;
		}

		if (c == cmds(CMDS_REG_DIR_SHOW))
		{
			hide_sub_node(OFF);
			continue;
		}

		if (c == cmds(CMDS_COMMON_GOTO2))
		{
			goto_dir();
			*dtptr = gbl(scr_cur)->cur_dir_tree;
			dptr_to_dirname(gbl(scr_cur)->dir_cursor, path);
			wmove(gbl(win_commands), y, x);
			wclrtoeol(gbl(win_commands));
			waddstr(gbl(win_commands), path);
			wrefresh(gbl(win_commands));
			dest_bang_line(show_opt);
			continue;
		}

		c = do_dir_cmd(c);
		if (!c)
		{
			*dtptr = gbl(scr_cur)->cur_dir_tree;
			dptr_to_dirname(gbl(scr_cur)->dir_cursor, path);
			wmove(gbl(win_commands), y, x);
			wclrtoeol(gbl(win_commands));
			waddstr(gbl(win_commands), path);
			wrefresh(gbl(win_commands));
			continue;
		}

		if (c == cmds(CMDS_COMMON_NEXT_NODE_1) ||
			c == cmds(CMDS_COMMON_NEXT_NODE_2))
		{
			if (bprev(gbl(scr_cur)->cur_node) || bnext(gbl(scr_cur)->cur_node))
			{
				dir_scroll_bar_off();
				while (TRUE)
				{
					gbl(scr_cur)->cur_node = bnextc(gbl(scr_cur)->cur_node);
					n = (NBLK *)bid(gbl(scr_cur)->cur_node);
					if (n->node_type == dsti->dest_ntype)
						break;
				}
				init_disp();
				dptr_to_dirname(gbl(scr_cur)->dir_cursor, path);
				wmove(gbl(win_commands), y, x);
				wclrtoeol(gbl(win_commands));
				waddstr(gbl(win_commands), path);
				wrefresh(gbl(win_commands));
				continue;
			}
		}

		if (c == cmds(CMDS_COMMON_PREV_NODE_1) ||
			c == cmds(CMDS_COMMON_PREV_NODE_2))
		{
			if (bprev(gbl(scr_cur)->cur_node) || bnext(gbl(scr_cur)->cur_node))
			{
				dir_scroll_bar_off();
				while (TRUE)
				{
					gbl(scr_cur)->cur_node = bprevc(gbl(scr_cur)->cur_node);
					n = (NBLK *)bid(gbl(scr_cur)->cur_node);
					if (n->node_type == dsti->dest_ntype)
						break;
				}
				init_disp();
				dptr_to_dirname(gbl(scr_cur)->dir_cursor, path);
				wmove(gbl(win_commands), y, x);
				wclrtoeol(gbl(win_commands));
				waddstr(gbl(win_commands), path);
				wrefresh(gbl(win_commands));
				continue;
			}
		}

		if (c == KEY_RETURN)
		{
			*dtptr = gbl(scr_cur)->cur_dir_tree;
			end_dest_dir(dsti);
			bang_restore(save_bang_str);
			return (0);
		}
	}
	/*NOTREACHED*/
}
