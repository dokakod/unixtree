/*------------------------------------------------------------------------
 * delete a window
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * delwin() - delete a window
 */
int delwin (WINDOW *w)
{
	WINLIST *	wl;
	SCREEN *	s;
	WIN_CLOCK *	wc;
	int			i;

	/*--------------------------------------------------------------------
	 * bail if no window
	 */
	if (w == 0)
		return (ERR);

	s = WIN_SCREEN(w);

	/*--------------------------------------------------------------------
	 * bail if this window has any sub-windows
	 */
	if (w->_ndescs)
		return (ERR);

	/*--------------------------------------------------------------------
	 * check if a clock is set for this window
	 */
	wc = SCR_CLOCK(s);
	if (wc->clk_win == w)
		win_clock(w, 0, 0, 0);

	/*--------------------------------------------------------------------
	 * check if this is a subwin
	 */
	if (w->_parent != 0)
	{
		/*----------------------------------------------------------------
		 * just decrement parent sub-win count
		 */
		(w->_parent)->_ndescs--;
	}
	else
	{
		/*----------------------------------------------------------------
		 * real window. free all lines
		 */
		for (i=0; i<getmaxy(w); i++)
		{
			if (w->_y[i])
				FREE(w->_y[i]);
		}
	}

	/*--------------------------------------------------------------------
	 * now free all data in the window
	 */
	if (w->_y != 0)
		FREE(w->_y);

	if (w->_firstch != 0)
		FREE(w->_firstch);

	if (w->_lastch != 0)
		FREE(w->_lastch);

	/*--------------------------------------------------------------------
	 * now free the window itself
	 */
	FREE(w);

	/*--------------------------------------------------------------------
	 * now remove window from list
	 */
	for (wl=s->winlist; wl; wl=wl->next)
	{
		if (wl->win == w)
		{
			if (wl->prev == 0)
			{
				if (wl->next != 0)
					(wl->next)->prev = 0;
				s->winlist = wl->next;
			}
			else
			{
				if (wl->prev != 0)
					(wl->prev)->next = wl->next;

				if (wl->next != 0)
					(wl->next)->prev = wl->prev;
			}

			FREE(wl);
			break;
		}
	}

	return (OK);
}
