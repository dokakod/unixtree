/*------------------------------------------------------------------------
 * header for signal processing
 */
#ifndef TCSIG_H
#define TCSIG_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern void			tcap_sig_init		(TERMINAL *tp);
extern void			tcap_sig_end		(TERMINAL *tp);
extern TERMINAL *	tcap_sig_sp			(void);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCSIG_H */
