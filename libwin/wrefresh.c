/*------------------------------------------------------------------------
 * refresh routines
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * win_key_repaint() - async key routine for KEY_REFRESH
 */
int win_key_repaint (int key, void *data)
{
	SCREEN * s	= (SCREEN *)data;
	WINDOW * cs	= SCR_CURSCR(s);

	wrefresh(cs);
	return (OK);
}

/*------------------------------------------------------------------------
 * wrefresh() - refresh a window
 */
int wrefresh (WINDOW *win)
{
	SCREEN *	s;
	WIN_CLOCK *	wc;
	time_t		clk;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0 || win->_flags & _ISPAD)
		return (ERR);

	s = WIN_SCREEN(win);

	/*--------------------------------------------------------------------
	 * indicate we are in a refresh
	 */
	s->in_refresh = TRUE;

	/*--------------------------------------------------------------------
	 * now refresh the window & output it if requested
	 */
	if (wnoutrefresh(win) || ! getleave(win))
		scrn_doupdate(s);

	/*--------------------------------------------------------------------
	 * indicate we are not in a refresh
	 */
	s->in_refresh = FALSE;

	/*--------------------------------------------------------------------
	 * check if a deferred clock needs to be output
	 */
	wc = SCR_CLOCK(s);
	clk = wc->clk_defer;
	if (clk)
	{
		wc->clk_defer = 0;
		win_clock_show(clk, wc->clk_win);
	}

	return (OK);
}

/*------------------------------------------------------------------------
 * wnoutrefresh() - refresh a window with no output
 */
int wnoutrefresh (WINDOW *win)
{
	int i;
	int changed;
	WINDOW *cs;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0 || win->_flags & _ISPAD)
		return (FALSE);

	/*--------------------------------------------------------------------
	 *	If we are refreshing curscr, this means to just output
	 *	the entire screen.
	 */
	cs = WIN_CURSCR(win);
	if (win == cs)
	{
		touchwin(cs);
		clearok(cs, TRUE);
		return (TRUE);
	}

	/*--------------------------------------------------------------------
	 * sync down from all parents
	 */
	wsyncdown(win);

	/*--------------------------------------------------------------------
	 * check if a clear is requested
	 */
	if (getclear(win) && (win->_flags & _FULLWIN))
	{
		clearok(cs, TRUE);
		clearok(win, FALSE);
	}

	/*--------------------------------------------------------------------
	 * now refresh each line
	 */
	changed = FALSE;
	for (i=0; i<getmaxy(win); i++)
	{
		if (wnoutrefreshln(win, i))
			changed = TRUE;
	}

	/*--------------------------------------------------------------------
	 * check if cursor should be moved back
	 */
	if (! getleave(win))
	{
		leaveok(cs, FALSE);
		setcury(cs, getbegy(win) + getcury(win));
		setcurx(cs, getbegx(win) + getcurx(win));
	}

	return (changed);
}

/*------------------------------------------------------------------------
 * wrefreshln() - refresh a line in a window
 */
int wrefreshln (WINDOW *win, int i)
{
	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * refresh the line & output it if changed
	 */
	if (wnoutrefreshln(win, i))
		scrn_doupdateln(WIN_SCREEN(win), i);

	return (OK);
}

/*------------------------------------------------------------------------
 * wrefreshln() - refresh a line in a window with no output
 */
int wnoutrefreshln (WINDOW *win, int i)
{
	int			y;
	int			x;
	int			k;
	int			j;
	int			f = 0;
	int			l = 0;
	int			last;
	chtype *	src;
	chtype *	dst;
	int			changed;
	WINDOW *	cs;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0 || win->_flags & _ISPAD)
		return (FALSE);

	/*--------------------------------------------------------------------
	 * bail if no change
	 */
	if (win->_firstch[i] == _NOCHANGE)
		return (FALSE);

	/*--------------------------------------------------------------------
	 * get line info
	 */
	cs = WIN_CURSCR(win);
	if ((last = win->_lastch[i]) >= getmaxx(win)-1)
		last = getmaxx(win)-1;

	y = getbegy(win);
	x = getbegx(win);
	k = i+y;
	src = win->_y[i];
	dst = cs->_y[k] + x;

	/*--------------------------------------------------------------------
	 * update all chars on line
	 */
	changed = FALSE;
	for (j=win->_firstch[i]; j<=last; j++)
	{
		if (dst[j] != src[j])
		{
			dst[j] = src[j];
			if (! changed)
				f = j;
			l = j;
			changed = TRUE;
		}
	}

	/*--------------------------------------------------------------------
	 * update first/last entries in curscr
	 */
	if (changed)
	{
		if (cs->_firstch[k] == _NOCHANGE || cs->_firstch[k] > f+x)
			cs->_firstch[k] = f + x;

		if (cs->_lastch [k] == _NOCHANGE || cs->_lastch [k] < l+x)
			cs->_lastch [k] = l + x;
	}

	/*--------------------------------------------------------------------
	 * clear first/last entries
	 */
	win->_firstch[i] = win->_lastch[i] = _NOCHANGE;

	return (changed);
}
