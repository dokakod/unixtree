/*------------------------------------------------------------------------
 * signal handling routines
 *
 * See syssig.h for details.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sys_defs.h"
#include "syssig.h"

/*------------------------------------------------------------------------
 * signal-info struct
 */
struct sig_info
{
	SIG_RTN *	sig_rtn;			/* routine to call		*/
	void *		sig_data;			/* pointer to user data	*/
};
typedef struct sig_info	SIG_INFO;

static SIG_INFO		sys_sig_tbl[NSIG] = { 0 };

/*------------------------------------------------------------------------
 * signal name struct
 */
struct sig_name
{
	int				si_signo;		/* signal number	*/
	const char *	si_signame;		/* SIG name			*/
	const char *	si_sigdesc;		/* SIG description	*/
};
typedef struct sig_name SIG_NAME;

static const SIG_NAME	sys_sig_names[] =
{
#ifdef SIGHUP
	{ SIGHUP,		"SIGHUP",		"Hangup"					},
#endif

#ifdef SIGINT
	{ SIGINT,		"SIGINT",		"Interrupt"					},
#endif

#ifdef SIGQUIT
	{ SIGQUIT,		"SIGQUIT",		"Quit"						},
#endif

#ifdef SIGILL
	{ SIGILL,		"SIGILL",		"Illegal instruction"		},
#endif

#ifdef SIGTRAP
	{ SIGTRAP,		"SIGTRAP",		"Trace trap"				},
#endif

#ifdef SIGABRT
	{ SIGABRT,		"SIGABRT",		"Abort"						},
#endif

#ifdef SIGIOT
	{ SIGIOT,		"SIGIOT",		"IOT trap"					},
#endif

#ifdef SIGEMT
	{ SIGEMT,		"SIGEMT",		"EMT trap"					},
#endif

#ifdef SIGFPE
	{ SIGFPE,		"SIGFPE",		"Floating point exception"	},
#endif

#ifdef SIGKILL
	{ SIGKILL,		"SIGKILL",		"Kill with prejudice"		},
#endif

#ifdef SIGBUS
	{ SIGBUS,		"SIGBUS",		"Bus error"					},
#endif

#ifdef SIGSEGV
	{ SIGSEGV,		"SIGSEGV",		"Segmentation violation"	},
#endif

#ifdef SIGSYS
	{ SIGSYS,		"SIGSYS",		"Bad system call"			},
#endif

#ifdef SIGPIPE
	{ SIGPIPE,		"SIGPIPE",		"Write to one-ended pipe"	},
#endif

#ifdef SIGALRM
	{ SIGALRM,		"SIGALRM",		"Alarm call"				},
#endif

#ifdef SIGTERM
	{ SIGTERM,		"SIGTERM",		"Software termination"		},
#endif

#ifdef SIGUSR1
	{ SIGUSR1,		"SIGUSR1",		"User signal 1"				},
#endif

#ifdef SIGUSR2
	{ SIGUSR2,		"SIGUSR2",		"User signal 2"				},
#endif

#ifdef SIGCLD
	{ SIGCLD,		"SIGCLD",		"Child death"				},
#endif

#ifdef SIGCHLD
	{ SIGCHLD,		"SIGCHLD",		"Child death"				},
#endif

#ifdef SIGPWR
	{ SIGPWR,		"SIGPWR",		"Power fail"				},
#endif

#ifdef SIGWINCH
	{ SIGWINCH,		"SIGWINCH",		"Window change"				},
#endif

#ifdef SIGURG
	{ SIGURG,		"SIGURG",		"Urgent I/O"				},
#endif

#ifdef SIGIO
	{ SIGIO,		"SIGIO",		"I/O available"				},
#endif

#ifdef SIGPOLL
	{ SIGPOLL,		"SIGPOLL",		"Pollable event occurred"	},
#endif

#ifdef SIGSTOP
	{ SIGSTOP,		"SIGSTOP",		"Software stop"				},
#endif

#ifdef SIGTSTP
	{ SIGTSTP,		"SIGTSTP",		"Terminal stop"				},
#endif

#ifdef SIGCONT
	{ SIGCONT,		"SIGCONT",		"Continue after stop"		},
#endif

#ifdef SIGTTIN
	{ SIGTTIN,		"SIGTTIN",		"bg tty read attempted"		},
#endif

#ifdef SIGTTOU
	{ SIGTTOU,		"SIGTTOU",		"bg tty write attempted"	},
#endif

#ifdef SIGVTALRM
	{ SIGVTALRM,	"SIGVTALRM",	"virtual timer expired"		},
#endif

#ifdef SIGPROF
	{ SIGPROF,		"SIGPROF",		"Profiling timer expired"	},
#endif

#ifdef SIGXCPU
	{ SIGXCPU,		"SIGXCPU",		"CPU limit exceeded"		},
#endif

#ifdef SIGXFSZ
	{ SIGXFSZ,		"SIGXFSZ",		"File size limit exceeded"	},
#endif

#ifdef SIGLOST
	{ SIGLOST,		"SIGLOST",		"Resource lost"				},
#endif

	{ 0, 0, 0 }
};

/*------------------------------------------------------------------------
 * sys_sig_rtn() - actual signal routine
 */
static void sys_sig_rtn (int sig)
{
	SIG_INFO *	si;

	/*--------------------------------------------------------------------
	 * validate signal number
	 */
	if (sig < 0 || sig > NSIG)
		return;
	si = sys_sig_tbl + sig;

	/*--------------------------------------------------------------------
	 * process according to the state
	 */
	if (si->sig_rtn != SIG_DEFAULT && si->sig_rtn != SIG_IGNORE)
	{
		(si->sig_rtn)(sig, si->sig_data);

		signal(sig, sys_sig_rtn);
	}
}

/*------------------------------------------------------------------------
 * sys_sig_set() - set signal handler for a given signal number
 */
void sys_sig_set (int sig, SIG_RTN *rtn, void *data)
{
	OS_SIG_RTN *os_rtn;
	SIG_INFO *	si;

	/*--------------------------------------------------------------------
	 * check if setting all signals
	 */
	if (sig == SIG_ALL)
	{
		int i;

		for (i=0; i<NSIG; i++)
			sys_sig_set(i, rtn, data);

		return;
	}

	/*--------------------------------------------------------------------
	 * validate signal number
	 */
	if (sig < 0 || sig > NSIG)
		return;
	si = sys_sig_tbl + sig;

	/*--------------------------------------------------------------------
	 * cache data in table
	 */
	si->sig_rtn		= rtn;
	si->sig_data	= data;

	/*--------------------------------------------------------------------
	 * now setup signal handling
	 */
	if (rtn == SIG_DEFAULT)
		os_rtn = SIG_DFL;
	else
	if (rtn == SIG_IGNORE)
		os_rtn = SIG_IGN;
	else
		os_rtn = sys_sig_rtn;

	signal(sig, os_rtn);
}

/*------------------------------------------------------------------------
 * sys_sig_get_rtn() - get signal routine for a given signal number
 */
SIG_RTN * sys_sig_get_rtn (int sig)
{
	if (sig < 0 || sig >= NSIG)
		return (0);

	return (sys_sig_tbl[sig].sig_rtn);
}

/*------------------------------------------------------------------------
 * sys_sig_get_data() - get user data for a given signal number
 */
void * sys_sig_get_data (int sig)
{
	if (sig < 0 || sig >= NSIG)
		return (0);

	return (sys_sig_tbl[sig].sig_data);
}

/*------------------------------------------------------------------------
 * sys_sig_send() - send a signal to a process
 */
int sys_sig_send (int pid, int sig)
{
#if V_WINDOWS
	return (-1);
#else
	return kill(pid, sig);
#endif
}

/*------------------------------------------------------------------------
 * sys_sig_name() - get name for a given signal number
 */
char * sys_sig_name (int sig, char *namebuf)
{
	const SIG_NAME *sn;

	for (sn = sys_sig_names; sn->si_signo >= 0; sn++)
	{
		if (sn->si_signo == sig)
		{
			strcpy(namebuf, sn->si_signame);
			return (namebuf);
		}
	}

	sprintf(namebuf, "<signal %d>", sig);
	return (namebuf);
}

/*------------------------------------------------------------------------
 * sys_sig_desc() - get description for a given signal number
 */
char * sys_sig_desc (int sig, char *descbuf)
{
	const SIG_NAME *sn;

	for (sn = sys_sig_names; sn->si_signo >= 0; sn++)
	{
		if (sn->si_signo == sig)
		{
			strcpy(descbuf, sn->si_sigdesc);
			return (descbuf);
		}
	}

	sprintf(descbuf, "<signal %d>", sig);
	return (descbuf);
}

/*------------------------------------------------------------------------
 * sys_sig_info() - get info for a given signal number
 */
void sys_sig_info (int sig, char *namebuf, char *descbuf)
{
	const SIG_NAME *sn;

	for (sn = sys_sig_names; sn->si_signo >= 0; sn++)
	{
		if (sn->si_signo == sig)
		{
			strcpy(namebuf, sn->si_signame);
			strcpy(descbuf, sn->si_sigdesc);
			return;
		}
	}

	sprintf(namebuf, "<signal %d>", sig);
	sprintf(descbuf, "<unknown>");
}
