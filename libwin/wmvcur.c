/*------------------------------------------------------------------------
 * move actual screen cursor
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * mvcur() - move screen position to specified position
 *
 * Note that this call is designed to be used even if the SCREEN
 * structures are not setup.
 */
int mvcur (int oldy, int oldx, int newy, int newx)
{
	SCREEN *	s	= win_cur_screen;
	TERMINAL *	t	= win_cur_term;
	int			rc = OK;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (t == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * only do this if position is different from old position
	 */
	if (oldy != newy || oldx != newx)
	{
		rc = tcap_outpos(t, newy, newx);
		if (rc == OK)
		{
			tcap_outflush(t);
			if (s != 0)
			{
				setcury(SCR_TRMSCR(s), newy);
				setcurx(SCR_TRMSCR(s), newx);
			}
		}
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * scrn_mvcur() - move screen position to specified position on a SCREEN
 */
int scrn_mvcur (SCREEN *s, int oldy, int oldx, int newy, int newx)
{
	TERMINAL *	t;
	int			rc;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (s == 0)
		return (ERR);

	t = SCR_TERM(s);

	/*--------------------------------------------------------------------
	 * only do this if position is different from old position
	 */
	if (oldy != newy || oldx != newx)
	{
		rc = tcap_outpos(t, newy, newx);
		if (rc == OK)
		{
			tcap_outflush(t);
			setcury(SCR_TRMSCR(s), newy);
			setcurx(SCR_TRMSCR(s), newx);
		}
	}

	return (rc);
}
