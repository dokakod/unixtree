/*------------------------------------------------------------------------
 *	Routines to read the mount table & get free-space for a file-system
 *
 *	platform: posix
 */
#include "oscommon.h"

/*------------------------------------------------------------------------
 * os_mnt_open() - open the mount table
 */
void * os_mnt_open (void)
{
	return (0);
}

/*------------------------------------------------------------------------
 * os_mnt_close() - close the mount table
 */
void os_mnt_close (void *md)
{
}

/*------------------------------------------------------------------------
 * os_mnt_read() - read the next entry from the mount table
 */
int os_mnt_read (void *md, MOUNT_INFO *mptr)
{
	return (-1);
}

/*------------------------------------------------------------------------
 * os_mnt_space() - get free space for a file-system a path is in
 */
long os_mnt_space (const char *path)
{
	return (-1);
}
