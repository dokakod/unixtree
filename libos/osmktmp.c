/*------------------------------------------------------------------------
 * make a temp file
 */
#include "oscommon.h"

char *os_make_temp_name (char *path, const char *dir, const char *ext)
{
	if (dir && *dir)
		strcpy(path, dir);
	else
		strcpy(path, fn_tmpname());

	fn_append_filename_to_dir(path, "fooXXXXXX");

#if V_UNIX
	mkstemp(path);
#else
	mktemp(path);
#endif

	if (ext != 0 && *ext != 0)
		fn_set_ext(path, ext);

	return (path);
}
