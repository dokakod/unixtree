/*------------------------------------------------------------------------
 * overlapped window routines
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * overlap() - overlap a window
 */
int overlap (WINDOW *src, WINDOW *dst, int flag)
{
	int rc;
	int maxy;
	int maxx;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (src == 0 || dst == 0)
		return (ERR);

	if (getmaxy(src) < getmaxy(dst))
		maxy = getmaxy(src);
	else
		maxy = getmaxy(dst);

	if (getmaxx(src) < getmaxx(dst))
		maxx = getmaxx(src);
	else
		maxx = getmaxx(dst);

	/*--------------------------------------------------------------------
	 *  do the overlap
	 */
	rc = copywin(src, dst, 0, 0, 0, 0, maxy, maxx, flag);

	return (rc);
}

/*------------------------------------------------------------------------
 * copywin() - copy the contents of a window to another window
 */
int copywin (WINDOW *src, WINDOW *dst,
	int sminrow, int smincol, int dminrow, int dmincol,
	int dmaxrow, int dmaxcol, int flag)
{
	int i;
	int j;
	int sy;
	int sx;
	int dy;
	int dx;
	int lines;
	int rows;
	chtype c;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (src == 0 || dst == 0)
		return (ERR);

	if (dminrow < 0 || dmaxrow >= getmaxy(dst))
		return (ERR);
	if (dmincol < 0 || dmaxcol >= getmaxx(dst))
		return (ERR);

	lines = dmaxrow - dminrow + 1;
	rows  = dmaxcol - dmincol + 1;

	if (sminrow < 0 || sminrow+lines > getmaxy(src))
		return (ERR);
	if (smincol < 0 || smincol+rows  > getmaxx(src))
		return (ERR);

	/*--------------------------------------------------------------------
	 * now do each line
	 */
	for (i=0; i<lines; i++)
	{
		sy = sminrow + i;
		dy = dminrow + i;

		/*----------------------------------------------------------------
		 * do each char in line
		 */
		for (j=0; j<rows; j++)
		{
			sx = smincol + j;
			dx = dmincol + j;

			c = src->_y[sy][sx];
			if (A_GETTEXT(c) == ' ')
			{
				if (! flag)
					dst->_y[dy][dx] = c;
			}
			else
			{
				dst->_y[dy][dx] = c;
			}
		}

		/*----------------------------------------------------------------
		 * adjust first/last entries
		 */
		if (dst->_firstch[dy] == _NOCHANGE || dst->_firstch[dy] > dmincol)
			dst->_firstch[dy] = dmincol;

		if (dst->_lastch[dy] == _NOCHANGE || dst->_lastch[dy] < dmaxcol)
			dst->_lastch[dy] = dmaxcol;
	}

	/*--------------------------------------------------------------------
	 * if auto-sync in effect, sync window with its parents
	 */
	if (getsync(dst))
		wsyncup(dst);

	/*--------------------------------------------------------------------
	 * if immed in effect, do a refresh
	 */
	if (getimmed(dst))
		return wrefresh(dst);

	return (OK);
}
