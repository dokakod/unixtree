/*------------------------------------------------------------------------
 * print to stdscr at specified position
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * mvprintw() - print to stdscr at specified position
 */
int mvprintw (int y, int x, const char *fmt, ...)
{
	va_list args;
	WINDOW *win;
	int rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	win = stdscr;
	if (win == 0 || fmt == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * move to position
	 */
	rc = wmove(win, y, x);
	if (rc == ERR)
		return (ERR);

	/*--------------------------------------------------------------------
	 * do the print
	 */
	va_start(args, fmt);
	rc = vwprintw(win, fmt, args);
	va_end(args);

	return (rc);
}
