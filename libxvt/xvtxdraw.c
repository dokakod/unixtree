/*------------------------------------------------------------------------
 * X draw routines
 */
#include "xvtcommon.h"
#include "xvtx.h"

/*------------------------------------------------------------------------
 * draw the window border & clean margin area
 */
void xvt_w_draw_border (TERMWIN *tw)
{
	int			x1;
	int			y1;
	int			x2;
	int			y2;
	XRectangle	r[4];

	/*--------------------------------------------------------------------
	 * do the border
	 */
	if (tw->border > 0)
	{
		x1	= 0;
		y1	= 0;
		x2	= tw->X->vt_width  - tw->border;
		y2	= tw->X->vt_height - tw->border;

		/* left border */

		r[0].x		= x1;
		r[0].y		= y1;
		r[0].width	= tw->border;
		r[0].height	= tw->X->vt_height;

		/* top border */

		r[1].x		= x1;
		r[1].y		= y1;
		r[1].width	= tw->X->vt_width;
		r[1].height	= tw->border;

		/* right border */

		r[2].x		= x2;
		r[2].y		= y1;
		r[2].width	= tw->border;
		r[2].height	= tw->X->vt_height;

		/* bottom border */

		r[3].x		= x1;
		r[3].y		= y2;
		r[3].width	= tw->X->vt_width;
		r[3].height	= tw->border;

		XSetForeground(tw->X->display, tw->X->gc_vt,
			COLOR_PIX(tw, XVT_CLR_BD));
		XSetBackground(tw->X->display, tw->X->gc_vt,
			COLOR_PIX(tw, XVT_CLR_BD));

		XFillRectangles(tw->X->display, tw->X->win_vt, tw->X->gc_vt, r, 4);

		XSetForeground(tw->X->display, tw->X->gc_vt,
			COLOR_PIX(tw, tw->cur_fg));
		XSetBackground(tw->X->display, tw->X->gc_vt,
			COLOR_PIX(tw, tw->cur_bg));
	}

	/*--------------------------------------------------------------------
	 * do the margin
	 */
	if (tw->margin > 0)
	{
		x1	= tw->border;
		y1	= tw->border;
		x2	= tw->X->vt_width  - tw->extra;
		y2	= tw->X->vt_height - tw->extra;

		/* left side */

		r[0].x		= x1;
		r[0].y		= y1;
		r[0].width	= tw->margin;
		r[0].height	= tw->X->vt_height;

		/* top side */

		r[1].x		= x1;
		r[1].y		= y1;
		r[1].width	= tw->X->vt_width;
		r[1].height	= tw->margin;

		/* right side */

		r[2].x		= x2;
		r[2].y		= y1;
		r[2].width	= tw->margin;
		r[2].height	= tw->X->vt_height;

		/* bottom side */

		r[3].x		= x1;
		r[3].y		= y2;
		r[3].width	= tw->X->vt_width;
		r[3].height	= tw->margin;

		XSetForeground(tw->X->display, tw->X->gc_vt,
			COLOR_PIX(tw, XVT_CLR_BG));
		XSetBackground(tw->X->display, tw->X->gc_vt,
			COLOR_PIX(tw, XVT_CLR_BG));

		XFillRectangles(tw->X->display, tw->X->win_vt, tw->X->gc_vt, r, 4);

		XSetForeground(tw->X->display, tw->X->gc_vt,
			COLOR_PIX(tw, tw->cur_fg));
		XSetBackground(tw->X->display, tw->X->gc_vt,
			COLOR_PIX(tw, tw->cur_bg));
	}
}

/*------------------------------------------------------------------------
 * fill a rectangle
 */
void xvt_w_draw_rect_fill (TERMWIN *tw, int x, int y, int w, int h)
{
	XFillRectangle(tw->X->display, tw->X->win_vt, tw->X->gc_vt,
		COL_TO_X(tw, x), ROW_TO_Y(tw, y),
		COL_TO_W(tw, w), ROW_TO_H(tw, h));
}

/*------------------------------------------------------------------------
 * copy a rectangle
 */
void xvt_w_draw_rect_copy (TERMWIN *tw, int dx, int dy,
	int dw, int dh, int sx, int sy)
{
    XCopyArea(tw->X->display, tw->X->win_vt, tw->X->win_vt, tw->X->gc_vt,
        COL_TO_X(tw, sx), ROW_TO_Y(tw, sy),
        COL_TO_W(tw, dw), ROW_TO_H(tw, dh),
        COL_TO_X(tw, dx), ROW_TO_Y(tw, dy));
}

/*------------------------------------------------------------------------
 * draw a rectangle around a character
 */
void xvt_w_draw_char_rect (TERMWIN *tw, int x, int y)
{
	XSetFillStyle (tw->X->display, tw->X->gc_vt, FillSolid);
	XDrawRectangle(tw->X->display, tw->X->win_vt, tw->X->gc_vt,
		COL_TO_X(tw, x),     ROW_TO_Y(tw, y),
		COL_TO_W(tw, 1) - 1, ROW_TO_H(tw, 1) - 1);
	XSetFillStyle (tw->X->display, tw->X->gc_vt, FillOpaqueStippled);
}

/*------------------------------------------------------------------------
 * draw a line under a char
 */
void xvt_w_draw_char_line (TERMWIN *tw, int x, int y)
{
	XSetFillStyle(tw->X->display, tw->X->gc_vt, FillSolid);

	x = COL_TO_X(tw, x) + CHAR_X_OFFSET(tw);
	y = ROW_TO_Y(tw, y) + CHAR_Y_OFFSET(tw) + 1;

	XDrawLine(tw->X->display, tw->X->win_vt, tw->X->gc_vt,
		x, y, x + COL_TO_W(tw, 1), y);

	XSetFillStyle(tw->X->display, tw->X->gc_vt, FillOpaqueStippled);
}

/*------------------------------------------------------------------------
 * draw a char
 */
void xvt_w_draw_char_disp (TERMWIN *tw, int x, int y, int c)
{
	char text[2];

	if (c < ' ')
	{
		XFontStruct *	fp;

		fp = (tw->cur_font_type == FONT_NORM ?
			tw->X->fonts[tw->cur_font_no].reg_font :
			tw->X->fonts[tw->cur_font_no].bld_font);

		if (! xvt_w_mc_char_present(fp, c))
		{
			xvt_w_mc_char_disp(tw, COL_TO_X(tw, x), ROW_TO_Y(tw, y), c);
			return;
		}
	}

	text[0] = (char)c;
	XDrawImageString(tw->X->display, tw->X->win_vt, tw->X->gc_vt,
		COL_TO_X(tw, x) + CHAR_X_OFFSET(tw),
		ROW_TO_Y(tw, y) + CHAR_Y_OFFSET(tw),
		text, 1);
}

/*------------------------------------------------------------------------
 * draw a char in bold
 */
void xvt_w_draw_char_bold (TERMWIN *tw, int x, int y, int c)
{
	if (tw->X->fonts[tw->cur_font_no].bld_font != 0)
	{
		/*----------------------------------------------------------------
		 * draw char using bold font if possible
		 */
		if (c >= ' ' ||
		    xvt_w_mc_char_present(tw->X->fonts[tw->cur_font_no].bld_font, c))
		{
			xvt_w_font_set_type(tw, FONT_BOLD);
			xvt_w_draw_char_disp(tw, x, y, c);
			xvt_w_font_set_type(tw, FONT_NORM);

			return;
		}
	}

	/*--------------------------------------------------------------------
	 * draw char, then copy the char over 1 pixel
	 */
	xvt_w_draw_char_disp(tw, x, y, c);

	XSetFunction(tw->X->display, tw->X->gc_vt, GXor);
	XCopyArea(tw->X->display, tw->X->win_vt, tw->X->win_vt, tw->X->gc_vt,
		COL_TO_X(tw, x),     ROW_TO_Y(tw, y),
		CHAR_WIDTH(tw) - 1,  CHAR_HEIGHT(tw),
		COL_TO_X(tw, x) + 1, ROW_TO_Y(tw, y));
	XSetFunction(tw->X->display, tw->X->gc_vt, GXcopy);
}
