/*------------------------------------------------------------------------
 * temp file functions
 */
#ifndef OSMKTMP_H
#define OSMKTMP_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern char *os_make_temp_name (char *path, const char *dir, const char *ext);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* OSMKTMP_H */
