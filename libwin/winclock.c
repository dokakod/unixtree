/*------------------------------------------------------------------------
 * clock routines
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * win_clock() - initialize clock processing
 */
int win_clock (WINDOW *win, WIN_CLOCK_RTN *rtn, void *data, int interval)
{
	SCREEN *	s;
	WIN_CLOCK *	wc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * store all args in the window
	 */
	s	= WIN_SCREEN(win);
	wc	= SCR_CLOCK(s);

	wc->clk_win		= win;
	wc->clk_rtn		= rtn;
	wc->clk_data	= data;
	wc->clk_intvl	= interval;
	wc->clk_defer	= 0;

	/*--------------------------------------------------------------------
	 * start/stop clock processing
	 */
	if (rtn != 0)
	{
		term_clock_init((interval * 1000), win_clock_process, win_screens);
	}
	else
	{
		term_clock_clear();
	}

	return (OK);
}

/*------------------------------------------------------------------------
 * win_clock_set() - turn the clock on/off
 */
void win_clock_set (int flag)
{
	term_clock_set(flag);
}

/*------------------------------------------------------------------------
 * win_clock_check() - check if a clock update is needed
 */
void win_clock_check (void)
{
	if (term_clock_check())
		term_clock_trip();
}

/*------------------------------------------------------------------------
 * win_clock_active() - set clock active or not
 */
int win_clock_active (int bf)
{
	return term_clock_active(bf);
}

/*------------------------------------------------------------------------
 * win_clock_process() - process clocks on all screens
 */
void win_clock_process (time_t clock, void *data)
{
	SCRLIST *	screens	= (SCRLIST *)data;
	SCRLIST *	sl;

	for (sl=screens; sl; sl=sl->next)
	{
		SCREEN *	s	= sl->screen;
		WIN_CLOCK *	wc	= SCR_CLOCK(s);

		if (wc->clk_win != 0)
			win_clock_show(clock, wc->clk_win);
	}
}

/*------------------------------------------------------------------------
 * win_clock_show() - the actual clock display routine
 */
void win_clock_show (time_t clock, WINDOW *win)
{
	SCREEN *	s;
	TERMINAL *	t;
	WIN_CLOCK *	wc;
	int			row;
	int			col;
	int			mo, dy, yr;
	int			hh, mm, ss;
	int			wday;
	int			yday;
	struct tm *	tmptr;

	/*--------------------------------------------------------------------
	 * if no clock window, just bail
	 */
	if (win == 0)
		return;

	s	= WIN_SCREEN(win);
	t	= SCR_TERM(s);
	wc	= SCR_CLOCK(s);

	/*--------------------------------------------------------------------
	 * if currently doing a refresh, just defer it
	 */
	if (s->in_refresh)
	{
		wc->clk_defer = clock;
		return;
	}

	/*--------------------------------------------------------------------
	 * remember current location of cursor
	 */
	row = getcury(WIN_TRMSCR(win));
	col = getcurx(WIN_TRMSCR(win));

	/*--------------------------------------------------------------------
	 * display the clock info
	 */
	werase(win);
	if (clock)
	{
		/*----------------------------------------------------------------
		 * get yr/mo/da/hr/mn/ss
		 */
		tmptr	= localtime(&clock);
		yr		= tmptr->tm_year + 1900;
		mo		= tmptr->tm_mon + 1;
		dy		= tmptr->tm_mday;
		hh		= tmptr->tm_hour;
		mm		= tmptr->tm_min;
		ss		= tmptr->tm_sec;
		wday	= tmptr->tm_wday;
		yday	= tmptr->tm_yday;

		/*----------------------------------------------------------------
		 * adjust seconds to be a multiple of the interval
		 */
		if (wc->clk_intvl < 60)
		{
			ss = (ss / wc->clk_intvl) * wc->clk_intvl;
		}
		else
		{
			ss = 0;
		}

		/*----------------------------------------------------------------
		 * call user's display routine
		 */
		(wc->clk_rtn)(win, wc->clk_data, yr, mo, dy, hh, mm, ss, wday, yday);
	}
	wrefresh(win);

	/*--------------------------------------------------------------------
	 * indicate no clock defer
	 */
	wc->clk_defer = 0;

	/*--------------------------------------------------------------------
	 * restore original cursor position
	 */
	tcap_outpos(t, row, col);
	tcap_outflush(t);

	setcury(SCR_TRMSCR(s), row);
	setcurx(SCR_TRMSCR(s), col);
}

/*------------------------------------------------------------------------
 * win_key_timer() - process an async KEY_TIMER key
 */
int win_key_timer (int key, void *data)
{
	win_clock_check();

	return (OK);
}
