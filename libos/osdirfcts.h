/*------------------------------------------------------------------------
 * directory functions
 */
#ifndef OSDIRFCTS_H
#define OSDIRFCTS_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int   os_dir_make	(const char *path);
extern int   os_dir_delete	(const char *path);
extern int   os_dir_rename	(const char *oldpath, const char *newpath);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* OSDIRFCTS_H */
