/*------------------------------------------------------------------------
 * end curses routines
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * endscreen() - leave curses environment
 */
int endscreen (SCREEN *s, int exiting)
{
	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (s == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * if in curses env, close it all out
	 */
	if (s->in_curses)
	{
		/*----------------------------------------------------------------
		 * hide mouse if showing
		 */
		scrn_mouse_hide_cursor(s);

		/*----------------------------------------------------------------
		 * clear all slk labels if displayed
		 */
		scrn_slk_clear(s);

		/*----------------------------------------------------------------
		 * reset terminal
		 */
		scrn_cexit(s, exiting);

		/*----------------------------------------------------------------
		 * indicate not in curses env
		 */
		s->in_curses = FALSE;
	}

	return (OK);
}

/*------------------------------------------------------------------------
 * endwin() - leave curses environment
 */
int endscr (int exiting)
{
	return endscreen(win_cur_screen, exiting);
}

/*------------------------------------------------------------------------
 * scrn_isendwin() - check if in curses env or not
 */
int scrn_isendwin (const SCREEN *s)
{
	if (s == 0)
		return (TRUE);

	return (! s->in_curses);
}

/*------------------------------------------------------------------------
 * isendwin() - check if in curses env or not
 */
int isendwin (void)
{
	return scrn_isendwin(win_cur_screen);
}

/*------------------------------------------------------------------------
 * end_curses() - clear out everything and delete all stuff off the heap
 */
void end_curses (void)
{
	while (win_cur_screen != 0)
		delscreen(win_cur_screen);
}
