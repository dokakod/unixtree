/*------------------------------------------------------------------------
 * save/load a window
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * putwin() - save a window to a stream
 */
int putwin (WINDOW *win, FILE *fp)
{
	int		y;
	int		x;
	chtype	ch;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0 || fp == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * header info
	 */
	put_4byte(fp, WIN_MAGIC);

	put_4byte(fp, win->_maxy);
	put_4byte(fp, win->_maxx);
	put_4byte(fp, win->_begy);
	put_4byte(fp, win->_begx);

	put_4byte(fp, win->_cury);
	put_4byte(fp, win->_curx);
	put_4byte(fp, win->_tmarg);
	put_4byte(fp, win->_bmarg);
	put_4byte(fp, win->_flags);
	put_4byte(fp, win->_clear);
	put_4byte(fp, win->_leaveit);
	put_4byte(fp, win->_scroll);
	put_4byte(fp, win->_immed);
	put_4byte(fp, win->_sync);
	put_4byte(fp, win->_use_keypad);
	put_4byte(fp, win->_use_idc);
	put_4byte(fp, win->_use_idl);
	put_4byte(fp, win->_notimeout);
	put_4byte(fp, win->_stmode);
	put_4byte(fp, win->_delay);
	put_4byte(fp, win->_attrs);
	put_4byte(fp, win->_attro);
	put_4byte(fp, win->_code);
	put_4byte(fp, win->_bkgd);

	/*--------------------------------------------------------------------
	 * now the data
	 *
	 */
	for (y=0; y<win->_maxy; y++)
	{
		for (x=0; x<win->_maxx; x++)
		{
			ch = win->_y[y][x];
			put_4byte(fp, ch);
		}
	}

	return (OK);
}

/*------------------------------------------------------------------------
 * getwin() - load a window from a stream
 */
WINDOW *getwin (FILE *fp)
{
	return scrn_getwin(win_cur_screen, fp);
}

/*------------------------------------------------------------------------
 * scrn_getwin() - load a window from a stream
 */
WINDOW *scrn_getwin (SCREEN *s, FILE *fp)
{
	WINDOW *		win;
	unsigned int	m;
	int				maxy;
	int				maxx;
	int				begy;
	int				begx;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (fp == 0)
		return (0);

	/*--------------------------------------------------------------------
	 * check for magic number
	 */
	m = get_4byte(fp);
	if (m != WIN_MAGIC)
		return ((WINDOW *)NULL);

	/*--------------------------------------------------------------------
	 * header info
	 */
	maxy = get_4byte(fp);
	maxx = get_4byte(fp);
	begy = get_4byte(fp);
	begx = get_4byte(fp);

	/*--------------------------------------------------------------------
	 * create the window
	 */
	win = scrn_newwin(s, maxy, maxx, begy, begx);
	if (win == 0)
		return ((WINDOW *)NULL);

	/*--------------------------------------------------------------------
	 * now read it in
	 */
	win_load(win, fp);

	return (win);
}

/*------------------------------------------------------------------------
 * win_load() - read window info from a stream
 *
 * Note: it is assumed that the window header has already been read
 */
int win_load (WINDOW *win, FILE *fp)
{
	unsigned int	flags;
	int				y;
	int				x;
	chtype			ch;

	/*--------------------------------------------------------------------
	 * read in rest of window data
	 */
	win->_cury			= get_4byte(fp);
	win->_curx			= get_4byte(fp);

	win->_tmarg			= get_4byte(fp);
	win->_bmarg			= get_4byte(fp);

	flags				= get_4byte(fp);
	flags &= ~(_FULLWIN | _FULLINE);
	if ((flags & _ISPAD) == 0)
	{
		if (getmaxx(win) == getmaxx(WIN_CURSCR(win)))
		{
			flags |= _FULLINE;
			if (getmaxy(win) == getmaxy(WIN_CURSCR(win)))
				flags |= _FULLWIN;
		}
	}
	win->_flags			= flags;

	win->_clear			= (int)get_4byte(fp);
	win->_leaveit		= (int)get_4byte(fp);
	win->_scroll		= (int)get_4byte(fp);
	win->_immed			= (int)get_4byte(fp);
	win->_sync			= (int)get_4byte(fp);
	win->_use_keypad	= (int)get_4byte(fp);
	win->_use_idc		= (int)get_4byte(fp);
	win->_use_idl		= (int)get_4byte(fp);
	win->_notimeout		= (int)get_4byte(fp);
	win->_stmode		= (int)get_4byte(fp);

	win->_delay			= (int)get_4byte(fp);

	win->_attrs			= (chtype)get_4byte(fp);
	win->_attro			= (chtype)get_4byte(fp);
	win->_code			= (chtype)get_4byte(fp);
	win->_bkgd			= (chtype)get_4byte(fp);

	/*--------------------------------------------------------------------
	 * read in all lines
	 */
	for (y=0; y<getmaxy(win); y++)
	{
		for (x=0; x<getmaxx(win); x++)
		{
			ch = (chtype)get_4byte(fp);
			win->_y[y][x] = ch;
		}
	}

	/*--------------------------------------------------------------------
	 * indicate it has changed
	 */
	touchwin(win);

	return (OK);
}
