/*------------------------------------------------------------------------
 * FTP connection routines
 */
#include "ftpcommon.h"
#include <setjmp.h>

/*------------------------------------------------------------------------
 * timeout processing
 */
static void ftp_timeout (time_t t, void *data)
{
	jmp_buf *j	= (jmp_buf *)data;

	longjmp(*j, 1);
}

/*------------------------------------------------------------------------
 * ftp_i_recv() - receive a stream, restarting if necessary
 */
static int ftp_i_recv (int s, void *buf, int len)
{
	int rc;

	while (TRUE)
	{
		rc = recv(s, buf, len, 0);
		if (rc == -1)
		{
			if (errno == EINTR)
				continue;
		}

		break;
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * ftp_i_send() - send a stream, restarting if necessary
 */
static int ftp_i_send (int s, void *buf, int len, int flags)
{
	int rc;

	while (TRUE)
	{
		rc = send(s, buf, len, flags);
		if (rc == -1)
		{
			if (errno == EINTR)
				continue;
		}

		break;
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * ftp_empty() - wait for a TCP/IP input event
 */
static int ftp_empty (fd_set *mask, int sec)
{
	struct timeval t;

	t.tv_sec  = sec;
	t.tv_usec = 0;
	return (select(32, mask, 0, 0, &t));
}

/*------------------------------------------------------------------------
 * ftp_lostpeer() - process a lost connection
 */
static
int ftp_lostpeer (FTP_NODE *ftp_ptr)
{
	if (ftp_ptr->connected)
	{
		if (ftp_ptr->cout > 0)
		{
			shutdown(ftp_ptr->cout, 1+1);
			closesocket(ftp_ptr->cout);
			ftp_ptr->cout = 0;
		}

		if (ftp_ptr->data >= 0)
		{
			shutdown(ftp_ptr->data, 1+1);
			closesocket(ftp_ptr->data);
			ftp_ptr->data = 0;
		}
		ftp_ptr->connected = FALSE;
	}

	return (0);
}

/*------------------------------------------------------------------------
 * ftp_hookup() - open a connection to a remote server
 */
static int ftp_hookup (FTP_NODE *ftp_ptr, int timeout)
{
	struct hostent *host_ent = (struct hostent *)NULL;
	int s;
	int len;
	int rc;
	FTP_SVC *ftp_svc = (FTP_SVC *)ftp_ptr->services;
	jmp_buf jmpbuf;

#if USE_INADDR
	struct in_addr addr;
#define ADDR	addr.s_addr
#else
	u_long addr;
#define ADDR	addr
#endif

	/*--------------------------------------------------------------------
	 * clear the remote address
	 */
	memset(&ftp_svc->rmt_addr, 0, sizeof (ftp_svc->rmt_addr));

	/*--------------------------------------------------------------------
	 * resolve the remote hostname
	 */
	addr = inet_addr(ftp_ptr->hostname);
	if (ADDR != (u_long)-1)
	{
		ftp_svc->rmt_addr.sin_family = AF_INET;
		memcpy(&ftp_svc->rmt_addr.sin_addr.s_addr, &addr, sizeof(addr));
	}
	else
	{
		host_ent = gethostbyname(ftp_ptr->hostname);
		if (host_ent == (struct hostent *)NULL)
		{
			ftp_ptr->ftp_errmsg("hookup: gethostbyname");
			ftp_ptr->code = -1;
			return (-1);
		}

		ftp_svc->rmt_addr.sin_family = host_ent->h_addrtype;
		memcpy(&ftp_svc->rmt_addr.sin_addr,
			host_ent->h_addr_list[0], host_ent->h_length);
	}

	/*--------------------------------------------------------------------
	 * store the port to talk to
	 */
	ftp_svc->rmt_addr.sin_port = ftp_svc->sp.s_port;

	/*--------------------------------------------------------------------
	 * create a socket & connect to remote machine
	 *
	 * Note that the connect call will block forever if the socket
	 * is blocking (which ours is), so we wrap a timer around this
	 * call.
	 */
	if (setjmp(jmpbuf) == 0)
	{
		win_alarm_set(WIN_ALARM_USER, timeout * 1000, ftp_timeout, &jmpbuf,
			FALSE, FALSE, TRUE);

		while (TRUE)
		{
			/*------------------------------------------------------------
			 * create the socket
			 */
			s = socket(ftp_svc->rmt_addr.sin_family, SOCK_STREAM, 0);
			if (s < 0)
			{
				win_alarm_clr(WIN_ALARM_USER);
				ftp_ptr->ftp_errmsg("hookup: socket");
				ftp_ptr->code = -1;
				return (-1);
			}

			/*------------------------------------------------------------
			 * now try to connect it
			 */
			rc = connect(s, (struct sockaddr *)&ftp_svc->rmt_addr,
				sizeof(ftp_svc->rmt_addr));
			if (rc == 0)
			{
				win_alarm_clr(WIN_ALARM_USER);
				break;
			}

			/*------------------------------------------------------------
			 * if we were interrupted, close the socket & try again
			 */
			if (errno == EINTR)
			{
				closesocket(s);
				s = 0;
				continue;
			}

			/*------------------------------------------------------------
			 * got some other error - just bail
			 */
			win_alarm_clr(WIN_ALARM_USER);
			ftp_ptr->ftp_errmsg("hookup: connect");
			ftp_ptr->code = -1;
			goto bad;
		}
	}
	else
	{
		win_alarm_clr(WIN_ALARM_USER);
		ftp_ptr->ftp_errmsg("hookup: timeout");
		ftp_ptr->code = -1;
		goto bad;
	}

	/*--------------------------------------------------------------------
	 * check if we have a socket
	 */
	if (s <= 0)
	{
		ftp_ptr->ftp_errmsg("hookup: open");
		ftp_ptr->code = -1;
		goto bad;
	}

	/*--------------------------------------------------------------------
	 * get local address
	 */
	len = sizeof(ftp_svc->lcl_addr);
	if (getsockname(s, (struct sockaddr *)&ftp_svc->lcl_addr, &len) < 0)
	{
		ftp_ptr->ftp_errmsg("hookup: getsockname");
		ftp_ptr->code = -1;
		goto bad;
	}

	/*--------------------------------------------------------------------
	 * cache socket in struct
	 */
	ftp_ptr->cin  = s;
	ftp_ptr->cout = s;

	/*--------------------------------------------------------------------
	 * now read in initial message from remote server
	 */
	if (ftp_getreply(ftp_ptr, 0) != 2)
	{
		if (ftp_ptr->cin > 0)
		{
			closesocket(ftp_ptr->cin);
			ftp_ptr->cin  = 0;
			ftp_ptr->cout = 0;
		}
		ftp_ptr->ftp_errmsg("hookup: getreply failed");
		ftp_ptr->code = -1;
		goto bad;
	}

	/*--------------------------------------------------------------------
	 * enable receiving out-of-band data
	 */
#ifdef SO_OOBINLINE
	{
		int on = 1;

		setsockopt(s, SOL_SOCKET, SO_OOBINLINE, (char *)&on, sizeof(on));
	}
#endif /* SO_OOBINLINE */

	return (0);

bad:
	if (s > 0)
		closesocket(s);
	return (-1);
}

/*------------------------------------------------------------------------
 * ftp_abort_remote() - abort a connection to a remote server
 */
static int ftp_abort_remote (FTP_NODE *ftp_ptr, int din)
{
	char buf[BUFSIZ];
	int nfnd;
	fd_set mask;
	int retn;
	int len;

	/*
	 * send IAC in urgent mode instead of DM because 4.3BSD places oob mark
	 * after urgent byte rather than before as is protocol now
	 */
	len = sprintf(buf, "%c%c%c", IAC, IP, IAC);
	retn = ftp_i_send(ftp_ptr->cout, buf, len, MSG_OOB);
	if (retn != 3)
	{
		ftp_ptr->ftp_errmsg("abort");
	}

	len = sprintf(buf, "%cABOR\r\n", DM);
	ftp_i_send(ftp_ptr->cout, buf, len, 0);

	FD_ZERO(&mask);
	FD_SET(ftp_ptr->cin, &mask);
	if (din > 0)
	{
		FD_SET(din, &mask);
	}
	if ((nfnd = ftp_empty(&mask, 10)) <= 0)
	{
		if (nfnd < 0)
		{
			ftp_ptr->ftp_errmsg("abort");
		}
		ftp_lostpeer(ftp_ptr);
		return (-2);
	}
	if (din > 0 && FD_ISSET(din, &mask))
	{
		while (ftp_i_recv(din, buf, BUFSIZ) > 0)
			;
	}
	if ((retn = ftp_getreply(ftp_ptr, 0)) == ERROR && ftp_ptr->code == 552)
	{
		/* 552 needed for nic style abort */
		retn = ftp_getreply(ftp_ptr, 0);
	}

	if (retn != -2)
	{
		retn = ftp_getreply(ftp_ptr, 0);
		if (retn != -2)
			retn = 0;
	}

	return (retn);
}

/*------------------------------------------------------------------------
 * ftp_getreply() - get a reply to a command sent to a remote server
 */
int ftp_getreply (FTP_NODE *ftp_ptr, int expecteof)
{
	int c;
	int n;
	int dig;
	char *cp;
	int originalcode = 0;
	int continuation = 0;
	int pflag = 0;
	char *pt = ftp_ptr->pasv;
	char msgbuf[512];
	char *pmsg;

	for (;;)
	{
		dig = n = ftp_ptr->code = 0;
		cp = ftp_ptr->reply_string;
		pmsg = msgbuf;

		if (ftp_ptr->debug || ftp_ptr->ftp_logmsg)
			pmsg += sprintf(pmsg, "<--- ");

		while (TRUE)
		{
			if (ftp_ptr->ibuf_ptr >= ftp_ptr->ibuf_len)
			{
				ftp_ptr->ibuf_len = ftp_i_recv(ftp_ptr->cin, ftp_ptr->ibuf,
					sizeof(ftp_ptr->ibuf));
				ftp_ptr->ibuf_ptr = 0;
			}

			if (ftp_ptr->ibuf_len == 0)
				c = EOF;
			else
				c = ftp_ptr->ibuf[ftp_ptr->ibuf_ptr++];

			if (c == '\n')
				break;

			if (c == IAC)
			{
				char obuf[128];
				int olen;

				/* handle telnet commands */
				c = ftp_ptr->ibuf[ftp_ptr->ibuf_ptr++];
				switch (c)
				{
					case WILL:
					case WONT:
						c = ftp_ptr->ibuf[ftp_ptr->ibuf_ptr++];
						olen = sprintf(obuf, "%c%c%c", IAC, DONT, c);
						ftp_i_send(ftp_ptr->cout, obuf, olen, 0);
						break;

					case DO:
					case DONT:
						c = ftp_ptr->ibuf[ftp_ptr->ibuf_ptr++];
						olen = sprintf(obuf, "%c%c%c", IAC, WONT, c);
						ftp_i_send(ftp_ptr->cout, obuf, olen, 0);
						break;

					default:
						break;
				}
				continue;
			}
			dig++;

			if (c == EOF)
			{
				if (expecteof)
				{
					/* 221 = service closing control connection ... logout */
					ftp_ptr->code = 221;
					return (0);
				}
				ftp_lostpeer(ftp_ptr);
				/*
					421 = service not available, service closing control
					connection ... logout
				*/

				ftp_ptr->ftp_errmsg("421 Service not available");

				ftp_ptr->code = 421;
				return (-2);
			}

			if (ftp_ptr->debug || ftp_ptr->ftp_logmsg)
			{
				if (c != '\r')
					*pmsg++ = c;
			}

			if (dig < 4 && isdigit(c))
				ftp_ptr->code = ftp_ptr->code * 10 + (c - '0');
			/* 227 = entering passive mode */
			if (!pflag && ftp_ptr->code == 227)
				pflag = 1;
			if (dig > 4 && pflag == 1 && isdigit(c))
				pflag = 2;
			if (pflag == 2)
			{
				if (c != '\r' && c != ')')
					*pt++ = c;
				else
				{
					*pt = '\0';
					pflag = 3;
				}
			}
			if (dig == 4 && c == '-')
			{
				if (continuation)
					ftp_ptr->code = 0;
				continuation++;
			}
			if (n == 0)
				n = c;
			if (cp < &ftp_ptr->reply_string[sizeof(ftp_ptr->reply_string) - 1])
			{
				if (c != '\r')
					*cp++ = c;
			}
		}
		*cp = '\0';

		if (ftp_ptr->debug || ftp_ptr->ftp_logmsg)
		{
			*pmsg = 0;

			if (ftp_ptr->debug)
			{
				fprintf(ftp_ptr->debug, "%s\n", msgbuf);
				fflush(ftp_ptr->debug);
			}

			if (ftp_ptr->ftp_logmsg)
				(*ftp_ptr->ftp_logmsg)(msgbuf);
		}

		if (continuation && ftp_ptr->code != originalcode)
		{
			if (originalcode == 0)
				originalcode = ftp_ptr->code;
			continue;
		}

		if (n != '1')
			ftp_ptr->cpend = 0;

		/*
			421 = service not available, service closing control
			connection ... logout
		*/
		if (ftp_ptr->code == 421 || originalcode == 421)
		{
			ftp_lostpeer(ftp_ptr);
			return (-2);
		}
		return (n - '0');
	}
}

/*------------------------------------------------------------------------
 * ftp_initconn() - start a listen on the data channel
 */
int ftp_initconn (FTP_NODE *ftp_ptr)
{
	unsigned char *p, *a;
	int retn;
	int len;
	int tmpno = 0;
	int on = 1;
	FTP_SVC *ftp_svc = (FTP_SVC *)ftp_ptr->services;

noport:
	ftp_svc->data_addr = ftp_svc->lcl_addr;
	if (ftp_ptr->sendport)
	{
		/* let system pick one */
		ftp_svc->data_addr.sin_port = 0;
	}

	if (ftp_ptr->data > 0)
	{
		closesocket(ftp_ptr->data);
	}

	if ((ftp_ptr->data = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		ftp_ptr->ftp_errmsg("ftp: socket");
		if (tmpno)
			ftp_ptr->sendport = 1;
		return (1);
	}

	if (!ftp_ptr->sendport)
	{
		if (setsockopt(ftp_ptr->data, SOL_SOCKET, SO_REUSEADDR,
			(char *)&on, sizeof (on)) < 0)
		{
			ftp_ptr->ftp_errmsg("ftp: setsockopt (reuse address)");
			goto bad;
		}
	}

	if (bind(ftp_ptr->data, (struct sockaddr *)&ftp_svc->data_addr,
		sizeof (ftp_svc->data_addr)) < 0)
	{
		ftp_ptr->ftp_errmsg("ftp: bind");
		goto bad;
	}

	len = sizeof (ftp_svc->data_addr);
	if (getsockname(ftp_ptr->data, (struct sockaddr *)&ftp_svc->data_addr,
		&len) < 0)
	{
		ftp_ptr->ftp_errmsg("ftp: getsockname");
		goto bad;
	}

	if (listen(ftp_ptr->data, 1) < 0)
		ftp_ptr->ftp_errmsg("ftp: listen");

	if (ftp_ptr->sendport)
	{
		a = (unsigned char *)&ftp_svc->data_addr.sin_addr;
		p = (unsigned char *)&ftp_svc->data_addr.sin_port;
		retn = ftp_cmd(ftp_ptr, "PORT %d,%d,%d,%d,%d,%d",
			a[0], a[1], a[2], a[3], p[0], p[1]);
		if (retn == ERROR && ftp_ptr->sendport == -1)
		{
			ftp_ptr->sendport = 0;
			tmpno = 1;
			goto noport;
		}
		if (retn == -2)
		{
			return (retn);
		}
		return (retn != COMPLETE);
	}

	if (tmpno)
		ftp_ptr->sendport = 1;

#if defined(IP_TOS) && defined(IPTOS_THROUGHPUT)
	on = IPTOS_THROUGHPUT;
	if (setsockopt(ftp_ptr->data, IPPROTO_IP, IP_TOS,
		(char *)&on, sizeof(int)) < 0)
	{
		ftp_ptr->ftp_errmsg("ftp: setsockopt TOS (ignored)");
	}
#endif
	return (0);

bad:
	closesocket(ftp_ptr->data);
	ftp_ptr->data = 0;
	if (tmpno)
		ftp_ptr->sendport = 1;
	return (1);
}

/*------------------------------------------------------------------------
 * ftp_dataconn() - open a data connection from a remote server
 */
int ftp_dataconn (FTP_NODE *ftp_ptr)
{
	struct sockaddr_in from;
	int fromlen = sizeof (from);
	int s;

	s = accept(ftp_ptr->data, (struct sockaddr *) &from, &fromlen);
	if (s < 0)
	{
		ftp_ptr->ftp_errmsg("ftp: accept");
		closesocket(ftp_ptr->data);
		ftp_ptr->data = 0;
		return (0);
	}

	closesocket(ftp_ptr->data);
	ftp_ptr->data = s;

#if defined(IP_TOS) && defined(IPTOS_THROUGHPUT)
	{
		int tos = IPTOS_THROUGHPUT;

		if (setsockopt(s, IPPROTO_IP, IP_TOS, (char *)&tos, sizeof(int)) < 0)
		{
			ftp_ptr->ftp_errmsg("ftp: setsockopt TOS (ignored)");
		}
	}
#endif

	return (s);
}

/*------------------------------------------------------------------------
 * ftp_recvrequest() - receive a response to a cmd sent to a remote server
 */
int ftp_recvrequest (FTP_NODE *ftp_ptr, const char *cmd,
	const char *local, const char *remote)
{
	FILE *fout = (FILE *)NULL;
	int din = 0;
	int retn;
	char buf[BUFSIZ];
	char obuf[BUFSIZ * 2];
	int c;
	int d;

	retn = ftp_initconn(ftp_ptr);
	if (retn)
	{
		ftp_ptr->code = -1;
		return ((retn == -2) ? -2 : -1);
	}

	if (ftp_ptr->server_type == MSDOS_SERVER
		&& strcmp(cmd, "LIST") == 0)
	{
		retn = ftp_cmd(ftp_ptr, "%s", cmd);
		if (retn != PRELIM)
		{
			return ((retn == -2) ? -2 : -1);
		}
	}
	else
	{
		retn = ftp_cmd(ftp_ptr, "%s %s", cmd, remote);
		if (retn != PRELIM)
		{
			return ((retn == -2) ? -2 : -1);
		}
	}

	din = ftp_dataconn(ftp_ptr);
	if (din == 0)
	{
		goto abort;
	}

	fout = fopen(local, "wb");
	if (fout == (FILE *)NULL)
	{
		goto abort;
	}

	switch (ftp_ptr->curtype)
	{
		case TYPE_I:
		case TYPE_L:
			errno = d = 0;
			while ((c = ftp_i_recv(din, buf, sizeof(buf))) > 0)
			{
				if ((d = fwrite(buf, 1, c, fout)) != c)
					break;
			}
			break;

		case TYPE_A:
			errno = d = 0;
			while ((c = ftp_i_recv(din, buf, sizeof(buf))) > 0)
			{
				int i;

				d = 0;
				for (i=0; i<c; i++)
				{
					if (buf[i] != '\r')
						obuf[d++] = buf[i];
				}
				c = d;

				if ((d = fwrite(obuf, 1, c, fout)) != c)
					break;
			}
			break;
	}
	fclose(fout);
	closesocket(din);
	if ((retn = ftp_getreply(ftp_ptr, 0)) != -2)
	{
		retn = 0;
	}
	return (retn);

abort: /* abort using RFC959 recommended IP, SYNC sequence  */
	if (!ftp_ptr->cpend)
	{
		ftp_ptr->code = -1;
		return (-1);
	}

	retn = ftp_abort_remote(ftp_ptr, din);
	if (retn != -2)
	{
		retn = -2;
	}
	ftp_ptr->code = -1;

	if (ftp_ptr->data != 0)
	{
		closesocket(ftp_ptr->data);
		ftp_ptr->data = 0;
	}

	if (fout != (FILE *)NULL)
		fclose(fout);

	if (din != 0)
		closesocket(din);

	return (retn);
}

/*------------------------------------------------------------------------
 * ftp_setpeer() - connect to a peer server & login if possible
 */
int ftp_setpeer (FTP_NODE *ftp_ptr, int timeout)
{
	if (ftp_hookup(ftp_ptr, timeout) == -1)
		return (-1);
	ftp_ptr->connected = TRUE;

	ftp_ptr->curtype = TYPE_A;

	if (ftp_login(ftp_ptr) < 0)
	{
		ftp_lostpeer(ftp_ptr);
		return (-1);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * ftp_detect_server_type() - determine type of O/S is on a remote server
 */
int ftp_detect_server_type (FTP_NODE *ftp_ptr)
{
	int retn;

	retn = ftp_cmd(ftp_ptr, "SYST");

	if (retn == COMPLETE)
	{
		if      (strstr(ftp_ptr->reply_string, "UNIX")  != (char *)NULL)
		{
			return (UNIX_SERVER);
		}
		else if (strstr(ftp_ptr->reply_string, "DOS") != (char *)NULL)
		{
			return (MSDOS_SERVER);
		}
		else if (strstr(ftp_ptr->reply_string, "VMS")   != (char *)NULL)
		{
			return (VMS_SERVER);
		}
	}
	return (0);
}
