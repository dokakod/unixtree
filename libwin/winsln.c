/*------------------------------------------------------------------------
 * insert a line into a window
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * winsertln() - insert a line into a window
 */
int winsertln (WINDOW *win)
{
	int i;
	int j;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * move all lines down one line
	 */
	for (i=getmaxy(win)-1; i>getcury(win); i--)
	{
		for (j=0; j<getmaxx(win); j++)
			win->_y[i][j] = win->_y[i-1][j];

		win->_firstch[i] = 0;
		win->_lastch[i]  = j-1;
	}

	/*--------------------------------------------------------------------
	 * add blank line
	 */
	for (j=0; j<getmaxx(win); j++)
		win->_y[i][j] = getbkgd(win) | getattrs(win) | win->_code;

	win->_firstch[i] = 0;
	win->_lastch[i]  = j-1;

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
