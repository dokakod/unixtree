/*------------------------------------------------------------------------
 * low-level FTP utility cmds
 */
#include "ftpcommon.h"

/*------------------------------------------------------------------------
 * idle time to set if remote system cannot tell us what its idle time is
 */
#define DEFAULT_IDLE_TIME   120		/* minutes */

/*------------------------------------------------------------------------
 * ftp_get_idle() - get idle time setting on remote server
 */
int ftp_get_idle (FTP_NODE *ftp_ptr)
{
	int idle_time;
	int retn;

	retn = ftp_cmd(ftp_ptr, "SITE IDLE");

	if (retn == COMPLETE)
	{
		sscanf(ftp_ptr->reply_string,
			"200 Current IDLE time limit is %*d seconds; max %d", &idle_time);
		if (!idle_time)
		{
			sscanf(ftp_ptr->reply_string,
				"200 Current IDLE time limit is %d", &idle_time);
			if (!idle_time)
			{
				idle_time = DEFAULT_IDLE_TIME;
			}
		}

		retn = ftp_cmd(ftp_ptr, "SITE IDLE %d", idle_time);
		if (retn == COMPLETE)
		{
			return (idle_time);
		}
	}
	if (retn == -2)
	{
		return (0);
	}
	return (DEFAULT_IDLE_TIME);
}

/*------------------------------------------------------------------------
 * ftp_set_idle() - set idle time on remote server
 */
int ftp_set_idle (FTP_NODE *ftp_ptr, time_t current_time)
{
	int retn;

	if (!ftp_ptr->idle_time)
	{
		ftp_ptr->idle_time = ftp_get_idle(ftp_ptr);
		if (ftp_ptr->idle_time == 0)
		{
			return (-2);
		}
		else
		{
			retn = ftp_cmd(ftp_ptr, "SITE IDLE %d", ftp_ptr->idle_time);
			if (retn == -2)
			{
				return (-2);
			}
			if (ftp_ptr->idle_time > DEFAULT_IDLE_TIME)
			{
				ftp_ptr->idle_time -= (DEFAULT_IDLE_TIME / 2);
			}
			else
			{
				ftp_ptr->idle_time /= 2;
			}
		}
		ftp_ptr->mark_idle_time = time((time_t *)NULL);
	}

	ftp_chk_idle(ftp_ptr, current_time);

	return (0);
}

/*------------------------------------------------------------------------
 * ftp_chk_idle() - check if idle time is up & send a NO-OP if so
 */
int ftp_chk_idle (FTP_NODE *ftp_ptr, time_t current_time)
{
	int retn;

	if (ftp_ptr->idle_time)
	{
		if (current_time > ftp_ptr->mark_idle_time + ftp_ptr->idle_time)
		{
			retn = ftp_cmd(ftp_ptr, "NOOP");
			if (retn == -2)
			{
				return (-2);
			}
		}
	}
	return (0);
}

/*------------------------------------------------------------------------
 * ftp_set_binary_transfer_mode() - set transfer mode to binary
 */
static int ftp_set_binary_transfer_mode (FTP_NODE *ftp_ptr)
{
	int retn;

	retn = ftp_cmd(ftp_ptr, "TYPE I");

	if (retn == COMPLETE)
	{
		ftp_ptr->curtype = TYPE_I;
		return (0);
	}
	return ((retn == -2) ? -2 : -1);
}

/*------------------------------------------------------------------------
 * ftp_set_ascii_transfer_mode() - set transfer mode to ascii
 */
static int ftp_set_ascii_transfer_mode (FTP_NODE *ftp_ptr)
{
	int retn;

	retn = ftp_cmd(ftp_ptr, "TYPE A");

	if (retn == COMPLETE)
	{
		ftp_ptr->curtype = TYPE_A;
		return (0);
	}
	return ((retn == -2) ? -2 : -1);
}

/*------------------------------------------------------------------------
 * ftp_set_tenex_transfer_mode() - set transfer mode to tenex
 */
static int ftp_set_tenex_transfer_mode (FTP_NODE *ftp_ptr)
{
	int retn;

	retn = ftp_cmd(ftp_ptr, "TYPE L 8");

	if (retn == COMPLETE)
	{
		ftp_ptr->curtype = TYPE_L;
		return (0);
	}
	return ((retn == -2) ? -2 : -1);
}

/*------------------------------------------------------------------------
 * ftp_set_transfer_mode() - set appropriate transfer mode
 */
int ftp_set_transfer_mode (FTP_NODE *ftp_ptr)
{
	switch (ftp_ptr->xfer_mode)
	{
		case ASCII_TRANSFER_MODE:
			return (ftp_set_ascii_transfer_mode(ftp_ptr));

		case TENEX_TRANSFER_MODE:
			return (ftp_set_tenex_transfer_mode(ftp_ptr));

		case BINARY_TRANSFER_MODE:
			return (ftp_set_binary_transfer_mode(ftp_ptr));
	}
	return (-1);
}

/*------------------------------------------------------------------------
 * ftp_set_umask() - set umask on remote server
 */
int ftp_set_umask (FTP_NODE *ftp_ptr, int cmask)
{
	int retn;

	retn = ftp_cmd(ftp_ptr, "SITE UMASK 0%o", (cmask & 0777));

	if (retn == COMPLETE)
	{
		return (0);
	}
	return ((retn == -2) ? -2 : -1);
}
