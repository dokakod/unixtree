/*------------------------------------------------------------------------
 * VMS filename functions
 */
#ifndef OSVFNAME_H
#define OSVFNAME_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int			vms_fn_is_type					(const char *path);
extern int			vms_fn_is_path_absolute			(const char *path);
extern int			vms_fn_is_path_a_dirname		(const char *path);
extern int			vms_fn_is_dir_in_dirname		(const char *dir,
														const char *dirname);
extern int			vms_fn_is_filename_hidden		(const char *dir,
														const char *filename);
extern int			vms_fn_bogus_filespec			(int c);
extern int			vms_fn_bogus_filename			(int c);
extern int			vms_fn_bogus_pathname			(int c);
extern const char *	vms_fn_all						(void);
extern const char *	vms_fn_rootname					(const char *path,
														char *root);
extern const char *	vms_fn_tmpname					(void);
extern const char *	vms_fn_cwdname					(void);
extern const char *	vms_fn_home						(void);
extern const char *	vms_fn_path_separator			(void);
extern const char *	vms_fn_null_path				(void);
extern char *		vms_fn_getcwd					(char *path);
extern char *		vms_fn_nodename					(char *path,
														char *node);
extern int			vms_fn_filename_index			(const char *path);
extern int			vms_fn_num_subdirs					(const char *path);
extern char *		vms_fn_basename					(const char *path);
extern char *		vms_fn_dirname					(const char *path,
														char *dir);
extern char *		vms_fn_terminate_dirname		(char *path);
extern char *		vms_fn_unterminate_dirname		(char *path);
extern char *		vms_fn_append_filename_to_dir	(char *dir,
														const char *file);
extern char *		vms_fn_append_dirname_to_dir	(char *dir,
														const char *subdir);
extern char *		vms_fn_cleanup_path				(char *path);
extern char *		vms_fn_get_nth_dir				(const char *path,
														int n,
														char *dir);
extern int			vms_fn_resolve_pathname			(char *path);
extern char *		vms_fn_disp_path				(const char *path,
														int len,
														char *fixed);
extern char *		vms_fn_get_abs_dir				(const char *base,
														const char *rel_name,
														char *abs_name);
extern char *		vms_fn_get_abs_path				(const char *base,
														const char *rel_name,
														char *abs_name);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* OSVFNAME_H */
