/*------------------------------------------------------------------------
 * curses/termcap interface routines
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * tcap_set_inp_nl() - set input nl options on/off
 */
int tcap_set_inp_nl (TERMINAL *tp, int bf)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_set_inp_nl(tp->term, bf);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_set_out_nl() - set output nl options on/off
 */
int tcap_set_out_nl (TERMINAL *tp, int bf)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_set_out_nl(tp->term, bf);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_set_raw() - set raw options on/off
 */
int tcap_set_raw (TERMINAL *tp, int bf)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_set_raw(tp->term, bf);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_set_cbreak() - set cbreak options on/off
 */
int tcap_set_cbreak (TERMINAL *tp, int bf)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_set_cbreak(tp->term, bf);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_set_echo() - set echo options on/off
 */
int tcap_set_echo (TERMINAL *tp, int bf)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_set_echo(tp->term, bf);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_set_qiflush() - set flush-on-interrupt options on/off
 */
int tcap_set_qiflush (TERMINAL *tp, int bf)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_set_qiflush(tp->term, bf);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_set_meta() - set 8-bit options on/off
 */
int tcap_set_meta (TERMINAL *tp, int bf)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_set_meta(tp->term, bf);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_get_curx() - get current x position
 */
int tcap_get_curx (const TERMINAL *tp)
{
	if (tp == 0)
		return (-1);

	return tp->scrn->curr_col;
}

/*------------------------------------------------------------------------
 * tcap_get_cury() - get current y position
 */
int tcap_get_cury (const TERMINAL *tp)
{
	if (tp == 0)
		return (-1);

	return tp->scrn->curr_row;
}

/*------------------------------------------------------------------------
 * tcap_set_curx() - set current x position
 */
int tcap_set_curx (TERMINAL *tp, int x)
{
	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		tp->scrn->curr_col = x;
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_set_cury() - set current y position
 */
int tcap_set_cury (TERMINAL *tp, int y)
{
	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		tp->scrn->curr_row = y;
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_napms() - sleep for given milliseconds
 */
int tcap_napms (int ms)
{
	if (ms < 0)
		return (-1);

	term_nap(ms);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_set_use_env() - specify whether to use LINES & COLS env vars
 */
void tcap_set_use_env (int bf)
{
	tcap_use_env_vars = bf;
}

/*------------------------------------------------------------------------
 * tcap_get_use_env() - get whether to use LINES & COLS env vars
 */
int tcap_get_use_env (void)
{
	return (tcap_use_env_vars);
}

/*------------------------------------------------------------------------
 * tcap_set_term_path() - set path to search for term files
 */
void tcap_set_term_path (const char *path)
{
	tcap_term_path = path;
}

/*------------------------------------------------------------------------
 * tcap_get_term_path() - get path to search for term files
 */
const char * tcap_get_term_path (void)
{
	return (tcap_term_path);
}
