/*------------------------------------------------------------------------
 * move to a position in a window
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * wmove() - move position in a window
 */
int wmove (WINDOW *win, int y, int x)
{
	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	if (y < 0 || y >= getmaxy(win))
		return (ERR);
	if (x < 0 || x >= getmaxx(win))
		return (ERR);

	/*--------------------------------------------------------------------
	 * update position
	 */
	setcury(win, y);
	setcurx(win, x);

	return (OK);
}
