/*------------------------------------------------------------------------
 * find a file in a path
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * defines
 */
#if V_WINDOWS
#  define PATH_SEPARATOR	';'
#  define PATH_DELIM		'\\'
#  define TERM_SUBDIR		"trm\\"
#else
#  define PATH_SEPARATOR	':'
#  define PATH_DELIM		'/'
#  define TERM_SUBDIR		"trm/"
#endif

/*------------------------------------------------------------------------
 * tcap_path_abs() - internal routine to check if a path is absolute
 *
 * We always check for a "/", which is valid in either Unix or Windows
 */
static int tcap_path_abs (const char *path)
{
	const char *p = strchr(path, '/');

	if (p == 0)
		p = strchr(path, PATH_DELIM);

	return (p != 0);
}

/*------------------------------------------------------------------------
 * tcap_path_exists() - internal routine to check if a path exists
 */
int tcap_path_exists (const char *path)
{
	struct stat stbuf;
	int rc;

	while (TRUE)
	{
		rc = stat(path, &stbuf);
		if (rc < 0)
		{
			if (errno == EINTR)
				continue;
			return (FALSE);
		}
		break;
	}

	return (TRUE);
}

/*------------------------------------------------------------------------
 * tcap_find_path() - find a filename in a path
 *
 * The path is a list of delimited directories (ala $PATH).
 * For each entry (and "." first), we check both the directory and
 * a "trm" sub-directory.
 */
int tcap_find_path (const char *filename, const char *path, char *fullpath)
{
	const char *p;

	/*----------------------------------------------------------------
	 * must have a filename & a buffer ptr
	 */
	if (filename == 0 || *filename == 0)
		return (-1);

	if (fullpath == 0)
		return (-1);

	/*----------------------------------------------------------------
	 * NULL path implies $PATH
	 */
	if (path == 0)
		path = getenv("PATH");

	if (path == 0)
		path = "";

	/*----------------------------------------------------------------
	 * always check "." first
	 */
	strcpy(fullpath, filename);
	if ( tcap_path_exists(fullpath) )
		return (0);

	/*----------------------------------------------------------------
	 * if abs path, we're done
	 */
	if ( tcap_path_abs(filename) )
		return (-1);

	/*----------------------------------------------------------------
	 * now check "./trm"
	 */
	strcpy(fullpath, TERM_SUBDIR);
	strcat(fullpath, filename);
	if ( tcap_path_exists(fullpath) )
		return (0);

	/*----------------------------------------------------------------
	 * run through the path
	 */
	p = path;

	while (*p)
	{
		char *fp = fullpath;

		/*--------------------------------------------------------
		 * copy next part
		 */
		for (; *p; p++)
		{
			if (*p == ';' || *p == PATH_SEPARATOR)
				break;
			*fp++ = *p;
		}
		*fp++ = PATH_DELIM;
		*fp = 0;

		/*--------------------------------------------------------
		 * only deal with a non-empty part
		 */
		if (fullpath[1] != 0)
		{
			strcpy(fp, filename);
			if ( tcap_path_exists(fullpath) )
				return (0);

			strcpy(fp, TERM_SUBDIR);
			strcat(fp, filename);
			if ( tcap_path_exists(fullpath) )
				return (0);
		}

		/*--------------------------------------------------------
		 * check if at end of string
		 */
		if (*p == 0)
			break;

		p++;
	}

	return (-1);
}
