/*------------------------------------------------------------------------
 * header for ssort
 */
#ifndef UTSSORT_H
#define UTSSORT_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int	ssort	(char *base, int nel, int width,
						int (*cmp)(const void *p1, const void *p2));

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* UTSSORT_H */
