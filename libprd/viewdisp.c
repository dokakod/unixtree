/*------------------------------------------------------------------------
 * view display routines
 */
#include "libprd.h"

static void disp_char (VFCB *v, int c)
{
	if (iscntrl(c & 0x7f))
	{
		c = '.';
	}
	else
	{
		if (c > 0x7f && (v->v_mode & V_MASKED))
			c = '.';
	}

	waddch(v->v_win, c);
}

static char *make_hex (unsigned int n, int len, char *buf)
{
	int i;
	int k;

	if (len > 8)
		len = 8;

	for (i=len-1; i>=0; i--)
	{
		k = n & 0x0f;
		n >>= 4;
		buf[i] = TO_HEX(k);
	}
	buf[len] = 0;

	return (buf);
}

static void view_hscroll_bar_off (VFCB *v)
{
	int i;

	wmove(gbl(win_border), getbegy(v->v_win) + getmaxy(v->v_win),
	                 getbegx(v->v_win));
	for (i=0; i<getmaxx(v->v_win); i++)
		wbox_chr(gbl(win_border), B_HO, B_DVDH);
	wrefresh(gbl(win_border));
}

static void do_view_hscroll_bar (VFCB *v)
{
	long beg, end;
	int is, ie;
	int i;

	/* turn off scroll bar (same as above w/o refresh */

	wmove(gbl(win_border), getbegy(v->v_win) + getmaxy(v->v_win),
	                	getbegx(v->v_win));
	for (i=0; i<getmaxx(v->v_win); i++)
		wbox_chr(gbl(win_border), B_HO, B_DVDH);

	if (v->v_filesize)
	{
		beg = v->v_top_pos;
		if (v->v_cur_ptr)
			end = v->v_cur_pos;
		else
			end = v->v_filesize;
		is = (beg * getmaxx(v->v_win)) / v->v_filesize;
		ie = (end * getmaxx(v->v_win)) / v->v_filesize;
		if (is == ie)
			ie++;

		wmove(gbl(win_border), getbegy(v->v_win) + getmaxy(v->v_win),
	                 	getbegx(v->v_win) + is);
		for (i=is; i<ie; i++)
			wbox_chr(gbl(win_border), B_HO, B_SVSH);
		wrefresh(gbl(win_border));
	}
}

static void view_vscroll_bar_off (VFCB *v)
{
	int i;

	for (i=0; i<getmaxy(v->v_win); i++)
	{
		wmove(gbl(win_border), getbegy(v->v_win) + i,
		                 getbegx(v->v_win) - 1);
		wbox_chr(gbl(win_border), B_VE, B_DVDH);
	}
	wrefresh(gbl(win_border));
}

static void do_view_vscroll_bar (VFCB *v)
{
	long beg, end;
	int is, ie;
	int i;

	/* turn off scroll bar (same as above w/o refresh */

	for (i=0; i<getmaxy(v->v_win); i++)
	{
		wmove(gbl(win_border), getbegy(v->v_win) + i,
		                 getbegx(v->v_win) - 1);
		wbox_chr(gbl(win_border), B_VE, B_DVDH);
	}

	if (v->v_filesize)
	{
		beg = v->v_top_pos;
		if (v->v_cur_ptr)
			end = v->v_cur_pos;
		else
			end = v->v_filesize;
		is = (beg * getmaxy(v->v_win)) / v->v_filesize;
		ie = (end * getmaxy(v->v_win)) / v->v_filesize;
		if (is == ie)
			ie++;

		for (i=is; i<ie; i++)
		{
			wmove(gbl(win_border), getbegy(v->v_win) + i,
		                 	getbegx(v->v_win) - 1);
			wbox_chr(gbl(win_border), B_VE, B_SVSH);
		}
		wrefresh(gbl(win_border));
	}
}

void view_scroll_bar_off (VFCB *v)
{
	if (v->v_mode & V_HSCROLLBAR)
		view_hscroll_bar_off(v);
	else if (v->v_mode & V_VSCROLLBAR)
		view_vscroll_bar_off(v);
}

static void do_view_scroll_bar (VFCB *v)
{
	if (v->v_mode & V_HSCROLLBAR)
		do_view_hscroll_bar(v);
	else if (v->v_mode & V_VSCROLLBAR)
		do_view_vscroll_bar(v);
}

void v_display_no_refresh (VFCB *v)
{
	int row;
	int col;
	int i;
	int c;

	werase(v->v_win);

	if (v->v_filesize)
	{
		char hbuf[9];

		switch (v->v_mode & V_MODE)
		{
		case V_DUMP:
					v->v_cur_pos = v->v_top_pos;
					get_buffer(v);
					for (row=0; row<getmaxy(v->v_win); row++)
					{
						wmove(v->v_win, row, 1);
						waddstr(v->v_win, make_hex(v->v_cur_pos, 6, hbuf));
						waddch(v->v_win, ' ');
						for (i=0; i<v->v_dump_width; i++)
						{
							if (!v->v_cur_ptr)
								break;
							c = *v->v_cur_ptr;
							bump_pos(v);
							disp_char(v, c);
						}
						if (!v->v_cur_ptr)
							break;
					}
					break;

		case V_HEX:
					v->v_cur_pos = v->v_top_pos & ~3;
					get_buffer(v);
					for (row=0; row<getmaxy(v->v_win); row++)
					{
						wmove(v->v_win, row, 1);
						waddstr(v->v_win, make_hex(v->v_cur_pos, 6, hbuf));
						waddch(v->v_win, ' ');
						for (i=0; i<v->v_hex_width; i++)
						{
							if (!v->v_cur_ptr)
								break;
							c = *v->v_cur_ptr;
							bump_pos(v);
							wmove(v->v_win, row, i*3+i/4+8);
							waddch(v->v_win, TO_HEX(c >> 4));
							waddch(v->v_win, TO_HEX(c & 0x0f));
							wmove(v->v_win, row, ((v->v_hex_width*13)/4)+8+i);
							disp_char(v, c);
						}
						if (!v->v_cur_ptr)
							break;
					}
					break;

		case V_ASCII:
		case V_WRAP:
					v->v_cur_pos = v->v_top_pos;
					get_buffer(v);

					for (row=0; row<getmaxy(v->v_win); row++)
					{
						wmove(v->v_win, row, 0);
						c = 0;
						for (col=0; col<v->v_ascii_width; )
						{
							if (!v->v_cur_ptr)
								break;
							c = *v->v_cur_ptr;
							bump_pos(v);
							if (iscntrl(c))
							{
								if (c == '\r')
									continue;
								if (c == '\n')
									break;
								if (c == '\t')
								{
									i = ((col+v->v_tab_width)/v->v_tab_width)*
											v->v_tab_width;
									for (; col<i; col++)
										waddch(v->v_win, ' ');
									continue;
								}
								c = '.';
							}
							disp_char(v, c);
							col++;
						}
						if ((v->v_mode & V_MODE) == V_ASCII && c != '\n')
						{
							while (col < V_MAX_LINEWIDTH)
							{
								if (!v->v_cur_ptr)
									break;
								c = *v->v_cur_ptr;
								bump_pos(v);
								if (c == '\n')
									break;
								col++;
							}
						}
						if (!v->v_cur_ptr)
							break;
					}
					break;
		}
	}

	v->v_bot_pos = v->v_cur_pos;

	do_view_scroll_bar(v);
}

void v_display (VFCB *v)
{
	v_display_no_refresh(v);
	wrefresh(v->v_win);
}
