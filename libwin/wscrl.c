/*------------------------------------------------------------------------
 * scroll routines
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * wsetscrreg() - set a scrolling region in a window
 */
int wsetscrreg (WINDOW *win, int t, int b)
{
	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	if (b < t)
		return (ERR);

	if (t < 0 || t >= getmaxy(win) ||
		b < 0 || b >= getmaxy(win) )
		return (ERR);

	/*--------------------------------------------------------------------
	 * store values
	 */
	win->_tmarg = t;
	win->_bmarg = b;

	return (OK);
}

/*------------------------------------------------------------------------
 * wscrl() - scroll a window n lines
 */
int wscrl (WINDOW *win, int n)
{
	int i;
	int j;
	int r;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	if (! getscroll(win))
		return (ERR);

	/*--------------------------------------------------------------------
	 * scroll up or down
	 */
	r = win->_bmarg - win->_tmarg + 1;

	if (n > 0)
	{
		/* scroll up n lines */

		if (n > r)
			n = r;

		if (n < r)
		{
			for (i=win->_tmarg; i<=win->_bmarg-n; i++)
			{
				for (j=0; j<getmaxx(win); j++)
					win->_y[i][j] = win->_y[i+n][j];
				win->_firstch[i] = 0;
				win->_lastch[i]  = j-1;
			}
		}
		for (i=win->_bmarg-n+1; i<=win->_bmarg; i++)
		{
			chtype c = getbkgd(win) | getattrs(win) | win->_code;

			for (j=0; j<getmaxx(win); j++)
				win->_y[i][j] = c;
			win->_firstch[i] = 0;
			win->_lastch[i]  = j-1;
		}
	}
	else if (n < 0)
	{
		/* scroll down n lines */

		n = -n;
		if (n > r)
			n = r;

		if (n < r)
		{
			for (i=win->_bmarg-n; i>=win->_tmarg; i--)
			{
				for (j=0; j<getmaxx(win); j++)
					win->_y[i][j] = win->_y[i-n][j];
				win->_firstch[i] = 0;
				win->_lastch[i]  = j-1;
			}
		}
		for (i=win->_tmarg; i<win->_tmarg+n; i++)
		{
			chtype c = getbkgd(win) | getattrs(win) | win->_code;

			for (j=0; j<getmaxx(win); j++)
				win->_y[i][j] = c;
			win->_firstch[i] = 0;
			win->_lastch[i]  = j-1;
		}
	}

	/*--------------------------------------------------------------------
	 * if auto-sync in effect, sync window with its parents
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
