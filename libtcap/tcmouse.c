/*------------------------------------------------------------------------
 * mouse routines
 *
 * Note: all these routines assume a valid TERMINAL pointer
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * defines
 */
#define MOUSE_STAT(tp)		( term_screen_dev_mouse(tp->term) )

/*------------------------------------------------------------------------
 * tcap_mouse_init() - initialize mouse processing
 */
int tcap_mouse_init (TERMINAL *tp)
{
	MOUSE_STATUS *	mp;
	int				rc;

	if (tp == 0)
		return (-1);

	if (! tcap_mouse_get_display(tp))
	{
		return (-1);
	}

	TERMINAL_LOCK(tp);
	{
		mp = MOUSE_STAT(tp);

		if (! mp->is_init)
		{
			int i;

			if (tp->tcap->ints.nb > NUM_BUTTONS)
				tp->tcap->ints.nb = NUM_BUTTONS;

			for (i=0; i<NUM_BUTTONS; i++)
			{
				mp->button[i]			= BUTTON_UP;
				mp->btime[i].tv_sec		= 0;
				mp->btime[i].tv_usec	= 0;
			}

			mp->is_on				= FALSE;
			mp->dbl_clk_interval	= tp->tcap->ints.dblclk_intvl;
			mp->y					= -1;
			mp->x					= -1;
			mp->old_y				= -1;
			mp->old_x				= -1;
			mp->is_init				= TRUE;

			rc = tcap_mouse_show(tp);
		}
		else
		{
			rc = 0;
		}
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_mouse_end() - end mouse processing
 */
int tcap_mouse_end (TERMINAL *tp)
{
	MOUSE_STATUS *mp;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		mp = MOUSE_STAT(tp);
		if (mp->is_on)
			tcap_mouse_hide(tp);
		mp->is_init = FALSE;
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_mouse_show() - turn the mouse on
 */
int tcap_mouse_show (TERMINAL *tp)
{
	MOUSE_STATUS *mp;

	if (tp == 0)
		return (-1);

	if (! tcap_mouse_get_display(tp))
	{
		return (-1);
	}

	TERMINAL_LOCK(tp);
	{
		mp = MOUSE_STAT(tp);
		if (! mp->is_on)
		{
			mp->is_on = TRUE;
			tcap_outcmd(tp, S_MBEG, tp->tcap->strs.mbeg);
			tcap_outflush(tp);
		}
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_mouse_hide() - turn the mouse off
 */
int tcap_mouse_hide (TERMINAL *tp)
{
	MOUSE_STATUS *mp;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		mp = MOUSE_STAT(tp);
		if (mp->is_on)
		{
			mp->is_on = FALSE;
			tcap_outcmd(tp, S_MEND, tp->tcap->strs.mend);
			tcap_outflush(tp);
		}
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_mouse_get_is_on() - check whether mouse is on or off
 */
int tcap_mouse_get_is_on (const TERMINAL *tp)
{
	MOUSE_STATUS *mp;

	if (tp == 0)
		return (FALSE);

	mp = MOUSE_STAT(tp);

	return (mp->is_on);
}

/*------------------------------------------------------------------------
 * tcap_mouse_invalidate() - invalidate mouse position
 */
int tcap_mouse_invalidate (TERMINAL *tp)
{
	MOUSE_STATUS *mp;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		mp = MOUSE_STAT(tp);
		mp->y		= -1;
		mp->x		= -1;
		mp->old_y	= -1;
		mp->old_x	= -1;
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_mouse_get_display() - check whether mouse should be displayed
 */
int tcap_mouse_get_display (const TERMINAL *tp)
{
	if (tp == 0)
		return (FALSE);

	return (tp->tcap->bools.md &&
		is_cmd_pres(tp->tcap->strs.mbeg) &&
	    is_cmd_pres(tp->tcap->strs.mend) &&
	    is_cmd_pres(tp->tcap->strs.mstr) );
}

/*------------------------------------------------------------------------
 * tcap_mouse_set_dbl_clk_intvl() - set double click interval
 */
int tcap_mouse_set_dbl_clk_intvl (TERMINAL *tp, int ms)
{
	MOUSE_STATUS *mp;

	if (tp == 0)
		return (-1);

	if (ms <= 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		mp = MOUSE_STAT(tp);
		mp->dbl_clk_interval = ms;
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_mouse_get_event() - get mouse event
 */
int tcap_mouse_get_event (const TERMINAL *tp)
{
	MOUSE_STATUS *mp;

	if (tp == 0)
		return (-1);

	mp = MOUSE_STAT(tp);

	return (mp->event);
}

/*------------------------------------------------------------------------
 * tcap_mouse_set_event() - set event types we are interested in
 */
int tcap_mouse_set_event (TERMINAL *tp, int event)
{
	MOUSE_STATUS *mp;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		mp = MOUSE_STAT(tp);
		mp->event = event;
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_mouse_get_x() - get current mouse x position
 */
int tcap_mouse_get_x (const TERMINAL *tp)
{
	MOUSE_STATUS *mp;

	if (tp == 0)
		return (-1);

	mp = MOUSE_STAT(tp);

	return (mp->x);
}

/*------------------------------------------------------------------------
 * tcap_mouse_get_y() - get current mouse y position
 */
int tcap_mouse_get_y (const TERMINAL *tp)
{
	MOUSE_STATUS *mp;

	if (tp == 0)
		return (-1);

	mp = MOUSE_STAT(tp);

	return (mp->y);
}

/*------------------------------------------------------------------------
 * tcap_mouse_get_buttons() - get current mouse button mask
 */
int tcap_mouse_get_buttons (const TERMINAL *tp)
{
	MOUSE_STATUS *mp;
	int i;
	int c;

	if (tp == 0)
		return (-1);

	mp = MOUSE_STAT(tp);

	c = 0;
	for (i=0; i<NUM_BUTTONS; i++)
	{
		if (mp->button[i])
			c |= (1 << i);
	}

	return (c);
}

/*------------------------------------------------------------------------
 * tcap_mouse_get_still_down() - check if any btn is still down (drag detect)
 */
int tcap_mouse_get_still_down (const TERMINAL *tp)
{
	MOUSE_STATUS *mp;
	int i;

	if (tp == 0)
		return (-1);

	mp = MOUSE_STAT(tp);

	for (i=0; i<NUM_BUTTONS; i++)
	{
		if (mp->button[i])
			return (TRUE);
	}

	return (FALSE);
}

/*------------------------------------------------------------------------
 * tcap_mouse_check_btn() - check for double click for a button
 */
int tcap_mouse_check_btn (TERMINAL *tp, int i)
{
	MOUSE_STATUS *mp;
	TIME_VAL now;
	long secs;
	long usecs;
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		mp = MOUSE_STAT(tp);

		term_timeval(&now);

		secs  = (mp->dbl_clk_interval / 1000);
		usecs = (mp->dbl_clk_interval % 1000) * 1000;

		rc = FALSE;
		if ((now.tv_sec - mp->btime[i].tv_sec) < secs)
		{
			rc = TRUE;
		}
		else if ((now.tv_sec  - mp->btime[i].tv_sec ) ==  secs &&
			     (now.tv_usec - mp->btime[i].tv_usec) <= usecs)
		{
			rc = TRUE;
		}

		if (rc)
		{
			if ((mp->x != mp->old_x) ||
				(mp->y != mp->old_y))
			{
				rc = FALSE;
			}
		}

		mp->btime[i].tv_sec		= now.tv_sec;
		mp->btime[i].tv_usec	= now.tv_usec;
		mp->old_x				= mp->x;
		mp->old_y				= mp->y;
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_mouse_get_input() - process mouse input string
 */
int tcap_mouse_get_input (TERMINAL *tp, const unsigned char *input)
{
	MOUSE_STATUS *			mp;
	const unsigned char *	str;
	const char *			tmplate;
	int						args[3];
	int						i;
	int						base_offset = 0;
	int						reverse	= 0;
	int						button_change;
	int						button_is_down;
	int						bi;
	int						x;
	int						y;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0)
		return (-1);

	mp = MOUSE_STAT(tp);

	/*--------------------------------------------------------------------
	 * check if we have a non-empty mouse-input string defined
	 */
	if (! is_cmd_pres(tp->tcap->strs.mstr))
		return (0);

	if (*tp->tcap->strs.mstr == 0)
		return (0);

	/*--------------------------------------------------------------------
	 *	convert string to args[]
	 *	tmplate will be something like "\e[M%+ %+ %+ "
	 *	args[0] = button info
	 *	args[1] = x (or y)
	 *	args[2] = y (or x)
	 */
	tmplate = tp->tcap->strs.mstr;

	str = input;
	args[0] = args[1] = args[2] = 0;

	for (i=0; *str && *tmplate; tmplate++)
	{
		if (*tmplate == '%')
		{
			tmplate++;
			switch (*tmplate)
			{
			case '+':
				tmplate++;
				args[i] = *str++ - *tmplate;
				i++;
				break;

			case 'd':
				if (isdigit(*str))
				{
					while (isdigit(*str))
					{
						args[i] = args[i] * 10 + *str - '0';
						str++;
					}
					i++;
					break;
				}
				else
				{
					return (0);
				}

			case 'i':
				base_offset = 1;
				break;

			case 'r':
				reverse = 1;
				break;

			case 'c':
				args[i] = *str++;
				i++;
				break;
			}
		}
		else
		{
			if (*str != *tmplate)
				return (0);
			str++;
		}
	}

	/*--------------------------------------------------------------------
	 * check if tmplate used up
	 */
	if (*tmplate)
		return (-1);

	/*--------------------------------------------------------------------
	 *	If (btn-info & 0x80) == 0, then we have input in "xterm" mode.
	 *	If (btn-info & 0x80) != 0, then we have input in "xvt"   mode.
	 *
	 *	1.	xterm mode:
	 *		Button info is encoded as:
	 *
	 *			bits 0-1	(0x03)	button info:
	 *
	 *							0	button 1 pressed
	 *							1	button 2 pressed
	 *							2	button 3 pressed
	 *							3	some button was released (no info on which)
	 *
	 *			bit  2		(0x04)	shift is down
	 *			bit  3		(0x08)	meta (alt) is down
	 *			bit  4		(0x10)	control is down
	 *			bit  5		(0x20)	reserved (for + ' ' encoding)
	 *			bit  6		(0x40)	always 0
	 *			bit  7		(0x80)	always 0
	 *	
	 *	2.	xvt mode:
	 *		Button info is encoded as:
	 *
	 *			bit  0		(0x01)	button 1 state (0 = dn, 1 = up)
	 *			bit  1		(0x02)	button 2 state (0 = dn, 1 = up)
	 *			bit  2		(0x04)	button 3 state (0 = dn, 1 = up)
	 *			bit  3		(0x08)	1 = scroll-wheel up one notch
	 *			bit  4		(0x10)	1 = scroll-wheel dn one notch
	 *			bit  5		(0x20)	reserved (for + ' ' encoding)
	 *			bit  6		(0x40)	1 = no button change, motion only
	 *			bit  7		(0x80)	always 1
	 *
	 *	Note that in either case, only one button's worth of change
	 *	is sent at a time, although the x-y position may change.
	 */
	button_is_down	= tcap_mouse_get_still_down(tp);
	button_change	= -1;
	bi				= args[0];

	TERMINAL_LOCK(tp);
	{
		if (bi & 0x80)
		{
			/*------------------------------------------------------------
			 * "xvt" mode
			 */
			if ((bi & 0x40) == 0)
			{
				if      (bi & (1 << BUTTON_SCRLUP))
				{
					button_change = MOUSE_SCROLLUP;
				}
				else if (bi & (1 << BUTTON_SCRLDN))
				{
					button_change = MOUSE_SCROLLDN;
				}
				else
				{
					for (i=0; i<NUM_BUTTONS; i++)
					{
						if (bi & (1 << i))
						{
							if (! mp->button[i])
							{
								mp->button[i] = BUTTON_DOWN;
								if (tcap_mouse_check_btn(tp, i))
									button_change = term_button_dbl[i];
								else
									button_change = term_button_down[i];
								break;
							}
						}
						else
						{
							if (mp->button[i])
							{
								mp->button[i] = BUTTON_UP;
								button_change = term_button_up[i];
								break;
							}
						}
					}
				}
			}
		}
		else
		{
			/*------------------------------------------------------------
			 * "xterm" mode
			 */
			button_change = -1;
			i = args[0];
			if (i < NUM_BUTTONS)
			{
				if (tp->tcap->ints.nb == 2 && i == BUTTON_MIDDLE)
					i = BUTTON_RIGHT;
				mp->button[i] = BUTTON_DOWN;
				if (tcap_mouse_check_btn(tp, i))
					button_change = term_button_dbl[i];
				else
					button_change = term_button_down[i];
				mp->saved_buttons = i;
			}
			else
			{
				if (mp->saved_buttons < NUM_BUTTONS)
				{
					mp->button[mp->saved_buttons] = BUTTON_UP;
					button_change = term_button_up[mp->saved_buttons];
				}
				else
				{
					for (i=0; i<NUM_BUTTONS; i++)
					{
						if (mp->button[i])
						{
							mp->button[i] = BUTTON_UP;
							button_change = term_button_up[i];
							break;
						}
					}
				}
				mp->saved_buttons = args[0];
			}
		}

		/*----------------------------------------------------------------
		 * adjust x & y positions and offsets
		 */
		x = args[1+reverse] - base_offset;
		y = args[2-reverse] - base_offset;

		/*----------------------------------------------------------------
		 *	At this point, either x-y has changed, a button changed, or both.
		 *	If both, queue the button changed info & return x-y change.
		 *	Else return what happened.
		 */
		if (x != mp->x || y != mp->y)
		{
			if (button_change != -1)
			{
				mp->queued_event = button_change;
				tcap_kbd_enq_ta(tp, KEY_MOUSE, TRUE);
			}

			mp->x = x;
			mp->y = y;
			if (button_is_down)
				mp->event = MOUSE_DRAG;
			else
				mp->event = MOUSE_MOVE;
		}
		else
		{
			mp->event = button_change;
		}
	}
	TERMINAL_UNLOCK(tp);

	return (KEY_MOUSE);
}

/*------------------------------------------------------------------------
 * tcap_mouse_deq_event() - dequeue last mouse event
 */
int tcap_mouse_deq_event (TERMINAL *tp)
{
	MOUSE_STATUS *mp;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		mp = MOUSE_STAT(tp);

		if (mp->queued_event)
		{
			mp->event = mp->queued_event;
			mp->queued_event = 0;
		}
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_mouse_check_position() - position mouse if position unknown
 */
int tcap_mouse_check_position (TERMINAL *tp)
{
	MOUSE_STATUS *mp;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		mp = MOUSE_STAT(tp);

		if (mp->y == -1 || mp->x == -1)
		{
			mp->y = tp->tcap->ints.maxrows / 2;
			mp->x = tp->tcap->ints.maxcols / 2;
		}
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_mouse_move_up() - move mouse up
 */
int tcap_mouse_move_up (TERMINAL *tp)
{
	MOUSE_STATUS *	mp;
	int				rc;

	if (tp == 0)
		return (-1);

	mp = MOUSE_STAT(tp);

	tcap_mouse_check_position(tp);

	TERMINAL_LOCK(tp);
	{
		if (mp->y > 0)
		{
			mp->y--;
			if (tcap_mouse_get_still_down(tp))
				mp->event = MOUSE_DRAG;
			else
				mp->event = MOUSE_MOVE;
			mp->queued_event = 0;

			rc = 0;
		}
		else
		{
			rc = -1;
		}
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_mouse_move_down() - move mouse down
 */
int tcap_mouse_move_down (TERMINAL *tp)
{
	MOUSE_STATUS *	mp;
	int				rc;

	if (tp == 0)
		return (-1);

	mp = MOUSE_STAT(tp);

	tcap_mouse_check_position(tp);

	TERMINAL_LOCK(tp);
	{
		if (mp->y < (tp->tcap->ints.maxrows-1))
		{
			mp->y++;
			if (tcap_mouse_get_still_down(tp))
				mp->event = MOUSE_DRAG;
			else
				mp->event = MOUSE_MOVE;
			mp->queued_event = 0;

			rc = 0;
		}
		else
		{
			rc = -1;
		}
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_mouse_move_left() - move mouse left
 */
int tcap_mouse_move_left (TERMINAL *tp)
{
	MOUSE_STATUS *	mp;
	int				rc;

	if (tp == 0)
		return (-1);

	mp = MOUSE_STAT(tp);

	tcap_mouse_check_position(tp);

	TERMINAL_LOCK(tp);
	{
		if (mp->x > 0)
		{
			mp->x--;
			if (tcap_mouse_get_still_down(tp))
				mp->event = MOUSE_DRAG;
			else
				mp->event = MOUSE_MOVE;
			mp->queued_event = 0;

			rc = 0;
		}
		else
		{
			rc = -1;
		}
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_mouse_move_right() - move mouse right
 */
int tcap_mouse_move_right (TERMINAL *tp)
{
	MOUSE_STATUS *	mp;
	int				rc;

	if (tp == 0)
		return (-1);

	mp = MOUSE_STAT(tp);

	tcap_mouse_check_position(tp);

	TERMINAL_LOCK(tp);
	{
		if (mp->x < (tp->tcap->ints.maxcols-1))
		{
			mp->x++;
			if (tcap_mouse_get_still_down(tp))
				mp->event = MOUSE_DRAG;
			else
				mp->event = MOUSE_MOVE;
			mp->queued_event = 0;

			rc = 0;
		}
		else
		{
			rc = -1;
		}
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_mouse_move_ul() - move mouse up & left
 */
int tcap_mouse_move_ul (TERMINAL *tp)
{
	MOUSE_STATUS *	mp;
	int				rc;

	if (tp == 0)
		return (-1);

	mp = MOUSE_STAT(tp);

	tcap_mouse_check_position(tp);

	TERMINAL_LOCK(tp);
	{
		if (mp->y > 0 && mp->x > 0)
		{
			mp->y--;
			mp->x--;
			if (tcap_mouse_get_still_down(tp))
				mp->event = MOUSE_DRAG;
			else
				mp->event = MOUSE_MOVE;
			mp->queued_event = 0;

			rc = 0;
		}
		else
		{
			rc = -1;
		}
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_mouse_move_ur() - move mouse up & right
 */
int tcap_mouse_move_ur (TERMINAL *tp)
{
	MOUSE_STATUS *	mp;
	int				rc;

	if (tp == 0)
		return (-1);

	mp = MOUSE_STAT(tp);

	tcap_mouse_check_position(tp);

	TERMINAL_LOCK(tp);
	{
		if (mp->y > 0 && mp->x < (tp->tcap->ints.maxcols-1))
		{
			mp->y--;
			mp->x++;
			if (tcap_mouse_get_still_down(tp))
				mp->event = MOUSE_DRAG;
			else
				mp->event = MOUSE_MOVE;
			mp->queued_event = 0;

			rc = 0;
		}
		else
		{
			rc = -1;
		}
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_mouse_move_dl() - move mouse down & left
 */
int tcap_mouse_move_dl (TERMINAL *tp)
{
	MOUSE_STATUS *	mp;
	int				rc;

	if (tp == 0)
		return (-1);

	mp = MOUSE_STAT(tp);

	tcap_mouse_check_position(tp);

	TERMINAL_LOCK(tp);
	{
		if (mp->y < (tp->tcap->ints.maxrows-1) && mp->x > 0)
		{
			mp->y++;
			mp->x--;
			if (tcap_mouse_get_still_down(tp))
				mp->event = MOUSE_DRAG;
			else
				mp->event = MOUSE_MOVE;
			mp->queued_event = 0;

			rc = 0;
		}
		else
		{
			rc = -1;
		}
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_mouse_move_dr() - move mouse down & right
 */
int tcap_mouse_move_dr (TERMINAL *tp)
{
	MOUSE_STATUS *	mp;
	int				rc;

	if (tp == 0)
		return (-1);

	mp = MOUSE_STAT(tp);

	tcap_mouse_check_position(tp);

	TERMINAL_LOCK(tp);
	{
		if (mp->y < (tp->tcap->ints.maxrows-1) &&
	    	mp->x < (tp->tcap->ints.maxcols-1))
		{
			mp->y--;
			mp->x--;
			if (tcap_mouse_get_still_down(tp))
				mp->event = MOUSE_DRAG;
			else
				mp->event = MOUSE_MOVE;
			mp->queued_event = 0;

			rc = 0;
		}
		else
		{
			rc = -1;
		}
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_mouse_set_button() - set button up or down
 */
int tcap_mouse_set_button (TERMINAL *tp, int but, int pos)
{
	MOUSE_STATUS *	mp;

	if (tp == 0)
		return (-1);

	mp = MOUSE_STAT(tp);

	tcap_mouse_check_position(tp);

	if (but < 0 || but > NUM_BUTTONS)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		if (pos == BUTTON_DOWN)
		{
			mp->button[but]  = BUTTON_DOWN;
			mp->queued_event = 0;
			if (tcap_mouse_check_btn(tp, but))
				mp->event = term_button_dbl[but];
			else
				mp->event = term_button_down[but];
		}
		else
		{
			mp->button[but]		= BUTTON_UP;
			mp->queued_event	= 0;
			mp->event			= term_button_up[but];
		}
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}
