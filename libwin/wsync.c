/*------------------------------------------------------------------------
 * sync routines
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * wsyncup() - sync all parents with specified window
 */
void wsyncup (WINDOW *win)
{
	WINDOW *w;
	int i;
	int j;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return;

	/*--------------------------------------------------------------------
	 * sync each parent of window
	 */
	for (w=win->_parent; w; w=w->_parent)
	{
		for (i=0; i<getmaxy(win); i++)
		{
			j = i + getbegy(win) - getbegy(w);

			if (win->_firstch[i] != _NOCHANGE &&
				win->_firstch[i] + getbegx(win) < w->_firstch[j] + getbegx(w))
			{
				w->_firstch[j] = win->_firstch[i] + getbegx(win) - getbegx(w);
			}

			if (win->_lastch[i] != _NOCHANGE &&
				win->_lastch[i] + getbegx(win) > w->_lastch[j] + getbegx(w))
			{
				w->_lastch[j] = win->_lastch[i] + getbegx(win) - getbegx(w);
			}
		}
	}
}

/*------------------------------------------------------------------------
 * wcursyncup() - sync cursor position of all parents to a window
 */
void wcursyncup (WINDOW *win)
{
	WINDOW *w;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return;

	/*--------------------------------------------------------------------
	 * sync current position for all parents
	 */
	for (w=win->_parent; w; w=w->_parent)
	{
		setcury(w, getcury(win) + getbegy(win) - getbegy(w));
		setcurx(w, getcurx(win) + getbegx(win) - getbegx(w));
	}
}

/*------------------------------------------------------------------------
 * wsyncdown() - sync a window with all its parents
 */
void wsyncdown (WINDOW *win)
{
	WINDOW *w;
	int i;
	int j;
	int k;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return;

	/*--------------------------------------------------------------------
	 * sync window with each parent
	 */
	for (w=win->_parent; w; w=w->_parent)
	{
		for (i=0; i<getmaxy(win); i++)
		{
			j = i + getbegy(win) - getbegy(w);
			if (w->_firstch[j] != _NOCHANGE &&
				w->_firstch[j] + getbegx(w) < win->_firstch[i] + getbegx(win))
			{
				k = w->_firstch[j]+getbegx(w)-getbegx(win);
				if (k < 0)
					k = 0;
				win->_firstch[i] = k;
			}

			if (w->_lastch[j] != _NOCHANGE &&
				w->_lastch[j] + getbegx(w) > win->_lastch[i] + getbegx(win))
			{
				k = w->_lastch[j]+getbegx(w)-getbegx(win);
				if (k < 0)
					k = 0;
				win->_lastch[i] = k;
			}
		}
	}
}
