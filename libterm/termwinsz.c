/*------------------------------------------------------------------------
 * window size routines
 */
#include "termcommon.h"

/*------------------------------------------------------------------------
 * term_window_check() - check if in a "window" environment
 */
int term_window_check (void)
{
#if V_UNIX
	/* Check for X11 */

	if (getenv("DISPLAY"))
		return (TRUE);

	return (FALSE);
#else
	return (TRUE);
#endif
}

/*------------------------------------------------------------------------
 * term_window_size_get() - get size of window
 */
int term_window_size_get (TERM_DATA *t, int *rows, int *cols)
{
#if V_UNIX
#  if defined(TIOCGWINSZ)
	{
		struct winsize win;

		if ((ioctl(t->tty_out,     TIOCGWINSZ, &win) == 0) ||
		    (ioctl(t->tty_inp,     TIOCGWINSZ, &win) == 0) ||
		    (ioctl(fileno(stderr), TIOCGWINSZ, &win) == 0) )
		{
			if (win.ws_row && win.ws_col)
			{
				*rows = win.ws_row;
				*cols = win.ws_col;

				return (0);
			}
		}
	}
#  endif
#else
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		BOOL bRC;

		bRC = GetConsoleScreenBufferInfo(t->tty_out, &csbi);
		if (bRC)
		{
			*rows = (csbi.srWindow.Bottom - csbi.srWindow.Top ) + 1;
			*cols = (csbi.srWindow.Right  - csbi.srWindow.Left) + 1;

			return (0);
		}
	}
#endif

	return (-1);
}

/*------------------------------------------------------------------------
 * term_window_size_set() - set size of window
 */
int term_window_size_set (TERM_DATA *t, int rows, int cols)
{
	int rc = -1;

#if V_UNIX
#  if defined(TIOCGWINSZ)
	{
		struct winsize win;

		win.ws_row    = rows;
		win.ws_col    = cols;
		win.ws_xpixel = 0;
		win.ws_ypixel = 0;

		rc = ioctl(t->tty_out, TIOCSWINSZ, &win);
	}
#  endif
#else
#endif

	return (rc);
}
