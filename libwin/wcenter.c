/*------------------------------------------------------------------------
 * add a string centered on a line
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * wcenter() - add a string centered on a line
 */
int wcenter (WINDOW *win, const char *str, int y)
{
	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0 || str == 0)
		return (ERR);

	return wccenter(win, str, y, win->_code);
}

/*------------------------------------------------------------------------
 * wccenter() - add a string centered on a line with a code
 */
int wccenter (WINDOW *win, const char *str, int y, int code)
{
	int l;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0 || str == 0)
		return (ERR);

	l = strlen(str);
	if (l > getmaxx(win))
		return (ERR);

	/*--------------------------------------------------------------------
	 * move to proper starting position
	 */
	if (wmove(win, y, (getmaxx(win)-l)/2))
		return (ERR);

	/*--------------------------------------------------------------------
	 * now add the string
	 */
	return (wcaddstr(win, str, code));
}
