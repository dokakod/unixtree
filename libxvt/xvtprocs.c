/*------------------------------------------------------------------------
 * libxvt external procedures
 */
#include "xvtcommon.h"

/*------------------------------------------------------------------------
 * asynchronous signal routine for death of a child
 *
 * Note that there may be a race condition where the child finishes
 * before the parent even deals with the child.  Therefore, we wait for
 * *any* pid, since we can't insure that we know the subtask PID.
 *
 * Another problem here is that the PID we catch may be for a printer
 * subtask.  But, at least, we know that pid so we can deal with it.
 */
static void xvt_procs_child_rtn (int sig, void *data)
{
	TERMDATA *	td		= (TERMDATA *)data;
	int			pid;
	int			status;
	int			exit_code;

	/*--------------------------------------------------------------------
	 * wait for child
	 */
	while (TRUE)
	{
		status = 0;
		pid = waitpid((pid_t)-1, &status, 0);
		if (pid < 0 && errno == EINTR)
			continue;
		break;
	}

	exit_code = (status >> 8);

	/*--------------------------------------------------------------------
	 * check if this PID is for the printer
	 */
	if (td->prt != 0 && pid == td->prt->prt_pid)
	{
		td->prt->prt_rc = exit_code;
	}
	else
	{
		/*----------------------------------------------------------------
		 * clear PID of child & store exit code
		 */
		td->pid_died	= pid;
		td->pid_child	= 0;
		td->exit_code	= exit_code;
	}
}

/*------------------------------------------------------------------------
 * close out everything
 */
static void xvt_procs_close (TERMDATA *td)
{
	if (td->init)
	{
		xvt_term_close_all(td);
	}

	if (td->pty_inpbufs != 0)
	{
		PTY_IOBUF *	p;
		PTY_IOBUF * n;

		for (p=td->pty_inpbufs; p; p=n)
		{
			n = p->next;
			FREE(p);
		}
	}

	if (td->pty_outbufs != 0)
	{
		PTY_IOBUF *	p;
		PTY_IOBUF * n;

		for (p=td->pty_outbufs; p; p=n)
		{
			n = p->next;
			FREE(p);
		}
	}

	sysmutex_delete(&td->pty_inpmtx);
	sysmutex_delete(&td->pty_outmtx);

	FREE(td);
}

/*------------------------------------------------------------------------
 * setup display structs
 */
static TERMDATA * xvt_procs_setup (XVT_DATA *xd, char **argv)
{
	TERMDATA *	td;
	const char *p;
	int			rc;

	/*--------------------------------------------------------------------
	 * set initial rows, cols & tab-width to env variables as follows:
	 *
	 *	LINES		number of rows in window
	 *	COLUMNS		number of cols in window
	 *	TABSIZE		number of spaces per tab
	 *
	 * This is for compatability with curses programs.
	 */
	if (xd->scr_cols <= 0)
	{
		p = getenv("LINES");
		if (p != 0 && *p != 0)
			xd->scr_rows = atoi(p);
	}

	if (xd->scr_cols <= 0)
	{
		p = getenv("COLUMNS");
		if (p != 0 && *p != 0)
			xd->scr_cols = atoi(p);
	}

	if (xd->scr_tabs <= 0)
	{
		p = getenv("TABSIZE");
		if (p != 0 && *p != 0)
			xd->scr_tabs = atoi(p);
	}

	/*--------------------------------------------------------------------
	 * validate termmode if specified
	 */
	if (xd->termmode != 0)
	{
		char msgbuf[128];

		rc = xvt_tio_build(0, xd->termmode, FALSE, msgbuf);
		if (rc)
		{
			sprintf(xd->error_msg, "termmode: %s", msgbuf);
			xd->error_num = XVT_ERR_INVARG;
			return (0);
		}
	}

	/*--------------------------------------------------------------------
	 * validate any colors specified
	 */
	rc = xvt_rgb_check(xd);
	if (rc)
		return (0);

	/*--------------------------------------------------------------------
	 * if options dump wanted, do it here
	 */
	if (xd->tkopts_fp != 0)
	{
		xvt_tk_options_dump(xd, xd->tkopts_fp, TRUE);
	}

	/*--------------------------------------------------------------------
	 * initialize data struct
	 */
	td = (TERMDATA *)MALLOC(sizeof(*td));
	if (td == 0)
	{
		sprintf(td->xd->error_msg, "No memory");
		td->xd->error_num = XVT_ERR_NOMEM;
		return (0);
	}

	memset(td, 0, sizeof(*td));

	td->no_detach	= (getenv("XVT_NO_DETACH") != 0);
	td->our_pid		= getpid();
	td->pid_child	= 0;

	sysmutex_init(&td->pty_inpmtx);
	sysmutex_init(&td->pty_outmtx);

	td->inp_data.bufptr		= td->inp_buffer;
	td->inp_data.buflen		= sizeof(td->inp_buffer);
	td->inp_data.bufcnt		= 0;
	td->inp_data.bufpos		= 0;
	td->inp_data.bufheap	= FALSE;

	td->out_data.bufptr		= td->out_buffer;
	td->out_data.buflen		= sizeof(td->out_buffer);
	td->out_data.bufcnt		= 0;
	td->out_data.bufpos		= 0;
	td->out_data.bufheap	= FALSE;

	td->inp_current			= &td->inp_data;

	/*--------------------------------------------------------------------
	 * set all "unset" values in the user-data to default values
	 */
	xvt_tk_data_init(xd, TRUE);

	/*--------------------------------------------------------------------
	 * now copy over options into our options array
	 */
	OPTION_SET(td, SM_AppCursorKeys,     FALSE);
	OPTION_SET(td, SM_AppKeypadKeys,     FALSE);
	OPTION_SET(td, SM_SmoothScroll,      FALSE);
	OPTION_SET(td, SM_OriginMode,        FALSE);
	OPTION_SET(td, SM_AutoRepeat,        FALSE);
	OPTION_SET(td, SM_PrintScreenFF,     FALSE);
	OPTION_SET(td, SM_PrintScreenRegion, FALSE);
	OPTION_SET(td, SM_PrintAuto,         FALSE);
	OPTION_SET(td, SM_ShowCursor,        TRUE);
	OPTION_SET(td, SM_Logging,           FALSE);
	OPTION_SET(td, SM_AltBuffer,         FALSE);
	OPTION_SET(td, SM_BSsendsDel,        FALSE);
	OPTION_SET(td, SM_DELsendsDel,       FALSE);
	OPTION_SET(td, SM_AutoNL,            FALSE);
	OPTION_SET(td, SM_LocalEcho,         FALSE);
	OPTION_SET(td, SM_KbdLocked,         FALSE);

	OPTION_SET(td, SM_MouseMode,         Mouse_None);
	OPTION_SET(td, SM_KbdMapMode,        Kbd_MapNone);

	OPTION_SET(td, SM_AllowWideMode,     xd->a132);
	OPTION_SET(td, SM_WideMode,          xd->c132);
	OPTION_SET(td, SM_ReverseVideo,      xd->reverse);
	OPTION_SET(td, SM_WrapAround,        xd->autowrap);
	OPTION_SET(td, SM_PrinterKeep,       xd->prtkeep);
	OPTION_SET(td, SM_HiliteCursor,      xd->ahcursor);
	OPTION_SET(td, SM_ShowScrollbar,     xd->sb_enable);
	OPTION_SET(td, SM_SpecialKeys,       xd->kpfont);
	OPTION_SET(td, SM_MarginBell,        xd->mbell);
	OPTION_SET(td, SM_ReverseWrap,       xd->revwrap);
	OPTION_SET(td, SM_ScrollOnOutput,    xd->sb_scrout);
	OPTION_SET(td, SM_ScrollOnKeypress,  xd->sb_scrkey);
	OPTION_SET(td, SM_MapOnOutput,       xd->mapoutput);
	OPTION_SET(td, SM_MapOnAlert,        xd->mapalert);
	OPTION_SET(td, SM_IgnoreBell,        xd->quiet);
	OPTION_SET(td, SM_VisualBell,        xd->visual);
	OPTION_SET(td, SM_LabelInfo,         xd->lblinfo);
	OPTION_SET(td, SM_MoreFix,           xd->cufix);

	memcpy(td->sav_options, td->cur_options, sizeof(td->sav_options));

	/*--------------------------------------------------------------------
	 * check if wide mode was allowed & requested
	 */
	if (! OPTION_GET(td, SM_AllowWideMode))
		OPTION_SET(td, SM_WideMode, FALSE);

	if (OPTION_GET(td, SM_WideMode))
		xd->scr_cols = 132;

	/*--------------------------------------------------------------------
	 * now do some sanity checks
	 */
	if (xd->scr_tabs >= xd->scr_cols)
		xd->scr_tabs = 0;

	if (xd->mbcols >= xd->scr_cols)
		xd->mbcols = (xd->scr_cols - 1);

	if (xd->fontno < 0 || xd->fontno >= XVT_NUM_FONTS)
	{
		xd->fontno = 0;
	}

	if (xvt_emul_set_tbl(0, xd->terminal) != 0)
	{
		xvt_procs_close(td);

		sprintf(td->xd->error_msg, "Invalid terminal type %s", xd->terminal);
		td->xd->error_num = XVT_ERR_INVOPT;
		return (0);
	}

	/*--------------------------------------------------------------------
	 * cache the program info into prog struct
	 */
	td->xd			= xd;
	td->argv		= argv;

	/*--------------------------------------------------------------------
	 * clear all return values in user data
	 */
	xd->term_type[0]	= 0;
	xd->term_defs		= 0;

	xd->event_rtn		= 0;
	xd->event_data		= 0;

	xd->fd_inp			= -1;
	xd->fd_out			= -1;

	xd->cur_cols		= xd->scr_cols;
	xd->cur_rows		= xd->scr_rows;

	xd->exit_code		= 0;

	xd->error_num		= XVT_OK;
	xd->error_quit		= FALSE;
	xd->error_msg[0]	= 0;

	/*--------------------------------------------------------------------
	 * cache window label & set to cmd if not specified
	 */
	if (td->xd->win_label != 0)
		strcpy(td->win_label, td->xd->win_label);
	else if (td->xd->appname != 0)
		strcpy(td->win_label, td->xd->appname);

	if (*td->win_label == 0)
	{
		if (td->argv != 0 && td->argv[0] != 0)
		{
			strcpy(td->win_label, xvt_path_basename(td->argv[0]));
		}
		else
		{
			if (td->xd->resname != 0)
				strcpy(td->ico_label, td->xd->resname);
			else
				strcpy(td->win_label, XVT_NAME);
		}
	}

	strcpy(td->org_label, td->win_label);

	/*--------------------------------------------------------------------
	 * now process icon label
	 */
	if (td->xd->ico_label != 0)
		strcpy(td->ico_label, td->xd->ico_label);
	else if (td->xd->appname != 0)
		strcpy(td->ico_label, td->xd->appname);
	else
		strcpy(td->ico_label, td->win_label);

	/*--------------------------------------------------------------------
	 * open the display
	 */
	rc = xvt_term_setup_all(td);
	if (rc)
	{
		return (0);
	}

	/*--------------------------------------------------------------------
	 * set some initial states
	 */
	td->run_interval	= 5;			/* normal timeout in ms	*/
	td->die_interval	= 100;			/* dying  timeout in ms	*/

	/*--------------------------------------------------------------------
	 * Open pseudo-tty master
	 */
	td->pd = xvt_pty_init(xd->ptyspec);
	if (td->pd == 0)
	{
		xvt_procs_close(td);

		sprintf(td->xd->error_msg, "Cannot open pseudo-tty");
		td->xd->error_num = XVT_ERR_CANTOPENPTYS;
		return (0);
	}

	/*--------------------------------------------------------------------
	 * indicate we are initialized
	 */
	td->init = TRUE;

	return (td);
}

/*------------------------------------------------------------------------
 * exec routine
 */
static int xvt_procs_exec (TERMDATA *td, int need_login)
{
	int	pid;

	/*--------------------------------------------------------------------
	 * setup signal catchers:
	 * cache our data pointer & enable catching of child death.
	 */
	sys_sig_set(SIGCHLD, xvt_procs_child_rtn, td);

	/*--------------------------------------------------------------------
	 * Execute child process (this returns as the parent)
	 */
	pid = xvt_child_setup(td, need_login);
	if (pid < 0)
	{
		return (td->xd->error_num);
	}
	td->pid_child	= pid;
	td->pid			= pid;

	/*--------------------------------------------------------------------
	 * setup signal catchers:
	 * We want to ignore these signals so they are just passed down
	 * to the child.
	 */
	sys_sig_set(SIGINT,  SIG_IGNORE, 0);
	sys_sig_set(SIGQUIT, SIG_IGNORE, 0);

	/*--------------------------------------------------------------------
	 * open the slave pty so we can report window changes
	 */
#if 0	/* DEFERRED until resize happens (to avoid race conditions) */
	xvt_pty_open(td->pd, FALSE, td->xd->termmode);
#endif

	/*--------------------------------------------------------------------
	 * now enter main loop
	 */
	xvt_term_main_loop(td);

	/*--------------------------------------------------------------------
	 * reset signal handlers
	 */
	sys_sig_set(SIGCHLD, SIG_DEFAULT, 0);
	sys_sig_set(SIGINT,  SIG_DEFAULT, 0);
	sys_sig_set(SIGQUIT, SIG_DEFAULT, 0);

	/*--------------------------------------------------------------------
	 * We get here by any of the following:
	 *
	 *	1.	The child died.
	 *	2.	We got an EOF reading child output.
	 *	3.	The WM told us to quit.
	 *
	 * If child is still alive, terminate him with extreme prejudice.
	 * Note that we already gave up on waiting for him by resetting
	 * our SIGCHLD catcher.
	 */
	if (td->pid_died == 0 &&
		td->pid_child != 0 &&
		td->pid != td->pid_child)
	{
		sys_sig_send(td->pid_child, SIGKILL);
		td->pid_child = 0;
	}

	return (XVT_OK);
}

/*------------------------------------------------------------------------
 * detach a task
 *
 * returns:
 *		-1	error
 *		 0	child  return
 *		>0	parent return (rc == pid of child)
 */
static int xvt_procs_detach (TERMDATA *td)
{
	int	pid;

	/*--------------------------------------------------------------------
	 * Check if no-detach option specified. This is for debugging.
	 */
	if (td->no_detach)
	{
		return (0);
	}

	/*--------------------------------------------------------------------
	 * Detach ourselves by forking and having the parent return.
	 * All the other happy BS needed to become a proper daemon
	 * (controlling terminals, etc.) is dealt with when we open the slave pty.
	 */
	pid = xvt_child_fork();
	switch (pid)
	{
	case -1:		/* error */
		td->xd->error_num = XVT_ERR_CANTFORK;
		sprintf(td->xd->error_msg, "Cannot fork");
		return (-1);

	case 0:			/* in child */
		td->our_pid = getpid();
		break;

	default:		/* in parent */
		return (pid);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * thread routine
 */
static int xvt_procs_thread (TERMDATA *td)
{
	THREAD_ID tid;

	/*--------------------------------------------------------------------
	 * This tells the main loop that there is a valid process,
	 * but it should not be messed with.
	 */
	td->pid = getpid();

	/*--------------------------------------------------------------------
	 * spawn off main-loop in a thread
	 */
	if (getenv("XVT_NO_THREADS") != 0)
	{
		tid = 0;
	}
	else
	{
		tid = systhread_begin(xvt_term_main_loop, td);
	}
	td->our_tid = tid;
	
	return (tid > 0 ? 0 : -1);
}

/*------------------------------------------------------------------------
 * run one xvt event loop
 */
static int xvt_procs_run_once (void *data, int ms)
{
	TERMDATA *	td	= (TERMDATA *)data;
	int			rc;

	/*--------------------------------------------------------------------
	 * validate that we are initialized
	 */
	if (! td->init)
	{
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * run the main loop
	 */
	rc = xvt_term_loop_once(td, ms);

	return (rc);
}

/*------------------------------------------------------------------------
 * setup for here processing
 */
static int xvt_procs_here_setup (TERMDATA *td, int detach)
{
	XVT_DATA *	xd = td->xd;
	int			rc;

	/*--------------------------------------------------------------------
	 * If we are detaching, first check all debug streams and
	 * "close" (i.e. clear) any which point to stderr.
	 */
	if (detach)
	{
		if (xd->events_fp == stderr)	xd->events_fp = 0;
		if (xd->action_fp == stderr)	xd->action_fp = 0;
		if (xd->output_fp == stderr)	xd->output_fp = 0;
		if (xd->screen_fp == stderr)	xd->screen_fp = 0;
		if (xd->inpkbd_fp == stderr)	xd->inpkbd_fp = 0;
		if (xd->keybrd_fp == stderr)	xd->keybrd_fp = 0;
		if (xd->resfil_fp == stderr)	xd->resfil_fp = 0;
		if (xd->tkopts_fp == stderr)	xd->tkopts_fp = 0;
	}

	/*--------------------------------------------------------------------
	 * detach from foreground if requested
	 */
	if (detach)
	{
		rc = xvt_procs_detach(td);
		if (rc < 0)
		{
			xvt_procs_close(td);
			return (-1);
		}

		if (rc > 0)
		{
			/*------------------------------------------------------------
			 * parent return - we just exit
			 */
			_exit(EXIT_SUCCESS);
		}

		td->detached = TRUE;
	}

	/*--------------------------------------------------------------------
	 * Now open our slave pty.
	 * This pty will become our new controlling terminal.
	 */
	rc = xvt_pty_open(td->pd, td->detached, td->xd->termmode);
	if (rc)
	{
		sprintf(td->xd->error_msg, "Cannot open pseudo-tty");
		td->xd->error_num = XVT_ERR_CANTOPENPTYS;
		xd->error_quit = TRUE;
		xvt_procs_close(td);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * set our env variables
	 *
	 * We do this only after we are sure that initialization was successful,
	 * since otherwise we still have to use the original term type.
	 */
	if (*td->dsp_var_str != 0)
		putenv(td->dsp_var_str);
	putenv(td->trm_var_str);
	putenv(td->wid_var_str);

	/*--------------------------------------------------------------------
	 * unset TERMCAP env variable if present, since it may be incorrect
	 */
	putenv((char *)"TERMCAP=");

	/*--------------------------------------------------------------------
	 * set window size
	 */
	xvt_pty_resize(td->pd, CUR_SCRN_COLS(td), CUR_SCRN_ROWS(td), 0);

	/*--------------------------------------------------------------------
	 * start thread for display process
	 */
	rc = xvt_procs_thread(td);

	/*--------------------------------------------------------------------
	 * set return values in prog struct
	 *
	 * Note that we always set the event data even though there may not
	 * be an event routine, since that way the caller can determine if he
	 * is in an "event-rtn" type of environment, even though there is
	 * no actual event routine to be called.
	 * Also, xvt_close_here() & xvt_clone() get the data pointer from there.
	 */
	xd->event_rtn	= rc ? xvt_procs_run_once : 0;
	xd->event_data	= td;

	if (td->detached)
	{
		xd->fd_inp	= fileno(stdin);
		xd->fd_out	= fileno(stdout);
	}
	else
	{
		xd->fd_inp	= td->pd->slv_fd;
		xd->fd_out	= td->pd->slv_fd;
	}

	return (XVT_OK);
}

/*------------------------------------------------------------------------
 * run xvt in "here" mode
 */
int xvt_here_run (XVT_DATA *xd, int detach)
{
	TERMDATA *	td;
	int			rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (xd == 0)
	{
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * setup all structs
	 */
	td = xvt_procs_setup(xd, 0);
	if (td == 0)
	{
		return (xd->error_num);
	}

	/*--------------------------------------------------------------------
	 * Now do it
	 */
	rc = xvt_procs_here_setup(td, detach);

	return (rc);
}

/*------------------------------------------------------------------------
 * close out from here mode
 */
int xvt_here_close (XVT_DATA *xd)
{
	TERMDATA *	td;

	/*--------------------------------------------------------------------
	 * check if ever initialized
	 */
	if (xd == 0)
		return (-1);

	td = (TERMDATA *)xd->event_data;
	if (td == 0 || ! td->init)
	{
		sprintf(xd->error_msg, "Not initialized");
		xd->error_num = XVT_ERR_NOTINIT;
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * If we have a thread running, we have to stop it.
	 */
	if (td->our_tid != 0)
	{
		td->bail = XVT_DIE_LOOPS;
		systhread_wait(td->our_tid);
	}

	/*--------------------------------------------------------------------
	 * free all allocated resources
	 */
	xvt_procs_close(td);

	return (XVT_OK);
}

/*------------------------------------------------------------------------
 * setup to run a sub-task
 */
static TERMDATA * xvt_procs_task_setup (XVT_DATA *xd, char **argv)
{
	TERMDATA *	td;
	char		fullpath[XVT_MAX_PATHLEN];
	int			rc;

	/*--------------------------------------------------------------------
	 * validate the cmd to execute
	 */
	rc = xvt_path_find(fullpath, *argv);
	if (rc)
	{
		xd->error_num = XVT_ERR_INVARG;
		sprintf(xd->error_msg, "Filename \"%s\" not in path", *argv);
		return (0);
	}

	rc = access(fullpath, X_OK);
	if (rc)
	{
		xd->error_num = XVT_ERR_CANTEXEC;
		sprintf(xd->error_msg, "Filename \"%s\" not executable", *argv);
		return (0);
	}

	/*--------------------------------------------------------------------
	 * do all the setup
	 */
	td = xvt_procs_setup(xd, argv);
	if (td == 0)
		return (0);

	/*--------------------------------------------------------------------
	 * set window title again if label-info wanted
	 */
	if (xd->lblinfo)
	{
		xvt_term_set_win_title(td, td->win_label);
	}

	td->argv = argv;
	return (td);
}

/*------------------------------------------------------------------------
 * run xvt in "task" mode
 */
int xvt_task_run (XVT_DATA *xd, char **argv)
{
	TERMDATA *	td;
	char *		temp_argv[2];
	int			need_login	= (argv == 0);
	int			rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (xd == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * check if any command specified
	 */
	if (argv == 0 || *argv == 0)
	{
		const char *	env;

		/*----------------------------------------------------------------
		 * no command specified, just use shell
		 */
		env = getenv("SHELL");
		if (env == 0)
			env = XVT_SHELL;

		temp_argv[0] = (char *)env;
		temp_argv[1] = 0;

		argv = temp_argv;
	}

	/*--------------------------------------------------------------------
	 * setup task processing
	 */
	td = xvt_procs_task_setup(xd, argv);
	if (td == 0)
	{
		return (xd->error_num);
	}

	/*--------------------------------------------------------------------
	 * run the main loop
	 */
	if (need_login)
		need_login = xd->login;
	rc = xvt_procs_exec(td, need_login);

	/*--------------------------------------------------------------------
	 * store exit code back
	 */
	xd->exit_code = td->exit_code;

	/*--------------------------------------------------------------------
	 * free all allocated resources
	 */
	xvt_procs_close(td);

	return (rc);
}

/*------------------------------------------------------------------------
 * create a clone of the current task
 */
int xvt_here_clone (XVT_DATA *xd)
{
	TERMDATA *	td;
	TTY_STRUCT	tty;
	int			rc;

	/*--------------------------------------------------------------------
	 * can't clone if not initialized
	 */
	if (xd == 0)
		return (-1);

	td = (TERMDATA *)xd->event_data;
	if (td == 0 || ! td->init)
	{
		sprintf(xd->error_msg, "Not initialized");
		xd->error_num = XVT_ERR_NOTINIT;
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * can't clone if not detached
	 */
	if (! td->detached || td->no_detach)
	{
		sprintf(xd->error_msg, "Not detached");
		xd->error_num = XVT_ERR_CANTDETACH;
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * first thing is to detach ourselves
	 */
	rc = xvt_procs_detach(td);
	if (rc < 0)
	{
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * Turn on PIDs in event log so we can identify which process an entry
	 * is from.
	 */
	td->log_pid = TRUE;

	/*--------------------------------------------------------------------
	 * The parent can just return & keep going.
	 * The child does all the work.
	 */
	if (rc > 0)
	{
		int pid = rc;
		int status;

		/*----------------------------------------------------------------
		 * The parent waits for child to die, since the child is
		 * detaching again (this way the parent has no zombies attached).
		 * Note that we totally ignore any exit status.
		 */
		while (TRUE)
		{
			rc = waitpid(pid, &status, 0);
			if (rc < 0 && errno == EINTR)
				continue;
			break;
		}
		
		return (XVT_OK);
	}

	/*--------------------------------------------------------------------
	 * We are now a child process, so we will either sucessfully restart
	 * or we will exit.
	 *
	 * At this point, we are a proper sub-task of the original program.
	 * So, we detach again & let our parent continue on.
	 *
	 * But first, we do any shutdown needed.
	 */
	{
		/*----------------------------------------------------------------
		 * cache current tty state
		 */
		xvt_pty_getmode(td->pd, &tty);

		/*----------------------------------------------------------------
		 * Now do any shutdown needed
		 */
		xvt_term_shutdown(td);

		/*----------------------------------------------------------------
		 * Now we detach again
		 */
		rc = xvt_procs_detach(td);
		if (rc != 0)
		{
			_exit(EXIT_SUCCESS);
		}
	}

	/*--------------------------------------------------------------------
	 * We *should* be at this point a true daemon. Now open a new window.
	 */
	rc = xvt_term_restart(td);
	if (rc)
	{
		_exit(EXIT_FAILURE);
	}

	/*--------------------------------------------------------------------
	 * Now open our slave pty.
	 *
	 * Note that after here we are no longer a *true* daemon, since
	 * the pseudo-tty we open will become a controlling terminal.
	 */
	rc = xvt_pty_open(td->pd, td->detached, td->xd->termmode);
	if (rc)
	{
		_exit(EXIT_FAILURE);
	}

	/*--------------------------------------------------------------------
	 * reset tty state to that of our parent
	 */
	xvt_pty_setmode(td->pd, &tty);

	/*--------------------------------------------------------------------
	 * close any pending kbd states
	 */
	xvt_term_force_kbd_state(td, 0);

	/*--------------------------------------------------------------------
	 * set window & icon labels
	 */
	xvt_term_set_win_title(td, td->win_label);
	xvt_term_set_ico_title(td, td->ico_label);

	/*--------------------------------------------------------------------
	 * set window size
	 */
	xvt_pty_resize(td->pd, CUR_SCRN_COLS(td), CUR_SCRN_ROWS(td), 0);

	/*--------------------------------------------------------------------
	 * just to be safe, re-initialize our mutexes
	 */
	sysmutex_delete(&td->pty_inpmtx);
	sysmutex_delete(&td->pty_outmtx);

	sysmutex_init(&td->pty_inpmtx);
	sysmutex_init(&td->pty_outmtx);

	/*--------------------------------------------------------------------
	 * Start thread for display process.
	 * Note that we ignore the return code, since we assume that if it
	 * succeeded last time, it will succeed this time and vice-versa.
	 */
	xvt_procs_thread(td);

	/*--------------------------------------------------------------------
	 * Note that we don't store new data in the xvt_data struct, because
	 * nothing has changed.
	 */
	return (XVT_OK);
}

/*------------------------------------------------------------------------
 * run a task keeping process control
 */
int xvt_here_task (XVT_DATA *xd, int detach, char **argv)
{
	TERMDATA *	td;
	char *		temp_argv[2];
	int			need_login	= (argv == 0);
	int			pid;
	int			rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (xd == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * check if any command specified
	 */
	if (argv == 0 || *argv == 0)
	{
		const char *	env;

		/*----------------------------------------------------------------
		 * no command specified, just use shell
		 */
		env = getenv("SHELL");
		if (env == 0)
			env = XVT_SHELL;

		temp_argv[0] = (char *)env;
		temp_argv[1] = 0;

		argv = temp_argv;
	}

	/*--------------------------------------------------------------------
	 * setup task processing
	 */
	td = xvt_procs_task_setup(xd, argv);
	if (td == 0)
	{
		return (xd->error_num);
	}

	if (need_login)
		need_login = xd->login;

	/*--------------------------------------------------------------------
	 * now setup here processing
	 */
	rc = xvt_procs_here_setup(td, detach);
	if (rc)
	{
		xvt_procs_close(td);
		return (rc);
	}

	/*--------------------------------------------------------------------
	 * setup signal catchers:
	 * cache our data pointer & enable catching of child death.
	 */
	sys_sig_set(SIGCHLD, xvt_procs_child_rtn, td);

	/*--------------------------------------------------------------------
	 * Execute child process (this returns as the parent)
	 */
	pid = xvt_child_setup(td, need_login);
	if (pid < 0)
	{
		xvt_procs_close(td);
		return (td->xd->error_num);
	}
	td->pid_child = pid;

	return (pid);
}

/*------------------------------------------------------------------------
 * query whether XVT is available
 */
int xvt_available (void)
{
	return (TRUE);
}

/*------------------------------------------------------------------------
 * perform a here check
 */
int xvt_here_check (XVT_DATA *xd, int ms)
{
	TERMDATA *		td;
	XVT_EVENT_RTN *	rtn;
	int				rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (xd == 0)
		return (-1);

	td = (TERMDATA *)xd->event_data;
	if (td == 0 || ! td->init)
	{
		sprintf(xd->error_msg, "Not initialized");
		xd->error_num = XVT_ERR_NOTINIT;
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * check if any event processing needs to be done
	 */
	rtn = xd->event_rtn;
	if (rtn != 0)
	{
		rc = (rtn)(td, ms);
		if (rc)
			return (rc);
	}

	/*--------------------------------------------------------------------
	 * now check if running a subtask
	 */
	if (td->argv != 0)
	{
		/*----------------------------------------------------------------
		 * if child died, we're done
		 */
		if (td->pid_died != 0)
			return (-1);

		/*----------------------------------------------------------------
		 * if no event routine was set, we have to wait here
		 */
		if (rtn == 0 && ms > 0)
		{
			struct timeval	tv;
			int				usecs = ms * 1000;

			tv.tv_sec	= usecs / 1000000;
			tv.tv_usec	= usecs % 1000000;

			select(0, 0, 0, 0, &tv);
		}
	}

	return (0);
}

/*------------------------------------------------------------------------
 * pseudo-input/output routines
 */
int xvt_here_input (XVT_DATA *xd, const void *buf, int len)
{
	TERMDATA *	td;

	if (xd == 0)
		return (-1);

	td = (TERMDATA *)xd->event_data;
	if (td == 0 || ! td->init)
	{
		sprintf(xd->error_msg, "Not initialized");
		xd->error_num = XVT_ERR_NOTINIT;
		return (-1);
	}

	if (buf == 0)
	{
		sprintf(xd->error_msg, "NULL buffer pointer");
		xd->error_num = XVT_ERR_INVARG;
		return (-1);
	}

	if (len <= 0)
		return (XVT_OK);

	return xvt_term_pty_input(td, buf, len);
}

int xvt_here_output (XVT_DATA *xd, const void *buf, int len)
{
	TERMDATA *	td;

	if (xd == 0)
		return (-1);

	td = (TERMDATA *)xd->event_data;
	if (td == 0 || ! td->init)
	{
		sprintf(xd->error_msg, "Not initialized");
		xd->error_num = XVT_ERR_NOTINIT;
		return (-1);
	}

	if (buf == 0)
	{
		sprintf(xd->error_msg, "NULL buffer pointer");
		xd->error_num = XVT_ERR_INVARG;
		return (-1);
	}

	if (len <= 0)
		return (XVT_OK);

	return xvt_term_pty_output(td, buf, len);
}

/*------------------------------------------------------------------------
 * dump routines
 */
void xvt_dump_colors (FILE *fp, int verbose)
{
	xvt_rgb_dump(fp, verbose);
}

void xvt_dump_pointers (FILE *fp, int verbose)
{
	xvt_term_list_pointers(fp, verbose);
}
