/*------------------------------------------------------------------------
 * header for device output processing
 *
 * This header is public.
 */
#ifndef TCDEVOUT_H
#define TCDEVOUT_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int		tcap_outflush		(TERMINAL *tp);
extern int		tcap_outch			(TERMINAL *tp, int c, int data);
extern int		tcap_outchar		(TERMINAL *tp, int c);
extern int		tcap_outcmd			(TERMINAL *tp, SCRN_CMD cmd,
										const char *s);
extern int		tcap_outparm		(TERMINAL *tp, SCRN_CMD cmd,
										int p1, int p2);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCDEVOUT_H */
