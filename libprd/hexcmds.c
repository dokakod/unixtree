/*------------------------------------------------------------------------
 * hexedit cmds
 */
#include "libprd.h"

/*------------------------------------------------------------------------
 * low-level internal routines
 */
static int hexedit_hex_digit (int c)
{
	c = tolower(c);

	if (c >= '0' && c <= '9')
		return (c - '0');

	if (c >= 'a' && c <= 'f')
		return (c - 'a' + 10);

	return (-1);
}

static int hexedit_ascii_char (int c)
{
	if (iscntrl(c & 0x7f))
		c = '.';
	else if (c > 0x7f && opt(display_mask))
		c = '.';

	return (c);
}

static void hexedit_addr (int n, char *buf)
{
	int i;
	int k;

	for (i=5; i>=0; i--)
	{
		k = n & 0x0f;
		n >>= 4;
		buf[i] = TO_HEX(k);
	}
	buf[6] = 0;
}

static void hexedit_move (int offs, int hex, int left)
{
	int row;
	int col;

	offs -= gbl(hxcb)->x_top_offs;
	row   = offs / gbl(hxcb)->x_width;
	offs %= gbl(hxcb)->x_width;

	if (hex)
	{
		col = (offs * 3) + (offs / 4) + 8;
		if (! left)
			col++;
	}
	else
	{
		col = ((gbl(hxcb)->x_width * 13) / 4) + 8 + offs;
	}

	wmove(gbl(hxcb)->x_win, row, col);
}

static void hexedit_position (void)
{
	hexedit_move(gbl(hxcb)->x_cur_offs, gbl(hxcb)->x_hex, gbl(hxcb)->x_left);
}

static void hexedit_display_hex_left (int offs)
{
	int cd	= gbl(hxcb)->x_disp_buf[offs];
	int co	= gbl(hxcb)->x_orig_buf[offs];
	int cdh = (cd >> 4);

	hexedit_move(offs, TRUE, TRUE);

	if (cd != co)
		wstandout(gbl(hxcb)->x_win);
	waddch(gbl(hxcb)->x_win, TO_HEX(cdh));
	if (cd != co)
		wstandend(gbl(hxcb)->x_win);
}

static void hexedit_display_hex_right (int offs)
{
	int cd	= gbl(hxcb)->x_disp_buf[offs];
	int co	= gbl(hxcb)->x_orig_buf[offs];
	int cdh = (cd & 0x0f);

	hexedit_move(offs, TRUE, FALSE);

	if (cd != co)
		wstandout(gbl(hxcb)->x_win);
	waddch(gbl(hxcb)->x_win, TO_HEX(cdh));
	if (cd != co)
		wstandend(gbl(hxcb)->x_win);
}

static void hexedit_display_ascii (int offs)
{
	int cd	= gbl(hxcb)->x_disp_buf[offs];
	int co	= gbl(hxcb)->x_orig_buf[offs];

	hexedit_move(offs, FALSE, FALSE);

	if (cd != co)
		wstandout(gbl(hxcb)->x_win);
	waddch(gbl(hxcb)->x_win, hexedit_ascii_char(cd));
	if (cd != co)
		wstandend(gbl(hxcb)->x_win);
}

static void hexedit_display_char (int offs)
{
	hexedit_display_hex_left  (offs);
	hexedit_display_hex_right (offs);
	hexedit_display_ascii     (offs);
}

static void hexedit_display_line (int offs)
{
	char	addr_buf[7];
	int		row;
	int		i;

	offs = (offs / gbl(hxcb)->x_width) * gbl(hxcb)->x_width;
	row  = (offs - gbl(hxcb)->x_top_offs) / gbl(hxcb)->x_width;

	hexedit_addr(offs, addr_buf);

	wmove    (gbl(hxcb)->x_win, row, 0);
	wclrtoeol(gbl(hxcb)->x_win);
	waddch   (gbl(hxcb)->x_win, ' ');
	waddstr  (gbl(hxcb)->x_win, addr_buf);
	waddch   (gbl(hxcb)->x_win, ' ');

	for (i = 0; i < gbl(hxcb)->x_width; i++)
	{
		if (offs >= gbl(hxcb)->x_filesize)
			break;

		hexedit_display_char(offs++);
	}
}

/*------------------------------------------------------------------------
 * internal window-move routines
 */
static void hexedit_cmd_up (void)
{
	if (gbl(hxcb)->x_cur_offs >= gbl(hxcb)->x_width)
	{
		gbl(hxcb)->x_cur_offs -= gbl(hxcb)->x_width;

		if (gbl(hxcb)->x_cur_offs < gbl(hxcb)->x_top_offs)
		{
			gbl(hxcb)->x_top_offs -= gbl(hxcb)->x_width;
			hexedit_redisplay();
		}
		else
		{
			hexedit_position();
		}
	}
}

static void hexedit_cmd_down (void)
{
	if ((gbl(hxcb)->x_cur_offs + gbl(hxcb)->x_width) <= gbl(hxcb)->x_filesize)
	{
		gbl(hxcb)->x_cur_offs += gbl(hxcb)->x_width;

		if ((gbl(hxcb)->x_cur_offs - gbl(hxcb)->x_top_offs) >=
			gbl(hxcb)->x_chars)
		{
			gbl(hxcb)->x_top_offs += gbl(hxcb)->x_width;
			hexedit_redisplay();
		}
		else
		{
			hexedit_position();
		}
	}
}

static void hexedit_cmd_left (void)
{
	if (gbl(hxcb)->x_hex && ! gbl(hxcb)->x_left)
	{
		gbl(hxcb)->x_left = TRUE;
		hexedit_position();
	}
	else
	{
		if ((gbl(hxcb)->x_cur_offs - 1) >= 0)
		{
			gbl(hxcb)->x_cur_offs--;
			gbl(hxcb)->x_left = FALSE;

			if (gbl(hxcb)->x_cur_offs < gbl(hxcb)->x_top_offs)
			{
				gbl(hxcb)->x_top_offs -= gbl(hxcb)->x_width;
				hexedit_redisplay();
			}
			else
			{
				hexedit_position();
			}
		}
	}
}

static void hexedit_cmd_right (void)
{
	if (gbl(hxcb)->x_hex && gbl(hxcb)->x_left)
	{
		gbl(hxcb)->x_left = FALSE;
		hexedit_position();
	}
	else
	{
		if ((gbl(hxcb)->x_cur_offs + 1) < gbl(hxcb)->x_filesize)
		{
			gbl(hxcb)->x_cur_offs++;
			gbl(hxcb)->x_left = TRUE;

			if ((gbl(hxcb)->x_cur_offs - gbl(hxcb)->x_top_offs) >=
				gbl(hxcb)->x_chars)
			{
				gbl(hxcb)->x_top_offs += gbl(hxcb)->x_width;
				hexedit_redisplay();
			}
			else
			{
				hexedit_position();
			}
		}
	}
}

static void hexedit_cmd_home (void)
{
	int old_top = gbl(hxcb)->x_top_offs;

	gbl(hxcb)->x_top_offs = 0;
	gbl(hxcb)->x_cur_offs = 0;

	if (old_top > 0)
	{
		hexedit_redisplay();
	}
	else
	{
		hexedit_position();
	}
}

static void hexedit_cmd_end (void)
{
	int old_top = gbl(hxcb)->x_top_offs;

	gbl(hxcb)->x_top_offs = (gbl(hxcb)->x_filesize / gbl(hxcb)->x_chars) *
		gbl(hxcb)->x_chars;
	gbl(hxcb)->x_cur_offs = gbl(hxcb)->x_filesize - 1;

	if (old_top != gbl(hxcb)->x_top_offs)
	{
		hexedit_redisplay();
	}
	else
	{
		hexedit_position();
	}
}

static void hexedit_cmd_pgup (void)
{
	if ((gbl(hxcb)->x_top_offs - gbl(hxcb)->x_chars) >= 0)
	{
		gbl(hxcb)->x_top_offs -= gbl(hxcb)->x_chars;
		gbl(hxcb)->x_cur_offs  = gbl(hxcb)->x_top_offs;

		hexedit_redisplay();
	}
	else
	{
		hexedit_cmd_home();
	}
}

static void hexedit_cmd_pgdn (void)
{
	if ((gbl(hxcb)->x_top_offs + gbl(hxcb)->x_chars) < gbl(hxcb)->x_filesize)
	{
		gbl(hxcb)->x_top_offs += gbl(hxcb)->x_chars;
		gbl(hxcb)->x_cur_offs  = gbl(hxcb)->x_top_offs;

		hexedit_redisplay();
	}
	else
	{
		hexedit_cmd_end();
	}
}

/*------------------------------------------------------------------------
 * internal command routines
 */
static void hexedit_cmd_save (void)
{
	hexedit_file_end();
}

static void hexedit_cmd_quit (void)
{
	gbl(hxcb)->x_dirty = FALSE;
	hexedit_cmd_save();
}

static void hexedit_cmd_toggle (void)
{
	gbl(hxcb)->x_hex = ! gbl(hxcb)->x_hex;
	if (gbl(hxcb)->x_hex)
		gbl(hxcb)->x_left = TRUE;

	hexedit_position();
}

static void hexedit_cmd_undo (void)
{
	memcpy(gbl(hxcb)->x_disp_buf, gbl(hxcb)->x_orig_buf,
		gbl(hxcb)->x_filesize);
	gbl(hxcb)->x_dirty = FALSE;

	hexedit_redisplay();
}

static void hexedit_cmd_mask (void)
{
	opt(display_mask) = ! opt(display_mask);

	hexedit_redisplay();
}

static void hexedit_cmd_char (int c)
{
	unsigned char *	cp	= gbl(hxcb)->x_disp_buf + gbl(hxcb)->x_cur_offs;

	if (gbl(hxcb)->x_hex)
	{
		c = hexedit_hex_digit(c);
		if (c < 0)
			return;

		if (gbl(hxcb)->x_left)
		{
			c = (c << 4) | (*cp & 0x0f);
		}
		else
		{
			c |= (*cp & 0xf0);
		}
	}

	*cp = (unsigned char)c;
	gbl(hxcb)->x_dirty = TRUE;
	hexedit_display_line(gbl(hxcb)->x_cur_offs);
	wrefresh(gbl(hxcb)->x_win);

	hexedit_cmd_right();
}

/*------------------------------------------------------------------------
 * global routines
 */
int do_hexedit_cmd (int c)
{
	switch (c)
	{
	case KEY_UP:	hexedit_cmd_up();		return (0);
	case KEY_DOWN:	hexedit_cmd_down();		return (0);
	case KEY_LEFT:	hexedit_cmd_left();		return (0);
	case KEY_RIGHT:	hexedit_cmd_right();	return (0);

	case KEY_PGUP:	hexedit_cmd_pgup();		return (0);
	case KEY_PGDN:	hexedit_cmd_pgdn();		return (0);
	case KEY_HOME:	hexedit_cmd_home();		return (0);
	case KEY_END:	hexedit_cmd_end();		return (0);
	}

	if (c == cmds(CMDS_ESCAPE))
	{
		hexedit_cmd_quit();
	}

	else if (c == cmds(CMDS_RETURN))
	{
		hexedit_cmd_save();
	}

	else if (c == cmds(CMDS_HEXEDIT_TOGGLE))
	{
		hexedit_cmd_toggle();
	}

	else if (c == cmds(CMDS_HEXEDIT_UNDO))
	{
		hexedit_cmd_undo();
	}

	else if (c == cmds(CMDS_HEXEDIT_MASK))
	{
		hexedit_cmd_mask();
	}

	else if (IS_ACHAR(c))
	{
		hexedit_cmd_char(c);
	}

	return (0);
}

void hexedit_redisplay (void)
{
	int	offs	= gbl(hxcb)->x_top_offs;
	int	row;

	werase(gbl(hxcb)->x_win);

	for (row = 0; row < gbl(hxcb)->x_lines; row++)
	{
		hexedit_display_line(offs);

		offs += gbl(hxcb)->x_width;
		if (offs >= gbl(hxcb)->x_filesize)
			break;
	}

	hexedit_position();

	wrefresh(gbl(hxcb)->x_win);
}
