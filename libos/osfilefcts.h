/*------------------------------------------------------------------------
 * file functions
 */
#ifndef OSFILEFCTS_H
#define OSFILEFCTS_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int   os_file_delete		(const char *path);
extern int   os_file_rename		(const char *oldpath, const char *newpath);
extern int   os_file_copy		(const char *oldpath, const char *newpath);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* OSFILEFCTS_H */
