/*------------------------------------------------------------------------
 * utility functions
 */
#include "xvtcommon.h"

#ifndef   S_ISREG
#  define S_ISREG(m)	((((m) & S_IFMT) == S_IFREG) || (((m) & S_IFMT) == 0))
#endif

/*------------------------------------------------------------------------
 * get the hostname for a machine
 */
static char * xvt_path_hostname (char *hostbuf, int len)
{
	char *	p;
	int		rc;

	/*--------------------------------------------------------------------
	 * get hostname from the system
	 */
#if defined(_WIN32) || defined(_MSC_VER)
	{
		DWORD	dwSize = len;

		rc = GetComputerName(hostbuf, &dwSize);
		rc = (rc ? 0 : -1);
	}
#else
	{
		rc = gethostname(hostbuf, len);
	}
#endif
	if (rc)
		return (0);

	/*--------------------------------------------------------------------
	 * remove any domain part
	 */
	p = strchr(hostbuf, '.');
	if (p != 0)
		*p = 0;

	return (hostbuf);
}

/*------------------------------------------------------------------------
 * get the basename of a path
 */
const char *xvt_path_basename (const char *path)
{
	const char *p;

	if (path == 0)
		return (0);

	p = strrchr(path, '/');
	if (p == 0)
		p = path;
	else
		p++;

	return (p);
}

/*------------------------------------------------------------------------
 * get the dir part of a path
 */
char * xvt_path_dirname (char *dirname, const char *path)
{
	char *	p;

	if (dirname == 0 || path == 0)
		return (0);

	strcpy(dirname, path);
	p = strrchr(dirname, '/');
	if (p == 0)
		strcpy(dirname, ".");
	else
		*p = 0;

	return (dirname);
}

/*------------------------------------------------------------------------
 * get the extension of a path
 */
const char * xvt_path_extension (const char *path)
{
	const char *	basename;
	const char *	extension;

	if (path == 0 || *path == 0)
		return (0);

	basename  = xvt_path_basename(path);
	extension = strrchr(basename, '.');
	if (extension != 0)
		extension++;

	return (extension);
}

/*------------------------------------------------------------------------
 * find a filename in a path
 */
int xvt_path_find (char *fullpath, const char *filename)
{
	const char *	path;
	const char *	p;
	int				delim	= ':';

	if (fullpath == 0 || filename == 0 || *filename == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * Check if filename represents an absolute path or a path which
	 * should not be searched for.
	 */
	if (xvt_path_basename(filename) != filename)
	{
		strcpy(fullpath, filename);
		return (0);
	}

	/*--------------------------------------------------------------------
	 * get path to search
	 */
	path = getenv("PATH");
	if (path == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * If a ";" is found in the path, use that as the delimiter.
	 */
	p = strchr(path, ';');
	if (p != 0)
		delim = ';';

	/*--------------------------------------------------------------------
	 * now check each path component
	 */
	p = path;
	while (TRUE)
	{
		struct stat	stbuf;
		char *		s;
		int			rc;

		/*----------------------------------------------------------------
		 * copy next path component
		 */
		for (s=fullpath; *p && *p != delim; p++)
			*s++ = *p;
		*s = 0;

		/*----------------------------------------------------------------
		 * if component is empty, use cwd
		 */
		if (*fullpath == 0)
			strcpy(fullpath, ".");

		/*----------------------------------------------------------------
		 * now append filename to path
		 */
		strcat(fullpath, "/");
		strcat(fullpath, filename);

		/*----------------------------------------------------------------
		 * check if path exists & is a file
		 */
		rc = stat(fullpath, &stbuf);
		if (rc == 0)
		{
			if (S_ISREG(stbuf.st_mode))
			{
				return (0);
			}
		}

		/*----------------------------------------------------------------
		 * go on to next path component
		 */
		if (*p++ == 0)
			break;
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * resolve a pathname
 *
 * returns: fullpath or NULL if can't resolve
 */
char * xvt_path_resolve (char *fullpath, const char *skeleton)
{
	const char *	s;
	char			skel_buf1[XVT_MAX_PATHLEN];
	char			skel_buf2[XVT_MAX_PATHLEN];

	if (fullpath == 0)
		return (0);

	if (skeleton == 0 || *skeleton == 0)
	{
		*fullpath = 0;
		return (0);
	}

	/*--------------------------------------------------------------------
	 * check if converting in place
	 */
	if (fullpath == skeleton)
	{
		strcpy(skel_buf1, skeleton);
		skeleton = skel_buf1;
	}

	*fullpath = 0;

	/*--------------------------------------------------------------------
	 * convert "~/..." to "$HOME/..."
	 */
	if (skeleton[0] == '~' && skeleton[1] == '/')
	{
		strcpy(skel_buf2, "$HOME");
		strcat(skel_buf2, skeleton+1);
		skeleton = skel_buf2;
	}

	/*--------------------------------------------------------------------
	 * now parse each part, looking for "$..."
	 */
	s = skeleton;

	if (*s == '/')
	{
		strcpy(fullpath, "/");
		s++;
	}

	while (*s != 0)
	{
		char *	p;
		char	part_buf[XVT_MAX_PATHLEN];

		/*----------------------------------------------------------------
		 * get next path part in part_buf
		 */
		for (p=part_buf; *s; s++)
		{
			if (*s == '/')
				break;

			*p++ = *s;
		}
		*p = 0;

		/*----------------------------------------------------------------
		 * if empty part, just ignore it,
		 * but skip over /  to avoid multiple /'s.
		 */
		if (*part_buf == 0)
		{
			if (*s == '/')
				s++;
			continue;
		}

		/*----------------------------------------------------------------
		 * now look for a $ in this part
		 */
		if (strchr(part_buf, '$') == 0)
		{
			strcat(fullpath, part_buf);
		}
		else
		{
			const char *	env;
			char			part_var[XVT_MAX_PATHLEN];
			char			part_new[XVT_MAX_PATHLEN];
			char *			m;
			char *			n;
			char *			v;

			n = part_new;
			m = part_buf;

			*n = 0;
			while (TRUE)
			{
				/*--------------------------------------------------------
				 * copy over chars until we get a $
				 */
				for (; *m; m++)
				{
					if (*m == '$')
						break;
					*n++ = *m;
				}
				*n = 0;

				if (*m == 0)
				{
					*n = 0;
					strcat(fullpath, part_new);
					break;
				}
				m++;

				/*--------------------------------------------------------
				 * we got a $, so copy all chars up to end-of-name
				 */
				if (*m == '{')
					m++;
				for (v=part_var; *m; m++)
				{
					if (*m == '}')
					{
						m++;
						break;
					}

					if (! isalnum(*m) && *m != '_')
						break;

					*v++ = *m;
				}
				*v = 0;

				if (*part_var == 0)
				{
					*n++ = '$';
					*n = 0;
					continue;
				}

				env = getenv(part_var);
				if (env == 0)
				{
					if (strcmp(part_var, "HOSTNAME") == 0)
						env = xvt_path_hostname(part_var, sizeof(part_var));
				}
				if (env == 0 || *env == 0)
				{
					*fullpath = 0;
					return (0);
				}

				strcat(n, env);
				n += strlen(env);
			}
		}

		if (*s == '/')
		{
			s++;
			strcat(fullpath, "/");
		}
	}

	return (fullpath);
}
