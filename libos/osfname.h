/*------------------------------------------------------------------------
 * filename functions
 */
#ifndef OSFNAME_H
#define OSFNAME_H

#include "osdfname.h"
#include "osufname.h"
#include "osvfname.h"
#include "osxfname.h"

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * system types used
 */
#define FN_UNIX			1
#define FN_DOS			2
#define FN_VMS			3

#if V_UNIX
#define FN_SYSTEM		FN_UNIX
#endif

#if V_WINDOWS
#define FN_SYSTEM		FN_DOS
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern char *		fn_convert_dirname				(const char *path,
														int from,
														int to,
														char *new_path);
extern char *		fn_convert_filename				(const char *path,
														int from,
														int to,
														char *new_path);

extern char *		fn_ext							(const char *path);
extern char *		fn_rem_ext						(char *path);
extern char *		fn_set_ext						(char *path,
														const char *ext);
extern int			fn_pathtype						(const char *path);

extern int			fn_is_type						(const char *path);
extern int			fn_is_path_absolute				(const char *path);
extern int			fn_is_path_a_dirname			(const char *path);
extern int			fn_is_dir_in_dirname			(const char *dir,
														const char *dirname);
extern int			fn_is_filename_hidden			(const char *dir,
														const char *filename);
extern int			fn_bogus_filespec				(int c);
extern int			fn_bogus_filename				(int c);
extern int			fn_bogus_pathname				(int c);
extern const char *	fn_all							(void);
extern const char *	fn_rootname						(const char *path,
														char *root);
extern const char *	fn_tmpname						(void);
extern const char *	fn_cwdname						(void);
extern const char *	fn_parent						(void);
extern const char *	fn_home							(void);
extern const char *	fn_path_separator				(void);
extern const char *	fn_null_path					(void);
extern char *		fn_getcwd						(char *path);
extern char *		fn_nodename						(char *path,
														char *node);
extern int			fn_filename_index				(const char *path);
extern int			fn_num_subdirs					(const char *path);
extern char *		fn_basename						(const char *path);
extern char *		fn_dirname						(const char *path,
														char *dir);
extern char *		fn_terminate_dirname			(char *path);
extern char *		fn_unterminate_dirname			(char *path);
extern char *		fn_append_filename_to_dir		(char *dir,
														const char *file);
extern char *		fn_append_dirname_to_dir		(char *dir,
														const char *subdir);
extern char *		fn_cleanup_path					(char *path);
extern char *		fn_get_nth_dir					(const char *path,
														int n,
														char *dir);
extern int			fn_resolve_pathname				(char *path);
extern char *		fn_disp_path					(const char *path,
														int len,
														char *fixed);
extern char *		fn_get_abs_dir					(const char *base,
														const char *rel_name,
														char *abs_name);
extern char *		fn_get_abs_path					(const char *base,
														const char *rel_name,
														char *abs_name);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* OSFNAME_H */
