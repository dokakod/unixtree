/*------------------------------------------------------------------------
 * filename functions
 */
#include "oscommon.h"

int
fn_is_type (const char *path)
{
	return x_fn_is_type(FN_SYSTEM, path);
}

int
fn_is_path_absolute (const char *path)
{
	return x_fn_is_path_absolute(FN_SYSTEM, path);
}

int
fn_is_path_a_dirname (const char *path)
{
	return x_fn_is_path_a_dirname(FN_SYSTEM, path);
}

int
fn_is_dir_in_dirname (const char *dir, const char *dirname)
{
	return x_fn_is_dir_in_dirname(FN_SYSTEM, dir, dirname);
}

int
fn_is_filename_hidden (const char *dir, const char *filename)
{
	return x_fn_is_filename_hidden(FN_SYSTEM, dir, filename);
}

int
fn_bogus_filespec (int c)
{
	return x_fn_bogus_filespec(FN_SYSTEM, c);
}

int
fn_bogus_filename (int c)
{
	return x_fn_bogus_filename(FN_SYSTEM, c);
}

int
fn_bogus_pathname (int c)
{
	return x_fn_bogus_pathname(FN_SYSTEM, c);
}

const char *
fn_all (void)
{
	return x_fn_all(FN_SYSTEM);
}

const char *
fn_rootname (const char *path, char *root)
{
	return x_fn_rootname(FN_SYSTEM, path, root);
}

const char *
fn_tmpname (void)
{
	return x_fn_tmpname(FN_SYSTEM);
}

const char *
fn_cwdname (void)
{
	return x_fn_cwdname(FN_SYSTEM);
}

const char *
fn_parent (void)
{
	return ("..");
}

const char *
fn_home (void)
{
	return x_fn_home(FN_SYSTEM);
}

const char *
fn_path_separator (void)
{
	return x_fn_path_separator(FN_SYSTEM);
}

const char *
fn_null_path (void)
{
	return x_fn_null_path(FN_SYSTEM);
}

char *
fn_getcwd (char *path)
{
	return x_fn_getcwd(FN_SYSTEM, path);
}

char *
fn_nodename (char *path, char *node)
{
	return x_fn_nodename(FN_SYSTEM, path, node);
}

int
fn_filename_index (const char *path)
{
	return x_fn_filename_index (FN_SYSTEM, path);
}

int
fn_num_subdirs (const char *path)
{
	return x_fn_num_subdirs(FN_SYSTEM, path);
}

char *
fn_basename (const char *path)
{
	return x_fn_basename(FN_SYSTEM, path);
}

char *
fn_dirname (const char *path, char *dir)
{
	return x_fn_dirname(FN_SYSTEM, path, dir);
}

char *
fn_terminate_dirname (char *path)
{
	return x_fn_terminate_dirname(FN_SYSTEM, path);
}

char *
fn_unterminate_dirname (char *path)
{
	return x_fn_unterminate_dirname(FN_SYSTEM, path);
}

char *
fn_append_filename_to_dir (char *dir, const char *file)
{
	return x_fn_append_filename_to_dir(FN_SYSTEM, dir, file);
}

char *
fn_append_dirname_to_dir (char *dir, const char *subdir)
{
	return x_fn_append_dirname_to_dir(FN_SYSTEM, dir, subdir);
}

char *
fn_cleanup_path (char *path)
{
	return x_fn_cleanup_path(FN_SYSTEM, path);
}

char *
fn_get_nth_dir (const char *path, int n, char *dir)
{
	return x_fn_get_nth_dir(FN_SYSTEM, path, n, dir);
}

int
fn_resolve_pathname (char *path)
{
	return x_fn_resolve_pathname(FN_SYSTEM, path);
}

char *
fn_disp_path (const char *path, int len, char *fixed)
{
	return x_fn_disp_path(FN_SYSTEM, path, len, fixed);
}

char *
fn_get_abs_dir (const char *base, const char *rel_name, char *abs_name)
{
	return x_fn_get_abs_dir(FN_SYSTEM, base, rel_name, abs_name);
}

char *
fn_get_abs_path (const char *base, const char *rel_name, char *abs_name)
{
	return x_fn_get_abs_path(FN_SYSTEM, base, rel_name, abs_name);
}

char *
fn_convert_dirname (const char *path, int from, int  to, char *new_path)
{
	char old_path[MAX_PATHLEN];
	int n;
	int i;
	char *p;

	if (from == 0)
		from = fn_pathtype(path);

	if (from == to)
	{
		strcpy(new_path, path);
		return (new_path);
	}

	if (x_fn_is_type(to, path))
	{
		strcpy(new_path, path);
		return (new_path);
	}

	strcpy(old_path, path);

	n = x_fn_num_subdirs(from, old_path);

	x_fn_rootname(to, old_path, new_path);

	for (i=0; i<n; i++)
	{
		char dir_name[MAX_FILELEN];

		p = x_fn_get_nth_dir(from, old_path, i, dir_name);
		x_fn_append_dirname_to_dir(to, new_path, p);
	}

	x_fn_unterminate_dirname(to, new_path);

	return (new_path);
}

char *
fn_convert_filename (const char *path, int from, int to, char *new_path)
{
	char dir_name[MAX_PATHLEN];
	char *p;
	char *f;

	if (from == 0)
		from = fn_pathtype(path);

	if (from == to)
	{
		strcpy(new_path, path);
		return (new_path);
	}

	f = x_fn_basename(from, path);
	p = x_fn_dirname(from, path, dir_name);
	p = fn_convert_dirname(p, from, to, new_path);
	x_fn_append_filename_to_dir(to, new_path, f);

	return (new_path);
}

char *
fn_ext (const char *path)
{
	char *p;

	p = fn_basename(path);
	p = strrchr(p, '.');
	if (p != (char *)NULL)
		p++;

	return (p);
}

char *
fn_rem_ext (char *path)
{
	char *p = fn_ext(path);

	if (p != (char *)NULL)
		p[-1] = 0;

	return (path);
}

char *
fn_set_ext (char *path, const char *ext)
{
	char *p = fn_ext(path);

	if (ext != 0 && *ext == '.')
		ext++;

	if (p == (char *)NULL)
	{
		if (ext != 0)
		{
			strcat(path, ".");
			strcat(path, ext);
		}
	}
	else
	{
		if (ext != 0)
			strcpy(p, ext);
		else
			p[-1] = 0;
	}

	return (path);
}

int
fn_pathtype (const char *path)
{
	if (unix_fn_is_type(path))	return (FN_UNIX);
	if ( vms_fn_is_type(path))	return (FN_VMS);
	if ( dos_fn_is_type(path))	return (FN_DOS);

	return (FN_SYSTEM);
}
