/*------------------------------------------------------------------------
 * pipe routines
 */
#include "xvtcommon.h"

/*------------------------------------------------------------------------
 * xvt_pipe_open() - open a pipe
 */
int xvt_pipe_open (PIPE *p, const char *cmd, const char *mode)
{
	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (p == 0)
	{
		return (-1);
	}

	if (cmd == 0 || *cmd == 0)
	{
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * Check mode
	 */
	if (mode == 0 || (*mode != 'r' && *mode != 'w'))
	{
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * open the pipe
	 */
	{
		char *			argv[4];
		int				pid;
		int				fd[2];
		FILE *			fp;
		const char *	exec_shell;

		/*----------------------------------------------------------------
		 * get exec shell
		 */
		exec_shell = getenv("SHELL");
		if (exec_shell == 0 || *exec_shell == 0)
			exec_shell = "/bin/sh";

		/*----------------------------------------------------------------
		 * Fill in arg vector
		 */
		argv[0] = (char *)exec_shell;
		argv[1] = (char *)"-c";
		argv[2] = (char *)cmd;
		argv[3] = 0;

		/*----------------------------------------------------------------
		 * Open pipe
		 */
		if (pipe(fd) != 0)
		{
			return (-1);
		}

		/*----------------------------------------------------------------
		 * ignore any SIGPIPE signals
		 */
		sys_sig_set(SIGPIPE, SIG_IGNORE, 0);

		/*----------------------------------------------------------------
		 * Fork & exec sub process
		 */
		pid = xvt_child_fork();
		if (pid == -1)
		{
			close(fd[0]);
			close(fd[1]);
			return (-1);
		}

		/*----------------------------------------------------------------
		 * parent code
		 */
		if (pid)
		{
			/*------------------------------------------------------------
			 * check if input or output
			 */
			if (*mode == 'r')
			{
				/*--------------------------------------------------------
				 * create stream for input side of pipe
				 */
				fp = fdopen(fd[0], mode);

				/*--------------------------------------------------------
				 * close output side of pipe
				 */
				close (fd[1]);
			}
			else
			{
				/*--------------------------------------------------------
				 * create stream for output side of pipe
				 */
				fp = fdopen(fd[1], mode);

				/*--------------------------------------------------------
				 * close input side of pipe
				 */
				close (fd[0]);
			}

			/*------------------------------------------------------------
			 * fill in pipe struct
			 */
			p->fp   = fp;
			p->pid  = pid;

			return (0);
		}

		/*----------------------------------------------------------------
		 * child code
		 */
		{
			/*------------------------------------------------------------
			 * check input or output
			 */
			if (*mode == 'r')
			{
				/*--------------------------------------------------------
				 * dup output side of pipe to stdout
				 */
				dup2(fd[1], fileno(stdout));
			}
			else
			{
				/*--------------------------------------------------------
				 * dup input side of pipe to stdin
				 */
				dup2(fd[0], fileno(stdin));
			}

			/*------------------------------------------------------------
			 * close both sides of pipe
			 */
			close(fd[0]);
			close(fd[1]);

			/*------------------------------------------------------------
			 * point stderr to bit bucket
			 */
			fclose(stderr);
			fp = fopen("/dev/null", "w");

			/*------------------------------------------------------------
			 * execute the command
			 */
			execvp(argv[0], argv);

			/*------------------------------------------------------------
			 * if we get here, the exec failed
			 */
			_exit(EXIT_FAILURE);
			/*NOTREACHED*/
		}
	}
}

/*------------------------------------------------------------------------
 * xvt_pipe_close() - close a pipe
 *
 * returns:
 *		>=0		exit status of subtask
 *		 -1		error
 *		 -2		could not get subtask status
 */
int xvt_pipe_close (PIPE *p)
{
	int		status;
	int		pid;
	int		rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (p == 0)
	{
		return (-1);
	}

	if (p->fp == 0)
	{
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * close the file pointer and clear it
	 */
	fclose(p->fp);
	p->fp = 0;

	/*--------------------------------------------------------------------
	 * wait for child to terminate
	 */
	pid = p->pid;
	while (TRUE)
	{
		rc = waitpid(pid, &status, 0);
		if (rc < 0 && errno == EINTR)
			continue;
		break;
	}
	p->pid = 0;

	/*--------------------------------------------------------------------
	 * An error from waitpid could be a real error, or it could indicate
	 * that an asynchronous SIGCLD routine caught the PID.
	 */
	if (rc < 0)
	{
		return (-2);
	}

	/*--------------------------------------------------------------------
	 * get exit code from child
	 */
	rc = (status >> 8) & 0xff;

	return (rc);
}
