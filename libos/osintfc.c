/*------------------------------------------------------------------------
 *	These routines are interface routines for system calls
 *	which can be interrupted by a signal and have to
 *	be restarted.
 *
 *	int os_access (path, amode)
 *	int os_chdir (path)
 *	int os_chmod (path, mode)
 *	int os_chown (path, owner, group)
 *	int os_close (fildes)
 *	int os_fstat (fildes, stbuf)
 *	int os_link (path1, path2)
 *	int os_open (path, oflag, mode)
 *	int os_read (fildes, buf, nbyte)
 *	int os_unlink (path)
 *	int os_utime (path, times)
 *	int os_write (fildes, buf, nbyte)
 *	int os_lstat (path, stbuf)
 *	int os_readlink (path, buf, bufsize)
 *	int os_symlink (path1, path2)
 *	int os_mknod (path, mode, rdev)
 */
#include "oscommon.h"

int os_access (const char *path, int amode)
{
	int rc;

	do
	{
		errno = 0;
	}
	while ((rc = access(path, amode)) < 0 && errno == EINTR);

	return (rc);
}

int os_chdir (const char *path)
{
	int rc;

	do
	{
		errno = 0;
	}
	while ((rc = chdir(path)) < 0 && errno == EINTR);

	return (rc);
}

int os_chmod (const char *path, int mode)
{
	int rc;

	do
	{
		errno = 0;
	}
	while ((rc = chmod(path, (mode_t)mode)) < 0 && errno == EINTR);

	return (rc);
}

int os_chown (const char *path, int owner, int group)
{
#if V_UNIX
	int rc;

	do
	{
		errno = 0;
	}
	while ((rc = chown(path, (uid_t)owner, (gid_t)group)) < 0 &&
		errno == EINTR);

	return (rc);
#else
	return (-1);
#endif
}

int os_close (int fildes)
{
	int rc;

	do
	{
		errno = 0;
	}
	while ((rc = close(fildes)) < 0 && errno == EINTR);

	return (rc);
}

int os_fstat (int fildes, struct stat *stbuf)
{
	int rc;

	do
	{
		errno = 0;
	}
	while ((rc = fstat(fildes, stbuf)) < 0 && errno == EINTR);

	return (rc);
}

int os_link (const char *path1, const char *path2)
{
#if V_UNIX
	int rc;

	do
	{
		errno = 0;
	}
	while ((rc = link(path1, path2)) < 0 && errno == EINTR);

	return (rc);
#else
	return (-1);
#endif
}

int os_open (const char *path, int oflag, int mode)
{
	int rc;

#ifdef O_BINARY
	oflag |= O_BINARY;
#endif

#if V_WINDOWS
	if (oflag & O_RDONLY)
	{
		struct stat stbuf;

		rc = os_lstat(path, &stbuf);
		if (rc)
			return (-1);

		if (S_ISLNK(stbuf.st_mode))
		{
			char linkpath[MAX_PATHLEN];

			rc = os_win_readlink(path, linkpath);
			if (rc)
				return (-1);

			rc = open(linkpath, oflag, mode);
			return (rc);
		}
	}
#endif

	do
	{
		errno = 0;
	}
	while ((rc = open(path, oflag, mode)) < 0 && errno == EINTR);

	return (rc);
}

int os_read (int fildes, char *buf, int nbyte)
{
	int rc;

	do
	{
		errno = 0;
	}
	while ((rc = read(fildes, buf, nbyte)) < 0 && errno == EINTR);

	return (rc);
}

int os_stat (const char *path, struct stat *stbuf)
{
	int rc;

	do
	{
		errno = 0;
	}
	while ((rc = stat(path, stbuf)) < 0 && errno == EINTR);

	return (rc);
}

int os_unlink (const char *path)
{
	int rc;

	do
	{
		errno = 0;
	}
	while ((rc = unlink(path)) < 0 && errno == EINTR);

	return (rc);
}

int os_utime (const char *path, struct utimbuf *times)
{
	int rc;

	do
	{
		errno = 0;
	}
	while ((rc = utime(path, times)) < 0 && errno == EINTR);

	return (rc);
}

int os_write (int fildes, char *buf, int nbyte)
{
	int rc;

	do
	{
		errno = 0;
	}
	while ((rc = write(fildes, buf, nbyte)) < 0 && errno == EINTR);

	return (rc);
}

int os_lstat (const char *path, struct stat *stbuf)
{
	int rc;

	do
	{
		errno = 0;
	}
#if X_IFLNK
	while ((rc = lstat(path, stbuf)) < 0 && errno == EINTR);
#else
	while ((rc = stat(path, stbuf)) < 0 && errno == EINTR);
#endif

#if V_WINDOWS
	if (rc == 0)
	{
		char *ext = fn_ext(path);

		if (ext != 0)
		{
			if (strcmp(ext, "lnk") == 0)
			{
				stbuf->st_mode = (stbuf->st_mode & ~S_IFMT) | S_IFLNK;
			}
		}
	}
#endif

	return (rc);
}

int os_readlink (const char *path, char *buf, int bufsize)
{
	int rc;

#if X_IFLNK
	do
	{
		errno = 0;
	}
	while ((rc = readlink(path, buf, bufsize)) < 0 && errno == EINTR);

	return (rc);
#else
	rc = os_win_readlink(path, buf);
	if (rc == 0)
		rc = strlen(buf);
	return (rc);
#endif
}

int os_symlink (const char *path1, const char *path2)
{
#if X_IFLNK
	int rc;

	do
	{
		errno = 0;
	}
	while ((rc = symlink(path1, path2)) < 0 && errno == EINTR);

	return (rc);
#else
	return os_win_makelink(path2, path1);
#endif
}

int os_mknod (const char *path, int mode, int rdev)
{
#if V_UNIX
	int rc;

	do
	{
		errno = 0;
	}
	while ((rc = mknod(path, (mode_t)mode, rdev)) < 0 && errno == EINTR);

	return (rc);
#else
	return (-1);
#endif
}
