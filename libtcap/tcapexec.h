/*------------------------------------------------------------------------
 * header for exec processing
 *
 * This header is public.
 */
#ifndef TCAPEXEC_H
#define TCAPEXEC_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int	tcap_exec_argv	(TERMINAL *tp, const char *dir, char **argv,
								int interactive, char *msgbuf);

extern int	tcap_exec_cmd	(TERMINAL *tp, const char *dir, const char *cmd,
								int interactive, char *msgbuf);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCAPEXEC_H */
