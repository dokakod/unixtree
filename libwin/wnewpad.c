/*------------------------------------------------------------------------
 * pad creation routines
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * create_pad() - internal pad create routine
 */
static WINDOW *create_pad (SCREEN *s, int nlines, int ncols, int y, int x)
{
	WINDOW *w;

	/*--------------------------------------------------------------------
	 * create the window
	 */
	w = scrn_create_win(s, nlines, ncols, y, x);
	if (w == 0)
		return ((WINDOW *)NULL);

	/*--------------------------------------------------------------------
	 * adjust values
	 */
	w->_flags = _ISPAD;

	return (w);
}

/*------------------------------------------------------------------------
 * newpad() - create a new pad
 */
WINDOW *newpad (int nlines, int ncols)
{
	return pad_new(win_cur_screen, nlines, ncols);
}

/*------------------------------------------------------------------------
 * pad_new() - create a new pad
 */
WINDOW *pad_new (SCREEN *s, int nlines, int ncols)
{
	WINDOW *w;
	int i;
	int j;
	chtype c;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (s == 0)
		return ((WINDOW *)NULL);

	/*--------------------------------------------------------------------
	 * create the pad
	 */
	w = create_pad(s, nlines, ncols, 0, 0);
	if (w == 0)
		return ((WINDOW *)NULL);

	/*--------------------------------------------------------------------
	 * allocate lines & clear them
	 */
	c = (getbkgd(w) | getattrs(w) | w->_code);
	for (i=0; i<nlines; i++)
	{
		w->_y[i] = (chtype *)MALLOC(ncols * sizeof(chtype));
		if (w->_y[i] == 0)
		{
			delwin(w);
			return ((WINDOW *)NULL);
		}

		for (j=0; j<ncols; j++)
			w->_y[i][j] = c;
	}

	return (w);
}

/*------------------------------------------------------------------------
 * subpad() - create a sub-pad
 */
WINDOW *subpad (WINDOW *p, int nlines, int ncols, int begin_y, int begin_x)
{
	WINDOW *w;
	int i;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (p == 0 || (p->_flags & _ISPAD) == 0)
		return ((WINDOW *)0);

	if (nlines < 0 || ncols < 0 || begin_y < 0 || begin_x < 0)
		return ((WINDOW *)0);

	/*--------------------------------------------------------------------
	 * adjust args
	 */
	if (nlines == 0)
		nlines = getmaxy(p) - begin_y;

	if (ncols == 0)
		ncols  = getmaxx(p) - begin_x;

	if ((nlines + begin_y) > getmaxy(p) || (ncols + begin_x) > getmaxx(p))
		return ((WINDOW *)0);

	/*--------------------------------------------------------------------
	 * create the sub-pad
	 */
	w = create_pad(WIN_SCREEN(p), nlines, ncols, begin_y, begin_x);
	if (w == 0)
		return ((WINDOW *)0);

	/*--------------------------------------------------------------------
	 * copy parent info into it
	 */
	w->_pary		= begin_y;
	w->_parx		= begin_x;

	w->_clear		= p->_clear;
	w->_leaveit		= p->_leaveit;
	w->_scroll		= p->_scroll;
	w->_immed		= p->_immed;
	w->_sync		= p->_sync;
	w->_use_keypad	= p->_use_keypad;
	w->_use_idc		= p->_use_idc;
	w->_use_idl		= p->_use_idl;
	w->_notimeout	= p->_notimeout;
	w->_stmode		= p->_stmode;

	w->_code		= p->_code;
	w->_attrs		= p->_attrs;
	w->_attro		= p->_attro;
	w->_bkgd		= p->_bkgd;

	w->_delay		= p->_delay;

	for (i=0; i<nlines; i++)
		w->_y[i] = p->_y[i+begin_y] + begin_x;

	w->_parent		= p;
	w->_screen		= p->_screen;

	wsyncdown(w);

	/*--------------------------------------------------------------------
	 * update parent descendent count
	 */
	p->_ndescs++;

	return (w);
}
