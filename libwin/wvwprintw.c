/*------------------------------------------------------------------------
 * print to a window with an arg-list
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * vwprintw() - print to a window with an arg-list
 */
int vwprintw (WINDOW *win, const char *fmt, va_list args)
{
	char buffer[BUFSIZ];
	int rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0 || fmt == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * print args to a buffer
	 */
	vsprintf(buffer, fmt, args);

	/*--------------------------------------------------------------------
	 * display the buffer
	 */
	rc = waddstr(win, buffer);

	return (rc);
}
