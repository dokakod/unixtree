/*------------------------------------------------------------------------
 * input args from stdscr at a specified position
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * mvscanw() - input args from stdscr at a specified position
 */
int mvscanw (int y, int x, const char *fmt, ...)
{
	va_list args;
	WINDOW *win;
	int rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	win = stdscr;
	if (win == 0 || fmt == 0 || *fmt == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * move to position
	 */
	rc = wmove(win, y, x);
	if (rc == ERR)
		return (ERR);

	/*--------------------------------------------------------------------
	 * do it
	 */
	va_start(args, fmt);
	rc = vwscanw(win, fmt, args);
	va_end(args);

	return (rc);
}
