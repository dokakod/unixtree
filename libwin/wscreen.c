/*------------------------------------------------------------------------
 * screen handling routines
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * screen_add() - internal routine to add a screen to the screen list
 */
static void screen_add (SCREEN *s)
{
	SCRLIST *	sl;

	if (win_screen_cs == 0)
	{
		win_screen_cs = sysmutex_alloc();
	}

	sl = (SCRLIST *)MALLOC(sizeof(*sl));
	if (sl != 0)
	{
		sysmutex_enter(win_screen_cs);
		{
			sl->prev	= 0;
			sl->next	= win_screens;
			sl->screen	= s;

			win_screens = sl;
		}
		sysmutex_leave(win_screen_cs);
	}
}

/*------------------------------------------------------------------------
 * screen_del() - internal routine to delete a screen from the screen list
 */
static void screen_del (SCREEN *s)
{
	SCRLIST *	sl;

	if (win_screen_cs == 0)
	{
		win_screen_cs = sysmutex_alloc();
	}

	sysmutex_enter(win_screen_cs);
	{
		for (sl=win_screens; sl; sl=sl->next)
		{
			if (sl->screen == s)
				break;
		}

		if (sl != 0)
		{
			if (sl->next != 0)
				(sl->next)->prev = sl->prev;

			if (sl->prev != 0)
				(sl->prev)->next = sl->next;

			if (sl == win_screens)
				win_screens = sl->next;

			FREE(sl);
		}
	}
	sysmutex_leave(win_screen_cs);

	if (win_screens == 0)
	{
		sysmutex_free(win_screen_cs);
		win_screen_cs = 0;
	}
}

/*------------------------------------------------------------------------
 * initwins() - internal routine to create standard windows
 */
static int initwins (SCREEN *s)
{
	attr_t	a;
	int		top;
	int		bot;
	int		slk;

	/*--------------------------------------------------------------------
	 * clear all window pointers
 	 * Note that this may leave dangling pointers to existing windows.
	 * This will happen if the user asks to resize a screen in which
	 * stdscr has existing sub-windows.
	 */
	s->stdscr_ptr = 0;
	s->curscr_ptr = 0;
	s->trmscr_ptr = 0;

	/*--------------------------------------------------------------------
	 * create trmscr
	 * This window is the size of the screen.
	 */
	s->trmscr_ptr = scrn_newwin(s, 0, 0, 0, 0);
	if (s->trmscr_ptr == 0)
	{
		return (ERR);
	}

	a = wattrget(s->trmscr_ptr);
	win_clear(s->trmscr_ptr, a);

	/*--------------------------------------------------------------------
	 * create curscr
	 * This window is the size of the screen.
	 */
	s->curscr_ptr = scrn_newwin(s, 0, 0, 0, 0);
	if (s->curscr_ptr == 0)
	{
		delwin(s->trmscr_ptr);
		return (ERR);
	}

	werase(s->curscr_ptr);
	clearok(s->curscr_ptr, TRUE);

	/*--------------------------------------------------------------------
	 * create stdscr
	 * This window is the size of the screen less any ripoff & slk windows.
	 */
	top	= s->ripoff_tbl.num_top;
	bot	= s->ripoff_tbl.num_bot;

	switch (s->slk_data.layout_fmt)
	{
	case SLK_LAYOUT_FMT_1_323:
	case SLK_LAYOUT_FMT_1_44:
		slk = 1;
		break;

	case SLK_LAYOUT_FMT_2_323:
	case SLK_LAYOUT_FMT_2_44:
	case SLK_LAYOUT_FMT_2_33:
	case SLK_LAYOUT_FMT_2_222:
		slk = 2;
		break;

	default:
		slk = 0;
		break;
	}

	s->stdscr_ptr = scrn_newwin(s,
		getmaxy(s->trmscr_ptr) - (top + bot + slk), 0, top, 0);
	if (s->stdscr_ptr == 0)
	{
		delwin(s->curscr_ptr);
		delwin(s->trmscr_ptr);
		return (ERR);
	}

	wattrset(s->stdscr_ptr, 0);
	werase(s->stdscr_ptr);

	/*--------------------------------------------------------------------
	 * process all ripoff lines
	 */
	top = 0;
	bot = getmaxy(s->trmscr_ptr);

	if (s->ripoff_tbl.num_ents > 0)
	{
		int	i;

		for (i=0; i<s->ripoff_tbl.num_ents; i++)
		{
			RIPOFF_ENTRY *	r	= s->ripoff_tbl.entries + i;

			if (r->init != 0)
			{
				int y;
				int n;

				if (r->win != 0)
					delwin(r->win);

				if (r->line < 0)
				{
					n    = -(r->line);
					y    = bot - n;
					bot -= n;
				}
				else
				{
					n    =  (r->line);
					y    = top;
					top += n;
				}
				r->win = scrn_newwin(s, n, 0, y, 0);
				if (r->win != 0)
					(*r->init)(r->win, getmaxx(r->win));
			}
		}
	}

	/*--------------------------------------------------------------------
	 * now process any slk data
	 */
	if (slk)
	{
		if (s->slk_data.win != 0)
			delwin(s->slk_data.win);

		s->slk_data.win = scrn_newwin(s, slk, 0, bot - slk, 0);
		if (s->slk_data.win != 0)
			scrn_slk_setup(s);
	}

	return (OK);
}

/*------------------------------------------------------------------------
 * resize_screen() - resize screen windows to new screen size
 */
int resize_screen (void)
{
	return scrn_resize_screen(win_cur_screen);
}

/*------------------------------------------------------------------------
 * scrn_resize_screen() - resize screen windows to new screen size
 */
int scrn_resize_screen (SCREEN *s)
{
	int rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (s == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * delete standard windows
	 *
	 * Note that this may not delete the stdscr if the user has
	 * sub-windows of it existing, but that is his problem.
	 * We don't want to delete his sub-windows here, since he will then
	 * have bogus window pointers.
	 * curscr & trmscr *should* not have any sub-windows (maybe we
	 * should enforce this?).
	 */
	delwin(s->stdscr_ptr);
	delwin(s->curscr_ptr);
	delwin(s->trmscr_ptr);

	/*--------------------------------------------------------------------
	 * Now re-create the standard windows.
	 * Note that this will delete & re-create all ripoff & slk windows.
	 */
	rc = initwins(s);

	return (rc);
}

/*------------------------------------------------------------------------
 * set_term() - set a new screen as the active screen
 */
SCREEN * set_term (SCREEN *s)
{
	SCREEN *osp = win_cur_screen;

	/*--------------------------------------------------------------------
	 * if this screen is the current screen, we're done
	 */
	if (s == osp)
		return (osp);

	/*--------------------------------------------------------------------
	 * if old screen existed, turn it off
	 */
	if (osp != 0)
	{
		tcap_sig_end(osp->term);
	}

	/*--------------------------------------------------------------------
	 * now set new current screen & turn it on if it exists
	 */
	if (s != 0)
	{
		win_cur_screen	= s;
		win_cur_term	= s->term;

		tcap_sig_init(s->term);
	}
	else
	{
		win_cur_screen	= 0;
		win_cur_term	= 0;
	}

	/*--------------------------------------------------------------------
	 * return old current-screen
	 */
	return (osp);
}

/*------------------------------------------------------------------------
 * get_term() - get a pointer to the current screen
 */
SCREEN * get_term (void)
{
	return (win_cur_screen);
}

/*------------------------------------------------------------------------
 * new_term() - create a new screen object using streams
 */
SCREEN * newterm (const char *term, FILE *out_fp, FILE *inp_fp)
{
	int	inp_fd = (inp_fp == 0 ? -1 : fileno(inp_fp));
	int	out_fd = (out_fp == 0 ? -1 : fileno(out_fp));

	return newscreen(term, out_fd, inp_fd);
}

/*------------------------------------------------------------------------
 * newscreen() - create a new screen object using file descriptors
 */
SCREEN * newscreen (const char *term, int out_fd, int inp_fd)
{
	return newscreen_defs(term, 0, out_fd, inp_fd);
}

SCREEN * newscreen_defs (const char *term, const char **defs,
	int out_fd, int inp_fd)
{
	SCREEN *	s;
	TERMINAL *	tp;
	int			rc;

	/*--------------------------------------------------------------------
	 * first create the TERMINAL
	 */
	tp = tcap_newscreen(term, defs, out_fd, inp_fd, 0);
	if (tp == 0)
		return (0);

	/*--------------------------------------------------------------------
	 * do all terminal initializations
	 */
	rc = tcap_cinit(tp);
	if (rc)
	{
		tcap_delterminal(tp);
		return (0);
	}

	/*--------------------------------------------------------------------
	 * now create the SCREEN
	 */
	s = (SCREEN *)MALLOC(sizeof(*s));
	if (s == 0)
	{
		tcap_delterminal(tp);
		return (0);
	}

	/*--------------------------------------------------------------------
	 * now fill in the SCREEN
	 */
	memset(s, 0, sizeof(*s));
	memcpy(&s->ripoff_tbl, &win_ripoff_tbl, sizeof(s->ripoff_tbl));

	s->in_curses	= TRUE;
	s->in_refresh	= FALSE;
	s->do_echo		= FALSE;
	s->do_inp_nl	= FALSE;
	s->do_out_nl	= FALSE;
	s->stdscr_ptr	= 0;
	s->curscr_ptr	= 0;
	s->trmscr_ptr	= 0;
	s->winlist		= 0;
	s->term			= tp;
	s->usrdata		= 0;

	/*--------------------------------------------------------------------
	 * initialize any slk data
	 */
	scrn_slk_init(s, win_slk_fmt);

	/*--------------------------------------------------------------------
	 * create windows
	 */
	rc = initwins(s);
	if (rc)
	{
		delscreen(s);
		return (0);
	}

	/*--------------------------------------------------------------------
	 * setup standard async keys
	 */
	tcap_kbd_add_async(tp, KEY_REFRESH,      win_key_repaint,	s);
	tcap_kbd_add_async(tp, KEY_TIMER,        win_key_timer,		s);
	tcap_kbd_add_async(tp, KEY_PRINT_SCREEN, win_key_prtscrn,	s);
	tcap_kbd_add_async(tp, KEY_SNAP,         win_key_snap,		s);

	/*--------------------------------------------------------------------
	 * now add this screen to the screen list
	 */
	screen_add(s);

	/*--------------------------------------------------------------------
	 * set this screen as the current
	 */
	set_term(s);

	/*--------------------------------------------------------------------
	 * now clear out slk & ripoff data for next time
	 */
	memset(&win_ripoff_tbl, 0, sizeof(win_ripoff_tbl));
	win_slk_fmt = SLK_LAYOUT_FMT_NONE;

	return (s);
}

/*------------------------------------------------------------------------
 * delscreen() - delete a SCREEN object
 */
void delscreen (SCREEN *s)
{
	int	was_curscrn;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (s == 0)
		return;

	/*--------------------------------------------------------------------
	 * check if this was the current screen
	 */
	was_curscrn = (s == win_cur_screen);

	/*--------------------------------------------------------------------
	 * now take it out of the screen list
	 */
	screen_del(s);

	/*--------------------------------------------------------------------
	 * if this was the current screen, set new one
	 */
	if (was_curscrn)
	{
		if (win_screens == 0)
		{
			set_term(0);
		}
		else
		{
			set_term(win_screens->screen);
		}
	}

	/*--------------------------------------------------------------------
	 * now actually delete the screen
	 */
	{
		/*----------------------------------------------------------------
		 * delete the TERMINAL struct
		 */
		tcap_delterminal(s->term);

		/*----------------------------------------------------------------
		 * delete all WINDOWS
		 *
		 * Note that this may take many iterations over the WINDOW list,
		 * since you can only delete a window which has no sub-wins.
		 */
		while (s->winlist != 0)
		{
			WINLIST *	wl;
			WINLIST *	wn;

			for (wl=s->winlist; wl; wl=wn)
			{
				WINDOW *	w = wl->win;

				wn = wl->next;

				if (w->_ndescs == 0)
					delwin(w);
			}
		}

		/*----------------------------------------------------------------
		 * check if user data should be deleted
		 */
		if (s->usrdata != 0 && s->data_on_heap)
		{
			FREE(s->usrdata);
		}

		/*----------------------------------------------------------------
		 * now free the SCREEN itself
		 */
		FREE(s);
	}
}

/*------------------------------------------------------------------------
 * set_usr_data() - store a pointer user data into a SCREEN
 */
void * set_usr_data (SCREEN *s, void *usrdata, int on_heap)
{
	void *	olddata	= 0;

	if (s != 0)
	{
		olddata = s->usrdata;
		if (s->data_on_heap)
			FREE(olddata);

		s->usrdata		= usrdata;
		s->data_on_heap	= on_heap;
	}

	return (olddata);
}

/*------------------------------------------------------------------------
 * get_usr_data() - get pointer to user data in a SCREEN
 */
void * get_usr_data (SCREEN *s)
{
	return (s == 0 ? 0 : s->usrdata);
}

/*------------------------------------------------------------------------
 * scrn_stdscr() - get stdscr pointer
 */
WINDOW * scrn_stdscr (SCREEN *s)
{
	return (s == 0 ? 0 : SCR_STDSCR(s));
}

/*------------------------------------------------------------------------
 * scrn_curscr() - get curscr pointer
 */
WINDOW * scrn_curscr (SCREEN *s)
{
	return (s == 0 ? 0 : SCR_CURSCR(s));
}

/*------------------------------------------------------------------------
 * scrn_trmscr() - get trmscr pointer
 */
WINDOW * scrn_trmscr (SCREEN *s)
{
	return (s == 0 ? 0 : SCR_TRMSCR(s));
}

/*------------------------------------------------------------------------
 * get_stdscr() - get stdscr pointer
 */
WINDOW * get_stdscr (void)
{
	return scrn_stdscr(win_cur_screen);
}

/*------------------------------------------------------------------------
 * get_curscr() - get curscr pointer
 */
WINDOW * get_curscr (void)
{
	return scrn_curscr(win_cur_screen);
}

/*------------------------------------------------------------------------
 * get_trmscr() - get trmscr pointer
 */
WINDOW * get_trmscr (void)
{
	return scrn_trmscr(win_cur_screen);
}

/*------------------------------------------------------------------------
 * get_screen() - get screen pointer for a window
 */
SCREEN * get_screen (WINDOW *win)
{
	if (win == 0)
		return (0);

	return (win->_screen);
}
