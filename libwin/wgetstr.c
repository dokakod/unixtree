/*------------------------------------------------------------------------
 * string input routines
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * wgetstr() - get a string in a window
 */
int wgetstr (WINDOW *win, char *s)
{
	int c;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * collect chars until CR is entered
	 */
	while (TRUE)
	{
		c = wgetch(win);
		if (c == EOF || c == '\r' || c == '\n')
			break;
		*s++ = c;
	}
	*s = 0;

	return (OK);
}

/*------------------------------------------------------------------------
 * wgetnstr() - get at most n chars from a window
 */
int wgetnstr (WINDOW *win, char *s, int n)
{
	int c;
	int i;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * collect chars until either n chars are input or a CR is entered
	 */
	for (i=0; i<n; i++)
	{
		c = wgetch(win);
		if (c == EOF || c == '\r' || c == '\n')
			break;
		*s++ = c;
	}
	*s = 0;

	return (OK);
}
