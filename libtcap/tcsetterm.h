/*------------------------------------------------------------------------
 * term setup routines
 *
 * This header is public.
 */
#ifndef TCSETTERM_H
#define TCSETTERM_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern TERMINAL *	tcap_newterm		(const char *term,
											FILE *out_fp, FILE *inp_fp,
											TCAP_ERRS *ta);

extern TERMINAL *	tcap_newscreen		(const char *term,
											const char **defs,
											int   out_fd, int   inp_fd,
											TCAP_ERRS *ta);
extern void			tcap_delterminal	(TERMINAL *tp);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCSETTERM_H */
