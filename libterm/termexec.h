/*------------------------------------------------------------------------
 * header for exec routines
 */
#ifndef TERMEXEC_H
#define TERMEXEC_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * function prototypes
 */
extern int		term_exec_argv   (TERM_DATA *t,
									const char *dir, char **argv,
									int interactive, char *msgbuf);

extern int		term_exec_cmd    (TERM_DATA *t,
									const char *dir, const char *cmd,
									int interactive, char *msgbuf);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TERMEXEC_H */
