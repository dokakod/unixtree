/*------------------------------------------------------------------------
 * process a history menu
 */
#include "libprd.h"

#define MP_MIN_WIDTH	10		/* min width of window */

typedef void MP_ACTION (XG_ENTRY *xe, XG_INFO *xi);

struct mp_entry
{
	WINDOW *	mp_wb;
	WINDOW *	mp_w;
	XG_LIST *	mp_xg;
	BLIST *		mp_list;
	BLIST *		mp_cur_ent;
	BLIST *		mp_top_ent;
	int			mp_num_ents;
	int			mp_cur_no;
	int			mp_cur_line;
	int			mp_max_line;
	int			mp_scrollbar;
	MP_ACTION *	mp_action;
};
typedef struct mp_entry MP_ENTRY;

static void do_action (MP_ENTRY *mp, XG_INFO *xi)
{
	if (mp->mp_action)
	{
		XG_ENTRY *xe = (XG_ENTRY *)bid(mp->mp_cur_ent);

		(*mp->mp_action)(xe, xi);
	}
}

static void mp_hilite (MP_ENTRY *mp, XG_INFO *xi, int on)
{
	int i;
	chtype c;
	attr_t a;

	do_action(mp, xi);

	if (on)
		a = wstandget(mp->mp_w);
	else
		a = wattrget(mp->mp_w);

	for (i=0; i<getmaxx(mp->mp_w); i++)
	{
		c = wchat(mp->mp_w, mp->mp_cur_line, i);
		c = A_GETTEXT(c) | a;
		woutch(mp->mp_w, mp->mp_cur_line, i, c);
	}

	wtouchln(mp->mp_w, mp->mp_cur_line, 1, TRUE);
	wrefresh(mp->mp_w);
}

static void mp_disp_ent (MP_ENTRY *mp, BLIST *b)
{
	XG_ENTRY *xe = (XG_ENTRY *)bid(b);

	waddch (mp->mp_w, xe->keep ? '*' : ' ');
	waddch (mp->mp_w, ' ');
	waddstr(mp->mp_w, xe->line);
}

static void mp_disp_list (MP_ENTRY *mp)
{
	BLIST *b;
	int i;

	werase(mp->mp_w);
	for (i=0, b=mp->mp_top_ent; b; b=bnext(b))
	{
		if (i >= mp->mp_max_line)
			break;
		wmove(mp->mp_w, i, 0);
		mp_disp_ent(mp, b);
		i++;
	}
}

static void mp_do_scrollbar (MP_ENTRY *mp, int flag)
{
	if (flag)
	{
		mp->mp_scrollbar = mp->mp_cur_no;
		scrollbar(mp->mp_wb, mp->mp_w, SCRLBAR_VL, mp->mp_scrollbar,
			mp->mp_num_ents);
	}
	else
	{
		scrollbar(mp->mp_wb, mp->mp_w, SCRLBAR_VL, -1, mp->mp_num_ents);
	}
}

static int mp_move (MP_ENTRY *mp, XG_INFO *xi, int c)
{
	int i;

	mp_do_scrollbar(mp, OFF);
	switch (c)
	{
	case KEY_UP:
	case KEY_LEFT:
		c = 0;
		if (mp->mp_cur_no == 0)
			break;
		mp_hilite(mp, xi, OFF);
		mp->mp_cur_no--;
		mp->mp_cur_ent = bprev(mp->mp_cur_ent);
		mp->mp_cur_line--;
		if (mp->mp_cur_line < 0)
		{
			mp->mp_cur_line = 0;
			mp->mp_top_ent = mp->mp_cur_ent;
			wmove(mp->mp_w, 0, 0);
			winsertln(mp->mp_w);
			mp_disp_ent(mp, mp->mp_cur_ent);
		}
		mp_hilite(mp, xi, ON);
		break;

	case KEY_DOWN:
	case KEY_RIGHT:
	case KEY_SPACE:
		c = 0;
		if (mp->mp_cur_no == mp->mp_num_ents-1)
			break;
		mp_hilite(mp, xi, OFF);
		mp->mp_cur_no++;
		mp->mp_cur_ent = bnext(mp->mp_cur_ent);
		mp->mp_cur_line++;
		if (mp->mp_cur_line >= mp->mp_max_line)
		{
			mp->mp_cur_line = mp->mp_max_line-1;
			mp->mp_top_ent = bnext(mp->mp_top_ent);
			wmove(mp->mp_w, 0, 0);
			wdeleteln(mp->mp_w);
			wmove(mp->mp_w, mp->mp_max_line-1, 0);
			mp_disp_ent(mp, mp->mp_cur_ent);
		}
		mp_hilite(mp, xi, ON);
		break;

	case KEY_PGUP:
		c = 0;
		if (mp->mp_cur_no == 0)
			break;
		mp_hilite(mp, xi, OFF);
		if (mp->mp_cur_line)
		{
			mp->mp_cur_no -= mp->mp_cur_line;
		}
		else
		{
			for (i=0; i<mp->mp_max_line-1; i++)
			{
				mp->mp_cur_no--;
				mp->mp_top_ent = bprev(mp->mp_top_ent);
				if (mp->mp_cur_no == 0)
					break;
			}
			mp_disp_list(mp);
		}
		mp->mp_cur_ent = mp->mp_top_ent;
		mp->mp_cur_line = 0;
		mp_hilite(mp, xi, ON);
		break;

	case KEY_PGDN:
		c = 0;
		if (mp->mp_cur_no == mp->mp_num_ents-1)
			break;
		mp_hilite(mp, xi, OFF);
		if (mp->mp_cur_line < mp->mp_max_line-1)
		{
			for (i=mp->mp_cur_line; i<mp->mp_max_line-1; i++)
			{
				if (! bnext(mp->mp_cur_ent))
					break;
				mp->mp_cur_ent = bnext(mp->mp_cur_ent);
				mp->mp_cur_no++;
				mp->mp_cur_line++;
			}
		}
		else
		{
			mp->mp_cur_line = 0;
			mp->mp_top_ent = mp->mp_cur_ent;
			for (i=0; i<mp->mp_max_line-1; i++)
			{
				if (! bnext(mp->mp_cur_ent))
					break;
				mp->mp_cur_ent = bnext(mp->mp_cur_ent);
				mp->mp_cur_no++;
				mp->mp_cur_line++;
			}
			mp_disp_list(mp);
		}
		mp_hilite(mp, xi, ON);
		break;

	case KEY_HOME:
		c = 0;
		if (mp->mp_cur_no == 0)
			break;
		mp_hilite(mp, xi, OFF);
		if (mp->mp_cur_no != mp->mp_cur_line)
		{
			mp->mp_top_ent = mp->mp_list;
			mp_disp_list(mp);
		}
		mp->mp_cur_no = 0;
		mp->mp_cur_line = 0;
		mp->mp_cur_ent = mp->mp_top_ent;
		mp_hilite(mp, xi, ON);
		break;

	case KEY_END:
		c = 0;
		if (mp->mp_cur_no == mp->mp_num_ents-1)
			break;
		mp_hilite(mp, xi, OFF);
		if ((mp->mp_cur_no-mp->mp_cur_line)+mp->mp_max_line >= mp->mp_num_ents)
		{
			for (; bnext(mp->mp_cur_ent); mp->mp_cur_ent=bnext(mp->mp_cur_ent))
				mp->mp_cur_line++;
		}
		else
		{
			for (; bnext(mp->mp_cur_ent); mp->mp_cur_ent=bnext(mp->mp_cur_ent))
				;
			mp->mp_top_ent = mp->mp_cur_ent;
			for (i=1; i<mp->mp_max_line; i++)
				mp->mp_top_ent = bprev(mp->mp_top_ent);
			mp->mp_cur_line = mp->mp_max_line-1;
			mp_disp_list(mp);
		}
		mp->mp_cur_no = mp->mp_num_ents-1;
		mp_hilite(mp, xi, ON);
		break;
	}

	/* display new scrollbar */

	mp_do_scrollbar(mp, ON);
	wrefresh(mp->mp_wb);

	return (c);
}

static int mp_mouse (MP_ENTRY *mp, XG_INFO *xi)
{
	int c;
	int m;
	int y;
	int top;
	int i;
	int j;

	c = 0;
	m = mouse_get_event(mp->mp_w);
	j = check_mouse_in_win(mp->mp_w, mp->mp_num_ents);
	if (j == 3)
	{
		y = mouse_y(mp->mp_w) - getbegy(mp->mp_w);
		top = mp->mp_cur_no - mp->mp_cur_line;
		if (m == MOUSE_LBD || m == MOUSE_DRAG)
		{
			if (y != mp->mp_cur_line)
			{
				if (top+y < mp->mp_num_ents)
				{
					mp_do_scrollbar(mp, OFF);
					mp_hilite(mp, xi, OFF);
					mp->mp_cur_line = y;
					mp->mp_cur_no = top + y;
					mp->mp_cur_ent = mp->mp_top_ent;
					for (i=0; i<y; i++)
						mp->mp_cur_ent = bnext(mp->mp_cur_ent);
					mp_hilite(mp, xi, ON);
					mp_do_scrollbar(mp, ON);
					wrefresh(mp->mp_wb);
				}
			}
		}
		else if (m == MOUSE_LBU)
		{
			c = KEY_RETURN;
		}
	}
	else if (j == 1 || j == 2)
	{
		if (m == MOUSE_LBD || m == MOUSE_DRAG)
		{
			if (j == 1)
				c = mp_move(mp, xi, KEY_UP);
			else
				c = mp_move(mp, xi, KEY_DOWN);
		}
	}
	else if (j == 4)
	{
		if (m == MOUSE_LBD || m == MOUSE_DRAG)
		{
			j = read_vert_scrollbar(mp->mp_w, mp->mp_num_ents);
			if (j != mp->mp_cur_no)
			{
				mp_do_scrollbar(mp, OFF);
				mp_hilite(mp, xi, OFF);
				top = mp->mp_cur_no - mp->mp_cur_line;
				if (j >= top && j-top < mp->mp_max_line)
				{
					mp->mp_cur_no = j;
					mp->mp_cur_line = j - top;
					mp->mp_cur_ent = mp->mp_top_ent;
					for (i=0; i<mp->mp_cur_line; i++)
						mp->mp_cur_ent = bnext(mp->mp_cur_ent);
				}
				else
				{
					top = calculate_top_item(j, mp->mp_num_ents,
						mp->mp_max_line);
					mp->mp_top_ent = mp->mp_list;
					for (i=0; i<top; i++)
						mp->mp_top_ent = bnext(mp->mp_top_ent);
					mp->mp_cur_no = j;
					mp->mp_cur_line = j - top;
					mp->mp_cur_ent = mp->mp_top_ent;
					for (i=0; i<mp->mp_cur_line; i++)
						mp->mp_cur_ent = bnext(mp->mp_cur_ent);
					mp_disp_list(mp);
				}
				mp_hilite(mp, xi, ON);
				mp_do_scrollbar(mp, ON);
				wrefresh(mp->mp_wb);
			}
		}
	}
	return (c);
}

static int do_mp (MP_ENTRY *mp, XG_INFO *xi)
{
	int c;
	int changed = FALSE;

	mp->mp_max_line = getmaxy(mp->mp_w);

	mp->mp_cur_no = 0;
	mp->mp_cur_line = 0;
	mp->mp_cur_ent = mp->mp_top_ent = mp->mp_list;
	mp_disp_list(mp);
	mp_do_scrollbar(mp, ON);
	if (mp->mp_num_ents > 1)
		mp_move(mp, xi, KEY_END);
	else
		mp_hilite(mp, xi, ON);

	while (TRUE)
	{
		position_cursor();
		wrefresh(stdscr);

		c = xgetch(mp->mp_wb);

		if (c == KEY_ESCAPE)
		{
			mp->mp_cur_no = -1;
			break;
		}

		else if (c == KEY_RETURN)
		{
			break;
		}

		else if (c == cmds(CMDS_XGETSTR_KEEP))
		{
			XG_ENTRY *	xe = (XG_ENTRY *)bid(mp->mp_cur_ent);

			xe->keep = ! xe->keep;
			wmove(mp->mp_w, mp->mp_cur_line, 0);
			mp_disp_ent(mp, mp->mp_cur_ent);
			mp_hilite(mp, xi, TRUE);
			changed = TRUE;
		}

		else if (c == KEY_DELETE)
		{
			XG_ENTRY *	xe = (XG_ENTRY *)bid(mp->mp_cur_ent);
			BLIST *		old_ent = mp->mp_cur_ent;
			BLIST *		b;

			xg_ent_free(xe);

			if (mp->mp_xg->num == 1)
			{
				/* only entry in list */

				mp->mp_xg->lines = BSCRAP(mp->mp_xg->lines, FALSE);
				mp->mp_xg->num	= 0;
				changed			= FALSE;
				mp->mp_cur_no	= -1;
				break;
			}

			if (bnext(mp->mp_cur_ent) == 0)
			{
				/* at bottom */

				if (bprev(mp->mp_top_ent) != 0)
					mp->mp_top_ent = bprev(mp->mp_top_ent);
				mp->mp_cur_ent = bprev(mp->mp_cur_ent);
				mp->mp_cur_no--;
			}
			else if (bprev(mp->mp_cur_ent) == 0)
			{
				/* at top */

				mp->mp_cur_ent = bnext(mp->mp_cur_ent);
				mp->mp_top_ent = mp->mp_cur_ent;
			}
			else
			{
				/* somewhere in the middle */

				if (mp->mp_top_ent == mp->mp_cur_ent)
					mp->mp_top_ent = bprev(mp->mp_top_ent);
				mp->mp_cur_ent = bnext(mp->mp_cur_ent);
			}

			mp->mp_xg->lines = BDELETE(mp->mp_xg->lines, old_ent, FALSE);
			mp->mp_xg->num--;
			mp->mp_num_ents--;

			mp->mp_cur_line = 0;
			for (b = mp->mp_top_ent; b; b = bnext(b))
			{
				if (b == mp->mp_cur_ent)
					break;
				mp->mp_cur_line++;
			}

			mp_disp_list(mp);
			mp_hilite(mp, xi, ON);

			mp_do_scrollbar(mp, TRUE);
			wrefresh(mp->mp_wb);
		}

		else if (c == KEY_MOUSE)
		{
			c = mp_mouse(mp, xi);
			if (c)
				break;
		}

		else
		{
			c = mp_move(mp, xi, c);
		}

		if (c)
			do_beep();
	}

	return (changed);
}

XG_ENTRY * xg_menu (XG_LIST *xg, XG_INFO *xi,
	void (*action)(XG_ENTRY *xe, XG_INFO *xi))
{
	MP_ENTRY	mp_ent;
	MP_ENTRY *	mp = &mp_ent;
	XG_ENTRY *	xe;
	BLIST *		b;
	int			win_wt;
	int			height;
	int			width;
	int			my;
	int			mx;
	int			py;
	int			px;

	/*--------------------------------------------------------------------
	 * if empty list, just bail
	 */
	if (xg == 0 || xg->num == 0)
	{
		return (0);
	}

	/*--------------------------------------------------------------------
	 * cache args
	 */
	mp->mp_xg     = xg;
	mp->mp_list   = xg->lines;
	mp->mp_action = action;

	/*--------------------------------------------------------------------
	 * get number of entries & max width
	 */
	width = MP_MIN_WIDTH;
	mp->mp_num_ents = 0;
	for (b=mp->mp_list; b; b=bnext(b))
	{
		char *	p = ((XG_ENTRY *)bid(b))->line;
		int		l = strlen(p);

		mp->mp_num_ents++;
		if (l > width)
			width = l;
	}

	width += 3;				/* for leading "* "  & trailing " " */

	/*--------------------------------------------------------------------
	 * now calculate menu window size & position
	 */
	my = getbegy(xi->xg_win) + xi->xg_y;
	mx = getbegx(xi->xg_win) + xi->xg_x;

	win_wt = getmaxx(stdscr);

	if (width > win_wt - (mx - 2) - 2)
		width = win_wt - (mx - 2) - 2;
	px = mx - 3;

	height = mp->mp_num_ents;
	if (height > (my - 4))
		height = (my - 4);
	py = my - height - 2;

	/*--------------------------------------------------------------------
	 * create menu window & menu-border window
	 */
	mp->mp_wb = newwin(height+2, width+2, py, px);
	if (mp->mp_wb == 0)
	{
		return (0);
	}
	leaveok(mp->mp_wb, TRUE);

	mp->mp_w = derwin(mp->mp_wb, height, width, 1, 1);
	if (mp->mp_w == 0)
	{
		delwin(mp->mp_wb);
		return (0);
	}

	/*--------------------------------------------------------------------
	 * set menu window attributes
	 */
	wattrset (mp->mp_wb, gbl(scr_menu_lolite_attr));
	wstandset(mp->mp_wb, gbl(scr_menu_cursor_attr));

	wattrset (mp->mp_w, gbl(scr_menu_lolite_attr));
	wstandset(mp->mp_w, gbl(scr_menu_cursor_attr));

	/*--------------------------------------------------------------------
	 * now draw menu border (& empty menu window)
	 */
	werase(mp->mp_wb);
	box_around(mp->mp_w, mp->mp_wb, pgm_const(border_type));
	scrollbar(mp->mp_wb, mp->mp_w, SCRLBAR_VL, 0, mp->mp_num_ents);
	wrefresh(mp->mp_wb);

	/*--------------------------------------------------------------------
	 * now do it
	 */
	do_mp(mp, xi);

	/*--------------------------------------------------------------------
	 * delete menu windows
	 */
	delwin(mp->mp_w);
	delwin(mp->mp_wb);

	/*--------------------------------------------------------------------
	 * refresh stdscr
	 */
	touchwin(stdscr);
	wrefresh(stdscr);

	if (mp->mp_cur_no >= 0)
	{
		xe = (XG_ENTRY *)bid(mp->mp_cur_ent);
	}
	else
	{
		xe = 0;
	}

	return (xe);
}
