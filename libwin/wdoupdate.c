/*------------------------------------------------------------------------
 * update routines
 */
#include "wincommon.h"

#define CM_LEN		6		/* average CM cmd length */

/*------------------------------------------------------------------------
 * do_check() - internal check if we need to "restart" windows
 */
static int do_check (SCREEN *s)
{
	/*--------------------------------------------------------------------
	 * error if curses was never initialized
	 */
	if (s == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * if not in curses, reset terminal mode
	 */
	if (! s->in_curses)
	{
		scrn_cinit(s);
		clearok(SCR_CURSCR(s), TRUE);
		scrn_slk_noutrefresh(s);
		s->in_curses = TRUE;
	}

	return (OK);
}

/*------------------------------------------------------------------------
 * doupdate() - update screen to match curscr
 */
int doupdate (void)
{
	return scrn_doupdate(win_cur_screen);
}

/*------------------------------------------------------------------------
 * scrn_doupdate() - update screen to match curscr
 */
int scrn_doupdate (SCREEN *s)
{
	WINDOW *	cs	= SCR_CURSCR(s);
	WINDOW *	ts	= SCR_TRMSCR(s);
	int			i;
	int			changed;
	int			rc;
	int			cur_x;
	int			cur_y;

	/*--------------------------------------------------------------------
	 * check if "restart" needed
	 */
	if (do_check(s))
		return (ERR);

	/*--------------------------------------------------------------------
	 * indicate we are "in a refresh"
	 */
	s->in_refresh = TRUE;

	/*--------------------------------------------------------------------
	 * check if a clear was asked for
	 */
	if (getclear(cs))
	{
		attr_t a = wattrget(ts);

		clearok(cs, FALSE);
		scrn_outattr(s, a, 0);
		scrn_clear_screen(s);
		win_clear(ts, a | ' ');
		touchwin(cs);
	}

	/*--------------------------------------------------------------------
	 * update each line
	 */
	changed = FALSE;
	for (i=0; i<getmaxy(ts); i++)
	{
		rc = scrn_doupdateln(s, i);
		if (rc == ERR)
			return (ERR);

		if (rc)
			changed = TRUE;
	}

	/*--------------------------------------------------------------------
	 * check if we have to re-position the cursor
	 */
	cur_x = scrn_get_curr_col(s);
	cur_y = scrn_get_curr_row(s);

	if (! getleave(cs))
	{
		if (changed || getcury(cs) != cur_y || getcurx(cs) != cur_x)
		{
			scrn_outpos(s, getcury(cs), getcurx(cs));
		}
		leaveok(cs, TRUE);
	}

	/*--------------------------------------------------------------------
	 * show current position
	 */
	setcury(ts, cur_y);
	setcurx(ts, cur_x);

	/*--------------------------------------------------------------------
	 * flush all output and clear "in refresh" flag
	 */
	scrn_outflush(s);
	s->in_refresh = FALSE;

	return (OK);
}

/*------------------------------------------------------------------------
 * doupdateln() - refresh a line of a window & return whether anything
 * was output.
 */
int doupdateln (int i)
{
	return scrn_doupdateln(win_cur_screen, i);
}

/*------------------------------------------------------------------------
 * scrn_doupdateln() - refresh a line of a window & return whether anything
 * was output.
 */
int scrn_doupdateln (SCREEN *s, int i)
{
	WINDOW *	cs;
	WINDOW *	ts;
	int			j;
	int			writing;
	int			last_line;
	int			last_write;
	int			changed;
	chtype *	src;
	chtype *	dst;
	chtype		a;
	int			c;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (s == 0)
		return (ERR);

	cs = SCR_CURSCR(s);
	ts = SCR_TRMSCR(s);
	
	/*--------------------------------------------------------------------
	 * check if a "restart" needed
	 */
	if (do_check(s))
		return (ERR);

	/*--------------------------------------------------------------------
	 * done if no changes in line
	 */
	if (cs->_firstch[i] == _NOCHANGE)
		return (FALSE);

	/*--------------------------------------------------------------------
	 * check if doing last line on screen
	 */
	if (scrn_need_ins_final(s))
		last_line = (i == getmaxy(cs)-1);
	else
		last_line = FALSE;

	if (cs->_lastch[i] >= getmaxx(cs)-1)
		cs->_lastch[i] = getmaxx(cs)-1-last_line;

	/*--------------------------------------------------------------------
	 * get line pointers
	 */
	src = cs->_y[i];
	dst = ts->_y[i];

	/*--------------------------------------------------------------------
	 * now output all changes in the line
	 */
	writing = FALSE;
	changed = FALSE;
	last_write = -1;
	for (j=cs->_firstch[i]; j<=cs->_lastch[i]; j++)
	{
		if (src[j] != dst[j])
		{
			c = A_GETTEXT(src[j]);
			a = A_GETATTR(src[j]);

			if (scrn_get_curr_attr(s) != a || (a & A_ALTCHARSET))
			{
				scrn_outattr(s, a, c);
			}

			if (! writing)
			{
				int	do_move = TRUE;

				/*--------------------------------------------------------
				 * If we did write something previously and all chars
				 * between there and here have the same attributes and
				 * the distance is less than the # chars in a CM cmd,
				 * just repeat the chars rather than moving.
				 */
				if (last_write >= 0 && (j - last_write) < CM_LEN)
				{
					int		k;
					attr_t	la = A_GETATTR(src[last_write]);

					for (k=last_write+1; k<=j; k++)
					{
						if (la != A_GETATTR(src[k]))
							break;
					}

					if (k > j)
					{
						do_move = FALSE;
						for (k=last_write+1; k<j; k++)
							scrn_outc(s, A_GETTEXT(src[k]));
					}
				}

				if (do_move)
				{
					scrn_outpos(s, i, j);
				}

				changed = TRUE;
				writing = TRUE;
			}

			scrn_outc(s, c);
			dst[j] = src[j];
			last_write = j;
		}
		else
		{
			writing = FALSE;
		}
	}

	/*--------------------------------------------------------------------
	 * if doing last line, do last char special (to avoid scrolling)
	 */
	if (last_line)
	{
		j = getmaxx(cs)-1;
		if (src[j] != dst[j])
		{
			c = scrn_write_final_char(s,
				A_GETTEXT(src[j-1]), A_GETATTR(src[j-1]),
				A_GETTEXT(src[j]),   A_GETATTR(src[j]));

			if (c == OK)
			{
				dst[j] = src[j];
				changed = TRUE;
			}
		}
	}

	/*--------------------------------------------------------------------
	 * clear first/last entries
	 */
	cs->_firstch[i] = cs->_lastch[i] = _NOCHANGE;

	/*--------------------------------------------------------------------
	 * show what the current attribute is
	 */
	wattrset(ts, scrn_get_curr_attr(s));

	/*--------------------------------------------------------------------
	 * if we output anything, check if mouse cursor has to be redisplayed
	 */
	if (changed)
	{
		if (i == mouse_y(ts))
			scrn_mouse_draw_cursor(s);
	}

	return (changed);
}
