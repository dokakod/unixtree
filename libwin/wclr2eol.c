/*------------------------------------------------------------------------
 * clear a line from current position to end-of-line
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * wclrtoeol() - clear a line from current position to EOL
 */
int wclrtoeol (WINDOW *win)
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
	 * set rest of line to "background"
	 */
	a = (getbkgd(win) | getattrs(win));
	i = getcury(win);
	c = win->_y[i];
	for (j=getcurx(win); j<getmaxx(win); j++)
		c[j] = a;

	/*--------------------------------------------------------------------
	 * adjust first/last entries
	 */
	if (win->_firstch[i] ==_NOCHANGE || win->_firstch[i] > getcurx(win))
		win->_firstch[i] = getcurx(win);
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
