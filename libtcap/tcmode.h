/*------------------------------------------------------------------------
 * mode functions
 *
 * This header is public.
 */
#ifndef TCMODE_H
#define TCMODE_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int	tcap_get_baud_rate	(const TERMINAL *tp);
extern int	tcap_get_kill_char	(const TERMINAL *tp);
extern int	tcap_get_quit_char	(const TERMINAL *tp);
extern int	tcap_get_intr_char	(const TERMINAL *tp);
extern int	tcap_get_erase_char	(const TERMINAL *tp);
extern int	tcap_get_wait		(const TERMINAL *tp);

extern int	tcap_set_baud_rate	(TERMINAL *tp, int br);
extern int	tcap_set_kill_char	(TERMINAL *tp, int ch);
extern int	tcap_set_quit_char	(TERMINAL *tp, int ch);
extern int	tcap_set_intr_char	(TERMINAL *tp, int ch);
extern int	tcap_set_erase_char	(TERMINAL *tp, int ch);
extern int	tcap_set_wait		(TERMINAL *tp, int bf);

extern int	tcap_flush_inp		(TERMINAL *tp);
extern int	tcap_flush_out		(TERMINAL *tp);

extern int	tcap_set_typeahead	(TERMINAL *tp, int fildes);
extern int	tcap_set_halfdelay	(TERMINAL *tp, int tenths);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCMODE_H */
