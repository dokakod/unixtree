/*------------------------------------------------------------------------
 * printer routines
 */
#include "termcommon.h"

/*------------------------------------------------------------------------
 * term_prt_open() - open a printer
 *
 *		mode =	P_SPOOL	0
 *				P_PIPE	1
 *				P_FILE	2
 *			
 *
 *		str  =	P_SPOOL		name of spool dest (NULL for default)
 *				P_PIPE		command to pipe to
 *				P_FILE		filename to output to
 *				P_NONE		just cached
 */
PRINTER * term_prt_open (int mode, const char *str, char *msgbuf)
{
	char		tmp_msgbuf[256];
	PRINTER *	p;

	/*--------------------------------------------------------------------
	 * check for NULL msgbuf
	 */
	if (msgbuf == 0)
		msgbuf = tmp_msgbuf;
	*msgbuf = 0;

	/*--------------------------------------------------------------------
	 * now do the open
	 */
	switch (mode)
	{
	case P_SPOOL:
		p = term_prt_open_spool(str, msgbuf);
		break;

	case P_PIPE:
		p = term_prt_open_pipe(str, msgbuf);
		break;

	case P_FILE:
		p = term_prt_open_file(str, msgbuf);
		break;

	case P_NONE:
		p = term_prt_open_none(str, msgbuf);
		break;

	default:
		sprintf(msgbuf, "Invalid printer type %d", mode);
		p = 0;
		break;
	}

	return (p);
}

/*------------------------------------------------------------------------
 * term_prt_open_spool() - open a printer to a spooler
 */
PRINTER * term_prt_open_spool (const char *dest, char *msgbuf)
{
	PRINTER *		p;
	struct stat		stbuf;
	PIPE			pipe;
	char			cmd[BUFSIZ];
	const char *	cmd_name;
	const char *	dest_opt;
	int				rc;

	/*--------------------------------------------------------------------
	 * allocate struct
	 */
	p = (PRINTER *)MALLOC(sizeof(*p));
	if (p == 0)
	{
		sprintf(msgbuf, "No memory");
		return (0);
	}

	/*--------------------------------------------------------------------
	 * determine whether SYS-V "lp" cmd or BSD cmd "lpr" should be used
	 */
	while (TRUE)
	{
		rc = stat("/usr/bin/lp", &stbuf);
		if (rc < 0 && errno == EINTR)
			continue;
		break;
	}

	if (rc == 0)
	{
		cmd_name = "/usr/bin/lp";
		dest_opt = "-d";
	}
	else
	{
		cmd_name = "/usr/bin/lpr";
		dest_opt = "-P";
	}

	/*--------------------------------------------------------------------
	 * create the cmd string
	 */
	strcpy(cmd, cmd_name);
	if (dest != 0 && *dest != 0)
	{
		strcat(cmd, " ");
		strcat(cmd, dest_opt);
		strcat(cmd, " \"");
		strcat(cmd, dest);
		strcat(cmd, "\"");
	}

	/*--------------------------------------------------------------------
	 * open the pipe
	 */
	rc = term_pipe_open(&pipe, cmd, "w", msgbuf);
	if (rc)
	{
		FREE(p);
		return (0);
	}

	/*--------------------------------------------------------------------
	 * store stuff
	 */
	p->prt_mode = P_SPOOL;
	p->prt_pid	= pipe.pid;
	p->prt_fp	= pipe.fp;
	strcpy(p->prt_str, cmd);

	return (p);
}

/*------------------------------------------------------------------------
 * term_prt_open_pipe() - open a printer piped to a command
 */
PRINTER * term_prt_open_pipe (const char *cmd, char *msgbuf)
{
	PRINTER *	p;
	PIPE		pipe;
	int			rc;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (cmd == 0 || *cmd == 0)
	{
		sprintf(msgbuf, "No command specified");
		return (0);
	}

	/*--------------------------------------------------------------------
	 * allocate struct
	 */
	p = (PRINTER *)MALLOC(sizeof(*p));
	if (p == 0)
	{
		sprintf(msgbuf, "No memory");
		return (0);
	}

	/*--------------------------------------------------------------------
	 * open the pipe
	 */
	rc = term_pipe_open(&pipe, cmd, "w", msgbuf);
	if (rc)
	{
		FREE(p);
		return (0);
	}

	/*--------------------------------------------------------------------
	 * store stuff
	 */
	p->prt_mode = P_PIPE;
	p->prt_pid	= pipe.pid;
	p->prt_fp	= pipe.fp;
	strcpy(p->prt_str, cmd);

	return (p);
}

/*------------------------------------------------------------------------
 * term_prt_open_file() - open a printer to a file
 */
PRINTER * term_prt_open_file (const char *filename, char *msgbuf)
{
	PRINTER *	p;
	FILE *		fp;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (filename == 0 || *filename == 0)
	{
		sprintf(msgbuf, "No filename specified");
		return (0);
	}

	/*--------------------------------------------------------------------
	 * allocate struct
	 */
	p = (PRINTER *)MALLOC(sizeof(*p));
	if (p == 0)
	{
		sprintf(msgbuf, "No memory");
		return (0);
	}

	/*--------------------------------------------------------------------
	 * open the file
	 */
	fp = fopen(filename, "w");
	if (fp == 0)
	{
		sprintf(msgbuf, "Cannot open file %s", filename);
		FREE(p);
		return (0);
	}

	/*--------------------------------------------------------------------
	 * store stuff
	 */
	p->prt_mode	= P_FILE;
	p->prt_pid	= 0;
	p->prt_fp	= fp;
	strcpy(p->prt_str, filename);

	return (p);
}

/*------------------------------------------------------------------------
 * term_prt_open_none() - open a dummy printer
 */
PRINTER * term_prt_open_none (const char *str, char *msgbuf)
{
	PRINTER *	p;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (str == 0)
		str = "";

	/*--------------------------------------------------------------------
	 * allocate struct
	 */
	p = (PRINTER *)MALLOC(sizeof(*p));
	if (p == 0)
	{
		sprintf(msgbuf, "No memory");
		return (0);
	}

	/*--------------------------------------------------------------------
	 * store stuff
	 */
	p->prt_mode	= P_NONE;
	p->prt_pid	= 0;
	p->prt_fp	= 0;
	strcpy(p->prt_str, str);

	return (p);
}

/*------------------------------------------------------------------------
 * term_prt_close() - close a printer
 */
int term_prt_close (PRINTER *p, char *msgbuf)
{
	char	tmp_msgbuf[256];
	PIPE	pipe;
	int		rc;

	/*--------------------------------------------------------------------
	 * check for NULL msgbuf
	 */
	if (msgbuf == 0)
		msgbuf = tmp_msgbuf;
	*msgbuf = 0;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (p == 0)
	{
		return (0);
	}

	/*--------------------------------------------------------------------
	 * do the appropriate close
	 */
	switch (p->prt_mode)
	{
	case P_SPOOL:
	case P_PIPE:
		pipe.fp  = p->prt_fp;
		pipe.pid = p->prt_pid;
		rc = term_pipe_close(&pipe, msgbuf);
		break;

	case P_FILE:
		fclose(p->prt_fp);
		rc = 0;
		break;

	case P_NONE:
		rc = 0;
		break;
	}

	/*--------------------------------------------------------------------
	 * free the printer struct
	 */
	FREE(p);

	return (rc);
}

/*------------------------------------------------------------------------
 * term_prt_output_str() - output a string to a printer
 */
int term_prt_output_str (PRINTER *p, const char *str)
{
	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (p == 0 || str == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * output string to stream
	 */
	if (p->prt_fp != 0)
		fputs(str, p->prt_fp);

	return (0);
}

/*------------------------------------------------------------------------
 * term_prt_output_fmt() - output a formatted to a printer
 */
int term_prt_output_fmt (PRINTER *p, const char *fmt, ...)
{
	va_list args;
	int		rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (p == 0 || fmt == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * output string to stream
	 */
	va_start(args, fmt);
	rc = term_prt_output_var(p, fmt, args);
	va_end(args);

	return (rc);
}

/*------------------------------------------------------------------------
 * term_prt_output_var() - output a variadic string to a printer
 */
int term_prt_output_var (PRINTER *p, const char *fmt, va_list args)
{
	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (p == 0 || fmt == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * output string to stream
	 */
	if (p->prt_fp != 0)
		vfprintf(p->prt_fp, fmt, args);

	return (0);
}
