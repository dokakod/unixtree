/*------------------------------------------------------------------------
 * header for screen-blank processing
 *
 * This header is public.
 */
#ifndef TCBLNKSCR_H
#define TCBLNKSCR_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int	tcap_blank_screen	(TERMINAL *tp, const char **logo);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCBLNKSCR_H */
