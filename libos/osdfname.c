/*------------------------------------------------------------------------
 * Windows filename functions
 */
#include "oscommon.h"

static const char dos_fn_bogus_filespec_chars[]	= "&()<>{}\\|!\"\'`";
static const char dos_fn_bogus_filename_chars[]	= "&()<>{}\\|!\"\'`[]^";
static const char dos_fn_bogus_pathname_chars[]	= "&()<>{}|!\"\'`[]^*?";

int
dos_fn_is_type (const char *path)
{
	if (strchr(path, '\\') != 0)
		return (TRUE);
	return (FALSE);
}

int
dos_fn_is_path_absolute (const char *path)
{
	if (*path == '\\' || *path == '/')
		return (TRUE);
	if (path[1] == ':' && (path[2] == '\\' || path[2] == '/'))
		return (TRUE);
	return (FALSE);
}

int
dos_fn_is_path_a_dirname (const char *path)
{
	int l = strlen(path) - 1;

	if (path[l] == '\\' || path[l] == '/')
		return (TRUE);
	return (FALSE);
}

int
dos_fn_is_dir_in_dirname (const char *dir, const char *dirname)
{
	int l;

	l = strlen(dir);

	if (strncmp(dir, dirname, l) == 0 &&
		(dirname[l] == 0 || dirname[l] == '\\' || dirname[l] == '/'))
		return (TRUE);
	return (FALSE);
}

/* ARGSUSED */
int
dos_fn_is_filename_hidden (const char *dir, const char *filename)
{
	return (FALSE);
}

int
dos_fn_bogus_filespec (int c)
{
	if (strchr(dos_fn_bogus_filespec_chars, c) == 0)
		return (FALSE);
	return (TRUE);
}

int
dos_fn_bogus_filename (int c)
{
	if (strchr(dos_fn_bogus_filename_chars, c) == 0)
		return (FALSE);
	return (TRUE);
}

int
dos_fn_bogus_pathname (int c)
{
	if (strchr(dos_fn_bogus_pathname_chars, c) == 0)
		return (FALSE);
	return (TRUE);
}

const char *
dos_fn_all (void)
{
	return ("*.*");
}

const char *
dos_fn_rootname (const char *path, char *root)
{
	if (path == 0 || *path == 0)
	{
		strcpy(root, "\\");
	}
	else if (dos_fn_is_path_absolute(path))
	{
		const char *p;
		char *r = root;

		for (p=path; *p; p++)
		{
			int c = *p;

			if (c == '/')
				c = '\\';

			*r++ = c;
			if (c == '\\')
				break;
		}
		*r = 0;
	}
	else
	{
		strcpy(root, "\\");
	}

	return (root);
}

const char *
dos_fn_tmpname (void)
{
	return ("\\temp");
}

const char *
dos_fn_cwdname (void)
{
	return (".");
}

const char *
dos_fn_home (void)
{
	return ("~");
}

const char *
dos_fn_path_separator (void)
{
	return (";");
}

const char *
dos_fn_null_path (void)
{
	return ("nul:");
}

char *
dos_fn_getcwd (char *path)
{
	os_get_cwd(path);

	return (path);
}

char *
dos_fn_nodename (char *path, char *buf)
{
#if 0
	char *p;

	for (p=path; *p; p++)
	{
		if (*p == ':')
			break;

		if (*p == '\\' || *p == '/')
			return (0);
	}

	if (! *p)
		return (0);

	*p = 0;
	strcpy(buf, path);

	strcpy(path, p+1);

	return (buf);
#else
	return (0);
#endif
}

int
dos_fn_filename_index (const char *path)
{
	const char *p;

	/* scan to end of path */

	for (p=path; *p; p++)
		;

	/* scan backwards looking for last / */

	for (p--; p>=path; p--)
	{
		if (*p == '\\' || *p == '/')
			break;
	}
	p++;

	return (p - path);
}

int
dos_fn_num_subdirs (const char *path)
{
	int i;

	i = 0;
	for (; *path; path++)
	{
		if ((path[0] == '\\' || path[0] == '/') && path[1] != 0)
			i++;
	}

	return (i);
}

char *
dos_fn_basename (const char *path)
{
	return (char *)(path + dos_fn_filename_index(path));
}

char *
dos_fn_dirname (const char *path, char *direct)
{
	char *dp;
	int fnd_slash = FALSE;

	for (dp=direct; *path; path++, dp++)
	{
		*dp = *path;
		if (*dp == '\\' || *dp == '/')
			fnd_slash = TRUE;
	}

	if (!fnd_slash)
	{
		strcpy(direct, dos_fn_cwdname());
	}
	else
	{
		for (dp--; dp>direct && *dp!='\\' && *dp != '/'; dp--)
			;
		if (dp == direct)
			dp++;
		*dp = 0;
	}

	return (direct);
}

char *
dos_fn_terminate_dirname (char *path)
{
	char *p;

	p = path + strlen(path) - 1;
	if (*p != '\\' && *p != '/')
		strcat(path, "\\");

	return (path);
}

char *
dos_fn_unterminate_dirname (char *path)
{
	char *p;

	p = path + strlen(path) - 1;
	if (*p == '\\' || *p == '/')
		*p = 0;

	return (path);
}

char *
dos_fn_append_filename_to_dir (char *dir, const char *file)
{
	dos_fn_terminate_dirname(dir);
	strcat(dir, file);

	return (dir);
}

char *
dos_fn_append_dirname_to_dir (char *dir, const char *subdir)
{
	dos_fn_terminate_dirname(dir);
	strcat(dir, subdir);

	return (dir);
}

char *
dos_fn_cleanup_path (char *path)
{
	char *save_path = path;
	char *prev;
	char *curr;

	/* make sure we are dealing with an absolute path */

	if (! dos_fn_is_path_absolute(path) )
		return (path);

	/* skip over drive */

	if (path[1] == ':')
		path += 2;

	/* add slash to end (we will remove it later) */

	strcat(path, "\\");

	/* get rid of leading  // & /./ & /../ */

start:
	while (TRUE)
	{
		if (strncmp(path, "\\\\", 2) == 0 && path[2])
			strcpy(path, path+1);
		else if (strncmp(path, "\\.\\", 3) == 0)
			strcpy(path, path+2);
		else if (strncmp(path, "\\..\\", 4) == 0)
			strcpy(path, path+3);
		else
			break;
	}

	/* position prev to 1st part & curr to next */

	prev = path+1;
	for (curr=prev; *curr != '\\'; curr++)
		;
	curr++;

	/* loop thru entire path */

	while (*curr)
	{
		if (curr[0] == '\\')
		{
			strcpy(curr, curr+1);
			continue;
		}
		if (curr[0] == '.' && curr[1] == '\\')
		{
			strcpy(curr, curr+2);
			continue;
		}
		if (curr[0] == '.' && curr[1] == '.' && curr[2] == '\\')
		{
			strcpy(prev, curr+3);
			goto start;
		}
		prev = curr;
		for (curr=prev; *curr != '\\'; curr++)
			;
		curr++;
	}

	/* zap trailing slash */

	if (path[1])
		path[strlen(path)-1] = 0;

	return (save_path);
}

char *
dos_fn_get_nth_dir (const char *path, int n, char *dirname)
{
	char pathname[MAX_PATHLEN];
	const char *p;
	int i;

	strcpy(pathname, path);
	dos_fn_terminate_dirname(pathname);
	path = pathname;

	p = strchr(path, ':');
	if (p != 0)
		path = p+1;

	if (*path == '\\' || *path == '/')
		path++;

	for (; n; n--)
	{
		p = strchr(path, '\\');
		if (p == 0)
			p = strchr(path, '/');
		if (p == 0)
			return (0);
		path = p+1;
	}

	if (*path == 0)
		return (0);

	for (i=0; path[i]; i++)
	{
		if (path[i] == '\\' || path[i] == '/')
			break;
		dirname[i] = path[i];
	}
	dirname[i] = 0;

	return (dirname);
}

int
dos_fn_resolve_pathname (char *path)
{
	char usr[MAX_PATHLEN];
	char tmp[MAX_PATHLEN];
	char *p;
	char *h;
	char *t;

	t = tmp;
	*t = 0;
	p = path;

	if (*p == '~')
	{
		p++;
		if (*p == 0 || *p == '\\' || *p == '/')
		{
			h = getenv("HOME");
			if (h)
			{
				strcpy(tmp, h);
				strcat(tmp, p);
			}
			else
			{
				strcpy(tmp, "c:");
				if (*p == 0)
					strcat(tmp, "\\");
				else
					strcat(tmp, p);
			}
		}
		else
		{
			h = usr;
			for (; *p && *p != '\\' && *p != '/'; p++)
				*h++ = *p;
			*h = 0;

			h = os_get_usr_home(usr, tmp);
			if (h)
			{
				strcat(tmp, p);
			}
			else
			{
				*tmp = 0;
			}
		}
	}
	else if (*p == '$')
	{
		p++;
		for (; *p && *p != '\\' && *p != '/'; p++)
			*t++ = *p;
		*t = 0;
		h = getenv(tmp);
		if (h)
		{
			strcpy(tmp, h);
			strcat(tmp, p);
		}
		else
		{
			*tmp = 0;
		}
	}

	if (*tmp)
		strcpy(path, tmp);

	for (p=path; *p; p++)
	{
		if (*p == '/')
			*p = '\\';
	}

	dos_fn_cleanup_path(path);

	return (0);
}

char *
dos_fn_disp_path (const char *path, int len, char *fixed)
{
	int i;
	const char *p;

	i = strlen(path);
	if (i < len)
	{
		strcpy(fixed, path);
		return (fixed);
	}

	for (p=path+i-len+3; *p; p++)
	{
		if (*p == '\\' || *p == '/')
			break;
	}
	strcpy(fixed, "...");
	if (!*p)			/* final filename too long */
		strcat(fixed, path+i-len+3);
	else
		strcat(fixed, p);

	for (i=0; fixed[i]; i++)
	{
		if (fixed[i] == '/')
			fixed[i] = '\\';
	}

	return (fixed);
}

char *
dos_fn_get_abs_dir (const char *base, const char *rel_name, char *abs_name)
{
	int i;
	char *p;
	char rel_dir[MAX_PATHLEN];

	strcpy(rel_dir, rel_name);
	dos_fn_resolve_pathname(rel_dir);
	if (dos_fn_is_path_absolute(rel_dir))
	{
		strcpy(abs_name, rel_name);
		return (abs_name);
	}

	strcpy(abs_name, base);

	i = 0;
	while (TRUE)
	{
		char dir_name[MAX_FILELEN];

		p = dos_fn_get_nth_dir(rel_dir, i++, dir_name);
		if (!p)
			break;
		dos_fn_append_dirname_to_dir(abs_name, p);
	}
	dos_fn_cleanup_path(abs_name);
	dos_fn_unterminate_dirname(abs_name);

	return (abs_name);
}

char *
dos_fn_get_abs_path (const char *base, const char *rel_name, char *abs_name)
{
	char dir_name[MAX_PATHLEN];
	int i;
	char *p;
	char *d;
	char *f;

	d = dos_fn_dirname(rel_name, dir_name);
	f = dos_fn_basename(rel_name);

	dos_fn_resolve_pathname(d);
	if (dos_fn_is_path_absolute(d))
	{
		strcpy(abs_name, d);
		dos_fn_append_filename_to_dir(abs_name, f);
		return (abs_name);
	}

	strcpy(abs_name, base);

	i = 0;
	while (TRUE)
	{
		char dir_name[MAX_FILELEN];

		p = dos_fn_get_nth_dir(d, i++, dir_name);
		if (!p)
			break;
		dos_fn_append_dirname_to_dir(abs_name, p);
	}
	dos_fn_cleanup_path(abs_name);
	dos_fn_append_filename_to_dir(abs_name, f);

	return (abs_name);
}
