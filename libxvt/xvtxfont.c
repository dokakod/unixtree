/*------------------------------------------------------------------------
 * X font routines
 */
#include "xvtcommon.h"
#include "xvtx.h"

/*------------------------------------------------------------------------
 * check if two fonts are the same size
 */
static int xvt_w_font_same (XFontStruct *f1, XFontStruct *f2)
{
	return (f1->max_bounds.width   == f2->max_bounds.width  &&
			f1->max_bounds.ascent  == f2->max_bounds.ascent &&
			f1->max_bounds.descent == f2->max_bounds.descent);
}

/*------------------------------------------------------------------------
 * load a font
 */
static int xvt_w_font_load (TERMWIN *tw, FONT_INFO *fi,
	const char *rname, const char *bname)
{
	/*--------------------------------------------------------------------
	 * check font names
	 */
	if (rname == 0 || *rname == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * cache names in font struct
	 */
	fi->reg_fname = STRDUP(rname);
	fi->bld_fname = STRDUP(bname);

	/*--------------------------------------------------------------------
	 * try to load the reg font
	 */
	fi->reg_font = XLoadQueryFont(tw->X->display, rname);
	if (fi->reg_font == 0)
	{
		if (fi->reg_fname != 0)
			FREE(fi->reg_fname);
		if (fi->bld_fname != 0)
			FREE(fi->bld_fname);

		fi->reg_fname = 0;
		fi->bld_fname = 0;
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * cache reg font info
	 */
	fi->cell_xhome	= 0;
	fi->cell_yhome	= fi->reg_font->max_bounds.ascent;

	fi->cell_width	= fi->reg_font->max_bounds.width;

	fi->cell_height	= fi->reg_font->max_bounds.ascent +
					  fi->reg_font->max_bounds.descent;

	/*--------------------------------------------------------------------
	 * try to load the bold font
	 */
	if (bname != 0 && *bname != 0)
		fi->bld_font = XLoadQueryFont(tw->X->display, bname);
	else
		fi->bld_font = 0;

	/*--------------------------------------------------------------------
	 * if bold-font load succeeded, check if same size as reg
	 */
	if (fi->bld_font != 0)
	{
		if (! xvt_w_font_same(fi->reg_font, fi->bld_font))
		{
			XUnloadFont(tw->X->display, fi->bld_font->fid);
			FREE(fi->bld_fname);
			fi->bld_font	= 0;
			fi->bld_fname	= 0;
		}
	}

	return (0);
}

/*------------------------------------------------------------------------
 * load all fonts to be used
 */
int xvt_w_setup_fonts (TERMWIN *tw, XVT_DATA *xd)
{
	FONT_INFO *		fi	= tw->X->fonts;
	FONT_INFO *		fd;
	int				i;

	/*--------------------------------------------------------------------
	 * first try loading any user fonts
	 */
	for (i=0; i<XVT_NUM_FONTS; i++)
		xvt_w_font_load(tw, fi + i, xd->reg_fname[i], xd->bld_fname[i]);

	/*--------------------------------------------------------------------
	 * now check if default font got loaded OK
	 */
	fd = fi + xd->fontno;
	if (fd->reg_font == 0)
	{
		if (fd->reg_fname == 0)
		{
			sprintf(xd->error_msg, "No font specified for font %d",
				xd->fontno);
		}
		else
		{
			sprintf(xd->error_msg, "Cannot load font %d (%s)",
				xd->fontno, fd->reg_fname);
		}
		xd->error_num = XVT_ERR_CANTLOADFONTS;
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * set initial font number
	 */
	tw->cur_font_no = xd->fontno;

	return (XVT_OK);
}

/*------------------------------------------------------------------------
 * free any font resources which were loaded
 */
void xvt_w_close_fonts (TERMWIN *tw)
{
	int i;

	for (i=0; i<XVT_NUM_FONTS; i++)
	{
		FONT_INFO *	fi = tw->X->fonts + i;

		if (fi->reg_font != 0)
		{
			XUnloadFont(tw->X->display, fi->reg_font->fid);
			fi->reg_font = 0;
		}

		if (fi->bld_font != 0)
		{
			XUnloadFont(tw->X->display, fi->bld_font->fid);
			fi->bld_font = 0;
		}

		if (fi->reg_fname != 0)
			FREE(fi->reg_fname);
		if (fi->bld_fname != 0)
			FREE(fi->bld_fname);
	}
}

/*------------------------------------------------------------------------
 * change font number
 *
 * direction: <0 down <direction> times, >0 UP <direction> times
 */
void xvt_w_font_chg_no (TERMWIN *tw, int direction)
{
	FONT_INFO *	fi;
	int			i;

	if (direction == 0)
	{
		return;
	}
	else if (direction < 0)
	{
		direction = -direction;
		i = tw->cur_font_no;
		while (direction > 0 && i >= 0)
		{
			for (i--; i>=0; i--)
			{
				fi = tw->X->fonts + i;
				if (fi->reg_font != 0)
					break;
			}
			direction--;
		}
	}
	else /* (direction > 0) */
	{
		i = tw->cur_font_no;
		while (direction > 0 && i < XVT_NUM_FONTS)
		{
			for (i++; i<XVT_NUM_FONTS; i++)
			{
				fi = tw->X->fonts + i;
				if (fi->reg_font != 0)
					break;
			}
			direction--;
		}
	}

	if (i >= 0 && i < XVT_NUM_FONTS)
		xvt_w_font_set_no(tw, i);
}

/*------------------------------------------------------------------------
 * setup current font number
 */
void xvt_w_font_set_no (TERMWIN *tw, int font_no)
{
	if (font_no != tw->cur_font_no)
	{
		if (font_no >= 0 && font_no < XVT_NUM_FONTS)
		{
			FONT_INFO *	fi = tw->X->fonts + font_no;

			if (fi->reg_font != 0)
			{
				int font_type = tw->cur_font_type;

				tw->cur_font_no = font_no;
				tw->cur_font_type = FONT_NONE;
				xvt_w_font_set_type(tw, font_type);
				xvt_w_resize_win(tw, -1, -1);
			}
		}
	}
}

/*------------------------------------------------------------------------
 * setup font number to name
 */
void xvt_w_font_set_name (TERMWIN *tw, int font_no, const char *font_name)
{
	FONT_INFO *		fi;
	XFontStruct *	fp;

	/*--------------------------------------------------------------------
	 * validate args
	 */
	if (font_name == 0 || *font_name == 0)
		return;

	if (font_no < 0 || font_no >= XVT_NUM_FONTS)
		return;
	fi = tw->X->fonts + font_no;

	/*--------------------------------------------------------------------
	 * load the font
	 */
	fp = XLoadQueryFont(tw->X->display, font_name);
	if (fp == 0)
		return;

	/*--------------------------------------------------------------------
	 * unload any current font
	 */
	if (fi->reg_font != 0)
	{
		XUnloadFont(tw->X->display, fi->reg_font->fid);
		fi->reg_font = 0;
	}

	if (fi->bld_font != 0)
	{
		XUnloadFont(tw->X->display, fi->bld_font->fid);
		fi->bld_font = 0;
	}

	if (fi->reg_fname != 0)
	{
		FREE(fi->reg_fname);
		fi->reg_fname = 0;
	}

	if (fi->bld_fname != 0)
	{
		FREE(fi->bld_fname);
		fi->bld_fname = 0;
	}

	/*--------------------------------------------------------------------
	 * update font pointer
	 */
	fi->reg_fname	= STRDUP(font_name);
	fi->reg_font	= fp;

	fi->cell_xhome	= 0;
	fi->cell_yhome	= fi->reg_font->max_bounds.ascent;

	fi->cell_width	= fi->reg_font->max_bounds.width;
	fi->cell_height = fi->reg_font->max_bounds.ascent +
					  fi->reg_font->max_bounds.descent;

	/*--------------------------------------------------------------------
	 * now check if this is the current font
	 */
	if (font_no == tw->cur_font_no)
	{
		int font_type = tw->cur_font_type;

		tw->cur_font_type = FONT_NONE;
		xvt_w_font_set_type(tw, font_type);
		xvt_w_resize_win(tw, -1, -1);
	}
}

/*------------------------------------------------------------------------
 * return font name
 */
const char * xvt_w_font_get_name (TERMWIN *tw, int font_no)
{
	FONT_INFO *	fi;

	if (font_no < 0 || font_no >= XVT_NUM_FONTS)
		return (0);
	fi = tw->X->fonts + font_no;

	return (fi->reg_fname);
}

/*------------------------------------------------------------------------
 * setup current font type
 */
void xvt_w_font_set_type (TERMWIN *tw, int font_type)
{
	if (font_type != tw->cur_font_type)
	{
		FONT_INFO *	fi = tw->X->fonts + tw->cur_font_no;
		Font		fid;

		switch (font_type)
		{
		case FONT_BOLD:
			if (fi->bld_font != 0)
			{
				fid = fi->bld_font->fid;
				break;
			}
			/*FALLTHROUGH*/

		case FONT_NORM:
			fid = fi->reg_font->fid;
			break;

		default:
			return;
		}

		XSetFont(tw->X->display, tw->X->gc_vt, fid);

		tw->cur_font_type = font_type;
	}
}
