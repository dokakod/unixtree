/*------------------------------------------------------------------------
 * scrollbar routines
 */
#include "xvtcommon.h"
#include "xvtx.h"

/*------------------------------------------------------------------------
 * draw triangular button with a shadow of SHADOW (1 or 2) pixels
 */
void xvt_sb_draw_button (TERMWIN *tw, int x, int y, int state, int dirn)
{
	const unsigned int	sz	= (XVT_SB_WIDTH);
	const unsigned int	sz2	= (XVT_SB_WIDTH / 2);
	XPoint				pt[3];
	GC					top, bot;

	/*--------------------------------------------------------------------
	 * get proper GC pointers
	 */
	switch (state)
	{
	case SB_UP:
		top = tw->X->gc_sb_ts;
		bot = tw->X->gc_sb_bs;
		break;
	case SB_DN:
		top = tw->X->gc_sb_bs;
		bot = tw->X->gc_sb_ts;
		break;
	default:
		top = tw->X->gc_sb;
		bot = tw->X->gc_sb;
		break;
	}

	/*--------------------------------------------------------------------
	 * now fill the triangle
	 */
	pt[0].x = x;
	pt[1].x = x + sz - 1;
	pt[2].x = x + sz2;
	if (dirn == SB_UP)
	{
		pt[0].y =
		pt[1].y = y + sz - 1;
		pt[2].y = y;
	}
	else
	{
		pt[0].y =
		pt[1].y = y;
		pt[2].y = y + sz - 1;
	}
	XFillPolygon(tw->X->display, tw->X->win_sb, tw->X->gc_sb,
		pt, 3, Convex, CoordModeOrigin);

	/*--------------------------------------------------------------------
	 * draw the base
	 */
	XDrawLine(tw->X->display, tw->X->win_sb, (dirn == SB_UP ? bot : top),
		pt[0].x, pt[0].y, pt[1].x, pt[1].y);

	/*--------------------------------------------------------------------
	 * draw the shadow on the left
	 */
	pt[1].x = x + sz2 - 1;
	pt[1].y = y + (dirn == SB_UP ? 0 : sz - 1);
	XDrawLine(tw->X->display, tw->X->win_sb, top,
		pt[0].x, pt[0].y, pt[1].x, pt[1].y);

#if (XVT_SB_SHADOW > 1)
	/*--------------------------------------------------------------------
	 * double the left shadow
	 */
	pt[0].x++;
	if (dirn == SB_UP)
	{
		pt[0].y--;
		pt[1].y++;
	}
	else
	{
		pt[0].y++;
		pt[1].y--;
	}
	XDrawLine(tw->X->display, tw->X->win_sb, top,
		pt[0].x, pt[0].y, pt[1].x, pt[1].y);
#endif

	/*--------------------------------------------------------------------
	 * draw the shadow on the right
	 */
	pt[1].x = x + sz - 1;
	pt[1].y = y + (dirn == SB_UP ? sz - 1 : 0);
	pt[2].y = y + (dirn == SB_UP ? 0 : sz - 1);
	XDrawLine(tw->X->display, tw->X->win_sb, bot,
		pt[2].x, pt[2].y, pt[1].x, pt[1].y);

#if (XVT_SB_SHADOW > 1)
	/*--------------------------------------------------------------------
	 * double the right shadow
	 */
	pt[1].x--;
	if (dirn == SB_UP)
	{
		pt[2].y++;
		pt[1].y--;
	}
	else
	{
		pt[2].y--;
		pt[1].y++;
	}
	XDrawLine(tw->X->display, tw->X->win_sb, bot,
		pt[2].x, pt[2].y, pt[1].x, pt[1].y);
#endif
}

/*------------------------------------------------------------------------
 * Draw top/left and bottom/right border shadows around windows
 */
void xvt_sb_draw_shadow (TERMWIN *tw, GC topShadow, GC botShadow,
	int x, int y, int w, int h)
{
    int	x1, y1, w1, h1, s, shadow;

    shadow = (w == 0 || h == 0) ? 1 : XVT_SB_SHADOW;

    w1 = w + x - 1;
    h1 = h + y - 1;
    x1 = x;
    y1 = y;
    for (s=0; s<shadow; x1++, y1++, w1--, h1--, s++)
	{
        XDrawLine(tw->X->display, tw->X->win_sb, topShadow, x1, y1, w1, y1);
        XDrawLine(tw->X->display, tw->X->win_sb, topShadow, x1, y1, x1, h1);
    }

    w1 = w + x - 1;
    h1 = h + y - 1;
    x1 = x + 1;
    y1 = y + 1;
    for (s=0; s<shadow; x1++, y1++, w1--, h1--, s++)
	{
        XDrawLine(tw->X->display, tw->X->win_sb, botShadow, w1, h1, w1, y1);
        XDrawLine(tw->X->display, tw->X->win_sb, botShadow, w1, h1, x1, h1);
    }
}

/*------------------------------------------------------------------------
 * update the scrollbar display
 */
int xvt_sb_display (TERMWIN *tw, int update)
{
	int			 xsb = 0;

	if (! scrollbar_visible(tw))
		return 0;

	if (update)
	{
		int			 top = (tw->nscrolled - tw->view_start);
		int			 bot = top + (tw->win_rows - 1);
		int			 len;

		len = (tw->nscrolled + (tw->win_rows - 1));
		if (len <= 1)
			len = 1;

		tw->X->sb.top = (tw->X->sb.beg + (top * scrollbar_size(tw)) / len);
		tw->X->sb.bot = (tw->X->sb.beg + (bot * scrollbar_size(tw)) / len
			+ XVT_SB_MINHEIGHT);

		/*----------------------------------------------------------------
		 * check if changed
		 */
		if ((tw->X->sb.top == tw->X->sb.last_top) &&
		    (tw->X->sb.bot == tw->X->sb.last_bot))
		{
			return 0;
		}
	}

	if (tw->X->sb.last_top < tw->X->sb.top)
	{
		XClearArea(tw->X->display, tw->X->win_sb,
			tw->sb_shadow + xsb, tw->X->sb.last_top,
			tw->X->sb_width, (tw->X->sb.top - tw->X->sb.last_top),
			False);
	}

	if (tw->X->sb.bot < tw->X->sb.last_bot)
	{
		XClearArea(tw->X->display, tw->X->win_sb,
			tw->sb_shadow + xsb, tw->X->sb.bot,
			tw->X->sb_width, (tw->X->sb.last_bot - tw->X->sb.bot),
			False);
	}

	tw->X->sb.last_top = tw->X->sb.top;
	tw->X->sb.last_bot = tw->X->sb.bot;

	/*--------------------------------------------------------------------
	 * draw the slider
	 */
	{
		int			 xofs;

		if (tw->sb_right)
			xofs = 0;
		else
			xofs = (tw->sb_shadow) ? XVT_SB_WIDTH : XVT_SB_WIDTH - 1;

		XDrawLine(tw->X->display, tw->X->win_sb, tw->X->gc_sb_bs,
			xofs, 0, xofs, tw->X->sb.end + XVT_SB_WIDTH);
	}
	XFillRectangle(tw->X->display, tw->X->win_sb, tw->X->gc_sb,
		tw->sb_shadow, tw->X->sb.top,
		tw->X->sb_width, (tw->X->sb.bot - tw->X->sb.top));

	/*--------------------------------------------------------------------
	 * draw the trough shadow if enabled
	 */
	if (tw->sb_shadow)
	{
		xvt_sb_draw_shadow(tw,
			tw->X->gc_sb_bs, tw->X->gc_sb_ts,
			0, 0,
			(tw->X->sb_width + 2 * tw->sb_shadow),
			(tw->X->sb.end + (tw->X->sb_width + 1) + tw->sb_shadow));
	}

	/*--------------------------------------------------------------------
	 * draw the slider shadow
	 */
	xvt_sb_draw_shadow(tw, tw->X->gc_sb_ts, tw->X->gc_sb_bs,
		tw->sb_shadow, tw->X->sb.top, tw->X->sb_width,
		(tw->X->sb.bot - tw->X->sb.top));

	/*--------------------------------------------------------------------
	 * Redraw scrollbar arrows
	 */
	xvt_sb_draw_button(tw, tw->sb_shadow, tw->sb_shadow,
		(scrollbar_isUp(tw)? -1 : +1), SB_UP);
	xvt_sb_draw_button(tw, tw->sb_shadow, (tw->X->sb.end + 1),
		(scrollbar_isDn(tw)? -1 : +1), SB_DN);

	return 1;
}
