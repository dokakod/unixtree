/*------------------------------------------------------------------------
 * tty name stuff
 */
#ifndef TERMNAME_H
#define TERMNAME_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * function prototypes
 */
extern const char *	term_get_tty_name		(const TERM_DATA *t);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TERMNAME_H */
