/*------------------------------------------------------------------------
 * header for sound processing
 *
 * This header is public.
 */
#ifndef TCBEEP_H
#define TCBEEP_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int	tcap_beep		(TERMINAL *tp);
extern int	tcap_flash		(TERMINAL *tp);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCBEEP_H */
