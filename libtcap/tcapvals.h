/*------------------------------------------------------------------------
 * header for term attribute processing
 *
 * This header is public.
 */
#ifndef TCAPVALS_H
#define TCAPVALS_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int			tcap_set_term_db		(const char *term_db_type);
extern const char *	tcap_get_term_db		(void);

extern const char *	tcap_get_term_db_name	(int num);
extern const char *	tcap_get_term_db_desc	(int num);

extern int			tcap_dump_term			(const char *term, FILE *fp);

extern int			tcap_set_icon			(TERMINAL *tp,
												int width, int height,
												const unsigned char *bits);

typedef int			TCAP_EVENT_RTN			(void *data, int ms);
extern int			tcap_set_evt_rtn		(TERMINAL *tp,
												TCAP_EVENT_RTN *rtn,
												void *data);

typedef int			TCAP_INPUT_RTN			(void *data, int mode, int ms);
extern int			tcap_set_inp_rtn		(TERMINAL *tp,
												TCAP_INPUT_RTN *rtn,
												void *data);

typedef int			TCAP_OUTPUT_RTN			(void *data, int mode,
												const unsigned char *buf,
												int n);
extern int			tcap_set_out_rtn		(TERMINAL *tp,
												TCAP_OUTPUT_RTN *rtn,
												void *data);

extern int			tcap_get_window_env		(void);
extern int			tcap_get_disp_in_win	(const TERMINAL *tp);
extern int			tcap_get_max_cols		(const TERMINAL *tp);
extern int			tcap_get_max_rows		(const TERMINAL *tp);
extern int			tcap_get_tab_size		(const TERMINAL *tp);
extern const char *	tcap_get_termname		(const TERMINAL *tp);
extern const char *	tcap_get_longname		(const TERMINAL *tp);
extern const char *	tcap_get_termtype		(const TERMINAL *tp);
extern const char *	tcap_get_ttyname		(const TERMINAL *tp);
extern attr_t		tcap_get_termattrs		(const TERMINAL *tp);

extern attr_t		tcap_get_orig_attr		(const TERMINAL *tp);
extern int			tcap_get_has_ic			(const TERMINAL *tp);
extern int			tcap_get_has_il			(const TERMINAL *tp);

extern int			tcap_get_cmdstr_by_enum	(const TERMINAL *tp, SCRN_CMD cmd,
												char *buf);

extern int			tcap_get_cmdstr_by_name	(const TERMINAL *tp,
												const char *name,
												SCRN_CMD *pcmd, char *buf);

extern int			tcap_get_cmdstr_by_str	(const TERMINAL *tp,
												const char *str,
												SCRN_CMD *pcmd, char *buf,
												int *pp1, int *pp2);

extern int			tcap_get_default_attrs	(const TERMINAL *tp,
												attr_t *pr, attr_t *ps,
												int use_color, int use_win);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCAPVALS_H */
