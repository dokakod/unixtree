/*------------------------------------------------------------------------
 * header for term mode processing
 *
 * This header is public.
 */
#ifndef TCCINIT_H
#define TCCINIT_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int		tcap_get_prog_mode		(TERMINAL *tp);
extern int		tcap_get_orig_mode		(TERMINAL *tp);
extern int		tcap_get_save_mode		(TERMINAL *tp);

extern int		tcap_set_prog_mode		(TERMINAL *tp);
extern int		tcap_set_orig_mode		(TERMINAL *tp);
extern int		tcap_set_save_mode		(TERMINAL *tp);

extern int		tcap_cexit				(TERMINAL *tp, int exit_flag);
extern int		tcap_cinit				(TERMINAL *tp);
extern int		tcap_exec_shell			(TERMINAL *tp);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCCINIT_H */
