/*------------------------------------------------------------------------
 * window size stuff
 */
#ifndef TERMWINSZ_H
#define TERMWINSZ_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * function prototypes
 */
extern int		term_window_check		(void);
extern int		term_window_size_get	(TERM_DATA *t, int *rows, int *cols);
extern int		term_window_size_set	(TERM_DATA *t, int  rows, int  cols);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TERMWINSZ_H */

