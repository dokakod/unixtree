/*------------------------------------------------------------------------
 * header for compress/uncompress routines
 */
#ifndef COMPRESS_H
#define COMPRESS_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int comp			(const char *path_inp, const char *path_out);
extern int decomp		(const char *path_inp, const char *path_out);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* COMPRESS_H */
