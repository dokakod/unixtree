/*------------------------------------------------------------------------
 * FTP login routine
 */
#include "ftpcommon.h"

/*------------------------------------------------------------------------
 * ftp_login() - login to a remote server
 */
int ftp_login (FTP_NODE *ftp_ptr)
{
	char *username = ftp_ptr->user;
	char *password = ftp_ptr->password;
	int retn;

	if (username == 0 || *username == 0)
	{
		return (-1);
	}

	if (password == 0)
	{
		return (-1);
	}

	retn = ftp_cmd(ftp_ptr, "USER %s", username);
	if (retn == -2)
	{
		return (-2);
	}

	if (retn == CONTINUE)
	{
		retn = ftp_cmd(ftp_ptr, "PASS %s", password);
		if (retn == -2)
		{
			return (-2);
		}
		if (retn == CONTINUE)
		{
			return (-1);
		}
	}
	if (retn != COMPLETE)
	{
		return (-1);
	}

	if (ftp_set_idle(ftp_ptr, time(0)) == -2)
	{
		return (-2);
	}

	return (0);
}
