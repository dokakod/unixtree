/*------------------------------------------------------------------------
 * insert/delete lines from a window
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * winsdelln() - insert or delete n lines from a window
 */
int winsdelln (WINDOW *win, int n)
{
	int i;
	int rc = OK;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * positive number is insert, negative is delete
	 */
	if (n > 0)
	{
		for (i=0; i<n; i++)
		{
			rc = winsertln(win);
			if (rc == ERR)
				break;
		}
	}
	else if (n < 0)
	{
		n = -n;
		for (i=0; i<n; i++)
		{
			rc = wdeleteln(win);
			if (rc == ERR)
				break;
		}
	}

	return (rc);
}
