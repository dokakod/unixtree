/*------------------------------------------------------------------------
 * curses initialization routines
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * win_clear() - clear out a window
 */
void win_clear (WINDOW *win, chtype ch)
{
	if (win != 0)
	{
		int y;
		int x;

		/*----------------------------------------------------------------
		 * indicate attributes of window
		 */
		wattrset(win, A_GETATTR(ch));

		/*----------------------------------------------------------------
		 * set win to specified char
		 */
		for (y=0; y<getmaxy(win); y++)
		{
			for (x=0; x<getmaxx(win); x++)
			{
				woutch(win, y, x, ch);
			}
		}

		touchwin(win);
	}
}

/*------------------------------------------------------------------------
 * initscr() - initialize curses
 */
WINDOW *initscr (void)
{
	SCREEN * s	= win_cur_screen;

	/*--------------------------------------------------------------------
	 * create default screen if not already there
	 */
	if (s == 0)
	{
		s = newterm(0, 0, 0);
		if (s == 0)
			return (0);
	}

	return (SCR_STDSCR(s));
}
