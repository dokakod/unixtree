/*------------------------------------------------------------------------
 * pad refresh
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * pnoutrefreshln() - refresh a line of a pad
 */
static int pnoutrefreshln (WINDOW *p, int i, int pminrow, int pmincol,
	int sminrow, int smincol, int smaxrow, int smaxcol)
{
	int f = 0;
	int l = 0;
	int j;
	int k;
	int first;
	int last;
	int changed;
	chtype *src;
	chtype *dst;
	WINDOW *cs = WIN_CURSCR(p);

	first = p->_firstch[i + sminrow];
	last  = p->_lastch [i + sminrow];

	if (first == _NOCHANGE)
		return (FALSE);

	if (first > smaxcol)
		return (FALSE);
	if (first < smincol)
		first = smincol;

	if (last < smincol)
		return (FALSE);
	if (last > smaxcol)
		last = smaxcol;

	k = i + pminrow;
	src = p->_y[i + sminrow];
	dst = cs->_y[k] + pmincol;

	changed = FALSE;
	for (j=first; j<last; j++)
	{
		if (dst[j] != src[j])
		{
			dst[j] = src[j];
			if (! changed)
				f = j;
			l = j;
			changed = TRUE;
		}
	}

	if (changed)
	{
		if (cs->_firstch[k] == _NOCHANGE || cs->_firstch[k] > f+pmincol)
		{
			cs->_firstch[k] = f+pmincol;
		}

		if (cs->_lastch[k]  == _NOCHANGE || cs->_lastch[k]  < l+pmincol)
		{
			cs->_lastch[k] = l+pmincol;
		}
	}

	p->_firstch[i] = p->_lastch[i] = _NOCHANGE;

	return (changed);
}

/*------------------------------------------------------------------------
 * pnoutrefresh() - refresh a pad with no output
 */
int pnoutrefresh (WINDOW *p, int pminrow, int pmincol,
	int sminrow, int smincol, int smaxrow, int smaxcol)
{
	int nrows;
	int ncols;
	int i;
	int changed;

	if (p == 0 || (p->_flags & _ISPAD) == 0)
		return (ERR);

	if (pminrow < 0)	pminrow = 0;
	if (pmincol < 0)	pmincol = 0;
	if (sminrow < 0)	sminrow = 0;
	if (smincol < 0)	smincol = 0;

	nrows = smaxrow - sminrow;
	ncols = smaxcol - smincol;

	if (nrows <= 0 || ncols <= 0)
		return (ERR);

	if ((pminrow + nrows) > getmaxy(p) || (pmincol + ncols) > getmaxx(p))
		return (ERR);

	if (smaxrow > getmaxy(WIN_CURSCR(p)) || smaxcol > getmaxx(WIN_CURSCR(p)))
		return (ERR);

	changed = FALSE;
	for (i=0; i<nrows; i++)
	{
		if (pnoutrefreshln(p, i, pminrow, pmincol, sminrow, smincol,
			smaxrow, smaxcol))
		{
			changed = TRUE;
		}
	}

	return (changed);
}

/*------------------------------------------------------------------------
 * prefresh() - refresh a pad
 */
int prefresh (WINDOW *p, int pminrow, int pmincol,
	int sminrow, int smincol, int smaxrow, int smaxcol)
{
	if (p == 0 || (p->_flags & _ISPAD) == 0)
		return (ERR);

	WIN_SCREEN(p)->in_refresh = TRUE;

	if (pnoutrefresh(p, pminrow, pmincol, sminrow, smincol,
		smaxrow, smaxcol) > 0)
	{
		scrn_doupdate(WIN_SCREEN(p));
	}

	p->_ppy = pminrow;
	p->_ppx = pmincol;
	p->_psy = sminrow;
	p->_psx = smincol;
	p->_ply = smaxrow;
	p->_plx = smaxcol;

	WIN_SCREEN(p)->in_refresh = FALSE;

	return (OK);
}

/*------------------------------------------------------------------------
 * pechochar() - output a char to a pad & display the pad at previous position
 */
int pechochar (WINDOW *p, chtype ch)
{
	int cury;
	int curx;

	if (p == 0 || (p->_flags & _ISPAD) == 0 || p->_ppy == ERR)
		return (ERR);

	cury = getcury(p);
	curx = getcurx(p);

	if (waddch(p, ch) == ERR)
		return (ERR);

	if (cury >= p->_ppy && cury < (p->_ppy + (p->_ply - p->_psy)) &&
	    curx >= p->_ppx && curx < (p->_ppx + (p->_plx - p->_psx)) )
	{
		int changed;

		changed = pnoutrefreshln(p, cury, p->_ppy, p->_ppx, p->_psy, p->_psx,
			p->_ply, p->_plx);

		if (changed)
			scrn_doupdateln(WIN_SCREEN(p), cury - p->_ppy + p->_psy);
	}

	return (OK);
}
