/*------------------------------------------------------------------------
 * child execution functions
 */
#include "xvtcommon.h"

/*------------------------------------------------------------------------
 * fork a process
 *
 * We have to be careful to make sure that no sub-threads are copied.
 */
int xvt_child_fork (void)
{
	int pid;

	if (getenv("XVT_NO_THREADS") != 0)
		pid = fork();
	else
		pid = systhread_fork();

	return (pid);
}

/*------------------------------------------------------------------------
 * setup child process
 */
int xvt_child_setup (TERMDATA *td, int login)
{
	char **	argv	= td->argv;
	int		pid;
	int		rc;

	/*--------------------------------------------------------------------
	 * Fork process & exec child program
	 */
	pid = xvt_child_fork();
	switch (pid)
	{
	case -1:
		/*----------------------------------------------------------------
		 * error in fork
		 */
		td->xd->error_num = XVT_ERR_CANTFORK;
		sprintf(td->xd->error_msg, "Cannot fork");
		return (-1);

	case 0:
		/*----------------------------------------------------------------
		 * Child process code
		 */
		{
			char	login_argv0[XVT_MAX_PATHLEN];
			char *	real_argv0;
			int		i;

			/*------------------------------------------------------------
			 * cache our pid (for debugging)
			 */
			td->our_pid = getpid();

			/*------------------------------------------------------------
			 * bump our priority by one so parent has control
			 */
			nice(1);

			/*------------------------------------------------------------
			 * open the slave pty
			 */
			rc = xvt_pty_open(td->pd, TRUE, td->xd->termmode);
			if (rc)
			{
				/*--------------------------------------------------------
				 * Nothing we can do here except bail.
				 */
				_exit(EXIT_FAILURE);
				/*NOTREACHED*/
			}

			/*------------------------------------------------------------
			 * check if console redirection wanted
			 */
			if (td->xd->console)
			{
#if defined(TIOCCONS)
				{
					unsigned int	on = 1;

					ioctl(fileno(stdin), TIOCCONS, &on);
				}
#elif defined(SRIOCSREDIR)
				{
					int fd	= open("/dev/console", O_WRONLY);

					if (fd > 0)
					{
						ioctl(fd, SRIOCSREDIR, fileno(stdin));
						close(fd);
					}
				}
#endif
			}

			/*------------------------------------------------------------
			 * set window size
			 */
			xvt_pty_resize(td->pd, CUR_SCRN_COLS(td), CUR_SCRN_ROWS(td), 0);

			/*------------------------------------------------------------
			 * Close unused file descriptors.
			 * Hopefully, this will insure that the child will die if the
			 * parent dies & not become a zombie. (It also reduces fds in
			 * the child.)
			 *
			 * There *should* be a standard define for the number of file
			 * descriptors, but, unfortunately, there isn't.
			 */
			for (i = fileno(stderr) + 1; i < 64; i++)
				close(i);

			/*------------------------------------------------------------
			 * Lose all signals.  Normally, any signals set to be ignored
			 * by the parent will be also ignored by the child.
			 */
			sys_sig_set(SIG_ALL, SIG_DEFAULT, 0);

			/*------------------------------------------------------------
			 * set our env vars
			 */
			if (*td->dsp_var_str != 0)
				putenv(td->dsp_var_str);
			putenv(td->trm_var_str);
			putenv(td->wid_var_str);

			/*------------------------------------------------------------
			 * unset TERMCAP env var if present, since it may be incorrect
			 */
			if (getenv("TERMCAP") != 0)
				putenv((char *)"TERMCAP=");

			/*------------------------------------------------------------
			 * set env var if specified
			 */
			if (*td->env_var_str != 0)
				putenv(td->env_var_str);

			/*------------------------------------------------------------
			 * adjust argv if doing login
			 */
			real_argv0 = argv[0];
			if (login)
			{
				*login_argv0 = '-';
				strcpy(login_argv0 + 1, argv[0]);
				argv[0] = login_argv0;
			}

			/*------------------------------------------------------------
			 * execute the child process
			 */
			execvp(real_argv0, argv);

			/*------------------------------------------------------------
			 * If we get here, the exec failed.
			 */
			_exit(EXIT_FAILURE);
			/*NOTREACHED*/
		}
		break;

	default:
		/*----------------------------------------------------------------
		 * parent code
		 */
		return (pid);
	}
}
