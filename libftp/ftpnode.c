/*------------------------------------------------------------------------
 * FTP_NODE routines
 */
#include "ftpcommon.h"

#if V_WINDOWS
/*------------------------------------------------------------------------
 * count of current FTP sessions active
 */
static int	ftp_count	= 0;
#endif

/*------------------------------------------------------------------------
 * ftp_tcp_start() - start a new session & start TCP if 1st active session
 */
static void ftp_tcp_start (void)
{
#if V_WINDOWS
	if (ftp_count++ == 0)
	{
		WORD	wVersionRequested	= MAKEWORD(1, 1);
		WSADATA	wsaData;

		WSAStartup(wVersionRequested, &wsaData);
	}
#endif
}

/*------------------------------------------------------------------------
 * ftp_tcp_end() - end a session & close TCP if no active sessions
 */
static void ftp_tcp_end (void)
{
#if V_WINDOWS
	if (--ftp_count == 0)
	{
		WSACleanup();
	}
#endif
}

/*------------------------------------------------------------------------
 * ftp_make_temp() - make a temp file
 */
static char *ftp_make_temp (char *path)
{
	return os_make_temp_name(path, NULL, NULL);
}

/*------------------------------------------------------------------------
 * ftp_make_node() - create an FTP_NODE
 */
FTP_NODE *ftp_make_node (const char *hostname, const char *username,
	const char *password,
	void (*ftp_logmsg)(const char *msg),
	int (*ftp_errmsg)(const char *msg))
{
	FTP_NODE *ftp_ptr;
	struct servent *sp;
	FTP_SVC *s;

	ftp_tcp_start();

	s = (FTP_SVC *)MALLOC(sizeof(FTP_SVC));
	if (s == (FTP_SVC *)NULL)
	{
		ftp_errmsg("ftp_open: MALLOC");
		return ((FTP_NODE *)NULL);
	}
	memset(s, 0, sizeof(*s));

	ftp_ptr = (FTP_NODE *)MALLOC(sizeof(FTP_NODE));
	if (ftp_ptr == (FTP_NODE *)NULL)
	{
		ftp_errmsg("ftp_open: MALLOC");
		FREE(s);
		return ((FTP_NODE *)NULL);
	}
	memset(ftp_ptr, 0, sizeof(*ftp_ptr));

	sp = getservbyname("ftp", "tcp");
	if (sp == (struct servent *)NULL)
	{
		ftp_errmsg("ftp_open: getservbyname");
		FREE(s);
		FREE(ftp_ptr);
		return ((FTP_NODE *)NULL);
	}

	memcpy(&s->sp, sp, sizeof(s->sp));

	ftp_ptr->services		= (void *)s;
	ftp_ptr->sendport		= -1;  /* not using ports */
	ftp_ptr->connected		= 0;
	ftp_ptr->data			= 0;
	ftp_ptr->crflag			= 1;	/* strip c/r on ascii gets */
	ftp_ptr->code			= 0;
	ftp_ptr->curtype		= TYPE_A;
	ftp_ptr->server_type	= 0;
	ftp_ptr->cpend			= 0;	/* no pending replies */
	ftp_ptr->debug			= 0;
	ftp_ptr->cin			= 0;
	ftp_ptr->cout			= 0;
	ftp_ptr->mark_idle_time	= 0;
	ftp_ptr->idle_time		= 0;
	ftp_ptr->xfer_mode		= BINARY_TRANSFER_MODE;
	ftp_ptr->ibuf_len		= 0;
	ftp_ptr->ibuf_ptr		= 0;
	strncpy(ftp_ptr->hostname, hostname, sizeof(ftp_ptr->hostname));
	strncpy(ftp_ptr->user,     username, sizeof(ftp_ptr->user));
	strncpy(ftp_ptr->password, password, sizeof(ftp_ptr->password));
	ftp_make_temp(ftp_ptr->tempname);
	ftp_ptr->owners			= 0;
	ftp_ptr->groups			= 0;
	ftp_ptr->symfiles		= 0;
	ftp_ptr->symdirs		= 0;
	ftp_ptr->ftp_logmsg		= ftp_logmsg;
	ftp_ptr->ftp_errmsg		= ftp_errmsg;

	return (ftp_ptr);
}

/*------------------------------------------------------------------------
 * ftp_free_node() - free an FTP_NODE
 */
int ftp_free_node (FTP_NODE *ftp_ptr)
{
	FTP_SVC *s;

	ftp_ptr->owners   = BSCRAP(ftp_ptr->owners,   TRUE);
	ftp_ptr->groups   = BSCRAP(ftp_ptr->groups,   TRUE);
	ftp_ptr->symfiles = BSCRAP(ftp_ptr->symfiles, TRUE);
	ftp_ptr->symdirs  = BSCRAP(ftp_ptr->symdirs,  TRUE);

	s = (FTP_SVC *)ftp_ptr->services;
	FREE(s);

	FREE(ftp_ptr);

	ftp_tcp_end();

	return (0);
}
