/*------------------------------------------------------------------------
 * background routines
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * wbkgdset() - set character to use for background
 */
void wbkgdset (WINDOW *win, chtype c)
{
	if (win != 0)
	{
		chtype ch = A_GETTEXT(c);
		chtype at = A_GETATTR(c);

		if (ch < ' ')
			ch = ' ';
		setbkgd(win, ch);

		wattrset(win, at);
	}
}

/*------------------------------------------------------------------------
 * wbkgd() - change the background char for all spaces in the window
 */
int wbkgd (WINDOW *win, chtype c)
{
	int		y;
	int		x;
	chtype	k;
	chtype	t;
	chtype	ch;
	attr_t	a;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * set the background
	 */
	wbkgdset(win, c);
	a = wattrget(win);
	c = getbkgd(win);

	/*--------------------------------------------------------------------
	 * now go through the window, changing all spaces
	 */
	for (y=0; y<getmaxy(win); y++)
	{
		for (x=0; x<getmaxx(win); x++)
		{
			ch = wchat(win, y, x);
			t = A_GETTEXT(ch);
			k = A_GETCODE(ch);
			if (t == ' ')
				t = c;
			woutch(win, y, x, a | k | t);
		}
	}

	/*--------------------------------------------------------------------
	 * show the window changed
	 */
	touchwin(win);

	/*--------------------------------------------------------------------
	 * if auto-sync in effect, sync window with its parents
	 */
	if (getsync(win))
		wsyncup(win);

	/*--------------------------------------------------------------------
	 * if immed in effect, do a refresh
	 */
	if (getimmed(win))
		return wrefresh(win);

	return (OK);
}
