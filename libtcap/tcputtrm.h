/*------------------------------------------------------------------------
 * header for save term-file processing
 *
 * This header is public.
 */
#ifndef TCPUTTRM_H
#define TCPUTTRM_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int	tcap_put_trm		(const TERMINAL *tp, int all,
									const char *path);

extern int	tcap_put_trm_rtn	(const TERMINAL *tp, int all,
									void (*rtn)(void *data, const char *line),
									void *data);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCPUTTRM_H */
