/*------------------------------------------------------------------------
 * signal processing
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * tcap_signal_handler() - actual signal handler
 */
static void tcap_signal_handler (int sig)
{
	TERMINAL *	tp = tcap_sig_sp();

	/*--------------------------------------------------------------------
	 * reset for next call
	 */
	signal(sig, tcap_signal_handler);

	/*--------------------------------------------------------------------
	 * process signal
	 */
	if (tp != 0)
	{
		/*----------------------------------------------------------------
		 * cache signal into kbd data
		 */
		term_kbd_sig(tp->term, sig);

		/*----------------------------------------------------------------
		 * now indicate a signal was received
		 */
		tcap_kbd_enq_ta(tp, KEY_SIGNAL, TRUE);
	}
}

/*------------------------------------------------------------------------
 * tcap_signal() - specify a signal handler
 */
void tcap_signal_set_rtn (TERMINAL *tp, int sig, SIG_RTN *rtn, void *data)
{
	SIG_INFO *	st;
	SIG_INFO *	si;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (tp == 0 || sig >= NSIG)
	{
		return;
	}

	st = tp->scrn->sig_tbl;

	/*--------------------------------------------------------------------
	 * a sig of -1 indicates a clear of all entries
	 */
	if (sig < 0)
	{
		int i;

		for (i=0; i<NSIG; i++)
		{
			si = st + i;

			if (si->sig_rtn != 0)
			{
				signal(i, SIG_DFL);

				si->sig_rtn  = 0;
				si->sig_data = 0;
			}
		}

		return;
	}

	/*--------------------------------------------------------------------
	 * cache entry into table
	 */
	si = st + sig;

	si->sig_rtn  = rtn;
	si->sig_data = data;

	/*--------------------------------------------------------------------
	 * Now catch the signal or use default depending
	 * on whether a routine was specified.
	 */
	signal(sig, rtn != 0 ? tcap_signal_handler : SIG_DFL);
}

/*------------------------------------------------------------------------
 * tcap_signal_reset() - set signals on or off
 */
void tcap_signal_reset (TERMINAL *tp, int reset)
{
	OS_SIG_RTN *	rtn	= (reset ? tcap_signal_handler : SIG_DFL);
	int				i;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0)
		return;

	/*--------------------------------------------------------------------
	 * now set all signals on or off
	 */
	for (i=0; i<NSIG; i++)
	{
		SIG_INFO *	si = tp->scrn->sig_tbl + i;

		if (si->sig_rtn != 0)
			signal(i, rtn);
	}
}

/*------------------------------------------------------------------------
 * tcap_signal_rtn() - fetch routine associated with a signal
 */
SIG_RTN * tcap_signal_get_rtn (const TERMINAL *tp, int sig)
{
	SIG_INFO *	si;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (tp == 0 || sig < 0 || sig >= NSIG)
	{
		return (0);
	}

	/*--------------------------------------------------------------------
	 * get info
	 */
	si = tp->scrn->sig_tbl + sig;

	return (si->sig_rtn);
}

/*------------------------------------------------------------------------
 * tcap_signal_data() - fetch data associated with a signal
 */
void * tcap_signal_get_data (const TERMINAL *tp, int sig)
{
	SIG_INFO *	si;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (tp == 0 || sig < 0 || sig >= NSIG)
	{
		return (0);
	}

	/*--------------------------------------------------------------------
	 * get info
	 */
	si = tp->scrn->sig_tbl + sig;

	return (si->sig_data);
}
