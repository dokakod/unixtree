/*------------------------------------------------------------------------
 * header for async key handling
 *
 * This header is private to this library only.
 */
#ifndef TCASYNC_H
#define TCASYNC_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int	tcap_async_add		(SCRN_DATA *sd, int key,
									ASYNC_RTN *rtn, void *data);
extern int	tcap_async_check	(const SCRN_DATA *sd, int key);
extern int	tcap_async_process	(const SCRN_DATA *sd, int key);
extern void	tcap_async_free		(SCRN_DATA *sd);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCASYNC_H */
