/*------------------------------------------------------------------------
 * print to a window
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * printw() - print to stdscr
 */
int printw (const char *fmt, ...)
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
	 * do it
	 */
	va_start(args, fmt);
	rc = vwprintw(win, fmt, args);
	va_end(args);

	return (rc);
}

/*------------------------------------------------------------------------
 * wprintw() - print to a window
 */
int wprintw (WINDOW *win, const char *fmt, ...)
{
	va_list args;
	int rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0 || fmt == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * do it
	 */
	va_start(args, fmt);
	rc = vwprintw(win, fmt, args);
	va_end(args);

	return (rc);
}
