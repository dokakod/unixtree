/*------------------------------------------------------------------------
 * terminfo/termcap database interface routines
 */
#include "wincommon.h"
#include "term.h"
#include "termcap.h"

/*------------------------------------------------------------------------
 * get_curterm() - get pointer to current TERMINAL
 */
TERMINAL * get_curterm (void)
{
	return (win_cur_term);
}

/*------------------------------------------------------------------------
 * set_curterm() - set pointer to current TERMINAL
 */
int set_curterm (TERMINAL *t)
{
	if (t == 0)
		return (ERR);

	win_cur_term = t;

	return (OK);
}

/*------------------------------------------------------------------------
 * del_curterm() - delete a TERMINAL pointer
 */
int del_curterm (TERMINAL *t)
{
	if (t == 0)
		return (ERR);

	tcap_delterminal(t);

	if (t == win_cur_term)
		win_cur_term = 0;

	return (OK);
}

/*------------------------------------------------------------------------
 * new_curterm() - create a new terminal
 *
 * Note: This does no setup of the curses environment, but it DOES
 * set the current TERMINAL pointer.
 */
TERMINAL * new_curterm (const char *term, int inp_fd, int out_fd, int *errret)
{
	TERMINAL *	tp;
	TCAP_ERRS	errs;
	TCAP_ERRS *	te	= &errs;

	/*--------------------------------------------------------------------
	 * create the terminal
	 */
	tp = tcap_newscreen(term, 0, inp_fd, out_fd, te);
	if (tp == 0)
	{
		if (errret != 0)
		{
			*errret = -1;
		}
		else
		{
			tcap_errmsg_print_list(te, stderr, FALSE);

			/*------------------------------------------------------------
			 * According to curses specs, we have to exit here.
			 * I sure hate seeing exit() calls in a library ...
			 */
			exit(1);
			/*NOTREACHED*/
		}

		tcap_errmsg_clear(te);
		return (0);
	}

	/*--------------------------------------------------------------------
	 * clear error-msg struct
	 */
	if (errret != 0)
		*errret = 1;
	tcap_errmsg_clear(te);

	/*--------------------------------------------------------------------
	 * set current term pointer
	 */
	win_cur_term = tp;

	return (tp);
}

/*------------------------------------------------------------------------
 * restartterm() - create a new TERMINAL for a SCREEN
 *
 * Note: If in a curses environment, the current screen is adjusted
 * to point to this TERMINAL.
 */
int restartterm (const char *term, int fd, int *errret)
{
	TERMINAL *	tp;

	/*--------------------------------------------------------------------
	 * delete old terminal if it exists
	 */
	if (win_cur_term != 0)
	{
		tcap_delterminal(win_cur_term);
		win_cur_term = 0;

		if (win_cur_screen != 0)
			win_cur_screen->term = 0;
	}

	/*--------------------------------------------------------------------
	 * create new terminal
	 */
	tp = new_curterm(term, -1, fd, errret);
	if (tp == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * if in curses environment, set term pointer
	 */
	if (win_cur_screen != 0)
		win_cur_screen->term = tp;

	return (OK);
}

/*------------------------------------------------------------------------
 * setupterm() - setup a SCREEN object
 *
 * Note: This does no setup of the curses environment, but it DOES
 * set the current TERMINAL pointer.
 */
int setupterm (const char *term, int fd, int *errret)
{
	TERMINAL *	tp;

	/*--------------------------------------------------------------------
	 * create the terminal
	 */
	tp = new_curterm(term, -1, fd, errret);

	return (tp != 0 ? OK : ERR);
}

/*------------------------------------------------------------------------
 * setterm() - setup a SCREEN using all defaults
 *
 * Note: This does no setup of the curses environment, but it DOES
 * set the current TERMINAL pointer.
 */
int setterm (const char *term)
{
	return setupterm(term, -1, 0);
}
