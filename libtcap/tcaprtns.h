/*------------------------------------------------------------------------
 *	Basic termcap header file
 *
 * This header is public.
 */
#ifndef TCAPRTNS_H
#define TCAPRTNS_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * function declarations
 */
extern int			tcap_set_inp_nl		(TERMINAL *tp, int bf);
extern int			tcap_set_out_nl		(TERMINAL *tp, int bf);
extern int			tcap_set_raw		(TERMINAL *tp, int bf);
extern int			tcap_set_cbreak		(TERMINAL *tp, int bf);
extern int			tcap_set_echo		(TERMINAL *tp, int bf);
extern int			tcap_set_qiflush	(TERMINAL *tp, int bf);
extern int			tcap_set_meta		(TERMINAL *tp, int bf);
extern int			tcap_napms			(int ms);

extern int			tcap_get_curx		(const TERMINAL *tp);
extern int			tcap_get_cury		(const TERMINAL *tp);
extern int			tcap_set_curx		(TERMINAL *tp, int x);
extern int			tcap_set_cury		(TERMINAL *tp, int y);

extern void			tcap_set_use_env	(int bf);
extern int			tcap_get_use_env	(void);

extern void			tcap_set_term_path	(const char *path);
extern const char *	tcap_get_term_path	(void);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCAPRTNS_H */
