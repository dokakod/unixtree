/*------------------------------------------------------------------------
 * cmd-exec routines
 */
#include "termcommon.h"

/*------------------------------------------------------------------------
 * term_strip() - strip white-space from a string
 */
static void term_strip (char *str)
{
	char *	s;
	char *	t;

	t = str;
	for (s=str; *s; s++)
	{
		if (! isspace(*s))
			t = s+1;
	}
	*t = 0;
}

#if V_WINDOWS

/*------------------------------------------------------------------------
 * term_win_cmdline() - internal create a cmd-line from an argv list
 */
static char * term_win_cmdline (char **argv)
{
	char *	pCommandLine;
	int		iLen;
	int		i;

	/*--------------------------------------------------------------------
	 * count length needed
	 */
	iLen = 1;
	for (i = 0; argv[i]; i++)
		iLen += strlen(argv[i]) + 3;

	/*--------------------------------------------------------------------
	 * allocate memory for cmd-line
	 */
	pCommandLine = (char *)MALLOC(iLen);
	if (pCommandLine == 0)
	{
		return (NULL);
	}

	/*--------------------------------------------------------------------
	 * copy all args, quoting each one as we go
	 */
	*pCommandLine = 0;
	for (i = 0; argv[i]; i++)
	{
		if (i > 0)
			strcat(pCommandLine, " ");
		strcat(pCommandLine, "\"");
		strcat(pCommandLine, argv[i]);
		strcat(pCommandLine, "\"");
	}

	return (pCommandLine);
}

/*------------------------------------------------------------------------
 * term_win_system() - internal NT execute a cmd line
 */
static int term_win_system (const char *pCommandLine, const char *dir,
	int interactive, char *msgbuf)
{
	char				tmp_msgbuf[256];
	char				err_path[MAX_PATHLEN];
	char *				pCmdBuf;
	DWORD				dwWaitObject;
	DWORD				dwExitCode;
	STARTUPINFO			start_info;
	PROCESS_INFORMATION	process_info;
	HANDLE				hStdInput	= GetStdHandle(STD_INPUT_HANDLE);
	HANDLE				hStdOutput	= GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE				hStdError	= GetStdHandle(STD_ERROR_HANDLE);
	int					iLen;
	int					rc;

	/*--------------------------------------------------------------------
	 * check for NULL msgbuf
	 */
	if (msgbuf == 0)
		msgbuf = tmp_msgbuf;
	*msgbuf = 0;

	/*--------------------------------------------------------------------
	 * cache copy of command line
	 */
	iLen = strlen(pCommandLine) + 3;
	pCmdBuf = (char *)MALLOC(iLen);
	if (pCmdBuf == 0)
	{
		sprintf(msgbuf, "No memory");
		return (-1);
	}

	strcpy(pCmdBuf, pCommandLine);

	/*--------------------------------------------------------------------
	 * create error file if not interactive
	 */
	if (! interactive)
	{
		char	tmp_path[MAX_PATHLEN];
		char *	tmp;

		/*----------------------------------------------------------------
		 * get temp directory
		 */
		tmp = getenv("TMP");

		if (tmp == 0)
			tmp = getenv("TEMP");

		if (tmp == 0)
			tmp = getenv("TMPDIR");

		if (tmp == 0)
		{
			tmp = getenv("windir");
			if (tmp != 0)
			{
				sprintf(tmp_path, "%s\\temp", tmp);
				tmp = tmp_path;
			}
		}

		if (tmp == 0)
			tmp = ".";

		/*----------------------------------------------------------------
		 * create path for error file
		 */
		sprintf(err_path, "%s\\err%05d.tmp", tmp, GetCurrentProcessId());

		/*----------------------------------------------------------------
		 * setup error file
		 */
		hStdError = CreateFile(err_path,
			GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			0,
			CREATE_NEW,
			0,
			NULL);

		if (hStdError == INVALID_HANDLE_VALUE)
		{
			FREE(pCmdBuf);
			sprintf(msgbuf, "Cannot create temp file %s", err_path);
			return (-1);
		}
	}
	else
	{
		*err_path = 0;
	}

	/*--------------------------------------------------------------------
	 * fill in the start info object
	 */
	start_info.cb              = sizeof(STARTUPINFO);
	start_info.lpReserved      = NULL;
	start_info.lpDesktop       = NULL;
	start_info.lpTitle         = NULL;
	start_info.dwX             = 0;
	start_info.dwY             = 0;
	start_info.dwXSize         = 0;
	start_info.dwYSize         = 0;
	start_info.dwXCountChars   = 80;
	start_info.dwYCountChars   = 25;
	start_info.dwFillAttribute = 0;
	start_info.dwFlags         = STARTF_USESTDHANDLES;
	start_info.wShowWindow     = 0;
	start_info.cbReserved2     = 0;
	start_info.lpReserved2     = NULL;
	start_info.hStdInput       = hStdInput;
	start_info.hStdOutput      = hStdOutput;
	start_info.hStdError       = hStdError;

	/*--------------------------------------------------------------------
	 * Try and create the process
	 */
	rc = CreateProcess(NULL,
		pCmdBuf,
		NULL,
		NULL,
		TRUE,
		0,
		NULL,
		dir,
	   	&start_info,
		&process_info);

	/*--------------------------------------------------------------------
	 * free cmdline buffer & close our handles
	 */
	FREE(pCmdBuf);

	if (*err_path != 0)
		CloseHandle(hStdError);

	/*--------------------------------------------------------------------
	 * On error just return error (make sure we delete the error file)
	 */
	if (! rc)
	{
		if (*err_path != 0)
			DeleteFile(err_path);
		sprintf(msgbuf, "Command failed to exec");
		return (-1);
	}

	CloseHandle(process_info.hThread);

	/*--------------------------------------------------------------------
	 * Wait for the process to exit
	 */
	dwWaitObject = WaitForSingleObject(process_info.hProcess, INFINITE);

	/*--------------------------------------------------------------------
	 * Did the process exit or was there an error ?
	 */
	if (dwWaitObject != WAIT_OBJECT_0)
	{
		if (*err_path != 0)
			DeleteFile(err_path);
		CloseHandle(process_info.hProcess);
		sprintf(msgbuf, "Command failed to exec");
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * Get the exit code
	 */
	GetExitCodeProcess(process_info.hProcess, &dwExitCode);
	CloseHandle(process_info.hProcess);

	rc = (int)dwExitCode;

	/*--------------------------------------------------------------------
	 * if exit-code not 0, try to read in error msg
	 */
	if (rc)
	{
		if (*err_path != 0)
		{
			hStdError = CreateFile(err_path,
				GENERIC_READ,
				0,
				NULL,
				OPEN_EXISTING,
				0,
				NULL);

			if (hStdError != INVALID_HANDLE_VALUE)
			{
				DWORD nRead = 0;

				ReadFile(hStdError, msgbuf, 256, &nRead, NULL);
				if (nRead != 0)
				{
					/*----------------------------------------------------
					 * strip trailing white space from string
					 */
					term_strip(msgbuf);
				}

				CloseHandle(hStdError);
			}
		}

		/*----------------------------------------------------------------
		 * check if we got any message
		 */
		if (*msgbuf == 0)
		{
			sprintf(msgbuf, "command failed with no err msg");
		}
	}

	/*--------------------------------------------------------------------
	 * delete error file
	 */
	if (*err_path != 0)
		DeleteFile(err_path);

	/*--------------------------------------------------------------------
	 * Return the exit code of the process
	 */
	return (rc);
}

/*------------------------------------------------------------------------
 * term_exec_argv() - execute a cmd from an argv list
 */
int term_exec_argv (TERM_DATA *t, const char *dir, char **argv,
	int interactive, char *msgbuf)
{
	char *pCmdLine = term_win_cmdline(argv);
	int rc;

	if (pCmdLine == 0)
		return (-1);

	rc = term_win_system(pCmdLine, dir, interactive, msgbuf);

	FREE(pCmdLine);

	return (rc);
}

/*------------------------------------------------------------------------
 * term_exec_cmd() - execute a cmd from a command line
 */
int term_exec_cmd (TERM_DATA *t, const char *dir, const char *cmd,
	int interactive, char *msgbuf)
{
	if (cmd == 0 || *cmd == 0)
	{
		interactive = TRUE;
		cmd = getenv("COMSPEC");
	}

	if (cmd == 0 || *cmd == 0)
	{
		interactive = TRUE;
		cmd = "command.com";
	}

	return term_win_system(cmd, dir, interactive, msgbuf);
}

#else

/*------------------------------------------------------------------------
 * term_exec_sigrtn() - signal handler
 */
static void term_exec_sigrtn (int sig)
{
	/*--------------------------------------------------------------------
	 * We do nothing here except re-enable the signal for next time.
	 */
	signal(sig, term_exec_sigrtn);
}

/*------------------------------------------------------------------------
 * term_exec_argv() - execute a cmd from an argv list
 */
int term_exec_argv (TERM_DATA *t, const char *dir, char **argv,
	int interactive, char *msgbuf)
{
	char		tmp_msgbuf[256];
	char		err_path[MAX_PATHLEN];
	FILE *		fp;
	int			pid;

	/*--------------------------------------------------------------------
	 * check for NULL msgbuf
	 */
	if (msgbuf == 0)
		msgbuf = tmp_msgbuf;
	*msgbuf = 0;

	/*--------------------------------------------------------------------
	 * create pathname for error file if not interactive
	 */
	if (! interactive)
	{
		const char *	tmp;

		/*----------------------------------------------------------------
		 * get tmp directory
		 */
		tmp = getenv("TMP");
		if (tmp == 0 || *tmp == 0)
			tmp = "/tmp";

		/*----------------------------------------------------------------
		 * get error filename
		 */
		sprintf(err_path, "%s/err%05d.tmp", tmp, (int)getpid());
	}
	else
	{
		*err_path = 0;
	}

	/*--------------------------------------------------------------------
	 * fork sub-task
	 */
	pid = fork();
	if (pid < 0)
	{
		sprintf(msgbuf, "Cannot fork");
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * child code
	 */
	if (pid == 0)
	{
		/*----------------------------------------------------------------
		 * change directory if one given
		 */
		if (dir != 0 && *dir != 0)
		{
			if (strcmp(dir, ".") != 0)
				chdir(dir);
		}

		/*----------------------------------------------------------------
		 * set stderr to error file if not interactive
		 */
		if (*err_path != 0)
		{
			close(fileno(stderr));
			fp = fopen(err_path, "w");
		}
		else
		{
			fp = 0;
		}

		/*----------------------------------------------------------------
		 * execute the sub-task
		 */
		execvp(argv[0], argv);

		/*----------------------------------------------------------------
		 * if we get here, the exec failed
		 */
		if (fp != 0)
			fprintf(fp, "Exec of %s failed\n", argv[0]);
		_exit(1);
		/*NOTREACHED*/
	}

	/*--------------------------------------------------------------------
	 * parent code
	 */
	{
		OS_SIG_RTN *	old_sigintr_rtn;
		OS_SIG_RTN *	old_sigquit_rtn;
		int				status;
		int				rc;
		int				lo_byte;
		int				hi_byte;

		/*----------------------------------------------------------------
		 * If the child process does not grab the controlling terminal,
		 * then if a SIGINT or a SIGQUIT is sent, it is delivered to BOTH
		 * the parent & the child.  We don't care about the child, but we
		 * care about the parent.
		 *
		 * Therefore, we setup a handler to ignore any SIGINTs & SIGQUITs
		 * while the child is running.
		 * We cache any old handlers & restore them.
		 */
		old_sigintr_rtn = signal(SIGINT,  term_exec_sigrtn);
		old_sigquit_rtn = signal(SIGQUIT, term_exec_sigrtn);

		/*----------------------------------------------------------------
		 * wait for child to finish
		 */
		while (TRUE)
		{
			if (t == 0 || t->evt_rtn == 0)
			{
				/*--------------------------------------------------------
				 * no event routine - just wait for child
				 */
				rc = waitpid(pid, &status, 0);
				if (rc < 0 && errno == EINTR)
					continue;
				break;
			}
			else
			{
				/*--------------------------------------------------------
				 * got event routine - use it
				 */
				term_screen_dev_busy(t, TERM_EVENT_WAIT, 5);

				/*--------------------------------------------------------
				 * check child
				 */
				rc = waitpid(pid, &status, WNOHANG);
				if (rc < 0 && errno == EINTR)
					continue;
				if (rc > 0)
					break;
			}
		}

		/*----------------------------------------------------------------
		 * restore all old signal handlers
		 */
		signal(SIGINT,  old_sigintr_rtn);
		signal(SIGQUIT, old_sigquit_rtn);

		/*----------------------------------------------------------------
		 * get status of child
		 */
		if (rc < 0)
		{
			if (*err_path != 0)
				remove(err_path);

			sprintf(msgbuf, "Error in wait for pid %d", pid);
			return (-1);
		}

		/*----------------------------------------------------------------
		 * check return code & get error msg if possible
		 */
		hi_byte = (status >> 8) & 0xff;
		lo_byte = (status     ) & 0xff;

		if (lo_byte == 0 && hi_byte != 0)
		{
			/*------------------------------------------------------------
			 * command failed - get error msg if possible
			 */
			if (*err_path != 0)
			{
				fp = fopen(err_path, "r");
				if (fp != 0)
				{
					/*----------------------------------------------------
					 * read in first line of error file
					 */
					fgets(msgbuf, 256, fp);
					fclose(fp);

					/*----------------------------------------------------
					 * strip off trailing white space
					 */
					term_strip(msgbuf);
				}
			}

			/*------------------------------------------------------------
			 * check if we got any message
			 */
			if (*msgbuf == 0)
			{
				sprintf(msgbuf, "command failed with exit code of %d",
					hi_byte);
			}

			rc = hi_byte;
		}
		else if (lo_byte != 0)
		{
			/*------------------------------------------------------------
			 * command exited with a signal: get it
			 */
			term_exit_status(status, msgbuf);

			rc = lo_byte;
		}
		else
		{
			/*------------------------------------------------------------
			 * command exited with no error
			 */
			rc = 0;
		}

		/*----------------------------------------------------------------
		 * remove error file if present
		 */
		if (*err_path != 0)
			remove(err_path);

		return (rc);
	}
}

/*------------------------------------------------------------------------
 * term_exec_cmd() - execute a cmd from a command line
 */
int term_exec_cmd (TERM_DATA *t, const char *dir, const char *cmd,
	int interactive, char *msgbuf)
{
	char *argv[4];
	char *shell;

	/*--------------------------------------------------------------------
	 * get shell to use
	 */
	shell = getenv("SHELL");
	if (shell == 0)
		shell = (char *)"/bin/sh";

	/*--------------------------------------------------------------------
	 * fill in argv vector
	 */
	if (cmd == 0 || *cmd == 0)
	{
		interactive = TRUE;
		argv[0] = shell;
		argv[1] = 0;
	}
	else
	{
		argv[0] = shell;
		argv[1] = (char *)"-c";
		argv[2] = (char *)cmd;
		argv[3] = 0;
	}

	/*--------------------------------------------------------------------
	 * execute it
	 */
	return term_exec_argv(t, dir, argv, interactive, msgbuf);
}

#endif
