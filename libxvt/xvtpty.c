/*------------------------------------------------------------------------
 * open a pair of pseudo-ttys
 *
 *	Pseudo-ttys are a pair of file-descriptors, a master & a slave fd.
 *	The "parent" uses the "master", and the "child" uses the "slave".
 *	The "slave" pty must have tty contols so the child can use termio
 *	on it.
 *
 *	We also want the "slave" pty to be our controlling terminal. A
 *	controlling terminal is created by removing any existing controlling
 *	terminal, then opening a tty or a pty, which then acquires controlling
 *	status. Only a controlling terminal can respond to signals such as
 *	INTR & QUIT.
 *
 *	Thus, the sequence is as follows:
 *
 *	1.	Get "master" & "slave" names & open the "master".
 *	2.	Lose our controlling terminal.
 *	3.	Open our "slave", which also becomes our controlling terminal.
 *	4.	Associate our process group with the "slave".
 *
 * Note that there are many ways of implementing pseudo-ttys.
 * This code implements two different ways:
 *
 *	1.	Traditional BSD mechanism (Linux uses this also)
 *
 *		"master"	has pathname of /dev/pty<major><minor>
 *		"slave"		has pathname of /dev/tty<major><minor>
 *
 *		where 		"major"	is in the range of [p-z]
 *					"minor" is in the range of [0-9a-z]
 *
 *		The mechanism is to try to open each possible "master"
 *		name until one succeeds, then create the "slave" name
 *		from the same major & minor values.
 *
 *		In this mechanism, the "slave" automatically has termio
 *		controls associated with it.
 *
 *	2.	Standard SYSV or SVR4 mechanism
 *
 *		"master"	has pathname of "/dev/ptmx" (which is a "special" name)
 *		"slave"		has pathname of "/dev/pty/nnn"
 *
 *		The mechanism is:
 *
 *			1.	Open the master:		int   mfd = open("/dev/ptmx", O_RDWR);
 *			2.	Grant slave access:		int    rc = grantpt(mfd);
 *			3.	Unlock the master:		int    rc = unlockpt(mfd);
 *			4.	Get slave name:			char *sfn = ptsname(mfd);
 *			5.	Open the slave:			int   sfd = open(sfn, O_RDWR);
 *
 *		Termio controls are associated with the slave by insuring
 *		that the proper stream modules are "pushed" onto the slave,
 *		using ioctl(sfd, I_PUSH, "<module-name>").  Unfortunately, the
 *		list of stream modules which have to be pushed are both
 *		O/S dependent and order-dependent.
 *
 *	Also, there are three different ways of creating a new controlling
 *	terminal and process group:
 *
 *		1.	BSD   mechanism
 *		2.	SVR4  mechanism
 *		3.	POSIX mechanism
 *
 *	Since all modern Unixes support POSIX, that is the mechanism we use here.
 *	For the same reason, we use POSIX termio for all termio support.
 */

#include "xvtcommon.h"

/*------------------------------------------------------------------------
 * O/S dependent functions
 */
static int			xvt_pty_master		(PTY_DATA *pd);
static int			xvt_pty_slave		(PTY_DATA *pd);

/*------------------------------------------------------------------------
 * get master & slave names and open the master
 */
PTY_DATA * xvt_pty_init (const char *spec)
{
	PTY_DATA *	pd;
	int	rc;

	/*--------------------------------------------------------------------
	 * create pty struct
	 */
	pd = (PTY_DATA *)MALLOC(sizeof(*pd));
	if (pd == 0)
		return (0);

	/*--------------------------------------------------------------------
	 * initialize & set to "not open"
	 */
	memset(pd, 0, sizeof(*pd));

	pd->mst_fd = -1;
	pd->slv_fd = -1;

	/*--------------------------------------------------------------------
	 * check if a spec was specified
	 */
	if (spec != 0 && *spec != 0)
	{
		/*----------------------------------------------------------------
		 * spec will be in the form of:
		 *
		 *	"xxd"	slave is /dev/tty<xx>	master is fd <d>
		 *	"c.n"	slave is /dev/pty/<c>	master is fd <n>
		 *
		 * We are interested only in the master fd.
		 */
		const char *	p;
		int				n	= -1;

		/*----------------------------------------------------------------
		 * get the fd of the master
		 */
		p = strrchr(spec, '.');
		if (p == 0)
		{
			/*------------------------------------------------------------
			 * spec is "xxn"
			 */
			for (p=spec; *p; p++)
			{
				if (isdigit(*p))
					break;
			}

			if (*p != 0)
				n = atoi(p);
		}
		else
		{
			/*------------------------------------------------------------
			 * spec is "c.n"
			 */
			p++;
			if (isdigit(*p))
				n = atoi(p);
		}

		/*----------------------------------------------------------------
		 * if the master fd is valid, use it
		 */
		if (n >= 0)
			pd->mst_fd = n;
	}

	/*--------------------------------------------------------------------
	 * get names & open master
	 */
	rc = xvt_pty_master(pd);
	if (rc)
	{
		FREE(pd);
		return (0);
	}

	return (pd);
}

/*------------------------------------------------------------------------
 * open the slave
 */
int xvt_pty_open (PTY_DATA *pd, int bDetach, const char *sttymode)
{
	int	rc;

	/*--------------------------------------------------------------------
	 * First we have to lose any controlling terminal,
	 * otherwise we can't set this one.
	 */
	if (bDetach)
	{
		pd->pgid = setsid();
		if (pd->pgid == (pid_t)-1)
			pd->pgid = getpgrp();
	}
	else
	{
		pd->pgid = getpgrp();
	}

	/*--------------------------------------------------------------------
	 * Now open the slave pty as our controlling terminal.
	 */
	if (pd->slv_fd < 0)
	{
		rc = xvt_pty_slave(pd);
		if (rc)
			return (-1);
	}

	/*--------------------------------------------------------------------
	 * get tty state of slave & fix it up
	 */
	rc = tcgetattr(pd->slv_fd, &pd->tty);
	if (rc == 0)
	{
		/*----------------------------------------------------------------
		 * Linux sets these settings off if run in background
		 */
		pd->tty.c_lflag		|= (ISIG | ICANON | ECHO);
		pd->tty.c_iflag		|= (ICRNL);
		pd->tty.c_oflag		|= (OPOST | ONLCR);

		/*----------------------------------------------------------------
		 * set INTR to ^C
		 *
		 * SCO always messes this one up (they use DEL)
		 */
		pd->tty.c_cc[VINTR]	 = 0x03;

		/*----------------------------------------------------------------
		 * set ERASE to ^H
		 *
		 * Sun always messes this one up (they use DEL)
		 */
		pd->tty.c_cc[VERASE] = 0x08;

		/*----------------------------------------------------------------
		 * We want to turn off any tab processing
		 */
#ifdef TAB3
		pd->tty.c_oflag		&= ~TAB3;
#endif
	}
	else
	{
		/*----------------------------------------------------------------
		 * We should here create a default struct, but I'm too lazy
		 * to do it right now, so I'll just trust that it will
		 * always work.  maybe later ...
		 */
	}

	/*--------------------------------------------------------------------
	 * check if any modes specified
	 *
	 * Note that we have already validated this string.
	 */
	if (sttymode != 0 && *sttymode != 0)
		xvt_tio_build(&pd->tty, sttymode, FALSE, 0);

	/*--------------------------------------------------------------------
	 * now set it back
	 */
	tcsetattr(pd->slv_fd, TCSANOW, &pd->tty);

	/*--------------------------------------------------------------------
	 * now adjust stdio stuff if detaching
	 */
	if (bDetach)
	{
		/*----------------------------------------------------------------
		 * now associate the slave with standard streams
		 */
		dup2(pd->slv_fd, fileno(stdin));
		dup2(pd->slv_fd, fileno(stdout));
		dup2(pd->slv_fd, fileno(stderr));

		/*----------------------------------------------------------------
		 * now associate our process group to the slave
		 */
		rc = tcsetpgrp(fileno(stdin), pd->pgid);
		if (rc)
		{
			/*------------------------------------------------------------
			 * In theory, we should bail with an error because we failed
			 * to set the controlling terminal.  But the program will
			 * still run, just the SIGINTS won't be delivered, so
			 * we continue on.
			 */
#if 0
			xvt_pty_close(pd);
			return (-1);
#endif
		}
	}

	return (0);
}

/*------------------------------------------------------------------------
 * close ptys
 */
void xvt_pty_close (PTY_DATA *pd)
{
	if (pd != 0)
	{
		/*----------------------------------------------------------------
		 * For some reason, Solaris generates a SIGHUP when
		 * we attempt to close the master, so we ignore it.
		 */
		sys_sig_set(SIGHUP, SIG_IGNORE, 0);

		/*----------------------------------------------------------------
		 * close the slave
		 */
		if (pd->slv_fd >= 0)
			close(pd->slv_fd);
		pd->slv_fd = -1;

		/*----------------------------------------------------------------
		 * close the master
		 */
		if (pd->mst_fd >= 0)
			close(pd->mst_fd);
		pd->mst_fd = -1;

		/*----------------------------------------------------------------
		 * finally free the struct
		 */
		FREE(pd);
	}
}

/*------------------------------------------------------------------------
 * get tty modes
 */
int xvt_pty_getmode (PTY_DATA *pd, TTY_STRUCT *t)
{
	int rc = -1;

	if (pd != 0 && pd->slv_fd >= 0)
	{
		rc = tcgetattr(pd->slv_fd, t);
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * set tty modes
 */
int xvt_pty_setmode (PTY_DATA *pd, TTY_STRUCT *t)
{
	int rc = -1;

	if (pd != 0 && pd->slv_fd >= 0)
	{
		/*----------------------------------------------------------------
		 * Note that we assume we have a valid tty struct to use.
		 */
		rc = tcsetattr(pd->slv_fd, TCSANOW, t);
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * process resize of slave
 */
int xvt_pty_resize (PTY_DATA *pd, int w, int h, int pid)
{
	int rc = -1;

	if (pd != 0)
	{
		struct winsize	win;

		/*----------------------------------------------------------------
		 * If the slave is not open, open it now.
		 *
		 * Note that the open of the slave (by the parent) is deferred
		 * until needed, to avoid the race condition of the child
		 * terminating while the parent is trying to open the slave
		 * (which causes the open to hang).
		 */
		if (pd->slv_fd < 0)
		{
			rc = xvt_pty_slave(pd);
			if (rc)
				return (rc);
		}

		/*----------------------------------------------------------------
		 * Change size of the pseudo-tty.
		 *
		 * Note that we issue the ioctl against the slave pty,
		 * not the master, since only the slave responds to ioctls.
		 *
		 * Note that many xterm implementations (e.g. rxvt) mistakenly
		 * use the master, which is why resizing doesn't work properly
		 * for them.
		 */
		memset(&win, 0, sizeof(win));
		win.ws_col = (unsigned short)w;
		win.ws_row = (unsigned short)h;

		rc = ioctl(pd->slv_fd, TIOCSWINSZ, &win);
		if (rc)
		{
			/*------------------------------------------------------------
			 * Inform child of change.
			 *
			 * The man page for the TIOCSWINSZ ioctl() says that ioctl()
			 * will automatically send a SIGWINCH signal to the process group,
			 * but it appears that Linux doesn't do that, so we will do it
			 * ourselves.
			 *
			 * This may result in the child getting two signals instead of just
			 * one, but that is preferrable to getting none.
			 */
			if (pid != 0)
				sys_sig_send(pid, SIGWINCH);
		}
	}

	return (rc);
}

#if defined(PTY_BSD)				|| \
	defined(linux)					|| \
	defined(_AIX) || defined(AIX)	|| \
	defined(__OpenBSD__)			|| \
	defined(__NetBSD__)				|| \
	defined(__FreeBSD__)

/*------------------------------------------------------------------------
 * PTY type: BSD
 */
static int xvt_pty_master (PTY_DATA *pd)
{
	/*--------------------------------------------------------------------
	 * check if we already have a master
	 */
	if (pd->mst_fd >= 0)
	{
		const char *	name;

		name = ttyname(pd->mst_fd);
		if (name == 0)
			return (-1);

		if (strncmp(name, "/dev/pty", 8) != 0)
			return (-1);

		strcpy(pd->mst_name, name);
		strcpy(pd->slv_name, name);
		pd->slv_name[5] = 't';		/* /dev/ptyxx -> /dev/ttyxx */
	}
	else
	{
		/*----------------------------------------------------------------
		 * run through filenames:
		 *
		 *	master:	/dev/pty<major><minor>
		 *	slave:	/dev/tty<major><minor>
		 */
		const char *	major	= "pqrstuvwxyz";
		const char *	minor	= "0123456789abcdef";
		const char *	m;
		const char *	n;

		strcpy(pd->mst_name, "/dev/ptyxy");
		strcpy(pd->slv_name, "/dev/ttyxy");

		for (m=major; *m; m++)
		{
			for (n=minor; *n; n++)
			{
				pd->mst_name[8] = pd->slv_name[8] = *m;
				pd->mst_name[9] = pd->slv_name[9] = *n;

				/*--------------------------------------------------------
				 * open master pty
				 */
				pd->mst_fd = open(pd->mst_name, O_RDWR);
				if (pd->mst_fd != -1)
				{
					/*----------------------------------------------------
					 * if master opened OK, try to access slave pty
					 */
					if (access(pd->slv_name, R_OK | W_OK) == 0)
					{
						return (0);
					}

					/*----------------------------------------------------
					 * close the master & try again with next name
					 */
					xvt_pty_close(pd);
				}
			}
		}
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * open the "slave"
 */
static int xvt_pty_slave (PTY_DATA *pd)
{
	/*--------------------------------------------------------------------
	 * Open the slave pty
	 */
	pd->slv_fd = open(pd->slv_name, O_RDWR);
	if (pd->slv_fd < 0)
	{
		xvt_pty_close(pd);
		return (-1);
	}

	return (0);
}

#else

/*------------------------------------------------------------------------
 * PTY type: SYSV or SVR4
 */
#include <stropts.h>

/*------------------------------------------------------------------------
 * list of streams modules to be pushed
 */
static const char *	xvt_pty_modules[]	=
{
	/*--------------------------------------------------------------------
	 * all systems load this module
	 */
	"ptem",				/* pseudo-tty module		*/

	/*--------------------------------------------------------------------
	 * OSF modules
	 */
#if defined(_OSF_SOURCE)
	"ldtty",			/* line discipline module	*/

	/*--------------------------------------------------------------------
	 * AIX modules
	 */
#elif defined(_AIX) || defined(AIX)
	"ldterm",			/* line discipline module	*/

	/*--------------------------------------------------------------------
	 * generic modules
	 */
#else
	"ldterm",			/* line discipline module	*/
	"ttcompat",			/* BSD compatability module	*/
#endif

	0
};

/*------------------------------------------------------------------------
 * get "master" & "slave" names & open the "master"
 */
static int xvt_pty_master (PTY_DATA *pd)
{
	int rc;

	/*--------------------------------------------------------------------
	 * get master name
	 */
	strcpy(pd->mst_name, "/dev/ptmx");

	/*--------------------------------------------------------------------
	 * open master pty if needed
	 */
	if (pd->mst_fd < 0)
	{
		pd->mst_fd = open(pd->mst_name, O_RDWR);
		if (pd->mst_fd == -1)
			return (-1);

		/*----------------------------------------------------------------
		 * grant access to slave pty
		 */
		rc = grantpt(pd->mst_fd);
		if (rc == -1)
		{
#if 0
			xvt_pty_close(pd);
			return (-1);
#endif
	}

		/*----------------------------------------------------------------
		 * unlock the master/slave pair
		 */
		rc = unlockpt(pd->mst_fd);
		if (rc == -1)
		{
#if 0
			xvt_pty_close(pd);
			return (-1);
#endif
		}
	}

	/*--------------------------------------------------------------------
	 * get name of the slave pty
	 */
	strcpy(pd->slv_name, ptsname(pd->mst_fd));

	return (0);
}

/*------------------------------------------------------------------------
 * open the "slave"
 */
static int xvt_pty_slave (PTY_DATA *pd)
{
	/*--------------------------------------------------------------------
	 * Open the slave pty
	 */
	pd->slv_fd = open(pd->slv_name, O_RDWR);
	if (pd->slv_fd < 0)
	{
		xvt_pty_close(pd);

		return (-1);
	}

	/*--------------------------------------------------------------------
	 * load appropriate streams modules
	 *
	 * Note: we push a module only if it is not already on the stream.
	 */
	{
		const char **	module;

		for (module = xvt_pty_modules; *module; module++)
		{
			if (ioctl(pd->slv_fd, I_FIND, *module) == 0)
				ioctl(pd->slv_fd, I_PUSH, *module);
		}
	}

	return (0);
}
#endif
