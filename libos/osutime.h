/*------------------------------------------------------------------------
 * file time functions
 */
#ifndef OSUTIME_H
#define OSUTIME_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int   os_set_file_time	(const char *path,
									time_t actime, time_t modtime);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* OSUTIME_H */
