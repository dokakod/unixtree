/*------------------------------------------------------------------------
 * screen output routines
 *
 * This header is public.
 */
#ifndef TCSCROUT_H
#define TCSCROUT_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * screen log filenames
 */
#define TCAP_SCRN_LOG_BIN	"scrn.log"	/* binary log file name	*/
#define TCAP_SCRN_LOG_TXT	"scrn.txt"	/* text   log file name	*/

/*------------------------------------------------------------------------
 * functions
 */
extern int		tcap_attrbuf			(const TERMINAL *tp, attr_t a, int c,
											char *buf);
extern int		tcap_outattr			(TERMINAL *tp, attr_t a, int c);
extern int		tcap_outc				(TERMINAL *tp, int c);
extern int		tcap_outs				(TERMINAL *tp, const char *s);
extern int		tcap_outpos				(TERMINAL *tp, int row, int col);

extern char *	tcap_eval_parm			(const TERMINAL *tp, char *buf,
											SCRN_CMD cmd, int p1, int p2);

extern int		tcap_get_valid_acs		(const TERMINAL *tp, int c);
extern int		tcap_set_valid_acs		(TERMINAL *tp, int bf);

extern int		tcap_curs_set			(TERMINAL *tp, int vis);
extern int		tcap_delay_output		(TERMINAL *tp, int ms,
											int (*outrtn)(TERMINAL *tp, int c));

extern int		tcap_out_debug			(TERMINAL *tp, const char *path,
											int bf, int text);

extern int		tcap_get_curr_row		(const TERMINAL *tp);
extern int		tcap_get_curr_col		(const TERMINAL *tp);
extern attr_t	tcap_get_curr_attr		(const TERMINAL *tp);

extern int		tcap_set_window_title	(TERMINAL *tp, const char *s);
extern int		tcap_clear_screen		(TERMINAL *tp);

extern int		tcap_need_ins_final		(const TERMINAL *tp);
extern int		tcap_write_final_char	(TERMINAL *tp, int cpc, attr_t cpa,
											int chc, attr_t cha);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCSCROUT_H */
