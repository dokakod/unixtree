/*------------------------------------------------------------------------
 * current screen routines
 */
#include "xvtcommon.h"

/*------------------------------------------------------------------------
 * set current-screen margins
 */
void xvt_curscrn_set_vmargins (TERMDATA *td, int top, int bot)
{
	xvt_scrn_set_vmargins(CUR_SCRN(td), top, bot);
}

void xvt_curscrn_set_hmargins (TERMDATA *td, int left, int right)
{
	xvt_scrn_set_hmargins(CUR_SCRN(td), left, right);
}

/*------------------------------------------------------------------------
 * clear the current-screen
 */
void xvt_curscrn_clear (TERMDATA *td, int prot)
{
	xvt_scrn_clear(CUR_SCRN(td), prot);
	xvt_term_disp_fill(td, prot, 0, 0, CUR_SCRN_COLS(td), CUR_SCRN_ROWS(td));
}

/*------------------------------------------------------------------------
 * set the current-screen to all 'c'
 */
void xvt_curscrn_set (TERMDATA *td, int c)
{
	int x;
	int y;

	for (y=0; y<CUR_SCRN_ROWS(td); y++)
	{
		CELL *	cp	= CUR_SCRN_LINE(td, y);

		for (x=0; x<CUR_SCRN_COLS(td); x++)
		{
			cp->ch = (unsigned char)c;
			xvt_term_disp_char(td, x, y, cp->ch, cp->fg, cp->bg, cp->at);
			cp++;
		}
	}
}

/*------------------------------------------------------------------------
 * clear chars on a line
 *
 * mode:
 *	-1		clear from BOL to cursor
 *	 0		clear entire line
 *	 1		clear from cursor to EOL
 *
 * prot:
 *	FALSE	do all chars
 *	TRUE	do only non-protected chars
 */
void xvt_curscrn_chars_clr (TERMDATA *td, int mode, int prot)
{
	CELL *	cp	= CUR_SCRN_LINE(td, CUR_SCRN_Y(td));
	int		j;
	int		sx;
	int		sy;
	int		dx;
	int		dy;
	int		mx	= CUR_SCRN_COLS(td);

	switch (mode)
	{
	case 1:
		/*----------------------------------------------------------------
		 * clear to EOL
		 */
		sx = CUR_SCRN_X(td);
		sy = CUR_SCRN_Y(td);

		if (sx >= mx)
			sx = mx - 1;

		dx = mx - sx;
		dy = 1;

		xvt_term_disp_fill(td, prot, sx, sy, dx, dy);

		for (j=sx; j<mx; j++)
		{
			if (prot == 0 || (cp[j].at & prot) == 0)
			{
				cp[j].ch = ' ';
				cp[j].fg = CUR_SCRN_FG(td);
				cp[j].bg = CUR_SCRN_BG(td);
				cp[j].at = CUR_SCRN_AT(td);
			}
		}
		break;

	case -1:
		/*----------------------------------------------------------------
		 * clear from BOL
		 */
		sx = 0;
		dx = CUR_SCRN_X(td) + 1;
		sy = CUR_SCRN_Y(td);
		dy = 1;

		if (dx > mx)
			dx = mx;

		xvt_term_disp_fill(td, prot, sx, sy, dx, dy);

		for (j=0; j<dx; j++)
		{
			if (prot == 0 || (cp[j].at & prot) == 0)
			{
				cp[j].ch = ' ';
				cp[j].fg = CUR_SCRN_FG(td);
				cp[j].bg = CUR_SCRN_BG(td);
				cp[j].at = CUR_SCRN_AT(td);
			}
		}
		break;

	case 0:
		/*----------------------------------------------------------------
		 * clear entire line
		 */
		xvt_curscrn_lines_clr(td, CUR_SCRN_Y(td), 1, prot);
		break;
	}
}

/*------------------------------------------------------------------------
 * erase n chars from current point
 */
void xvt_curscrn_chars_era (TERMDATA *td, int n, int prot)
{
	int		sx	= CUR_SCRN_X(td);
	int		sy	= CUR_SCRN_Y(td);
	int		dx	= n;
	int		dy	= 1;
	CELL *	cp;

	if (sx >= CUR_SCRN_COLS(td))
		sx = CUR_SCRN_COLS(td) - 1;

	cp	= CUR_SCRN_CHAR(td, sy, sx);

	if ((sx + dx) > CUR_SCRN_COLS(td))
		dx = CUR_SCRN_COLS(td) - sx + 1;

	xvt_term_disp_fill(td, prot, sx, sy, dx, dy);

	for (; sx <= dx; sx++)
	{
		if (prot == 0 || (cp->at & prot) == 0)
		{
			cp->ch = ' ';
			cp->fg = CUR_SCRN_FG(td);
			cp->bg = CUR_SCRN_BG(td);
			cp->at = CUR_SCRN_AT(td);
		}
		cp++;
	}
}

/*------------------------------------------------------------------------
 * delete n chars from current point
 */
void xvt_curscrn_chars_del (TERMDATA *td, int n)
{
	int	x	= CUR_SCRN_X(td);
	int	y	= CUR_SCRN_Y(td);
	int	mx	= CUR_SCRN_COLS(td);

	if (x >= mx)
		x = mx - 1;

	if (n == 0)
		n = 1;

	if (n > (mx - x) + 1)
		n = (mx - x) + 1;

	{
		CELL *	line = CUR_SCRN_LINE(td, y);
		int		dx;

		/*----------------------------------------------------------------
		 * move line over n chars
		 */
		for (dx = x; dx < mx - n; dx++)
		{
			line[dx].ch = line[dx+n].ch;
			line[dx].fg = line[dx+n].fg;
			line[dx].bg = line[dx+n].bg;
			line[dx].at = line[dx+n].at;
		}

		/*----------------------------------------------------------------
		 * set last n chars on line to blank
		 */
		for (; dx < mx; dx++)
		{
			line[dx].ch = ' ';
			line[dx].fg = CUR_SCRN_FG(td);
			line[dx].bg = CUR_SCRN_BG(td);
			line[dx].at = CUR_SCRN_AT(td);
		}

		/*----------------------------------------------------------------
		 * now redisplay all changed chars
		 */
		for (dx=x; dx<mx; dx++)
		{
			xvt_term_disp_char(td, dx, y,
				line[dx].ch,
				line[dx].fg,
				line[dx].bg,
				line[dx].at);
		}
	}
}

/*------------------------------------------------------------------------
 * insert n chars at current point
 */
void xvt_curscrn_chars_ins (TERMDATA *td, int n)
{
	int	x	= CUR_SCRN_X(td);
	int y	= CUR_SCRN_Y(td);
	int	mx	= CUR_SCRN_COLS(td);

	if (x >= mx)
		x = mx - 1;

	if (n == 0)
		n = 1;

	if (n > mx - x)
		n = mx - x;

	{
		CELL *	line = CUR_SCRN_LINE(td, y);
		int		dx;

		/*----------------------------------------------------------------
		 * copy line over n chars
		 */
		for (dx = mx - n - 1; dx >= x; dx--)
		{
			line[dx+n].ch = line[dx].ch;
			line[dx+n].fg = line[dx].fg;
			line[dx+n].bg = line[dx].bg;
			line[dx+n].at = line[dx].at;
		}

		/*----------------------------------------------------------------
		 * blank out n chars
		 */
		for (dx = x + n - 1; dx >= x; dx--)
		{
			line[dx].ch = ' ';
			line[dx].fg = CUR_SCRN_FG(td);
			line[dx].bg = CUR_SCRN_BG(td);
			line[dx].at = CUR_SCRN_AT(td);
		}

		/*----------------------------------------------------------------
		 * now display all changed chars
		 */
		for (dx=x; dx<mx; dx++)
		{
			xvt_term_disp_char(td, dx, y,
				line[dx].ch,
				line[dx].fg,
				line[dx].bg,
				line[dx].at);
		}
	}
}

/*------------------------------------------------------------------------
 * clear n lines in the current-screen
 */
void xvt_curscrn_lines_clr (TERMDATA *td, int y, int n, int prot)
{
	xvt_scrn_clear_lines(CUR_SCRN(td), y, n, prot);
	xvt_term_disp_fill(td, prot, 0, y, CUR_SCRN_COLS(td), n);
}

/*------------------------------------------------------------------------
 * copy n lines in the current-screen
 */
void xvt_curscrn_lines_cpy (TERMDATA *td, int yt, int yf, int n)
{
	xvt_scrn_copy_lines(CUR_SCRN(td), yt, yf, n);
	xvt_term_disp_copy(td,
		0, yt,
		CUR_SCRN_COLS(td), n,
		0, yf);
}

/*------------------------------------------------------------------------
 * delete n lines from current line
 */
void xvt_curscrn_lines_del (TERMDATA *td, int n)
{
	int	top		= CUR_SCRN_TOP(td);
	int	bot		= CUR_SCRN_BOT(td);
	int cur;
	int	range;
	int length;

	/*--------------------------------------------------------------------
	 * reduce scroll region if a lock affects it
	 */
	if (top < CUR_SCRN_LOCK(td))
		top = CUR_SCRN_LOCK(td);

	cur		= CUR_SCRN_Y(td) - top;
	range	= (bot - top) + 1;
	length	= (range - cur);

	/*--------------------------------------------------------------------
	 * no delete if out of margin-box
	 */
	if (CUR_SCRN_Y(td) < top || CUR_SCRN_Y(td) > bot)
		return;

	/*--------------------------------------------------------------------
	 * ignore negative & zero requests
	 */
	if (n <= 0)
		return;

	/*--------------------------------------------------------------------
	 * if n > lines in box below current line, just clear it
	 */
	if (n >= length)
	{
		xvt_curscrn_lines_clr(td, CUR_SCRN_Y(td), length, 0);
		return;
	}

	/*--------------------------------------------------------------------
	 * copy the lines up & clear the vacated lines
	 */
	if (length > n)
	{
		xvt_curscrn_lines_cpy(td, CUR_SCRN_Y(td), CUR_SCRN_Y(td) + n,
			length - n);
	}
	xvt_curscrn_lines_clr(td, bot - n + 1, n, 0);
}

/*------------------------------------------------------------------------
 * insert n lines at current line
 */
void xvt_curscrn_lines_ins (TERMDATA *td, int n)
{
	int	top		= CUR_SCRN_TOP(td);
	int	bot		= CUR_SCRN_BOT(td);
	int cur;
	int	range;
	int length;

	/*--------------------------------------------------------------------
	 * reduce scroll region if a lock affects it
	 */
	if (top < CUR_SCRN_LOCK(td))
		top = CUR_SCRN_LOCK(td);

	cur		= CUR_SCRN_Y(td) - top;
	range	= (bot - top) + 1;
	length	= (range - cur);

	/*--------------------------------------------------------------------
	 * no insert if out of margin-box
	 */
	if (CUR_SCRN_Y(td) < top || CUR_SCRN_Y(td) > bot)
		return;

	/*--------------------------------------------------------------------
	 * ignore negative & zero requests
	 */
	if (n <= 0)
		return;

	/*--------------------------------------------------------------------
	 * copy the lines up & clear the vacated lines
	 */
	if (CUR_SCRN_Y(td) == bot)
	{
		xvt_curscrn_lines_cpy(td, top, top+1, range - 1);
		xvt_curscrn_lines_clr(td, CUR_SCRN_Y(td), 1, 0);
	}
	else
	{
		/*----------------------------------------------------------------
		 * if n > lines in box above current line, just clear it
		 */
		if (n >= length)
		{
			xvt_curscrn_lines_clr(td, CUR_SCRN_Y(td), length, 0);
		}
		else
		{
			if (length > n)
			{
				xvt_curscrn_lines_cpy(td, CUR_SCRN_Y(td) + n, CUR_SCRN_Y(td),
					length - n);
			}
			xvt_curscrn_lines_clr(td, CUR_SCRN_Y(td), n, 0);
		}
	}
}

/*------------------------------------------------------------------------
 * insert columns
 */
void xvt_curscrn_cols_ins (TERMDATA *td, int n)
{
	int sx	= CUR_SCRN_X(td);
	int	sy	= CUR_SCRN_Y(td);
	int	x	= sx;
	int y	= 0;

	/*--------------------------------------------------------------------
	 * check if at right edge of screen
	 */
	if (x >= CUR_SCRN_COLS(td))
		x = CUR_SCRN_COLS(td) - 1;

	/*--------------------------------------------------------------------
	 * check memory lock
	 */
	if (y < CUR_SCRN_LOCK(td))
		y = CUR_SCRN_LOCK(td) + 1;

	/*--------------------------------------------------------------------
	 * ignore negative & zero requests
	 */
	if (n <= 0)
		return;

	if (n > (CUR_SCRN_COLS(td) - x))
		n = CUR_SCRN_COLS(td) - x;

	/*--------------------------------------------------------------------
	 * now do it
	 */
	for (; y < CUR_SCRN_ROWS(td); y++)
	{
		CUR_SCRN_X(td) = x;
		CUR_SCRN_Y(td) = y;

		xvt_curscrn_chars_ins(td, n);
	}

	/*--------------------------------------------------------------------
	 * restore original cursor position
	 */
	CUR_SCRN_X(td) = sx;
	CUR_SCRN_Y(td) = sy;
}

/*------------------------------------------------------------------------
 * delete columns
 */
void xvt_curscrn_cols_del (TERMDATA *td, int n)
{
	int sx	= CUR_SCRN_X(td);
	int	sy	= CUR_SCRN_Y(td);
	int	x	= sx;
	int y	= 0;

	/*--------------------------------------------------------------------
	 * check if at right edge of screen
	 */
	if (x >= CUR_SCRN_COLS(td))
		x = CUR_SCRN_COLS(td) - 1;

	/*--------------------------------------------------------------------
	 * check memory lock
	 */
	if (y < CUR_SCRN_LOCK(td))
		y = CUR_SCRN_LOCK(td) + 1;

	/*--------------------------------------------------------------------
	 * ignore negative & zero requests
	 */
	if (n <= 0)
		return;

	if (n > (CUR_SCRN_COLS(td) - x))
		n = CUR_SCRN_COLS(td) - x;

	/*--------------------------------------------------------------------
	 * now do it
	 */
	for (; y < CUR_SCRN_ROWS(td); y++)
	{
		CUR_SCRN_X(td) = x;
		CUR_SCRN_Y(td) = y;

		xvt_curscrn_chars_del(td, n);
	}

	/*--------------------------------------------------------------------
	 * restore original cursor position
	 */
	CUR_SCRN_X(td) = sx;
	CUR_SCRN_Y(td) = sy;
}

/*------------------------------------------------------------------------
 * scroll screen n lines
 */
void xvt_curscrn_scroll (TERMDATA *td, int n)
{
	int save_y	= CUR_SCRN_Y(td);

	if (n > 0)
	{
		/*----------------------------------------------------------------
		 * scroll down n lines
		 */
		int top = CUR_SCRN_TOP(td);
		if (top < CUR_SCRN_LOCK(td))
			top = CUR_SCRN_LOCK(td);

		CUR_SCRN_Y(td) = top;
		xvt_curscrn_lines_ins(td, n);
	}
	else if (n < 0)
	{
		/*----------------------------------------------------------------
		 * scroll up n lines
		 */
		CUR_SCRN_Y(td) = CUR_SCRN_BOT(td);
		xvt_curscrn_lines_ins(td, -n);
	}

	CUR_SCRN_Y(td) = save_y;
}
