/*------------------------------------------------------------------------
 * functions to get stat info
 */
#ifndef OSSTATINFO_H
#define OSSTATINFO_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int		os_get_major	(struct stat *s);
extern int		os_get_minor	(struct stat *s);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* OSSTATINFO_H */
