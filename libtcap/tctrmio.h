/*------------------------------------------------------------------------
 * header for compiled term-file processing
 *
 * This header is public.
 */
#ifndef TCTRMIO_H
#define TCTRMIO_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int		tcap_get_trc		(TERMINAL *tp, const char *path);
extern int		tcap_put_trc		(const TERMINAL *tp, const char *path);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCTRMIO_H */
