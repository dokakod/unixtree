/*------------------------------------------------------------------------
 * mouse routines
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * display_char() - internal routine to display one char
 */
static void display_char (SCREEN *s, int y, int x, chtype ch)
{
	WINDOW *ts = SCR_TRMSCR(s);
	chtype	cp;
	attr_t	a;
	int		c;

	/*--------------------------------------------------------------------
	 * display char, checking if last char on last line or not
	 */
	if (! scrn_need_ins_final(s) ||
	    y != getmaxy(ts)-1 || x != getmaxx(ts)-1)
	{
		if (scrn_get_curr_row(s) != y ||
			scrn_get_curr_col(s) != x)
		{
			scrn_outpos(s, y, x);
		}

		c = A_GETTEXT(ch);
		a = A_GETATTR(ch);
		scrn_outattr(s, a, c);
		scrn_outc(s, c);
	}
	else
	{
		cp = wchat(ts, y, x-1);
		scrn_write_final_char(s, A_GETTEXT(cp), A_GETATTR(cp),
			A_GETTEXT(ch), A_GETATTR(ch));
	}

	/*--------------------------------------------------------------------
	 * cache current attribute
	 */
	wattrset(ts, scrn_get_curr_attr(s));
}

/*------------------------------------------------------------------------
 * display_mouse_cursor() - internal routine to display the mouse on/off
 */
static void display_mouse_cursor (SCREEN *s, int flag)
{
	WINDOW *ts = SCR_TRMSCR(s);
	WINDOW *cs = SCR_CURSCR(s);
	int		cy;
	int		cx;
	int		my;
	int		mx;
	chtype	ch;
	attr_t	a;

	/*--------------------------------------------------------------------
	 * bail if mouse if not currently being displayed
	 */
	if (! scrn_mi_get_display(s))
		return;

	/*--------------------------------------------------------------------
	 * get current cursor position & mouse position
	 */
	my = mouse_y(ts);
	mx = mouse_x(ts);

	if (mx < 0 || my < 0)
		return;

	cy = getcury(ts);
	cx = getcurx(ts);

	/*--------------------------------------------------------------------
	 * get real char at the mouse position
	 * (Remember that curscr has real data, only trmscr shows the mouse.)
	 */
	ch = wchat(cs, my, mx);

	/*--------------------------------------------------------------------
	 * if showing the mouse, change attributes to that of the mouse
	 */
	if (flag)
	{
		a = A_GETATTR(ch);
		if (a & A_ALTCHARSET)
			a = scrn_mi_get_ptr_attrs(s) | A_ALTCHARSET;
		else
			a = scrn_mi_get_ptr_attrs(s);
		ch = (ch & ~A_ATTRIBUTES) | a;
	}

	/*--------------------------------------------------------------------
	 * now output the adjusted char
	 */
	woutch(ts, my, mx, ch);
	display_char(s, my, mx, ch);

	/*--------------------------------------------------------------------
	 * restore original cursor position
	 */
	scrn_outpos(s, cy, cx);
	scrn_outflush(s);
}

/*------------------------------------------------------------------------
 * disp_mouse_cmd() - internal routine to update attributes of a string
 * of chars around the mouse
 */
static void disp_mouse_cmd (SCREEN *s, int y, int x, int flag)
{
	WINDOW *ts = SCR_TRMSCR(s);
	WINDOW *cs = SCR_CURSCR(s);
	chtype	ch;
	chtype	cc;
	attr_t	ah;
	attr_t	a;
	int		code;
	int		xs;
	int		xe;
	int		xp;

	/*--------------------------------------------------------------------
	 * bail if mouse if not being displayed
	 */
	if (! scrn_mi_get_display(s))
		return;

	/*--------------------------------------------------------------------
	 * adjust specified mouse position
	 */
	if (x < 0)
		x = 0;

	if (x >= getmaxx(ts))
		x = getmaxx(ts) - 1;

	if (y < 0)
		y = 0;

	if (y >= getmaxy(ts))
		y = getmaxy(ts) - 1;

	/*--------------------------------------------------------------------
	 * get the mouse code at this location
	 */
	code = A_GETCODE(wchat(ts, y, x));

	/*--------------------------------------------------------------------
	 * get start/end of cmd string
	 *
	 * This will be only the mouse position if no code is present
	 * or the positions of the first & last chars around it with the
	 * same code.
	 */
	if (code && scrn_mi_get_cmd_attrs(s))
	{
		for (xs=x-1; xs>=0; xs--)
		{
			if (code != A_GETCODE(wchat(ts, y, xs)))
				break;
		}
		xs++;

		for (xe=x+1; xe<getmaxx(ts); xe++)
		{
			if (code != A_GETCODE(wchat(ts, y, xe)))
				break;
		}
		xe--;
	}
	else
	{
		xs = xe = x;
	}

	/*--------------------------------------------------------------------
	 * now adjust the attributes for all chars from xs to xe
	 */
	for (xp=xs; xp<=xe; xp++)
	{
		/*----------------------------------------------------------------
		 * get current char
		 */
		ch = wchat(ts, y, xp);

		/*----------------------------------------------------------------
		 * if display is on, set attributes according to whether we are
		 * on the mouse position or not
		 *
		 * if display is off, restore char to its original attributes
		 */
		if (flag)
		{
			if (xp == x)
				a = scrn_mi_get_ptr_attrs(s);
			else
				a = scrn_mi_get_cmd_attrs(s);

			ah = A_GETATTR(ch);
			if (ah & A_ALTCHARSET)
				a |= A_ALTCHARSET;

			if (a != ah)
			{
				ch = (ch & ~A_ATTRIBUTES) | a;
				display_char(s, y, xp, ch);
				woutch(ts, y, xp, ch);
			}
		}
		else
		{
			cc = wchat(cs, y, xp);
			if (cc != ch)
			{
				display_char(s, y, xp, cc);
				woutch(ts, y, xp, cc);
			}
		}
	}
}

/*------------------------------------------------------------------------
 * check_mouse_cursor() - internal routine to check if mouse display
 * needs to be updated
 */
static void check_mouse_cursor (SCREEN *s)
{
	WINDOW *ts;

	if (s == 0)
		return;

	ts = SCR_TRMSCR(s);

	/*--------------------------------------------------------------------
	 * only do this if mouse is being displayed
	 */
	if (scrn_mi_get_process(s))
	{
		/*----------------------------------------------------------------
		 * get current position, mouse position, & prev mouse info
		 */
		int cy	= getcury(ts);
		int cx	= getcurx(ts);
		int my	= mouse_y(ts);
		int mx	= mouse_x(ts);
		int py	= scrn_mi_get_prev_y(s);
		int px	= scrn_mi_get_prev_x(s);
		int pc	= scrn_mi_get_prev_c(s);

		/*----------------------------------------------------------------
		 * adjust mouse position if needed
		 */
		if (mx < 0)
			mx = 0;

		if (mx >= getmaxx(ts))
			mx = getmaxx(ts) - 1;

		if (my < 0)
			my = 0;

		if (my >= getmaxy(ts))
			my = getmaxy(ts) - 1;

		/*----------------------------------------------------------------
		 * update display if mouse moved
		 */
		if (my != py || mx != px)
		{
			int c	= A_GETCODE(wchat(ts, my, mx));

			/*------------------------------------------------------------
			 * "undisplay" old cmd if mouse moved from it
			 */
			if (py != -1 && px != -1)
			{
				int clr_old = FALSE;

				if (pc == 0 || pc != c || my != py)
					clr_old = TRUE;

				if (my == py && mx != px)
				{
					int x1;
					int x2;
					int tx;

					if (px < mx)
					{
						x1 = px;
						x2 = mx;
					}
					else
					{
						x1 = mx;
						x2 = px;
					}

					for (tx = x1; tx <= x2; tx++)
					{
						int tc = A_GETCODE(wchat(ts, my, tx));

						if (c != tc)
						{
							clr_old = TRUE;
							break;
						}
					}
				}

				if (clr_old)
					disp_mouse_cmd(s, py, px, FALSE);
			}

			/*------------------------------------------------------------
			 * display mouse cmd string
			 */
			disp_mouse_cmd(s, my, mx, TRUE);
			scrn_mi_set_prev_y(s, my);
			scrn_mi_set_prev_x(s, mx);
			scrn_mi_set_prev_c(s, c);

			/*------------------------------------------------------------
			 * restore original cursor position
			 */
			scrn_outpos(s, cy, cx);
			scrn_outflush(s);
		}
	}
}

/*------------------------------------------------------------------------
 * process_mouse_event() - async key routine to process mouse movements
 */
static int process_mouse_event (int key, void *data)
{
	SCREEN *s = (SCREEN *)data;

	if (s != 0)
		check_mouse_cursor(s);

	return (key);
}

/*------------------------------------------------------------------------
 * mouse_setup() - initialize mouse processing
 */
int mouse_setup (WINDOW *win, attr_t ptr_attrs, attr_t cmd_attrs)
{
	if (win == 0)
		return (ERR);

	return scrn_mouse_setup(WIN_SCREEN(win), ptr_attrs, cmd_attrs);
}

/*------------------------------------------------------------------------
 * scrn_mouse_setup() - initialize mouse processing
 */
int scrn_mouse_setup (SCREEN *s, attr_t ptr_attrs, attr_t cmd_attrs)
{
	int rc;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (s == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * cache attributes to display mouse & cmd in
	 */
	scrn_mi_set_ptr_attrs(s, ptr_attrs);
	scrn_mi_set_cmd_attrs(s, cmd_attrs);

	/*--------------------------------------------------------------------
	 * initialize mouse processing
	 */
	rc = scrn_mouse_init(s);
	if (rc == OK)
	{
		scrn_mi_set_process(s, TRUE);
		scrn_mi_set_display(s, scrn_mouse_get_display(s));
		scrn_set_async_key(s, KEY_MOUSE, process_mouse_event, s);
	}

	/*--------------------------------------------------------------------
	 * if reg mouse attributes are normal, turn off display
	 */
	if (scrn_mi_get_ptr_attrs(s) == A_NORMAL)
	{
		scrn_mi_set_display(s, FALSE);
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * scrn_mouse_draw_cursor() - redraw mouse cmd (for after a refresh)
 */
int scrn_mouse_draw_cursor (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	scrn_mi_set_prev_y(s, -1);
	scrn_mi_set_prev_x(s, -1);
	scrn_mi_set_prev_c(s, 0);

	check_mouse_cursor(s);

	return (OK);
}

/*------------------------------------------------------------------------
 * scrn_mouse_hide_cursor() - turn off mouse display
 */
int scrn_mouse_hide_cursor (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	display_mouse_cursor(s, FALSE);
	return (OK);
}

/*------------------------------------------------------------------------
 * scrn_mouse_show_cursor() - turn on mouse display
 */
int scrn_mouse_show_cursor (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	display_mouse_cursor(s, TRUE);
	return (OK);
}

/*------------------------------------------------------------------------
 * mouse_hide() - turn off mouse processing
 */
int mouse_hide (WINDOW *win)
{
	SCREEN *s;

	if (win == 0)
		return (ERR);

	s = WIN_SCREEN(win);
	scrn_mouse_hide_cursor(s);
	scrn_mi_set_process(s, FALSE);

	return (OK);
}

/*------------------------------------------------------------------------
 * mouse_show() - turn on mouse processing
 */
int mouse_show (WINDOW *win)
{
	SCREEN *s;

	if (win == 0)
		return (ERR);

	s = WIN_SCREEN(win);
	scrn_mi_set_process(s, TRUE);
	scrn_mouse_show_cursor(s);

	return (OK);
}

/*------------------------------------------------------------------------
 * mouse_is_in_window() - check if mouse is in a specified window
 */
int mouse_is_in_win (WINDOW *win)
{
	int y;
	int x;

	if (win == 0)
		return (FALSE);

	y = mouse_y(win);
	x = mouse_x(win);

	if (y < 0 || x < 0)
		return (FALSE);

	if (y < getbegy(win) || y >= getbegy(win) + getmaxy(win))
		return (FALSE);

	if (x < getbegx(win) || x >= getbegx(win) + getmaxx(win))
		return (FALSE);

	return (TRUE);
}
