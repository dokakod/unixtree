/*------------------------------------------------------------------------
 * pipe routines
 */
#include "termcommon.h"

/*------------------------------------------------------------------------
 * term_pipe_errpath() - get error pathname
 */
static void term_pipe_errpath (int pid, char *err_path)
{
	const char *	tmp;

	/*--------------------------------------------------------------------
	 * get temp directory
	 */
	tmp = getenv("TMP");
	if (tmp == 0)
		tmp = "/tmp";

	/*--------------------------------------------------------------------
	 * create error pathname
	 */
	sprintf(err_path, "%s/prt%05d.tmp", tmp, pid);
}

/*------------------------------------------------------------------------
 * term_pipe_open() - open a pipe
 */
int term_pipe_open (PIPE *p, const char *cmd, const char *mode, char *msgbuf)
{
	char tmp_msgbuf[256];

	/*--------------------------------------------------------------------
	 * check for no error msg buffer
	 */
	if (msgbuf == 0)
		msgbuf = tmp_msgbuf;
	*msgbuf = 0;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (p == 0)
	{
		sprintf(msgbuf, "Empty PIPE pointer");
		return (-1);
	}

	if (cmd == 0 || *cmd == 0)
	{
		sprintf(msgbuf, "No command specified");
		return (-1);
	}

	if (mode == 0)
	{
		sprintf(msgbuf, "No mode specified");
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * Check mode
	 */
	if (*mode != 'r' && *mode != 'w')
	{
		sprintf(msgbuf, "Invalid mode \"%s\"", mode);
		return (-1);
	}

#if V_WINDOWS
	/*--------------------------------------------------------------------
	 * Windows version
	 */
	{
		sprintf(msgbuf, "Not supported");
		return (-1);
	}
#else
	/*--------------------------------------------------------------------
	 * Unix version
	 */
	{
		char *	argv[4];
		int		pid;
		int		fd[2];
		FILE *	fp;
		char *	exec_shell;
		char	err_path[MAX_PATHLEN];

		/*----------------------------------------------------------------
		 * get exec shell
		 */
		exec_shell = getenv("SHELL");
		if (exec_shell == 0 || *exec_shell == 0)
			exec_shell = (char *)"/bin/sh";

		/*----------------------------------------------------------------
		 * Fill in arg vector
		 */
		argv[0] = exec_shell;
		argv[1] = (char *)"-c";
		argv[2] = (char *)cmd;
		argv[3] = (char *)NULL;

		/*----------------------------------------------------------------
		 * Open pipe
		 */
		if (pipe(fd) != 0)
		{
			sprintf(msgbuf, "Cannot create pipe");
			return (-1);
		}

		/*----------------------------------------------------------------
		 * Fork & exec sub process
		 */
		pid = fork();
		if (pid == -1)
		{
			close(fd[0]);
			close(fd[1]);
			sprintf(msgbuf, "Cannot fork");
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
				 * copy output side of pipe to stdout
				 */
				close(fileno(stdout));
				dup(fd[1]);
			}
			else
			{
				/*--------------------------------------------------------
				 * copy input side of pipe to stdin
				 */
				close(fileno(stdin));
				dup(fd[0]);
			}

			/*------------------------------------------------------------
			 * close both sides of pipe
			 */
			close(fd[0]);
			close(fd[1]);

			/*------------------------------------------------------------
			 * point stderr to error file
			 */
			term_pipe_errpath(getpid(), err_path);

			close(fileno(stderr));
			fp = fopen(err_path, "w");

			/*------------------------------------------------------------
			 * execute the command
			 */
			execvp(argv[0], argv);

			/*------------------------------------------------------------
			 * if we get here, the exec failed
			 */
			fprintf(fp, "Exec of \"%s\" failed.\n", argv[0]);
			exit(1);
			/*NOTREACHED*/
		}
	}
#endif
}

/*------------------------------------------------------------------------
 * term_pipe_close() - close a pipe
 */
int term_pipe_close (PIPE *p, char *msgbuf)
{
	char tmp_msgbuf[256];

	/*--------------------------------------------------------------------
	 * check for no error msg buffer
	 */
	if (msgbuf == 0)
		msgbuf = tmp_msgbuf;
	*msgbuf = 0;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (p == 0)
	{
		sprintf(msgbuf, "Null PIPE pointer");
		return (-1);
	}

	if (p->fp == 0)
	{
		sprintf(msgbuf, "NULL stream pointer");
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * Windows version
	 */
#if V_WINDOWS
	{
		sprintf(msgbuf, "Not supported");
		return (-1);
	}
#else
	/*--------------------------------------------------------------------
	 * Unix version
	 */
	{
		char	err_path[MAX_PATHLEN];
		int		status;
		int		pid;
		int		rc;

		/*----------------------------------------------------------------
		 * close the file pointer and clear it
		 */
		fclose(p->fp);
		p->fp	= 0;

		/*----------------------------------------------------------------
		 * get PID and clear it
		 */
		pid = p->pid;
		p->pid	= 0;

		/*----------------------------------------------------------------
		 * get pathname of error file
		 */
		term_pipe_errpath(pid, err_path);

		/*----------------------------------------------------------------
		 * wait for child to terminate
		 */
		while (TRUE)
		{
			rc = waitpid(pid, &status, 0);
			if (rc < 0 && errno == EINTR)
				continue;
			break;
		}

		if (rc < 0)
		{
			remove(err_path);

			sprintf(msgbuf, "Error from wait for pid %d", pid);
			return (-1);
		}

		/*----------------------------------------------------------------
		 * get exit code from child
		 */
		rc = (status >> 8) & 0xff;
		if (rc)
		{
			FILE *	fp;
			char *	s;
			char *	t;

			/*------------------------------------------------------------
			 * command failed - get error msg if possible
			 */
			fp = fopen(err_path, "r");
			if (fp != 0)
			{
				fgets(msgbuf, 256, fp);
				fclose(fp);
			}

			/*------------------------------------------------------------
			 * strip off trailing white space
			 */
			t = msgbuf;
			for (s = msgbuf; *s; s++)
			{
				if (! isspace(*s))
					t = s+1;
			}
			*t = 0;

			/*------------------------------------------------------------
			 * check if we got any message
			 */
			if (*msgbuf == 0)
			{
				sprintf(msgbuf, "command failed");
			}
		}

		/*----------------------------------------------------------------
		 * delete error file
		 */
		remove(err_path);

		return (rc);
	}
#endif
}
