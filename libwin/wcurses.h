/*------------------------------------------------------------------------
 * internal curses header
 */
#ifndef WCURSES_H
#define WCURSES_H

#ifdef NOMACROS
#undef NOMACROS
#endif

#ifdef COMPILING_MACROS
#define NOMACROS	1
#endif

#include "libtcap.h"
#include "wscreen.h"
#include "ecurses.h"
#include "wblklets.h"

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 *	misc definitions
 */
#define WIN_MAGIC	0x03131980		/* magic number for putwin() & getwin() */

/*------------------------------------------------------------------------
 * internal global variables
 */
struct win_data
{
	/*--------------------------------------------------------------------
	 * This is global data which is copied into the next screen
	 * that is created & cleared out for the next screen.
	 */
	int				slk_fmt;			/* slk format					*/
	RIPOFF			ripoff_tbl;			/* global ripoff table			*/

	/*--------------------------------------------------------------------
	 * screen list stuff
	 */
	SCRLIST *		screens;			/* list of screens				*/
	THREAD_MUTEX *	screen_cs;			/* ptr to mutex for screen list	*/

	/*--------------------------------------------------------------------
	 * current screen & terminal pointer
	 */
	SCREEN *		cur_screen;			/* current screen ptr			*/
	TERMINAL *		cur_term;			/* current terminal ptr			*/
};
typedef struct win_data WIN_DATA;

extern WIN_DATA	win_data;

#define win_slk_fmt			(win_data.slk_fmt)
#define win_ripoff_tbl		(win_data.ripoff_tbl)

#define win_screens			(win_data.screens)
#define win_screen_cs		(win_data.screen_cs)

#define win_cur_screen		(win_data.cur_screen)
#define win_cur_term		(win_data.cur_term)

/*------------------------------------------------------------------------
 * win->screen-data macros
 */
#define WIN_SCREEN(w)		((w)->_screen)
#define WIN_TERM(w)			SCR_TERM(WIN_SCREEN(w))
#define WIN_STDSCR(w)		SCR_STDSCR(WIN_SCREEN(w))
#define WIN_CURSCR(w)		SCR_CURSCR(WIN_SCREEN(w))
#define WIN_TRMSCR(w)		SCR_TRMSCR(WIN_SCREEN(w))

/*------------------------------------------------------------------------
 * misc internal curses functions
 */
extern WINDOW *	scrn_create_win		(SCREEN *s,
										int rows, int cols, int y, int x);
extern int		win_load			(WINDOW *win, FILE *fp);
extern void		win_clear			(WINDOW *win, chtype ch);
extern void		win_clock_show		(time_t clock, WINDOW *win);
extern void		win_clock_process	(time_t clock, void *data);
extern int		scrn_slk_init		(SCREEN *s, int layout_fmt);
extern int		scrn_slk_setup		(SCREEN *s);

/*------------------------------------------------------------------------
 * internal async key routines
 */
extern int		win_key_timer		(int key, void *data);
extern int		win_key_prtscrn		(int key, void *data);
extern int		win_key_repaint		(int key, void *data);
extern int		win_key_snap		(int key, void *data);

/*------------------------------------------------------------------------
 * internal curses mouse functions
 */
extern int		scrn_mouse_show_cursor	(SCREEN *s);
extern int		scrn_mouse_hide_cursor	(SCREEN *s);
extern int		scrn_mouse_draw_cursor	(SCREEN *s);

/*------------------------------------------------------------------------
 * internal tcap-interface mouse functions
 */
extern int		scrn_mi_get_prev_y		(const SCREEN *s);
extern int		scrn_mi_get_prev_x		(const SCREEN *s);
extern int		scrn_mi_get_prev_c		(const SCREEN *s);
extern attr_t	scrn_mi_get_ptr_attrs	(const SCREEN *s);
extern attr_t	scrn_mi_get_cmd_attrs	(const SCREEN *s);
extern int		scrn_mi_get_process		(const SCREEN *s);
extern int		scrn_mi_get_display		(const SCREEN *s);

extern int		scrn_mi_set_prev_y		(SCREEN *s, int y);
extern int		scrn_mi_set_prev_x		(SCREEN *s, int x);
extern int		scrn_mi_set_prev_c		(SCREEN *s, int c);
extern int		scrn_mi_set_ptr_attrs	(SCREEN *s, attr_t ptr_attrs);
extern int		scrn_mi_set_cmd_attrs	(SCREEN *s, attr_t cmd_attrs);
extern int		scrn_mi_set_process		(SCREEN *s, int process);
extern int		scrn_mi_set_display		(SCREEN *s, int display);

/*------------------------------------------------------------------------
 * internal tcap-interface functions
 */
extern int		scrn_cinit				(SCREEN *s);
extern int		scrn_cexit				(SCREEN *s, int exit_flag);

extern int		scrn_outattr			(SCREEN *s, attr_t a, int c);
extern int		scrn_outc				(SCREEN *s, int c);
extern int		scrn_outs				(SCREEN *s, const char *str);
extern int		scrn_outpos				(SCREEN *s, int row, int col);
extern int		scrn_outflush			(SCREEN *s);

extern int		scrn_clear_screen		(SCREEN *s);
extern int		scrn_need_ins_final		(const SCREEN *s);
extern int		scrn_write_final_char	(SCREEN *s,
										int cpc, attr_t cpa,
										int chc, attr_t cha);

extern int		scrn_check_window_size	(const SCREEN *s);
extern int		scrn_reset_window_size	(SCREEN *s, int rows, int cols);

extern int		scrn_getkey				(SCREEN *s, int mode, int interval);
extern int		scrn_chkkey				(SCREEN *s, int mode);
extern int		scrn_pushkey			(SCREEN *s, int key, int front);
extern int		scrn_get_timeout		(const SCREEN *s);
extern int		scrn_set_timeout		(SCREEN *s, int bf);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* WCURSES_H */
