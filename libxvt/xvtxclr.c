/*------------------------------------------------------------------------
 * X Color routines
 */
#include "xvtcommon.h"
#include "xvtx.h"

/*------------------------------------------------------------------------
 * Color arrays
 *
 * These arrays contain a default RGB value in case we can't parse the
 * color name, and a "name" describing the color entry.
 */
#define ALL3		( DoRed | DoGreen | DoBlue )
#define PIXL(n)		( (n) << 8 )
#define CMAP(r,g,b)	{ 0, PIXL(r), PIXL(g), PIXL(b), ALL3, 0 }

struct x_color
{
	XColor			clr_xcolor;		/* default XColor entry	*/
	const char *	clr_name;		/* color name			*/
};
typedef struct x_color	X_COLOR;

static const X_COLOR xvt_w_colors[] =
{
	/*--------------------------------------------------------------------
	 * screen colors
	 */
	{ CMAP(0x00, 0x00, 0x00),	"black"			},
	{ CMAP(0xcd, 0x00, 0x00),	"red"			},
	{ CMAP(0x00, 0xcd, 0x00),	"green"			},
	{ CMAP(0xcd, 0xcd, 0x00),	"yellow"		},
	{ CMAP(0x00, 0x00, 0xcd),	"blue"			},
	{ CMAP(0xcd, 0x00, 0xcd),	"magenta"		},
	{ CMAP(0x00, 0xcd, 0xcd),	"cyan"			},
	{ CMAP(0xfa, 0xeb, 0xd7),	"white"			},
 
	{ CMAP(0x40, 0x40, 0x40),	"grey"			},
	{ CMAP(0xff, 0x00, 0x00),	"lt-red"		},
	{ CMAP(0x00, 0xff, 0x00),	"lt-green"		},
	{ CMAP(0xff, 0xff, 0x00),	"lt-yellow"		},
	{ CMAP(0x00, 0x00, 0xff),	"lt-blue"		},
	{ CMAP(0xff, 0x00, 0xff),	"lt-magenta"	},
	{ CMAP(0x00, 0xff, 0xff),	"lt-cyan"		},
	{ CMAP(0xff, 0xff, 0xff),	"lt-white"		},

	/*--------------------------------------------------------------------
	 * other colors
	 */
	{ CMAP(0x00, 0x00, 0x00),	"foreground"	},
	{ CMAP(0xff, 0xff, 0xff),	"background"	},
	{ CMAP(0x00, 0x00, 0x00),	"pointer"		},
	{ CMAP(0x00, 0x00, 0x00),	"border"		},
	{ CMAP(0x00, 0x00, 0x00),	"cursor"		},
	{ CMAP(0xff, 0xff, 0xff),	"scrollbar"		},
	{ CMAP(0x00, 0x00, 0x00),	"SB-trough"		},

	/*--------------------------------------------------------------------
	 * calculated colors
	 */
	{ CMAP(0x00, 0x00, 0x00),	"SB-top-shadow"	},
	{ CMAP(0x00, 0x00, 0x00),	"SB-bot-shadow"	}
};

/*------------------------------------------------------------------------
 * create a color name from RGB values
 */
static void xvt_w_cms_pixel_to_name (TERMWIN *tw, Pixel pix, char *buf)
{
	XColor xcolor;

	xcolor.pixel = pix;
	XQueryColor(tw->X->display, tw->X->cmap, &xcolor);

	sprintf(buf, "rgb:%04x/%04x/%04x",
		xcolor.red, xcolor.green, xcolor.blue);
}

/*------------------------------------------------------------------------
 * set a color entry
 */
static void xvt_w_cms_color_set (TERMWIN *tw, int n, Pixel p, int g,
	const char *name)
{
	CLR_TBL *	t = tw->X->clr_tbl + n;

	t->pix = p;
	t->got = g;

	if (t->name != 0)
		FREE(t->name);
	t->name = STRDUP(name);
}

/*------------------------------------------------------------------------
 * setup mono colors
 */
static void xvt_w_cms_setup_mono (TERMWIN *tw, XVT_DATA *xd)
{
	Pixel	bp	= XBlackPixel(tw->X->display, tw->X->screen);
	Pixel	wp	= XWhitePixel(tw->X->display, tw->X->screen);

	/*--------------------------------------------------------------------
	 * if mono, all colors are either black or white
	 */
	xvt_w_cms_color_set(tw, XVT_CLR_BLACK,     bp, FALSE, "black");
	xvt_w_cms_color_set(tw, XVT_CLR_RED,       bp, FALSE, "black");
	xvt_w_cms_color_set(tw, XVT_CLR_GREEN,     bp, FALSE, "black");
	xvt_w_cms_color_set(tw, XVT_CLR_YELLOW,    bp, FALSE, "black");
	xvt_w_cms_color_set(tw, XVT_CLR_BLUE,      bp, FALSE, "black");
	xvt_w_cms_color_set(tw, XVT_CLR_MAGENTA,   bp, FALSE, "black");
	xvt_w_cms_color_set(tw, XVT_CLR_CYAN,      bp, FALSE, "black");
	xvt_w_cms_color_set(tw, XVT_CLR_WHITE,     wp, FALSE, "white");

	xvt_w_cms_color_set(tw, XVT_CLR_GREY,      bp, FALSE, "black");
	xvt_w_cms_color_set(tw, XVT_CLR_LTRED,     bp, FALSE, "black");
	xvt_w_cms_color_set(tw, XVT_CLR_LTGREEN,   bp, FALSE, "black");
	xvt_w_cms_color_set(tw, XVT_CLR_LTYELLOW,  bp, FALSE, "black");
	xvt_w_cms_color_set(tw, XVT_CLR_LTBLUE,    bp, FALSE, "black");
	xvt_w_cms_color_set(tw, XVT_CLR_LTMAGENTA, bp, FALSE, "black");
	xvt_w_cms_color_set(tw, XVT_CLR_LTCYAN,    bp, FALSE, "black");
	xvt_w_cms_color_set(tw, XVT_CLR_LTWHITE,   wp, FALSE, "white");

	xvt_w_cms_color_set(tw, XVT_CLR_FG,        bp, FALSE, "black");
	xvt_w_cms_color_set(tw, XVT_CLR_BG,        wp, FALSE, "white");
	xvt_w_cms_color_set(tw, XVT_CLR_PR,        bp, FALSE, "black");
	xvt_w_cms_color_set(tw, XVT_CLR_BD,        bp, FALSE, "black");
	xvt_w_cms_color_set(tw, XVT_CLR_CR,      None, FALSE, "none");
	xvt_w_cms_color_set(tw, XVT_CLR_SB,        bp, FALSE, "black");
	xvt_w_cms_color_set(tw, XVT_CLR_ST,        bp, FALSE, "black");
	xvt_w_cms_color_set(tw, XVT_CLR_SB_TS,     wp, FALSE, "white");
	xvt_w_cms_color_set(tw, XVT_CLR_SB_BS,     wp, FALSE, "white");

	/*--------------------------------------------------------------------
	 * If reverse requested, switch FG & BG colors
	 */
	if (xd->reverse)
		xvt_w_set_rev_video(tw, TRUE);

	/*--------------------------------------------------------------------
	 * setup extended TERM name & definitions
	 */
	strcpy(xd->term_type, XVT_TERM_MONO);
	xd->term_defs = xvt_termdefs_m;
}

/*------------------------------------------------------------------------
 * parse a color
 */
static int xvt_w_parse_color (TERMWIN *tw, const char *color, XColor *xc,
	int n)
{
	char			color_buf[128];
	char *			b	= color_buf;
	const char *	s;
	int				rc;

	/*--------------------------------------------------------------------
	 * remove spaces, "-", & "_" from name
	 */
	for (s=color; *s; s++)
	{
		if (isspace(*s) || *s == '-' || *s == '_')
			continue;
		*b++ = *s;
	}
	*b = 0;

	/*--------------------------------------------------------------------
	 * try to parse the color & use default if can't
	 */
	rc = 0;
	if (XParseColor(tw->X->display, tw->X->cmap, color_buf, xc) == 0)
	{
		rc = -1;
		if (n >= 0)
			memcpy(xc, &xvt_w_colors[n].clr_xcolor, sizeof(*xc));
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * change a color
 */
int xvt_w_chg_clr (TERMWIN *tw, int n, const char *color)
{
	XColor	xcolor;
	int		rc;

	/*--------------------------------------------------------------------
	 * do some validation of args
	 */
	if (! tw->can_be_color)
		return (-1);

	if (color == 0 || *color == 0 || xvt_strccmp(color, "none") == 0)
		return (-1);

	if (n < 0 || n >= XVT_CMS_SIZE)
		return (-1);

	/*--------------------------------------------------------------------
	 * now set the color if it is valid
	 */
	rc = xvt_w_parse_color(tw, color, &xcolor, -1);
	if (rc == 0)
	{
		/*----------------------------------------------------------------
		 * allocate the new color if we can
		 */
		if (XAllocColor(tw->X->display, tw->X->cmap, &xcolor) == 0)
		{
			rc = -1;
		}
		else
		{
			XFreeColors(tw->X->display, tw->X->cmap, &tw->X->clr_tbl[n].pix,
				1, 0);
			xvt_w_cms_color_set(tw, n, xcolor.pixel, TRUE, color);
		}
	}

	/*--------------------------------------------------------------------
	 * We do some changes right here
	 */
	if (rc == 0)
	{
		switch (n)
		{
		case XVT_CLR_BD:
			/*------------------------------------------------------------
			 * new border color
			 */
			xvt_w_draw_border(tw);
			break;

		case XVT_CLR_PR:
			/*------------------------------------------------------------
			 * new pointer color
			 */
			if (tw->X->cursor != None)
			{
				XColor	cursor_fg;
				XColor	cursor_bg;

				cursor_fg.pixel = COLOR_PIX(tw, XVT_CLR_PR);
				cursor_bg.pixel = COLOR_PIX(tw, XVT_CLR_BG);

				XQueryColor(tw->X->display, tw->X->cmap, &cursor_fg);
				XQueryColor(tw->X->display, tw->X->cmap, &cursor_bg);

				XRecolorCursor(tw->X->display, tw->X->cursor,
					&cursor_fg, &cursor_bg);
			}
			break;
		}
	}
	return (rc);
}

/*------------------------------------------------------------------------
 * allocate a color
 */
static void xvt_w_set_clr (TERMWIN *tw, const char *color, int n,
	Pixel pix_dflt)
{
	char clr_buf[24];

	if (color == 0 || *color == 0)
	{
		xvt_w_cms_pixel_to_name(tw, pix_dflt, clr_buf);
		xvt_w_cms_color_set(tw, n, pix_dflt, FALSE, 0);
	}
	else if (xvt_strccmp(color, "none") == 0)
	{
		xvt_w_cms_color_set(tw, n, pix_dflt, FALSE, color);
	}
	else
	{
		XColor	xcolor;

		xvt_w_parse_color(tw, color, &xcolor, n);
		if (XAllocColor(tw->X->display, tw->X->cmap, &xcolor) == 0)
		{
			xvt_w_cms_pixel_to_name(tw, pix_dflt, clr_buf);
			xvt_w_cms_color_set(tw, n, pix_dflt, FALSE, 0);
		}
		else
		{
			xvt_w_cms_color_set(tw, n, xcolor.pixel, TRUE, color);
		}
	}
}

/*------------------------------------------------------------------------
 * calculate shadow colors
 *
 * These calculations were lifted from the rxvt source
 * (which took them from the fvwm source).
 */
static Pixel xvt_w_get_top_shadow (TERMWIN *tw, int color)
{
	XColor	xcolor;

	/*--------------------------------------------------------------------
	 * Bottom shadow:	1/2 of the color
	 */
	xcolor.pixel = COLOR_PIX(tw, color);
	XQueryColor(tw->X->display, tw->X->cmap, &xcolor);

	xcolor.red		= (xcolor.red	/ 2);
	xcolor.green	= (xcolor.green	/ 2);
	xcolor.blue		= (xcolor.blue	/ 2);

	if (XAllocColor(tw->X->display, tw->X->cmap, &xcolor) == 0)
		xcolor.pixel = COLOR_PIX(tw, color);

	return (xcolor.pixel);
}

static Pixel xvt_w_get_bot_shadow (TERMWIN *tw, int color)
{
	XColor	xcolor;
	XColor	xwhite;
	unsigned short	r, g, b;

	/*--------------------------------------------------------------------
	 * Top shadow: magic calculation
	 */
	xcolor.pixel = COLOR_PIX(tw, color);
	XQueryColor(tw->X->display, tw->X->cmap, &xcolor);

	xwhite.pixel = COLOR_PIX(tw, XVT_CLR_LTWHITE);
	XQueryColor(tw->X->display, tw->X->cmap, &xwhite);

	r = (xwhite.red   / 5);
	g = (xwhite.green / 5);
	b = (xwhite.blue  / 5);
	if (xcolor.red   < r)	xcolor.red   = r;
	if (xcolor.green < g)	xcolor.green = g;
	if (xcolor.blue  < b)	xcolor.blue  = b;

	xcolor.red   = ((xcolor.red   * 7) / 5);
	xcolor.green = ((xcolor.green * 7) / 5);
	xcolor.blue  = ((xcolor.blue  * 7) / 5);
	r = (xwhite.red);
	g = (xwhite.green);
	b = (xwhite.blue);
	if (xcolor.red   > r)	xcolor.red   = r;
	if (xcolor.green > g)	xcolor.green = g;
	if (xcolor.blue  > b)	xcolor.blue  = b;

	if (XAllocColor(tw->X->display, tw->X->cmap, &xcolor) == 0)
		xcolor.pixel = COLOR_PIX(tw, color);

	return (xcolor.pixel);
}

/*------------------------------------------------------------------------
 * setup scrollbar shadow colors
 */
static void xvt_w_set_sb_shadows (TERMWIN *tw)
{
	Pixel	pix;
	char	clr_buf[24];

	pix = xvt_w_get_top_shadow(tw, XVT_CLR_SB);
	xvt_w_cms_pixel_to_name(tw, pix, clr_buf);
	xvt_w_cms_color_set(tw, XVT_CLR_SB_TS, pix, TRUE, clr_buf);

	pix = xvt_w_get_bot_shadow(tw, XVT_CLR_SB);
	xvt_w_cms_pixel_to_name(tw, pix, clr_buf);
	xvt_w_cms_color_set(tw, XVT_CLR_SB_BS, pix, TRUE, clr_buf);
}

/*------------------------------------------------------------------------
 * set reverse video
 */
void xvt_w_set_rev_video (TERMWIN *tw, int reverse)
{
	if (reverse != tw->reverse)
	{
		CLR_TBL *	f = tw->X->clr_tbl + XVT_CLR_FG;
		CLR_TBL *	b = tw->X->clr_tbl + XVT_CLR_BG;
		Pixel		p;
		int			g;
		char *		n;

		p	    = f->pix;
		f->pix	= b->pix;
		b->pix	= p;

		g	    = f->got;
		f->got	= b->got;
		b->got	= g;

		n		= f->name;
		f->name	= b->name;
		b->name	= n;

		tw->reverse = reverse;
	}
}

/*------------------------------------------------------------------------
 * setup color colors
 */
static void xvt_w_cms_setup_color (TERMWIN *tw, XVT_DATA *xd)
{
	Pixel	bp	= XBlackPixel(tw->X->display, tw->X->screen);
	Pixel	wp	= XWhitePixel(tw->X->display, tw->X->screen);

	/*--------------------------------------------------------------------
	 * Setup all colors from user-spec or default
	 */
	xvt_w_set_clr(tw, xd->scr_color[XVT_CLR_BLACK    ], XVT_CLR_BLACK,     bp);
	xvt_w_set_clr(tw, xd->scr_color[XVT_CLR_RED      ], XVT_CLR_RED,       bp);
	xvt_w_set_clr(tw, xd->scr_color[XVT_CLR_GREEN    ], XVT_CLR_GREEN,     bp);
	xvt_w_set_clr(tw, xd->scr_color[XVT_CLR_YELLOW   ], XVT_CLR_YELLOW,    bp);
	xvt_w_set_clr(tw, xd->scr_color[XVT_CLR_BLUE     ], XVT_CLR_BLUE,      bp);
	xvt_w_set_clr(tw, xd->scr_color[XVT_CLR_MAGENTA  ], XVT_CLR_MAGENTA,   bp);
	xvt_w_set_clr(tw, xd->scr_color[XVT_CLR_CYAN     ], XVT_CLR_CYAN,      bp);
	xvt_w_set_clr(tw, xd->scr_color[XVT_CLR_WHITE    ], XVT_CLR_WHITE,     wp);

	xvt_w_set_clr(tw, xd->scr_color[XVT_CLR_GREY     ], XVT_CLR_GREY,      bp);
	xvt_w_set_clr(tw, xd->scr_color[XVT_CLR_LTRED    ], XVT_CLR_LTRED,     bp);
	xvt_w_set_clr(tw, xd->scr_color[XVT_CLR_LTGREEN  ], XVT_CLR_LTGREEN,   bp);
	xvt_w_set_clr(tw, xd->scr_color[XVT_CLR_LTYELLOW ], XVT_CLR_LTYELLOW,  bp);
	xvt_w_set_clr(tw, xd->scr_color[XVT_CLR_LTBLUE   ], XVT_CLR_LTBLUE,    bp);
	xvt_w_set_clr(tw, xd->scr_color[XVT_CLR_LTMAGENTA], XVT_CLR_LTMAGENTA, bp);
	xvt_w_set_clr(tw, xd->scr_color[XVT_CLR_LTCYAN   ], XVT_CLR_LTCYAN,    bp);
	xvt_w_set_clr(tw, xd->scr_color[XVT_CLR_LTWHITE  ], XVT_CLR_LTWHITE,   wp);

	xvt_w_set_clr(tw, xd->fg_color,                     XVT_CLR_FG,        bp);
	xvt_w_set_clr(tw, xd->bg_color,                     XVT_CLR_BG,        wp);
	xvt_w_set_clr(tw, xd->pr_color,                     XVT_CLR_PR,        bp);
	xvt_w_set_clr(tw, xd->bd_color,                     XVT_CLR_BD,        bp);
	xvt_w_set_clr(tw, xd->cr_color,                     XVT_CLR_CR,      None);
	xvt_w_set_clr(tw, xd->sb_color,                     XVT_CLR_SB,        bp);
	xvt_w_set_clr(tw, xd->st_color,                     XVT_CLR_ST,        wp);

	/*--------------------------------------------------------------------
	 * Setup scrollbar shadow colors
	 */
	xvt_w_set_sb_shadows(tw);

	/*--------------------------------------------------------------------
	 * If reverse requested, switch FG & BG colors
	 */
	if (xd->reverse)
		xvt_w_set_rev_video(tw, TRUE);

	/*--------------------------------------------------------------------
	 * setup extended TERM name & definitions
	 */
	strcpy(xd->term_type, XVT_TERM_COLOR);
	xd->term_defs = xvt_termdefs_c;
}

/*------------------------------------------------------------------------
 * setup initial colors
 */
void xvt_w_setup_color (TERMWIN *tw, XVT_DATA *xd)
{
	/*--------------------------------------------------------------------
	 * Check if in color or mono
	 */
	tw->can_be_color = (tw->X->depth > 1);

	/*--------------------------------------------------------------------
	 * allow user-override to mono
	 */
	if (xd->monodisp)
		tw->can_be_color = FALSE;

	/*--------------------------------------------------------------------
	 * now setup colors
	 */
	tw->X->cmap = DefaultColormap(tw->X->display, tw->X->screen);

	if (tw->can_be_color)
		xvt_w_cms_setup_color(tw, xd);
	else
		xvt_w_cms_setup_mono (tw, xd);
}

/*------------------------------------------------------------------------
 * free any color resources created
 */
void xvt_w_close_color (TERMWIN *tw)
{
	int		i;

	/*--------------------------------------------------------------------
	 * free the colors we allocated
	 */
	if (tw->can_be_color)
	{
		/*----------------------------------------------------------------
		 * Free any colors that we allocated.
		 */
		Pixel	pixels[XVT_CMS_SIZE];
		int		n;

		for (n=0, i=0; i<XVT_CMS_SIZE; i++)
		{
			CLR_TBL *	t = tw->X->clr_tbl + i;

			if (t->got)
				pixels[n++] = t->pix;
		}

		if (n > 0)
			XFreeColors(tw->X->display, tw->X->cmap, pixels, n, 0);
	}

	/*--------------------------------------------------------------------
	 * Now free all color names
	 */
	for (i=0; i<XVT_CMS_SIZE; i++)
	{
		CLR_TBL *	t = tw->X->clr_tbl + i;

		if (t->name != 0)
		{
			FREE(t->name);
			t->name = 0;
		}
	}
}

/*------------------------------------------------------------------------
 * set foreground color
 */
void xvt_w_set_fg	(TERMWIN *tw, int fg, int force)
{
	if (force || fg != tw->cur_fg)
	{
		tw->cur_fg = fg;
		XSetForeground(tw->X->display, tw->X->gc_vt,
			COLOR_PIX(tw, tw->cur_fg));
	}
}

/*------------------------------------------------------------------------
 * set background color
 */
void xvt_w_set_bg	(TERMWIN *tw, int bg, int force)
{
	if (force || bg != tw->cur_bg)
	{
		tw->cur_bg = bg;
		XSetBackground(tw->X->display, tw->X->gc_vt,
			COLOR_PIX(tw, tw->cur_bg));
	}
}

/*------------------------------------------------------------------------
 * get text cursor color
 */
int xvt_w_get_cursor_color (TERMWIN *tw)
{
	if (COLOR_PIX(tw, XVT_CLR_CR) == None)
		return (-1);

	return (XVT_CLR_CR);
}

/*------------------------------------------------------------------------
 * get current color name
 */
const char * xvt_w_get_color_name (TERMWIN *tw, int n)
{
	if (n < 0 || n >= XVT_CMS_SIZE)
		return (0);

	return (tw->X->clr_tbl[n].name);
}
