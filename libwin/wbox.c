/*------------------------------------------------------------------------
 * border/box routines
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * box char table
 */
const unsigned char box_chrs[6][11] =
{
/*	B_BL B_BC B_BR B_ML B_MC B_MR B_TL B_TC B_TR B_VE B_HO					*/
 {	 '+', '+', '+', '+', '+', '+', '+', '+', '+', '|', '-' }, /* 0  B_ASCII	*/
 {	0xc0,0xc1,0xd9,0xc3,0xc5,0xb4,0xda,0xc2,0xbf,0xb3,0xc4 }, /* 1  B_SHSV	*/
 {	0xc8,0xca,0xbc,0xcc,0xce,0xb9,0xc9,0xcb,0xbb,0xba,0xcd }, /* 2  B_DHDV	*/
 {	0xd4,0xcf,0xbe,0xc6,0xd8,0xb5,0xd5,0xd1,0xb8,0xb3,0xcd }, /* 3  B_DHSV	*/
 {	0xd3,0xd0,0xbd,0xc7,0xd7,0xb6,0xd6,0xd2,0xb7,0xba,0xc4 }, /* 4  B_SHDV	*/
 {	 ' ',0xdc, ' ',0xdd,0xdb,0xde, ' ',0xdf, ' ', ' ', ' ' }  /* 5  B_BLOCK	*/
};

/*------------------------------------------------------------------------
 * translation arrays for box-char to new box-char
 */
const unsigned char
/*          B_BL  B_BC  B_BR  B_ML  B_MC  B_MR  B_TL  B_TC  B_TR  B_VE  B_HO */
bl_chrs[]={ B_BL, B_BC, B_BC, B_ML, B_MC, B_MC, B_ML, B_MC, B_MC, B_ML, B_BC },
bc_chrs[]={ B_BC, B_BC, B_BC, B_MC, B_MC, B_MC, B_MC, B_MC, B_MC, B_MC, B_BC },
br_chrs[]={ B_BC, B_BC, B_BR, B_MC, B_MC, B_MR, B_MC, B_MC, B_MR, B_MR, B_BC },
ml_chrs[]={ B_ML, B_MC, B_MC, B_ML, B_MC, B_MC, B_ML, B_MC, B_MC, B_ML, B_MC },
mc_chrs[]={ B_MC, B_MC, B_MC, B_MC, B_MC, B_MC, B_MC, B_MC, B_MC, B_MC, B_MC },
mr_chrs[]={ B_MC, B_MC, B_MR, B_MC, B_MC, B_MR, B_MC, B_MC, B_MR, B_MR, B_MC },
tl_chrs[]={ B_ML, B_MC, B_MC, B_ML, B_MC, B_MC, B_TL, B_TC, B_TC, B_ML, B_TC },
tc_chrs[]={ B_MC, B_MC, B_MC, B_MC, B_MC, B_MC, B_TC, B_TC, B_TC, B_MC, B_TC },
tr_chrs[]={ B_MC, B_MC, B_MR, B_MC, B_MC, B_MR, B_TC, B_TC, B_TR, B_MR, B_TC },
ve_chrs[]={ B_ML, B_MC, B_MR, B_ML, B_MC, B_MR, B_ML, B_MC, B_MR, B_VE, B_MC },
ho_chrs[]={ B_BC, B_BC, B_BC, B_MC, B_MC, B_MC, B_TC, B_TC, B_TC, B_MC, B_HO };

/*------------------------------------------------------------------------
 * wborder() - draw a border in a window
 */
int wborder  (WINDOW *win,
	chtype ls,
	chtype rs,
	chtype ts,
	chtype bs,
	chtype tl,
	chtype tr,
	chtype bl,
	chtype br)
{
	attr_t	o;
	int		i;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	o = getattrs(win);

	/*--------------------------------------------------------------------
	 * adjust args
	 */
	if (ls == 0)
		ls = ACS_VLINE;
	ls |= (o | win->_code);

	if (rs == 0)
		rs = ACS_VLINE;
	rs |= (o | win->_code);

	if (ts == 0)
		ts = ACS_HLINE;
	ts |= (o | win->_code);

	if (bs == 0)
		bs = ACS_HLINE;
	bs |= (o | win->_code);

	if (tl == 0)
		tl = ACS_ULCORNER;
	tl |= (o | win->_code);

	if (tr == 0)
		tr = ACS_URCORNER;
	tr |= (o | win->_code);

	if (bl == 0)
		bl = ACS_LLCORNER;
	bl |= (o | win->_code);

	if (br == 0)
		br = ACS_LRCORNER;
	br |= (o | win->_code);

	/*--------------------------------------------------------------------
	 * top line of box
	 */
	woutch(win, 0, 0, tl);
	for (i=1; i<getmaxx(win)-1; i++)
		woutch(win, 0, i, ts);
	woutch(win, 0, getmaxx(win)-1, tr);

	/*--------------------------------------------------------------------
	 * sides of box
	 */
	for (i=1; i<getmaxy(win)-1; i++)
	{
		woutch(win, i, 0,              ls);
		woutch(win, i, getmaxx(win)-1, rs);
	}

	/*--------------------------------------------------------------------
	 * bottom line of box
	 */
	woutch(win, getmaxy(win)-1, 0, bl);
	for (i=1; i<getmaxx(win)-1; i++)
		woutch(win, getmaxy(win)-1, i, bs);
	woutch(win, getmaxy(win)-1, getmaxx(win)-1, br);

	/*--------------------------------------------------------------------
	 * show window changed
	 */
	touchwin(win);

	/*--------------------------------------------------------------------
	 * if auto-sync in effect, sync window with its parents
	 */
	if (getsync(win))
		wsyncup(win);

	/*--------------------------------------------------------------------
	 * if immed in effect, do a refresh
	 */
	if (getimmed(win))
		return wrefresh(win);

	return (OK);
}

/*------------------------------------------------------------------------
 * whline() - draw a horiz line
 */
int whline (WINDOW *win, chtype c, int n)
{
	int		i;
	int		y;
	int		x;
	attr_t	o;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	o = getattrs(win);

	/*--------------------------------------------------------------------
	 * adjust number of chars to fit in window
	 */
	if (n < 0 || n > getmaxx(win) - getcurx(win))
		n = getmaxx(win) - getcurx(win);

	/*--------------------------------------------------------------------
	 * adjust horiz line char if needed
	 */
	if (c == 0)
		c = ACS_HLINE;
	c |= (o | win->_code);

	/*--------------------------------------------------------------------
	 * now do it
	 */
	y = getcury(win);
	x = getcurx(win);
	for (i=0; i<n; i++)
	{
		woutch(win, y, x+i, c);
	}

	/*--------------------------------------------------------------------
	 * show line changed
	 */
	touchline(win, y, 1);

	/*--------------------------------------------------------------------
	 * if auto-sync in effect, sync window with its parents
	 */
	if (getsync(win))
		wsyncup(win);

	/*--------------------------------------------------------------------
	 * if immed in effect, do a refresh
	 */
	if (getimmed(win))
		return wrefresh(win);

	return (OK);
}

/*------------------------------------------------------------------------
 * wvline() - draw a vertical line
 */
int wvline (WINDOW *win, chtype c, int n)
{
	int		i;
	int		y;
	int		x;
	attr_t	o;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	o = getattrs(win);

	/*--------------------------------------------------------------------
	 * adjust number of chars to fit in window
	 */
	if (n < 0 || n > getmaxy(win) - getcury(win))
		n = getmaxy(win) - getcury(win);

	/*--------------------------------------------------------------------
	 * adjust vert line char if needed
	 */
	if (c == 0)
		c = ACS_VLINE;
	c |= (o | win->_code);

	/*--------------------------------------------------------------------
	 * now do it
	 */
	y = getcury(win);
	x = getcurx(win);
	for (i=0; i<n; i++)
	{
		woutch(win, y+i, x, c);
	}

	/*--------------------------------------------------------------------
	 * show lines changed
	 */
	touchline(win, y, n);

	/*--------------------------------------------------------------------
	 * if auto-sync in effect, sync window with its parents
	 */
	if (getsync(win))
		wsyncup(win);

	/*--------------------------------------------------------------------
	 * if immed in effect, do a refresh
	 */
	if (getimmed(win))
		return wrefresh(win);

	return (OK);
}

/*------------------------------------------------------------------------
 * box_chk_type() - internal routine to adjust box type according to what
 * char-sets are defined
 */
static int box_chk_type (WINDOW *win, int type, int c)
{
	c = box_chrs[type][c];
	if (! scrn_get_valid_acs(WIN_SCREEN(win), c))
		type = B_ASCII;

	return (type);
}

/*------------------------------------------------------------------------
 * wbox_chr() - draw a single box character
 */
int wbox_chr (WINDOW *win, int chr, int type)
{
	int rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * adjust box type
	 */
	type = box_chk_type(win, type, chr);

	/*--------------------------------------------------------------------
	 * draw the char
	 */
	if (type)
	{
		rc = waddch(win, box_chrs[type][chr] | A_ALTCHARSET);
	}
	else
	{
		rc = waddch(win, box_chrs[B_ASCII][chr]);
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * wblk_chr() - draw a single block character
 */
int wblk_chr (WINDOW *win, int blk, int chr)
{
	int type;
	int rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * adjust type
	 */
	type = box_chk_type(win, B_BLOCK, blk);

	/*--------------------------------------------------------------------
	 * now draw the char
	 */
	if (type == 0)
	{
		if (chr == 0)
			chr = 'x';
		rc = waddch(win, chr);
	}
	else
	{
		rc = waddch(win, box_chrs[B_BLOCK][blk] | A_ALTCHARSET);
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * wbox() - draw a box of a specified type inside the edges of a window
 */
int wbox (WINDOW *win, int type)
{
	int i;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * top line
	 */
	wmove(win, 0, 0);
	wbox_chr(win, B_TL, type);
	for (i=1; i<getmaxx(win)-1; i++)
		wbox_chr(win, B_HO, type);
	wbox_chr(win, B_TR, type);

	/*--------------------------------------------------------------------
	 * sides
	 */
	for (i=1; i<getmaxy(win)-1; i++)
	{
		wmove(win, i, 0);
		wbox_chr(win, B_VE, type);

		wmove(win, i, getmaxx(win)-1);
		wbox_chr(win, B_VE, type);
	}

	/*--------------------------------------------------------------------
	 * bottom line
	 */
	wmove(win, getmaxy(win)-1, 0);
	wbox_chr(win, B_BL, type);
	for (i=1; i<getmaxx(win)-1; i++)
		wbox_chr(win, B_HO, type);
	wbox_chr(win, B_BR, type);

	return (OK);
}

/*------------------------------------------------------------------------
 * smart_wbox() - draw a "smart" box, adjusting the chars to match what
 * is already there
 */
int smart_wbox (WINDOW *win, int y_start, int x_start,
	int rows, int cols, int type)
{
	int y, x;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	if (y_start+rows > getmaxy(win))
		return (ERR);
	if (x_start+cols > getmaxx(win))
		return (ERR);

	/*--------------------------------------------------------------------
	 * top row of box
	 */
	smart_box_chr(win, y_start, x_start, B_TL, type);
	for (x=1; x<cols-1; x++)
		smart_box_chr(win, y_start, x_start+x, B_HO, type);
	smart_box_chr(win, y_start, x_start+x, B_TR, type);

	/*--------------------------------------------------------------------
	 * middle rows
	 */
	for (y=1; y<rows-1; y++)
	{
		smart_box_chr(win, y_start+y, x_start,        B_VE, type);
		smart_box_chr(win, y_start+y, x_start+cols-1, B_VE, type);
	}

	/*--------------------------------------------------------------------
	 * bottom row
	 */
	smart_box_chr(win, y_start+y, x_start, B_BL, type);
	for (x=1; x<cols-1; x++)
		smart_box_chr(win, y_start+y, x_start+x, B_HO, type);
	smart_box_chr(win, y_start+y, x_start+x, B_BR, type);

	/*--------------------------------------------------------------------
	 * show window changed
	 */
	touchwin(win);

	/*--------------------------------------------------------------------
	 * if auto-sync in effect, sync window with its parents
	 */
	if (getsync(win))
		wsyncup(win);

	/*--------------------------------------------------------------------
	 * if immed in effect, do a refresh
	 */
	if (getimmed(win))
		return wrefresh(win);

	return (OK);
}

/*------------------------------------------------------------------------
 * box_around() - draw a "smart" box around a window in a specified
 * border-window
 */
int box_around (WINDOW *win, WINDOW *brdr_win, int type)
{
	int y, x;
	int rows;
	int cols;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * make sure win is contained in brdr_win
	 */
	if (getbegy(win) <= getbegy(brdr_win))
		return (ERR);
	if (getbegx(win) <= getbegx(brdr_win))
		return (ERR);
	if (getbegy(win)+getmaxy(win) >= getbegy(brdr_win)+getmaxy(brdr_win))
		return (ERR);
	if (getbegx(win)+getmaxx(win) >= getbegx(brdr_win)+getmaxx(brdr_win))
		return (ERR);

	/*--------------------------------------------------------------------
	 * get starting location in brdr_win
	 */
	y = getbegy(win)-getbegy(brdr_win)-1;
	x = getbegx(win)-getbegx(brdr_win)-1;

	rows = getmaxy(win)+2;
	cols = getmaxx(win)+2;

	return (smart_wbox(brdr_win, y, x, rows, cols, type));
}

/*------------------------------------------------------------------------
 * smart_box_chr() - draw a "smart" box character
 */
int smart_box_chr (WINDOW *win, int y, int x, int ch, int type)
{
	chtype c;
	attr_t a;
	int k;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * get box type
	 */
	type = box_chk_type(win, type, ch);

	/*--------------------------------------------------------------------
	 * get current char in window
	 */
	c = wchat(win, y, x);
	a = wattrget(win) | A_ALTCHARSET;

	/*--------------------------------------------------------------------
	 * check if char to be added needs to be adjusted
	 */
	if ((A_GETATTR(c) & A_ALTCHARSET) == 0)
	{
		/*----------------------------------------------------------------
		 * no - just use it
		 */
		k = ch;
	}
	else
	{
		/*----------------------------------------------------------------
		 * yes - adjust according to char type
		 */
		k = A_GETTEXT(c);

		if      (k == box_chrs[type][B_BL])
			k = bl_chrs[ch];

		else if (k == box_chrs[type][B_BC])
			k = bc_chrs[ch];

		else if (k == box_chrs[type][B_BR])
			k = br_chrs[ch];

		else if (k == box_chrs[type][B_ML])
			k = ml_chrs[ch];

		else if (k == box_chrs[type][B_MC])
			k = mc_chrs[ch];

		else if (k == box_chrs[type][B_MR])
			k = mr_chrs[ch];

		else if (k == box_chrs[type][B_TL])
			k = tl_chrs[ch];

		else if (k == box_chrs[type][B_TC])
			k = tc_chrs[ch];

		else if (k == box_chrs[type][B_TR])
			k = tr_chrs[ch];

		else if (k == box_chrs[type][B_VE])
			k = ve_chrs[ch];

		else if (k == box_chrs[type][B_HO])
			k = ho_chrs[ch];

		else
			k = ch;
	}

	/*--------------------------------------------------------------------
	 * now add the char
	 */
	k = box_chrs[type][k];
	woutch(win, y, x, k | a);

	return (OK);
}
