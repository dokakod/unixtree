/*------------------------------------------------------------------------
 * generic filename functions
 */
#include "oscommon.h"

int
x_fn_is_type (int system_type, const char *path)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_is_type(path);
	case FN_VMS:	return  vms_fn_is_type(path);
	case FN_DOS:	return  dos_fn_is_type(path);
	}
	return (FALSE);
}

int
x_fn_is_path_absolute (int system_type, const char *path)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_is_path_absolute(path);
	case FN_VMS:	return  vms_fn_is_path_absolute(path);
	case FN_DOS:	return  dos_fn_is_path_absolute(path);
	}
	return (FALSE);
}

int
x_fn_is_path_a_dirname (int system_type, const char *path)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_is_path_a_dirname(path);
	case FN_VMS:	return  vms_fn_is_path_a_dirname(path);
	case FN_DOS:	return  dos_fn_is_path_a_dirname(path);
	}
	return (FALSE);
}

int
x_fn_is_dir_in_dirname (int system_type, const char *dir, const char *dirname)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_is_dir_in_dirname(dir, dirname);
	case FN_VMS:	return  vms_fn_is_dir_in_dirname(dir, dirname);
	case FN_DOS:	return  dos_fn_is_dir_in_dirname(dir, dirname);
	}
	return (FALSE);
}

int
x_fn_is_filename_hidden (int system_type, const char *dir,
	const char *filename)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_is_filename_hidden(dir, filename);
	case FN_VMS:	return  vms_fn_is_filename_hidden(dir, filename);
	case FN_DOS:	return  dos_fn_is_filename_hidden(dir, filename);
	}
	return (FALSE);
}

int
x_fn_bogus_filespec (int system_type, int c)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_bogus_filespec(c);
	case FN_VMS:	return  vms_fn_bogus_filespec(c);
	case FN_DOS:	return  dos_fn_bogus_filespec(c);
	}
	return (FALSE);
}

int
x_fn_bogus_filename (int system_type, int c)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_bogus_filename(c);
	case FN_VMS:	return  vms_fn_bogus_filename(c);
	case FN_DOS:	return  dos_fn_bogus_filename(c);
	}
	return (FALSE);
}

int
x_fn_bogus_pathname (int system_type, int c)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_bogus_pathname(c);
	case FN_VMS:	return  vms_fn_bogus_pathname(c);
	case FN_DOS:	return  dos_fn_bogus_pathname(c);
	}
	return (FALSE);
}

const char *
x_fn_all (int system_type)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_all();
	case FN_VMS:	return  vms_fn_all();
	case FN_DOS:	return  dos_fn_all();
	}
	return ("");
}

const char *
x_fn_rootname (int system_type, const char *path, char *root)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_rootname(path, root);
	case FN_VMS:	return  vms_fn_rootname(path, root);
	case FN_DOS:	return  dos_fn_rootname(path, root);
	}
	return ("");
}

const char *
x_fn_tmpname (int system_type)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_tmpname();
	case FN_VMS:	return  vms_fn_tmpname();
	case FN_DOS:	return  dos_fn_tmpname();
	}
	return ("");
}

const char *
x_fn_cwdname (int system_type)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_cwdname();
	case FN_VMS:	return  vms_fn_cwdname();
	case FN_DOS:	return  dos_fn_cwdname();
	}
	return ("");
}

const char *
x_fn_home (int system_type)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_home();
	case FN_VMS:	return  vms_fn_home();
	case FN_DOS:	return  dos_fn_home();
	}
	return ("");
}

const char *
x_fn_path_separator (int system_type)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_path_separator();
	case FN_VMS:	return  vms_fn_path_separator();
	case FN_DOS:	return  dos_fn_path_separator();
	}
	return ("");
}

const char *
x_fn_null_path (int system_type)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_null_path();
	case FN_VMS:	return  vms_fn_null_path();
	case FN_DOS:	return  dos_fn_null_path();
	}
	return ("");
}

char *
x_fn_getcwd (int system_type, char *path)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_getcwd(path);
	case FN_VMS:	return  vms_fn_getcwd(path);
	case FN_DOS:	return  dos_fn_getcwd(path);
	}
	return ((char *)"");
}

char *
x_fn_nodename (int system_type, char *path, char *node)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_nodename(path, node);
	case FN_VMS:	return  vms_fn_nodename(path, node);
	case FN_DOS:	return  dos_fn_nodename(path, node);
	}
	return ((char *)"");
}

int
x_fn_filename_index (int system_type, const char *path)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_filename_index(path);
	case FN_VMS:	return  vms_fn_filename_index(path);
	case FN_DOS:	return  dos_fn_filename_index(path);
	}
	return (0);
}

int
x_fn_num_subdirs (int system_type, const char *path)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_num_subdirs(path);
	case FN_VMS:	return  vms_fn_num_subdirs(path);
	case FN_DOS:	return  dos_fn_num_subdirs(path);
	}
	return (0);
}

char *
x_fn_basename (int system_type, const char *path)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_basename(path);
	case FN_VMS:	return  vms_fn_basename(path);
	case FN_DOS:	return  dos_fn_basename(path);
	}
	return ((char *)"");
}

char *
x_fn_dirname (int system_type, const char *path, char *dir)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_dirname(path, dir);
	case FN_VMS:	return  vms_fn_dirname(path, dir);
	case FN_DOS:	return  dos_fn_dirname(path, dir);
	}
	return ((char *)"");
}

char *
x_fn_terminate_dirname (int system_type, char *path)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_terminate_dirname(path);
	case FN_VMS:	return  vms_fn_terminate_dirname(path);
	case FN_DOS:	return  dos_fn_terminate_dirname(path);
	}
	return ((char *)"");
}

char *
x_fn_unterminate_dirname (int system_type, char *path)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_unterminate_dirname(path);
	case FN_VMS:	return  vms_fn_unterminate_dirname(path);
	case FN_DOS:	return  dos_fn_unterminate_dirname(path);
	}
	return ((char *)"");
}

char *
x_fn_append_filename_to_dir (int system_type, char *dir, const char *file)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_append_filename_to_dir(dir, file);
	case FN_VMS:	return  vms_fn_append_filename_to_dir(dir, file);
	case FN_DOS:	return  dos_fn_append_filename_to_dir(dir, file);
	}
	return ((char *)"");
}

char *
x_fn_append_dirname_to_dir (int system_type, char *dir, const char *subdir)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_append_dirname_to_dir(dir, subdir);
	case FN_VMS:	return  vms_fn_append_dirname_to_dir(dir, subdir);
	case FN_DOS:	return  dos_fn_append_dirname_to_dir(dir, subdir);
	}
	return ((char *)"");
}

char *
x_fn_cleanup_path (int system_type, char *path)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_cleanup_path(path);
	case FN_VMS:	return  vms_fn_cleanup_path(path);
	case FN_DOS:	return  dos_fn_cleanup_path(path);
	}
	return ((char *)"");
}

char *
x_fn_get_nth_dir (int system_type, const char *path, int n, char *dir)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_get_nth_dir(path, n, dir);
	case FN_VMS:	return  vms_fn_get_nth_dir(path, n, dir);
	case FN_DOS:	return  dos_fn_get_nth_dir(path, n, dir);
	}
	return ((char *)"");
}

int
x_fn_resolve_pathname (int system_type, char *path)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_resolve_pathname(path);
	case FN_VMS:	return  vms_fn_resolve_pathname(path);
	case FN_DOS:	return  dos_fn_resolve_pathname(path);
	}
	return (0);
}

char *
x_fn_disp_path (int system_type, const char *path, int len, char *fixed)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_disp_path(path, len, fixed);
	case FN_VMS:	return  vms_fn_disp_path(path, len, fixed);
	case FN_DOS:	return  dos_fn_disp_path(path, len, fixed);
	}
	return ((char *)"");
}

char *
x_fn_get_abs_dir (int system_type, const char *base, const char *rel_name,
	char *abs_name)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_get_abs_dir(base, rel_name, abs_name);
	case FN_VMS:	return  vms_fn_get_abs_dir(base, rel_name, abs_name);
	case FN_DOS:	return  dos_fn_get_abs_dir(base, rel_name, abs_name);
	}
	return ((char *)"");
}

char *
x_fn_get_abs_path (int system_type, const char *base, const char *rel_name,
	char *abs_name)
{
	switch (system_type)
	{
	case FN_UNIX:	return unix_fn_get_abs_path(base, rel_name, abs_name);
	case FN_VMS:	return  vms_fn_get_abs_path(base, rel_name, abs_name);
	case FN_DOS:	return  dos_fn_get_abs_path(base, rel_name, abs_name);
	}
	return ((char *)"");
}
