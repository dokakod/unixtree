/*------------------------------------------------------------------------
 * Basic mouse header file
 *
 * This header is public.
 */
#ifndef TCMOUSE_H
#define TCMOUSE_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * function prototypes
 */
extern int		tcap_mouse_init				(TERMINAL *tp);
extern int		tcap_mouse_end				(TERMINAL *tp);

extern int		tcap_mouse_show				(TERMINAL *tp);
extern int		tcap_mouse_hide				(TERMINAL *tp);

extern int		tcap_mouse_get_is_on		(const TERMINAL *tp);
extern int		tcap_mouse_get_display		(const TERMINAL *tp);
extern int		tcap_mouse_invalidate		(TERMINAL *tp);
extern int		tcap_mouse_set_intvl		(TERMINAL *tp, int ms);
extern int		tcap_mouse_get_event		(const TERMINAL *tp);
extern int		tcap_mouse_set_event		(TERMINAL *tp, int event);
extern int		tcap_mouse_get_x			(const TERMINAL *tp);
extern int		tcap_mouse_get_y			(const TERMINAL *tp);
extern int		tcap_mouse_get_buttons		(const TERMINAL *tp);
extern int		tcap_mouse_get_still_down	(const TERMINAL *tp);

/*------------------------------------------------------------------------
 * low-level mouse routines
 */
extern int		tcap_mouse_check_btn		(TERMINAL *tp, int button);
extern int		tcap_mouse_get_input		(TERMINAL *tp,
												const unsigned char *input);
extern int		tcap_mouse_deq_event		(TERMINAL *tp);
extern int		tcap_mouse_check_position	(TERMINAL *tp);
extern int		tcap_mouse_set_button		(TERMINAL *tp, int but, int pos);

/*------------------------------------------------------------------------
 * routines to simulate mouse actions
 */
extern int		tcap_mouse_move_up			(TERMINAL *tp);
extern int		tcap_mouse_move_down		(TERMINAL *tp);
extern int		tcap_mouse_move_left		(TERMINAL *tp);
extern int		tcap_mouse_move_right		(TERMINAL *tp);
extern int		tcap_mouse_move_ul			(TERMINAL *tp);
extern int		tcap_mouse_move_ur			(TERMINAL *tp);
extern int		tcap_mouse_move_dl			(TERMINAL *tp);
extern int		tcap_mouse_move_dr			(TERMINAL *tp);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCMOUSE_H */
