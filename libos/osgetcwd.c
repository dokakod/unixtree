/*------------------------------------------------------------------------
 * get current working directory
 */
#include "oscommon.h"

char * os_get_cwd (char *path)
{
	*path = 0;
	getcwd(path, MAX_PATHLEN-1);

	return (path);
}
