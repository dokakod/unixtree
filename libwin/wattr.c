/*------------------------------------------------------------------------
 * Attribute functions
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * set & get window attributes (ecurs_attr)
 */
int wattroff (WINDOW *w, attr_t a)
{
	attr_t	a_cmn	= A_GETATTR(a & (A_MONO | A_COMMON));
	attr_t	a_clr	= A_GETATTR(a & (A_COLOR));

	if (w == 0)
		return (ERR);

	if (a_cmn != 0)
		w->_attrs &= ~a_cmn;

	if (a_clr != 0)
		w->_attrs &= ~A_COLOR;

	return (OK);
}

int wattron (WINDOW *w, attr_t a)
{
	attr_t	a_cmn	= A_GETATTR(a & (A_MONO | A_COMMON));
	attr_t	a_clr	= A_GETATTR(a & (A_COLOR));

	if (w == 0)
		return (ERR);

	if (a_cmn != 0)
		w->_attrs |= a_cmn;

	if (a_clr != 0)
	{
		w->_attrs &= ~A_COLOR;
		w->_attrs |= a_clr;
	}


	return (OK);
}
