/*------------------------------------------------------------------------
 * print to a window at a specified position
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * mvwprintw() - print to a window at a specified position
 */
int mvwprintw (WINDOW *win, int y, int x, const char *fmt, ...)
{
	va_list args;
	int rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0 || fmt == 0)
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
	rc = vwprintw(win, fmt, args);
	va_end(args);

	return (rc);
}
