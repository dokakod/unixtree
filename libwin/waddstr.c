/*------------------------------------------------------------------------
 * add a string to a window
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * waddnstr() - add n chars of a string
 */
int waddnstr (WINDOW *win, const char *s, int n)
{
	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0 || s == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * add the string
	 */
	return wcaddnstr(win, s, n, win->_code);
}

/*------------------------------------------------------------------------
 * waddstr() - add a string (using pre-defined code)
 */
int waddstr (WINDOW *win, const char *s)
{
	return waddnstr(win, s, -1);
}

/*------------------------------------------------------------------------
 * waddtstr() - add a string (toggling stdandout)
 */
int waddtstr (WINDOW *win, const char *s, int c)
{
	const char *	str;
	int				so	= FALSE;
	int				rc	= OK;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0 || s == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * add the string, toggling standout if needed
	 */
	for (str=s; *str; str++)
	{
		if (*str == c)
		{
			if (! so)
			{
				wstandout(win);
				so = TRUE;
			}
			else
			{
				wstandend(win);
				so = FALSE;
			}
		}
		else
		{
			rc = waddch(win, *str);
		}
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * wcaddstr() - add a string (using specified code)
 */
int wcaddstr (WINDOW *win, const char *s, int code)
{
	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0 || s == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * add the string
	 */
	return wcaddnstr(win, s, -1, code);
}

/*------------------------------------------------------------------------
 * wcaddnstr() - add n chars of a string with a code
 */
int wcaddnstr (WINDOW *win, const char *s, int n, int code)
{
	const unsigned char *us = (const unsigned char *)s;
	int i;
	int rc = OK;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0 || s == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * adjust number of chars (-1 implies to end-of-line)
	 */
	if (n < 0 || n > getmaxx(win) - getcurx(win))
		n = getmaxx(win) - getcurx(win);

	/*--------------------------------------------------------------------
	 * add each char
	 */
	for (i=0; i<n; i++)
	{
		if (*us == 0)
			break;

		rc = wcaddch(win, *us++, code);
		if (rc == ERR)
			break;
	}

	return (rc);
}
