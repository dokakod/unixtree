/*------------------------------------------------------------------------
 * file functions
 */
#include "oscommon.h"

int os_file_delete (const char *path)
{
	int rc;

	rc = remove(path);
	if (rc == -1 && errno == ENOENT)
	{
		rc = 0;
		errno = 0;
	}

	return (rc);
}

int os_file_rename (const char *oldpath, const char *newpath)
{
	int rc;

	rc = rename(oldpath, newpath);
	if (rc)
	{
#ifdef EXDEV
		if (errno == EXDEV)
		{
			rc = os_file_copy(oldpath, newpath);
			if (rc == 0)
				rc = os_file_delete(oldpath);
		}
#endif
	}

	return (rc);
}

int os_file_copy (const char *oldpath, const char *newpath)
{
	int inp;
	int out;
	int size;
	char buffer[BUFSIZ];
	int buflen = sizeof(buffer);
	struct stat stbuf;
	int rc;

	rc = os_stat(oldpath, &stbuf);
	if (rc)
	{
		return (-1);
	}

	inp = os_open(oldpath, O_RDONLY, 0666);
	if (inp == -1)
	{
		return (-1);
	}

	out = os_open(newpath, O_WRONLY|O_CREAT|O_TRUNC, stbuf.st_mode & 07777);
	if (out == -1)
	{
		os_close(inp);
		return (-1);
	}

	while (TRUE)
	{
		size = os_read(inp, buffer, buflen);
		if (size == 0)
			break;
		if (size == -1 || os_write(out, buffer, size) == -1)
		{
			os_close(inp);
			os_close(out);
			return (-1);
		}
	}

	os_close(inp);
	os_close(out);

	return (0);
}
