/*------------------------------------------------------------------------
 * move a window
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * mvwin() - move a window
 */
int mvwin (WINDOW *win, int y, int x)
{
	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * must be a sub-window
	 */
	if (win->_parent == 0)
	{
		return (ERR);
	}

	/*--------------------------------------------------------------------
	 * check if window fits at new position
	 */
	if (y < 0 || (y+getmaxy(win)) > getmaxy(WIN_CURSCR(win)))
		return (ERR);

	if (x < 0 || (x+getmaxx(win)) > getmaxx(WIN_CURSCR(win)))
		return (ERR);

	/*--------------------------------------------------------------------
	 * adjust window position
	 */
	setbegy(win, y);
	setbegx(win, x);

	/*--------------------------------------------------------------------
	 * indicate whether it has changed
	 */
	untouchwin(win);
	wsyncdown(win);

	return (OK);
}
