/*------------------------------------------------------------------------
 * FTP cmd routine
 */
#include "ftpcommon.h"

/*------------------------------------------------------------------------
 * ftp_cmd() - process an FTP cmd
 */
int ftp_cmd (FTP_NODE *ftp_ptr, const char *fmt, ...)
{
	int retn;
	va_list ap;
	char buf[BUFSIZ];
	int len;

	va_start(ap, fmt);
	len = vsprintf(buf, fmt, ap);
	va_end(ap);

	if (ftp_ptr->debug || ftp_ptr->ftp_logmsg)
	{
		char msgbuf[BUFSIZ];
		int i;

		strcpy(msgbuf, "---> ");
		strcat(msgbuf, buf);
		if (strncmp("PASS ", buf, 5) == 0)
		{
			for (i=10; msgbuf[i]; i++)
				msgbuf[i] = '*';
		}

		if (ftp_ptr->debug)
		{
			fprintf(ftp_ptr->debug, "%s\n", msgbuf);
			fflush(ftp_ptr->debug);
		}

		if (ftp_ptr->ftp_logmsg)
			(*ftp_ptr->ftp_logmsg)(msgbuf);
	}

	buf[len++] = '\r';
	buf[len++] = '\n';
	send(ftp_ptr->cout, buf, len, 0);

	ftp_ptr->cpend = TRUE;
	ftp_ptr->mark_idle_time = time((time_t *)NULL);

	retn = ftp_getreply(ftp_ptr, ! strncmp(buf, "QUIT", 4));

	return (retn);
}
