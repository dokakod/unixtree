/*------------------------------------------------------------------------
 * scrollbar routines
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * scrollbar definitions
 */
#define SCRLBAR_V_ALT_ON	0xb2 | A_ALTCHARSET
#define SCRLBAR_V_ALT_OFF	0xb0 | A_ALTCHARSET
#define SCRLBAR_V_REG_ON	'I'
#define SCRLBAR_V_REG_OFF	'|'

#define SCRLBAR_H_ALT_ON	0xb2 | A_ALTCHARSET
#define SCRLBAR_H_ALT_OFF	0xb0 | A_ALTCHARSET
#define SCRLBAR_H_REG_ON	'='
#define SCRLBAR_H_REG_OFF	'-'

/*------------------------------------------------------------------------
 * scrollbar tables
 */
static const int scrlbar_alt_chars[4][2] =
{
	{ SCRLBAR_V_ALT_OFF,	SCRLBAR_V_ALT_ON },
	{ SCRLBAR_V_ALT_OFF,	SCRLBAR_V_ALT_ON },
	{ SCRLBAR_H_ALT_OFF,	SCRLBAR_H_ALT_ON },
	{ SCRLBAR_H_ALT_OFF,	SCRLBAR_H_ALT_ON }
};

static const int scrlbar_reg_chars[4][2] =
{
	{ SCRLBAR_V_REG_OFF,	SCRLBAR_V_REG_ON },
	{ SCRLBAR_V_REG_OFF,	SCRLBAR_V_REG_ON },
	{ SCRLBAR_H_REG_OFF,	SCRLBAR_H_REG_ON },
	{ SCRLBAR_H_REG_OFF,	SCRLBAR_H_REG_ON }
};

/*------------------------------------------------------------------------
 * get_scrlbar_char() - internal routine to get proper char to display
 */
static chtype get_scrlbar_char (WINDOW *win, int type, int on)
{
	int		index = (on ? 1 : 0);
	chtype	ch;

	ch = scrlbar_alt_chars[type][index];
	if (! scrn_get_valid_acs(WIN_SCREEN(win), (int)ch))
		ch = scrlbar_reg_chars[type][index];

	return (ch);
}

/*------------------------------------------------------------------------
 * scrollbar() - display a scrollbar
 */
int scrollbar (WINDOW *brdr_win, WINDOW *win, int type, int cur, int tot)
{
	int		beg_y;
	int		beg_x;
	int		cur_x;
	int		cur_y;
	int		add_x;
	int		add_y;
	int		first;
	int		last;
	int		len;
	int		pos;
	int		i;
	chtype	char_on;
	chtype	char_off;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (brdr_win == 0 || win == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * get window coordinates & size
	 */
	switch (type)
	{
	case SCRLBAR_VL:
		beg_x = (getbegx(win) - getbegx(brdr_win)) - 1;
		beg_y = (getbegy(win) - getbegy(brdr_win));
		add_x = 0;
		add_y = 1;
		first = 0;
		last  = getmaxy(win) - 1;
		len   = (last - first);
		break;

	case SCRLBAR_VR:
		beg_x = (getbegx(win) - getbegx(brdr_win)) + (getmaxx(win) - 1);
		beg_y = (getbegy(win) - getbegy(brdr_win));
		add_x = 0;
		add_y = 1;
		first = 0;
		last  = getmaxy(win) - 1;
		len   = (last - first);
		break;

	case SCRLBAR_HT:
		beg_x = getbegx(brdr_win);
		beg_y = (getbegy(win) - getbegy(brdr_win));
		add_x = 1;
		add_y = 0;
		first = 0;
		last  = getmaxx(win) - 1;
		len   = (last - first);
		break;

	case SCRLBAR_HB:
		beg_x = (getbegx(win) - getbegx(brdr_win)) + 1;
		beg_y = (getbegy(win) - getbegy(brdr_win)) + (getmaxy(win) - 1);
		add_x = 1;
		add_y = 0;
		first = 0;
		last  = getmaxx(win) - 1;
		len   = (last - first);
		break;

	default:
		return (ERR);
	}

	/*--------------------------------------------------------------------
	 * get scrollbar chars
	 */
	char_on  = get_scrlbar_char(brdr_win, type, TRUE);
	char_off = get_scrlbar_char(brdr_win, type, FALSE);

	/*--------------------------------------------------------------------
	 * first, always clear the scrollbar
	 */
	{
		cur_x = beg_x;
		cur_y = beg_y;

		for (i=0; i<=len; i++)
		{
			wmove(brdr_win, cur_y, cur_x);
			waddch(brdr_win, char_off);
			cur_x += add_x;
			cur_y += add_y;
		}
	}

	/*--------------------------------------------------------------------
	 * Now turn on the correct char if something to turn on.
	 */
	if (cur >= 0 && tot > 0)
	{
		if (cur == 0)
			pos = first;
		else if (cur >= tot-1)
			pos = last;
		else
			pos = first + ((cur * len) / tot);

		wmove(brdr_win, beg_y + (pos * add_y), beg_x + (pos * add_x));
		waddch(brdr_win, char_on);
	}

	return (OK);
}
