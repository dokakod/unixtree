/*------------------------------------------------------------------------
 * header for date-convert routines
 */
#ifndef DATECVT_H
#define DATECVT_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern time_t datecvt (int yr, int mo, int dy, int hh, int mm, int ss);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* DATECVT_H */
