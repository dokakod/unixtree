/*------------------------------------------------------------------------
 * insert a chtype/string into a window
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * winsch() - insert a char into a window
 */
int winsch (WINDOW *win, chtype ch)
{
	int c;
	int x;
	int y;
	int k;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * cannot insert non-printable chars
	 */
	c = A_GETTEXT(ch);
	if (c < ' ' || c > 0xff)
	{
		return (ERR);
	}

	/*--------------------------------------------------------------------
	 * get current position in window
	 */
	y = getcury(win);
	x = getcurx(win);

	/*--------------------------------------------------------------------
	 * move all existing chars over one position
	 */
	for (k=getmaxx(win)-2; k>=x; k--)
		win->_y[y][k+1] = win->_y[y][k];

	/*--------------------------------------------------------------------
	 * put char in position
	 */
	win->_y[y][x] = ch | getattrs(win) | win->_code;

	/*--------------------------------------------------------------------
	 * update first/last entries
	 */
	if (win->_firstch[y]==_NOCHANGE || win->_firstch[y]>x)
		win->_firstch[y] = x;
	win->_lastch[y] = getmaxx(win)-1;

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

/*------------------------------------------------------------------------
 * winsnstr() - insert a string into a window
 */
int winsnstr (WINDOW *win, const char *s, int n)
{
	int i;
	int rc = OK;

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
	 * check if n is > than length of string
	 */
	for (i=0; i<n; i++)
	{
		if (s[i] == 0)
			break;
	}

	/*--------------------------------------------------------------------
	 * now insert all chars (in reverse order, since we are inserting)
	 */
	for (i--; i>=0; i--)
	{
		rc = winsch(win, s[i]);
		if (rc == ERR)
			break;
	}

	return (rc);
}
