/*------------------------------------------------------------------------
 * extract a string from a window
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * winnstr() - extract a string from a window
 */
int winnstr (WINDOW *win, char *s, int n)
{
	int		i;
	chtype	c;
	int		ch;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0 || s == 0)
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
	{
		c = win->_y[win->_cury][win->_curx+i];
		ch = A_GETTEXT(c);
		*s++ = ch;
	}
	*s = 0;

	return (n);
}
