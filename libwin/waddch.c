/*------------------------------------------------------------------------
 * add a character to a window
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * wcdch() - internal routine to do the actual add
 */
static int wcdch (WINDOW *win, int code, chtype ch)
{
	int		y;
	int		x;
	int		c;
	int		ts;
	attr_t	a;
	attr_t	o;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * get current position & attributes
	 */
	y = getcury(win);
	x = getcurx(win);

	o = getattrs(win);

	/*--------------------------------------------------------------------
	 * get char & attributes of char to add
	 */
	c = A_GETTEXT(ch);
	a = A_GETATTR(ch);

	/*--------------------------------------------------------------------
	 * check if control character & not in alt-char-set
	 */
	if (c < ' ' &&
		(! ((o & A_ALTCHARSET) || (a & A_ALTCHARSET)) ) )
	{
		/*----------------------------------------------------------------
		 * Treat as a control char.  We only do some of them.
		 * The rest are just ignored.
		 */
		switch (c)
		{
		case '\n':
			/*------------------------------------------------------------
			 * NL - clear line and move to (y+1, 0)
			 *
			 * Note: here is where we would check "do_out_nl", but it
			 * is always done, so there.
			 */
			wclrtoeol(win);
			if (y < win->_bmarg)
			{
				setcury(win, getcury(win)+1);
				setcurx(win, 0);
			}
			else
			{
				setcurx(win, 0);
				scroll(win);
			}
			break;

		case '\r':
			/*------------------------------------------------------------
			 * CR - just move col pointer to start of line
			 */
			setcurx(win, 0);
			break;

		case '\t':
			/*------------------------------------------------------------
			 * TAB - insert spaces up to next tab position
			 */
			waddch(win, ' ');
			ts = scrn_get_tab_size(WIN_SCREEN(win));
			while (getcurx(win) % ts)
			{
				if (waddch(win, ' ') == ERR)
					break;
			}
			break;

		case '\b':
			/*------------------------------------------------------------
			 * BS - back up col pointer one position if not at start-of-line
			 */
			if (x == 0)
				return (ERR);
			else
				setcurx(win, getcurx(win)-1);
			break;

		case '\f':
			/*------------------------------------------------------------
			 * FF - clear the window and move to (0, 0)
			 */
			werase(win);
			setcury(win, 0);
			setcurx(win, 0);
			break;

		case 0x0b:
			/*------------------------------------------------------------
			 * VT - move row position down 1, keeping current col position
			 */
			if (y < win->_bmarg)
			{
				setcury(win, getcury(win)+1);
			}
			else
			{
				scroll(win);
			}
			break;

		case 0x07:
			/*------------------------------------------------------------
			 * BL - just ring the bell
			 */
			scrn_beep(WIN_SCREEN(win));
			break;

		default:
			break;
		}
	}
	else
	{
		/*----------------------------------------------------------------
		 * regular char or in alt-char-set
		 */
		if (c == ' ')
		{
			/*------------------------------------------------------------
			 * if space, use the pre-defined background char
			 */
			win->_y[y][x] = getbkgd(win) | a | o | code;
		}
		else
		{
			/*------------------------------------------------------------
			 * if not space, just store the char
			 */
			win->_y[y][x] = ch | o | code;
		}

		/*----------------------------------------------------------------
		 * adjust first/last entries
		 */
		if (win->_firstch[y] == _NOCHANGE || win->_firstch[y] > x)
			win->_firstch[y] = x;

		if (win->_lastch [y] == _NOCHANGE || win->_lastch [y] < x)
			win->_lastch [y] = x;

		/*----------------------------------------------------------------
		 * check if we are at the end of the line
		 */
		if (x < getmaxx(win)-1)
		{
			/*------------------------------------------------------------
			 * nope, just bump x position
			 */
			setcurx(win, getcurx(win)+1);
		}
		else
		{
			/*------------------------------------------------------------
			 * yup, check if we should scroll the window
			 */
			if (! getscroll(win))
				return (ERR);

			setcurx(win, 0);
			if (y < getmaxy(win)-1)
				setcury(win, getcury(win)+1);
			else
				scroll(win);
		}
	}

	/*--------------------------------------------------------------------
	 * if auto-sync in effect, sync this window with its parents
	 */
	if (getsync(win))
		wsyncup(win);

	/*--------------------------------------------------------------------
	 * if immed in effect, do a refresh
	 */
	if (getimmed(win))
		return wrefresh(win);

	return (OK);
}

/*------------------------------------------------------------------------
 * waddch() - add a char to a window (using code from window)
 */
int waddch (WINDOW *win, chtype ch)
{
	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * do the add
	 */
	return (wcdch(win, win->_code, ch));
}

/*------------------------------------------------------------------------
 * wcaddch() - add a char to a window (using specified code)
 */
int wcaddch (WINDOW *win, chtype ch, int code)
{
	int c;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * get code in proper form
	 */
	c = A_SETCODE(code);

	/*--------------------------------------------------------------------
	 * do the add
	 */
	return (wcdch(win, c, ch));
}

/*------------------------------------------------------------------------
 * wechochar() - add a char to a window (and refresh it)
 */
int wechochar (WINDOW *win, chtype ch)
{
	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * do the add
	 */
	waddch(win, ch);

	/*--------------------------------------------------------------------
	 * refresh either the window or just the line
	 */
	if (A_GETTEXT(ch) != '\n')
		wrefreshln(win, getcury(win));
	else
		wrefresh(win);

	return (OK);
}

/*------------------------------------------------------------------------
 * waddchnstr() - insert a string into a window
 */
int waddchnstr (WINDOW *win, const chtype *s, int n)
{
	int			i;
	chtype *	ch;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0 || s == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * adjust number of chars to fit in line (-1 implies to end-of-line)
	 */
	if (n < 0 || n > getmaxx(win) - getcurx(win))
		n = getmaxx(win) - getcurx(win);

	/*--------------------------------------------------------------------
	 * now insert the chars
	 */
	ch = &win->_y[win->_cury][win->_curx];
	for (i=0; i<n; i++)
	{
		if (*s == 0)
			break;
		*ch++ = *s++;
	}

	return (OK);
}
