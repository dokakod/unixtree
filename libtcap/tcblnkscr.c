/*------------------------------------------------------------------------
 * screen-saver routine
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * static constants used in routine
 */
#define TCAP_SAVE_INTVL		5000		/* # of msecs to wait */
#define TCAP_MAX_RAND		0x7fff		/* random number mask */

/*------------------------------------------------------------------------
 * random function used
 */
#define tcap_random()		(rand() & TCAP_MAX_RAND)

/*------------------------------------------------------------------------
 * tcap_blank_screen() - blank the screen with a bouncing logo
 */
int tcap_blank_screen (TERMINAL *tp, const char **logo)
{
	int		y, x;
	int		old_y,  old_x;
	int		orig_y, orig_x;
	int		c;
	int		logo_width;
	int		logo_height;
	int		i;
	int		l;
	int		clock_was_running;
	int		cursor;
	int		do_color;

	if (tp == 0)
		return (-1);

#if 0
	/*--------------------------------------------------------------------
	 * If the strings exist to iconify the window,
	 * then reduce to an icon instead of blanking the window.
	 *
	 * The problem is that we do not know when we are
	 * uniconified, i.e. redisplayed.  At that time,
	 * we should clear any elapsed time counters.
	 */
	if (is_cmd_pres(tp->tcap->strs.icc))
	{
		tcap_outcmd(tp, S_ICC, tp->tcap->strs.icc);
		return (0);
	}
#endif

	/*--------------------------------------------------------------------
	 * check for user-supplied refresh routine
	 *
	 * If we don't have one, just bail since we cannot restore the screen.
	 */
	if (! tcap_async_check(tp->scrn, KEY_REFRESH))
		return (-1);

	/*--------------------------------------------------------------------
	 * cache current cursor position
	 */
	orig_x = tp->scrn->curr_col;
	orig_y = tp->scrn->curr_row;

	/*--------------------------------------------------------------------
	 * get number of lines & length of longest line in logo
	 */
	logo_height	= 0;
	logo_width	= 0;
	for (i=0; logo[i]; i++)
	{
		logo_height++;

		l = strlen(logo[i]);
		if (l > logo_width)
			logo_width = l;
	}

	/*--------------------------------------------------------------------
	 * turn off clock if running
	 */
	clock_was_running = term_clock_check();
	if (clock_was_running)
		term_clock_off();

	/*--------------------------------------------------------------------
	 * turn off cursor if possible
	 */
	cursor = tcap_curs_set(tp, VISIBILITY_OFF);

	/*--------------------------------------------------------------------
	 * check if display in color
	 */
	do_color = ((tp->scrn->curr_attr & A_COLOR) != 0);

	/*--------------------------------------------------------------------
	 * now clear the screen, display a bouncing logo, & wait for a keypress
	 */
	old_y = old_x = -1;

	while (TRUE)
	{
		/*----------------------------------------------------------------
		 * if in color, choose a random color
		 */
		if (do_color)
		{
			/*------------------------------------------------------------
			 * random color between (1-(NUM_FG_FND-1))
			 */
			int f = ((tcap_random() *
				(tp->tcap->ints.max_fg - 1)) / TCAP_MAX_RAND) + 1;
			int b = tp->scrn->disp_in_window ? COLOR_LTWHITE : COLOR_BLACK;

			tcap_outattr(tp, A_CLR(f, b), ' ');
		}

		/*----------------------------------------------------------------
		 * clear either the whole screen or just our logo
		 */
		if (old_x == -1)
		{
			/*------------------------------------------------------------
			 * first time: clear the whole screen
			 */
			tcap_clear_screen(tp);
		}
		else
		{
			/*------------------------------------------------------------
			 * If we have a CL cmd, it is quicker to use
			 */
			if (is_cmd_pres(tp->tcap->strs.cl))
			{
				tcap_outcmd(tp, S_CL, tp->tcap->strs.cl);
			}
			else
			{
				for (i=0; i<logo_height; i++)
				{
					l = strlen(logo[i]);
					tcap_outpos(tp, old_y+i, old_x+(logo_width-l)/2);
					for (; l; l--)
						tcap_outc(tp, ' ');
				}
			}
		}

		/*----------------------------------------------------------------
		 * get random (y, x) to display logo at
		 */
		y = (tcap_random() *
			(tp->tcap->ints.maxrows - (logo_height-1))) / TCAP_MAX_RAND;
		x = (tcap_random() *
			(tp->tcap->ints.maxcols - (logo_width -1))) / TCAP_MAX_RAND;

		/*----------------------------------------------------------------
		 * now display our logo at that position
		 */
		for (i=0; i<logo_height; i++)
		{
			l = strlen(logo[i]);
			tcap_outpos(tp, y+i, x+(logo_width-l)/2);
			tcap_outs(tp, logo[i]);
		}

		tcap_outflush(tp);

		/*----------------------------------------------------------------
		 * cache this position for next time around
		 */
		old_y = y;
		old_x = x;

		/*----------------------------------------------------------------
		 * now wait for a key-press & break if we got one
		 */
		c = tcap_kbd_get_key(tp, TCAP_SAVE_INTVL, TRUE, TRUE, TRUE);
		if (c != -1)
			break;
	}

	/*--------------------------------------------------------------------
	 * call user-specified refresh routine
	 */
	tcap_async_process(tp->scrn, KEY_REFRESH);

	/*--------------------------------------------------------------------
	 * restore cursor state
	 */
	tcap_curs_set(tp, cursor);

	/*--------------------------------------------------------------------
	 * turn clock back on if it was running
	 */
	if (clock_was_running)
		term_clock_on();

	/*--------------------------------------------------------------------
	 * reset original cursor position
	 */
	if (orig_x != -1 && orig_y != -1)
	{
		tcap_outpos(tp, orig_y, orig_x);
		tcap_outflush(tp);
	}

	return (0);
}
