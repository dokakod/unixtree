/*------------------------------------------------------------------------
 * "diff" cmd routines
 */
#include "libprd.h"

static void diff_load_file (DIFF_LINE *dl)
{
	DIFF_INFO *	di = &gbl(diff_info);
	DIFF_LINE *	d;

	/*--------------------------------------------------------------------
	 * initialize everything
	 */
	di->diff_lines		= dl;

	di->diff_all_lines	= 0;
	di->diff_dif_lines	= 0;

	di->diff_all_cnt	= 0;
	di->diff_dif_cnt	= 0;

	/*--------------------------------------------------------------------
	 * now create 2 lists of line pointers (all & changed)
	 */
	for (d=dl; d; d=d->next)
	{
		di->diff_all_lines = bappend(di->diff_all_lines, BNEW(d));
		di->diff_all_cnt++;

		if (LINE_TYPE(d) != COMMON)
		{
			di->diff_dif_lines = bappend(di->diff_dif_lines, BNEW(d));
			di->diff_dif_cnt++;
		}
	}

	/*--------------------------------------------------------------------
	 * now set current list to one just created
	 */
	if (opt(diff_show_diff))
	{
		di->diff_cur_lines	= di->diff_dif_lines;
		di->diff_cur_cnt	= di->diff_dif_cnt;
	}
	else
	{
		di->diff_cur_lines	= di->diff_all_lines;
		di->diff_cur_cnt	= di->diff_all_cnt;
	}
}

static void diff_free_file (void)
{
	DIFF_INFO *	di = &gbl(diff_info);

	di->diff_lines		= diff_free_lines(di->diff_lines);

	di->diff_all_lines	= BSCRAP(di->diff_all_lines, FALSE);
	di->diff_dif_lines	= BSCRAP(di->diff_dif_lines, FALSE);
	di->diff_cur_lines	= 0;

	di->diff_all_cnt	= 0;
	di->diff_dif_cnt	= 0;
	di->diff_cur_cnt	= 0;

	gbl(scr_in_autoview) = di->diff_save_in_av;
	gbl(scr_in_fullview) = di->diff_save_in_fv;

	gbl(scr_in_diffview) = FALSE;
}

static void diff_refresh (void)
{
	wrefresh(gbl(win_diff_lf));
	wrefresh(gbl(win_diff_tp));
	wrefresh(gbl(win_diff_rt));
}

static void diff_display_cur_line (BLIST *bp, int lineno)
{
	DIFF_INFO *		di	= &gbl(diff_info);
	DIFF_LINE *		d	= (DIFF_LINE *)bid(bp);
	char *			lp	= LINE_PTR_LF(d);
	char *			rp	= LINE_PTR_RT(d);
	const char *	t;
	attr_t			attr;
	char			buf[2048];
	int				len;
	int				start;

	/*--------------------------------------------------------------------
	 * get type of entry & line attribute
	 */
	switch (LINE_TYPE(d))
	{
	case COMMON:	t = "  ";	attr = gbl(scr_diff_same_attr);	break;
	case LEFT:		t = "<-";	attr = gbl(scr_diff_lfrt_attr);	break;
	case RIGHT:		t = "->";	attr = gbl(scr_diff_lfrt_attr);	break;
	case CHANGED:	t = "<>";	attr = gbl(scr_diff_diff_attr);	break;
	default:		t = "??";	attr = gbl(scr_diff_same_attr);	break;
	}

	/*--------------------------------------------------------------------
	 * display left line
	 */
	wmove(gbl(win_diff_lf), lineno, 0);

	if (opt(diff_show_numbers))
	{
		wattrset(gbl(win_diff_lf), gbl(scr_diff_numb_attr));

		if (LINE_NUM_LF(d) > 0)
		{
			char dbuf[12];

			waddstr (gbl(win_diff_lf), xform2(dbuf, LINE_NUM_LF(d), 4));
			waddch  (gbl(win_diff_lf), ' ');
		}
		else
		{
			waddstr (gbl(win_diff_lf), "     ");
		}
	}

	len = str_expand_tabs(lp, opt(tab_width), buf);
	if (di->diff_start_col > 0)
		start = (di->diff_start_col < len ? di->diff_start_col : len);
	else
		start = 0;

	wattrset (gbl(win_diff_lf), attr);
	wclrtoeol(gbl(win_diff_lf));
	waddstr  (gbl(win_diff_lf), buf + start);

	/*--------------------------------------------------------------------
	 * display type
	 */
	wmove  (gbl(win_diff_tp), lineno, 0);
	waddstr(gbl(win_diff_tp), t);

	/*--------------------------------------------------------------------
	 * display right line
	 */
	wmove(gbl(win_diff_rt), lineno, 0);

	if (opt(diff_show_numbers))
	{
		wattrset(gbl(win_diff_rt), gbl(scr_diff_numb_attr));

		if (LINE_NUM_RT(d) > 0)
		{
			char dbuf[12];

			waddstr (gbl(win_diff_rt), xform2(dbuf, LINE_NUM_RT(d), 4));
			waddch  (gbl(win_diff_rt), ' ');
		}
		else
		{
			waddstr (gbl(win_diff_rt), "     ");
		}
	}

	len = str_expand_tabs(rp, opt(tab_width), buf);
	if (di->diff_start_col > 0)
		start = (di->diff_start_col < len ? di->diff_start_col : len);
	else
		start = 0;

	wattrset (gbl(win_diff_rt), attr);
	wclrtoeol(gbl(win_diff_rt));
	waddstr  (gbl(win_diff_rt), buf + start);

	wattrset(gbl(win_diff_lf), gbl(scr_diff_same_attr));
	wattrset(gbl(win_diff_rt), gbl(scr_diff_same_attr));
}

static void diff_display_list (void)
{
	DIFF_INFO *	di = &gbl(diff_info);
	BLIST *		b;
	int			i;

	/*--------------------------------------------------------------------
	 * erase all windows
	 */
	wattrset(gbl(win_diff_lf), gbl(scr_diff_same_attr));
	wattrset(gbl(win_diff_rt), gbl(scr_diff_same_attr));
	werase  (gbl(win_diff_lf));
	werase  (gbl(win_diff_rt));
	werase  (gbl(win_diff_tp));

	/*--------------------------------------------------------------------
	 * now do all lines
	 */
	for (i = 0, b = di->diff_top_line;
		i < getmaxy(gbl(win_diff_lf)) && b;
		i++, b = bnext(b))
	{
		diff_display_cur_line(b, i);
	}

	diff_refresh();
}

static void diff_edit (char *path1, char *path2)
{
	char	cmd[MAX_PATHLEN * 2];

	strcpy(cmd, opt(editor));
	strcat(cmd, " \"");
	strcat(cmd, path1);
	if (path2 != 0)
	{
		strcat(cmd, "\" \"");
		strcat(cmd, path2);
	}
	strcat(cmd, "\"");

	xsystem(cmd, "", FALSE);
}

void diff_display_init (void)
{
	DIFF_INFO *	di = &gbl(diff_info);

	di->diff_top_line	= di->diff_cur_lines;
	di->diff_top_lineno	= 0;
	di->diff_start_col	= 0;
}

int do_diff_cmd (int c)
{
	DIFF_INFO *	di = &gbl(diff_info);

	/*--------------------------------------------------------------------
	 * bail from diffview
	 */
	if (c == KEY_ESCAPE ||
		c == KEY_RETURN ||
		c == KEY_BS ||
		c == cmds(CMDS_COMMON_QUIT))
	{
		di->diff_start_col	= 0;
		di->diff_top_line	= 0;
		di->diff_top_lineno	= 0;

		diff_free_file();
		setup_display();
		disp_cmds();
	}

	/*--------------------------------------------------------------------
	 * movement keys
	 */
	else if (c == KEY_UP)
	{
		if (bprev(di->diff_top_line) != 0)
		{
			di->diff_top_line = bprev(di->diff_top_line);
			di->diff_top_lineno--;

			diff_display_list();
		}
	}

	else if (c == KEY_DOWN)
	{
		if (bnext(di->diff_top_line) != 0)
		{
			di->diff_top_line = bnext(di->diff_top_line);
			di->diff_top_lineno++;

			diff_display_list();
		}
	}

	else if (c == KEY_LEFT)
	{
		if (di->diff_start_col > 0)
		{
			di->diff_start_col--;
			diff_display_list();
		}
	}

	else if (c == KEY_RIGHT)
	{
		di->diff_start_col++;
		diff_display_list();
	}

	else if (c == KEY_HOME)
	{
		if (di->diff_top_line != di->diff_cur_lines)
		{
			di->diff_top_line	= di->diff_cur_lines;
			di->diff_top_lineno	= 0;

			diff_display_list();
		}
	}

	else if (c == KEY_END)
	{
		if ((di->diff_cur_cnt - di->diff_top_lineno) >
			getmaxy(gbl(win_diff_lf)))
		{
			di->diff_top_lineno	=
				(di->diff_cur_cnt - getmaxy(gbl(win_diff_lf))) - 1;
			di->diff_top_line	=
				bnth(di->diff_cur_lines, di->diff_top_lineno);

			diff_display_list();
		}
	}

	else if (c == KEY_PGUP)
	{
		if (di->diff_top_lineno > 0)
		{
			if (di->diff_top_lineno > getmaxy(gbl(win_diff_lf)))
				di->diff_top_lineno -= getmaxy(gbl(win_diff_lf));
			else
				di->diff_top_lineno = 0;

			di->diff_top_line = bnth(di->diff_cur_lines, di->diff_top_lineno);

			diff_display_list();
		}
	}

	else if (c == KEY_PGDN)
	{
		if ((di->diff_cur_cnt - di->diff_top_lineno) >
			getmaxy(gbl(win_diff_lf)))
		{
			di->diff_top_lineno += getmaxy(gbl(win_diff_lf));
			di->diff_top_line	=
				bnth(di->diff_cur_lines, di->diff_top_lineno);

			diff_display_list();
		}
	}

	/*--------------------------------------------------------------------
	 * diff cmds
	 */
	else if (c == cmds(CMDS_DIFF_TOGGLE_NUM))
	{
		opt(diff_show_numbers) = ! opt(diff_show_numbers);
		diff_display_list();
	}

	else if (c == cmds(CMDS_DIFF_TOGGLE_COM))
	{
		opt(diff_show_diff) = ! opt(diff_show_diff);

		if (opt(diff_show_diff))
		{
			DIFF_LINE *	d = (DIFF_LINE *)bid(di->diff_top_line);

			di->diff_cur_lines	= di->diff_dif_lines;
			di->diff_cur_cnt	= di->diff_dif_cnt;

			if (LINE_TYPE(d) == COMMON)
			{
				DIFF_LINE *	t;
				BLIST *		b;

				for (b = bnext(di->diff_top_line); b; b = bnext(b))
				{
					t = (DIFF_LINE *)bid(b);

					if (LINE_TYPE(t) != COMMON)
					{
						di->diff_top_line = b;
						break;
					}
				}

				if (b == 0)
				{
					for (; b; b = bprev(b))
					{
						t = (DIFF_LINE *)bid(b);

						if (LINE_TYPE(t) != COMMON)
						{
							di->diff_top_line = b;
							break;
						}
					}
				}
			}
		}
		else
		{
			di->diff_cur_lines	= di->diff_all_lines;
			di->diff_cur_cnt	= di->diff_all_cnt;
		}

		di->diff_top_line	=
			bfind(di->diff_cur_lines, bid(di->diff_top_line));
		di->diff_top_lineno	= bindex (di->diff_cur_lines, di->diff_top_line);

		diff_display_list();
	}

	else if (c == cmds(CMDS_DIFF_EDIT_BOTH))
	{
		diff_edit(di->diff_path1, di->diff_path2);
		setup_display();
	}

	else if (c == cmds(CMDS_DIFF_EDIT_LEFT))
	{
		diff_edit(di->diff_path1, 0);
		setup_display();
	}

	else if (c == cmds(CMDS_DIFF_EDIT_RIGHT))
	{
		diff_edit(di->diff_path2, 0);
		setup_display();
	}

	else if (c == cmds(CMDS_DIFF_VIEW_LEFT))
	{
		view_file(di->diff_path1);
		setup_display();
	}

	else if (c == cmds(CMDS_DIFF_VIEW_RIGHT))
	{
		view_file(di->diff_path2);
		setup_display();
	}

	else if (c == cmds(CMDS_DIFF_NEXT_DIFF_1) ||
			 c == cmds(CMDS_DIFF_NEXT_DIFF_2))
	{
		if (bnext(di->diff_top_line) != 0)
		{
			BLIST *b;

			for (b = bnext(di->diff_top_line); b; b = bnext(b))
			{
				DIFF_LINE *d = (DIFF_LINE *)bid(b);

				if (LINE_TYPE(d) != COMMON)
					break;
			}

			if (b != 0)
			{
				di->diff_top_line	= b;
				di->diff_top_lineno	= bindex(di->diff_cur_lines,
					di->diff_top_line);

				diff_display_list();
			}
		}
	}

	else if (c == cmds(CMDS_DIFF_PREV_DIFF_1) ||
			 c == cmds(CMDS_DIFF_PREV_DIFF_2))
	{
		if (bprev(di->diff_top_line) != 0)
		{
			BLIST *b;

			for (b = bprev(di->diff_top_line); b; b = bprev(b))
			{
				DIFF_LINE *d = (DIFF_LINE *)bid(b);

				if (LINE_TYPE(d) != COMMON)
					break;
			}

			if (b != 0)
			{
				di->diff_top_line	= b;
				di->diff_top_lineno	= bindex(di->diff_cur_lines,
					di->diff_top_line);

				diff_display_list();
			}
		}
	}

	else if (c == cmds(CMDS_DIFF_REPORT))
	{
		diff_print(di->diff_lines);
		disp_cmds();
	}

	else if (c == cmds(CMDS_WIN_TABWIDTH))
	{
		opt(tab_width) = (opt(tab_width) == 8 ? 4 : 8);
		diff_display_list();
	}

	return (0);
}

void diff_view (const char *path1, const char *path2, DIFF_LINE *dl)
{
	DIFF_INFO *	di = &gbl(diff_info);

	/*--------------------------------------------------------------------
	 * cache path names
	 */
	strcpy(di->diff_path1, path1);
	strcpy(di->diff_path2, path2);

	/*--------------------------------------------------------------------
	 * load up diff file
	 */
	diff_load_file(dl);

	/*--------------------------------------------------------------------
	 * save old state & setup new state
	 */
	di->diff_save_in_fv = gbl(scr_in_fullview);
	di->diff_save_in_av = gbl(scr_in_autoview);

	gbl(scr_in_autoview) = FALSE;
	gbl(scr_in_fullview) = FALSE;

	gbl(scr_in_diffview) = TRUE;

	/*--------------------------------------------------------------------
	 * now do it
	 */
	diff_display_init();
	setup_display();
}

void diff_view_setup (void)
{
	DIFF_INFO *	di = &gbl(diff_info);
	char		pathbuf[MAX_PATHLEN];
	int			l;

	win_clock_set(FALSE);
	diff_border();

	l = getmaxx(gbl(win_path_line_split)[0]) - 2;
	wattrset (gbl(win_path_line_split)[0], gbl(scr_reg_lolite_attr));
	wstandset(gbl(win_path_line_split)[0], gbl(scr_reg_hilite_attr));
	werase   (gbl(win_path_line_split)[0]);
	wmove    (gbl(win_path_line_split)[0], 0, 1);
	wstandout(gbl(win_path_line_split)[0]);
	zaddstr  (gbl(win_path_line_split)[0], fn_disp_path(di->diff_path1,
		l, pathbuf));
	wstandend(gbl(win_path_line_split)[0]);

	l = getmaxx(gbl(win_path_line_split)[1]) - 2;
	wattrset (gbl(win_path_line_split)[1], gbl(scr_reg_lolite_attr));
	wstandset(gbl(win_path_line_split)[1], gbl(scr_reg_hilite_attr));
	werase   (gbl(win_path_line_split)[1]);
	wmove    (gbl(win_path_line_split)[1], 0, 2);
	wstandout(gbl(win_path_line_split)[1]);
	zaddstr  (gbl(win_path_line_split)[1], fn_disp_path(di->diff_path2,
		l, pathbuf));
	wstandend(gbl(win_path_line_split)[1]);

	diff_display_list();

	touchwin(stdscr);
	wrefresh(stdscr);
}
