/*------------------------------------------------------------------------
 * terminal init/deinit routines
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * tcap_cinit() - init terminal state stuff
 */
int tcap_cinit (TERMINAL *tp)
{
	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		/*----------------------------------------------------------------
		 * cache old & new terminal states
		 */
		tcap_get_orig_mode(tp);
		tcap_get_prog_mode(tp);

		/*----------------------------------------------------------------
		 * now switch terminal to new state
		 */
		tcap_set_prog_mode(tp);
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_cexit() - uninit terminal state stuff
 */
int tcap_cexit (TERMINAL *tp, int exit_flag)
{
	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		/*----------------------------------------------------------------
		 * switch terminal to orig state
		 */
		tcap_set_orig_mode(tp);

		/*----------------------------------------------------------------
		 * if exiting, clear out everything
		 */
		if (exit_flag)
		{
			/*------------------------------------------------------------
			 * close out debug logs
			 */
			tcap_kbd_debug(tp, 0, FALSE, FALSE);
			tcap_out_debug(tp, 0, FALSE, FALSE);

			/*------------------------------------------------------------
			 * clear signal table
			 */
			tcap_signal_set_rtn(tp, -1, 0, 0);

			/*------------------------------------------------------------
			 * close devices
			 */
			term_screen_dev_close(tp->term);
		}
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_exec_shell() - shell out to a command shell
 */
int tcap_exec_shell (TERMINAL *tp)
{
	int status;
	int y;
	int x;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * ignore request if no way to refresh the screen
	 */
	if (! tcap_async_check(tp->scrn, KEY_REFRESH))
		return (-1);

	TERMINAL_LOCK(tp);
	{
		/*------------------------------------------------------------
		 * save original screen position & clear the screen
		 */
		y = tp->scrn->curr_row;
		x = tp->scrn->curr_col;

		tcap_clear_screen(tp);
		tcap_outpos(tp, 0, 0);
		tcap_outflush(tp);

		/*------------------------------------------------------------
		 * shell out
		 */
		tcap_set_orig_mode(tp);
		status = tcap_exec_cmd(tp, NULL, NULL, TRUE, NULL);
		tcap_set_prog_mode(tp);

		/*------------------------------------------------------------
		 * refresh the screen
		 */
		tcap_async_process(tp->scrn, KEY_REFRESH);

		/*------------------------------------------------------------
		 * re-position cursor to where it was
		 */
		tcap_outpos(tp, y, x);
		tcap_outflush(tp);
	}
	TERMINAL_UNLOCK(tp);

	return (status);
}

/*------------------------------------------------------------------------
 * tcap_get_prog_mode() - get program terminal mode
 */
int tcap_get_prog_mode (TERMINAL *tp)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_get_prog_mode(tp->term);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_get_orig_mode() - get original terminal mode
 */
int tcap_get_orig_mode (TERMINAL *tp)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_get_orig_mode(tp->term);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_set_prog_mode() - put screen in program mode
 */
int tcap_set_prog_mode (TERMINAL *tp)
{
	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		/*----------------------------------------------------------------
		 * set terminal to program state
		 */
		term_set_prog_mode(tp->term);

		/*----------------------------------------------------------------
		 * indicate current position unknown
		 */
		tp->scrn->curr_row = -1;
		tp->scrn->curr_col = -1;

		/*----------------------------------------------------------------
		 * reset alarm time if any
		 */
		if (tp->scrn->alarm_time_left)
		{
			term_alarm_start(tp->scrn->alarm_time_left);
			tp->scrn->alarm_time_left = 0;
		}

		/*----------------------------------------------------------------
		 * send term-initialization strings
		 */
		tcap_outcmd(tp, S_TI,  tp->tcap->strs.ti);
		tcap_outcmd(tp, S_ACE, tp->tcap->strs.ace);

		/*----------------------------------------------------------------
		 * inform parent to turn on any keyboard mapping
		 */
		tcap_outcmd(tp, S_KBO, tp->tcap->strs.kbo);

		/*----------------------------------------------------------------
		 * check if mouse was in effect
		 */
		if (tp->scrn->mouse_restore)
		{
			tp->scrn->mouse_restore = FALSE;
			tcap_mouse_init(tp);
		}

		/*----------------------------------------------------------------
		 * restore window title if there was one
		 */
		if (*tp->scrn->win_title != 0)
		{
			tcap_set_window_title(tp, 0);
		}

		/*----------------------------------------------------------------
		 * flush any pending output
		 */
		tcap_outflush(tp);

		/*----------------------------------------------------------------
		 * initialize signals
		 */
		tcap_sig_init(tp);

		/*----------------------------------------------------------------
		 * set mode on
		 */
		tp->scrn->mode = TRUE;
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_set_orig_mode() - restore screen to original mode
 */
int tcap_set_orig_mode (TERMINAL *tp)
{
	if (tp == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * check if already in orig mode
	 */
	if (! tp->scrn->mode)
		return (0);

	/*--------------------------------------------------------------------
	 * now do it
	 */
	TERMINAL_LOCK(tp);
	{
		/*----------------------------------------------------------------
		 * end signal processing
		 */
		tcap_sig_end(tp);

		/*----------------------------------------------------------------
		 * save old alarm time
		 */
		tp->scrn->alarm_time_left = term_alarm_stop();

		/*----------------------------------------------------------------
		 * check if mouse in effect
		 */
		if (tcap_mouse_get_is_on(tp))
		{
			tp->scrn->mouse_restore = TRUE;
			tcap_mouse_end(tp);
		}

		/*----------------------------------------------------------------
		 * set fonts to normal
		 */
		if (tp->scrn->curr_reg_font)
		{
			tcap_outcmd(tp, tcap_reg_font_cmds[tp->scrn->curr_reg_font].off,
				tp->tcap->strs.rne[tp->scrn->curr_reg_font-1]);
			tp->scrn->curr_reg_font = 0;
		}

		if (tp->scrn->curr_alt_font)
		{
			tcap_outcmd(tp, tcap_alt_font_cmds[tp->scrn->curr_alt_font].off,
				tp->tcap->strs.ane[tp->scrn->curr_alt_font-1]);
			tp->scrn->curr_alt_font = 0;
		}

		/*----------------------------------------------------------------
		 * reset screen attrs if original known
		 */
		if (tp->scrn->orig_attr != A_UNSET)
		{
			tcap_outattr(tp, tp->scrn->orig_attr, ' ');
			tcap_clear_screen(tp);
			tcap_outpos(tp, 0, 0);
		}

		/*----------------------------------------------------------------
		 * restore cursor if different from normal
		 */
		tcap_curs_set(tp, VISIBILITY_ON);

		/*----------------------------------------------------------------
		 * inform parent to turn off any keyboard mapping
		 */
		tcap_outcmd(tp, S_KBF, tp->tcap->strs.kbf);

		/*----------------------------------------------------------------
		 * send term-uninit strings
		 */
		tcap_outcmd(tp, S_ACD, tp->tcap->strs.acd);
		tcap_outcmd(tp, S_TE,  tp->tcap->strs.te);

		/*----------------------------------------------------------------
		 * flush any pending output
		 */
		tcap_outflush(tp);

		/*----------------------------------------------------------------
		 * Before flushing any input, give the terminal emulator time
		 * to finish up what he is told to do.
		 */
		term_nap(100);

		/*----------------------------------------------------------------
		 * This won't flush any input (that was already done, but
		 * it will give any event handlers one last chance.
		 */
		tcap_outflush(tp);

		/*----------------------------------------------------------------
		 * Flush any pending input.
		 * This is needed to clear out any "mapped" keyboard input.
		 */
		tcap_kbd_clr_ta(tp);

		/*----------------------------------------------------------------
		 * set terminal to original state
		 */
		term_set_orig_mode(tp->term);

		/*----------------------------------------------------------------
		 * indicate in orig mode
		 */
		tp->scrn->mode = FALSE;
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_get_save_mode() - save current terminal mode
 */
int tcap_get_save_mode (TERMINAL *tp)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_get_save_mode(tp->term);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_set_save_mode() - set terminal to saved terminal mode
 */
int tcap_set_save_mode (TERMINAL *tp)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_set_save_mode(tp->term);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}
