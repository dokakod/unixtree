/*------------------------------------------------------------------------
 * delete a char from a window
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * wdelch() - delete the current char from a window
 */
int wdelch (WINDOW *win)
{
	int i;
	int j;
	int k;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * move all chars over one position
	 */
	i = getcury(win);
	j = getcurx(win);
	for (k=j; k<getmaxx(win)-1; k++)
		win->_y[i][k] = win->_y[i][k+1];

	/*--------------------------------------------------------------------
	 * add a "background" char at end
	 */
	win->_y[i][k] = getbkgd(win) | getattrs(win) | win->_code;

	/*--------------------------------------------------------------------
	 * adjust first/last entries
	 */
	if (win->_firstch[i]==_NOCHANGE || win->_firstch[i]>j)
		win->_firstch[i] = j;
	win->_lastch[i] = k;

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
