/*------------------------------------------------------------------------
 * VMS filename routines
 */
#include "oscommon.h"

#define VMS_ROOT		"000000"
#define VMS_ROOT_LEN	6
#define VMS_ROOT_NAME	"[000000]"
#define VMS_HOME_NAME	"SYS$LOGIN"
#define VMS_TMP_NAME	"[]"
#define VMS_CWD_NAME	"[]"
#define VMS_PATH_SEP	";"

static const char vms_fn_bogus_filespec_chars[]	= "&()<>{}\\|!\"\'`~";
static const char vms_fn_bogus_filename_chars[]	= " &()<>{}\\|!\"\'`~[]^";
static const char vms_fn_bogus_pathname_chars[]	= " &()<>{}\\|!\"\'`[]^*?";

int
vms_fn_is_type (const char *path)
{
	if (strchr(path, '[') != 0)
		return (TRUE);
	return (FALSE);
}

int
vms_fn_is_path_absolute (const char *path)
{
	const char *p;

	p = strchr(path, '[');
	if (!p)
		return (FALSE);

	if (p[1] == '.' || p[1] == '-')
		return (FALSE);

	return (TRUE);
}

int
vms_fn_is_path_a_dirname (const char *path)
{
	if (path[strlen(path)-1] == ']')
		return (TRUE);
	return (FALSE);
}

int
vms_fn_is_dir_in_dirname (const char *dir, const char *dirname)
{
	int len;

	for (dir=dir; *dir; dir++)
	{
		if (*dir == '[')
			break;
	}
	if (!*dir++)
		return (FALSE);
	dir++;
	if (strncmp(dir, VMS_ROOT, VMS_ROOT_LEN) == 0)
		dir += VMS_ROOT_LEN;
	if (*dir == '.')
		dir++;

	for (dirname=dirname; *dirname; dirname++)
	{
		if (*dirname == '[')
			break;
	}
	if (!*dirname++)
		return (FALSE);
	if (strncmp(dirname, VMS_ROOT, VMS_ROOT_LEN) == 0)
		dirname += VMS_ROOT_LEN;
	if (*dirname == '.')
		dirname++;

	len = strlen(dir) - 1;

	if (strncmp(dir, dirname, len) != 0)
		return (FALSE);

	if (dirname[len] != '.' || dirname[len] != ']')
		return (FALSE);

	return (TRUE);
}

/* ARGSUSED */
int
vms_fn_is_filename_hidden (const char *dir, const char *filename)
{
	return (FALSE);
}

int
vms_fn_bogus_filespec (int c)
{
	if (strchr(vms_fn_bogus_filespec_chars, c) == 0)
		return (FALSE);
	return (TRUE);
}

int
vms_fn_bogus_filename (int c)
{
	if (strchr(vms_fn_bogus_filename_chars, c) == 0)
		return (FALSE);
	return (TRUE);
}

int
vms_fn_bogus_pathname (int c)
{
	if (strchr(vms_fn_bogus_pathname_chars, c) == 0)
		return (FALSE);
	return (TRUE);
}

const char *
vms_fn_all (void)
{
	return ("*.*");
}

const char *
vms_fn_rootname (const char *path, char *root)
{
	strcpy(root, VMS_ROOT_NAME);

	return (root);
}

const char *
vms_fn_tmpname (void)
{
	return (VMS_TMP_NAME);
}

const char *
vms_fn_cwdname (void)
{
	return (VMS_CWD_NAME);
}

const char *
vms_fn_home (void)
{
	return (VMS_HOME_NAME);
}

const char *
vms_fn_path_separator (void)
{
	return (VMS_PATH_SEP);
}

const char *
vms_fn_null_path (void)
{
	return ("foo");
}

char *
vms_fn_getcwd (char *path)
{
	os_get_cwd(path);

	return (path);
}

char *
vms_fn_nodename (char *path, char *buf)
{
	char *p;

	for (p=path; *p; p++)
	{
		if (*p == '[')
			return (0);

		if (*p == ':' && p[1] == '[')
			break;
	}

	if (! *p)
		return (0);

	*p = 0;
	strcpy(buf, path);

	strcpy(path, p+1);

	return (buf);
}

int
vms_fn_filename_index (const char *path)
{
	const char *p;

	/* scan to end of path */

	for (p=path; *p; p++)
		;

	/* scan backwards looking for ] */

	for (p--; p>=path; p--)
	{
		if (*p == ']')
			break;
	}
	p++;

	return (p - path);
}

int
vms_fn_num_subdirs (const char *path)
{
	int i;

	i = 0;
	for (; *path; path++)
	{
		if (*path == '[')
			break;
	}
	path++;
	if (strncmp(path, VMS_ROOT, VMS_ROOT_LEN) == 0)
	{
		path += VMS_ROOT_LEN;
		if (*path == '.')
			path++;
	}

	if (*path == ']')
		return (i);

	if (*path)
	{
		for (; *path; path++)
		{
			if (*path == ']')
			{
				i++;
				break;
			}
			if (*path == '.')
				i++;
		}
	}

	return (i);
}

char *
vms_fn_basename (const char *path)
{
	return (char *)(path + vms_fn_filename_index(path));
}

char *
vms_fn_dirname (const char *path, char *direct)
{
	int i;

	for (i=0; path[i]; i++)
	{
		direct[i] = path[i];
		if (*path == ']')
			break;
	}
	direct[i] = 0;

	return (direct);
}

char *
vms_fn_terminate_dirname (char *path)
{
	return (path);
}

char *
vms_fn_unterminate_dirname (char *path)
{
	return (path);
}

char *
vms_fn_append_filename_to_dir (char *dir, const char *file)
{
	strcat(dir, file);

	return (dir);
}

char *
vms_fn_append_dirname_to_dir (char *dir, const char *subdir)
{
	char *pl;
	char *pr;

	pl = strchr(dir, '[');
	if (!pl)
	{
		pl = dir;
		strcpy(pl, "[]");
	}
	pl++;
	if (strncmp(pl, VMS_ROOT, VMS_ROOT_LEN) == 0)
	{
		pr = pl+VMS_ROOT_LEN;
		if (*pr == '.')
			pr++;
		strcpy(pl, pr);
	}

	pr = strchr(dir, ']');
	if (pr)
	{
		if (pr[-1] != '[')
			*pr++ = '.';
		strcpy(pr, subdir);
		strcat(pr, "]");
	}
	else
	{
		strcat(dir, "[");
		strcat(dir, subdir);
		strcat(dir, "]");
	}

	return (dir);
}

char *
vms_fn_cleanup_path (char *path)
{
	return (path);
}

char *
vms_fn_get_nth_dir (const char *path, int n, char *dirname)
{
	const char *p;
	int i;

	p = strchr(path, '[');
	if (!p)
		return (0);
	path = p+1;

	if (strncmp(path, VMS_ROOT, VMS_ROOT_LEN) == 0)
	{
		path += VMS_ROOT_LEN;
		if (*path == '.')
			path++;
	}

	if (n)
	{
		for (; n; n--)
		{
			p = strchr(path, '.');
			if (p == 0)
				return (0);
			path = p+1;
		}
	}

	for (i=0; path[i]; i++)
	{
		if (path[i] == '.' || path[i] == ']')
			break;
		dirname[i] = path[i];
	}
	dirname[i] = 0;

	return (dirname);
}

/* ARGSUSED */
int
vms_fn_resolve_pathname (char *path)
{
	return (0);
}

char *
vms_fn_disp_path (const char *path, int len, char *fixed)
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
		if (*p == '.')
			break;
	}
	strcpy(fixed, "[..");
	if (!*p)			/* final filename too long */
		strcat(fixed, path+i-len+3);
	else
		strcat(fixed, p);

	return (fixed);
}

char *
vms_fn_get_abs_dir (const char *base, const char *rel_name, char *abs_name)
{
	int i;
	char *p;
	char rel_dir[MAX_PATHLEN];

	strcpy(rel_dir, rel_name);
	vms_fn_resolve_pathname(rel_dir);
	if (vms_fn_is_path_absolute(rel_dir))
	{
		strcpy(abs_name, rel_name);
		return (abs_name);
	}

	strcpy(abs_name, base);

	i = 0;
	while (TRUE)
	{
		char dir_name[MAX_FILELEN];

		p = vms_fn_get_nth_dir(rel_dir, i++, dir_name);
		if (!p)
			break;
		vms_fn_append_dirname_to_dir(abs_name, p);
	}
	vms_fn_cleanup_path(abs_name);
	vms_fn_unterminate_dirname(abs_name);

	return (abs_name);
}

char *
vms_fn_get_abs_path (const char *base, const char *rel_name, char *abs_name)
{
	char dir_name[MAX_PATHLEN];
	int i;
	char *p;
	char *d;
	char *f;

	d = vms_fn_dirname(rel_name, dir_name);
	f = vms_fn_basename(rel_name);

	vms_fn_resolve_pathname(d);
	if (vms_fn_is_path_absolute(d))
	{
		strcpy(abs_name, d);
		vms_fn_append_filename_to_dir(abs_name, f);
		return (abs_name);
	}

	strcpy(abs_name, base);

	i = 0;
	while (TRUE)
	{
		char dir_name[MAX_FILELEN];

		p = vms_fn_get_nth_dir(d, i++, dir_name);
		if (!p)
			break;
		vms_fn_append_dirname_to_dir(abs_name, p);
	}
	vms_fn_cleanup_path(abs_name);
	vms_fn_append_filename_to_dir(abs_name, f);

	return (abs_name);
}
