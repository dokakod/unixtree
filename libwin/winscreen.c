/*------------------------------------------------------------------------
 * curses-tcap interfaces for a specified SCREEN
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * initialization functions (ecurs_initscr)
 */
void set_term_path (const char *path)
{
	tcap_set_term_path(path);
}

int set_term_db (const char *db_type)
{
	return tcap_set_term_db(db_type);
}

const char * get_term_db (void)
{
	return tcap_get_term_db();
}

const char * get_term_db_name (int type)
{
	return tcap_get_term_db_name(type);
}

const char * get_term_db_desc (int type)
{
	return tcap_get_term_db_desc(type);
}

/*------------------------------------------------------------------------
 * curses kernel routines (ecurs_kernel)
 */
int napms (int ms)
{
	return tcap_napms(ms);
}

int scrn_curs_set (SCREEN *s, int visibility)
{
	if (s == 0)
		return (ERR);

	return tcap_curs_set(SCR_TERM(s), visibility);
}

int scrn_set_syx (SCREEN *s, int y, int x)
{
	if (s == 0)
		return (ERR);

	tcap_set_cury(SCR_TERM(s), y);
	tcap_set_curx(SCR_TERM(s), x);

	return (OK);
}

int scrn_get_syx (const SCREEN *s, int *y, int *x)
{
	if (s == 0)
		return (ERR);

	*y = tcap_get_cury(SCR_TERM(s));
	*x = tcap_get_curx(SCR_TERM(s));

	return (OK);
}

int scrn_def_shell_mode (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_get_orig_mode(SCR_TERM(s));
}

int scrn_reset_shell_mode (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_set_orig_mode(SCR_TERM(s));
}

int scrn_def_prog_mode (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_get_prog_mode(SCR_TERM(s));
}

int scrn_reset_prog_mode (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_set_prog_mode(SCR_TERM(s));
}

int scrn_savetty (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_get_save_mode(SCR_TERM(s));
}

int scrn_resetty (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_set_save_mode(SCR_TERM(s));
}

/*------------------------------------------------------------------------
 * alarm functions (ecurs_beep)
 */
int scrn_beep (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_beep(SCR_TERM(s));
}

int scrn_flash (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_flash(SCR_TERM(s));
}

/*------------------------------------------------------------------------
 * input options (ecurs_inopts)
 */
int scrn_nlinp (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	s->do_inp_nl = TRUE;

	return (OK);
}

int scrn_nonlinp (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	s->do_inp_nl = FALSE;

	return (OK);
}

int scrn_raw (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_set_raw(SCR_TERM(s), TRUE);
}

int scrn_noraw (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_set_raw(SCR_TERM(s), FALSE);
}

int scrn_cbreak (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_set_cbreak(SCR_TERM(s), TRUE);
}

int scrn_nocbreak (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_set_cbreak(SCR_TERM(s), FALSE);
}

int scrn_echo (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	s->do_echo = TRUE;

	return (OK);
}

int scrn_noecho (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	s->do_echo = FALSE;

	return (OK);
}

int scrn_metaon (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_set_meta(SCR_TERM(s), TRUE);
}

int scrn_metaoff (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_set_meta(SCR_TERM(s), FALSE);
}

int scrn_qiflush (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_set_qiflush(SCR_TERM(s), TRUE);
}

int scrn_noqiflush (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_set_qiflush(SCR_TERM(s), FALSE);
}

int scrn_typeahead (SCREEN *s, int fildes)
{
	if (s == 0)
		return (ERR);

	return tcap_set_typeahead(SCR_TERM(s), fildes);
}

int scrn_halfdelay (SCREEN *s, int tenths)
{
	if (s == 0)
		return (ERR);

	return tcap_set_halfdelay(SCR_TERM(s), tenths);
}

int scrn_addkey (SCREEN *s, const char *string, int code)
{
	if (s == 0)
		return (ERR);

	return tcap_kbd_add_key(SCR_TERM(s), string, code);
}

/*------------------------------------------------------------------------
 * output options (ecurs_outopts)
 */
int scrn_nl (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	s->do_inp_nl = TRUE;
	s->do_out_nl = TRUE;

	return (OK);
}

int scrn_nonl (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	s->do_inp_nl = FALSE;
	s->do_out_nl = FALSE;

	return (OK);
}

int scrn_nlout (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	s->do_out_nl = TRUE;

	return (OK);
}

int scrn_nonlout (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	s->do_out_nl = FALSE;

	return (OK);
}

int scrn_get_valid_acs (const SCREEN *s, int c)
{
	if (s == 0)
		return (ERR);

	return tcap_get_valid_acs(SCR_TERM(s), c);
}

int scrn_set_valid_acs (SCREEN *s, int bf)
{
	if (s == 0)
		return (ERR);

	return tcap_set_valid_acs(SCR_TERM(s), bf);
}

/*------------------------------------------------------------------------
 * terminal attribute functions (ecurs_termattrs)
 */
int scrn_baudrate (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_get_baud_rate(SCR_TERM(s));
}

int scrn_killchar (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_get_kill_char(SCR_TERM(s));
}

int scrn_erasechar (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_get_erase_char(SCR_TERM(s));
}

int scrn_quitchar (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_get_quit_char(SCR_TERM(s));
}

int scrn_intrchar (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_get_intr_char(SCR_TERM(s));
}

int scrn_has_ic (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_get_has_ic(SCR_TERM(s));
}

int scrn_has_il (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_get_has_il(SCR_TERM(s));
}

int scrn_get_window_env (const SCREEN *s)
{
	if (s == 0)
		return tcap_get_window_env();

	return tcap_get_disp_in_win(SCR_TERM(s));
}

attr_t scrn_termattrs (const SCREEN *s)
{
	if (s == 0)
		return (0);

	return tcap_get_termattrs(SCR_TERM(s));
}

const char * scrn_termname (const SCREEN *s)
{
	if (s == 0)
		return ("?");

	return tcap_get_termtype(SCR_TERM(s));
}

const char * scrn_longname (const SCREEN *s)
{
	if (s == 0)
		return ("?");

	return tcap_get_longname(SCR_TERM(s));
}

const char * scrn_termtype (const SCREEN *s)
{
	if (s == 0)
		return ("?");

	return tcap_get_termtype(SCR_TERM(s));
}

const char * scrn_get_ttyname (const SCREEN *s)
{
	if (s == 0)
		return ("?");

	return tcap_get_ttyname(SCR_TERM(s));
}

int scrn_get_curr_row (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_get_curr_row(SCR_TERM(s));
}

int scrn_get_curr_col (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_get_curr_col(SCR_TERM(s));
}

int scrn_get_max_cols (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_get_max_cols(SCR_TERM(s));
}

int scrn_get_max_rows (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_get_max_rows(SCR_TERM(s));
}

int scrn_get_tab_size (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_get_tab_size(SCR_TERM(s));
}

attr_t scrn_get_orig_attr (const SCREEN *s)
{
	if (s == 0)
		return (A_UNSET);

	return tcap_get_orig_attr(SCR_TERM(s));
}

attr_t scrn_get_curr_attr (const SCREEN *s)
{
	if (s == 0)
		return (A_UNSET);

	return tcap_get_curr_attr(SCR_TERM(s));
}

/*------------------------------------------------------------------------
 * utility functions (ecurs_util)
 */
void use_env (int bf)
{
	tcap_set_use_env(bf);
}

int scrn_flushinp (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_flush_inp(SCR_TERM(s));
}

int scrn_delay_output (SCREEN *s, int ms)
{
	if (s == 0)
		return (ERR);

	return tcap_delay_output(SCR_TERM(s), ms, 0);
}

int scrn_filter (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return (OK);
}

/*------------------------------------------------------------------------
 * color functions (ecurs_color)
 */
int scrn_start_color (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_start_color(SCR_TERM(s));
}

int scrn_has_colors (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_has_colors(SCR_TERM(s));
}

int scrn_init_pair (SCREEN *s, short pair, short  f, short  b)
{
	if (s == 0)
		return (ERR);

	return tcap_init_pair(SCR_TERM(s), pair, f, b);
}

int scrn_pair_content (const SCREEN *s, short pair, short *f, short *b)
{
	if (s == 0)
		return (ERR);

	return tcap_pair_content(SCR_TERM(s), pair, f, b);
}

int scrn_init_all_pairs (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_init_all_pairs(SCR_TERM(s));
}

int scrn_color_content (const SCREEN *s, short color,
	short *r, short *g, short *b)
{
	if (s == 0)
		return (ERR);

	return tcap_color_content(SCR_TERM(s), color, r, g, b);
}

int scrn_init_color (SCREEN *s, short color, short  r, short  g, short  b)
{
	if (s == 0)
		return (ERR);

	return tcap_init_color(SCR_TERM(s), color, r, g, b);
}

int scrn_can_change_color (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_can_change_color(SCR_TERM(s));
}

int scrn_pair_number (const SCREEN *s, attr_t a)
{
	if (s == 0)
		return (ERR);

	return tcap_pair_number(SCR_TERM(s), a);
}

attr_t scrn_color_pair (const SCREEN *s, int n)
{
	if (s == 0)
		return (A_UNSET);

	return tcap_color_pair(SCR_TERM(s), n);
}

/*------------------------------------------------------------------------
 * printer functions (ecurs_printer)
 */
PRINTER * scrn_prt_open (SCREEN *s, int mode, const char *str, char *msgbuf)
{
	if (s == 0)
	{
		sprintf(msgbuf, "screen not initialized");
		return (0);
	}

	return tcap_prt_open(SCR_TERM(s), mode, str, msgbuf);
}

int scrn_prt_close (SCREEN *s, PRINTER *p, char *msgbuf)
{
	if (s == 0)
	{
		sprintf(msgbuf, "screen not initialized");
		return (ERR);
	}

	return tcap_prt_close(SCR_TERM(s), p, msgbuf);
}

int scrn_prt_output_str (SCREEN *s, PRINTER *p, const char *str)
{
	if (s == 0)
	{
		return (ERR);
	}

	return tcap_prt_output_str(SCR_TERM(s), p, str);
}

int scrn_prt_output_fmt (SCREEN *s, PRINTER *p, const char *fmt, ...)
{
	va_list args;
	int		rc;

	va_start(args, fmt);
	rc = scrn_prt_output_var(s, p, fmt, args);
	va_end(args);

	return (rc);
}

int scrn_prt_output_var (SCREEN *s, PRINTER *p, const char *fmt, va_list args)
{
	if (s == 0)
	{
		return (ERR);
	}

	return tcap_prt_output_var(SCR_TERM(s), p, fmt, args);
}

/*------------------------------------------------------------------------
 * cmd-exec functions (ecurs_exec)
 */
int scrn_exec_argv (SCREEN *s, const char *dir, char **argv, int interactive,
	char *msgbuf)
{
	TERMINAL *t;

	if (s == 0)
	{
		t = 0;
	}
	else
	{
		t = SCR_TERM(s);
		endscreen(s, FALSE);
	}

	return tcap_exec_argv(t, dir, argv, interactive, msgbuf);
}

int scrn_exec_cmd (SCREEN *s, const char *dir, const char *cmd,
	int interactive, char *msgbuf)
{
	TERMINAL *t;

	if (s == 0)
	{
		t = 0;
	}
	else
	{
		t = SCR_TERM(s);
		endscreen(s, FALSE);
	}

	return tcap_exec_cmd(t, dir, cmd, interactive, msgbuf);
}

/*------------------------------------------------------------------------
 * mouse functions (ecurs_mouse)
 */
int scrn_mouse_init (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_mouse_init(SCR_TERM(s));
}

int scrn_mouse_get_display (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_mouse_get_display(SCR_TERM(s));
}

int scrn_mouse_get_event (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_mouse_get_event(SCR_TERM(s));
}

int scrn_mouse_x (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_mouse_get_x(SCR_TERM(s));
}

int scrn_mouse_y (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_mouse_get_y(SCR_TERM(s));
}

/*------------------------------------------------------------------------
 * mono/color attribute functions (ecurs_attributes)
 */
int scrn_get_default_attrs (SCREEN *s, attr_t *pr, attr_t *ps,
	int use_color, int use_win)
{
	if (s == 0)
		return (ERR);

	return tcap_get_default_attrs(SCR_TERM(s), pr, ps,
		use_color, use_win);
}

/*------------------------------------------------------------------------
 * miscellaneous routines (ecurs_misc)
 */
int scrn_set_title (SCREEN *s, const char *str)
{
	if (s == 0)
		return (ERR);

	return tcap_set_window_title(SCR_TERM(s), str);
}

int scrn_set_async_key (SCREEN *s, int key, WIN_ASYNC_KEY_RTN *rtn, void *data)
{
	if (s == 0)
		return (ERR);

	if (key == KEY_SIGNAL)
		return (ERR);

	return tcap_kbd_add_async(SCR_TERM(s), key, rtn, data);
}

int scrn_blank_screen (SCREEN *s, const char **logo)
{
	if (s == 0)
		return (ERR);

	return tcap_blank_screen(SCR_TERM(s), logo);
}

int scrn_set_icon (SCREEN *s, int width, int height, const unsigned char *bits)
{
	if (s == 0)
		return (ERR);

	return tcap_set_icon(SCR_TERM(s), width, height, bits);
}

int scrn_set_debug_scr (SCREEN *s, const char *path, int bf, int text)
{
	if (s == 0)
		return (FALSE);

	return tcap_out_debug(SCR_TERM(s), path, bf, text);
}

int scrn_set_debug_kbd (SCREEN *s, const char *path, int bf, int text)
{
	if (s == 0)
		return (ERR);

	return tcap_kbd_debug(SCR_TERM(s), path, bf, text);
}

int scrn_set_event_rtn (SCREEN *s, WIN_EVENT_RTN *rtn, void *data)
{
	if (s == 0)
		return (-1);

	return tcap_set_evt_rtn(SCR_TERM(s), rtn, data);
}

int scrn_set_input_rtn (SCREEN *s, WIN_INPUT_RTN *rtn, void *data)
{
	if (s == 0)
		return (-1);

	return tcap_set_inp_rtn(SCR_TERM(s), rtn, data);
}

int scrn_set_output_rtn (SCREEN *s, WIN_OUTPUT_RTN *rtn, void *data)
{
	if (s == 0)
		return (-1);

	return tcap_set_out_rtn(SCR_TERM(s), rtn, data);
}

/*------------------------------------------------------------------------
 * low-level mouse routines
 */
int scrn_mi_get_prev_y (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return (s->mouse_info.prev_y);
}

int scrn_mi_get_prev_x (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return (s->mouse_info.prev_x);
}

int scrn_mi_get_prev_c (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return (s->mouse_info.prev_c);
}

attr_t scrn_mi_get_ptr_attrs (const SCREEN *s)
{
	if (s == 0)
		return (A_UNSET);

	return (s->mouse_info.ptr_attrs);
}

attr_t scrn_mi_get_cmd_attrs (const SCREEN *s)
{
	if (s == 0)
		return (A_UNSET);

	return (s->mouse_info.cmd_attrs);
}

int scrn_mi_get_process (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return (s->mouse_info.process);
}

int scrn_mi_get_display (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return (s->mouse_info.display);
}

int scrn_mi_set_prev_y (SCREEN *s, int y)
{
	if (s == 0)
		return (ERR);

	s->mouse_info.prev_y = y;

	return (OK);
}

int scrn_mi_set_prev_x (SCREEN *s, int x)
{
	if (s == 0)
		return (ERR);

	s->mouse_info.prev_x = x;

	return (OK);
}

int scrn_mi_set_prev_c (SCREEN *s, int c)
{
	if (s == 0)
		return (ERR);

	s->mouse_info.prev_c = c;

	return (OK);
}

int scrn_mi_set_ptr_attrs (SCREEN *s, attr_t ptr_attrs)
{
	if (s == 0)
		return (ERR);

	s->mouse_info.ptr_attrs = ptr_attrs;

	return (OK);
}

int scrn_mi_set_cmd_attrs (SCREEN *s, attr_t cmd_attrs)
{
	if (s == 0)
		return (ERR);

	s->mouse_info.cmd_attrs = cmd_attrs;

	return (OK);
}

int scrn_mi_set_process (SCREEN *s, int process)
{
	if (s == 0)
		return (ERR);

	s->mouse_info.process = process;

	return (OK);
}

int scrn_mi_set_display (SCREEN *s, int display)
{
	if (s == 0)
		return (ERR);

	s->mouse_info.display = display;

	return (OK);
}

/*------------------------------------------------------------------------
 * low-level tcap interface routines
 */
int scrn_cinit (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_cinit(SCR_TERM(s));
}

int scrn_cexit (SCREEN *s, int exit_flag)
{
	if (s == 0)
		return (ERR);

	return tcap_cexit(SCR_TERM(s), exit_flag);
}

int scrn_outattr (SCREEN *s, attr_t a, int c)
{
	if (s == 0)
		return (ERR);

	return tcap_outattr(SCR_TERM(s), a, c);
}

int scrn_outc (SCREEN *s, int c)
{
	if (s == 0)
		return (ERR);

	return tcap_outc(SCR_TERM(s), c);
}

int scrn_outs (SCREEN *s, const char *str)
{
	if (s == 0)
		return (ERR);

	return tcap_outs(SCR_TERM(s), str);
}

int scrn_outpos (SCREEN *s, int row, int col)
{
	if (s == 0)
		return (ERR);

	return tcap_outpos(SCR_TERM(s), row, col);
}

int scrn_outflush (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_outflush(SCR_TERM(s));
}

int scrn_clear_screen (SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_clear_screen(SCR_TERM(s));
}

int scrn_need_ins_final (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_need_ins_final(SCR_TERM(s));
}

int scrn_write_final_char (SCREEN *s, int cpc, attr_t cpa, int chc, attr_t cha)
{
	if (s == 0)
		return (ERR);

	return tcap_write_final_char(SCR_TERM(s), cpc, cpa, chc, cha);
}

int scrn_check_window_size (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_winsize_check(SCR_TERM(s));
}

int scrn_reset_window_size (SCREEN *s, int rows, int cols)
{
	if (s == 0)
		return (ERR);

	return tcap_winsize_reset(SCR_TERM(s), rows, cols);
}

int scrn_getkey (SCREEN *s, int mode, int interval)
{
	if (s == 0)
		return (ERR);

	return tcap_kbd_get_key(SCR_TERM(s), interval, mode, TRUE, TRUE);
}

int scrn_chkkey (SCREEN *s, int mode)
{
	if (s == 0)
		return (ERR);

	return tcap_kbd_chk_key(SCR_TERM(s), mode);
}

int scrn_pushkey (SCREEN *s, int key, int front)
{
	if (s == 0)
		return (ERR);

	return tcap_kbd_enq_ta(SCR_TERM(s), key, front);
}

int scrn_get_timeout (const SCREEN *s)
{
	if (s == 0)
		return (ERR);

	return tcap_kbd_get_timeout(SCR_TERM(s));
}

int scrn_set_timeout (SCREEN *s, int bf)
{
	if (s == 0)
		return (ERR);

	return tcap_kbd_set_timeout(SCR_TERM(s), bf);
}
