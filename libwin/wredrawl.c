/*------------------------------------------------------------------------
 * redraw lines in a window
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * wredrawln() - redraw lines in a window
 */
int wredrawln (WINDOW *win, int i, int n)
{
	int y;
	int x;
	int j;
	int l;
	WINDOW *cs;
	WINDOW *ts;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	cs = WIN_CURSCR(win);
	ts = WIN_TRMSCR(win);

	/*--------------------------------------------------------------------
	 * get starting position
	 */
	y = i + getbegy(win);
	x = getbegx(win);

	/*--------------------------------------------------------------------
	 * now do all specified lines
	 */
	for (l=0; l<n; l++)
	{
		for (j=0; j<getmaxx(win); j++)
		{
			ts->_y[y+l][x+j] = 0;
		}
		cs->_firstch[y+l] = x;
		cs->_lastch[y+l]  = x + getmaxx(win) - 1;
		scrn_doupdateln(WIN_SCREEN(win), y+l);
	}

	return (OK);
}
