/*------------------------------------------------------------------------
 * process device-table menu
 */
#include "libprd.h"

#define MP_MIN_WIDTH	10		/* min width of window */

typedef int MP_FUNCT (int n, int x);

struct mp_entry
{
	WINDOW *	mp_wb;
	WINDOW *	mp_w;
	BLIST *		mp_list;
	BLIST *		mp_cur_ent;
	BLIST *		mp_top_ent;
	int			mp_num_ents;
	int			mp_cur_no;
	int			mp_cur_line;
	int			mp_max_line;
	int			mp_scrollbar;
	int			mp_x;
	MP_FUNCT *	mp_action;
};
typedef struct mp_entry MP_ENTRY;

static void mp_hilite (MP_ENTRY *mp, int on)
{
	int i;
	chtype c;
	attr_t a;

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
	ARCH_DEV *a = (ARCH_DEV *)bid(b);

	waddch(mp->mp_w, ' ');
	waddstr(mp->mp_w, a->d_desc);
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

static int mp_move (MP_ENTRY *mp, int c)
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
		mp_hilite(mp, OFF);
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
		mp_hilite(mp, ON);
		break;

	case KEY_DOWN:
	case KEY_RIGHT:
	case KEY_SPACE:
		c = 0;
		if (mp->mp_cur_no == mp->mp_num_ents-1)
			break;
		mp_hilite(mp, OFF);
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
		mp_hilite(mp, ON);
		break;

	case KEY_PGUP:
		c = 0;
		if (mp->mp_cur_no == 0)
			break;
		mp_hilite(mp, OFF);
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
		mp_hilite(mp, ON);
		break;

	case KEY_PGDN:
		c = 0;
		if (mp->mp_cur_no == mp->mp_num_ents-1)
			break;
		mp_hilite(mp, OFF);
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
		mp_hilite(mp, ON);
		break;

	case KEY_HOME:
		c = 0;
		if (mp->mp_cur_no == 0)
			break;
		mp_hilite(mp, OFF);
		if (mp->mp_cur_no != mp->mp_cur_line)
		{
			mp->mp_top_ent = mp->mp_list;
			mp_disp_list(mp);
		}
		mp->mp_cur_no = 0;
		mp->mp_cur_line = 0;
		mp->mp_cur_ent = mp->mp_top_ent;
		mp_hilite(mp, ON);
		break;

	case KEY_END:
		c = 0;
		if (mp->mp_cur_no == mp->mp_num_ents-1)
			break;
		mp_hilite(mp, OFF);
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
		mp_hilite(mp, ON);
		break;
	}

	/* display new scrollbar */

	mp_do_scrollbar(mp, ON);
	wrefresh(mp->mp_wb);

	/* perform action if moved */

	if (mp->mp_action && c == 0)
		(*mp->mp_action)(mp->mp_cur_no, mp->mp_x);

	return (c);
}

static int mp_mouse (MP_ENTRY *mp)
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
					mp_hilite(mp, OFF);
					mp->mp_cur_line = y;
					mp->mp_cur_no = top + y;
					mp->mp_cur_ent = mp->mp_top_ent;
					for (i=0; i<y; i++)
						mp->mp_cur_ent = bnext(mp->mp_cur_ent);
					mp_hilite(mp, ON);
					mp_do_scrollbar(mp, ON);
					wrefresh(mp->mp_wb);
				}
				if (mp->mp_action)
					(*mp->mp_action)(mp->mp_cur_no, mp->mp_x);
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
				c = mp_move(mp, KEY_UP);
			else
				c = mp_move(mp, KEY_DOWN);
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
				mp_hilite(mp, OFF);
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
				mp_hilite(mp, ON);
				mp_do_scrollbar(mp, ON);
				wrefresh(mp->mp_wb);
			}
		}
	}
	return (c);
}

static void do_mp (MP_ENTRY *mp)
{
	int c;

	mp->mp_max_line = getmaxy(mp->mp_w);

	mp->mp_cur_no = 0;
	mp->mp_cur_line = 0;
	mp->mp_cur_ent = mp->mp_top_ent = mp->mp_list;
	mp_disp_list(mp);
	mp_do_scrollbar(mp, ON);
	mp_hilite(mp, ON);

	if (mp->mp_action)
		(*mp->mp_action)(mp->mp_cur_no, mp->mp_x);

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

		else if (c == KEY_MOUSE)
		{
			c = mp_mouse(mp);
			if (c)
				break;
		}

		else
		{
			c = mp_move(mp, c);
		}

		if (c)
			do_beep();
	}
}

int menupick (BLIST *menu_list, int (*action)(int n, int x), WINDOW *win,
	int ox)
{
	MP_ENTRY	mp_ent;
	MP_ENTRY *	mp = &mp_ent;
	BLIST *		b;
	int			l;
	int			win_ht;
	int			height;
	int			width;
	int			y;
	int			x;
	attr_t		reg_attr  = gbl(scr_menu_lolite_attr);
	attr_t		curs_attr = gbl(scr_menu_cursor_attr);

	if (menu_list == 0)
	{
		return (-1);
	}
	mp->mp_list = menu_list;
	mp->mp_action = action;
	mp->mp_x = ox;

	/* get number of entries & max width */

	width = MP_MIN_WIDTH;
	mp->mp_num_ents = 0;
	for (b=mp->mp_list; b; b=bnext(b))
	{
		ARCH_DEV *a = (ARCH_DEV *)bid(b);
		mp->mp_num_ents++;
		l = strlen(a->d_desc);
		if (l > width)
			width = l;
	}
	if (width > getmaxx(win)-4)
		width = getmaxx(win)-4;
	x = (getmaxx(win)-width-4)/2;

	win_ht = getmaxy(win);
	height = win_ht - 2;
	y = 0;
	if (height > mp->mp_num_ents)
	{
		height = mp->mp_num_ents;
		y = win_ht - (height + 2);
	}

	mp->mp_wb = newwin(height+2, width+4, y+getbegy(win), x+getbegx(win));
	if (mp->mp_wb == 0)
	{
		return (-1);
	}
	leaveok(mp->mp_wb, TRUE);

	mp->mp_w = derwin(mp->mp_wb, height, width+2, 1, 1);
	if (mp->mp_w == 0)
	{
		delwin(mp->mp_wb);
		return (-1);
	}

	wattrset (mp->mp_wb, reg_attr);
	wstandset(mp->mp_wb, curs_attr);
	wattrset (mp->mp_w, reg_attr);
	wstandset(mp->mp_w, curs_attr);
	werase(mp->mp_wb);
	box_around(mp->mp_w, mp->mp_wb, pgm_const(border_type));
	scrollbar(mp->mp_wb, mp->mp_w, SCRLBAR_VL, 0, mp->mp_num_ents);
	wrefresh(mp->mp_wb);

	do_mp(mp);

	delwin(mp->mp_w);
	delwin(mp->mp_wb);

	touchwin(win);
	wrefresh(win);

	return (mp->mp_cur_no);
}
