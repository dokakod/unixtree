/*------------------------------------------------------------------------
 * set a file time
 */
#include "oscommon.h"

int os_set_file_time (const char *path, time_t actime, time_t modtime)
{
	struct utimbuf timebuf;

	memset(&timebuf, 0, sizeof(timebuf));
	timebuf.actime  = actime;
	timebuf.modtime = modtime;

	return (os_utime(path, &timebuf));
}
