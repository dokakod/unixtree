/*------------------------------------------------------------------------
 * set file sync mode
 */
#include "oscommon.h"

int os_set_sync_mode (int fd)
{
	int rc = 0;

#ifdef O_SYNC
	int oflag = fcntl(fd, F_GETFL, 0);
	if (oflag != -1)
	{
		oflag |= O_SYNC;
		rc = fcntl(fd, F_SETFL, oflag);
	}
#endif

	return (rc);
}
