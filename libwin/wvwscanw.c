/*------------------------------------------------------------------------
 * input from a window with an arg-list
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * vwscanw() - input from a window with an arg-list
 *
 * NOTE:	There is no *standard* scan function which takes a va_list.
 *			We gotta write one soon ...
 */
int vwscanw (WINDOW *win, const char *fmt, va_list args)
{
	char buffer[BUFSIZ];
	int rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0 || fmt == 0 || *fmt == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * input chars into buffer
	 */
	wgetnstr(win, buffer, sizeof(buffer));

	/*--------------------------------------------------------------------
	 * scan buffer for args
	 *
	 * TODO: get/write a real vscanf()
	 */
	{
		rc = ERR;
	}

	return (rc);
}
