/*------------------------------------------------------------------------
 * mode functions
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * tcap_get_baud_rate() - get current baudrate
 */
int	tcap_get_baud_rate	(const TERMINAL *tp)
{
	if (tp == 0)
		return (-1);

	return term_get_baud_rate(tp->term);
}

/*------------------------------------------------------------------------
 * tcap_get_kill_char() - get current kill char
 */
int	tcap_get_kill_char	(const TERMINAL *tp)
{
	if (tp == 0)
		return (-1);

	return term_get_kill_char(tp->term);
}

/*------------------------------------------------------------------------
 * tcap_get_quit_char() - get current quit char
 */
int	tcap_get_quit_char	(const TERMINAL *tp)
{
	if (tp == 0)
		return (-1);

	return term_get_quit_char(tp->term);
}

/*------------------------------------------------------------------------
 * tcap_get_intr_char() - get current intr char
 */
int	tcap_get_intr_char	(const TERMINAL *tp)
{
	if (tp == 0)
		return (-1);

	return term_get_intr_char(tp->term);
}

/*------------------------------------------------------------------------
 * tcap_get_erase_char() - get current erase char
 */
int	tcap_get_erase_char	(const TERMINAL *tp)
{
	if (tp == 0)
		return (-1);

	return term_get_erase_char(tp->term);
}

/*------------------------------------------------------------------------
 * tcap_get_wait() - get current wait mode
 */
int	tcap_get_wait		(const TERMINAL *tp)
{
	if (tp == 0)
		return (-1);

	return term_get_wait(tp->term);
}

/*------------------------------------------------------------------------
 * tcap_set_baud_rate() - set current baud-rate
 */
int	tcap_set_baud_rate	(TERMINAL *tp, int br)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_set_baud_rate(tp->term, br);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_set_kill_char() - set current kill char
 */
int	tcap_set_kill_char	(TERMINAL *tp, int ch)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_set_kill_char(tp->term, ch);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_set_quit_char() - set current quit char
 */
int	tcap_set_quit_char	(TERMINAL *tp, int ch)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_set_quit_char(tp->term, ch);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_set_intr_char() - set current intr char
 */
int	tcap_set_intr_char	(TERMINAL *tp, int ch)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_set_intr_char(tp->term, ch);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_set_erase_char() - set current erase char
 */
int	tcap_set_erase_char	(TERMINAL *tp, int ch)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_set_erase_char(tp->term, ch);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_set_wait() - set current wait mode
 */
int	tcap_set_wait		(TERMINAL *tp, int bf)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_set_wait(tp->term, bf);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_flush_inp() - flush input queue
 */
int tcap_flush_inp (TERMINAL *tp)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_flush_inp(tp->term);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_flush_out() - flush output queue
 */
int tcap_flush_out (TERMINAL *tp)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_flush_out(tp->term);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_set_typeahead() - set typeahead processing
 */
int tcap_set_typeahead (TERMINAL *tp, int fildes)
{
	if (tp == 0)
		return (-1);

	/* NOT IMPLEMENTED */

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_set_halfdelay() - set halfdelay mode
 */
int tcap_set_halfdelay (TERMINAL *tp, int tenths)
{
	int rc;

	if (tp == 0)
		return (-1);

	if (tenths < 1 || tenths > 255)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_set_read_mode(tp->term, tenths * 100);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}
