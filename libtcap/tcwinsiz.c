/*------------------------------------------------------------------------
 * window size routines
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * tcap_winsize_check() - check size of window
 */
int tcap_winsize_check (const TERMINAL *tp)
{
	int changed = FALSE;

	if (tp != 0)
	{
		int old_rows	= tp->tcap->ints.maxrows;
		int old_cols	= tp->tcap->ints.maxcols;
		int new_rows;
		int new_cols;

		if (term_window_size_get(tp->term, &new_rows, &new_cols) == 0)
		{
			tp->tcap->ints.maxrows = new_rows;
			tp->tcap->ints.maxcols = new_cols;
		}

		if (old_rows != tp->tcap->ints.maxrows ||
		    old_cols != tp->tcap->ints.maxcols)
		{
			changed = TRUE;
		}
	}

	return (changed);
}

/*------------------------------------------------------------------------
 * tcap_winsize_reset() - reset window size
 */
int tcap_winsize_reset (TERMINAL *tp, int rows, int cols)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_window_size_set(tp->term, rows, cols);
		if (rc == 0)
		{
			tp->tcap->ints.maxrows = rows;
			tp->tcap->ints.maxcols = cols;
			tcap_mouse_invalidate(tp);
		}
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}
