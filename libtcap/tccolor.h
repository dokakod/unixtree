/*------------------------------------------------------------------------
 * "standard" color processing functions.
 *
 * Note that these routines do not need to be used in "ecurses" to
 * access color capability.  They are provided for compatability
 * with "standard" curses.
 *
 * This header is public.
 */
#ifndef TCCOLOR_H
#define TCCOLOR_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * function prototypes
 */
extern int		tcap_has_colors			(const TERMINAL *tp);
extern int		tcap_start_color		(TERMINAL *tp);

extern int		tcap_init_pair			(TERMINAL *tp,
											short pair, short  f, short  b);
extern int		tcap_pair_content		(const TERMINAL *tp,
											short pair, short *f, short *b);

extern int		tcap_init_all_pairs		(TERMINAL *tp);

extern int		tcap_color_content		(const TERMINAL *tp, short color,
											short *r, short *g, short *b);
extern int		tcap_init_color			(TERMINAL *tp, short color,
											short  r, short  g, short  b);

extern int		tcap_can_change_color	(const TERMINAL *tp);
extern int		tcap_pair_number		(const TERMINAL *tp, attr_t a);
extern attr_t	tcap_color_pair			(const TERMINAL *tp, int n);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCCOLOR_H */
