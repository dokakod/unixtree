/*------------------------------------------------------------------------
 * get a character from the keyboard
 */
#include "libprd.h"

/*------------------------------------------------------------------------
 * get a char or a mouse event from stdscr
 */
int xgetch (WINDOW *win)
{
	int c;

	while (TRUE)
	{
		c = rgetch(win);
		switch(c)
		{
		case KEY_SHIFT_PRESS:
		case KEY_SHIFT_RELEASE:
		case KEY_CTRL_PRESS:
		case KEY_CTRL_RELEASE:
		case KEY_ALT_PRESS:
		case KEY_ALT_RELEASE:
			continue;
		}
		break;
	}

	return (c);
}

/*------------------------------------------------------------------------
 * if kbd event pending, get it
 */
int xchk_kbd (void)
{
	WINDOW *win = stdscr;
	int c = 0;

	if (wchkkbd(win))
	{
		int save_delay = getdelay(win);

		setdelay(win, 0);
		c = xgetch(stdscr);
		setdelay(win, save_delay);
	}

	return (c);
}

/*------------------------------------------------------------------------
 * get a char or a mouse event from stdscr (including key press/release)
 */
int rgetch (WINDOW *win)
{
	int c;

	while (TRUE)
	{
		/*----------------------------------------------------------------
		 * Check this first, since it may invalidate "win".
		 * We leave the changed flag to be checked later.
		 */
		if (trm_is_changed(FALSE))
			return (ERR);

		/*----------------------------------------------------------------
		 * get char and return if none present
		 */
		c = wgetch(win);
		if (c == ERR)
			return (ERR);

		/*----------------------------------------------------------------
		 * check if mouse event
		 */
		if (c == KEY_MOUSE)
		{
			int m;
			int code;

			/*------------------------------------------------------------
			 * get mouse event
			 */
			m = mouse_get_event(win);

			/*------------------------------------------------------------
			 * translate scroll events into cursor moves
			 */
			if (m == MOUSE_SCROLLUP)
				return (KEY_UP);

			if (m == MOUSE_SCROLLDN)
				return (KEY_DOWN);

			/*------------------------------------------------------------
			 * get code at window position
			 */
			code = mouse_c(win);
#if 0
			if (code == ERR || code == 0)
				continue;
#endif

			/*------------------------------------------------------------
			 * RETURN & ESCAPE are always honored
			 */
			if (m == MOUSE_LBD && code == CMDS_ESCAPE)
				return (cmds(code));

			if (m == MOUSE_LBD && code == CMDS_RETURN)
				return (cmds(code));

			/*------------------------------------------------------------
			 * check if mouse event happened in specified window
			 */
			if (! mouse_is_in_win(win))
				continue;

			/*------------------------------------------------------------
			 * if LBD & code present, pretend the code was entered
			 */
			if (m == MOUSE_LBD && code != 0)
				c = cmds(code);
		}

		break;
	}

	return (c);
}
