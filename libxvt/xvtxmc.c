/*------------------------------------------------------------------------
 * routine to draw missing chars from a font
 *
 * These tables were taken from the XFree86 xterm source
 * (xc/programs/xterm/fontutils.c).
 */
#include "xvtcommon.h"
#include "xvtx.h"

/*------------------------------------------------------------------------
 * Table of missing chars.
 *
 * The grid is abitrary, enough resolution that nothing's lost.
 */
#define BOX_HIGH	60
#define BOX_WIDE	60

#define SCALE_X(x,w)	x = (x * (w - 1)) / (BOX_WIDE - 1)
#define SCALE_Y(y,h)	y = (y * (h - 1)) / (BOX_HIGH - 1)

typedef struct
{
	int		x_beg;
	int		y_beg;
	int		x_end;
	int		y_end;
} LineSeg;

static const LineSeg	xvt_w_mc_diamond[] =
{
	{ BOX_WIDE*1/2,	BOX_HIGH*1/4,	BOX_WIDE*3/4,	BOX_HIGH*1/2	},
	{ BOX_WIDE*3/4,	BOX_HIGH*1/2,	BOX_WIDE*1/2,	BOX_HIGH*3/4	},
	{ BOX_WIDE*1/2,	BOX_HIGH*3/4,	BOX_WIDE*1/4,	BOX_HIGH*1/2	},
	{ BOX_WIDE*1/4,	BOX_HIGH*1/2,	BOX_WIDE*1/2,	BOX_HIGH*1/4	},
	{ BOX_WIDE*1/2,	BOX_HIGH*1/3,	BOX_WIDE*2/3,	BOX_HIGH*1/2	},
	{ BOX_WIDE*2/3,	BOX_HIGH*1/2,	BOX_WIDE*1/2,	BOX_HIGH*2/3	},
	{ BOX_WIDE*1/2,	BOX_HIGH*2/3,	BOX_WIDE*1/3,	BOX_HIGH*1/2	},
	{ BOX_WIDE*1/3,	BOX_HIGH*1/2,	BOX_WIDE*1/2,	BOX_HIGH*1/3	},
	{ BOX_WIDE*1/4,	BOX_HIGH*1/2,	BOX_WIDE*3/4,	BOX_HIGH*1/2	},
	{ BOX_WIDE*1/2,	BOX_HIGH*1/4,	BOX_WIDE*1/2,	BOX_HIGH*3/4	},
	{ -1 }
};
static const LineSeg	xvt_w_mc_checker_board[] =
{
	{ BOX_WIDE*1/4,	0,				BOX_WIDE*1/4,	BOX_HIGH		},
	{ BOX_WIDE*1/2,	0,				BOX_WIDE*1/2,	BOX_HIGH		},
	{ BOX_WIDE*3/4,	0,				BOX_WIDE*3/4,	BOX_HIGH		},
	{ 0,			BOX_HIGH*1/4,	BOX_WIDE,		BOX_HIGH*1/4	},
	{ 0,			BOX_HIGH*1/2,	BOX_WIDE,		BOX_HIGH*1/2	},
	{ 0,			BOX_HIGH*3/4,	BOX_WIDE,		BOX_HIGH*3/4	},
	{ -1 }
};
static const LineSeg	xvt_w_mc_degrees[] =
{
	{ BOX_WIDE*1/2,	BOX_HIGH*1/4,	BOX_WIDE*2/3,	BOX_HIGH*3/8	},
	{ BOX_WIDE*2/3,	BOX_HIGH*3/8,	BOX_WIDE*1/2,	BOX_HIGH*1/2	},
	{ BOX_WIDE*1/2,	BOX_HIGH*1/2,	BOX_WIDE*1/3,	BOX_HIGH*3/8	},
	{ BOX_WIDE*1/3,	BOX_HIGH*3/8,	BOX_WIDE*1/2,	BOX_HIGH*1/4	},
	{ -1 }
};
static const LineSeg	xvt_w_mc_lower_right_corner[] =
{
	{ 0,			BOX_HIGH*1/2,	BOX_WIDE*1/2,	BOX_HIGH*1/2	},
	{ BOX_WIDE*1/2,	BOX_HIGH*1/2,	BOX_WIDE*1/2,	0				},
	{ -1 }
};
static const LineSeg	xvt_w_mc_upper_right_corner[] =
{
	{ 0,			BOX_HIGH*1/2,	BOX_WIDE*1/2,	BOX_HIGH*1/2	},
	{ BOX_WIDE*1/2,	BOX_HIGH*1/2,	BOX_WIDE*1/2,	BOX_HIGH		},
	{ -1 }
};
static const LineSeg	xvt_w_mc_upper_left_corner[] =
{
	{ BOX_WIDE*1/2,	BOX_HIGH*1/2,	BOX_WIDE,		BOX_HIGH*1/2	},
	{ BOX_WIDE*1/2,	BOX_HIGH*1/2,	BOX_WIDE*1/2,	BOX_HIGH		},
	{ -1 }
};
static const LineSeg	xvt_w_mc_lower_left_corner[] =
{
	{ BOX_WIDE*1/2,	0,				BOX_WIDE*1/2,	BOX_HIGH*1/2	},
	{ BOX_WIDE*1/2,	BOX_HIGH*1/2,	BOX_WIDE,		BOX_HIGH*1/2	},
	{ -1 }
};
static const LineSeg	xvt_w_mc_cross[] =
{
	{ 0,			BOX_HIGH*1/2,	BOX_WIDE,		BOX_HIGH*1/2	},
	{ BOX_WIDE*1/2,	0,				BOX_WIDE*1/2,	BOX_HIGH		},
	{ -1 }
};
static const LineSeg	xvt_w_mc_scan_line_1[] =
{
	{ 0,			0,				BOX_WIDE,		0				},
	{ -1 }
};
static const LineSeg	xvt_w_mc_scan_line_3[] =
{
	{ 0,			BOX_HIGH*1/4,	BOX_WIDE,		BOX_HIGH*1/4	},
	{ -1 }
};
static const LineSeg	xvt_w_mc_scan_line_7[] =
{
	{ 0,			BOX_HIGH*1/2,	BOX_WIDE,		BOX_HIGH*1/2	},
	{ -1 }
};
static const LineSeg	xvt_w_mc_scan_line_9[] =
{
	{ 0,			BOX_HIGH*3/4,	BOX_WIDE,		BOX_HIGH*3/4	},
	{ -1 }
};
static const LineSeg	xvt_w_mc_horizontal_line[] =
{
	{ 0,			BOX_HIGH,		BOX_WIDE,		BOX_HIGH		},
	{ -1 }
};
static const LineSeg	xvt_w_mc_left_tee[] =
{
	{ BOX_WIDE*1/2,	0,				BOX_WIDE*1/2,	BOX_HIGH		},
	{ BOX_WIDE*1/2,	BOX_HIGH*1/2,	BOX_WIDE,		BOX_HIGH*1/2	},
	{ -1 }
};
static const LineSeg	xvt_w_mc_right_tee[] =
{
	{ BOX_WIDE*1/2,	0,				BOX_WIDE*1/2,	BOX_HIGH		},
	{ BOX_WIDE*1/2,	BOX_HIGH*1/2,	0,				BOX_HIGH*1/2	},
	{ -1 }
};
static const LineSeg	xvt_w_mc_bottom_tee[] =
{
	{ 0,			BOX_HIGH*1/2,	BOX_WIDE,		BOX_HIGH*1/2	},
	{ BOX_WIDE*1/2,	0,				BOX_WIDE*1/2,	BOX_HIGH*1/2	},
	{ -1 }
};
static const LineSeg	xvt_w_mc_top_tee[] =
{
	{ 0,			BOX_HIGH*1/2,	BOX_WIDE,		BOX_HIGH*1/2	},
	{ BOX_WIDE*1/2,	BOX_HIGH*1/2,	BOX_WIDE*1/2,	BOX_HIGH		},
	{ -1 }
};
static const LineSeg	xvt_w_mc_vertical_line[] =
{
	{ BOX_WIDE*1/2,	0,				BOX_WIDE*1/2,	BOX_HIGH		},
	{ -1 }
};
static const LineSeg	xvt_w_mc_less_than_or_equal[] =
{
	{ BOX_WIDE*5/6,	BOX_HIGH*1/6,	BOX_WIDE*1/5,	BOX_HIGH*1/2	},
	{ BOX_WIDE*5/6,	BOX_HIGH*5/6,	BOX_WIDE*1/5,	BOX_HIGH*1/2	},
	{ BOX_WIDE*1/6,	BOX_HIGH*5/6,	BOX_WIDE*5/6,	BOX_HIGH*5/6	},
	{ -1 }
};
static const LineSeg	xvt_w_mc_greater_than_or_equal[] =
{
	{ BOX_WIDE*1/6,	BOX_HIGH*1/6,	BOX_WIDE*5/6,	BOX_HIGH*1/2	},
	{ BOX_WIDE*1/6,	BOX_HIGH*5/6,	BOX_WIDE*5/6,	BOX_HIGH*1/2	},
	{ BOX_WIDE*1/6,	BOX_HIGH*5/6,	BOX_WIDE*5/6,	BOX_HIGH*5/6	},
	{ -1 }
};

static const LineSeg *	xvt_w_mc_lines[] =
{
	0,								/* 00 */	/* solid block		*/
	xvt_w_mc_diamond,				/* 01 */
	xvt_w_mc_checker_board,			/* 02 */
	0,								/* 03 */	/* HT				*/
	0,								/* 04 */	/* FF				*/
	0,								/* 05 */	/* CR				*/
	0,								/* 06 */	/* LF				*/
	xvt_w_mc_degrees,				/* 07 */
	0,								/* 08 */	/* plus-minus		*/
	0,								/* 09 */	/* NL				*/
	0,								/* 0A */	/* VT				*/
	xvt_w_mc_lower_right_corner,	/* 0B */
	xvt_w_mc_upper_right_corner,	/* 0C */
	xvt_w_mc_upper_left_corner,		/* 0D */
	xvt_w_mc_lower_left_corner,		/* 0E */
	xvt_w_mc_cross,					/* 0F */
	xvt_w_mc_scan_line_1,			/* 10 */
	xvt_w_mc_scan_line_3,			/* 11 */
	xvt_w_mc_scan_line_7,			/* 12 */
	xvt_w_mc_scan_line_9,			/* 13 */
	xvt_w_mc_horizontal_line,		/* 14 */
	xvt_w_mc_left_tee,				/* 15 */
	xvt_w_mc_right_tee,				/* 16 */
	xvt_w_mc_bottom_tee,			/* 17 */
	xvt_w_mc_top_tee,				/* 18 */
	xvt_w_mc_vertical_line,			/* 19 */
	xvt_w_mc_less_than_or_equal,	/* 1A */
	xvt_w_mc_greater_than_or_equal,	/* 1B */
	0,								/* 1C */	/* PI				*/
	0,								/* 1D */	/* not-equal		*/
	0,								/* 1E */	/* English pound	*/
	0								/* 1F */
};

#define NUM_MC_CHARS	(sizeof(xvt_w_mc_lines) / sizeof(*xvt_w_mc_lines))

/*------------------------------------------------------------------------
 * Draw the given graphic character, if it is simple enough (i.e., a
 * line-drawing character).
 */
void xvt_w_mc_char_disp (TERMWIN *tw, int x, int y, int ch)
{
	const LineSeg *	ls;

	/*--------------------------------------------------------------------
	 * we always clear out the rectangle, even if we don't have the char
	 */
	XFillRectangle(tw->X->display, tw->X->win_vt, tw->X->gc_vt,
		x, y, CHAR_WIDTH(tw), CHAR_HEIGHT(tw));

	/*--------------------------------------------------------------------
	 * now bail if we don't have the char
	 */
	if (ch < 0 || ch > NUM_MC_CHARS)
		return;

	ls = xvt_w_mc_lines[ch];
	if (ls == 0)
		return;

	/*--------------------------------------------------------------------
	 * now draw the char
	 */
	XSetFillStyle(tw->X->display, tw->X->gc_vt, FillSolid);

	for (; ls->x_beg >= 0; ls++)
	{
		int	x_beg	= ls->x_beg;
		int	y_beg	= ls->y_beg;
		int	x_end	= ls->x_end;
		int	y_end	= ls->y_end;

		SCALE_X(x_beg, CHAR_WIDTH(tw));	SCALE_Y(y_beg, CHAR_HEIGHT(tw));
		SCALE_X(x_end, CHAR_WIDTH(tw));	SCALE_Y(y_end, CHAR_HEIGHT(tw));

		XDrawLine(tw->X->display, tw->X->win_vt, tw->X->gc_vt,
				x + x_beg, y + y_beg, x + x_end, y + y_end);
	}

	XSetFillStyle(tw->X->display, tw->X->gc_vt, FillOpaqueStippled);
}

/*------------------------------------------------------------------------
 * query whether a given char is in a font
 *
 * returns TRUE/FALSE
 */
int xvt_w_mc_char_present (XFontStruct *fp, int ch)
{
	XCharStruct *	cp;

#if TESTING
	if (fp != 0) return (FALSE);
#endif

	if (fp == 0 || fp->per_char == 0)
		return (FALSE);

	if (fp->all_chars_exist)
		return (TRUE);

	if (ch < fp->min_char_or_byte2 || ch > fp->max_char_or_byte2)
		return (FALSE);

	cp = fp->per_char + (ch - fp->min_char_or_byte2);

	if (cp->width == 0)
		return (FALSE);

	if (cp->lbearing == 0 && cp->rbearing == 0 &&
	    cp->ascent   == 0 && cp->descent  == 0)
	{
		return (FALSE);
	}

	return (TRUE);
}
