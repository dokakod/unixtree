/*------------------------------------------------------------------------
 * printer routines
 */
#include "xvtcommon.h"

/*------------------------------------------------------------------------
 * xvt_prt_open() - open a printer
 *
 *		mode =
 *				P_NONE		print according to str
 *							NULL		P_SPOOL
 *							"str"		P_SPOOL str
 *							"| str"		P_PIPE  str+1
 *							"> str"		P_FILE  str+1
 *							">>str"		P_FILE  str+2 (append)
 *				P_SPOOL		print to system print spooler
 *				P_PIPE		print to cmd pipe
 *				P_FILE		print to file
 *			
 *
 *		str  =
 *				P_SPOOL		name of spool dest (NULL for default)
 *				P_PIPE		command to pipe to
 *				P_FILE		filename to output to
 */
PRINTER * xvt_prt_open (int mode, const char *str)
{
	/*--------------------------------------------------------------------
	 * skip over any leading white-space
	 */
	if (str != 0)
	{
		for (; *str; str++)
		{
			if (! isspace(*str))
				break;
		}
	}

	/*--------------------------------------------------------------------
	 * if mode is P_NONE, figure out actual mode from string context
	 */
	if (mode == P_NONE)
	{
		if (str == 0 || *str == 0)
		{
			mode = P_SPOOL;
		}
		else
		{
			switch (*str)
			{
			case '|':	mode = P_PIPE;	break;
			case '>':	mode = P_FILE;	break;
			default:	mode = P_SPOOL;	break;
			}
		}
	}

	/*--------------------------------------------------------------------
	 * now do the actual open
	 */
	switch (mode)
	{
	case P_SPOOL:	return xvt_prt_open_spool(str);
	case P_PIPE:	return xvt_prt_open_pipe(str);
	case P_FILE:	return xvt_prt_open_file(str);
	default:		return 0;
	}
}

/*------------------------------------------------------------------------
 * xvt_prt_open_spool() - open a printer to a spooler
 */
PRINTER * xvt_prt_open_spool (const char *dest)
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
		return (0);
	}

	/*--------------------------------------------------------------------
	 * determine what printer command should be used:
	 *	SYSV	"lp  -d <dest>"
	 *	BSD		"lpr -P <dest>"
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
	rc = xvt_pipe_open(&pipe, cmd, "w");
	if (rc)
	{
		FREE(p);
		return (0);
	}

	/*--------------------------------------------------------------------
	 * store stuff
	 */
	p->prt_fp	= pipe.fp;
	p->prt_pid	= pipe.pid;
	p->prt_rc	= -1;	
	strcpy(p->prt_str, cmd);

	return (p);
}

/*------------------------------------------------------------------------
 * xvt_prt_open_pipe() - open a printer piped to a command
 */
PRINTER * xvt_prt_open_pipe (const char *cmd)
{
	PRINTER *	p;
	PIPE		pipe;
	int			rc;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (cmd == 0 || *cmd == 0)
	{
		return (0);
	}

	/*--------------------------------------------------------------------
	 * skip over any leading pipe symbol
	 */
	for (; *cmd; cmd++)
	{
		if (! isspace(*cmd))
			break;
	}

	if (*cmd == '|')
		cmd++;

	for (; *cmd; cmd++)
	{
		if (! isspace(*cmd))
			break;
	}

	/*--------------------------------------------------------------------
	 * allocate struct
	 */
	p = (PRINTER *)MALLOC(sizeof(*p));
	if (p == 0)
	{
		return (0);
	}

	/*--------------------------------------------------------------------
	 * open the pipe
	 */
	rc = xvt_pipe_open(&pipe, cmd, "w");
	if (rc)
	{
		FREE(p);
		return (0);
	}

	/*--------------------------------------------------------------------
	 * store stuff
	 */
	p->prt_fp	= pipe.fp;
	p->prt_pid	= pipe.pid;
	p->prt_rc	= -1;
	strcpy(p->prt_str, cmd);

	return (p);
}

/*------------------------------------------------------------------------
 * xvt_prt_open_file() - open a printer to a file
 */
PRINTER * xvt_prt_open_file (const char *filename)
{
	PRINTER *	p;
	FILE *		fp;
	const char *mode = "w";

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (filename == 0 || *filename == 0)
	{
		return (0);
	}

	/*--------------------------------------------------------------------
	 * skip over any leading ">" symbols
	 */
	for (; *filename; filename++)
	{
		if (! isspace(*filename))
			break;
	}

	if (*filename == '>')
	{
		filename++;
		if (*filename == '>')
		{
			filename++;
			mode = "a";
		}
	}

	for (; *filename; filename++)
	{
		if (! isspace(*filename))
			break;
	}

	/*--------------------------------------------------------------------
	 * allocate struct
	 */
	p = (PRINTER *)MALLOC(sizeof(*p));
	if (p == 0)
	{
		return (0);
	}

	/*--------------------------------------------------------------------
	 * open the file
	 */
	fp = fopen(filename, mode);
	if (fp == 0)
	{
		FREE(p);
		return (0);
	}

	/*--------------------------------------------------------------------
	 * store stuff
	 */
	p->prt_fp	= fp;
	p->prt_pid	= 0;
	p->prt_rc	= 0;
	strcpy(p->prt_str, filename);

	return (p);
}

/*------------------------------------------------------------------------
 * xvt_prt_close() - close a printer
 */
int xvt_prt_close (PRINTER *p)
{
	int		rc;

	if (p == 0)
	{
		return (0);
	}

	if (p->prt_pid != 0)
	{
		PIPE	pipe;

		pipe.fp  = p->prt_fp;
		pipe.pid = p->prt_pid;
		rc = xvt_pipe_close(&pipe);
		if (rc == -2)
			rc = p->prt_rc;
	}
	else
	{
		if (p->prt_fp != 0)
			fclose(p->prt_fp);
		rc = 0;
	}

	FREE(p);

	return (rc);
}

/*------------------------------------------------------------------------
 * xvt_prt_output_chr() - output a char to a printer
 */
int xvt_prt_output_chr (PRINTER *p, int ch)
{
	if (p == 0)
		return (-1);

	if (p->prt_fp != 0)
		fputc(ch, p->prt_fp);

	return (0);
}

/*------------------------------------------------------------------------
 * xvt_prt_output_str() - output a string to a printer
 */
int xvt_prt_output_str (PRINTER *p, const char *str)
{
	if (p == 0 || str == 0)
		return (-1);

	if (p->prt_fp != 0)
		fputs(str, p->prt_fp);

	return (0);
}

/*------------------------------------------------------------------------
 * xvt_prt_output_fmt() - output a formatted string to a printer
 */
int xvt_prt_output_fmt (PRINTER *p, const char *fmt, ...)
{
	va_list args;
	int		rc;

	if (p == 0 || fmt == 0)
		return (-1);

	va_start(args, fmt);
	rc = xvt_prt_output_var(p, fmt, args);
	va_end(args);

	return (rc);
}

/*------------------------------------------------------------------------
 * xvt_prt_output_var() - output a variadic string to a printer
 */
int xvt_prt_output_var (PRINTER *p, const char *fmt, va_list args)
{
	if (p == 0 || fmt == 0)
		return (-1);

	if (p->prt_fp != 0)
		vfprintf(p->prt_fp, fmt, args);

	return (0);
}

/*------------------------------------------------------------------------
 * flush output to the printer
 */
int xvt_prt_flush (PRINTER *p)
{
	if (p == 0)
		return (-1);

	if (p->prt_fp != 0)
		fflush(p->prt_fp);

	return (0);
}
