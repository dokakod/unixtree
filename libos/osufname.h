/*------------------------------------------------------------------------
 * UNIX filename functions
 */
#ifndef OSUFNAME_H
#define OSUFNAME_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int			unix_fn_is_type					(const char *path);
extern int			unix_fn_is_path_absolute		(const char *path);
extern int			unix_fn_is_path_a_dirname		(const char *path);
extern int			unix_fn_is_dir_in_dirname		(const char *dir,
														const char *dirname);
extern int			unix_fn_is_filename_hidden		(const char *dir,
														const char *filename);
extern int			unix_fn_bogus_filespec			(int c);
extern int			unix_fn_bogus_filename			(int c);
extern int			unix_fn_bogus_pathname			(int c);
extern const char *	unix_fn_all						(void);
extern const char *	unix_fn_rootname				(const char *path,
														char *root);
extern const char *	unix_fn_tmpname					(void);
extern const char *	unix_fn_cwdname					(void);
extern const char *	unix_fn_home					(void);
extern const char *	unix_fn_path_separator			(void);
extern const char *	unix_fn_null_path				(void);
extern char *		unix_fn_getcwd					(char *path);
extern char *		unix_fn_nodename				(char *path,
														char *node);
extern int			unix_fn_filename_index			(const char *path);
extern int			unix_fn_num_subdirs				(const char *path);
extern char *		unix_fn_basename				(const char *path);
extern char *		unix_fn_dirname					(const char *path,
														char *dir);
extern char *		unix_fn_terminate_dirname		(char *path);
extern char *		unix_fn_unterminate_dirname		(char *path);
extern char *		unix_fn_append_filename_to_dir	(char *dir,
														const char *file);
extern char *		unix_fn_append_dirname_to_dir	(char *dir,
														const char *subdir);
extern char *		unix_fn_cleanup_path			(char *path);
extern char *		unix_fn_get_nth_dir				(const char *path,
														int n,
														char *dir);
extern int			unix_fn_resolve_pathname		(char *path);
extern char *		unix_fn_disp_path				(const char *path,
														int len,
														char *fixed);
extern char *		unix_fn_get_abs_dir				(const char *base,
														const char *rel_name,
														char *abs_name);
extern char *		unix_fn_get_abs_path			(const char *base,
														const char *rel_name,
														char *abs_name);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* OSUFNAME_H */
