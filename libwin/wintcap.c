/*------------------------------------------------------------------------
 * curses interfaces
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * curses kernel routines (ecurs_kernel)
 */
int curs_set (int visibility)
{
	return scrn_curs_set(win_cur_screen, visibility);
}

int set_syx (int y, int x)
{
	return scrn_set_syx(win_cur_screen, y, x);
}

int get_syx (int *y, int *x)
{
	return scrn_get_syx(win_cur_screen, y, x);
}

int def_shell_mode (void)
{
	return scrn_def_shell_mode(win_cur_screen);
}

int reset_shell_mode (void)
{
	return scrn_reset_shell_mode(win_cur_screen);
}

int def_prog_mode (void)
{
	return scrn_def_prog_mode(win_cur_screen);
}

int reset_prog_mode (void)
{
	return scrn_reset_prog_mode(win_cur_screen);
}

int savetty (void)
{
	return scrn_savetty(win_cur_screen);
}

int resetty (void)
{
	return scrn_resetty(win_cur_screen);
}

/*------------------------------------------------------------------------
 * alarm functions (ecurs_beep)
 */
int beep (void)
{
	return scrn_beep(win_cur_screen);
}

int flash (void)
{
	return scrn_flash(win_cur_screen);
}

/*------------------------------------------------------------------------
 * input options (ecurs_inopts)
 */
int nlinp (void)
{
	return scrn_nlinp(win_cur_screen);
}

int nonlinp (void)
{
	return scrn_nonlinp(win_cur_screen);
}

int raw (void)
{
	return scrn_raw(win_cur_screen);
}

int noraw (void)
{
	return scrn_noraw(win_cur_screen);
}

int cbreak (void)
{
	return scrn_cbreak(win_cur_screen);
}

int nocbreak (void)
{
	return scrn_nocbreak(win_cur_screen);
}

int echo (void)
{
	return scrn_echo(win_cur_screen);
}

int noecho (void)
{
	return scrn_noecho(win_cur_screen);
}

int metaon (void)
{
	return scrn_metaon(win_cur_screen);
}

int metaoff (void)
{
	return scrn_metaoff(win_cur_screen);
}

int qiflush (void)
{
	return scrn_qiflush(win_cur_screen);
}

int noqiflush (void)
{
	return scrn_noqiflush(win_cur_screen);
}

int typeahead (int fildes)
{
	return scrn_typeahead(win_cur_screen, fildes);
}

int halfdelay (int tenths)
{
	return scrn_halfdelay(win_cur_screen, tenths);
}

int addkey (const char *string, int code)
{
	return scrn_addkey(win_cur_screen, string, code);
}

/*------------------------------------------------------------------------
 * output options (ecurs_outopts)
 */
int nl (void)
{
	return scrn_nl(win_cur_screen);
}

int nonl (void)
{
	return scrn_nonl(win_cur_screen);
}

int nlout (void)
{
	return scrn_nlout(win_cur_screen);
}

int nonlout (void)
{
	return scrn_nonlout(win_cur_screen);
}

int get_valid_acs (int c)
{
	return scrn_get_valid_acs(win_cur_screen, c);
}

int set_valid_acs (int bf)
{
	return scrn_set_valid_acs(win_cur_screen, bf);
}

/*------------------------------------------------------------------------
 * terminal attribute functions (ecurs_termattrs)
 */
int baudrate (void)
{
	return scrn_baudrate(win_cur_screen);
}

int killchar (void)
{
	return scrn_killchar(win_cur_screen);
}

int erasechar (void)
{
	return scrn_erasechar(win_cur_screen);
}

int quitchar (void)
{
	return scrn_quitchar(win_cur_screen);
}

int intrchar (void)
{
	return scrn_intrchar(win_cur_screen);
}

int has_ic (void)
{
	return scrn_has_ic(win_cur_screen);
}

int has_il (void)
{
	return scrn_has_il(win_cur_screen);
}

int get_window_env (void)
{
	return scrn_get_window_env(win_cur_screen);
}

attr_t termattrs (void)
{
	return scrn_termattrs(win_cur_screen);
}

const char * termname (void)
{
	return scrn_termname(win_cur_screen);
}

const char * longname (void)
{
	return scrn_longname(win_cur_screen);
}

const char * termtype (void)
{
	return scrn_termtype(win_cur_screen);
}

const char * get_ttyname (void)
{
	return scrn_get_ttyname(win_cur_screen);
}

int get_curr_row (void)
{
	return scrn_get_curr_row(win_cur_screen);
}

int get_curr_col (void)
{
	return scrn_get_curr_col(win_cur_screen);
}

int get_max_cols (void)
{
	return scrn_get_max_cols(win_cur_screen);
}

int get_max_rows (void)
{
	return scrn_get_max_rows(win_cur_screen);
}

int get_tab_size (void)
{
	return scrn_get_tab_size(win_cur_screen);
}

attr_t get_orig_attr (void)
{
	return scrn_get_orig_attr(win_cur_screen);
}

attr_t get_curr_attr (void)
{
	return scrn_get_curr_attr(win_cur_screen);
}

/*------------------------------------------------------------------------
 * utility functions (ecurs_util)
 */
int flushinp (void)
{
	return scrn_flushinp(win_cur_screen);
}

int delay_output (int ms)
{
	return scrn_delay_output(win_cur_screen, ms);
}

int filter (void)
{
	return scrn_filter(win_cur_screen);
}

/*------------------------------------------------------------------------
 * color functions (ecurs_color)
 */
int start_color (void)
{
	return scrn_start_color(win_cur_screen);
}

int has_colors (void)
{
	return scrn_has_colors(win_cur_screen);
}

int init_pair (short pair, short  f, short  b)
{
	return scrn_init_pair(win_cur_screen, pair, f, b);
}

int pair_content (short pair, short *f, short *b)
{
	return scrn_pair_content(win_cur_screen, pair, f, b);
}

int init_all_pairs (void)
{
	return scrn_init_all_pairs(win_cur_screen);
}

int color_content (short color, short *r, short *g, short *b)
{
	return scrn_color_content(win_cur_screen, color, r, g, b);
}

int init_color (short color, short  r, short  g, short  b)
{
	return scrn_init_color(win_cur_screen, color, r, g, b);
}

int can_change_color (void)
{
	return scrn_can_change_color(win_cur_screen);
}

int PAIR_NUMBER (attr_t a)
{
	return scrn_pair_number(win_cur_screen, a);
}

attr_t COLOR_PAIR (int n)
{
	return scrn_color_pair(win_cur_screen, n);
}

/*------------------------------------------------------------------------
 * cmd-exec functions (ecurs_exec)
 */
int exec_argv (const char *dir, char **argv, int interactive,
	char *msgbuf)
{
	return scrn_exec_argv(win_cur_screen, dir, argv, interactive, msgbuf);
}

int exec_cmd (const char *dir, const char *cmd, int interactive,
	char *msgbuf)
{
	return scrn_exec_cmd(win_cur_screen, dir, cmd, interactive, msgbuf);
}

/*------------------------------------------------------------------------
 * mouse functions (ecurs_mouse)
 */
int mouse_init (WINDOW *win)
{
	if (win == 0)
		return (ERR);

	return scrn_mouse_init(WIN_SCREEN(win));
}

int mouse_get_display (WINDOW *win)
{
	if (win == 0)
		return (ERR);

	return scrn_mouse_get_display(WIN_SCREEN(win));
}

int mouse_get_event (WINDOW *win)
{
	if (win == 0)
		return (ERR);

	return scrn_mouse_get_event(WIN_SCREEN(win));
}

int mouse_x (WINDOW *win)
{
	if (win == 0)
		return (ERR);

	return scrn_mouse_x(WIN_SCREEN(win));
}

int mouse_y (WINDOW *win)
{
	if (win == 0)
		return (ERR);

	return scrn_mouse_y(WIN_SCREEN(win));
}

int mouse_c (WINDOW *win)
{
	int		x;
	int		y;
	int		c;
	chtype	ch;

	if (win == 0)
		return (ERR);
	win = WIN_CURSCR(win);

	x = mouse_x(win);
	y = mouse_y(win);

	if (x < 0 || x >= getmaxx(win))
		return (ERR);

	if (y < 0 || y >= getmaxy(win))
		return (ERR);

	ch = wchat(win, y, x);
	c = A_GETCODE(ch);

	return (c);
}

/*------------------------------------------------------------------------
 * mono/color attribute functions (ecurs_attributes)
 */
int get_default_attrs (attr_t *pr, attr_t *ps, int use_color, int use_win)
{
	return scrn_get_default_attrs(win_cur_screen, pr, ps, use_color, use_win);
}

/*------------------------------------------------------------------------
 * signal routines (ecurs_signal)
 */
void set_signal_rtn (int sig, WIN_SIG_RTN *rtn, void *data)
{
	sys_sig_set(sig, rtn, data);
}

WIN_SIG_RTN * get_signal_rtn (int sig)
{
	return sys_sig_get_rtn(sig);
}

void * get_signal_data (int sig)
{
	return sys_sig_get_data(sig);
}

/*------------------------------------------------------------------------
 * miscellaneous routines (ecurs_misc)
 */
int blank_screen (const char **logo)
{
	return scrn_blank_screen(win_cur_screen, logo);
}

int set_title (const char *s)
{
	return scrn_set_title(win_cur_screen, s);
}

int set_async_key (int key, WIN_ASYNC_KEY_RTN *rtn, void *data)
{
	return scrn_set_async_key(win_cur_screen, key, rtn, data);
}

int set_icon (int width, int height, const unsigned char *bits)
{
	return scrn_set_icon(win_cur_screen, width, height, bits);
}

int set_debug_scr (const char *path, int bf, int text)
{
	return scrn_set_debug_scr(win_cur_screen, path, bf, text);
}

int set_debug_kbd (const char *path, int bf, int text)
{
	return scrn_set_debug_kbd(win_cur_screen, path, bf, text);
}

int set_event_rtn (WIN_EVENT_RTN *rtn, void *data)
{
	return scrn_set_event_rtn(win_cur_screen, rtn, data);
}

int set_input_rtn (WIN_INPUT_RTN *rtn, void *data)
{
	return scrn_set_input_rtn(win_cur_screen, rtn, data);
}

int set_output_rtn (WIN_OUTPUT_RTN *rtn, void *data)
{
	return scrn_set_output_rtn(win_cur_screen, rtn, data);
}

/*------------------------------------------------------------------------
 * screen dump routines (ecurs_scrdump)
 */
int scr_dump (const char *filename)
{
	return scrn_scr_dump(win_cur_screen, filename);
}

int scr_init (const char *filename)
{
	return scrn_scr_init(win_cur_screen, filename);
}

int scr_restore (const char *filename)
{
	return scrn_scr_restore(win_cur_screen, filename);
}

int scr_set (const char *filename)
{
	return scrn_scr_set(win_cur_screen, filename);
}

/*------------------------------------------------------------------------
 * SLK - soft-label functions (ecurs_slk)
 */
int slk_set (int labnum, const char *label, int label_fmt)
{
	return slk_set_code(labnum, label, label_fmt, 0);
}

int slk_set_code (int labnum, const char *label, int label_fmt, int code)
{
	return scrn_slk_set_code(win_cur_screen, labnum, label, label_fmt, code);
}

char * slk_label (int labnum)
{
	return scrn_slk_label(win_cur_screen, labnum);
}

int slk_attron (attr_t attrs)
{
	return scrn_slk_attron(win_cur_screen, attrs);
}

int slk_attroff (attr_t attrs)
{
	return scrn_slk_attroff(win_cur_screen, attrs);
}

int slk_attrset (attr_t attrs)
{
	return scrn_slk_attrset(win_cur_screen, attrs);
}

int slk_attrbkgd (attr_t attrs)
{
	return scrn_slk_attrbkgd(win_cur_screen, attrs);
}

int slk_refresh (void)
{
	return scrn_slk_refresh(win_cur_screen);
}

int slk_noutrefresh (void)
{
	return scrn_slk_noutrefresh(win_cur_screen);
}

int slk_clear (void)
{
	return scrn_slk_clear(win_cur_screen);
}

int slk_restore (void)
{
	return scrn_slk_restore(win_cur_screen);
}

int slk_touch (void)
{
	return scrn_slk_touch(win_cur_screen);
}
