/*------------------------------------------------------------------------
 * eject a floppy disk
 */
#include "oscommon.h"

#if V_UNIX
#  if SUN
#    include <sys/dkio.h>
#  endif
#endif

int os_floppy_eject (int fd)
{
	int rc = 0;

#ifdef DKIOCEJECT
	rc = ioctl(fd, DKIOCEJECT, 0);
#endif

	return (rc);
}
