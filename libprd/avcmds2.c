/*------------------------------------------------------------------------
 * autoview low-level routines
 */
#include "libprd.h"

void open_av_file (FBLK *f)
{
	gbl(scr_cur)->av_file_displayed = f;
	(gbl(scr_cur)->vfcb)->v_mode &= ~V_SYM;
	if (S_ISLNK(f->stbuf.st_mode))
		(gbl(scr_cur)->vfcb)->v_mode |= V_SYM;
	fblk_to_pathname(f, (gbl(scr_cur)->vfcb)->v_pathname);
	view_command(gbl(scr_cur)->vfcb, V_CMD_OPEN_RO);
	disp_av_mode();
}

void close_av_file (void)
{
	view_command(gbl(scr_cur)->vfcb, V_CMD_CLOSE);
	gbl(scr_cur)->av_file_displayed = 0;
}

void display_av (void)
{
	if (gbl(scr_cur)->av_file_displayed)
	{
		view_command(gbl(scr_cur)->vfcb, V_CMD_REDISPLAY);
		disp_av_pct(TRUE);
	}
}

void disp_av_pct (int ref)
{
	WINDOW *w = gbl(scr_cur)->cur_av_mode_win;
	VFCB *v = gbl(scr_cur)->vfcb;
	char dbuf[12];
	int top;
	int bot;

	wmove(w, 0, 11);
	wclrtoeol(w);

	if (v->v_filesize == 0)
	{
		top = 0;
		bot = 100;
	}
	else
	{
		top = (v->v_top_pos * 100) / v->v_filesize;
		bot = (v->v_bot_pos * 100) / v->v_filesize;
	}

	waddstr(w, xform2(dbuf, top, 3));
	waddstr(w, "%-");
	waddstr(w, xform2(dbuf, bot, 3));
	waddch (w, '%');

	if (ref)
		wrefresh(w);
}

void disp_av_mode (void)
{
	WINDOW *w = gbl(scr_cur)->cur_av_mode_win;

	wmove(w, 0, 0);
	if (gbl(scr_cur)->vfcb->v_mode & V_AUTO)
		waddch(w, ' ');
	else
		waddch(w, '*');
	switch (gbl(scr_cur)->vfcb->v_mode & V_MODE)
	{
	case V_ASCII:
				waddstr(w, msgs(m_avcmds2_asc));
				break;
	case V_DUMP:
				waddstr(w, msgs(m_avcmds2_dmp));
				break;
	case V_HEX:
				waddstr(w, msgs(m_avcmds2_hex));
				break;
	case V_WRAP:
				waddstr(w, msgs(m_avcmds2_wrp));
				break;
	default:
				waddstr(w, msgs(m_avcmds2_qqq));
				break;
	}
	if (gbl(scr_cur)->vfcb->v_mode & V_MASKED)
		waddstr(w, msgs(m_avcmds2_mas));
	else
		waddstr(w, msgs(m_avcmds2_nmk));

	disp_av_pct(FALSE);

	wrefresh(w);
}

int get_disp_mode (void)
{
	int c;

	bang("");
	esc_msg();
	wmove(gbl(win_message), 0, 0);
	xaddstr(gbl(win_message), msgs(m_avcmds2_dsp));
	xcaddstr(gbl(win_message), CMDS_WIN_ASCII, msgs(m_cmds_ascii));
	waddstr(gbl(win_message), "  ");
	xcaddstr(gbl(win_message), CMDS_WIN_DUMP, msgs(m_cmds_dump));
	waddstr(gbl(win_message), "  ");
	xcaddstr(gbl(win_message), CMDS_WIN_HEX, msgs(m_cmds_hex));
	waddstr(gbl(win_message), "  ");
	xcaddstr(gbl(win_message), CMDS_WIN_KEEP, msgs(m_cmds_keep));
	waddstr(gbl(win_message), "  ");
	xcaddstr(gbl(win_message), CMDS_WIN_MASK, msgs(m_cmds_mask));
	waddstr(gbl(win_message), "  ");
	xcaddstr(gbl(win_message), CMDS_WIN_WRAP, msgs(m_cmds_wrap));
	waddstr(gbl(win_message), "  ");
	wrefresh(gbl(win_message));

	while (TRUE)
	{
		c = xgetch(gbl(win_message));

		if (c == KEY_ESCAPE           ||
			c == KEY_BS               ||
			c == cmds(CMDS_WIN_ASCII) ||
			c == cmds(CMDS_WIN_DUMP)  ||
			c == cmds(CMDS_WIN_HEX)   ||
			c == cmds(CMDS_WIN_KEEP)  ||
			c == cmds(CMDS_WIN_MASK)  ||
			c == cmds(CMDS_WIN_WRAP) )
			break;
	}

	bang("");
	return (c);
}
