/*------------------------------------------------------------------------
 * screen output routines
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * tcap_outbuf() - add a cmd string to a buffer
 */
static void tcap_outbuf (const char *str, char *buf)
{
	if (is_cmd_pres(str))
		strcat(buf, str);
}

/*------------------------------------------------------------------------
 * tcap_buf_mono() - set mono attributes into a buffer
 */
static void tcap_buf_mono (const TERMINAL *tp, int on, attr_t a, attr_t t,
	int force, char *buf)
{
	if (on)
	{
		if ((a & A_REVERSE)   && ((t & A_REVERSE)   || force))
			tcap_outbuf(tp->tcap->strs.sts, buf);

		if ((a & A_UNDERLINE) && ((t & A_UNDERLINE) || force))
			tcap_outbuf(tp->tcap->strs.uls, buf);

		if ((a & A_BOLD)      && ((t & A_BOLD)      || force))
			tcap_outbuf(tp->tcap->strs.bds, buf);

		if ((a & A_BLINK)     && ((t & A_BLINK)     || force))
			tcap_outbuf(tp->tcap->strs.bls, buf);

		if ((a & A_DIM)       && ((t & A_DIM)       || force))
			tcap_outbuf(tp->tcap->strs.dms, buf);

		if ((a & A_INVIS)     && ((t & A_INVIS)     || force))
			tcap_outbuf(tp->tcap->strs.ivs, buf);

		if ((a & A_PROTECT)   && ((t & A_PROTECT)   || force))
			tcap_outbuf(tp->tcap->strs.prs, buf);
	}
	else
	{
		if ((a & A_REVERSE)   && (t & A_REVERSE))
			tcap_outbuf(tp->tcap->strs.ste, buf);

		if ((a & A_UNDERLINE) && (t & A_UNDERLINE))
			tcap_outbuf(tp->tcap->strs.ule, buf);

		if ((a & A_BOLD)      && (t & A_BOLD))
			tcap_outbuf(tp->tcap->strs.bde, buf);

		if ((a & A_BLINK)     && (t & A_BLINK))
			tcap_outbuf(tp->tcap->strs.ble, buf);

		if ((a & A_DIM)       && (t & A_DIM))
			tcap_outbuf(tp->tcap->strs.dme, buf);

		if ((a & A_INVIS)     && (t & A_INVIS))
			tcap_outbuf(tp->tcap->strs.ive, buf);

		if ((a & A_PROTECT)   && (t & A_PROTECT))
			tcap_outbuf(tp->tcap->strs.pre, buf);
	}
}

/*------------------------------------------------------------------------
 * tcap_buf_color() - set color attributes into a buffer
 */
static void tcap_buf_color (const TERMINAL *tp, attr_t a, char *buf)
{
	TCAP_DATA *td = tp->tcap;

	if (tp->scrn->color_flag)
	{
		int	fg;
		int	bg;

		/*----------------------------------------------------------------
		 * get fg & bg values
		 */
		fg = A_FG_CLRNUM(a) % td->ints.max_fg;
		bg = A_BG_CLRNUM(a) % td->ints.max_bg;

		/*----------------------------------------------------------------
		 * output appropriate cmds
		 */
		if (is_cmd_pres(td->parms.csr)    || is_cmd_pres(td->parms.csm)    ||
			is_cmd_pres(td->parms.csr_ti) || is_cmd_pres(td->parms.csm_ti) ||
			is_cmd_pres(td->parms.csr_tc) || is_cmd_pres(td->parms.csm_tc) )
		{
			char cs_buf[128];

			tcap_eval_parm(tp, cs_buf, S_CS, bg, fg);
			tcap_outbuf(cs_buf, buf);
		}
		else
		{
			tcap_outbuf(tp->tcap->strs.clr_fg[fg], buf);
			tcap_outbuf(tp->tcap->strs.clr_bg[bg], buf);
		}
	}
}

/*------------------------------------------------------------------------
 * tcap_attrbuf() - store attr cmds into a buffer
 */
int tcap_attrbuf (const TERMINAL *tp, attr_t a, int c, char *buf)
{
	attr_t	o;
	attr_t	t;
	int		f;
	int		char_set_changed = FALSE;
	int		old_is_color;
	int		new_is_color;
	int		need_init = FALSE;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0 || buf == 0)
		return (-1);

	*buf = 0;

	/*--------------------------------------------------------------------
	 * if attribute is UNSET, set to "default"
	 */
	if (a == A_UNSET)
	{
		attr_t r;
		attr_t s;

		tcap_get_default_attrs(tp, &r, &s, TRUE, TRUE);

		a = r;
	}

	/*--------------------------------------------------------------------
	 * if both color & mono attrs are specified, bail
	 */
	if ((a & A_COLOR) != 0 && (a & A_MONO) != 0)
	{
		/*----------------------------------------------------------------
		 * before we bail, convert BOLD/BLINK to FG-BRIGHT/BG-BRIGHT
		 */
		int am = a & (A_BOLD | A_BLINK);
		int at = a & (A_MONO);

		if (am != at)
			return (-1);

		if (am & A_BOLD)
		{
			a |= COLOR_FG_BRIGHT;
			a &= ~A_BOLD;
		}

		if (am & A_BLINK)
		{
			a |= COLOR_BG_BRIGHT;
			a &= ~A_BLINK;
		}
	}

	/*--------------------------------------------------------------------
	 * if current attribute is UNSET, treat as 0
	 */
	o = tp->scrn->curr_attr;
	if (c == 0)
	{
		o = A_UNSET;
		c = ' ';
	}

	if (o == A_UNSET)
	{
		need_init = TRUE;
		o = 0;
	}

	/*--------------------------------------------------------------------
	 * determine whether old & new attrs are in color
	 *
	 * Note that this test fails if an attr is color of (black/black),
	 * but this doesn't happen very often.
	 */
	old_is_color = ( (o & A_COLOR) != 0 );
	new_is_color = ( (a & A_COLOR) != 0 );

	/*--------------------------------------------------------------------
	 * check if we are switching from color->mono or mono->color
	 */
	if (old_is_color != new_is_color && tp->scrn->color_flag)
	{
		if (old_is_color)
		{
			attr_t r;
			attr_t s;

			/*------------------------------------------------------------
			 * switching color->mono
			 *
			 * Switch color to "standard" mono-type color (e.g. white/black).
			 */
			tcap_get_default_attrs(tp, &r, &s, TRUE, TRUE);

			tcap_buf_color(tp, r, buf);

			/*------------------------------------------------------------
			 * set reverse on/off
			 * (in hopes this will set the emulator to mono)
			 */
			tcap_outbuf(tp->tcap->strs.sts, buf);
			tcap_outbuf(tp->tcap->strs.ste, buf);
		}
		else
		{
			/*------------------------------------------------------------
			 * switching mono->color
			 *
			 * Turn off all attributes so we have a "normal" screen.
			 */
			tcap_buf_mono(tp, FALSE, o, o, TRUE, buf);
		}
	}

	/*--------------------------------------------------------------------
	 * store new attribute as current
	 */
	tp->scrn->curr_attr = a;

	/*--------------------------------------------------------------------
	 * get diffs between old & new attrs
	 */
	t = o ^ a;

	/*--------------------------------------------------------------------
	 * First do any offs if in mono mode
	 */
	if (! old_is_color)
	{
		tcap_buf_mono(tp, FALSE, o, t, char_set_changed, buf);
	}

	/*--------------------------------------------------------------------
	 * Do any alt char switching first, since brain-dead
	 * SCO XENIX ansi driver resets color on font change.
	 */
	{
		/*----------------------------------------------------------------
		 * first do the offs
		 */
		if (o & A_ALTCHARSET)
		{
			f = (a & A_ALTCHARSET) ? tp->tcap->alt_font_tbl[c] : 0;

			if (tp->scrn->alt_chars_on && tp->scrn->curr_alt_font != f)
			{
				if (tp->scrn->curr_alt_font != 0)
				{
					tcap_outbuf(tp->tcap->strs.ane[tp->scrn->curr_alt_font-1],
						buf);
				}
				tp->scrn->curr_alt_font = 0;
				char_set_changed = TRUE;
				tp->scrn->alt_chars_on = FALSE;
			}
		}
		else
		{
			f = (! (a & A_ALTCHARSET)) ? tp->tcap->reg_font_tbl[c] : 0;

			if (! tp->scrn->alt_chars_on && tp->scrn->curr_reg_font != f)
			{
				if (tp->scrn->curr_reg_font != 0)
				{
					tcap_outbuf(tp->tcap->strs.rne[tp->scrn->curr_reg_font-1],
						buf);
				}
				tp->scrn->curr_reg_font = 0;
				char_set_changed = TRUE;
			}
		}

		/*----------------------------------------------------------------
		 * now do the ons
		 */
		if (a & A_ALTCHARSET)
		{
			f = tp->tcap->alt_font_tbl[c];

			if (f && tp->scrn->curr_alt_font != f)
			{
				if (f != 0)
				{
					tcap_outbuf(tp->tcap->strs.ans[f-1], buf);
				}
				tp->scrn->curr_alt_font = f;
				char_set_changed = TRUE;
			}
			tp->scrn->alt_chars_on = TRUE;
		}
		else
		{
			f = tp->tcap->reg_font_tbl[c];

			if (f && tp->scrn->curr_reg_font != f)
			{
				if (f != 0)
				{
					tcap_outbuf(tp->tcap->strs.rns[f-1], buf);
				}
				tp->scrn->curr_reg_font = f;
				char_set_changed = TRUE;
			}
		}
	}

	/*--------------------------------------------------------------------
	 * now do any color processing if we have color
	 */
	if (new_is_color)
	{
		/*----------------------------------------------------------------
		 * output new color if changed, or
		 * if we are switching to or from alt-char set, or
		 * if fonts are different.
		 */
		if (t & A_COLOR || char_set_changed)
		{
			tcap_buf_color(tp, a, buf);
		}
	}

	/*--------------------------------------------------------------------
	 * Now do any ons if in mono mode
	 */
	if (! new_is_color)
	{
		tcap_buf_mono(tp, TRUE, a, t, char_set_changed, buf);
	}

	/*--------------------------------------------------------------------
	 * Now if any attribute changes happened & we are in alt-char mode,
	 * re-issue the alt cmd (since brain-dead eterm (linux) loses it).
	 */
	if (t != 0 && tp->scrn->alt_chars_on == TRUE)
	{
		if (tp->scrn->curr_alt_font != 0)
		{
			tcap_outbuf(tp->tcap->strs.ans[tp->scrn->curr_alt_font-1], buf);
		}
	}

	return (0);
}
