/*------------------------------------------------------------------------
 * window create routines
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * scrn_create_win() - internal routine to create a window
 */
WINDOW *scrn_create_win (SCREEN *s, int rows, int cols, int y, int x)
{
	WINDOW *	w;
	WINLIST *	wl;
	int			i;
	int			max_x = scrn_get_max_cols(s);
	int			max_y = scrn_get_max_rows(s);

	/*--------------------------------------------------------------------
	 * allocate window struct
	 */
	w = (WINDOW *)MALLOC(sizeof(WINDOW));
	if (w == 0)
		return ((WINDOW *)NULL);
	memset(w, 0, sizeof(*w));

	/*--------------------------------------------------------------------
	 * allocate line pointers
	 */
	w->_y = (chtype **)MALLOC(rows * sizeof(*w->_y));
	if (w->_y == 0)
	{
		delwin(w);
		return ((WINDOW *)NULL);
	}

	/*--------------------------------------------------------------------
	 * allocate first array
	 */
	w->_firstch = (int *)MALLOC(rows * sizeof(*w->_firstch));
	if (w->_firstch == 0)
	{
		delwin(w);
		return ((WINDOW *)NULL);
	}

	/*--------------------------------------------------------------------
	 * allocate last array
	 */
	w->_lastch = (int *)MALLOC(rows * sizeof(*w->_lastch));
	if (w->_lastch == 0)
	{
		delwin(w);
		return ((WINDOW *)NULL);
	}

	/*--------------------------------------------------------------------
	 * window geometery
	 */
	w->_maxy		= rows;
	w->_maxx		= cols;
	w->_begy		= y;
	w->_begx		= x;
	w->_cury		= 0;
	w->_curx		= 0;
	w->_pary		= ERR;
	w->_parx		= ERR;
	w->_tmarg		= 0;
	w->_bmarg		= rows-1;

	/*--------------------------------------------------------------------
	 * pad display info
	 */
	w->_ppy			= ERR;
	w->_ppx			= ERR;
	w->_psy			= ERR;
	w->_psx			= ERR;
	w->_ply			= ERR;
	w->_plx			= ERR;

	/*--------------------------------------------------------------------
	 * window flags
	 */
	w->_flags		= 0;

	if ((x + cols) == max_x)
	{
		w->_flags |= _ENDLINE;
	}

	if (x == 0 && cols == max_x)
	{
		w->_flags |= _FULLINE;
	}

	if (x == 0 && y == 0 && rows == max_y && cols == max_x)
	{
		w->_flags |= _FULLWIN;
	}

	w->_clear		= FALSE;
	w->_leaveit		= FALSE;
	w->_scroll		= FALSE;
	w->_immed		= FALSE;
	w->_sync		= FALSE;
	w->_use_keypad	= TRUE;
	w->_use_idc		= TRUE;
	w->_use_idl		= TRUE;
	w->_notimeout	= FALSE;
	w->_stmode		= FALSE;

	/*--------------------------------------------------------------------
	 * window display data
	 */
	scrn_get_default_attrs(s, &w->_attrs, &w->_attro, scrn_has_colors(s),
		TRUE);
	w->_code		= 0;
	w->_bkgd		= ' ';

	/*--------------------------------------------------------------------
	 * keyboard input data
	 */
	w->_delay		= -1;

	/*--------------------------------------------------------------------
	 * window line data
	 */
	for (i=0; i<rows; i++)
	{
		w->_y[i]		= 0;
		w->_firstch[i]	= _NOCHANGE;
		w->_lastch[i]	= _NOCHANGE;
	}

	/*--------------------------------------------------------------------
	 * sub-window data
	 */
	w->_parent		= 0;
	w->_ndescs		= 0;

	/*--------------------------------------------------------------------
	 * screen data
	 */
	w->_screen		= s;

	/*--------------------------------------------------------------------
	 * now hook window into list
	 */
	wl = (WINLIST *)MALLOC(sizeof(*wl));
	if (wl != 0)
	{
		wl->next	= s->winlist;
		wl->prev	= 0;
		wl->win		= w;

		s->winlist = wl;

		if (wl->next != 0)
			(wl->next)->prev = wl;
	}

	return (w);
}

/*------------------------------------------------------------------------
 * newwin() - create a new window
 */
WINDOW *newwin (int rows, int cols, int y, int x)
{
	return scrn_newwin(win_cur_screen, rows, cols, y, x);
}

/*------------------------------------------------------------------------
 * scrn_newwin() - create a new window
 */
WINDOW *scrn_newwin (SCREEN *s, int rows, int cols, int y, int x)
{
	WINDOW *	w;
	int			i;
	int			j;
	chtype		c;
	int			max_x;
	int			max_y;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (s == 0)
		return ((WINDOW *)NULL);

	max_x = scrn_get_max_cols(s);
	max_y = scrn_get_max_rows(s);

	if ((y + rows) > max_y || (x + cols) > max_x)
		return ((WINDOW *)NULL);

	if (rows == 0)
		rows = max_y - y;

	if (cols == 0)
		cols = max_x - x;

	/*--------------------------------------------------------------------
	 * create the window
	 */
	w = scrn_create_win(s, rows, cols, y, x);
	if (w == 0)
		return ((WINDOW *)NULL);

	/*--------------------------------------------------------------------
	 * allocate the lines & clear them
	 */
	c = (getbkgd(w) | getattrs(w) | w->_code);
	for (i=0; i<rows; i++)
	{
		w->_y[i] = (chtype *)MALLOC(cols * sizeof(chtype));
		if (w->_y[i] == 0)
		{
			delwin(w);
			return ((WINDOW *)NULL);
		}

		for (j=0; j<cols; j++)
			w->_y[i][j] = c;
	}

	return (w);
}
