/*------------------------------------------------------------------------
 * extract a string from a window
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * winchnstr() - extract a string from a window
 */
int winchnstr (WINDOW *win, chtype *s, int n)
{
	int i;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * adjust number of chars (-1 implies to EOL)
	 */
	if (n < 0 || n > (getmaxx(win) - getcurx(win)))
		n = getmaxx(win) - getcurx(win);

	/*--------------------------------------------------------------------
	 * get chars
	 */
	for (i=0; i<n; i++)
		*s++ = win->_y[win->_cury][win->_curx+i];
	*s = 0;

	return (OK);
}
