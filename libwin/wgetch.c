/*------------------------------------------------------------------------
 * keyboard routines
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * wgetch() - get a char using a window's parameters
 */
int wgetch (WINDOW *win)
{
	SCREEN *s;
	int c;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	s = WIN_SCREEN(win);

	/*--------------------------------------------------------------------
	 * refresh the window used, but keep the cursor where it was
	 */
	c = getleave(win);
	leaveok(win, FALSE);
	wrefresh(win);
	leaveok(win, c);

	/*--------------------------------------------------------------------
	 * get the char with or without a timeout
	 */
	if (win->_notimeout)
	{
		scrn_set_timeout(s, FALSE);
		c = scrn_getkey(s, getkeypad(win), getdelay(win));
		scrn_set_timeout(s, TRUE);
	}
	else
	{
		c = scrn_getkey(s, getkeypad(win), getdelay(win));
	}

	/*--------------------------------------------------------------------
	 * do cr -> nl translation if requested
	 */
	if (c == '\r' && s->do_inp_nl)
		c = '\n';

	/*--------------------------------------------------------------------
	 * echo the char if requested
	 */
	if (s->do_echo)
	{
		if (c != ERR && c <= 0xff)
			waddch(win, c);
	}

	return (c);
}

/*------------------------------------------------------------------------
 * wmgetch() - get a char from the keyboard & translate to mouse
 * code if needed
 */
int wmgetch (WINDOW *win)
{
	int	c;

	c = wgetch(win);
	if (c == KEY_MOUSE && mouse_is_in_win(win))
	{
		int	m = mouse_get_event(win);

		if (m == MOUSE_LBD)
		{
			m = mouse_c(win);
			if (m != 0)
				c = m;
		}
	}

	return (c);
}

/*------------------------------------------------------------------------
 * wungetch() - push a char back
 */
int wungetch (WINDOW *win, int ch)
{
	return scrn_pushkey(WIN_SCREEN(win), ch, TRUE);
}

/*------------------------------------------------------------------------
 * wchkkbd() - check if any chars pending
 */
int wchkkbd (WINDOW *win)
{
	if (win == 0)
		return (ERR);

	return (scrn_chkkey(WIN_SCREEN(win), getkeypad(win)));
}
