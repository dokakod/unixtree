/*------------------------------------------------------------------------
 * OS readlink mechanism
 */
#ifndef OSRDLINK_H
#define OSRDLINK_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int	os_win_readlink	(const char *linkname,       char *pathname);
extern int	os_win_makelink	(const char *linkname, const char *pathname);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* OSRDLINK_H */
