/*------------------------------------------------------------------------
 * routines to find a file in a path
 */
#ifndef OSPATH_H
#define OSPATH_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern char *	os_get_path	(const char *filename, const char *path,
										char *fullpath);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* OSPATH_H */
