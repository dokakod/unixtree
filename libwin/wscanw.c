/*------------------------------------------------------------------------
 * input from a window
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * scanw() - input from stdscr
 */
int scanw (const char *fmt, ...)
{
	WINDOW *win;
	va_list args;
	int rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	win = stdscr;
	if (win == 0 || fmt == 0 || *fmt == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * do it
	 */
	va_start(args, fmt);
	rc = vwscanw(win, fmt, args);
	va_end(args);

	return (rc);
}

/*------------------------------------------------------------------------
 * wscanw() - input from a window
 */
int wscanw (WINDOW *win, const char *fmt, ...)
{
	va_list args;
	int rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0 || fmt == 0 || *fmt == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * do it
	 */
	va_start(args, fmt);
	rc = vwscanw(win, fmt, args);
	va_end(args);

	return (rc);
}
