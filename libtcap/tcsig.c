/*------------------------------------------------------------------------
 * signal handling routines
 *
 *	Most signals will cause the program to print out a message
 *	indicating the signal received, then the terminal is put back
 *	in its normal shell state and the program terminates.
 *
 *	Note: if the environ var TC_IGNORE_SIGNALS is found, then
 *	signal handling is not done (for debugging).
 *
 *	Note: some signals are handled in a special manner,
 *	as follows:
 *
 *		SIGTSTP		If present, this signal will be processed by
 *					putting the terminal back in its shell state,
 *					stopping & waiting for a continue, resetting
 *					the terminal state, and redrawing the screen if we can.
 *
 *		SIGWINCH	If present, this signal is used to determine if
 *					the window size has changed.  This signal is used
 *					in a windowing env such as Sunview, Open Look,
 *					Open Desktop, etc.
 *
 *					When this signal is caught, if the screen size has
 *					changed, the maxrows & maxcols are set to the new
 *					values, and a KEY_WINRESIZE entry is enqueued in the
 *					keyboard buffer.
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * tcap_sig_sp() - get cached screen pointer
 */
TERMINAL * tcap_sig_sp (void)
{
	return (tcap_cur_terminal);
}

/*------------------------------------------------------------------------
 * tcap_sig_async() - async signal handler
 */
static int tcap_sig_async (int key, void *data)
{
	TERMINAL *	tp	= (TERMINAL *)data;

	/*--------------------------------------------------------------------
	 * process this key
	 */
	if (tp != 0)
	{
		SIG_INFO *	si;
		int			sig;

		/*----------------------------------------------------------------
		 * clear cached signal in kbd buffer
		 */
		sig	= term_kbd_sig(tp->term, 0);

		/*----------------------------------------------------------------
		 * get ptr to signal entry
		 */
		si	= tp->scrn->sig_tbl + sig;

		/*----------------------------------------------------------------
		 * call the routine
		 */
		if (si->sig_rtn != 0)
			(si->sig_rtn)(sig, si->sig_data);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_sig_msg() - internal routine to output a message
 */
static void tcap_sig_msg (int sig)
{
	char		namebuf[64];
	char		descbuf[64];

	/*--------------------------------------------------------------------
	 * close out terminal mode
	 */
	tcap_cexit(tcap_cur_terminal, TRUE);

	/*--------------------------------------------------------------------
	 * get signal info
	 */
	sys_sig_info(sig, namebuf, descbuf);

	/*--------------------------------------------------------------------
	 * output message
	 */
	fprintf(stderr, "\n%s signal [%s] received.\n", namebuf, descbuf);

	/*--------------------------------------------------------------------
	 * exit
	 */
	exit(1);
	/*NOTREACHED*/
}

/*------------------------------------------------------------------------
 * tcap_sig_msg_int() - internal routine to output a message
 */
static void tcap_sig_msg_int (int sig, void *data)
{
	tcap_sig_msg(sig);
}

#if defined(SIGWINCH)
/*------------------------------------------------------------------------
 * tcap_sigwinch() - process screen size changes
 */
static void tcap_sigwinch (int sig, void *data)
{
	if (tcap_cur_terminal != 0)
	{
		tcap_kbd_enq_ta(tcap_cur_terminal, KEY_WINRESIZE, TRUE);
	}
}
#endif /* SIGWINCH */

#if defined(SIGTSTP)
/*------------------------------------------------------------------------
 * tcap_sigtstp() - process job-control signals
 */
static void tcap_sigtstp (int sig)
{
	TERMINAL *	tp;
	int			alarm_intvl;

	/*--------------------------------------------------------------------
	 * Stop any alarms
	 */
	alarm_intvl = term_alarm_stop();

	/*--------------------------------------------------------------------
	 * Reset signal handler for next time
	 */
	signal(sig, tcap_sigtstp);

	/*--------------------------------------------------------------------
	 * Cache our screen pointer because tcap_cexit() will clear it.
	 */
	tp = tcap_cur_terminal;

	/*--------------------------------------------------------------------
	 * set cached signal in kbd buffer
	 */
	term_kbd_sig(tp->term, sig);

	/*---------------------------------------------------------------------
	 * schedule a redraw the screen
	 */
	tcap_kbd_enq_ta(tp, KEY_REFRESH, TRUE);

#if 0
	/*--------------------------------------------------------------------
	 * Reset terminal to shell mode.
	 */
	tcap_cexit(tp, FALSE);
#endif

	/*--------------------------------------------------------------------
	 * Kill ourselves with a SIGSTOP.  This will tell us to stop
	 * and we won't wake up until the shell sends us a SIGCONT signal.
	 * This signal cannot be caught.
	 */
	kill(getpid(), SIGSTOP);

#if 0
	/*--------------------------------------------------------------------
	 * Now set our terminal back & redraw the screen.
	 */
	tcap_cinit(tp);
#endif

	/*--------------------------------------------------------------------
	 * Restart any alarms
	 */
	term_alarm_start(alarm_intvl);
}
#endif /* SIGTSTP */

/*------------------------------------------------------------------------
 * tcap_sig_init() - initialize signal processing
 */
void tcap_sig_init (TERMINAL *tp)
{
	char *env_ptr;

	if (tp == 0)
		return;

	/*--------------------------------------------------------------------
	 * cache screen pointer
	 */
	tcap_cur_terminal = tp;

	/*--------------------------------------------------------------------
	 * set async handler
	 */
	tcap_async_add(tp->scrn, KEY_SIGNAL, tcap_sig_async, tp);

	/*--------------------------------------------------------------------
	 * set any cached signals on
	 */
	tcap_signal_reset(tp, TRUE);

	/*--------------------------------------------------------------------
	 * We always catch the following signals using our signal mechanism.
	 */
#if defined(SIGINT)
	if (tcap_signal_get_rtn(tp, SIGINT) == 0)
		tcap_signal_set_rtn(tp, SIGINT,   tcap_sig_msg_int, 0);
#endif

#if defined(SIGQUIT)
	if (tcap_signal_get_rtn(tp, SIGQUIT) == 0)
		tcap_signal_set_rtn(tp, SIGQUIT,  tcap_sig_msg_int, 0);
#endif

#if defined(SIGTERM)
	if (tcap_signal_get_rtn(tp, SIGTERM) == 0)
		tcap_signal_set_rtn(tp, SIGTERM,  tcap_sig_msg_int, 0);
#endif

#if defined(SIGWINCH)
	if (tcap_signal_get_rtn(tp, SIGWINCH) == 0)
		tcap_signal_set_rtn(tp, SIGWINCH, tcap_sigwinch, 0);
#endif

#if defined(SIGTSTP) && 0
	/*--------------------------------------------------------------------
	 * We catch SIGTSTP only if we are not in an "event-rtn" environment.
	 */
	if (term_screen_dev_event_data(tp->term) == 0)
		signal(SIGTSTP, tcap_sigtstp);
#endif

	/*--------------------------------------------------------------------
	 * These signals we just plain ignore.
	 */
#if defined(SIGHUP)
	signal(SIGHUP,   SIG_IGN);
#endif

	/*--------------------------------------------------------------------
	 * Setup all other signal handlers using the system mechanism.
	 *
	 * Don't catch these signals if the user has defined the following
	 * variable in his environment (for debugging).
	 */
	env_ptr = getenv("TC_IGNORE_SIGNALS");
	if (env_ptr == 0)
	{
#if defined(SIGILL)
		signal(SIGILL,  tcap_sig_msg);
#endif

#if defined(SIGBUS)
		signal(SIGBUS,  tcap_sig_msg);
#endif

#if defined(SIGSEGV)
		signal(SIGSEGV, tcap_sig_msg);
#endif

#if defined(SIGSYS)
		signal(SIGSYS,  tcap_sig_msg);
#endif

#if defined(SIGABRT)
		signal(SIGABRT, tcap_sig_msg);
#endif

#if defined(SIGPIPE)
		signal(SIGPIPE, tcap_sig_msg);
#endif
	}
}

/*------------------------------------------------------------------------
 * tcap_sig_end() - end signal processing
 */
void tcap_sig_end (TERMINAL *tp)
{
	/*--------------------------------------------------------------------
	 * set any cached signals off
	 */
	tcap_signal_reset(tp, FALSE);

	/*--------------------------------------------------------------------
	 * clear async handler
	 */
	tcap_async_add(tp->scrn, KEY_SIGNAL, 0, 0);

	/*--------------------------------------------------------------------
	 * clear cached screen pointer
	 */
	tcap_cur_terminal = 0;

	/*--------------------------------------------------------------------
	 * clear all system signals (our signals were already reset)
	 */
#if defined(SIGILL)
	signal(SIGILL,  SIG_DFL);
#endif

#if defined(SIGBUS)
	signal(SIGBUS,  SIG_DFL);
#endif

#if defined(SIGSEGV)
	signal(SIGSEGV, SIG_DFL);
#endif

#if defined(SIGSYS)
	signal(SIGSYS,  SIG_DFL);
#endif

#if defined(SIGABRT)
	signal(SIGABRT, SIG_DFL);
#endif

#if defined(SIGPIPE)
	signal(SIGPIPE, SIG_DFL);
#endif

#if defined(SIGHUP)
	signal(SIGHUP,  SIG_DFL);
#endif
}
