/*------------------------------------------------------------------------
 * sub-window routines
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * subwin() - create a sub-window at an absoute position
 */
WINDOW *subwin (WINDOW *p, int rows, int cols, int y, int x)
{
	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (p == 0)
		return ((WINDOW *)0);

	/*--------------------------------------------------------------------
	 * do it
	 */
	return (derwin(p, rows, cols, y-getbegy(p), x-getbegx(p)));
}

/*------------------------------------------------------------------------
 * derwin() - create a sub-window at a relative position
 */
WINDOW *derwin (WINDOW *p, int rows, int cols, int y, int x)
{
	WINDOW *w;
	int i;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (p == 0 || p->_flags & _ISPAD)
		return ((WINDOW *)0);

	if (y < 0 || x < 0 || rows < 0 || cols < 0)
		return ((WINDOW *)0);

	/*--------------------------------------------------------------------
	 * adjust & check rows & cols
	 */
	if (rows == 0)
		rows = getmaxy(p)-y;

	if (cols == 0)
		cols = getmaxx(p)-x;

	if ((y+rows) > getmaxy(p) || (x+cols) > getmaxx(p))
		return ((WINDOW *)0);

	/*--------------------------------------------------------------------
	 * create a window
	 */
	w = scrn_create_win(WIN_SCREEN(p), rows,cols,y+getbegy(p),x+getbegx(p));
	if (w == 0)
		return ((WINDOW *)0);

	/*--------------------------------------------------------------------
	 * copy parent info into it
	 */
	w->_pary		= y;
	w->_parx		= x;

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

	for (i=0; i<rows; i++)
		w->_y[i] = p->_y[i+y] + x;

	w->_parent		= p;
	w->_screen		= p->_screen;

	wsyncdown(w);

	/*--------------------------------------------------------------------
	 * bump parent descendent count
	 */
	p->_ndescs++;

	return (w);
}

/*------------------------------------------------------------------------
 * mvderwin() - move a sub-window
 */
int mvderwin (WINDOW *w, int par_y, int par_x)
{
	WINDOW *	p;
	int			i;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (w == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * error if not a sub-window
	 */
	p = w->_parent;
	if (p == 0)
	{
		return (ERR);
	}

	/*--------------------------------------------------------------------
	 * error if a pad
	 */
	if (w->_flags & _ISPAD)
		return (ERR);

	/*--------------------------------------------------------------------
	 * check if new position still fits in parent
	 */
	if ((getmaxy(w) + par_y) > getmaxy(p) ||
	    (getmaxx(w) + par_x) > getmaxx(p))
	{
		return (ERR);
	}

	/*--------------------------------------------------------------------
	 * adjust window
	 */
	w->_pary = par_y;
	w->_parx = par_x;

	for (i=0; i<getmaxy(w); i++)
		w->_y[i] = p->_y[i+par_y] + par_x;

	/*--------------------------------------------------------------------
	 * indicate window changed
	 */
	touchwin(w);

	return (0);
}
