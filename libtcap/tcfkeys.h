/*------------------------------------------------------------------------
 * header file for function key processing
 *
 * This header is private to this library only.
 */
#ifndef TCFKEYS_H
#define TCFKEYS_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * KFUNC (function-key list) routines
 */
extern int		tcap_kf_addkey			(TCAP_DATA *td,
											const char *str, int code);
extern int		tcap_kf_delkey			(TCAP_DATA *td, int code);
extern KFUNC *	tcap_kf_new				(void);
extern void		tcap_kf_free			(TCAP_DATA *td);

/*------------------------------------------------------------------------
 * FKEY (function-key tree) routines
 */
extern int		tcap_fk_add_dumb_keys	(TCAP_DATA *td);
extern int		tcap_fk_addkey			(TCAP_DATA *td, const char *string,
											int code, int debug);
extern int		tcap_fk_find			(const TCAP_DATA *td,
											const unsigned char *s);
extern int		tcap_fk_setup			(TCAP_DATA *td);
extern void		tcap_fk_free			(TCAP_DATA *td);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCFKEYS_H */
