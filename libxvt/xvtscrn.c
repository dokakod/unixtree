/*------------------------------------------------------------------------
 * TERMSCRN functions
 */
#include "xvtcommon.h"

/*------------------------------------------------------------------------
 * init a TERMSCRN
 */
TERMSCRN * xvt_scrn_init (int rows, int cols, int tabs,
	int mode, int fg, int bg, int at)
{
	TERMSCRN *	s;
	int			nchars	= (rows * cols);
	int			size	= (sizeof(*s) + (nchars * sizeof(CELL)) + cols);

	s = (TERMSCRN *)MALLOC(size);
	if (s != 0)
	{
		SCRN_DISP(s)		= (CELL *)(s + 1);
		SCRN_COLS(s)		= cols;
		SCRN_ROWS(s)		= rows;
		SCRN_TABS(s)		= tabs;

		SCRN_TOP(s)			= 0;
		SCRN_BOT(s)			= SCRN_ROWS(s) - 1;
		SCRN_LEFT(s)		= 0;
		SCRN_RIGHT(s)		= SCRN_COLS(s) - 1;

		SCRN_TABSTOPS(s)	= (unsigned char *)(SCRN_DISP(s) + nchars);
		SCRN_MODE(s)		= mode;
		SCRN_FG(s)			= fg;
		SCRN_BG(s)			= bg;
		SCRN_AT(s)			= at;

		SCRN_GCS(s, CHAR_SET_G0)	= GCS_US;
		SCRN_GCS(s, CHAR_SET_G1)	= GCS_US;
		SCRN_GCS(s, CHAR_SET_G2)	= GCS_US;
		SCRN_GCS(s, CHAR_SET_G3)	= GCS_US;

		SCRN_CS(s)			= CHAR_SET_G0;
		SCRN_TMPCS(s)		= CHAR_SET_NONE;

		SCRN_X(s)			= 0;
		SCRN_Y(s)			= 0;

		SCRN_SAVE_MODE(s)	= SCRN_MODE(s);
		SCRN_SAVE_FG(s)		= SCRN_FG(s);
		SCRN_SAVE_BG(s)		= SCRN_BG(s);
		SCRN_SAVE_AT(s)		= SCRN_AT(s);
		SCRN_SAVE_GCS(s, CHAR_SET_G0)	= SCRN_GCS(s, CHAR_SET_G0);
		SCRN_SAVE_GCS(s, CHAR_SET_G1)	= SCRN_GCS(s, CHAR_SET_G1);
		SCRN_SAVE_GCS(s, CHAR_SET_G2)	= SCRN_GCS(s, CHAR_SET_G2);
		SCRN_SAVE_GCS(s, CHAR_SET_G3)	= SCRN_GCS(s, CHAR_SET_G3);

		SCRN_SAVE_CS(s)		= SCRN_CS(s);
		SCRN_SAVE_X(s)		= SCRN_X(s);
		SCRN_SAVE_Y(s)		= SCRN_Y(s);

		SCRN_INS(s)			= FALSE;
		SCRN_ORG(s)			= FALSE;
		SCRN_LOCK(s)		= -1;

		SCRN_CSV(s)			= TRUE;
		SCRN_CON(s)			= FALSE;
	}

	return (s);
}

/*------------------------------------------------------------------------
 * free a TERMSCRN
 */
TERMSCRN * xvt_scrn_free (TERMSCRN *s)
{
	if (s != 0)
	{
		FREE(s);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * reset a screen to all defaults
 */
void xvt_scrn_reset (TERMSCRN *s)
{
	if (s != 0)
	{
		SCRN_TOP(s)			= 0;
		SCRN_BOT(s)			= SCRN_ROWS(s) - 1;
		SCRN_LEFT(s)		= 0;
		SCRN_RIGHT(s)		= SCRN_COLS(s) - 1;

		SCRN_MODE(s)		= SCRN_MODE_MONO;
		SCRN_FG(s)			= XVT_CLR_FG;
		SCRN_BG(s)			= XVT_CLR_BG;
		SCRN_AT(s)			= A_NORMAL;

		SCRN_GCS(s, CHAR_SET_G0)	= GCS_US;
		SCRN_GCS(s, CHAR_SET_G1)	= GCS_US;
		SCRN_GCS(s, CHAR_SET_G2)	= GCS_US;
		SCRN_GCS(s, CHAR_SET_G3)	= GCS_US;

		SCRN_CS(s)			= CHAR_SET_G0;
		SCRN_TMPCS(s)		= CHAR_SET_NONE;

		SCRN_X(s)			= 0;
		SCRN_Y(s)			= 0;

		SCRN_SAVE_MODE(s)	= SCRN_MODE(s);
		SCRN_SAVE_FG(s)		= SCRN_FG(s);
		SCRN_SAVE_BG(s)		= SCRN_BG(s);
		SCRN_SAVE_AT(s)		= SCRN_AT(s);
		SCRN_SAVE_GCS(s, CHAR_SET_G0)	= SCRN_GCS(s, CHAR_SET_G0);
		SCRN_SAVE_GCS(s, CHAR_SET_G1)	= SCRN_GCS(s, CHAR_SET_G1);
		SCRN_SAVE_GCS(s, CHAR_SET_G2)	= SCRN_GCS(s, CHAR_SET_G2);
		SCRN_SAVE_GCS(s, CHAR_SET_G3)	= SCRN_GCS(s, CHAR_SET_G3);
		SCRN_SAVE_CS(s)		= SCRN_CS(s);
		SCRN_SAVE_X(s)		= SCRN_X(s);
		SCRN_SAVE_Y(s)		= SCRN_Y(s);

		SCRN_INS(s)			= FALSE;
		SCRN_ORG(s)			= FALSE;
		SCRN_LOCK(s)		= -1;

		SCRN_CSV(s)			= TRUE;
		SCRN_CON(s)			= FALSE;

		xvt_scrn_clear_lines(s, 0, SCRN_ROWS(s), 0);
		xvt_scrn_tab_set(s, SCRN_TAB_SET_ALL);
	}
}

/*------------------------------------------------------------------------
 * set screen margins
 */
void xvt_scrn_set_vmargins (TERMSCRN *s, int top, int bot)
{
	int rows	= SCRN_ROWS(s);

	if (top  < 0)		top = 0;
	if (top >= rows)	top = rows - 1;

	if (bot  < 0)		bot = 0;
	if (bot >= rows)	bot = rows - 1;

	if (top == 0 && bot == 0)
		bot = rows - 1;

	if (top < bot)
	{
		SCRN_TOP(s)	= top;
		SCRN_BOT(s)	= bot;
	}
}

void xvt_scrn_set_hmargins (TERMSCRN *s, int left, int right)
{
	int cols	= SCRN_COLS(s);

	if (left  < 0)		left = 0;
	if (left >= cols)	left = cols - 1;

	if (right  < 0)		right = 0;
	if (right >= cols)	right = cols - 1;

	if (left == 0 && right == 0)
		right = cols - 1;

	if (left < right)
	{
		SCRN_LEFT(s)	= left;
		SCRN_RIGHT(s)	= right;
	}
}

/*------------------------------------------------------------------------
 * clear a TERMSCRN
 */
void xvt_scrn_clear (TERMSCRN *s, int prot)
{
	xvt_scrn_clear_lines(s, 0, SCRN_ROWS(s), prot);
}

/*------------------------------------------------------------------------
 * copy a TERMSCRN to another
 */
void xvt_scrn_copy (TERMSCRN *t, const TERMSCRN *s)
{
	int	y;
	int	x;

	/*--------------------------------------------------------------------
	 * if old screen doesn't exist, just clear the screen & set tabs
	 */
	if (s == 0)
	{
		xvt_scrn_clear(t, 0);
		xvt_scrn_tab_set(t, SCRN_TAB_SET_ALL);
		return;
	}

	/*--------------------------------------------------------------------
	 * copy over each line at a time
	 */
	for (y = 0; y < SCRN_ROWS(t); y++)
	{
		CELL *	cp;
		CELL *	op;

		/*----------------------------------------------------------------
		 * point to target screen line
		 */
		cp = SCRN_LINE(t, y);

		/*----------------------------------------------------------------
		 * If this line is in the old screen, copy the data.
		 * Otherwise, just blank it out.
		 */
		if (y < SCRN_ROWS(s))
		{
			op = SCRN_LINE(s, y);

			/*------------------------------------------------------------
			 * only copy part of line which fits
			 */
			for (x = 0; x < SCRN_COLS(t) && x < SCRN_COLS(s); x++)
			{
				cp->ch = op->ch;
				cp->fg = op->fg;
				cp->bg = op->bg;
				cp->at = op->at;

				cp++;
				op++;
			}

			/*------------------------------------------------------------
			 * blank out rest of line (with current attr)
			 */
			for (; x < SCRN_COLS(t); x++)
			{
				cp->ch = ' ';
				cp->fg = SCRN_FG(s);
				cp->bg = SCRN_BG(s);
				cp->at = SCRN_AT(s);

				cp++;
			}
		}
		else
		{
			/*------------------------------------------------------------
			 * blank out entire line (with current attr)
			 */
			for (x = 0; x < SCRN_COLS(t); x++)
			{
				cp->ch = ' ';
				cp->fg = SCRN_FG(s);
				cp->bg = SCRN_BG(s);
				cp->at = SCRN_AT(s);

				cp++;
			}
		}
	}

	/*--------------------------------------------------------------------
	 * now copy over the tab array
	 */
	for (x=0; x < SCRN_COLS(t); x++)
	{
		if (x < SCRN_COLS(s))
		{
			SCRN_TABSTOP(t, x) = SCRN_TABSTOP(s, x);
		}
		else
		{
			if (SCRN_TABS(s) > 0 && (x % SCRN_TABS(s)) == 0)
				SCRN_TABSTOP(t, x) = 1;
			else
				SCRN_TABSTOP(t, x) = 0;
		}
	}

	/*--------------------------------------------------------------------
	 * now copy old attribute info
	 */
	SCRN_GCS(t, CHAR_SET_G0)	= SCRN_GCS(s, CHAR_SET_G0);
	SCRN_GCS(t, CHAR_SET_G1)	= SCRN_GCS(s, CHAR_SET_G1);
	SCRN_GCS(t, CHAR_SET_G2)	= SCRN_GCS(s, CHAR_SET_G2);
	SCRN_GCS(t, CHAR_SET_G3)	= SCRN_GCS(s, CHAR_SET_G3);

	SCRN_CS(t)		= SCRN_CS(s);
	SCRN_TMPCS(t)	= SCRN_TMPCS(s);

	SCRN_X(t)		= SCRN_X(s);
	if (SCRN_X(t) >= SCRN_COLS(t))
		SCRN_X(t) = SCRN_COLS(t) - 1;

	SCRN_Y(t)		= SCRN_Y(s);
	if (SCRN_Y(t) >= SCRN_ROWS(t))
		SCRN_Y(t) = SCRN_ROWS(t) - 1;

	/*--------------------------------------------------------------------
	 * Copy over saved info
	 */
	SCRN_SAVE_MODE(t)	= SCRN_SAVE_MODE(s);
	SCRN_SAVE_FG(t)		= SCRN_SAVE_FG(s);
	SCRN_SAVE_BG(t)		= SCRN_SAVE_BG(s);
	SCRN_SAVE_AT(t)		= SCRN_SAVE_AT(s);

	SCRN_SAVE_GCS(t, CHAR_SET_G0)	= SCRN_SAVE_GCS(s, CHAR_SET_G0);
	SCRN_SAVE_GCS(t, CHAR_SET_G1)	= SCRN_SAVE_GCS(s, CHAR_SET_G1);
	SCRN_SAVE_GCS(t, CHAR_SET_G2)	= SCRN_SAVE_GCS(s, CHAR_SET_G2);
	SCRN_SAVE_GCS(t, CHAR_SET_G3)	= SCRN_SAVE_GCS(s, CHAR_SET_G3);

	SCRN_SAVE_CS(t)		= SCRN_SAVE_CS(s);

	SCRN_SAVE_X(t)		= SCRN_SAVE_X(s);
	if (SCRN_SAVE_X(t) >= SCRN_COLS(t))
		SCRN_SAVE_X(t) = SCRN_COLS(t) - 1;

	SCRN_SAVE_Y(t)		= SCRN_SAVE_Y(s);
	if (SCRN_SAVE_Y(t) >= SCRN_ROWS(t))
		SCRN_SAVE_Y(t) = SCRN_ROWS(t) - 1;

	/*--------------------------------------------------------------------
	 * now copy over scrolling regions
	 */
	if (SCRN_TOP(s) > 0 || SCRN_BOT(s) < (SCRN_ROWS(s) - 1))
	{
		xvt_scrn_set_vmargins(t, SCRN_TOP(s), SCRN_BOT(s));
	}

	if (SCRN_LEFT(s) > 0 || SCRN_RIGHT(s) < (SCRN_COLS(s) - 1))
	{
		xvt_scrn_set_hmargins(t, SCRN_LEFT(s), SCRN_RIGHT(s));
	}

	/*--------------------------------------------------------------------
	 * now copy over other misc info
	 */
	SCRN_INS(t)		= SCRN_INS(s);
	SCRN_ORG(t)		= SCRN_ORG(s);

	SCRN_LOCK(t)	= SCRN_LOCK(s);
	if (SCRN_LOCK(t) >= 0 && SCRN_LOCK(t) >= SCRN_ROWS(t))
		SCRN_LOCK(t)	= SCRN_ROWS(t) - 1;

	SCRN_CSV(t)		= SCRN_CSV(s);
	SCRN_CON(t)		= SCRN_CON(s);
}

/*------------------------------------------------------------------------
 * copy n TERMSCRN lines to another line
 */
void xvt_scrn_copy_lines (TERMSCRN *s, int yt, int yf, int n)
{
	int	rows	= SCRN_ROWS(s);
	int	cols	= SCRN_COLS(s);

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (n <= 0)
		n = 1;

	if (yt  < 0)	yt = 0;
	if (yt >= rows)	yt = rows - 1;

	if (yf  < 0)	yf = 0;
	if (yf >= rows)	yf = rows - 1;

	if (yf == yt)
		return;

	/*--------------------------------------------------------------------
	 * check if source range out-of-bounds
	 */
	if ((yf + n) > rows)
		n = rows - yf;

	/*--------------------------------------------------------------------
	 * check if target range out-of-bounds
	 */
	if ((yt + n) > rows)
		n = rows - yt;

	/*--------------------------------------------------------------------
	 * check if target range overlaps the source range
	 */
	if ((yt + n) >= yf || yt <= (yf + n))
	{
		int i;

		/*----------------------------------------------------------------
		 * regions overlap, do a line-by line copy in the proper direction
		 */
		if (yf > yt)
		{
			/*------------------------------------------------------------
			 * source is later - do a first-to-last copy
			 */
			for (i=0; i < n; i++)
			{
				memcpy(SCRN_LINE(s, yt + i), SCRN_LINE(s, yf + i),
					(cols * sizeof(CELL)));
			}
		}
		else
		{
			/*------------------------------------------------------------
			 * source is earlier - do a last-to-first copy
			 */
			for (i=n-1; i >= 0; i--)
			{
				memcpy(SCRN_LINE(s, yt + i), SCRN_LINE(s, yf + i),
					(cols * sizeof(CELL)));
			}
		}
	}
	else
	{
		/*----------------------------------------------------------------
		 * no overlap of regions, just blast a memcpy()
		 */
		memcpy(SCRN_LINE(s, yt), SCRN_LINE(s, yf), n * (cols * sizeof(CELL)));
	}
}

/*------------------------------------------------------------------------
 * clear n TERMSCRN lines
 */
void xvt_scrn_clear_lines (TERMSCRN *s, int y, int n, int prot)
{
	int	rows	= SCRN_ROWS(s);
	int	cols	= SCRN_COLS(s);
	int	i;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (y  < 0)				y = 0;
	if (y >= rows)			y = rows - 1;

	if (n <= 0)				n = 1;
	if (n  > (y + rows))	n = rows - y;

	if (n > (cols - y) + 1)	n = (cols - y) + 1;

	/*--------------------------------------------------------------------
	 * now clear it line-by-line
	 */
	for (i=0; i<n; i++)
	{
		CELL *	cp = SCRN_LINE(s, y + i);
		int		j;

		for (j=0; j < cols; j++)
		{
			if (prot == 0 || (cp->at & prot) == 0)
			{
				cp->ch = ' ';
				cp->fg = SCRN_FG(s);
				cp->bg = SCRN_BG(s);
				cp->at = SCRN_AT(s);
			}
			cp++;
		}
	}
}

/*------------------------------------------------------------------------
 * tab functions
 */
void xvt_scrn_tab_set (TERMSCRN *s, int mode)
{
	switch (mode)
	{
	case SCRN_TAB_SET_CUR:
		SCRN_TABSTOP(s, SCRN_X(s)) = 1;
		break;

	case SCRN_TAB_SET_ALL:
		{
			int x;

			for (x=0; x<SCRN_COLS(s); x++)
			{
				if (SCRN_TABS(s) > 0 && (x % SCRN_TABS(s)) == 0)
					SCRN_TABSTOP(s, x) = 1;
				else
					SCRN_TABSTOP(s, x) = 0;
			}
		}
		break;

	case SCRN_TAB_CLR_CUR:
		SCRN_TABSTOP(s, SCRN_X(s)) = 0;
		break;

	case SCRN_TAB_CLR_ALL:
		memset(SCRN_TABSTOPS(s), 0, SCRN_COLS(s));
		break;
	}
}

/*------------------------------------------------------------------------
 * print a screen
 */
static const unsigned char xvt_scrn_spec_chars[] =
{
	' ',  '#',  '.',  ' ',  ' ',  ' ',  ' ',  ' ',
	' ',  ' ',  ' ',  '+',  '+',  '+',  '+',  '+',
	'-',  '-',  '-',  '-',  '-',  '+',  '+',  '+',
	'+',  '|',  ' ',  ' ',  ' ',  ' ',  ' ',  ' '
};

void xvt_scrn_print_line (TERMSCRN *s, PRINTER *p, int y)
{
	char	line[BUFSIZ];
	char *	lp = line;
	char *	ep = line;
	CELL *	cp = SCRN_LINE(s, y);
	int		x;

	for (x=0; x < SCRN_COLS(s); x++)
	{
		int ch = (cp++)->ch;

		if (ch < 0x20)
			ch = xvt_scrn_spec_chars[ch];

		*lp++ = ch;
		if (ch != ' ')
			ep = lp;
	}
	*ep++ = '\r';
	*ep++ = '\n';
	*ep   = 0;

	xvt_prt_output_str(p, line);
	xvt_prt_flush(p);
}

void xvt_scrn_print (TERMSCRN *s, PRINTER *p, int top, int bot,
	int left, int right, int ff)
{
	int	y;

	if (top   <= 0)				top		= 0;
	if (top   >= SCRN_ROWS(s))	top		= SCRN_ROWS(s) - 1;

	if (bot   <= 0)				bot		= SCRN_ROWS(s) - 1;
	if (bot   >= SCRN_ROWS(s))	bot		= SCRN_ROWS(s) - 1;

	if (left  <= 0)				left	= 0;
	if (left  >= SCRN_COLS(s))	left	= SCRN_COLS(s) - 1;

	if (right <= 0)				right	= SCRN_COLS(s) - 1;
	if (right >= SCRN_COLS(s))	right	= SCRN_COLS(s) - 1;

	for (y=top; y <= bot; y++)
	{
		xvt_scrn_print_line(s, p, y);
	}

	if (ff)
		xvt_prt_output_chr(p, '\f');
}
