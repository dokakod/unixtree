/*------------------------------------------------------------------------
 * find a file in a path
 */
#include "oscommon.h"

char * os_get_path (const char *filename, const char *path, char *fullpath)
{
	struct stat stbuf;
	char *p;
	int rc;
	const char *s;
	int delim;

	delim = *fn_path_separator();

	/* look thru path for directories */

	while (TRUE)
	{
		for (p=fullpath; *path && *path != delim; path++)
			*p++ = *path;
		*p = 0;

		if (*fullpath == 0)				/* empty string? */
			strcpy(fullpath, fn_cwdname());	/* use cwd */
		else
			fn_resolve_pathname(fullpath);

		fn_terminate_dirname(fullpath);
		p = fullpath + strlen(fullpath);

		if (fn_is_path_absolute(filename))
		{
			strcpy(fullpath, filename);
		}
		else
		{
			strcpy(p, filename);
		}
		s = fullpath;

		while (TRUE)
		{
			errno = 0;
			rc = os_stat(s, &stbuf);
			if (rc && errno == EINTR)
				continue;
			break;
		}

		if (rc == 0)
		{
			if (S_ISREG(stbuf.st_mode))
			{
				return (fullpath);
			}
		}

		if (*path++ == 0)
			break;
	}

	return (0);
}
