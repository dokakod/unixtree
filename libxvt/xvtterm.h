/*------------------------------------------------------------------------
 * header for window routines
 */
#ifndef XVTTERM_H
#define XVTTERM_H

/*------------------------------------------------------------------------
 * misc defines
 */
#define XVT_DIE_LOOPS		10				/* max loops to wait for child	*/
											/* to die before bailing		*/

/*------------------------------------------------------------------------
 * number of params supported
 */
#define ESC_NUM_PARAMS		16				/* terminfo supports 9			*/
											/* termcap  supports 2			*/
											/* real VTs support  16			*/

/*------------------------------------------------------------------------
 * data length needed to hold an icon
 */
#define XVT_ICON_LEN		(XVT_MAX_BITS * XVT_MAX_BITS / 8)

/*------------------------------------------------------------------------
 * screen modes
 */
#define SCRN_MODE_NONE		-1
#define SCRN_MODE_MONO		0
#define SCRN_MODE_COLOR		1

/*------------------------------------------------------------------------
 * printing modes
 */
#define PRT_MODE_NONE		0				/* not printing					*/
#define PRT_MODE_NORM		1				/* printing: normal				*/
#define PRT_MODE_ESC		2				/* printing: in esc seq			*/

/*------------------------------------------------------------------------
 * indexes into options arrays
 */
typedef enum
{
	/*--------------------------------------------------------------------
	 * options with no resource/option to initialize them
	 */
	SM_AppCursorKeys,
	SM_AppKeypadKeys,
	SM_SmoothScroll,
	SM_OriginMode,
	SM_AutoRepeat,
	SM_PrintScreenFF,
	SM_PrintScreenRegion,
	SM_PrintAuto,
	SM_ShowCursor,
	SM_Logging,
	SM_AltBuffer,
	SM_BSsendsDel,
	SM_DELsendsDel,
	SM_AutoNL,
	SM_LocalEcho,
	SM_KbdLocked,

	SM_MouseMode,
	SM_KbdMapMode,

	/*--------------------------------------------------------------------
	 * options with a resource/option to initialize them
	 */
	SM_AllowWideMode,
	SM_WideMode,
	SM_ReverseVideo,
	SM_WrapAround,
	SM_PrinterKeep,
	SM_HiliteCursor,
	SM_ShowScrollbar,
	SM_SpecialKeys,
	SM_MarginBell,
	SM_ReverseWrap,
	SM_ScrollOnOutput,
	SM_ScrollOnKeypress,
	SM_MapOnOutput,
	SM_MapOnAlert,
	SM_IgnoreBell,
	SM_VisualBell,
	SM_LabelInfo,
	SM_MoreFix,

	SM_Last
} Option_Names;

/*------------------------------------------------------------------------
 * Mouse modes (for SM_MouseMode)
 */
typedef enum
{
	Mouse_None,
	Mouse_X10,
	Mouse_X11,
	Mouse_Hilite,
	Mouse_CellMotion,
	Mouse_AllMotion,
	Mouse_XVT
} Mouse_Modes;

/*------------------------------------------------------------------------
 * keyboard map modes (for SM_KbdMapMode)
 */
typedef enum
{
	Kbd_MapNone,
	Kbd_MapSome,
	Kbd_MapAll
} Kbd_Modes;

/*------------------------------------------------------------------------
 * option setting commands
 */
typedef enum
{
	Option_Set,
	Option_Reset,
	Option_Save,
	Option_Restore,
	Option_Toggle
} Option_Cmd;

/*------------------------------------------------------------------------
 * term-data escape-info struct
 */
struct escinfo
{
	int					reading_esc_seq;		/* T if reading esc-seq		*/

	char				esc_chars[128];			/* esc-seq buffer			*/
	int					esc_char_cnt;			/* # of chars in esc_chars	*/

	int					code;					/* current code				*/

	int					n[ESC_NUM_PARAMS];		/* esc-seq params			*/
	int					n_index;				/* cur index of n[]			*/
	int					char_code;				/* char code found			*/

	char				data_chars[1024 + 4];	/* data chars				*/
	int					data_cnt;				/* # chars of data read		*/

	int					vt_mode;				/* processing mode			*/
	int					vt_type;				/* data type (GCS & OSC)	*/
};
typedef struct escinfo ESCINFO;

/*------------------------------------------------------------------------
 * forward reference for cmd-tbl (found in xvtcmds.h)
 */
typedef struct cmd_tbl	CMD_TBL;

/*------------------------------------------------------------------------
 * pseudo input/output struct
 *
 * These structs are allocated on the heap when pseudo-input/output
 * are presented to be input or output.
 */
typedef struct pty_iobuf	PTY_IOBUF;
struct pty_iobuf
{
	PTY_IOBUF *			next;					/* pointer to next entry	*/

	unsigned char *		bufptr;					/* pointer to buffer		*/
	int					buflen;					/* length of buffer			*/
	int					bufcnt;					/* # chars in buffer		*/
	int					bufpos;					/* position in buffer		*/
	int					bufheap;				/* TRUE if on heap			*/
};

/*------------------------------------------------------------------------
 * term-data struct
 */
struct termdata
{
	/*--------------------------------------------------------------------
	 * top-level info
	 */
	int					init;					/* TRUE if initialized		*/
	int					detached;				/* TRUE if detached			*/
	int					no_detach;				/* TRUE to inhibit detach	*/

	int					run_interval;			/* run wait interval (ms)	*/
	int					die_interval;			/* die wait interval (ms)	*/
	int					bail;					/* non-zero if bailing		*/

	/*--------------------------------------------------------------------
	 * program info
	 */
	XVT_DATA *			xd;						/* ptr to user's xvt info	*/

	char				org_label[128];			/* original window label	*/
	char				win_label[128];			/* current  window label	*/
	char				ico_label[128];			/* current  icon   label	*/

	/*--------------------------------------------------------------------
	 * environment variable buffers ( "NAME=VALUE" )
	 */
	char				dsp_var_str[128];		/* DISPLAY env string		*/
	char				env_var_str[128];		/* env var string			*/
	char				trm_var_str[128];		/* TERM env string			*/
	char				wid_var_str[128];		/* WINDOWID env string		*/

	/*--------------------------------------------------------------------
	 * icon info
	 */
	unsigned char *		ico_bits;				/* ptr to icon definition	*/
	int					ico_width;				/* icon width  in pixels	*/
	int					ico_height;				/* icon height in pixels	*/

	/*--------------------------------------------------------------------
	 * screen info
	 */
	TERMSCRN *			reg_scrn;				/* ptr to regular   screen	*/
	TERMSCRN *			alt_scrn;				/* ptr to alternate screen	*/
	TERMSCRN *			cur_scrn;				/* ptr to current   screen	*/

	/*--------------------------------------------------------------------
	 * terminal emulator info
	 */
	const CMD_TBL *		cmd_tbl_ptr;			/* ptr to command table		*/

	const KEY_DATA *	keys_fn;				/* function key table		*/
	const KEY_DATA *	keys_ms;				/* misc     key table		*/
	const KEY_DATA *	keys_ct;				/* control  key table		*/
	const KEY_DATA *	keys_kp;				/* keypad   key table		*/
	const KEY_DATA *	keys_pf;				/* prefix   key table		*/
	const KEY_DATA *	keys_st_up;				/* state-up key table		*/
	const KEY_DATA *	keys_st_dn;				/* state-dn key table		*/

	int					cur_options[SM_Last];	/* current options			*/
	int					sav_options[SM_Last];	/* saved   options			*/

	/*--------------------------------------------------------------------
	 * color info
	 */
	int					scrn_mode;				/* screen mode				*/

	/*--------------------------------------------------------------------
	 * mouse info
	 */
	int					mouse_x;				/* current  x-position		*/
	int					mouse_y;				/* current  y-position		*/

	int					mouse_col;				/* current  col position	*/
	int					mouse_row;				/* current  row position	*/
	int					mouse_cprev;			/* previous col position	*/
	int					mouse_rprev;			/* previous row position	*/

	int					mouse_btns;				/* current  button state	*/
	int					mouse_bprev;			/* previous button state	*/

	/*--------------------------------------------------------------------
	 * program state info
	 */
	int					our_pid;				/* for debugging			*/
	int					our_tid;				/* sub-thread id			*/
	int					pid;					/* pid of user				*/

	int					focus_mode;				/* T if we have focus		*/
	int					read_pending;			/* T if last op was a read	*/

	/*--------------------------------------------------------------------
	 * log info
	 */
	int					output_debug_seq;		/* T if last output was seq	*/
	int					log_pid;				/* T to add pid to evt log	*/

	/*--------------------------------------------------------------------
	 * child process info
	 */
	char **				argv;					/* arg vector				*/
	int					pid_child;				/* PID of child				*/
	int					pid_died;				/* pid from waitpid			*/
	int					exit_code;				/* exit code				*/
	PTY_DATA *			pd;						/* pty data					*/

	/*--------------------------------------------------------------------
	 * printer info
	 */
	PRINTER *			prt;					/* printer struct			*/
	int					prt_mode;				/* printing mode			*/

	/*--------------------------------------------------------------------
	 * input/output processing
	 */
	PTY_IOBUF *			inp_current;			/* current input buffer		*/
	int					inp_bail;				/* TRUE to bail early		*/

	unsigned char		inp_buffer[1024];		/* input from child			*/
	PTY_IOBUF			inp_data;				/* input data				*/

	unsigned char		out_buffer[1024];		/* output to child			*/
	PTY_IOBUF			out_data;				/* output data				*/

	PTY_IOBUF *			pty_inpbufs;			/* pseudo-input  chain		*/
	PTY_IOBUF *			pty_outbufs;			/* pseudo-output chain		*/

	THREAD_MUTEX		pty_inpmtx;				/* mutex for input  chain	*/
	THREAD_MUTEX		pty_outmtx;				/* mutex for output chain	*/

	/*--------------------------------------------------------------------
	 * escape-seq info
	 */
	ESCINFO				esc;					/* escape-info struct		*/
	int					prev_char;				/* previous char output		*/

	/*--------------------------------------------------------------------
	 * icon info
	 */
	unsigned char		icon_bits[XVT_ICON_LEN];	/* icon-bits data		*/
	int					icon_width;				/* width  of icon bits		*/
	int					icon_height;			/* height of icon-bits		*/

	/*--------------------------------------------------------------------
	 * display info
	 */
	TERMWIN *			tw;						/* ptr to window info		*/
};
typedef struct termdata	TERMDATA;

/*------------------------------------------------------------------------
 * option accessor macros
 */
#define OPTION_SET(td,o,v)	((td)->cur_options[o] = (v))
#define OPTION_GET(td,o)	((td)->cur_options[o])

#define OPTION_SAV(td,o)	((td)->sav_options[o] = (td)->cur_options[o])
#define OPTION_RES(td,o)	((td)->cur_options[o] = (td)->sav_options[o])

/* toggle boolean option */
#define OPTION_TGL(td,o)	((td)->cur_options[o] = ! (td)->cur_options[o])

/* toggle value option */
#define OPTION_TGV(td,o,v,d) \
							((td)->cur_options[o] = \
							 (td)->cur_options[o] == (v) ? (d) : (v))

/*------------------------------------------------------------------------
 * current screen accessor macros
 */
#define CUR_SCRN(td)			( (td)->cur_scrn )

#define CUR_SCRN_COLS(td)		SCRN_COLS		(CUR_SCRN(td))
#define CUR_SCRN_ROWS(td)		SCRN_ROWS		(CUR_SCRN(td))
#define CUR_SCRN_TABS(td)		SCRN_TABS		(CUR_SCRN(td))

#define CUR_SCRN_TOP(td)		SCRN_TOP		(CUR_SCRN(td))
#define CUR_SCRN_BOT(td)		SCRN_BOT		(CUR_SCRN(td))
#define CUR_SCRN_LEFT(td)		SCRN_LEFT		(CUR_SCRN(td))
#define CUR_SCRN_RIGHT(td)		SCRN_RIGHT		(CUR_SCRN(td))

#define CUR_SCRN_DISP(td)		SCRN_DISP		(CUR_SCRN(td))
#define CUR_SCRN_LINE(td,y)		SCRN_LINE		(CUR_SCRN(td), y)
#define CUR_SCRN_CHAR(td,y,x)	SCRN_CHAR		(CUR_SCRN(td), y, x)

#define CUR_SCRN_TABSTOPS(td)	SCRN_TABSTOPS	(CUR_SCRN(td))
#define CUR_SCRN_TABSTOP(td,x)	SCRN_TABSTOP	(CUR_SCRN(td), x)

#define CUR_SCRN_MODE(td)		SCRN_MODE		(CUR_SCRN(td))
#define CUR_SCRN_FG(td)			SCRN_FG			(CUR_SCRN(td))
#define CUR_SCRN_BG(td)			SCRN_BG			(CUR_SCRN(td))
#define CUR_SCRN_AT(td)			SCRN_AT			(CUR_SCRN(td))

#define CUR_SCRN_GCS(td, n)		SCRN_GCS		(CUR_SCRN(td), n)
#define CUR_SCRN_CS(td)			SCRN_CS			(CUR_SCRN(td))
#define CUR_SCRN_TMPCS(td)		SCRN_TMPCS		(CUR_SCRN(td))

#define CUR_SCRN_X(td)			SCRN_X			(CUR_SCRN(td))
#define CUR_SCRN_Y(td)			SCRN_Y			(CUR_SCRN(td))

#define CUR_SCRN_SAVE_MODE(td)	SCRN_SAVE_MODE	(CUR_SCRN(td))
#define CUR_SCRN_SAVE_FG(td)	SCRN_SAVE_FG	(CUR_SCRN(td))
#define CUR_SCRN_SAVE_BG(td)	SCRN_SAVE_BG	(CUR_SCRN(td))
#define CUR_SCRN_SAVE_AT(td)	SCRN_SAVE_AT	(CUR_SCRN(td))

#define CUR_SCRN_SAVE_GCS(td,n)	SCRN_SAVE_GCS	(CUR_SCRN(td), n)
#define CUR_SCRN_SAVE_CS(td)	SCRN_SAVE_CS	(CUR_SCRN(td))

#define CUR_SCRN_SAVE_X(td)		SCRN_SAVE_X		(CUR_SCRN(td))
#define CUR_SCRN_SAVE_Y(td)		SCRN_SAVE_Y		(CUR_SCRN(td))

#define CUR_SCRN_INS(td)		SCRN_INS		(CUR_SCRN(td))
#define CUR_SCRN_ORG(td)		SCRN_ORG		(CUR_SCRN(td))
#define CUR_SCRN_LOCK(td)		SCRN_LOCK		(CUR_SCRN(td))

#define CUR_SCRN_CSV(td)		SCRN_CSV		(CUR_SCRN(td))
#define CUR_SCRN_CON(td)		SCRN_CON		(CUR_SCRN(td))

/*------------------------------------------------------------------------
 * internal functions
 */
extern void		xvt_term_raise				(TERMDATA *td);
extern void		xvt_term_lower				(TERMDATA *td);
extern void		xvt_term_get_size			(TERMDATA *td, int *w, int *h);
extern void		xvt_term_set_size			(TERMDATA *td, int  w, int  h);
extern void		xvt_term_get_position		(TERMDATA *td, int *x, int *y);
extern void		xvt_term_set_position		(TERMDATA *td, int  x, int  y);

extern void		xvt_term_maximize			(TERMDATA *td);
extern void		xvt_term_restore			(TERMDATA *td);
extern void		xvt_term_resize				(TERMDATA *td, int cols, int rows);
extern void		xvt_term_repaint			(TERMDATA *td);
extern void		xvt_term_disp_fill			(TERMDATA *td, int prot,
												int dx, int dy,
												int dw, int dh);
extern void		xvt_term_disp_copy			(TERMDATA *td,
												int dx, int dy,
												int dw, int dh,
												int sx, int sy);
extern void		xvt_term_disp_char			(TERMDATA *td,
												int x, int y, int ch,
												int fg, int bg, int at);

extern void		xvt_term_screen_print		(TERMDATA *td,
												int all, int decpex);
extern void		xvt_term_screen_print_line	(TERMDATA *td);
extern void		xvt_term_screen_clipboard	(TERMDATA *td);
extern void		xvt_term_write_report		(TERMDATA *td, const char *fmt,
												...);
extern void		xvt_term_force_kbd_state	(TERMDATA *td, int it_is);
extern void		xvt_term_set_kbd_state		(TERMDATA *td, int it_is);
extern void		xvt_term_set_scrn_mode		(TERMDATA *td, int mode);
extern void		xvt_term_set_win_title		(TERMDATA *td, char *str);
extern void		xvt_term_set_ico_title		(TERMDATA *td, char *str);
extern void		xvt_term_set_icon			(TERMDATA *td,
												int width, int height,
												unsigned char *bits);
extern void		xvt_term_chg_color			(TERMDATA *td, int n,
												const char *color);
extern const char *
				xvt_term_get_color_name		(TERMDATA *td, int n);
extern void		xvt_term_set_txt_cursor		(TERMDATA *td, int state);
extern void		xvt_term_set_pointer		(TERMDATA *td, int shape);
extern void		xvt_term_set_icon_state		(TERMDATA *td, int state);
extern void		xvt_term_set_font_name		(TERMDATA *td, int n,
												const char *font_name);
extern const char *
				xvt_term_get_font_name		(TERMDATA *td, int n);
extern void		xvt_term_set_font_no		(TERMDATA *td, int n);
extern void		xvt_term_chg_font_no		(TERMDATA *td, int n);
extern void		xvt_term_set_rev_video		(TERMDATA *td, int reverse);
extern void		xvt_term_bell				(TERMDATA *td);

extern void		xvt_term_printer_open		(TERMDATA *td);
extern void		xvt_term_printer_close		(TERMDATA *td);

extern int		xvt_term_loop_once			(TERMDATA *td, int ms);

extern void *	xvt_term_main_loop			(void *data);

extern int		xvt_term_setup_all			(TERMDATA *td);
extern void		xvt_term_close_all			(TERMDATA *td);

extern void		xvt_term_shutdown			(TERMDATA *td);
extern int		xvt_term_restart			(TERMDATA *td);

extern int		xvt_term_pty_input			(TERMDATA *td,
												const void *buf, int len);
extern int		xvt_term_pty_output			(TERMDATA *td,
												const void *buf, int len);

extern void		xvt_term_list_pointers		(FILE *fp, int verbose);

#endif /* XVTTERM_H */
