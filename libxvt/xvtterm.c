/*------------------------------------------------------------------------
 * xterm clone logic routines
 */
#include "xvtcommon.h"

/*------------------------------------------------------------------------
 * key state masks
 */
static const int xvt_term_key_masks[] =
{
	KEY_MASK_SHFT,
	KEY_MASK_CTRL,
	KEY_MASK_META,
	KEY_MASK_NUML
};

/*------------------------------------------------------------------------
 * free screen arrays
 */
static void xvt_term_close_text (TERMDATA *td)
{
	td->reg_scrn = xvt_scrn_free(td->reg_scrn);
	td->alt_scrn = xvt_scrn_free(td->alt_scrn);
	td->cur_scrn = 0;
}

/*------------------------------------------------------------------------
 * create screen arrays
 */
static void xvt_term_setup_text (TERMDATA *td, int w, int h)
{
	TERMSCRN *	old_reg	= td->reg_scrn;
	TERMSCRN *	old_alt	= td->alt_scrn;
	TERMSCRN *	old_cur	= td->cur_scrn;
	TERMSCRN *	new_reg;
	TERMSCRN *	new_alt;
	int			mode;
	int			fg;
	int			bg;
	int			at;

	/*--------------------------------------------------------------------
	 * adjust current position if outside of new window
	 */
	if (old_cur != 0)
	{
		if (CUR_SCRN_X(td) >= w)
			CUR_SCRN_X(td) = w-1;

		if (CUR_SCRN_Y(td) >= h)
			CUR_SCRN_Y(td) = h-1;
	}

	/*--------------------------------------------------------------------
	 * allocate new reg screen array
	 */
	if (old_reg == 0)
	{
		mode	= td->scrn_mode;
		fg		= XVT_CLR_FG;
		bg		= XVT_CLR_BG;
		at		= A_NORMAL;
	}
	else
	{
		mode	= SCRN_MODE(old_reg);
		fg		= SCRN_FG(old_reg);
		bg		= SCRN_BG(old_reg);
		at		= SCRN_AT(old_reg);
	}
	new_reg	= xvt_scrn_init(h, w, td->xd->scr_tabs, mode, fg, bg, at);

	/*--------------------------------------------------------------------
	 * allocate new alt screen array
	 */
	if (old_alt == 0)
	{
		mode	= td->scrn_mode;
		fg		= XVT_CLR_FG;
		bg		= XVT_CLR_BG;
		at		= A_NORMAL;
	}
	else
	{
		mode	= SCRN_MODE(old_alt);
		fg		= SCRN_FG(old_alt);
		bg		= SCRN_BG(old_alt);
		at		= SCRN_AT(old_alt);
	}
	new_alt	= xvt_scrn_init(h, w, td->xd->scr_tabs, mode, fg, bg, at);

	/*--------------------------------------------------------------------
	 * now copy over screen contents
	 */
	xvt_scrn_copy(new_reg, old_reg);
	xvt_scrn_copy(new_alt, old_alt);

	/*--------------------------------------------------------------------
	 * free the old screens
	 */
	xvt_scrn_free(old_reg);
	xvt_scrn_free(old_alt);

	/*--------------------------------------------------------------------
	 * store new screens
	 */
	td->reg_scrn = new_reg;
	td->alt_scrn = new_alt;
	td->cur_scrn = (old_cur == old_reg ? new_reg : new_alt);

	/*--------------------------------------------------------------------
	 * finally, set new size in user data
	 */
	td->xd->cur_cols = w;
	td->xd->cur_rows = h;
}

/*------------------------------------------------------------------------
 * set reg font
 *
 * If we are in color & we have a bold font defined, use it.
 * Otherwise, use the reg font (which is always there).
 */
static void xvt_term_set_reg_font (TERMDATA *td)
{
	int font_type = (td->scrn_mode == SCRN_MODE_COLOR ? FONT_BOLD : FONT_NORM);

	xvt_w_font_set_type(td->tw, font_type);
}

/*------------------------------------------------------------------------
 * flush an output buffer
 *
 * returns: 0 if buffer completely flushed, -1 if not
 */
static int xvt_term_write_flsbuf (TERMDATA *td, PTY_IOBUF *p)
{
	int	max_len	= 128;
	int rc		= 0;

	/*--------------------------------------------------------------------
	 * Try to write out as much of the buffer as we can.
	 *
	 * Note that we write the buffer in small chunks, since a PTY can
	 * only hold so much data.  We use 1/2 the POSIX max limit for PTYs.
	 *
	 * TODO: use select() to see if a write is OK?
	 */
	while (p->bufpos < p->bufcnt)
	{
		int len	= (p->bufcnt - p->bufpos);
		int num;

		if (len > max_len)
			len = max_len;

		num = write(td->pd->mst_fd, p->bufptr + p->bufpos, len);
		if (num <= 0)
			break;

		p->bufpos += num;
	}

	/*--------------------------------------------------------------------
	 * if this buffer is done, reset its counters
	 */
	if (p->bufpos >= p->bufcnt)
	{
		p->bufpos = 0;
		p->bufcnt = 0;
	}
	else
	{
		rc = -1;
	}

	/*--------------------------------------------------------------------
	 * If this buffer is on the heap and is empty,
	 * remove it from the chain and free it up.
	 */
	if (p->bufheap && p->bufcnt == 0)
	{
		sysmutex_enter(&td->pty_outmtx);
		{
			if (p == td->pty_outbufs)
			{
				td->pty_outbufs = p->next;
			}
			else
			{
				PTY_IOBUF *	n;

				for (n=td->pty_outbufs; n->next; n=n->next)
				{
					if (p == n->next)
						break;
				}
				n->next = p->next;
			}
		}
		sysmutex_leave(&td->pty_outmtx);

		FREE(p);
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * flush all output buffers
 */
static void xvt_term_write_flush (TERMDATA *td)
{
	PTY_IOBUF *	p;
	PTY_IOBUF *	n;
	int			rc;

	/*--------------------------------------------------------------------
	 * first check main output buffer
	 */
	p = &td->out_data;
	rc = xvt_term_write_flsbuf(td, p);
	if (rc)
		return;

	/*--------------------------------------------------------------------
	 * now check all pseudo-buffers
	 */
	for (p=td->pty_outbufs; p; p=n)
	{
		n = p->next;

		rc = xvt_term_write_flsbuf(td, p);
		if (rc)
			break;
	}
}

/*------------------------------------------------------------------------
 * write a buffer to the child
 */
static void xvt_term_write_input (TERMDATA *td, const char *buf, int len)
{
	PTY_IOBUF *	p = &td->out_data;

	/*--------------------------------------------------------------------
	 * keybrd debugging
	 */
	xvt_log_keybrd_buf(td, buf, len);

	/*--------------------------------------------------------------------
	 * now send it
	 *
	 * This is done by caching it into the output buffer & flushing it.
	 * Note that if there is not enough room to cache it, we drop it.
	 */
	if (len < (p->buflen - p->bufcnt))
	{
		memcpy(p->bufptr + p->bufcnt, buf, len);
		p->bufcnt += len;

		xvt_term_write_flush(td);
	}
}

/*------------------------------------------------------------------------
 * write a key sequence to the child
 */
static void xvt_term_write_keys (TERMDATA *td, const char *p)
{
	int l;

	/*--------------------------------------------------------------------
	 * check that string length > 0
	 */
	l = strlen(p);
	if (l == 0)
		return;

	/*--------------------------------------------------------------------
	 * check if keyboard is locked
	 */
	if (OPTION_GET(td, SM_KbdLocked))
		return;

	/*--------------------------------------------------------------------
	 * now write it
	 */
	xvt_term_write_input(td, p, l);
}

/*------------------------------------------------------------------------
 * send data to the program (for reporting)
 */
void xvt_term_write_report (TERMDATA *td, const char *fmt, ...)
{
	char	buffer[128];
	int		len;
	va_list	args;

	va_start(args, fmt);
	len = vsprintf(buffer, fmt, args);
	va_end(args);

	xvt_term_write_input(td, buffer, len);
}

/*------------------------------------------------------------------------
 * set user's idea of the kbd state to what we think it is
 */
void xvt_term_set_kbd_state (TERMDATA *td, int it_is)
{
	int diff;

	/*--------------------------------------------------------------------
	 * Check if we need to get real kbd state
	 */
	if (it_is == -1)
		it_is = xvt_w_event_kbd_query(td->tw);

	/*--------------------------------------------------------------------
	 * Only output this if the kbd is "mapped" for these keys
	 */
	if (OPTION_GET(td, SM_KbdMapMode) == Kbd_MapAll)
	{
		/*----------------------------------------------------------------
		 * Now send necessary strings
		 */
		diff  = (td->tw->keys ^ it_is);
		if (diff != 0)
		{
			/*------------------------------------------------------------
			 * check shift key
			 */
			if (diff & KEY_MASK_SHFT)
			{
				if ((it_is & KEY_MASK_SHFT) != 0)
				{
					xvt_term_write_keys(td, td->keys_st_dn[XVT_KEY_SHFT].send);
					xvt_log_inpkbd_fmt (td, td->keys_st_dn[XVT_KEY_SHFT].name);
				}
				else
				{
					xvt_term_write_keys(td, td->keys_st_up[XVT_KEY_SHFT].send);
					xvt_log_inpkbd_fmt (td, td->keys_st_up[XVT_KEY_SHFT].name);
				}
			}

			/*------------------------------------------------------------
			 * check ctrl key
			 */
			if (diff & KEY_MASK_CTRL)
			{
				if ((it_is & KEY_MASK_CTRL) != 0)
				{
					xvt_term_write_keys(td, td->keys_st_dn[XVT_KEY_CTRL].send);
					xvt_log_inpkbd_fmt (td, td->keys_st_dn[XVT_KEY_CTRL].name);
				}
				else
				{
					xvt_term_write_keys(td, td->keys_st_up[XVT_KEY_CTRL].send);
					xvt_log_inpkbd_fmt (td, td->keys_st_up[XVT_KEY_CTRL].name);
				}
			}

			/*------------------------------------------------------------
			 * check meta key
			 */
			if (diff & KEY_MASK_META)
			{
				if ((it_is & KEY_MASK_META) != 0)
				{
					xvt_term_write_keys(td, td->keys_st_dn[XVT_KEY_META].send);
					xvt_log_inpkbd_fmt (td, td->keys_st_dn[XVT_KEY_META].name);
				}
				else
				{
					xvt_term_write_keys(td, td->keys_st_up[XVT_KEY_META].send);
					xvt_log_inpkbd_fmt (td, td->keys_st_up[XVT_KEY_META].name);
				}
			}
		}
	}

	td->tw->keys = it_is;
}

/*------------------------------------------------------------------------
 * set user's idea of the kbd state to what we think it should be
 */
void xvt_term_force_kbd_state (TERMDATA *td, int it_is)
{
	td->tw->keys = ~it_is;
	xvt_term_set_kbd_state(td, it_is);
}

/*------------------------------------------------------------------------
 * switch between color & mono if possible
 */
void xvt_term_set_scrn_mode (TERMDATA *td, int mode)
{
	/*--------------------------------------------------------------------
	 * check if this is needed
	 */
	if (td->scrn_mode == mode)
		return;

	/*--------------------------------------------------------------------
	 * set to requested mode if possible
	 */
	td->scrn_mode = xvt_w_chk_scrn_mode(td->tw, mode);

	/*--------------------------------------------------------------------
	 * set our reg font, since we use bold for color if we have it.
	 */
	xvt_term_set_reg_font(td);

	/*--------------------------------------------------------------------
	 * Now set appropriate colors
	 */
	if (CUR_SCRN(td) != 0)
	{
		CUR_SCRN_AT(td)		= A_NORMAL;
		CUR_SCRN_MODE(td)	= td->scrn_mode;

		if (td->scrn_mode == SCRN_MODE_COLOR)
		{
			td->tw->cur_fg = CUR_SCRN_FG(td);
			td->tw->cur_bg = CUR_SCRN_BG(td);
		}
		else
		{
			td->tw->cur_fg = XVT_CLR_FG;
			td->tw->cur_bg = XVT_CLR_BG;
		}
	}
	else
	{
		td->tw->cur_fg = XVT_CLR_FG;
		td->tw->cur_bg = XVT_CLR_BG;
	}

	/*--------------------------------------------------------------------
	 * now force our fg & bg colors
	 */
	xvt_w_set_fg(td->tw, td->tw->cur_fg, TRUE);
	xvt_w_set_bg(td->tw, td->tw->cur_bg, TRUE);
}

/*------------------------------------------------------------------------
 * low level output a character
 */
void xvt_term_disp_char (TERMDATA *td, int x, int y, int ch,
	int fg, int bg, int at)
{
	/*--------------------------------------------------------------------
	 * Check if we are in the wrong color mode
	 */
	if (fg < XVT_CLR_NUM && bg < XVT_CLR_NUM)
	{
		if (CUR_SCRN_MODE(td) == SCRN_MODE_MONO)
			xvt_term_set_scrn_mode(td, SCRN_MODE_COLOR);
	}
	else if (bg != XVT_CLR_CR)
	{
		if (CUR_SCRN_MODE(td) == SCRN_MODE_COLOR)
			xvt_term_set_scrn_mode(td, SCRN_MODE_MONO);
	}

	/*--------------------------------------------------------------------
	 * If in color, use attributes to adjust color
	 */
	if (CUR_SCRN_MODE(td) == SCRN_MODE_COLOR ||
	    xvt_w_get_cursor_color(td->tw) != -1)
	{
		if (at & A_BOLD)
		{
			fg |= XVT_CLR_BRIGHT;
			at &= ~A_BOLD;
		}
		xvt_w_set_fg(td->tw, fg, FALSE);

		if (at & A_BLINK)
		{
			bg |= XVT_CLR_BRIGHT;
			at &= ~A_BLINK;
		}
		xvt_w_set_bg(td->tw, bg, FALSE);

		at &= ~A_UNDERLINE;
	}

	/*--------------------------------------------------------------------
	 * Draw char according to mono attributes
	 *
	 * Here are the attributes we deal with (there may be more than
	 * one specified):
	 *
	 *	A_NORMAL		no attributes specified (this is not a bit)
	 *	A_BOLD			draw as bold char
	 *	A_BLINK			draw as bold char
	 *	A_UNDERLINE		draw line just below baseline
	 *	A_REVERSE		draw in reverse
	 *	A_HIDDEN		draw a blank instead of the char specified
	 */
	at &= A_DISPLAY;

	/*--------------------------------------------------------------------
	 * first do som preliminary checks
	 */
	if (at & A_HIDDEN)
	{
		ch = ' ';
		at &= ~A_HIDDEN;
	}

	if (at & A_BLINK)
	{
		at |=  A_BOLD;
		at &= ~A_BLINK;
	}

	/*--------------------------------------------------------------------
	 * Now check if we have a "simple" char to display
	 */
	if (at == A_NORMAL)
	{
		/*----------------------------------------------------------------
		 * draw a normal char
		 */
		xvt_w_draw_char_disp(td->tw, x, y, ch);
	}
	else
	{
		int f;
		int b;

		/*----------------------------------------------------------------
		 * switch colors if in reverse
		 */
		if (at & A_REVERSE)
		{
			f = td->tw->cur_fg;
			b = td->tw->cur_bg;
			xvt_w_set_fg(td->tw, b, FALSE);
			xvt_w_set_bg(td->tw, f, FALSE);
		}

		/*----------------------------------------------------------------
		 * now draw either a reg or bold char
		 */
		if (at & A_BOLD)
		{
			/*------------------------------------------------------------
			 * draw a bold char
			 */
			xvt_w_draw_char_bold(td->tw, x, y, ch);
		}
		else
		{
			/*------------------------------------------------------------
			 * draw a regular char
			 */
			xvt_w_draw_char_disp(td->tw, x, y, ch);
		}

		/*----------------------------------------------------------------
		 * if underlined, draw the line
		 */
		if (at & A_UNDERLINE)
			xvt_w_draw_char_line(td->tw, x, y);

		/*----------------------------------------------------------------
		 * switch colors back if we reversed them
		 */
		if (at & A_REVERSE)
		{
			xvt_w_set_fg(td->tw, f, FALSE);
			xvt_w_set_bg(td->tw, b, FALSE);
		}

	}
}

/*------------------------------------------------------------------------
 * draw box for the cursor
 */
static void xvt_term_cursor_box (TERMDATA *td)
{
	CELL *	cp;
	int		x;
	int		y;
	int		cb;
	int		fg;

	x	= CUR_SCRN_X(td);
	y	= CUR_SCRN_Y(td);

	if (x >= CUR_SCRN_COLS(td))
		x = CUR_SCRN_COLS(td) - 1;

	cp	= CUR_SCRN_CHAR(td, y, x);
	cb	= xvt_w_get_cursor_color(td->tw);
	fg	= CUR_SCRN_FG(td);

	if (cb == -1)
	{
		if (CUR_SCRN_MODE(td) == SCRN_MODE_COLOR)
			cb = cp->fg;
		else
			cb = (cp->at & A_REVERSE ? XVT_CLR_BG : XVT_CLR_FG);
	}

	xvt_w_set_fg(td->tw, cb, FALSE);
	xvt_w_draw_char_rect(td->tw, x, y);
	xvt_w_set_fg(td->tw, fg, FALSE);
}

/*------------------------------------------------------------------------
 * turn cursor display on/off
 */
void xvt_term_set_txt_cursor (TERMDATA *td, int state)
{
	CELL *	cp;
	int		old_mode	= CUR_SCRN_MODE(td);
	int		old_fg		= CUR_SCRN_FG(td);
	int		old_bg		= CUR_SCRN_BG(td);
	int		x			= CUR_SCRN_X(td);
	int		y			= CUR_SCRN_Y(td);
	int		changed		= FALSE;

	if (x >= CUR_SCRN_COLS(td))
		x = CUR_SCRN_COLS(td) - 1;

	if (state)
	{
		/*----------------------------------------------------------------
		 * turn cursor on
		 */
		if (CUR_SCRN_CSV(td) && ! CUR_SCRN_CON(td))
		{
			CUR_SCRN_CON(td) = TRUE;

			if (td->focus_mode || OPTION_GET(td, SM_HiliteCursor))
			{
				int	fg;
				int	bg;
				int	at;

				cp = CUR_SCRN_CHAR(td, y, x);

				if (xvt_w_get_cursor_color(td->tw) == -1)
				{
					fg = cp->fg;
					bg = cp->bg;
					at = cp->at & A_REVERSE ?
						cp->at & ~A_REVERSE :
						cp->at |  A_REVERSE;
				}
				else
				{
					fg = XVT_CLR_BG;
					bg = XVT_CLR_CR;
					at = A_NORMAL;
				}

				xvt_term_disp_char(td, x, y, cp->ch, fg, bg, at);
			}
			else
			{
				xvt_term_cursor_box(td);
			}

			changed = TRUE;
		}
	}
	else
	{
		/*----------------------------------------------------------------
		 * turn cursor off
		 */
		if (CUR_SCRN_CSV(td) && CUR_SCRN_CON(td))
		{
			CUR_SCRN_CON(td) = FALSE;

			cp = CUR_SCRN_CHAR(td, y, x);
			xvt_term_disp_char(td, x, y, cp->ch, cp->fg, cp->bg, cp->at);
			changed = TRUE;
		}
	}

	/*--------------------------------------------------------------------
	 * if any change made, restore color state
	 *
	 * We do this because we may be in *mono* mode, but have a
	 * text cursor which is in color.
	 */
	if (changed)
	{
		CUR_SCRN_FG(td) = old_fg;
		CUR_SCRN_BG(td) = old_bg;
		xvt_term_set_scrn_mode(td, old_mode);
	}
}

/*------------------------------------------------------------------------
 * set window cursor type
 */
void xvt_term_set_pointer (TERMDATA *td, int shape)
{
	xvt_w_set_pointer(td->tw, shape);
}

/*------------------------------------------------------------------------
 * fill a rectangle with current attributes
 */
void xvt_term_disp_fill (TERMDATA *td, int prot,
	int dx, int dy, int dw, int dh)
{
	/*--------------------------------------------------------------------
	 * set color if needed
	 */
	xvt_w_set_fg(td->tw, CUR_SCRN_FG(td), FALSE);
	xvt_w_set_bg(td->tw, CUR_SCRN_BG(td), FALSE);

	/*--------------------------------------------------------------------
	 * fill in requested area
	 */
	if (prot || CUR_SCRN_AT(td) != A_NORMAL)
	{
		/*----------------------------------------------------------------
		 * gotta preserve protected chars
		 */
		int x;
		int y;

		for (y=0; y<dh; y++)
		{
			CELL *	cp = CUR_SCRN_CHAR(td, dy + y, dx);

			for (x=0; x<dw; x++)
			{
				if ((cp->at & prot) == 0)
				{
					xvt_term_disp_char(td, dx + x, dy + y, ' ',
						cp->fg, cp->bg, cp->at);
				}
				cp++;
			}
		}
	}
	else
	{
		/*----------------------------------------------------------------
		 * no protected chars or attributes - just blast a rectangle
		 */
		xvt_w_draw_rect_fill(td->tw, dx, dy, dw, dh);
	}

	/*--------------------------------------------------------------------
	 * check if we drew over the cursor
	 */
	if (CUR_SCRN_X(td) >= dx && CUR_SCRN_X(td) <= (dx + dw) &&
		CUR_SCRN_Y(td) >= dy && CUR_SCRN_Y(td) <= (dy + dh))
	{
		CUR_SCRN_CON(td) = FALSE;
	}
}

/*------------------------------------------------------------------------
 * redraw a specified rectangle
 */
static void xvt_term_redraw_proc (TERMDATA *td, int s_x, int s_y,
	int w_x, int w_y, int invert)
{
	int x, y;
	int c_x, c_y;

	/*--------------------------------------------------------------------
	 * redraw the specified rectangle of chars
	 */
	c_y = s_y;
	for (y=0; y<w_y; y++)
	{
		CELL *	cp	= CUR_SCRN_LINE(td, c_y);

		c_x = s_x;
		for (x=0; x<w_x; x++)
		{
			if (invert)
			{
				xvt_term_disp_char(td, c_x, c_y, cp[c_x].ch,
					cp[c_x].fg, cp[c_x].bg,
					cp[c_x].at & A_REVERSE ?
						cp[c_x].at & ~A_REVERSE : cp[c_x].at | A_REVERSE);
			}
			else
			{
				xvt_term_disp_char(td, c_x, c_y, cp[c_x].ch,
					cp[c_x].fg, cp[c_x].bg, cp[c_x].at);
			}

			c_x++;
		}

		c_y++;
	}    

	/*--------------------------------------------------------------------
	 * check if cursor needs to be redisplayed
	 */
	if (CUR_SCRN_X(td) >= s_x && CUR_SCRN_X(td) < (s_x + w_x) &&
		CUR_SCRN_Y(td) >= s_y && CUR_SCRN_Y(td) < (s_y + w_y))
	{
		if (CUR_SCRN_CSV(td))
		{
			CUR_SCRN_CON(td) = FALSE;
			xvt_term_set_txt_cursor(td, TRUE);
		}
	}
}

/*------------------------------------------------------------------------
 * screen redraw CB routine
 */
static void xvt_term_repaint_proc (TERMDATA *td, int x, int y, int w, int h)
{
	if (x + w > CUR_SCRN_COLS(td))
		w = CUR_SCRN_COLS(td) - x;

	if (y + h > CUR_SCRN_ROWS(td))
		h = CUR_SCRN_ROWS(td) - y;

	if (! td->tw->doing_resize)
		xvt_term_redraw_proc(td, x, y, w, h, FALSE);
}

/*------------------------------------------------------------------------
 * repaint procedure
 */
void xvt_term_repaint (TERMDATA *td)
{
	xvt_term_repaint_proc(td, 0, 0, CUR_SCRN_COLS(td), CUR_SCRN_ROWS(td));
}

/*------------------------------------------------------------------------
 * screen interface routines
 */
void xvt_term_raise (TERMDATA *td)
{
	xvt_w_raise(td->tw);
}

void xvt_term_lower (TERMDATA *td)
{
	xvt_w_lower(td->tw);
}

void xvt_term_get_size (TERMDATA *td, int *w, int *h)
{
	xvt_w_get_size(td->tw, w, h);
}

void xvt_term_set_size (TERMDATA *td, int  w, int  h)
{
	xvt_w_set_size(td->tw, w, h);
	td->inp_bail = TRUE;
}

void xvt_term_get_position (TERMDATA *td, int *x, int *y)
{
	xvt_w_get_position(td->tw, x, y);
}

void xvt_term_set_position (TERMDATA *td, int  x, int  y)
{
	xvt_w_set_position(td->tw, x, y);
}

void xvt_term_maximize (TERMDATA *td)
{
	xvt_w_set_max_size(td->tw);
	td->inp_bail = TRUE;
}

void xvt_term_restore (TERMDATA *td)
{
	xvt_w_set_old_size(td->tw);
	td->inp_bail = TRUE;
}

void xvt_term_resize (TERMDATA *td, int cols, int rows)
{
	xvt_w_resize(td->tw, cols, rows);
	td->inp_bail = TRUE;
}

/*------------------------------------------------------------------------
 * screen flash routine
 */
void xvt_term_flash (TERMDATA *td)
{
	xvt_term_redraw_proc(td, 0, 0, CUR_SCRN_COLS(td), CUR_SCRN_ROWS(td),
		TRUE);
	xvt_term_redraw_proc(td, 0, 0, CUR_SCRN_COLS(td), CUR_SCRN_ROWS(td),
		FALSE);
}

/*------------------------------------------------------------------------
 * low-level area copy routine
 */
void xvt_term_disp_copy (TERMDATA *td, int dx, int dy, int dw, int dh,
	int sx, int sy)
{
	xvt_w_draw_rect_copy(td->tw, dx, dy, dw, dh, sx, sy);
}

/*------------------------------------------------------------------------
 * print the screen
 */
static void xvt_term_screen_printscrn (TERMDATA *td, TERMSCRN *s, int decpex)
{
	int top;
	int bot;
	int left;
	int right;

	if (decpex)
	{
		top	= SCRN_TOP(s);
		bot	= SCRN_BOT(s);
		left	= SCRN_LEFT(s);
		right	= SCRN_RIGHT(s);
	}
	else
	{
		top	= 0;
		bot	= SCRN_ROWS(s) - 1;
		left	= 0;
		right	= SCRN_COLS(s) - 1;
	}

	xvt_scrn_print(s, td->prt, top, bot, left, right,
		OPTION_GET(td, SM_PrintScreenFF));
}

void xvt_term_screen_print (TERMDATA *td, int all, int decpex)
{
	xvt_term_printer_open(td);
	if (td->prt != 0)
	{
		if (all)
		{
			xvt_term_screen_printscrn(td, td->reg_scrn, decpex);
			xvt_term_screen_printscrn(td, td->alt_scrn, decpex);
		}
		else
		{
			xvt_term_screen_printscrn(td, CUR_SCRN(td), decpex);
		}
		xvt_term_printer_close(td);
	}
}

/*------------------------------------------------------------------------
 * print line cursor is on
 */
void xvt_term_screen_print_line (TERMDATA *td)
{
	xvt_term_printer_open(td);
	if (td->prt != 0)
	{
		xvt_scrn_print_line(CUR_SCRN(td), td->prt, CUR_SCRN_Y(td));
		xvt_term_printer_close(td);
	}
}

/*------------------------------------------------------------------------
 * copy the screen to the clipboard
 */
void xvt_term_screen_clipboard (TERMDATA *td)
{
	unsigned char *	scr_cb_data;
	int				scr_cb_len;
	unsigned char *	s;
	int				x;
	int				y;

	scr_cb_len  = (CUR_SCRN_ROWS(td) * (CUR_SCRN_COLS(td) + 1)) + 1;
	scr_cb_data = (unsigned char *)MALLOC(scr_cb_len);
	if (scr_cb_data == 0)
		return;

	s = scr_cb_data;
	for (y=0; y < CUR_SCRN_ROWS(td); y++)
	{
		CELL *	cp = CUR_SCRN_LINE(td, y);

		for (x=0; x < CUR_SCRN_COLS(td); x++)
		{
			*s++ = (cp++)->ch;
		}
		*s++ = '\n';
	}
	*s = 0;

	if (td->tw->selection_ptr != 0)
	{
		FREE(td->tw->selection_ptr);
	}
	td->tw->selection_ptr = scr_cb_data;
	td->tw->selection_len = scr_cb_len;

	xvt_w_cb_copy_to(td->tw);
}

/*------------------------------------------------------------------------
 * process a screen-resize message
 */
static void xvt_term_resize_proc (TERMDATA *td, int w, int h)
{
	/*--------------------------------------------------------------------
	 * check if window changed size
	 */
	if (td->pid == 0 || (w != CUR_SCRN_COLS(td) || h != CUR_SCRN_ROWS(td)))
	{
		/*----------------------------------------------------------------
		 * create new screen array & copy old contents to it
		 */
		xvt_term_setup_text(td, w, h);

		/*----------------------------------------------------------------
		 * change size of pseudo-tty
		 */
		xvt_pty_resize(td->pd, w, h, td->pid);

		/*----------------------------------------------------------------
		 * debug
		 */
		xvt_log_events_fmt(td, "events", "new screen (%d,%d)", w, h);

		if (td->pid != 0)
		{
			xvt_log_events_fmt(td, "events", "sending SIGWINCH to %d",
				td->pid);
		}

		/*----------------------------------------------------------------
		 * if not detached, send RESIZE key
		 */
		if (! td->detached || td->no_detach)
		{
			xvt_term_write_keys(td,
				td->keys_ms[XVT_KEY_MS_RESIZE & 0xff].send);
		}
	}

	/*--------------------------------------------------------------------
	 * check cursor position
	 */
	if (CUR_SCRN_X(td) >= w)
		CUR_SCRN_X(td) = w-1;

	if (CUR_SCRN_Y(td) >= h)
		CUR_SCRN_Y(td) = h-1;
}

/*------------------------------------------------------------------------
 * output the current mouse status (position & button state)
 */
static void xvt_term_mouse_output (TERMDATA *td)
{
	int	b		= 0;
	int	s		= FALSE;
	int mode	= OPTION_GET(td, SM_MouseMode);

	/*--------------------------------------------------------------------
	 * Determine what & when to send mouse data.
	 *
	 *	Mouse data is always sent as:
	 *
	 *			ESC [ M Cb Cx Cy
	 *
	 *	Cb, Cx, & Cy are button info, and x & y position. They are
	 *	each encoded as one byte + ' ' (0x20).  x & y are one-based,
	 *	i.e., the top left is (1,1).
	 *
	 *	1.	xterm mode:
	 *
	 *		X10 mode:			Send info only if button pressed.
	 *		X11 mode:			Send info only if button state changed.
	 *		CellMotion mode:	Send info if button state changed or
	 *							cell location changed.
	 *		AllMotion mode:		Always send info.
	 *
	 *		Button info is encoded as follows:
	 *
	 *			bits 0-1	(0x03)	button info:
	 *
	 *						0	button 1 pressed
	 *						1	button 2 pressed
	 *						2	button 3 pressed
	 *						3	some button was released (no info on which)
	 *
	 *			bit  2		(0x04)	shift is down
	 *			bit  3		(0x08)	meta (alt) is down
	 *			bit  4		(0x10)	control is down
	 *			bit  5		(0x20)	reserved (for + ' ' encoding)
	 *			bit  6		(0x40)	1 = no button info, motion only
	 *								(CellMotion & AllMotion only)
	 *			bit  7		(0x80)	always 0
	 *	
	 *	2.	xvt mode:
	 *
	 *		Send info if button state changed or cell location changed.
	 *		Button info is encoded as follows:
	 *
	 *			bit  0		(0x01)	button 1 state (0 = dn, 1 = up)
	 *			bit  1		(0x02)	button 2 state (0 = dn, 1 = up)
	 *			bit  2		(0x04)	button 3 state (0 = dn, 1 = up)
	 *			bit  3		(0x08)	1 = scroll-wheel up one notch
	 *			bit  4		(0x10)	1 = scroll-wheel dn one notch
	 *			bit  5		(0x20)	reserved (for + ' ' encoding)
	 *			bit  6		(0x40)	1 = no button change, motion only
	 *			bit  7		(0x80)	always 1
	 *
	 *		Note that if the button info did not change, but only the
	 *		x,y changed, then this is a motion event. Also, a button
	 *		event can also contain a motion event.
	 *
	 *		Motion events are triggered only by the mouse moving across
	 *		a "character" boundary. Thus, if a mouse only moves a few pixels
	 *		within a character-cell on the screen, no movement is reported.
	 */
	switch (mode)
	{
	case Mouse_X10:
		/*----------------------------------------------------------------
		 * X10 mode
		 * only output data if it is a button press
		 */
		if (td->mouse_btns == 0)
			break;
		/*FALLTHROUGH*/

	case Mouse_X11:
		/*----------------------------------------------------------------
		 * X10 & X11 mode
		 * only output data if any mouse button changed
		 */
		if (td->mouse_bprev == td->mouse_btns)
			break;
		/*FALLTHROUGH*/

	case Mouse_CellMotion:
		/*----------------------------------------------------------------
		 * X10, X11 & CellMotion mode
		 * if motion only, only output if char cell changed
		 */
		if (td->mouse_bprev == td->mouse_btns)
		{
			if (td->mouse_col == td->mouse_cprev &&
			    td->mouse_row == td->mouse_rprev)
			{
				break;
			}
		}
		/*FALLTHROUGH*/

	case Mouse_AllMotion:
		/*----------------------------------------------------------------
		 * get button change mask
		 */
		if      ((td->mouse_btns & BUT_L) && ! (td->mouse_bprev & BUT_L))
			b = 0x00;
		else if ((td->mouse_btns & BUT_M) && ! (td->mouse_bprev & BUT_M))
			b = 0x01;
		else if ((td->mouse_btns & BUT_R) && ! (td->mouse_bprev & BUT_R))
			b = 0x02;
		else
			b = 0x03;

		/*----------------------------------------------------------------
		 * If not X10 or X11 mode, add in motion-only state if buttons
		 * didn't change
		 */
		if (td->mouse_bprev == td->mouse_btns)
			b |= 0x40;

		/*----------------------------------------------------------------
		 * If not X10 mode, add in meta-key states
		 */
		if (mode != Mouse_X10)
		{
			int k = td->tw->keys;

			if (k & KEY_MASK_SHFT)	b |= 0x04;
			if (k & KEY_MASK_META)	b |= 0x08;
			if (k & KEY_MASK_CTRL)	b |= 0x10;
		}

		s = TRUE;
		break;

	case Mouse_XVT:
		/*----------------------------------------------------------------
		 * xvt mode
		 * output info if button changed or cell location changed
		 */
		if (td->mouse_btns != td->mouse_bprev ||
			td->mouse_col  != td->mouse_cprev ||
		    td->mouse_row  != td->mouse_rprev)
		{
			b = td->mouse_btns;
			if (td->mouse_btns == td->mouse_bprev)
				b |= 0x40;
			b |= 0x80;

			s = TRUE;
		}
		break;
	}

	if (s)
	{
		char	mouse_buf[128];

		sprintf(mouse_buf, "\033[M%c%c%c",
			b + ' ',
			(td->mouse_col + 1) + ' ',
			(td->mouse_row + 1) + ' ');
		xvt_term_write_keys(td, mouse_buf);

		xvt_log_inpkbd_fmt(td, "mouse(%02x,%3d,%3d)",
			b, td->mouse_col, td->mouse_row);
	}
}

/*------------------------------------------------------------------------
 * action debugging
 */
static void xvt_term_debug_action (TERMDATA *td, const char *fmt, ...)
{
	va_list	args;

	va_start(args, fmt);
	xvt_log_action_var(td, fmt, args);
	va_end(args);
}

/*------------------------------------------------------------------------
 * process key-press event
 *
 * This routine processes key up/down events.  We always process down
 * events, but the only up events we are interested in are for "modifier"
 * keys (shift, control, alt).
 */
static void xvt_term_event_key (TERMDATA *td, XVT_EVENT_KEYPRESS *xe)
{
	char			keybuf[2];
	int				shft;
	int				ctrl;
	int				meta;
	int				rshft;
	int				rctrl;
	int				rmeta;
	int				state;
	int				send;
	int				key;
	int				kbd_state;
	const char *	p = 0;
	const char *	k = 0;

	/*--------------------------------------------------------------------
	 * get state (TRUE if down)
	 */
	state	= xe->key_state;
	send	= state;

	kbd_state	= xe->kbd_state;

	/*--------------------------------------------------------------------
	 * cache modifiers
	 */
	rshft	= shft	= ( (xe->kbd_state & KEY_MASK_SHFT) != 0 );
	rctrl	= ctrl	= ( (xe->kbd_state & KEY_MASK_CTRL) != 0 );
	rmeta	= meta	= ( (xe->kbd_state & KEY_MASK_META) != 0 );

	/*--------------------------------------------------------------------
	 * check key value
	 */
	key = xe->key_value;
	if (key < 0)
		return;

	/*--------------------------------------------------------------------
	 * if lookup was successful, use it
	 */
	if (key <= 0xff)
	{
		/*----------------------------------------------------------------
		 * regular or control key
		 */
		if (key <= 0x1f)
		{
			/*------------------------------------------------------------
			 * use control char table
			 */
			p = td->keys_ct[key].send;
			k = td->keys_ct[key].name;

			/*------------------------------------------------------------
			 * special checks
			 */
			switch (key)
			{
			case '\r':
				if (OPTION_GET(td, SM_AutoNL))
					p = "\r\n";
				break;

			case '\b':
				if (OPTION_GET(td, SM_BSsendsDel))
					p = "\177";
				break;
			}
		}
		else
		{
			/*------------------------------------------------------------
			 * regular key
			 */
			keybuf[0] = key;
			keybuf[1] = 0;

			p = keybuf;
			k = keybuf;
		}

		shft = ctrl = FALSE;

		/*----------------------------------------------------------------
		 * check if doing local echo
		 */
		if (OPTION_GET(td, SM_LocalEcho) && send)
			xvt_emul_local_echo(td, key);
	}
	else
	{
		/*----------------------------------------------------------------
		 * special key
		 */
		int	key_type	= key & 0xff;
		int	key_mask	= key & XVT_KEY_MASK;

		if (key_mask == XVT_KEY_FN)
		{
			/*------------------------------------------------------------
			 * function key
			 */
			p = td->keys_fn[key_type].send;
			k = td->keys_fn[key_type].name;
		}

		else if (key_mask == XVT_KEY_KP)
		{
			/*------------------------------------------------------------
			 * keypad key
			 */
			p = td->keys_kp[key_type].send;
			k = td->keys_kp[key_type].name;

			/*------------------------------------------------------------
			 * check if special key
			 */
			if (send && (td->tw->keys & KEY_MASK_NUML) == 0)
			{
				switch (key)
				{
				case XVT_KEY_KP_ADD:
					if (OPTION_GET(td, SM_SpecialKeys))
					{
						xvt_w_font_chg_no(td->tw, +1);
						send = FALSE;
					}
					break;

				case XVT_KEY_KP_SUB:
					if (OPTION_GET(td, SM_SpecialKeys))
					{
						xvt_w_font_chg_no(td->tw, -1);
						send = FALSE;
					}
					break;

				case XVT_KEY_KP_DELETE:
					if (OPTION_GET(td, SM_DELsendsDel))
					{
						p = "\177";
					}
					break;
				}
			}
		}

		else if (key_mask == XVT_KEY_MS)
		{
			/*------------------------------------------------------------
			 * misc key
			 */
			p = td->keys_ms[key_type].send;
			k = td->keys_ms[key_type].name;

			/*------------------------------------------------------------
			 * check if special key
			 */
			if (send)
			{
				switch (key)
				{
				case XVT_KEY_MS_PRTSCR:
					if (meta)
						xvt_term_screen_clipboard(td);
					else
						xvt_term_screen_print(td, FALSE, FALSE);
					send = FALSE;
					break;
				}
			}
		}

		else if (key_mask == XVT_KEY_PF)
		{
			/*------------------------------------------------------------
			 * prefix key
			 *
			 * Note that when a prefix key is pressed, the state-bit is
			 * not set (e.g. when the SHIFT key is pressed, the SHIFT bit
			 * in the event struct is not on yet).
			 *
			 * Note that we don't send these keys directly, as they are
			 * sent by the call to xvt_term_set_kbd_state().
			 */
			send = FALSE;
			shft = ctrl = meta = FALSE;

			if (state)
			{
				p = td->keys_st_dn[key_type].send;
				k = td->keys_st_dn[key_type].name;
			}
			else
			{
				p = td->keys_st_up[key_type].send;
				k = td->keys_st_up[key_type].name;
			}

			if (state)
				kbd_state |=  xvt_term_key_masks[key_type];
			else
				kbd_state &= ~xvt_term_key_masks[key_type];
		}
	}

	/*--------------------------------------------------------------------
	 * check if we need to output keyboard state
	 */
	xvt_term_set_kbd_state(td, kbd_state);

	/*--------------------------------------------------------------------
	 * If we got a valid key, output it
	 */
	if (p != 0)
	{
		/*----------------------------------------------------------------
		 * debug output
		 */
		xvt_term_debug_action(td,
			"key %s (%c%c%c %04x) (%s%s%s%s) [%02x]",
			state ? "dn" : "up",
			rmeta ? 'm' : '.',
			rctrl ? 'c' : '.',
			rshft ? 's' : '.',
			xe->key_symbol,
			meta ? td->keys_pf[XVT_KEY_META].name : "",
			ctrl ? td->keys_pf[XVT_KEY_CTRL].name : "",
			shft ? td->keys_pf[XVT_KEY_SHFT].name : "",
			k,
			td->tw->keys);

		/*----------------------------------------------------------------
		 * write key to child if not empty & if we should send this key
		 */
		if (send && *p != 0)
		{
			char buf[128];

			*buf = 0;
			if (meta) strcat(buf, td->keys_pf[XVT_KEY_META].send);
			if (ctrl) strcat(buf, td->keys_pf[XVT_KEY_CTRL].send);
			if (shft) strcat(buf, td->keys_pf[XVT_KEY_SHFT].send);
			strcat(buf, p);
			xvt_term_write_keys(td, buf);

			xvt_log_inpkbd_fmt(td, "%s%s%s%s",
				meta ? td->keys_pf[XVT_KEY_META].name : "",
				ctrl ? td->keys_pf[XVT_KEY_CTRL].name : "",
				shft ? td->keys_pf[XVT_KEY_SHFT].name : "",
				k);

			td->read_pending = TRUE;
		}
	}
	else
	{
		xvt_term_debug_action(td,
			"key %s (%c%c%c %04x)",
			state ? "dn" : "up",
			rmeta ? 'm' : '.',
			rctrl ? 'c' : '.',
			rshft ? 's' : '.',
			xe->key_symbol);
	}
}

/*------------------------------------------------------------------------
 * process keymap-notify event
 *
 * This routine processes keymap notify events.  These events are always
 * sent after we get focus, and it is used to determine the state of
 * "modifier" keys (shift / control / alt) at the time we get focus.
 */
static void xvt_term_event_keymap (TERMDATA *td, XVT_EVENT_KEYMAP *xe)
{
	xvt_term_debug_action(td, "keymap (%c%c%c)",
		xe->kbd_state & KEY_MASK_META ? 'm' : '.',
		xe->kbd_state & KEY_MASK_CTRL ? 'c' : '.',
		xe->kbd_state & KEY_MASK_SHFT ? 's' : '.');

	xvt_term_set_kbd_state(td, xe->kbd_state);
}

/*------------------------------------------------------------------------
 * process button event
 *
 * This routine processes button up/down events, and can also generate
 * a pointer-moved event.
 */
static void xvt_term_event_button (TERMDATA *td, XVT_EVENT_BUTTON *xe)
{
	const char *	but_no;
	const char *	but_pos;

	/*--------------------------------------------------------------------
	 * save old button state
	 */
	td->mouse_bprev = td->mouse_btns;

	/*--------------------------------------------------------------------
	 * get button number
	 */
	switch (xe->button)
	{
	case BUT_L:	but_no = "L";	break;
	case BUT_M:	but_no = "M";	break;
	case BUT_R:	but_no = "R";	break;
	case BUT_U:	but_no = "U";	break;
	case BUT_D:	but_no = "D";	break;
	}

	/*--------------------------------------------------------------------
	 * get up/dn state
	 */
	switch (xe->button)
	{
	case BUT_L:
	case BUT_M:
	case BUT_R:
		if (xe->btn_state)
		{
			td->mouse_btns |=  xe->button;
			but_pos = "dn";
		}
		else
		{
			td->mouse_btns &= ~xe->button;
			but_pos = "up";
		}
		break;

	case BUT_U:
	case BUT_D:
		td->mouse_btns |= xe->button;
		but_pos      = "--";
		break;
	}

	/*--------------------------------------------------------------------
	 * save old col/row position
	 */
	td->mouse_cprev	= td->mouse_col;
	td->mouse_rprev	= td->mouse_row;

	/*--------------------------------------------------------------------
	 * get (x,y) in pixels & chars
	 */
	td->mouse_x		= xe->x;
	td->mouse_y		= xe->y;

	td->mouse_col	= xe->col;
	td->mouse_row	= xe->row;

	/*--------------------------------------------------------------------
	 * debug output
	 */
	xvt_term_debug_action(td,
		"ptr btn   (%3d, %3d) [%4d, %4d] (%s %s)",
		td->mouse_col, td->mouse_row,
		td->mouse_x, td->mouse_y,
		but_no, but_pos);

	/*--------------------------------------------------------------------
	 * send to child if asked for
	 */
	if (OPTION_GET(td, SM_MouseMode) != Mouse_None)
		xvt_term_mouse_output(td);

	/*--------------------------------------------------------------------
	 * clear out transient (scroll-wheel) events
	 */
	td->mouse_btns &= ~(BUT_U | BUT_D);
}

/*------------------------------------------------------------------------
 * process pointer-moved event
 *
 * This routine processes pointer-movement events.  Note that a pointer-moved
 * event can also be internally generated in conjunction with a button event.
 */
static void xvt_term_event_motion (TERMDATA *td, XVT_EVENT_MOTION *xe)
{
	/*--------------------------------------------------------------------
	 * save old button state
	 */
	td->mouse_bprev = td->mouse_btns;

	/*--------------------------------------------------------------------
	 * save old col/row position
	 */
	td->mouse_cprev	= td->mouse_col;
	td->mouse_rprev	= td->mouse_row;

	/*--------------------------------------------------------------------
	 * set new values
	 */
	td->mouse_x		= xe->x;
	td->mouse_y		= xe->y;

	td->mouse_col	= xe->col;
	td->mouse_row	= xe->row;

	/*--------------------------------------------------------------------
	 * debug output
	 */
	xvt_term_debug_action(td,
		"ptr move  (%3d, %3d) [%4d, %4d]",
		td->mouse_col, td->mouse_row, td->mouse_x, td->mouse_y);

	/*--------------------------------------------------------------------
	 * send to child if he asked for it
	 */
	if (OPTION_GET(td, SM_MouseMode) != Mouse_None)
		xvt_term_mouse_output(td);
}

/*------------------------------------------------------------------------
 * process focus event
 *
 * This routine processes focus events.  It is used to show the cursor
 * as "solid" or "box", depending on whether we have focus or not.
 */
static void xvt_term_event_focus (TERMDATA *td, XVT_EVENT_FOCUS *xe)
{
	/*--------------------------------------------------------------------
	 * debug output
	 */
	xvt_term_debug_action(td,
		"focus %s",
		xe->focus ? "in" : "out");

	/*--------------------------------------------------------------------
	 * set cursor mode according to focus
	 */
	td->focus_mode = xe->focus;

	/*--------------------------------------------------------------------
	 * If losing focus,  set kbd_state to none.
	 * If gaining focus, set kbd_state to real state.
	 */
	xvt_term_set_kbd_state(td, td->focus_mode ? -1 : 0);

	/*--------------------------------------------------------------------
	 * now redraw cursor if visible
	 */
	if (CUR_SCRN_CSV(td))
	{
		xvt_term_set_txt_cursor(td, FALSE);
		xvt_term_set_txt_cursor(td, TRUE);
	}
}

/*------------------------------------------------------------------------
 * process expose event
 *
 * This routine processes expose events, which are represented as rectangles
 * which must be redrawn.  These events are internal only, i.e. they are
 * not sent to the "child" process.
 */
static void xvt_term_event_expose (TERMDATA *td, XVT_EVENT_EXPOSE *xe)
{
	int x, y;
	int w, h;

	/*--------------------------------------------------------------------
	 * check if icon expose
	 */
	if (xe->type == XVT_EXPOSE_TYPE_ICON)
	{
		xvt_term_debug_action(td, "expose icon");
		xvt_w_set_icon_display(td->tw, CUR_SCRN(td));
		return;
	}

	/*--------------------------------------------------------------------
	 * convert info to cells
	 */
	x = xe->x;
	y = xe->y;

	w = xe->w;
	h = xe->h;

	if ((x + w) > CUR_SCRN_COLS(td))
		w = CUR_SCRN_COLS(td) - x;

	if ((y + h) > CUR_SCRN_ROWS(td))
		h = CUR_SCRN_ROWS(td) - y;

	/*--------------------------------------------------------------------
	 * debug output
	 */
	xvt_term_debug_action(td,
		"expose    (%3d, %3d, %3d, %3d)",
		x, y, w, h);

	/*--------------------------------------------------------------------
	 * now do the repaint
	 */
	xvt_term_repaint_proc(td, x, y, w, h);
}

/*------------------------------------------------------------------------
 * process configure event
 *
 * This routine processes window-resize events. Note that we have set
 * the window to only allow the size to be a multiple of the font-char
 * size, so we can just take the new size & allocate new screen buffers.
 * This will also result in the "child" process being sent a resize signal.
 */
static void xvt_term_event_config (TERMDATA *td, XVT_EVENT_CONFIG *xe)
{
	/*--------------------------------------------------------------------
	 * debug output
	 */
	xvt_term_debug_action(td,
		"configure (%3d, %3d)",
		xe->w, xe->h);

	/*--------------------------------------------------------------------
	 * ignore event if it is a resize following a resize
	 */
	if (td->tw->prev_action == XVT_E_CONFIG)
		return;

	/*--------------------------------------------------------------------
	 * do the resize
	 */
	xvt_term_resize_proc(td, xe->w, xe->h);
	td->tw->doing_resize = TRUE;
}

/*------------------------------------------------------------------------
 * process client event
 *
 * This routine processes Window manager events which are sent to us.
 * Right now the only event we look for is WM_DELETE, which is a quit msg.
 */
static void xvt_term_event_client (TERMDATA *td, XVT_EVENT_WINMSG *xe)
{
	/*--------------------------------------------------------------------
	 * process message
	 */
	switch (xe->winmsg)
	{
	case XVT_W_DELETE:
		/*----------------------------------------------------------------
		 * debug output
		 */
		xvt_term_debug_action(td, "wm-delete");

		/*----------------------------------------------------------------
		 * tell child to terminate
		 */
		if (td->pid != 0)
		{
			sys_sig_send(td->pid, SIGTERM);
		}

		/*----------------------------------------------------------------
		 * start shutdown process
		 */
		td->bail = 1;
		break;
	}
}

/*------------------------------------------------------------------------
 * process map event
 */
static void xvt_term_event_map (TERMDATA *td, XVT_EVENT_MAP *xe)
{
	/*--------------------------------------------------------------------
	 * here is where we could send a MAP/UNMAP key to the program
	 */
	if (xe->mapped)
	{
		xvt_term_debug_action(td, "map window");
	}
	else
	{
		xvt_term_debug_action(td, "unmap window");
		xvt_w_set_icon_display(td->tw, CUR_SCRN(td));
	}
}

/*------------------------------------------------------------------------
 * process an event
 *
 * Here is where all X events we are interested in are processed.
 */
static void xvt_term_process_event (TERMDATA *td, XVT_EVENT *xe)
{
	EVENT_DATA *	xk		= &xe->event_data;
	int				action	= xe->event_type;

	td->tw->doing_resize = FALSE;

	/*--------------------------------------------------------------------
	 * perform proper event procedure
	 */
	switch (action)
	{
	case XVT_E_KEYPRESS:
						xvt_term_event_key		(td, &xk->keypress);	break;
	case XVT_E_KEYMAP:	xvt_term_event_keymap	(td, &xk->keymap);		break;
	case XVT_E_BUTTON:	xvt_term_event_button	(td, &xk->button);		break;
	case XVT_E_MOTION:	xvt_term_event_motion	(td, &xk->motion);		break;
	case XVT_E_FOCUS:	xvt_term_event_focus	(td, &xk->focus);		break;
	case XVT_E_EXPOSE:	xvt_term_event_expose	(td, &xk->expose);		break;
	case XVT_E_CONFIG:	xvt_term_event_config	(td, &xk->config);		break;
	case XVT_E_WINMSG:	xvt_term_event_client	(td, &xk->winmsg);		break;
	case XVT_E_MAP:		xvt_term_event_map		(td, &xk->map);			break;

	default:
		xvt_term_debug_action(td, "action %d", action);
		break;
	}

	td->tw->prev_action = action;
}

/*------------------------------------------------------------------------
 * set window title
 */
void xvt_term_set_win_title (TERMDATA *td, char *str)
{
	char	temp_title[256];

	if (str != 0 && *str != 0)
	{
		if (OPTION_GET(td, SM_LabelInfo) && td->pd != 0)
		{
			strcpy(temp_title, str);
			sprintf(temp_title + strlen(temp_title),
				" [%d: %s: %s]",
				td->our_pid,
				td->pd->slv_name,
				xvt_w_font_get_name(td->tw, td->tw->cur_font_no));
			str = temp_title;
		}

		xvt_w_set_win_title(td->tw, str);
	}
}

/*------------------------------------------------------------------------
 * set icon title
 */
void xvt_term_set_ico_title (TERMDATA *td, char *str)
{
	if (str != 0 && *str != 0)
		xvt_w_set_ico_title(td->tw, str);
}

/*------------------------------------------------------------------------
 * set the icon for a window
 */
void xvt_term_set_icon (TERMDATA *td, int width, int height,
	unsigned char *bits)
{
	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (width <= 0 || height <= 0 || bits == 0)
		return;

	/*--------------------------------------------------------------------
	 * cache icon data
	 */
	td->ico_bits	= bits;
	td->ico_width	= width;
	td->ico_height	= height;

	/*--------------------------------------------------------------------
	 * now set it
	 */
	xvt_w_set_icon(td->tw, td->ico_width, td->ico_height, td->ico_bits);
}

/*------------------------------------------------------------------------
 * iconify the window
 */
void xvt_term_set_icon_state (TERMDATA *td, int state)
{
	xvt_w_set_icon_state(td->tw, state);
}

/*------------------------------------------------------------------------
 * change a color name
 */
void xvt_term_chg_color (TERMDATA *td, int n, const char *color)
{
	int	rc;

	rc = xvt_w_chg_clr(td->tw, n, color);
	if (rc == 0)
	{
		/*----------------------------------------------------------------
		 * We have to repaint all occurances of the old color to
		 * the new color.  We will, for now, just take the lazy route
		 * of repainting the window (with some exceptions).
		 */
		if (n <  XVT_CLR_NUM ||
		    n == XVT_CLR_FG  ||
		    n == XVT_CLR_BG  ||
			n == XVT_CLR_CR)
		{
			/*------------------------------------------------------------
			 * We do, however, have to force the colors.
			 */
			xvt_w_set_fg(td->tw, td->tw->cur_fg, TRUE);
			xvt_w_set_bg(td->tw, td->tw->cur_bg, TRUE);

			xvt_term_repaint(td);
		}
	}
}

/*------------------------------------------------------------------------
 * get a color name
 */
const char * xvt_term_get_color_name (TERMDATA *td, int n)
{
	return xvt_w_get_color_name(td->tw, n);
}

/*------------------------------------------------------------------------
 * set font name
 */
void xvt_term_set_font_name (TERMDATA *td, int n, const char *font_name)
{
	if (n < 0)
		n = td->tw->cur_font_no;
	xvt_w_font_set_name(td->tw, n, font_name);
	if (n == td->tw->cur_font_no)
		xvt_term_set_win_title(td, td->win_label);
}

/*------------------------------------------------------------------------
 * get font name
 */
const char * xvt_term_get_font_name (TERMDATA *td, int n)
{
	return (xvt_w_font_get_name(td->tw, n));
}

/*------------------------------------------------------------------------
 * set font number
 */
void xvt_term_set_font_no (TERMDATA *td, int n)
{
	xvt_w_font_set_no(td->tw, n);
	xvt_term_set_win_title(td, td->win_label);
}

/*------------------------------------------------------------------------
 * change font number
 */
void xvt_term_chg_font_no (TERMDATA *td, int n)
{
	xvt_w_font_chg_no(td->tw, n);
	xvt_term_set_win_title(td, td->win_label);
}

/*------------------------------------------------------------------------
 * set reverse video
 */
void xvt_term_set_rev_video (TERMDATA *td, int reverse)
{
	xvt_w_set_rev_video(td->tw, reverse);
	td->scrn_mode = SCRN_MODE_NONE;
	xvt_term_set_scrn_mode(td, SCRN_MODE_MONO);
	xvt_term_repaint(td);
}

/*------------------------------------------------------------------------
 * sound the bell
 */
void xvt_term_bell (TERMDATA *td)
{
	if (! OPTION_GET(td, SM_IgnoreBell))
	{
		/*----------------------------------------------------------------
		 * check if we need to unmap
		 */
		if (td->tw->iconified && OPTION_GET(td, SM_MapOnAlert))
			xvt_w_set_icon_state(td->tw, FALSE);

		/*----------------------------------------------------------------
		 * check if visual or auditory
		 */
		if (OPTION_GET(td, SM_VisualBell))
			xvt_term_flash(td);
		else
			xvt_w_bell(td->tw);
	}
}

/*------------------------------------------------------------------------
 * open the printer
 */
void xvt_term_printer_open (TERMDATA *td)
{
	if (td->prt == 0)
	{
		td->prt = xvt_prt_open(P_NONE, td->xd->printer);
	}
}

/*------------------------------------------------------------------------
 * close the printer
 */
void xvt_term_printer_close (TERMDATA *td)
{
	if (! OPTION_GET(td, SM_PrinterKeep))
	{
		if (td->prt != 0)
		{
			xvt_prt_close(td->prt);
			td->prt = 0;
		}
	}
}

/*------------------------------------------------------------------------
 * fill input buffer from child output
 */
static int xvt_term_fill_buffer (TERMDATA *td, int have_real)
{
	PTY_IOBUF *	p	= td->inp_current;
	int			num;

	/*--------------------------------------------------------------------
	 * If we have data in the buffer, just return with what we have,
	 * since a read may block at this point.
	 */
	if (p->bufpos < p->bufcnt)
		return (0);

	/*--------------------------------------------------------------------
	 * We are done with this input buffer.  Check if it should be freed.
	 */
	if (p->bufheap)
	{
		/*----------------------------------------------------------------
		 * Note that if this is on the heap, it will be the first one
		 * on the input chain.
		 */
		sysmutex_enter(&td->pty_inpmtx);
		{
			td->pty_inpbufs = p->next;
		}
		sysmutex_leave(&td->pty_inpmtx);

		FREE(p);
	}

	/*--------------------------------------------------------------------
	 * Now check if there are any pseudo-buffers to process.
	 */
	if (td->pty_inpbufs != 0)
	{
		td->inp_current = td->pty_inpbufs;
		return (0);
	}

	/*--------------------------------------------------------------------
	 * If no real input is pending, just bail, since we don't want to block.
	 */
	td->inp_current = &td->inp_data;
	p = td->inp_current;
	p->bufcnt = 0;
	p->bufpos = 0;

	if (! have_real)
		return (0);

	/*--------------------------------------------------------------------
	 * Now we have to do a real read.  We always read as much as we can,
	 * up to the size of our input buffer.
	 */
	num = read(td->pd->mst_fd, p->bufptr, p->buflen);

	/*--------------------------------------------------------------------
	 * log the results of our read
	 */
	xvt_log_screen_buf(td, p->bufptr, num);

	/*--------------------------------------------------------------------
	 * Check if we got an EOF from the read.
	 * In task mode, this would indicate that the sub-task terminated.
	 * In here mode, this would indicate that the process closed the pty.
	 */
	if (num < 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * cache the number of chars that we read
	 */
	p->bufcnt	= num;

	return (0);
}

/*------------------------------------------------------------------------
 * process child output
 */
static int xvt_term_process_child_output (TERMDATA *td, int have_real)
{
	PTY_IOBUF *	p;
	int			count;
	int			rc;

	/*--------------------------------------------------------------------
	 * check if we need to unmap
	 */
	if (td->tw->iconified && OPTION_GET(td, SM_MapOnOutput))
		xvt_w_set_icon_state(td->tw, FALSE);

	/*--------------------------------------------------------------------
	 * read in as much child output as we can
	 */
	rc = xvt_term_fill_buffer(td, have_real);
	if (rc)
		return (-1);

	/*--------------------------------------------------------------------
	 * now process the input
	 */
	p = td->inp_current;
	count = 0;
	while (p->bufpos < p->bufcnt)
	{
		int ch;

		ch = p->bufptr[p->bufpos++];

		count++;
		xvt_emul_char_input(td, ch);

		if (td->inp_bail)
		{
			td->inp_bail = FALSE;
			break;
		}
	}

	return (count);
}

/*------------------------------------------------------------------------
 * loop once
 *
 *	returns:
 *		-1	time to bail
 *		 0	OK
 */
int xvt_term_loop_once (TERMDATA *td, int ms)
{
	fd_set			readfds;
	struct timeval	tv;
	int				rc;
	int				xfd;
	int				cfd;
	int				max_fd;
	int				secs;
	int				usecs;
	int				input_pending;
	int				have_real;

	/*--------------------------------------------------------------------
	 * check if we are really done
	 */
	if (! td->init || td->bail >= XVT_DIE_LOOPS)
	{
		/*----------------------------------------------------------------
		 * If running in "here" mode, we don't know if our caller will
		 * ever terminate, even we gave him "lots" of time.
		 * So, nail him forcibly.
		 */
		if (td->pid == td->our_pid)
		{
			sys_sig_send(td->pid, SIGKILL);
		}

		return (-1);
	}

	/*--------------------------------------------------------------------
	 * get fd of X display & input from child
	 */
	xfd = xvt_w_get_xfd(td->tw);
	cfd = td->pd->mst_fd;

	/*--------------------------------------------------------------------
	 * determine max # fds to look for
	 */
	if (xfd > cfd)
		max_fd = xfd + 1;
	else
		max_fd = cfd + 1;

	/*--------------------------------------------------------------------
	 * determine amount of time to wait
	 */
	if (td->bail > 0)
	{
		/*----------------------------------------------------------------
		 * If the child was told to die, we bump our count &
		 * give him time to die.
		 *
		 * Note that the "child" may be the calling process if we
		 * are running in "here" mode.
		 */
		td->bail++;
		usecs	= (td->die_interval * 1000);
	}
	else
	{
		/*----------------------------------------------------------------
		 * use caller's wait interval
		 */
		usecs	= ms * 1000;
	}

	/*--------------------------------------------------------------------
	 * check if we have to adjust our time interval
	 */
	secs	= usecs / 1000000;
	usecs	= usecs % 1000000;

	/*--------------------------------------------------------------------
	 * wait for something to happen
	 */
	while (TRUE)
	{
		/*----------------------------------------------------------------
		 * set file-descriptors to wait for
		 *
		 * We have to set this every time through the loop since select
		 * always clears it.
		 */
		FD_ZERO(&readfds);
		FD_SET(cfd, &readfds);
		FD_SET(xfd, &readfds);

		/*----------------------------------------------------------------
		 * set program wait interval
		 *
		 * We have to set this interval every time through the loop
		 * since Linux (unlike other Unixes) clears the timeval struct
		 * when select returns.
		 */
		tv.tv_sec	= secs;
		tv.tv_usec	= usecs;

		/*----------------------------------------------------------------
		 * now do the actual wait
		 */
		rc = select(max_fd, &readfds, 0, 0, &tv);
		if (rc < 0 && errno == EINTR)
			continue;
		break;
	}

	/*--------------------------------------------------------------------
	 * Process any X cmds if any.  We don't believe what select says, but
	 * we are hoping that at least any X events would have triggered the
	 * select.
	 */
	while (TRUE)
	{
		XVT_EVENT	xe;

		if (! xvt_w_event_get(td->tw, &xe))
			break;

		xvt_term_process_event(td, &xe);
	}

	/*--------------------------------------------------------------------
	 * If select returned with error, then bail.  Note we do this after
	 * flushing any lingering X events.
	 */
	if (rc < 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * Now break out if child died.  We do this after we processed any
	 * lingering X cmds the child issued before dying.
	 */
	if (td->pid == 0 || td->pid_died != 0)
		return (-1);

	/*----------------------------------------------------------------
	 * determine if we have any pending input
	 */
	input_pending	= FALSE;
	have_real		= FALSE;

	if (FD_ISSET(cfd, &readfds))
	{
		/*----------------------------------------------------------------
		 * data pending on PTY (note that we may not read it in this pass)
		 */
		input_pending	= TRUE;
		have_real		= TRUE;
	}
	else
	{
		/*----------------------------------------------------------------
		 * check input buffers
		 */
		PTY_IOBUF *	p = td->inp_current;

		if (p->bufpos < p->bufcnt)
		{
			/*------------------------------------------------------------
			 * data still pending in current input buffer
			 */
			input_pending = TRUE;
		}
		else
		{
			/*------------------------------------------------------------
			 * The current input buffer is empty, and since we always
			 * empty an input buffer before going on, we know that the
			 * *real* input buffer is empty.  So we only have to check if
			 * there are any pseudo-buffers to process.
			 */
			if (td->pty_inpbufs != 0)
			{
				if (p != td->pty_inpbufs || p->next != 0)
				{
					input_pending = TRUE;
				}
			}
		}
	}

	/*----------------------------------------------------------------
	 * process any program output if any
	 */
	if (input_pending)
	{
		/*------------------------------------------------------------
		 * turn off cursor while processing output
		 */
		xvt_term_set_txt_cursor(td, FALSE);

		/*--------------------------------------------------------
		 * process output
		 */
		rc = xvt_term_process_child_output(td, have_real);
		if (rc < 0)
		{
			/*----------------------------------------------------
			 * got EOF - start shutdown process
			 */
			if (td->pid == td->our_pid)
			{
				/*--------------------------------------------------------
				 * Running in "here" mode.  Immediately signal death the
				 * next time around.
				 */
				td->bail = XVT_DIE_LOOPS;
			}
			else
			{
				/*--------------------------------------------------------
				 * Running in "task" mode.  Start slowdown & wait for
				 * child to die.  We essentially give him XVT_DIE_LOOPS
				 * times "td->die_interval" (100) ms.  If he hasn't died
				 * by then, we just bail and terminate him forcibly.
				 */
				td->bail = 1;
			}
		}
		else if (rc == 0)
		{
			/*------------------------------------------------------------
			 * We read nothing, even though select() said we had something.
			 * Ignore what select() says & break out.
			 * But we do turn the cursor back on if needed.
			 */
			xvt_term_set_txt_cursor(td, TRUE);
		}
	}
	else
	{
		/*--------------------------------------------------------
		 * turn cursor back on if needed
		 */
		xvt_term_set_txt_cursor(td, TRUE);
	}

	/*--------------------------------------------------------------------
	 * finally, check if any output is pending
	 */
	xvt_term_write_flush(td);

	return (0);
}

/*------------------------------------------------------------------------
 * main loop
 *
 * Note that this routine may be run as a sub-thread.
 */
void * xvt_term_main_loop (void *data)
{
	TERMDATA *	td = (TERMDATA *)data;
	int			rc;

	/*--------------------------------------------------------------------
	 * we loop until told to bail
	 */
	while (TRUE)
	{
		rc = xvt_term_loop_once(td, td->run_interval);
		if (rc < 0)
			break;
	}

	/*--------------------------------------------------------------------
	 * If running as a thread, our returning will terminate the thread,
	 * so clear out our thread-id so we know we aren't running.
	 *
	 * If we are not running as a thread, this is already cleared.
	 */
	td->our_tid = 0;

	return (0);
}

/*------------------------------------------------------------------------
 * setup everything
 */
int xvt_term_setup_all (TERMDATA *td)
{
	/*--------------------------------------------------------------------
	 * do display setup
	 */
	td->tw = xvt_w_setup_all(td->xd);
	if (td->tw == 0)
		return (td->xd->error_num);

	/*--------------------------------------------------------------------
	 * setup display window color & font
	 */
	td->scrn_mode = SCRN_MODE_NONE;
	xvt_term_set_scrn_mode(td, SCRN_MODE_MONO);

	/*--------------------------------------------------------------------
	 * setup current font
	 */
	td->tw->cur_font_type = FONT_NONE;
	xvt_term_set_reg_font(td);

	/*--------------------------------------------------------------------
	 * setup text screen
	 */
	xvt_term_setup_text(td, td->tw->win_cols, td->tw->win_rows);

	/*--------------------------------------------------------------------
	 * set window & icon labels
	 */
	xvt_term_set_win_title(td, td->win_label);
	xvt_term_set_ico_title(td, td->ico_label);

	/*--------------------------------------------------------------------
	 * setup terminal emulator
	 */
	xvt_emul_set_tbl(td, td->xd->terminal);

	/*--------------------------------------------------------------------
	 * set DISPLAY var string if specified
	 */
	if (td->xd->display != 0 && *td->xd->display != 0)
	{
		sprintf(td->dsp_var_str, "DISPLAY=%s", td->xd->display);
	}

	/*--------------------------------------------------------------------
	 * set env var string if specified
	 */
	if (td->xd->envvar != 0 && *td->xd->envvar != 0)
	{
		sprintf(td->env_var_str, "%s=%s", td->xd->envvar, td->xd->term_type);
	}

	/*--------------------------------------------------------------------
	 * Set TERM variable string
	 */
	sprintf(td->trm_var_str, "TERM=%s",
		td->xd->termname != 0 ? td->xd->termname : xvt_emul_get_term(td));

	/*--------------------------------------------------------------------
	 * Set WINDOWID variable string
	 */
	sprintf(td->wid_var_str, "WINDOWID=%u", xvt_w_get_windowid(td->tw));

	return (0);
}

/*------------------------------------------------------------------------
 * close out everything
 */
void xvt_term_close_all (TERMDATA *td)
{
	xvt_pty_close(td->pd);
	xvt_term_close_text(td);
	xvt_w_close_all(td->tw);
	if (td->prt != 0)
		xvt_prt_close(td->prt);

	td->tw		= 0;
	td->pd		= 0;
	td->prt		= 0;
	td->init	= FALSE;
}

/*------------------------------------------------------------------------
 * shutdown prior to cloning
 */
void xvt_term_shutdown (TERMDATA *td)
{
	/*--------------------------------------------------------------------
	 * close the event sub-thread if running
	 *
	 * The man page for fork() states that, when using POSIX threads,
	 * fork() will only copy the main thread, not any sub-threads.
	 * But, on Solaris (at least), fork() appears to copy ALL threads.
	 * So, we have to terminate the sub-thread if one is running.
	 *
	 * Note: if the sub-thread is not running at this time, this will
	 * silently do nothing.
	 */
#if 0
	/*--------------------------------------------------------------------
	 * NOTE: this is commented out for now, since Linux apparently
	 * does not copy sub-threads in a fork(), and pthread_join() hangs
	 * when called with a bogus thread id.  For Solaris, we will just
	 * use fork1() for now, which does only copy the main thread.
	 */
	if (td->our_tid != 0)
	{
		td->bail = XVT_DIE_LOOPS;
		xvtthread_wait(td->our_tid);
		td->bail = 0;
	}
#endif

	/*--------------------------------------------------------------------
	 * Now we can close the pty.
	 */
	xvt_pty_close(td->pd);
	td->pd = 0;

	/*----------------------------------------------------------------
	 * Close the display.  All we do is close the connection, since
	 * calling XCloseDisplay() will free parent resources, which will
	 * hose our parent program.
	 *
	 * Note that we don't call any X routines to free anything in the
	 * X info struct (for the same reason), but we do free the struct itself.
	 *
	 * Note that we cache our current font number first.
	 */
	td->xd->fontno = td->tw->cur_font_no;
	xvt_w_close_con(td->tw);

	xvt_w_free(td->tw);
	td->tw = 0;

	/*----------------------------------------------------------------
	 * close any printer that is open, since we can't clone any piped
	 * processes.
	 */
	if (td->prt != 0)
	{
		xvt_prt_close(td->prt);
		td->prt = 0;
	}
}

/*------------------------------------------------------------------------
 * restart the display
 */
int xvt_term_restart (TERMDATA *td)
{
	/*--------------------------------------------------------------------
	 * Re-open a new display.
	 */
	td->tw = xvt_w_setup_all(td->xd);
	if (td->tw == 0)
	{
		xvt_term_close_all(td);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * Now open a new pty.
	 */
	td->pd = xvt_pty_init(0);
	if (td->pd == 0)
	{
		xvt_term_close_all(td);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * Setup display color to what we had before (remember that setup
	 * always sets it to mono).
	 */
	xvt_term_set_scrn_mode(td, td->scrn_mode);

	/*--------------------------------------------------------------------
	 * setup current font
	 */
	td->tw->cur_font_type = FONT_NONE;
	xvt_term_set_reg_font(td);

	/*--------------------------------------------------------------------
	 * set window & icon labels and the icon
	 */
	xvt_term_set_win_title(td, td->win_label);
	xvt_term_set_ico_title(td, td->ico_label);

	if (td->ico_width > 0 && td->ico_height > 0)
		xvt_w_set_icon(td->tw, td->ico_width, td->ico_height, td->ico_bits);

	return (0);
}

/*------------------------------------------------------------------------
 * present data as input to pty
 *
 * Note that "input to pty" is "output" for us
 */
int xvt_term_pty_input (TERMDATA *td, const void *buf, int len)
{
	PTY_IOBUF *	p_new;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (buf == 0 || len <= 0)
		return (XVT_OK);

	/*--------------------------------------------------------------------
	 * allocate new i/o buffer
	 *
	 * Note that the actual data buffer is located just past the struct.
	 */
	p_new = (PTY_IOBUF *)MALLOC(sizeof(*p_new) + len);
	if (p_new == 0)
	{
		sprintf(td->xd->error_msg, "no memory");
		td->xd->error_num = XVT_ERR_NOMEM;
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * copy data into it
	 */
	p_new->next		= 0;
	p_new->bufptr	= (unsigned char *)(p_new + 1);
	p_new->bufcnt	= len;
	p_new->buflen	= len;
	p_new->bufpos	= 0;
	p_new->bufheap	= TRUE;

	memcpy(p_new->bufptr, buf, len);

	/*--------------------------------------------------------------------
	 * now hook it into the chain
	 */
	sysmutex_enter(&td->pty_outmtx);
	{
		if (td->pty_outbufs == 0)
		{
			td->pty_outbufs = p_new;
		}
		else
		{
			PTY_IOBUF *	p;

			for (p=td->pty_outbufs; p->next; p=p->next)
				;
			p->next = p_new;
		}
	}
	sysmutex_leave(&td->pty_outmtx);

	return (XVT_OK);
}

/*------------------------------------------------------------------------
 * present data as output from pty
 *
 * Note that "output from pty" is "input" for us
 */
int xvt_term_pty_output (TERMDATA *td, const void *buf, int len)
{
	PTY_IOBUF *	p_new;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (buf == 0 || len <= 0)
		return (XVT_OK);

	/*--------------------------------------------------------------------
	 * allocate new i/o buffer
	 *
	 * Note that the actual data buffer is located just past the struct.
	 */
	p_new = (PTY_IOBUF *)MALLOC(sizeof(*p_new) + len);
	if (p_new == 0)
	{
		sprintf(td->xd->error_msg, "no memory");
		td->xd->error_num = XVT_ERR_NOMEM;
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * copy data into it
	 */
	p_new->next		= 0;
	p_new->bufptr	= (unsigned char *)(p_new + 1);
	p_new->bufcnt	= len;
	p_new->buflen	= len;
	p_new->bufpos	= 0;
	p_new->bufheap	= TRUE;

	memcpy(p_new->bufptr, buf, len);

	/*--------------------------------------------------------------------
	 * now hook it into the chain
	 */
	sysmutex_enter(&td->pty_inpmtx);
	{
		if (td->pty_inpbufs == 0)
		{
			td->pty_inpbufs = p_new;
		}
		else
		{
			PTY_IOBUF *	p;

			for (p=td->pty_inpbufs; p->next; p=p->next)
				;
			p->next = p_new;
		}
	}
	sysmutex_leave(&td->pty_inpmtx);

	return (XVT_OK);
}

/*------------------------------------------------------------------------
 * output list of cursor names
 */
void xvt_term_list_pointers (FILE *fp, int verbose)
{
	int	i;

	for (i=0; ; i++)
	{
		const char *name	= xvt_w_ptr_name(i);
		int			code	= xvt_w_ptr_code(i);

		if (name == 0)
			break;

		if (verbose)
			fprintf(fp, "%3d\t%s\n", code, name);
		else
			fprintf(fp, "%s\n", name);
	}
}
