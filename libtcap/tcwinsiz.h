/*------------------------------------------------------------------------
 * header for window size processing
 *
 * This header is public.
 */
#ifndef TCWINSIZ_H
#define TCWINSIZ_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int		tcap_winsize_check		(const TERMINAL *tp);
extern int		tcap_winsize_reset		(TERMINAL *tp, int rows, int cols);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCWINSIZ_H */
