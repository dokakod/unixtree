/*------------------------------------------------------------------------
 * header for display-specific routines
 */
#ifndef XVTDISPLAY_H
#define XVTDISPLAY_H

/*------------------------------------------------------------------------
 * button definitions
 */
#define BUT_L			0x01		/* left   button	*/
#define BUT_M			0x02		/* middle button	*/
#define BUT_R			0x04		/* right  button	*/
#define BUT_U			0x08		/* scroll up		*/
#define BUT_D			0x10		/* scroll down		*/

/*------------------------------------------------------------------------
 * mono attributes
 */
#define A_NORMAL		0x0000
#define A_BOLD			0x0001
#define A_UNDERLINE		0x0002
#define A_BLINK			0x0004
#define A_REVERSE		0x0008
#define A_HIDDEN		0x0010

#define A_DISPLAY		0x001f		/* display attributes	*/

/*------------------------------------------------------------------------
 * protection attributes
 *
 * There are two different character protection attributes:
 *
 *	A_DECPROT	is set with the DEC-style DECSCA cmd.
 *				ED & EL cmds will erase them, but DECSED & DECSEL
 *				cmds will preserve them.
 *
 *	A_ISOPROT	is set with the SPA/EPA cmds.
 *				ED & EL cmds will preserve them, but DECSED & DECSEL
 *				cmds will erase them.
 */
#define A_DECPROT		0x0020
#define A_ISOPROT		0x0040

/*------------------------------------------------------------------------
 * font id's
 */
#define FONT_NONE		-1
#define FONT_NORM		0
#define FONT_BOLD		1

/*------------------------------------------------------------------------
 * These are "extensions" to the user-color table for all the
 * other colors used.  This provides one array with all the colors
 * that are used.
 */
#define XVT_CLR_FG		( XVT_CLR_NUM + 0 )		/* foreground		*/
#define XVT_CLR_BG		( XVT_CLR_NUM + 1 )		/* background		*/

#define XVT_CLR_PR		( XVT_CLR_NUM + 2 )		/* pointer			*/
#define XVT_CLR_BD		( XVT_CLR_NUM + 3 )		/* border			*/
#define XVT_CLR_CR		( XVT_CLR_NUM + 4 )		/* text cursor		*/

#define XVT_CLR_SB		( XVT_CLR_NUM + 5 )		/* scrollbar		*/
#define XVT_CLR_ST		( XVT_CLR_NUM + 6 )		/* scrollbar trough	*/
#define XVT_CLR_SB_TS	( XVT_CLR_NUM + 7 )		/* SB top shadow	*/
#define XVT_CLR_SB_BS	( XVT_CLR_NUM + 8 )		/* SB bottom shadow	*/

#define XVT_CMS_SIZE	( XVT_CLR_NUM + 9 )

/*------------------------------------------------------------------------
 * x-data struct
 */
typedef struct x_data	X_DATA;

/*------------------------------------------------------------------------
 * window data struct
 */
struct termwin
{
	/*--------------------------------------------------------------------
	 * user configuration stuff (from XVT_DATA)
	 */
	int			reverse;				/* reverse video			*/
	int			acticon;				/* active icon				*/

	int			border;					/* border width in pixels	*/
	int			margin;					/* margin width in pixels	*/
	int			extra;					/* border + margin			*/

	int			sb_enable;				/* enable scrollbar			*/
	int			sb_right;				/* scrollbar on right		*/
	int			sb_shadow;				/* scrollbar shadow			*/
	int			sb_size;				/* scrollbar size			*/

	/*--------------------------------------------------------------------
	 * color table stuff (these never change: server dictates color/mono)
	 */
	int			can_be_color;			/* T if color supported		*/

	/*--------------------------------------------------------------------
	 * window stuff (changes only if user changes the window)
	 */
	int			win_cols;				/* width  of win in cols	*/
	int			win_rows;				/* height of win in rows	*/

	int			old_cols;				/* saved width  if maxed	*/
	int			old_rows;				/* saved height if maxed	*/
	int			old_x;					/* saved x position			*/
	int			old_y;					/* saved y position			*/

	/*--------------------------------------------------------------------
	 * window state (these constantly change)
	 */
	int			cur_font_no;			/* cur font number			*/
	int			cur_font_type;			/* cur font type			*/

	int			cur_fg;					/* cur fg display is in		*/
	int			cur_bg;					/* cur bg display is in		*/

	int			doing_resize;			/* TRUE if doing resize		*/
	int			iconified;				/* TRUE if iconified		*/

	int			keys;					/* cur state of prefix-keys	*/
	int			prev_action;			/* previous action			*/

	int			nscrolled;				/* number of lines scrolled	*/
	int			view_start;				/* start of view			*/

	/*--------------------------------------------------------------------
	 * selection stuff
	 */
	unsigned char *	selection_ptr;		/* pointer to selection		*/
	int				selection_len;		/* length of selection		*/

	/*--------------------------------------------------------------------
	 * window stuff which is only known internally
	 */
	X_DATA *	X;						/* ptr to x-data struct		*/
};
typedef struct termwin TERMWIN;

/*------------------------------------------------------------------------
 * setup/close functions
 */
extern TERMWIN *	xvt_w_setup_all			(XVT_DATA *xd);
extern void			xvt_w_close_all			(TERMWIN *tw);
extern void			xvt_w_close_con			(TERMWIN *tw);
extern void			xvt_w_free				(TERMWIN *tw);

/*------------------------------------------------------------------------
 * window functions
 */
extern void			xvt_w_resize			(TERMWIN *tw, int cols, int rows);
extern void			xvt_w_raise				(TERMWIN *tw);
extern void			xvt_w_lower				(TERMWIN *tw);

/*------------------------------------------------------------------------
 * misc get/set functions
 */
extern int			xvt_w_get_xfd			(TERMWIN *tw);
extern unsigned int	xvt_w_get_windowid		(TERMWIN *tw);

extern void			xvt_w_get_size			(TERMWIN *tw, int *w, int *h);
extern void			xvt_w_set_size			(TERMWIN *tw, int  w, int  h);

extern void			xvt_w_set_max_size		(TERMWIN *tw);
extern void			xvt_w_set_old_size		(TERMWIN *tw);

extern void			xvt_w_get_position		(TERMWIN *tw, int *x, int *y);
extern void			xvt_w_set_position		(TERMWIN *tw, int  x, int  y);

extern void			xvt_w_set_pointer		(TERMWIN *tw, int shape);
extern void			xvt_w_set_icon_state	(TERMWIN *tw, int state);
extern void			xvt_w_set_win_title		(TERMWIN *tw, const char *str);
extern void			xvt_w_set_ico_title		(TERMWIN *tw, const char *str);
extern int			xvt_w_chk_scrn_mode		(TERMWIN *tw, int mode);

extern void			xvt_w_bell				(TERMWIN *tw);

/*------------------------------------------------------------------------
 * icon functions
 */
extern void			xvt_w_set_icon			(TERMWIN *tw,
												int width, int height,
												unsigned char *bits);

extern void			xvt_w_set_icon_display	(TERMWIN *tw, TERMSCRN *s);

/*------------------------------------------------------------------------
 * color functions
 */
extern void			xvt_w_set_rev_video		(TERMWIN *tw, int reverse);
extern int			xvt_w_chg_clr			(TERMWIN *tw, int n,
												const char *color);
extern const char *	xvt_w_get_color_name	(TERMWIN *tw, int n);
extern int			xvt_w_get_cursor_color	(TERMWIN *tw);
extern void			xvt_w_set_fg			(TERMWIN *tw, int fg, int force);
extern void			xvt_w_set_bg			(TERMWIN *tw, int bg, int force);

/*------------------------------------------------------------------------
 * font functions
 */
extern void			xvt_w_font_chg_no		(TERMWIN *tw,
												int direction);
extern void			xvt_w_font_set_no		(TERMWIN *tw,
												int font_no);
extern void			xvt_w_font_set_name		(TERMWIN *tw,
												int font_no,
												const char *font_name);
extern const char *	xvt_w_font_get_name		(TERMWIN *tw,
												int font_no);
extern void			xvt_w_font_set_type		(TERMWIN *tw,
												int font_type);

/*------------------------------------------------------------------------
 * drawing functions
 */
extern void			xvt_w_draw_rect_fill	(TERMWIN *tw,
												int x, int y,
												int w, int h);

extern void			xvt_w_draw_rect_copy	(TERMWIN *tw,
												int dx, int dy,
												int dw, int dh,
												int sx, int sy);

extern void			xvt_w_draw_char_rect	(TERMWIN *tw,
												int x, int y);
extern void			xvt_w_draw_char_line	(TERMWIN *tw,
												int x, int y);
extern void			xvt_w_draw_char_disp	(TERMWIN *tw,
												int x, int y, int c);
extern void			xvt_w_draw_char_bold	(TERMWIN *tw,
												int x, int y, int c);

/*------------------------------------------------------------------------
 * event functions
 */
extern int			xvt_w_event_get			(TERMWIN *tw, XVT_EVENT *xe);
extern int			xvt_w_event_kbd_query	(TERMWIN *tw);

/*------------------------------------------------------------------------
 * pointer functions
 */
extern int			xvt_w_ptr_value			(const char *name);
extern const char *	xvt_w_ptr_name			(int n);
extern int			xvt_w_ptr_code			(int n);

/*------------------------------------------------------------------------
 * clipboard functions
 */
extern int			xvt_w_cb_copy_to		(TERMWIN *tw);

extern unsigned char *
					xvt_w_cb_copy_from		(TERMWIN *tw, int *plen);

#endif /* XVTDISPLAY_H */
