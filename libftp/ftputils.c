/*------------------------------------------------------------------------
 * FTP utility routines
 */
#include "ftpcommon.h"

/*------------------------------------------------------------------------
 * ftp_hostname() - get hostname associated with a connection
 */
char *ftp_hostname (FTP_NODE *ftp_ptr)
{
	return (ftp_ptr->hostname);
}

/*------------------------------------------------------------------------
 * ftp_hostaddr() - get IP address associated with a connection
 */
char *ftp_hostaddr (FTP_NODE *ftp_ptr)
{
	FTP_SVC *s = (FTP_SVC *)ftp_ptr->services;

	return (inet_ntoa(s->rmt_addr.sin_addr));
}

/*------------------------------------------------------------------------
 * ftp_server_type() - get server type associated with a connection
 */
int ftp_server_type (FTP_NODE *ftp_ptr)
{
	return (ftp_ptr->server_type);
}
