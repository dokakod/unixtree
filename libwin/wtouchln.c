/*------------------------------------------------------------------------
 * line touch routines
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * wlntouch() - touch a range of specified lines in a window
 */
int wlntouch (WINDOW *win, int line, int num, int xs, int xe)
{
	int i;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	if (line < 0 || num < 0 || (line+num) > getmaxy(win))
		return (ERR);

	if (xs < 0 || xs >= getmaxx(win) ||
        xe < 0 || xe >= getmaxx(win))
	{
		return (ERR);
	}

	/*--------------------------------------------------------------------
	 * do each line specified
	 */
	for (i=0; i<num; i++)
	{
		if (win->_firstch[line+i] == _NOCHANGE ||
		    win->_firstch[line+i] >  xs)
		{
			win->_firstch[line+i] = xs;
		}

		if (win->_lastch[line+i] == _NOCHANGE ||
		    win->_lastch[line+i] <  xe)
		{
			win->_lastch[line+i] = xe;
		}
	}

	return (OK);
}

/*------------------------------------------------------------------------
 * wtouchln() - touch/untouch specified lines in a window
 */
int wtouchln (WINDOW *win, int line, int num, int flag)
{
	int i;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	if (line < 0 || num < 0 || (line+num) > getmaxy(win))
		return (ERR);

	/*--------------------------------------------------------------------
	 * do each line specified
	 */
	for (i=0; i<num; i++)
	{
		if (flag)
		{
			win->_firstch[line+i] = 0;
			win->_lastch [line+i] = getmaxx(win)-1;
		}
		else
		{
			win->_firstch[line+i] = _NOCHANGE;
			win->_lastch [line+i] = _NOCHANGE;
		}
	}

	return (OK);
}

/*------------------------------------------------------------------------
 * is_linetouched() - check if a line is touched
 */
int is_linetouched (WINDOW *win, int line)
{
	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (FALSE);

	if (line < 0 || line >= getmaxy(win))
		return (FALSE);

	/*--------------------------------------------------------------------
	 * check if first/last have changed
	 */
	if (win->_firstch[line] != _NOCHANGE ||
		win->_lastch [line] != _NOCHANGE)
	{
		return (TRUE);
	}

	return (FALSE);
}

/*------------------------------------------------------------------------
 * is_wintouched() - check if a window has been touched
 */
int is_wintouched (WINDOW *win)
{
	int y;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (FALSE);

	/*--------------------------------------------------------------------
	 * check each line
	 */
	for (y=0; y<getmaxy(win); y++)
	{
		if (win->_firstch[y] != _NOCHANGE ||
			win->_lastch [y] != _NOCHANGE)
		{
			return (TRUE);
		}
	}

	return (FALSE);
}
