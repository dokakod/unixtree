/*------------------------------------------------------------------------
 * clear window from current position to end-of-window
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * wclrtobot() - clear a window from current position to EOS
 */
int wclrtobot (WINDOW *win)
{
	int i;
	int j;
	chtype *c;
	chtype a;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * clear current line
	 */
	wclrtoeol(win);

	/*--------------------------------------------------------------------
	 * now do rest of window
	 */
	a = (getbkgd(win) | getattrs(win));
	for (i=getcury(win)+1; i<getmaxy(win); i++)
	{
		/*----------------------------------------------------------------
		 * set line to all "background"
		 */
		c = win->_y[i];
		for (j=0; j<getmaxx(win); j++)
			c[j] = a;

		/*----------------------------------------------------------------
		 * adjust first/last entries
		 */
		win->_firstch[i] = 0;
		win->_lastch[i]  = j-1;
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
