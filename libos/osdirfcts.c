/*------------------------------------------------------------------------
 * directory functions
 */
#include "oscommon.h"

int os_dir_make (const char *pathname)
{
	struct stat stbuf;

	if (os_stat(pathname, &stbuf) == 0)
	{
		if (S_ISDIR(stbuf.st_mode))
			return (1);
		else
		{
			errno = EEXIST;
			return (-1);
		}
	}

#if V_UNIX
	if (mkdir(pathname, 0777))
#else
	if (mkdir(pathname))
#endif
	{
		return (-1);
	}

	return (0);
}

int os_dir_delete (const char *pathname)
{
	struct stat stbuf;

	if (os_stat(pathname, &stbuf))
	{
		if (errno == ENOENT)
			return (0);
		else
		{
			return (-1);
		}
	}

	if (rmdir(pathname))
	{
		return (-1);
	}

	return (0);
}

int os_dir_rename (const char *oldpath, const char *newpath)
{
	struct stat stbuf;

	if (os_stat(newpath, &stbuf) == 0)
	{
		errno = EEXIST;
		return (-1);
	}

	if (rename(oldpath, newpath))
	{
		return (-1);
	}

	return (0);
}
