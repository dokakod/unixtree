/*------------------------------------------------------------------------
 * misc message display routines
 */
#include "libprd.h"

void bang (const char *string)
{
	wmove(gbl(win_message), 0, 0);
	wclrtoeol(gbl(win_message));
	xaddstr(gbl(win_message), string);
	wrefresh(gbl(win_message));
}

void bangnc (const char *string)
{
	wmove(gbl(win_message), 0, 0);
	xaddstr(gbl(win_message), string);
	wrefresh(gbl(win_message));
}

void bang_save (chtype *save)
{
	int len = getmaxx(gbl(win_message));
	int i;

	for (i=0; i<len; i++)
		save[i] = wchat(gbl(win_message), 0, i);
}

void bang_restore (chtype *save)
{
	int len = getmaxx(gbl(win_message));
	int i;

	for (i=0; i<len; i++)
		woutch(gbl(win_message), 0, i, save[i]);

	touchwin(gbl(win_message));
	wrefresh(gbl(win_message));
}

int anykey (void)
{
	int c;

	c = wanykey(gbl(win_message));
	werase(gbl(win_message));
	wrefresh(gbl(win_message));
	return (c);
}

int wanykey (WINDOW *win)
{
	int c;

	xaddstr(win, msgs(m_bang_anykey));
	wrefresh(win);
	while (TRUE)
	{
		c = xgetch(win);
		if (c == ERR)
			continue;
		if (c != KEY_MOUSE)
			break;
		if (mouse_get_event(win) == MOUSE_LBD)
			break;
	}
	if (c == KEY_ESCAPE)
		return (-1);
	else
		return (1);
}

int anykey_msg (const char *mesg)
{
	werase(gbl(win_message));
	esc_msg();
	bangnc(mesg);
	return (anykey());
}

int yesno (void)
{
	int c;

	c = wyesno(gbl(win_message));
	werase(gbl(win_message));
	wrefresh(gbl(win_message));
	return (c);
}

int wyesno (WINDOW *win)
{
	int c;

	waddstr(win, " (");
	xcaddstr(win, CMDS_YUP1, msgs(m_bang_yes));
	waddstr(win, "/");
	xcaddstr(win, CMDS_NOPE1, msgs(m_bang_no));
	waddstr(win, ") ? ");
	waddch(win, cmds(CMDS_NOPE1));
	waddch(win, '\b');
	wrefresh(win);
	while (TRUE)
	{
		c = xgetch(win);
		if (c == ERR)
			continue;
		if (c != KEY_MOUSE)
			break;
	}
	if (c == KEY_ESCAPE)
		return (-1);
	if (TO_LOWER(c) == cmds(CMDS_YUP1) || TO_LOWER(c) == cmds(CMDS_YUP2))
		return (0);
	return (1);
}

int yesno_msg (const char *mesg)
{
	werase(gbl(win_message));
	esc_msg();
	bangnc(mesg);
	return (yesno());
}

void do_beep(void)
{
	if (opt(beep_on_error))
		beep();
}

void dest_msg (int code)
{
	wmove(gbl(win_message), 0, 40);
	fk_msg(gbl(win_message), code, msgs(m_bang_copfil));
	wrefresh(gbl(win_message));
}

int check_read_only (const char *cmd)
{
	if (opt(read_only))
	{
		errmsgi(ER_ROM, cmd, ERR_ANY);
		return (TRUE);
	}

	switch ((gbl(scr_cur)->cur_root)->node_type)
	{
	case N_ARCH:
		errmsgi(ER_TARMOD, cmd, ERR_ANY);
		return (TRUE);
	}

	return (FALSE);
}

int check_tag_count (void)
{
	int c;

	if (!gbl(scr_cur)->dir_tagged_count)
	{
		c = errmsg(ER_NTF, "", ERR_ANY);
		return (c);
	}
	return (0);
}

void esc_msg (void)
{
	char *m1 = msgs(m_bang_esc);

	/*
	 * print "ESC cancel" at far right of window.
	 * We avoid the final char, since some terminals
	 * can't write there without scrolling.
	 * Maybe we should figure a global boolean in tcap
	 * somewhere for this.
	 */

	wmove(gbl(win_message), 0, getmaxx(gbl(win_message))-display_len(m1)-1);
	xcaddstr(gbl(win_message), CMDS_ESCAPE, m1);
}

void ret_ok (void)
{
	char *m1 = msgs(m_bang_esc);
	char *m2 = msgs(m_bang_ok);
	char *m3 = msgs(m_bang_ret);

	/*
	 * print "RET ok " before esc message above.
	 */

	wmove(gbl(win_message),
		0,
		getmaxx(gbl(win_message))-display_len(m1)-1-
			display_len(m2)-display_len(m3)-1);
	ret_msg(gbl(win_message));
	xcaddstr(gbl(win_message), CMDS_RETURN, m2);
}

static void left_arrowhead (WINDOW *win)
{
#if 0
	int left_arrowhead = 0x11;	/* must be a define for this somewhere */

	if (use_alt_char_sets && get_valid_acs(left_arrowhead))
	{
		wattron(win, A_ALTCHARSET);
		waddch(win, left_arrowhead);
		wattroff(win, A_ALTCHARSET);
	}
	else
#endif
	{
		waddch(win, '<');
	}
}

/*
 * print either RET or <-/
 * note: assumes RET is three chars long (note xlaters)
 * or at least same length as arrow.
 */

void ret_msg (WINDOW *win)
{
	wstandout(win);
	if (! opt(use_alt_char_sets))
	{
		xcaddstr(win, CMDS_RETURN, msgs(m_bang_ret));
	}
	else
	{
		setcode(win, CMDS_RETURN);
		left_arrowhead(win);
		wbox_chr(win, B_HO, B_SVSH);
		wbox_chr(win, B_BR, B_SVSH);
		setcode(win, 0);
	}
	wstandend(win);
}

void left_arrow (WINDOW *win)
{
	left_arrowhead(win);
	waddch(win, '-');
}

/*
 * this routine supposedly places the cursor in the top left of
 * pathline window above the active file or dir window
 *
 * We could turn off the cursor here (if the ability is available),
 * but we would lose the impact of being above the active window
 * in a multi-window env.  Maybe only turn off if single window?
 */

void position_cursor (void)
{
	if (gbl(scr_in_hexedit))
	{
		wmove(stdscr,
			getbegy(gbl(hxcb)->x_win) + getcury(gbl(hxcb)->x_win),
			getbegx(gbl(hxcb)->x_win) + getcurx(gbl(hxcb)->x_win));
	}
	else if (gbl(scr_in_diffview))
	{
		wmove(stdscr, 0, 0);
	}
	else
	{
		wmove(stdscr,
			getbegy(gbl(scr_cur)->cur_path_line) - getbegy(stdscr),
			getbegx(gbl(scr_cur)->cur_path_line) - getbegx(stdscr));
	}
}

void fk_msg (WINDOW *win, int code, const char *mesg)
{
	char buf[128];

	xcaddstr(win, code, fk_msg_str(code, mesg, buf));
}

char *fk_msg_str (int code, const char *mesg, char *buf)
{
	const char *p;
	char *b;

	b = buf;
	p = keyname(cmds(code));
	*b++ = *msgs(m_pgm_hilite);
	for (; *p; p++)
		*b++ = toupper(*p);
	*b++ = *msgs(m_pgm_hilite);
	*b++ = ' ';
	strcpy(b, mesg);

	return (buf);
}
