/*------------------------------------------------------------------------
 * terminal mode stuff
 */
#ifndef TERMMODE_H
#define TERMMODE_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * function protptypes
 */
extern int		term_get_orig_mode		(TERM_DATA *t);
extern int		term_get_prog_mode		(TERM_DATA *t);
extern int		term_get_save_mode		(TERM_DATA *t);

extern int		term_set_orig_mode		(TERM_DATA *t);
extern int		term_set_prog_mode		(TERM_DATA *t);
extern int		term_set_save_mode		(TERM_DATA *t);

extern int		term_flush_inp			(TERM_DATA *t);
extern int		term_flush_out			(TERM_DATA *t);

extern int		term_get_baud_rate		(const TERM_DATA *t);
extern int		term_get_kill_char		(const TERM_DATA *t);
extern int		term_get_quit_char		(const TERM_DATA *t);
extern int		term_get_intr_char		(const TERM_DATA *t);
extern int		term_get_erase_char		(const TERM_DATA *t);
extern int		term_get_wait			(const TERM_DATA *t);

extern int		term_set_baud_rate		(TERM_DATA *t, int br);
extern int		term_set_kill_char		(TERM_DATA *t, int ch);
extern int		term_set_quit_char		(TERM_DATA *t, int ch);
extern int		term_set_intr_char		(TERM_DATA *t, int ch);
extern int		term_set_erase_char		(TERM_DATA *t, int ch);
extern int		term_set_wait			(TERM_DATA *t, int bf);

extern int		term_set_inp_nl			(TERM_DATA *t, int bf);
extern int		term_set_out_nl			(TERM_DATA *t, int bf);
extern int		term_set_raw			(TERM_DATA *t, int bf);
extern int		term_set_cbreak			(TERM_DATA *t, int bf);
extern int		term_set_echo			(TERM_DATA *t, int bf);
extern int		term_set_meta			(TERM_DATA *t, int bf);
extern int		term_set_qiflush		(TERM_DATA *t, int bf);

extern int		term_get_read_mode		(const TERM_DATA *t);
extern int		term_set_read_mode		(TERM_DATA *t, int interval);

/*------------------------------------------------------------------------
 * Windows functions
 */
#if V_WINDOWS
extern int		term_win_set_cursor		(TERM_DATA *t, int visibility);
extern int		term_win_set_mouse		(TERM_DATA *t, int bf);
extern int		term_win_set_position	(TERM_DATA *t, int y, int x);
extern int		term_win_set_color		(TERM_DATA *t, int  fg, int  bg);
extern int		term_win_get_color		(TERM_DATA *t, int *fg, int *bg);
extern int		term_win_set_title		(TERM_DATA *t, const char *str);
#endif

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TERMMODE_H */
