/*------------------------------------------------------------------------
 * duplicate a window
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * dupwin() - duplicate a window
 */
WINDOW *dupwin (WINDOW *win)
{
	WINDOW *w;
	int i;
	int j;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (0);

	/*--------------------------------------------------------------------
	 * create the new window
	 */
	if (win->_parent)
	{
		/*----------------------------------------------------------------
		 * copy a subwin
		 */
		w = derwin(win->_parent, getmaxy(win), getmaxx(win),
				getpary(win), getparx(win));
		if (w == 0)
			return ((WINDOW *)NULL);
	}
	else
	{
		/*----------------------------------------------------------------
		 * copy a full window
		 */
		w = scrn_newwin(WIN_SCREEN(win), getmaxy(win), getmaxx(win),
			getbegy(win), getbegx(win));
		if (w == 0)
			return ((WINDOW *)NULL);

		for (i=0; i<getmaxy(win); i++)
		{
			for (j=0; j<getmaxx(win); j++)
				w->_y[i][j] = win->_y[i][j];
		}
	}

	/*--------------------------------------------------------------------
	 * now copy orig window stuff into it
	 */
	w->_cury		= win->_cury;
	w->_curx		= win->_curx;
	w->_tmarg		= win->_tmarg;
	w->_bmarg		= win->_bmarg;
	w->_flags		= win->_flags;
	w->_clear		= win->_clear;
	w->_leaveit		= win->_leaveit;
	w->_scroll		= win->_scroll;
	w->_immed		= win->_immed;
	w->_sync		= win->_sync;
	w->_use_keypad	= win->_use_keypad;
	w->_use_idc		= win->_use_idc;
	w->_use_idl		= win->_use_idl;
	w->_stmode		= win->_stmode;
	w->_delay		= win->_delay;
	w->_attrs		= win->_attrs;
	w->_attro		= win->_attro;
	w->_code		= win->_code;
	w->_bkgd		= win->_bkgd;
	for (i=0; i<getmaxy(win); i++)
	{
		w->_firstch[i] = win->_firstch[i];
		w->_lastch[i]  = win->_lastch[i];
	}

	return (w);
}
