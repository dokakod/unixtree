/*------------------------------------------------------------------------
 * get hostname
 */
#include "oscommon.h"

char * os_get_hostname (char *buf)
{
	int buf_size = 64;

	*buf = 0;

#if V_WINDOWS
	{
		DWORD dwSize = buf_size;

		GetComputerName(buf, &dwSize);
	}
#else
	{
		gethostname(buf, buf_size);
	}
#endif

	return (buf);
}
