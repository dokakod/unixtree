/*------------------------------------------------------------------------
 * header for X11 window routines
 */
#ifndef XVTX_H
#define XVTX_H

#include <X11/Intrinsic.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * font-info struct
 */
struct font_info
{
	char *			reg_fname;				/* reg  font name			*/
	char *			bld_fname;				/* bold font name			*/

	XFontStruct *	reg_font;				/* ptr to reg  font			*/
	XFontStruct *	bld_font;				/* ptr to bold font or 0	*/

	int				cell_width;				/* width  of font char		*/
	int				cell_height;			/* height of font char		*/

	int				cell_xhome;				/* x-offset of font pos		*/
	int				cell_yhome;				/* y-offset of font pos		*/
};
typedef struct font_info	FONT_INFO;

/*------------------------------------------------------------------------
 * color-table
 */
struct clr_tbl
{
	Pixel			pix;					/* pixel value				*/
	int				got;					/* TRUE if allocated		*/
	char *			name;					/* color name				*/
};
typedef struct clr_tbl	CLR_TBL;

/*------------------------------------------------------------------------
 * scrollbar struct
 */
struct scrollbar
{
	int				beg;		/* beg of slider sub-window		*/
	int				end;		/* end of slider sub-window		*/
	int				top;		/* top of slider				*/
	int				bot;		/* bot of slider				*/
	int				last_top;	/* last top value				*/
	int				last_bot;	/* last bot value				*/
	int				state;		/* scrollbar state				*/
};
typedef struct scrollbar SCROLLBAR;

#define SB_UP		(+1)
#define SB_DN		(-1)

#define scrollbar_visible(tw)	(tw->X->sb.state != 0)
#define scrollbar_isUp(tw)		(tw->X->sb.state == 'U')
#define scrollbar_isDn(tw)		(tw->X->sb.state == 'D')
#define scrollbar_size(tw)		(tw->X->sb.end - tw->X->sb.beg)

/*------------------------------------------------------------------------
 * x-data struct
 */
struct x_data
{
	/*--------------------------------------------------------------------
	 * display stuff
	 */
	Display *		display;				/* X display				*/
	Visual *		visual;					/* X visual					*/
	int				screen;					/* X screen number			*/
	int				depth;					/* X display depth			*/
	int				xfd;					/* X connection number		*/

	/*--------------------------------------------------------------------
	 * window stuff
	 */
	Window			win_root;				/* X root		window		*/
	Window			win_parent;				/* X parent		window		*/
	Window			win_vt;					/* X term		window		*/
	Window			win_sb;					/* X scrollbar	window		*/
	Window			win_icon;				/* X icon		window		*/

	XSizeHints		s_hints;				/* X size		hints		*/
	XWMHints		w_hints;				/* X win mgr	hints		*/
	XClassHint		c_hints;				/* X class		hints		*/

	/*--------------------------------------------------------------------
	 * other stuff
	 */
	GC				gc_vt;					/* X GC for vt window		*/
	GC				gc_sb;					/* X GC for sb window		*/
	GC				gc_sb_ts;				/* X GC for sb top shadow	*/
	GC				gc_sb_bs;				/* X GC for sb bot shadow	*/

	XImage *		image;					/* X image for icon window	*/
	Cursor			cursor;					/* X Cursor being used		*/
	Pixmap			icon;					/* X icon of window			*/
	Pixmap			fill_stipple;			/* X stipple pixmap in GC	*/
	Atom			wm_delete;				/* X WM delete message		*/
	Atom			cb_text;				/* X clipboard message		*/

	/*--------------------------------------------------------------------
	 * color stuff
	 */
	Colormap		cmap;					/* X colormap				*/
	CLR_TBL			clr_tbl[XVT_CMS_SIZE];	/* color pixel entries		*/

	/*--------------------------------------------------------------------
	 * font stuff
	 */
	FONT_INFO		fonts[XVT_NUM_FONTS];	/* font info structs		*/

	/*--------------------------------------------------------------------
	 * scrollbar info
	 */
	SCROLLBAR		sb;						/* scrollbar info			*/

	/*--------------------------------------------------------------------
	 * window info
	 */
	int				vt_x;					/* vt x position			*/
	int				vt_y;					/* vt y position			*/
	int				vt_width;				/* vt width  in pixels		*/
	int				vt_height;				/* vt height in pixels		*/

	int				sb_x;					/* sb x position			*/
	int				sb_y;					/* sb y position			*/
	int				sb_width;				/* sb width  in pixels		*/
	int				sb_height;				/* sb height in pixels		*/

	int				ico_width;				/* icon width  in pixels	*/
	int				ico_height;				/* icon height in pixels	*/
};

/*------------------------------------------------------------------------
 * misc macros
 */
#define COLOR_PIX(tw, n)		((tw)->X->clr_tbl[n].pix)
#define COLOR_GOT(tw, n)		((tw)->X->clr_tbl[n].got)

#define CHAR_WIDTH(tw)			((tw)->X->fonts[(tw)->cur_font_no].cell_width)
#define CHAR_HEIGHT(tw)			((tw)->X->fonts[(tw)->cur_font_no].cell_height)

#define CHAR_X_OFFSET(tw)		((tw)->X->fonts[(tw)->cur_font_no].cell_xhome)
#define CHAR_Y_OFFSET(tw)		((tw)->X->fonts[(tw)->cur_font_no].cell_yhome)

#define W_TO_COL(tw, w)			((w) / CHAR_WIDTH (tw))
#define H_TO_ROW(tw, h)			((h) / CHAR_HEIGHT(tw))

#define COL_TO_W(tw, x)			((x) * CHAR_WIDTH (tw))
#define ROW_TO_H(tw, y)			((y) * CHAR_HEIGHT(tw))

#define COL_TO_X(tw, x)			(COL_TO_W(tw, (x)) + (tw)->extra)
#define ROW_TO_Y(tw, y)			(ROW_TO_H(tw, (y)) + (tw)->extra)

#define X_TO_COL(tw, x)			(W_TO_COL(tw, (x) - (tw)->extra))
#define Y_TO_ROW(tw, y)			(H_TO_ROW(tw, (y) - (tw)->extra))

#define X_ON_L_BORDER(tw, x)	((x) <  (tw)->border)
#define X_ON_R_BORDER(tw, x)	((x) >= ((tw)->X->vt_width  - (tw)->extra))

#define Y_ON_T_BORDER(tw, y)	((y) <  (tw)->border)
#define Y_ON_B_BORDER(tw, y)	((y) >= ((tw)->X->vt_height - (tw)->extra))

#define X_ON_BORDER(tw, x)		(X_ON_L_BORDER(tw, x) || X_ON_R_BORDER(tw, x))
#define Y_ON_BORDER(tw, y)		(Y_ON_T_BORDER(tw, y) || Y_ON_B_BORDER(tw, y))

/*------------------------------------------------------------------------
 * internal functions
 */
extern int		xvt_w_error_rtn_dummy	(Display *display, XErrorEvent *ev);
extern int		xvt_w_error_rtn			(Display *display, XErrorEvent *ev);

extern void		xvt_w_setup_color		(TERMWIN *tw, XVT_DATA *xd);
extern void		xvt_w_close_color		(TERMWIN *tw);

extern int		xvt_w_setup_fonts		(TERMWIN *tw, XVT_DATA *xd);
extern void		xvt_w_close_fonts		(TERMWIN *tw);

extern void		xvt_w_draw_border		(TERMWIN *tw);
extern void		xvt_w_resize_win		(TERMWIN *tw, int x, int y);

extern int		xvt_w_mc_char_present	(XFontStruct *fp, int ch);
extern void		xvt_w_mc_char_disp		(TERMWIN *tw, int x, int y, int ch);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* XVTX_H */
