/*------------------------------------------------------------------------
 * xvt logging routines
 */
#include "xvtcommon.h"

/*------------------------------------------------------------------------
 * get log stream
 */
static FILE * xvt_log_stream (TERMDATA *td, int log)
{
	switch (log)
	{
	case XVT_LOG_EVENTS:	return (td->xd->events_fp);
	case XVT_LOG_ACTION:	return (td->xd->action_fp);
	case XVT_LOG_OUTPUT:	return (td->xd->output_fp);
	case XVT_LOG_SCREEN:	return (td->xd->screen_fp);
	case XVT_LOG_INPKBD:	return (td->xd->inpkbd_fp);
	case XVT_LOG_KEYBRD:	return (td->xd->keybrd_fp);
	case XVT_LOG_RESFIL:	return (td->xd->resfil_fp);
	case XVT_LOG_TKOPTS:	return (td->xd->tkopts_fp);
	default:				return (0);
	}
}

/*------------------------------------------------------------------------
 * get log path
 */
static char * xvt_log_pathname (TERMDATA *td, int log)
{
	switch (log)
	{
	case XVT_LOG_EVENTS:	return (td->xd->events_path);
	case XVT_LOG_ACTION:	return (td->xd->action_path);
	case XVT_LOG_OUTPUT:	return (td->xd->output_path);
	case XVT_LOG_SCREEN:	return (td->xd->screen_path);
	case XVT_LOG_INPKBD:	return (td->xd->inpkbd_path);
	case XVT_LOG_KEYBRD:	return (td->xd->keybrd_path);
	case XVT_LOG_RESFIL:	return (td->xd->resfil_path);
	case XVT_LOG_TKOPTS:	return (td->xd->tkopts_path);
	default:				return (0);
	}
}

/*------------------------------------------------------------------------
 * get time string suitable for display
 */
static const char * xvt_log_time (char *buf)
{
	struct timeval	tv;
	struct tm *	tm;
	time_t		secs;

	gettimeofday(&tv, 0);
	secs = (time_t)tv.tv_sec;
	tm = localtime(&secs);

	sprintf(buf, "%02d.%02d.%02d.%03d",
		tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec / 1000);

	return (buf);
}

/*------------------------------------------------------------------------
 * output log prefix info (used for events log)
 *
 * Here is the place to add any timestamps, pid, etc.
 */
static void xvt_log_prefix (TERMDATA *td, FILE *fp, const char *str)
{
	if (str != 0)
	{
		char	tbuf[24];

		if (td->log_pid)
			fprintf(fp, "%-5d: ", td->our_pid);

		fprintf(fp, "%s: ", xvt_log_time(tbuf));
		fprintf(fp, "%-6s : ", str);
	}
}

/*------------------------------------------------------------------------
 * set a path for a log file
 */
void xvt_log_path (TERMDATA *td, int log, const char *path)
{
	char *	path_ptr = xvt_log_pathname(td, log);

	if (path_ptr != 0)
		strcpy(path_ptr, path);
}

/*------------------------------------------------------------------------
 * change log state
 */
void xvt_log_set (TERMDATA *td, int log, int on)
{
	FILE **			pfp;
	const char *	file;

	/*--------------------------------------------------------------------
	 * get pointer to stream-pointer
	 */
	switch (log)
	{
	case XVT_LOG_EVENTS:
		pfp = &td->xd->events_fp;	file = td->xd->events_path;	break;

	case XVT_LOG_ACTION:
		pfp = &td->xd->action_fp;	file = td->xd->action_path;	break;

	case XVT_LOG_OUTPUT:
		pfp = &td->xd->output_fp;	file = td->xd->output_path;	break;

	case XVT_LOG_SCREEN:
		pfp = &td->xd->screen_fp;	file = td->xd->screen_path;	break;

	case XVT_LOG_INPKBD:
		pfp = &td->xd->inpkbd_fp;	file = td->xd->inpkbd_path;	break;

	case XVT_LOG_KEYBRD:
		pfp = &td->xd->keybrd_fp;	file = td->xd->keybrd_path;	break;

	default:
		return;
	}

	/*--------------------------------------------------------------------
	 * now set it on/off
	 */
	if (on)
	{
		if (*pfp == 0)
		{
			if (strcmp(file, "-") == 0)
			{
				if (! td->detached)
					*pfp = stderr;
			}
			else
			{
				*pfp = fopen(file, "w");
			}
		}
	}
	else
	{
		if (*pfp != 0)
		{
			if (*pfp != stderr)
				fclose(*pfp);
			*pfp = 0;
		}
	}
}

/*------------------------------------------------------------------------
 * query whether a particular log is on
 *
 * We separated this from testing the stream pointer, since we may
 * want to add some debug-level info someday.
 */
int xvt_log_is_on (TERMDATA *td, int log)
{
	FILE *	fp = xvt_log_stream(td, log);

	return (fp != 0);
}

/*------------------------------------------------------------------------
 * event logging
 */
void xvt_log_events_fmt (TERMDATA *td, const char *event,
	const char *fmt, ...)
{
	va_list	args;

	va_start(args, fmt);
	xvt_log_events_var(td, event, fmt, args);
	va_end(args);
}

void xvt_log_events_var (TERMDATA *td, const char *event,
	const char *fmt, va_list args)
{
	if (xvt_log_is_on(td, XVT_LOG_EVENTS) && event != 0)
	{
		FILE *	fp = xvt_log_stream(td, XVT_LOG_EVENTS);

		xvt_log_prefix(td, fp, event);
		vfprintf(fp, fmt, args);
		fputc('\n', fp);
		fflush(fp);
	}
}

void xvt_log_events_buf (TERMDATA *td, const char *event,
	const unsigned char *buf, int num)
{
	if (xvt_log_is_on(td, XVT_LOG_EVENTS) && event != 0)
	{
		FILE *	fp = xvt_log_stream(td, XVT_LOG_EVENTS);
		int		i;

		xvt_log_prefix(td, fp, event);

		for (i=0; i<num; i++)
		{
			if (buf[i] < ' ')
			{
				fputc('^', fp);
				fputc(buf[i] + '@', fp);
			}
			else
			{
				fputc(buf[i], fp);
			}
		}
		fputc('\n', fp);
		fflush(fp);
	}
}

void xvt_log_events_str (TERMDATA *td, const char *event,
	const char *str)
{
	xvt_log_events_buf(td, event, (const unsigned char *)str, strlen(str));
}

/*------------------------------------------------------------------------
 * output logging
 */
void xvt_log_output_seq (TERMDATA *td, const char *fmt, ...)
{
	va_list	args;

	va_start(args, fmt);
	xvt_log_events_var(td, "output", fmt, args);

	if (xvt_log_is_on(td, XVT_LOG_OUTPUT))
	{
		FILE *	fp = xvt_log_stream(td, XVT_LOG_OUTPUT);

		/*----------------------------------------------------------------
		 * if last output was not an esc-seq, then output a NL
		 */
		if (! td->output_debug_seq)
			fputc('\n', fp);
		fputc('<', fp);
		td->output_debug_seq = TRUE;

		/*----------------------------------------------------------------
		 * output message
		 */
		vfprintf(fp, fmt, args);
		fputs(">\n", fp);
		fflush(fp);
	}
	va_end(args);
}

void xvt_log_output_chr (TERMDATA *td, const char *fmt, ...)
{
	va_list	args;

	va_start(args, fmt);
	xvt_log_events_var(td, NULL, fmt, args);

	if (xvt_log_is_on(td, XVT_LOG_OUTPUT))
	{
		FILE *	fp = xvt_log_stream(td, XVT_LOG_OUTPUT);

		/*----------------------------------------------------------------
		 * indicate not in an esc-seq
		 */
		td->output_debug_seq = FALSE;

		/*----------------------------------------------------------------
		 * output message
		 */
		vfprintf(fp, fmt, args);
		fflush(fp);
	}
	va_end(args);
}

/*------------------------------------------------------------------------
 * action logging
 */
void xvt_log_action_var (TERMDATA *td, const char *fmt, va_list args)
{
	xvt_log_events_var(td, "action", fmt, args);

	if (xvt_log_is_on(td, XVT_LOG_ACTION))
	{
		FILE *	fp = xvt_log_stream(td, XVT_LOG_ACTION);

		vfprintf(fp, fmt, args);
		fputc('\n', fp);
		fflush(fp);
	}
}

/*------------------------------------------------------------------------
 * screen logging
 */
void xvt_log_screen_buf (TERMDATA *td, const unsigned char *buf, int num)
{
	if (num < 0)
		xvt_log_events_fmt(td, "screen", "EOF");
	else
		xvt_log_events_fmt(td, "screen", "%d bytes", num);

	if (xvt_log_is_on(td, XVT_LOG_SCREEN) && num > 0)
	{
		FILE *	fp = xvt_log_stream(td, XVT_LOG_SCREEN);

		fwrite(buf, num, 1, fp);
		fflush(fp);
	}
}

/*------------------------------------------------------------------------
 * inpkbd logging
 */
void xvt_log_inpkbd_fmt (TERMDATA *td, const char *fmt, ...)
{
	va_list	args;

	va_start(args, fmt);
	xvt_log_events_var(td, "inpkbd", fmt, args);

	if (xvt_log_is_on(td, XVT_LOG_INPKBD))
	{
		FILE *	fp = xvt_log_stream(td, XVT_LOG_INPKBD);

		vfprintf(fp, fmt, args);
		fputc('\n', fp);
		fflush(fp);
	}
	va_end(args);
}

/*------------------------------------------------------------------------
 * keybrd logging
 */
void xvt_log_keybrd_buf (TERMDATA *td, const char *buf, int num)
{
	xvt_log_events_buf(td, "keybrd", (const unsigned char *)buf, num);

	if (xvt_log_is_on(td, XVT_LOG_KEYBRD))
	{
		FILE *	fp = xvt_log_stream(td, XVT_LOG_KEYBRD);

		fwrite(buf, num, 1, fp);
		fflush(fp);
	}
}
