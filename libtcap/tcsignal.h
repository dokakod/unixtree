/*------------------------------------------------------------------------
 * header for signal processing
 *
 * This header is public.
 */
#ifndef TCSIGNAL_H
#define TCSIGNAL_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern void			tcap_signal_set_rtn		(TERMINAL *tp, int sig,
												SIG_RTN *rtn, void *data);

extern void			tcap_signal_reset		(TERMINAL *tp, int reset);

extern SIG_RTN *	tcap_signal_get_rtn		(const TERMINAL *tp, int sig);
extern void *		tcap_signal_get_data	(const TERMINAL *tp, int sig);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCSIGNAL_H */
