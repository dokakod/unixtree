/*------------------------------------------------------------------------
 * generic filename functions
 */
#ifndef OSXFNAME_H
#define OSXFNAME_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int			x_fn_is_type					(int type,
														const char *path);
extern int			x_fn_is_path_absolute			(int type,
														const char *path);
extern int			x_fn_is_path_a_dirname			(int type,
														const char *path);
extern int			x_fn_is_dir_in_dirname			(int type,
														const char *dir,
														const char *dirname);
extern int			x_fn_is_filename_hidden			(int type,
														const char *dir,
														const char *filename);
extern int			x_fn_bogus_filespec				(int type,
														int c);
extern int			x_fn_bogus_filename				(int type,
														int c);
extern int			x_fn_bogus_pathname				(int type,
														int c);
extern const char *	x_fn_all						(int type);
extern const char *	x_fn_rootname					(int type,
														const char *path,
														char *root);
extern const char *	x_fn_tmpname					(int type);
extern const char *	x_fn_cwdname					(int type);
extern const char *	x_fn_home						(int type);
extern const char *	x_fn_path_separator				(int type);
extern const char *	x_fn_null_path					(int type);
extern char *		x_fn_getcwd						(int type,
														char *path);
extern char *		x_fn_nodename					(int type,
														char *path,
														char *node);
extern int			x_fn_filename_index				(int type,
														const char *path);
extern int			x_fn_num_subdirs				(int type,
														const char *path);
extern char *		x_fn_basename					(int type,
														const char *path);
extern char *		x_fn_dirname					(int type,
														const char *path,
														char *dir);
extern char *		x_fn_terminate_dirname			(int type,
														char *path);
extern char *		x_fn_unterminate_dirname		(int type,
														char *path);
extern char *		x_fn_append_filename_to_dir		(int type,
														char *dir,
														const char *file);
extern char *		x_fn_append_dirname_to_dir		(int type,
														char *dir,
														const char *subdir);
extern char *		x_fn_cleanup_path				(int type,
														char *path);
extern char *		x_fn_get_nth_dir				(int type,
														const char *path,
														int n,
														char *dir);
extern int			x_fn_resolve_pathname			(int type,
														char *path);
extern char *		x_fn_disp_path					(int type,
														const char *path,
														int len,
														char *fixed);
extern char *		x_fn_get_abs_dir				(int type,
														const char *base,
														const char *rel_name,
														char *abs_name);
extern char *		x_fn_get_abs_path				(int type,
														const char *base,
														const char *rel_name,
														char *abs_name);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* OSXFNAME_H */
