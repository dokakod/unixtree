/*------------------------------------------------------------------------
 * FTP open/close routines
 */
#include "ftpcommon.h"

/*------------------------------------------------------------------------
 * ftp_open() - open an FTP connection
 */
int ftp_open (FTP_NODE *ftp_ptr, int timeout, int debug)
{
	char dbg_file[MAX_PATHLEN];

	if (debug)
	{
		strcpy(dbg_file, "ftp-");
		strcat(dbg_file, ftp_ptr->hostname);
		strcat(dbg_file, ".dbg");
		ftp_ptr->debug = fopen(dbg_file, "w");
	}

	if (ftp_setpeer(ftp_ptr, timeout) != 0)
	{
		/* login failed */
		return (-1);
	}

	if (! ftp_ptr->server_type)
	{
		ftp_ptr->server_type = ftp_detect_server_type(ftp_ptr);
		if (ftp_ptr->server_type <= 0)
		{
			/* couldn't determine remote site type */
			ftp_ptr->ftp_errmsg("ftp_open: detect_server_type failed");
			return (-1);
		}
	}

	if (ftp_set_transfer_mode(ftp_ptr) == -2)
	{
		/* couldn't set up transfer mode ... lost connection */
		ftp_ptr->ftp_errmsg("ftp_open: ftp_set_transfer_mode failed");
		return (-1);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * ftp_close() - close an FTP connection & free all associated memory
 */
int ftp_close (FTP_NODE *ftp_ptr)
{
	int retn = 0;

	if (ftp_ptr != (FTP_NODE *)NULL)
	{
		if (ftp_ptr->connected)
		{
			retn = ftp_cmd(ftp_ptr, "QUIT");
			ftp_ptr->connected = FALSE;
		}

		if (ftp_ptr->debug != (FILE *)NULL)
		{
			fclose(ftp_ptr->debug);
			ftp_ptr->debug = (FILE *)NULL;
		}

		if (ftp_ptr->cin != 0)
		{
			closesocket(ftp_ptr->cin);
			ftp_ptr->cin = 0;
		}

		if (ftp_ptr->cout != 0)
		{
			closesocket(ftp_ptr->cout);
			ftp_ptr->cout = 0;
		}

		ftp_ptr->owners   = BSCRAP(ftp_ptr->owners,   TRUE);
		ftp_ptr->groups   = BSCRAP(ftp_ptr->groups,   TRUE);
		ftp_ptr->symfiles = BSCRAP(ftp_ptr->symfiles, TRUE);
		ftp_ptr->symdirs  = BSCRAP(ftp_ptr->symdirs,  TRUE);
	}

	return (retn);
}
