/*------------------------------------------------------------------------
 * header for path processing
 *
 * This header is private to this library only.
 */
#ifndef TCFNDPTH_H
#define TCFNDPTH_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int		tcap_path_exists	(const char *path);

extern int		tcap_find_path		(const char *filename, const char *path,
										char *fullpath);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCFNDPTH_H */
