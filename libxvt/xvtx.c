/*------------------------------------------------------------------------
 * xvt X routines
 */
#include "xvtcommon.h"
#include "xvtx.h"

#if USE_XPM
#  include "xpm.h"
#endif

/*------------------------------------------------------------------------
 * get connection number
 */
int xvt_w_get_xfd (TERMWIN *tw)
{
	if (tw == 0)
		return (-1);

	return (tw->X->xfd);
}

/*------------------------------------------------------------------------
 * get window ID
 */
unsigned int xvt_w_get_windowid	(TERMWIN *tw)
{
	if (tw == 0)
		return (0);

	return ((unsigned int)tw->X->win_parent);
}

/*------------------------------------------------------------------------
 * setup the display
 */
static TERMWIN * xvt_w_setup_display (XVT_DATA *xd)
{
	X_DATA *		xp;
	TERMWIN *		tw;
	const char *	display;

	/*--------------------------------------------------------------------
	 * Check if in a window
	 */
	display = xd->display;
	if (display == 0 || *display == 0)
	{
		sprintf(xd->error_msg, "Not in window environment");
		xd->error_num = XVT_ERR_NOTINWIN;
		return (0);
	}

	/*--------------------------------------------------------------------
	 * initialize x-info struct
	 */
	xp = (X_DATA *)MALLOC(sizeof(*xp));
	if (xp == 0)
	{
		sprintf(xd->error_msg, "No memory");
		xd->error_num = XVT_ERR_NOMEM;
		return (0);
	}

	memset(xp,  0, sizeof(*xp));

	/*--------------------------------------------------------------------
	 * initialize win-info struct
	 */
	tw = (TERMWIN *)MALLOC(sizeof(*tw));
	if (tw == 0)
	{
		FREE(xp);
		sprintf(xd->error_msg, "No memory");
		xd->error_num = XVT_ERR_NOMEM;
		return (0);
	}

	memset(tw,  0, sizeof(*tw));
	tw->X	= xp;

	/*--------------------------------------------------------------------
	 * set defaults in struct
	 */
	tw->reverse		= FALSE;
	tw->acticon		= xd->acticon;

	tw->border		= xd->border;
	tw->margin		= xd->margin;
	tw->extra		= (tw->border + tw->margin);

	tw->sb_enable	= xd->sb_enable;
	tw->sb_right	= xd->sb_right;
	tw->sb_shadow	= xd->sb_shadow;

	/*--------------------------------------------------------------------
	 * open the display
	 */
	tw->X->display = XOpenDisplay(display);
	if (tw->X->display == None)
	{
		xvt_w_free(tw);

		sprintf(xd->error_msg, "Cannot open display %s", display);
		xd->error_num = XVT_ERR_CANTOPENDISP;
		return (0);
	}

	tw->X->win_root	= DefaultRootWindow(tw->X->display);
	tw->X->screen	= DefaultScreen(tw->X->display);
	tw->X->visual	= DefaultVisual(tw->X->display, tw->X->screen);
	tw->X->depth	= DisplayPlanes(tw->X->display, tw->X->screen);
	tw->X->xfd		= XConnectionNumber(tw->X->display);

	/*--------------------------------------------------------------------
	 * install error routine
	 */
	if (xd->xerrors)
		XSetErrorHandler(xvt_w_error_rtn);
	else
		XSetErrorHandler(xvt_w_error_rtn_dummy);

	/*--------------------------------------------------------------------
	 * get WM delete atom
	 */
	tw->X->wm_delete = XInternAtom(tw->X->display, "WM_DELETE_WINDOW", False);

	/*--------------------------------------------------------------------
	 * get clipboard text atom
	 */
	tw->X->cb_text = XInternAtom(tw->X->display, "TEXT", False);

	return (tw);
}

/*------------------------------------------------------------------------
 * close the display
 */
static void xvt_w_close_display (TERMWIN *tw)
{
	if (tw != 0)
	{
		if (tw->X->display != 0)
		{
			XCloseDisplay(tw->X->display);
			tw->X->display = 0;
		}

		xvt_w_free(tw);
	}
}

/*------------------------------------------------------------------------
 * close the cursor
 */
static void xvt_w_close_cursor (TERMWIN *tw)
{
	if (tw->X->cursor != 0)
	{
		XFreeCursor(tw->X->display, tw->X->cursor);
		tw->X->cursor = 0;
	}
}

/*------------------------------------------------------------------------
 * adjust the size struct
 */
static void xvt_w_setup_size (TERMWIN *tw)
{
	/*--------------------------------------------------------------------
	 * calculate scrollbar size
	 */
	if (tw->sb_enable)
	{
		tw->sb_size	= XVT_SB_WIDTH;
		if (tw->sb_shadow)
			tw->sb_size += (2 * XVT_SB_SHADOW);
	}
	else
	{
		tw->sb_size	= 0;
	}

	/*--------------------------------------------------------------------
	 * parent window stuff
	 */
	tw->X->s_hints.min_width		= COL_TO_W(tw, XVT_MIN_COLS) +
										(2 * tw->extra) + tw->sb_size;
	tw->X->s_hints.min_height		= ROW_TO_H(tw, XVT_MIN_ROWS) +
										(2 * tw->extra);

	tw->X->s_hints.width_inc		= COL_TO_W(tw, 1);
	tw->X->s_hints.height_inc		= ROW_TO_H(tw, 1);

	tw->X->s_hints.width			= COL_TO_W(tw, tw->win_cols) +
										(2 * tw->extra) + tw->sb_size;
	tw->X->s_hints.height			= ROW_TO_H(tw, tw->win_rows) +
										(2 * tw->extra);

	/*--------------------------------------------------------------------
	 * vt window stuff
	 */
	if (tw->sb_enable)
	{
		tw->X->vt_x = (tw->sb_right ? 0 : tw->sb_size);
		tw->X->vt_y = 0;
	}
	else
	{
		tw->X->vt_x = 0;
		tw->X->vt_y = 0;
	}
	tw->X->vt_width		= tw->X->s_hints.width - tw->sb_size;
	tw->X->vt_height	= tw->X->s_hints.height;

	/*--------------------------------------------------------------------
	 * sb window stuff
	 */
	if (tw->sb_enable)
	{
		tw->X->sb_x			= (tw->sb_right ? tw->X->vt_width : 0);
		tw->X->sb_y			= 0;
		tw->X->sb_width		= tw->sb_size;
		tw->X->sb_height	= tw->X->s_hints.height;
	}
	else
	{
		tw->X->sb_x			= 0;
		tw->X->sb_y			= 0;
		tw->X->sb_width		= 0;
		tw->X->sb_height	= 0;
	}
}

/*------------------------------------------------------------------------
 * setup the windows
 */
static int xvt_w_setup_windows (TERMWIN *tw, XVT_DATA *xd)
{
	unsigned int	mask;
	unsigned int	flags	= PMinSize | PResizeInc | PWinGravity;
	int				x_pos	= 0;
	int				y_pos	= 0;
	unsigned int	gravity	= NorthWestGravity;

	/*--------------------------------------------------------------------
	 * check # rows & cols
	 */
	if (xd->cur_cols < XVT_MIN_COLS)
		xd->cur_cols = XVT_MIN_COLS;

	if (xd->cur_rows < XVT_MIN_ROWS)
		xd->cur_rows = XVT_MIN_ROWS;

	/*--------------------------------------------------------------------
	 * check if geometry specified
	 */
	if (xd->geometry != 0 && *xd->geometry != 0)
	{
		int				x;
		int				y;
		unsigned int	width;
		unsigned int	height;
		int				rc;

		rc = XParseGeometry(xd->geometry, &x, &y, &width, &height);

		/*----------------------------------------------------------------
		 * check if width specified
		 */
		if (rc & WidthValue)
		{
			flags |= USSize;
			xd->cur_cols = width;

			if (xd->cur_cols < XVT_MIN_COLS)
				xd->cur_cols = XVT_MIN_COLS;
		}

		/*----------------------------------------------------------------
		 * check if height specified
		 */
		if (rc & HeightValue)
		{
			flags |= USSize;
			xd->cur_rows = height;

			if (xd->cur_rows < XVT_MIN_ROWS)
				xd->cur_rows = XVT_MIN_ROWS;
		}

		/*----------------------------------------------------------------
		 * check if x position specified
		 */
		if (rc & XValue)
		{
			flags |= USPosition;
			x_pos  = x;

			if (rc & XNegative)
			{
				gravity = NorthEastGravity;

				x_pos += DisplayWidth(tw->X->display, tw->X->screen)
					- (COL_TO_W(tw, xd->cur_cols) + (2 * tw->extra));
			}
		}

		/*----------------------------------------------------------------
		 * check if y position specified
		 */
		if (rc & YValue)
		{
			flags |= USPosition;
			y_pos  = y;

			if (rc & YNegative)
			{
				gravity = (gravity == NorthEastGravity ?
					SouthEastGravity : SouthWestGravity);

				y_pos += DisplayHeight(tw->X->display, tw->X->screen)
					- (ROW_TO_H(tw, xd->cur_rows) + (2 * tw->extra));
			}
		}
	}

	tw->win_cols	= xd->cur_cols;
	tw->win_rows	= xd->cur_rows;

	/*--------------------------------------------------------------------
	 * setup size hints
	 */
	tw->X->s_hints.flags			= flags;
	tw->X->s_hints.x				= x_pos;
	tw->X->s_hints.y				= y_pos;
	tw->X->s_hints.win_gravity		= gravity;

	xvt_w_setup_size(tw);

	/*--------------------------------------------------------------------
	 * setup window hints
	 */
	tw->X->w_hints.flags			= InputHint | StateHint;
	tw->X->w_hints.input			= True;
	tw->X->w_hints.initial_state	= xd->iconic ? IconicState : NormalState;

	tw->iconified					= xd->iconic;

	/*--------------------------------------------------------------------
	 * load icon file if specified & create icon window if not
	 */
	if (xd->ico_file != 0 && *xd->ico_file != 0)
	{
		char	icon_path[XVT_MAX_PATHLEN];

		xvt_path_resolve(icon_path, xd->ico_file);
		if (*icon_path != 0)
		{
			const char *	ext = xvt_path_extension(xd->ico_file);

			if (ext == 0)
				ext = "";

			if (strcmp(ext, "xbm") == 0)
			{
				unsigned int	w, h;
				int				hx, hy;
				int				rc;

				rc = XReadBitmapFile(tw->X->display, tw->X->win_root,
					icon_path, &w, &h, &tw->X->icon, &hx, &hy);
				if (rc == BitmapSuccess)
				{
					tw->X->w_hints.flags		|= IconPixmapHint;
					tw->X->w_hints.icon_pixmap	 = tw->X->icon;
					tw->X->ico_width			 = (int)w;
					tw->X->ico_height			 = (int)h;
				}
			}
#if USE_XPM
			else if (strcmp(ext, "xpm") == 0)
			{
				XpmAttributes	attrs;
				int				rc;

				attrs.valuemask = 0;
				rc = XpmReadFileToPixmap(tw->X->display, tw->X->win_root,
					(char *)xd->ico_file, &tw->X->icon, 0, &attrs);
				if (rc == XpmSuccess)
				{
					tw->X->w_hints.flags		|= IconPixmapHint;
					tw->X->w_hints.icon_pixmap	 = tw->X->icon;
					tw->X->ico_width			 = (int)attrs.width;
					tw->X->ico_height			 = (int)attrs.height;
				}
			}
#endif
		}
	}
	else if (tw->acticon)
	{
		int pad;

		tw->X->ico_width	= tw->win_cols / 2;
		tw->X->ico_height	= tw->win_rows;

		tw->X->win_icon = XCreateSimpleWindow(tw->X->display, tw->X->win_root,
			0, 0, tw->X->ico_width, tw->X->ico_height, 0,
			COLOR_PIX(tw, XVT_CLR_FG), None);
		if (tw->X->win_icon != None)
		{
			XSelectInput(tw->X->display, tw->X->win_icon, ExposureMask);

			tw->X->w_hints.flags		|= IconWindowHint;
			tw->X->w_hints.icon_window	 = tw->X->win_icon;

			if (tw->X->depth > 16)	pad = 32;
			else
			if (tw->X->depth >  8)	pad = 16;
			else					pad =  8;

			tw->X->image = XCreateImage(tw->X->display, tw->X->visual,
				tw->X->depth, ZPixmap, 0, 0,
				tw->X->ico_width, tw->X->ico_height,
				pad, 0);
			if (tw->X->image != 0)
			{
				tw->X->image->data = (char *)MALLOC(tw->X->ico_height *
					tw->X->image->bytes_per_line);
			}
		}
		else
		{
			tw->acticon = FALSE;
		}
	}

	/*--------------------------------------------------------------------
	 * check if any icon-geometry was specified
	 *
	 * Note that we assume an icon size of XVT_MAX_BITS by XVT_MAX_BITS
	 * if unknown.
	 */
	if (xd->icongeom != 0 && *xd->icongeom != 0)
	{
		int				x;
		int				y;
		unsigned int	width;
		unsigned int	height;
		int				rc;

		rc = XParseGeometry(xd->icongeom, &x, &y, &width, &height);

		/*----------------------------------------------------------------
		 * check if x position specified
		 */
		if (rc & XValue)
		{
			tw->X->w_hints.flags |= IconPositionHint;
			tw->X->w_hints.icon_x = x;

			if (rc & XNegative)
			{
				tw->X->w_hints.icon_x =
					DisplayWidth(tw->X->display, tw->X->screen) -
						(tw->X->ico_width ? tw->X->ico_width : XVT_MAX_BITS);
			}
		}

		/*----------------------------------------------------------------
		 * check if y position specified
		 */
		if (rc & YValue)
		{
			tw->X->w_hints.flags |= IconPositionHint;
			tw->X->w_hints.icon_y = y;

			if (rc & YNegative)
			{
				tw->X->w_hints.icon_y =
					DisplayHeight(tw->X->display, tw->X->screen) -
						(tw->X->ico_height ? tw->X->ico_height : XVT_MAX_BITS);
			}
		}
	}

	/*--------------------------------------------------------------------
	 * setup class hints
	 */
	{
		const char *	resname;

		if (xd->resname != 0 && *xd->resname != 0)
			resname = xd->resname;
		else if (xd->appname != 0 && *xd->appname != 0)
			resname = xd->appname;
		else
			resname = XVT_NAME;

		tw->X->c_hints.res_name		= (char *)resname;
		tw->X->c_hints.res_class	= (char *)XVT_APPCLASS;
	}

	/*--------------------------------------------------------------------
	 * create parent window
	 */
	tw->X->win_parent = XCreateSimpleWindow(tw->X->display, tw->X->win_root,
		tw->X->s_hints.x, tw->X->s_hints.y,
		tw->X->s_hints.width, tw->X->s_hints.height, 0,
		COLOR_PIX(tw, XVT_CLR_BD), COLOR_PIX(tw, XVT_CLR_BG));

	if (tw->X->win_parent == None)
	{
		sprintf(xd->error_msg, "Cannot create parent window");
		xd->error_num = XVT_ERR_CANTMAKEWIN;
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * set window properties
	 */
	XSetWMProperties(tw->X->display, tw->X->win_parent,
		0, 0, 0, 0,
		&tw->X->s_hints, &tw->X->w_hints, &tw->X->c_hints);

	XSetWMProtocols(tw->X->display, tw->X->win_parent, &tw->X->wm_delete, 1);

	/*--------------------------------------------------------------------
	 * create cursor to use
	 */
	{
		int	shape = -1;

		if (xd->pointer != 0)
			shape = xvt_w_ptr_value(xd->pointer);

		if (shape != -1)
			xvt_w_set_pointer(tw, shape);
	}

	/*--------------------------------------------------------------------
	 * now create vt window
	 */
	tw->X->win_vt = XCreateSimpleWindow(tw->X->display, tw->X->win_parent,
		tw->X->vt_x, tw->X->vt_y, 
		tw->X->vt_width, tw->X->vt_height, 0,
		COLOR_PIX(tw, XVT_CLR_BD), COLOR_PIX(tw, XVT_CLR_BG));

	if (tw->X->win_vt == None)
	{
		sprintf(xd->error_msg, "Cannot create vt window");
		xd->error_num = XVT_ERR_CANTMAKEWIN;
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * now create sb window if requested
	 */
	if (tw->sb_enable)
	{
		tw->X->win_sb = XCreateSimpleWindow(tw->X->display, tw->X->win_parent,
			tw->X->sb_x, tw->X->sb_y, 
			tw->X->sb_width, tw->X->sb_height, 0,
			COLOR_PIX(tw, XVT_CLR_SB), COLOR_PIX(tw, XVT_CLR_ST));

		if (tw->X->win_sb == None)
		{
			sprintf(xd->error_msg, "Cannot create scrollbar window");
			xd->error_num = XVT_ERR_CANTMAKEWIN;
			return (-1);
		}
	}

	/*--------------------------------------------------------------------
	 * set selection masks
	 */
	mask = 0
			| FocusChangeMask
			| StructureNotifyMask
			;

	XSelectInput(tw->X->display, tw->X->win_parent, mask);

	mask = 0
			| KeyPressMask
			| KeyReleaseMask
			| KeymapStateMask
			| ExposureMask
			| FocusChangeMask
			;

	if (! xd->ignptr)
	{
		mask |= 0
				| ButtonPressMask
				| ButtonReleaseMask
				| PointerMotionMask
				;
	}

	XSelectInput(tw->X->display, tw->X->win_vt, mask);

	if (tw->sb_enable)
	{
		mask = 0
				| ButtonPressMask
				| ButtonReleaseMask
				| PointerMotionMask
				;

		XSelectInput(tw->X->display, tw->X->win_sb, mask);
	}

	/*--------------------------------------------------------------------
	 * set window to be displayed
	 */
	if (tw->sb_enable)
		XMapWindow(tw->X->display, tw->X->win_sb);
	XMapWindow(tw->X->display, tw->X->win_vt);
	XMapWindow(tw->X->display, tw->X->win_parent);

	return (0);
}

/*------------------------------------------------------------------------
 * resize the windows
 */
void xvt_w_resize_win (TERMWIN *tw, int x, int y)
{
	xvt_w_setup_size(tw);

	XSetWMNormalHints(tw->X->display, tw->X->win_parent,
		&tw->X->s_hints);

	if (x >= 0 && y >= 0)
	{
		XMoveResizeWindow(tw->X->display, tw->X->win_parent,
			x, y,
			tw->X->s_hints.width, tw->X->s_hints.height);
	}
	else
	{
		XResizeWindow(tw->X->display, tw->X->win_parent,
			tw->X->s_hints.width, tw->X->s_hints.height);
	}

	XMoveResizeWindow(tw->X->display, tw->X->win_vt,
		tw->X->vt_x, tw->X->vt_y,
		tw->X->vt_width, tw->X->vt_height);

	if (tw->X->win_icon != None)
	{
		if (tw->win_cols != tw->X->ico_width * 2 ||
		    tw->win_rows != tw->X->ico_height)
		{
			int pad = tw->X->image->bitmap_pad;

			tw->X->ico_width	= tw->win_cols / 2;
			tw->X->ico_height	= tw->win_rows;

			XResizeWindow(tw->X->display, tw->X->win_icon,
				tw->X->ico_width, tw->X->ico_height);

			if (tw->X->image->data != 0)
			{
				FREE(tw->X->image->data);
				tw->X->image->data = 0;
			}
			XDestroyImage(tw->X->image);

			tw->X->image = XCreateImage(tw->X->display, tw->X->visual,
				tw->X->depth, ZPixmap, 0, 0,
				tw->X->ico_width, tw->X->ico_height,
				pad, 0);
			if (tw->X->image != 0)
			{
				tw->X->image->data = (char *)MALLOC(tw->X->ico_height *
					tw->X->image->bytes_per_line);
			}
		}
	}

	if (tw->sb_enable)
	{
		XMoveResizeWindow(tw->X->display, tw->X->win_sb,
			tw->X->sb_x, tw->X->sb_y,
			tw->X->sb_width, tw->X->sb_height);
	}

	XSync(tw->X->display, FALSE);
}

/*------------------------------------------------------------------------
 * user call to resize the windows
 */
void xvt_w_resize (TERMWIN *tw, int cols, int rows)
{
	if (cols >= XVT_MIN_COLS)
		tw->win_cols = cols;

	if (rows >= XVT_MIN_ROWS)
		tw->win_rows = rows;

	xvt_w_resize_win(tw, -1, -1);
}

/*------------------------------------------------------------------------
 * raise the window
 */
void xvt_w_raise (TERMWIN *tw)
{
	XRaiseWindow(tw->X->display, tw->X->win_parent);
}

/*------------------------------------------------------------------------
 * lower the window
 */
void xvt_w_lower (TERMWIN *tw)
{
	XLowerWindow(tw->X->display, tw->X->win_parent);
}

/*------------------------------------------------------------------------
 * get window size
 */
void xvt_w_get_size (TERMWIN *tw, int *w, int *h)
{
	XWindowAttributes	attrs;

	XGetWindowAttributes(tw->X->display, tw->X->win_parent, &attrs);
	*w = attrs.width;
	*h = attrs.height;
}

/*------------------------------------------------------------------------
 * set window size
 */
void xvt_w_set_size (TERMWIN *tw, int w, int h)
{
	int cols = W_TO_COL(tw, w);
	int rows = H_TO_ROW(tw, h);

	xvt_w_resize(tw, cols, rows);
}

/*------------------------------------------------------------------------
 * set window size to max
 */
void xvt_w_set_max_size (TERMWIN *tw)
{
	if (tw->old_cols == 0 && tw->old_rows == 0)
	{
		int cols;
		int rows;

		cols = DisplayWidth (tw->X->display, tw->X->screen);
		rows = DisplayHeight(tw->X->display, tw->X->screen);

		cols -= (2 * tw->extra) + tw->sb_size;
		rows -= (2 * tw->extra);

		cols = W_TO_COL(tw, cols) - 1;
		rows = H_TO_ROW(tw, rows) - 1;

		tw->old_cols = tw->win_cols;
		tw->old_rows = tw->win_rows;
		xvt_w_get_position(tw, &tw->old_x, &tw->old_y);
		tw->win_cols = cols;
		tw->win_rows = rows;
		xvt_w_resize_win(tw, 0, 0);
	}
}

/*------------------------------------------------------------------------
 * set window size to old value
 */
void xvt_w_set_old_size (TERMWIN *tw)
{
	if (tw->old_cols != 0 && tw->old_rows != 0)
	{
		int cols = tw->old_cols;
		int rows = tw->old_rows;

		tw->old_cols = 0;
		tw->old_rows = 0;
		tw->win_cols = cols;
		tw->win_rows = rows;
		xvt_w_resize_win(tw, tw->old_x, tw->old_y);
	}
}

/*------------------------------------------------------------------------
 * get window position
 */
void xvt_w_get_position (TERMWIN *tw, int *x, int *y)
{
	XWindowAttributes	attrs;
	Window				dummy;

	XGetWindowAttributes(tw->X->display, tw->X->win_parent, &attrs);
	XTranslateCoordinates(tw->X->display, tw->X->win_parent, tw->X->win_root,
		-attrs.border_width, -attrs.border_width, x, y, &dummy);
}

/*------------------------------------------------------------------------
 * set window position
 */
void xvt_w_set_position (TERMWIN *tw, int x, int y)
{
	XMoveWindow(tw->X->display, tw->X->win_parent, x, y);
}

/*------------------------------------------------------------------------
 * close the windows
 */
static void xvt_w_close_windows (TERMWIN *tw)
{
	if (tw->X->icon != None)
	{
		XFreePixmap(tw->X->display, tw->X->icon);
		tw->X->icon = None;
	}

	if (tw->X->image != 0)
	{
		if (tw->X->image->data != 0)
		{
			FREE(tw->X->image->data);
			tw->X->image->data = 0;
		}

		XDestroyImage(tw->X->image);
		tw->X->image = 0;
	}
}

/*------------------------------------------------------------------------
 * setup the GC
 */
static void xvt_w_setup_gc (TERMWIN *tw)
{
	XGCValues	gc_val;
	int			valuemask;
	char		fill_stipple_bits[1];

	/*--------------------------------------------------------------------
	 * create the stipple to use
	 */
	fill_stipple_bits[0] = 0;
	tw->X->fill_stipple = XCreateBitmapFromData(tw->X->display, tw->X->win_vt,
		fill_stipple_bits, 8, 1);

	/*--------------------------------------------------------------------
	 * create the vt GC
	 */
	memset(&gc_val, 0, sizeof(gc_val));

	gc_val.function				= GXcopy;
	gc_val.fill_style			= FillOpaqueStippled;
	gc_val.stipple				= tw->X->fill_stipple;
	gc_val.graphics_exposures	= FALSE;
	gc_val.foreground			= COLOR_PIX(tw, XVT_CLR_FG);
	gc_val.background			= COLOR_PIX(tw, XVT_CLR_BG);

	valuemask					= 0
									| GCFunction
									| GCFillStyle
									| GCStipple
									| GCForeground
									| GCBackground
									| GCGraphicsExposures
									;

	tw->X->gc_vt = XCreateGC(tw->X->display, tw->X->win_vt, valuemask,
		&gc_val);

	/*--------------------------------------------------------------------
	 * create the sb GCs
	 */
	if (tw->sb_enable)
	{
		memset(&gc_val, 0, sizeof(gc_val));

		gc_val.foreground = COLOR_PIX(tw, XVT_CLR_SB);
		tw->X->gc_sb    = XCreateGC(tw->X->display, tw->X->win_sb,
			GCForeground, &gc_val);

		gc_val.foreground = COLOR_PIX(tw, XVT_CLR_SB_TS);
		tw->X->gc_sb_ts = XCreateGC(tw->X->display, tw->X->win_sb,
			GCForeground, &gc_val);

		gc_val.foreground = COLOR_PIX(tw, XVT_CLR_SB_BS);
		tw->X->gc_sb_bs = XCreateGC(tw->X->display, tw->X->win_sb,
			GCForeground, &gc_val);

		if (tw->sb_shadow)
		{
			XSetWindowBackground(tw->X->display, tw->X->win_sb,
				COLOR_PIX(tw, XVT_CLR_ST));
			XClearWindow(tw->X->display, tw->X->win_sb);
		}
	}
}

/*------------------------------------------------------------------------
 * free the GC
 */
static void xvt_w_close_gc (TERMWIN *tw)
{
	if (tw->X->gc_vt != None)
	{
		XFreeGC(tw->X->display, tw->X->gc_vt);
		tw->X->gc_vt = None;
	}

	if (tw->X->gc_sb != None)
	{
		XFreeGC(tw->X->display, tw->X->gc_sb);
		tw->X->gc_sb = None;
	}

	if (tw->X->gc_sb_ts != None)
	{
		XFreeGC(tw->X->display, tw->X->gc_sb_ts);
		tw->X->gc_sb_ts = None;
	}

	if (tw->X->gc_sb_bs != None)
	{
		XFreeGC(tw->X->display, tw->X->gc_sb_bs);
		tw->X->gc_sb_bs = None;
	}

	if (tw->X->fill_stipple != None)
	{
		XFreePixmap(tw->X->display, tw->X->fill_stipple);
		tw->X->fill_stipple = None;
	}
}

/*------------------------------------------------------------------------
 * check screen mode requested
 */
int xvt_w_chk_scrn_mode (TERMWIN *tw, int mode)
{
	switch (mode)
	{
	case SCRN_MODE_MONO:
		break;

	case SCRN_MODE_COLOR:
		if (! tw->can_be_color)
			mode = SCRN_MODE_MONO;
		break;

	default:
		mode = SCRN_MODE_NONE;
		break;
	}

	return (mode);
}

/*------------------------------------------------------------------------
 * specify X pointer
 */
void xvt_w_set_pointer (TERMWIN *tw, int shape)
{
	if (shape >= 0 && shape < XC_num_glyphs)
	{
		Cursor cursor;

		shape &= ~1;
		cursor = XCreateFontCursor(tw->X->display, shape);
		if (cursor != 0)
		{
			if (tw->can_be_color)
			{
				XColor	cursor_fg;
				XColor	cursor_bg;

				cursor_fg.pixel = COLOR_PIX(tw, XVT_CLR_PR);
				cursor_bg.pixel = COLOR_PIX(tw, XVT_CLR_BG);

				XQueryColor(tw->X->display, tw->X->cmap, &cursor_fg);
				XQueryColor(tw->X->display, tw->X->cmap, &cursor_bg);

				XRecolorCursor(tw->X->display, cursor, &cursor_fg, &cursor_bg);
			}

			xvt_w_close_cursor(tw);
			tw->X->cursor = cursor;
			XDefineCursor(tw->X->display, tw->X->win_parent, tw->X->cursor);
		}
	}
}

/*------------------------------------------------------------------------
 * setup everything
 */
TERMWIN * xvt_w_setup_all (XVT_DATA *xd)
{
	TERMWIN *	tw;
	int			rc;

	/*--------------------------------------------------------------------
	 * open the display
	 */
	tw = xvt_w_setup_display(xd);
	if (tw == 0)
	{
		return (0);
	}

	/*--------------------------------------------------------------------
	 * setup fonts to use
	 */
	rc = xvt_w_setup_fonts(tw, xd);
	if (rc)
	{
		xvt_w_close_all(tw);
		return (0);
	}

	/*--------------------------------------------------------------------
	 * setup colors
	 */
	xvt_w_setup_color(tw, xd);

	/*--------------------------------------------------------------------
	 * create the window
	 */
	rc = xvt_w_setup_windows(tw, xd);
	if (rc)
	{
		xvt_w_close_all(tw);
		return (0);
	}

	/*--------------------------------------------------------------------
	 * create Graphics Context
	 */
	xvt_w_setup_gc(tw);

	return (tw);
}

/*------------------------------------------------------------------------
 * free our TERMWIN struct
 */
void xvt_w_free (TERMWIN *tw)
{
	if (tw != 0)
	{
		FREE(tw->X);
		if (tw->selection_ptr != 0)
			FREE(tw->selection_ptr);
		FREE(tw);
	}
}

/*------------------------------------------------------------------------
 * close out everything
 */
void xvt_w_close_all (TERMWIN *tw)
{
	if (tw != 0)
	{
		xvt_w_close_cursor(tw);
		xvt_w_close_windows(tw);
		xvt_w_close_gc(tw);
		xvt_w_close_color(tw);
		xvt_w_close_fonts(tw);
		xvt_w_close_display(tw);
	}
}

/*------------------------------------------------------------------------
 * close out connection
 */
void xvt_w_close_con (TERMWIN *tw)
{
	if (tw != 0)
	{
		close(tw->X->xfd);
	}
}

/*------------------------------------------------------------------------
 * ring the bell
 */
void xvt_w_bell (TERMWIN *tw)
{
	XBell(tw->X->display, 0);
}

/*------------------------------------------------------------------------
 * iconify the window
 */
void xvt_w_set_icon_state (TERMWIN *tw, int state)
{
	if (state)
	{
		XIconifyWindow(tw->X->display, tw->X->win_parent, tw->X->screen);
	}
	else
	{
		XMapWindow(tw->X->display, tw->X->win_parent);
	}

	tw->iconified = state;
}

/*------------------------------------------------------------------------
 * set window title
 */
void xvt_w_set_win_title (TERMWIN *tw, const char *str)
{
	char *	title;

	if (XFetchName(tw->X->display, tw->X->win_parent, &title))
		title = 0;

	if (title == 0 || strcmp(title, str) != 0)
		XStoreName(tw->X->display, tw->X->win_parent, str);

	if (title != 0)
		XFree(title);
}

/*------------------------------------------------------------------------
 * set icon title
 */
void xvt_w_set_ico_title (TERMWIN *tw, const char *str)
{
	char *	title;

	if (XGetIconName(tw->X->display, tw->X->win_parent, &title))
		title = 0;

	if (title == 0 || strcmp(title, str) != 0)
		XSetIconName(tw->X->display, tw->X->win_parent, str);

	if (title != 0)
		XFree(title);
}

/*------------------------------------------------------------------------
 * set the icon for a window
 */
void xvt_w_set_icon (TERMWIN *tw, int width, int height, unsigned char *bits)
{
	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (width <= 0 || height <= 0 || bits == 0)
		return;

	/*--------------------------------------------------------------------
	 * Apparently, once you set an IconWindow, you cannot change over
	 * to an IconPixmap.  So, if active-icon is set, we just bail.
	 */
	if (tw->acticon)
		return;

	/*--------------------------------------------------------------------
	 * free any existing icon pixmap
	 */
	if (tw->X->icon != None)
	{
		XFreePixmap(tw->X->display, tw->X->icon);

		tw->X->w_hints.flags		&= ~IconPixmapHint;
		tw->X->w_hints.icon_pixmap	 = None;
	}

	/*--------------------------------------------------------------------
	 * free any existing icon window (THIS DOESN'T WORK)
	 */
	if (tw->acticon)
	{
		if (tw->X->win_icon != None)
		{
			XDestroyWindow(tw->X->display, tw->X->win_icon);
			tw->X->win_icon = None;
		}

		if (tw->X->image != 0)
		{
			if (tw->X->image->data != 0)
			{
				FREE(tw->X->image->data);
				tw->X->image->data = 0;
			}
			XDestroyImage(tw->X->image);
			tw->X->image = 0;
		}

		tw->X->w_hints.flags		&= ~IconWindowHint;
		tw->X->w_hints.icon_window	 = None;

		tw->acticon = FALSE;
	}

	/*--------------------------------------------------------------------
	 * create new icon pixmap
	 */
	tw->X->icon = XCreateBitmapFromData(tw->X->display, tw->X->win_root,
		(char *)bits, width, height);
	if (tw->X->icon != None)
	{
		tw->X->w_hints.flags		|= IconPixmapHint;
		tw->X->w_hints.icon_pixmap	 = tw->X->icon;
		tw->X->ico_width			 = width;
		tw->X->ico_height			 = height;
	}

	/*--------------------------------------------------------------------
	 * set icon into window properties
	 */
	XSetWMProperties(tw->X->display, tw->X->win_parent,
		0, 0, 0, 0, &tw->X->s_hints, &tw->X->w_hints, 0);
}

/*------------------------------------------------------------------------
 * set the icon window to be a copy of a screen
 */
void xvt_w_set_icon_display (TERMWIN *tw, TERMSCRN *s)
{
	/*--------------------------------------------------------------------
	 * only do this if we doing active icon
	 */
	if (tw->acticon)
	{
		int x;
		int y;

		for (y=0; y<SCRN_ROWS(s); y++)
		{
			CELL *	cp = SCRN_LINE(s, y);

			for (x=0; x<SCRN_COLS(s); x+=2)
			{
				Pixel	p;
				int		c;

				if (cp->ch == ' ')
				{
					CELL *	c2 = cp+1;

					if (c2->ch != ' ')
					{
						c = c2->fg;
						if (SCRN_MODE(s) == SCRN_MODE_COLOR && c2->at & A_BOLD)
							c |= XVT_CLR_BRIGHT;
					}
					else
					{
						c = cp->bg;
						if (SCRN_MODE(s) == SCRN_MODE_COLOR && cp->at & A_BLINK)
							c |= XVT_CLR_BRIGHT;
					}
				}
				else
				{
					c = cp->fg;
					if (SCRN_MODE(s) == SCRN_MODE_COLOR && cp->at & A_BOLD)
						c |= XVT_CLR_BRIGHT;
				}
				p = COLOR_PIX(tw, c);
				cp++;
				cp++;

				XPutPixel(tw->X->image, x/2, y, p);
			}
		}

		XPutImage(tw->X->display, tw->X->win_icon, tw->X->gc_vt, tw->X->image,
			0, 0, 0, 0, tw->X->ico_width, tw->X->ico_height);
	}
}
