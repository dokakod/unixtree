/*------------------------------------------------------------------------
 * screen output routines
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * tcap_outc() - output a character
 */
int tcap_outc (TERMINAL *tp, int c)
{
	int ch;
	int ic = c;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * Check if negative (from calling with a high-order char as a
	 * signed char) & force modulo 256.
	 */
	ic %= 256;
	if (ic < 0)
		ic += 256;

	/*--------------------------------------------------------------------
	 * output the char
	 */
	TERMINAL_LOCK(tp);
	{
		/*----------------------------------------------------------------
		 * translate char according to appropriate table
		 */
		if (tp->scrn->alt_chars_on)
		{
			/*------------------------------------------------------------
			 * translate using the alt-chars table
			 */
			ch = tp->tcap->alt_tbl[ic];

			/*------------------------------------------------------------
			 * If the alt-font number for this char is different from the
			 * current alt-font number, re-output the current attributes,
			 * but with them suitable for the new char, not the old.
			 */
			if (tp->tcap->alt_font_tbl[ic] != tp->scrn->curr_alt_font)
				tcap_outattr(tp, tp->scrn->curr_attr, ic);
		}
		else
		{
			/*------------------------------------------------------------
			 * translate using the reg-chars table
			 */
			ch = tp->tcap->reg_tbl[ic];

			/*------------------------------------------------------------
			 * If the reg-font number for this char is different from the
			 * current reg-font number, re-output the current attributes,
			 * but with them suitable for the new char, not the old.
			 */
			if (tp->tcap->reg_font_tbl[ic] != tp->scrn->curr_reg_font)
				tcap_outattr(tp, tp->scrn->curr_attr, ic);
		}

		/*----------------------------------------------------------------
		 * output the char
		 */
		tcap_outch(tp, ch, TRUE);

		/*----------------------------------------------------------------
		 * check current position
		 */
		if (! tp->scrn->alt_chars_on && c < ' ')
		{
			/*------------------------------------------------------------
			 * process control char
			 */
			switch (ic)
			{
			case '\b':
				if (tp->scrn->curr_col)
					tp->scrn->curr_col--;
				break;

			case '\r':
				tp->scrn->curr_col = 0;
				break;

			case '\n':
				tp->scrn->curr_col = 0;
				if (++tp->scrn->curr_row >= tp->tcap->ints.maxrows)
					tp->scrn->curr_row = tp->tcap->ints.maxrows-1;
				break;
			}
		}
		else
		{
			/*------------------------------------------------------------
			 * process past EOL condition
			 */
			if (++tp->scrn->curr_col >= tp->tcap->ints.maxcols)
			{
				if (tp->tcap->bools.am)
				{
					/* wrap to next line */

					tp->scrn->curr_col = 0;
					if (++tp->scrn->curr_row >= tp->tcap->ints.maxrows)
						tp->scrn->curr_row = tp->tcap->ints.maxrows-1;
				}
				else
				{
					/* just stay where we were */

					tp->scrn->curr_col = tp->tcap->ints.maxcols-1;
				}
			}
		}
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_outs() - output a string
 */
int tcap_outs (TERMINAL *tp, const char *s)
{
	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		while (*s)
			tcap_outc(tp, *s++);
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_set_mono() - set mono attributes
 */
static void tcap_set_mono (TERMINAL *tp, int on, attr_t a, attr_t t, int force)
{
	if (on)
	{
		if ((a & A_REVERSE)   && ((t & A_REVERSE)   || force))
			tcap_outcmd(tp, S_STS, tp->tcap->strs.sts);

		if ((a & A_UNDERLINE) && ((t & A_UNDERLINE) || force))
			tcap_outcmd(tp, S_ULS, tp->tcap->strs.uls);

		if ((a & A_BOLD)      && ((t & A_BOLD)      || force))
			tcap_outcmd(tp, S_BDS, tp->tcap->strs.bds);

		if ((a & A_BLINK)     && ((t & A_BLINK)     || force))
			tcap_outcmd(tp, S_BLS, tp->tcap->strs.bls);

		if ((a & A_DIM)       && ((t & A_DIM)       || force))
			tcap_outcmd(tp, S_DMS, tp->tcap->strs.dms);

		if ((a & A_INVIS)     && ((t & A_INVIS)     || force))
			tcap_outcmd(tp, S_IVS, tp->tcap->strs.ivs);

		if ((a & A_PROTECT)   && ((t & A_PROTECT)   || force))
			tcap_outcmd(tp, S_PRS, tp->tcap->strs.prs);
	}
	else
	{
		if ((a & A_REVERSE)   && (t & A_REVERSE))
			tcap_outcmd(tp, S_STE, tp->tcap->strs.ste);

		if ((a & A_UNDERLINE) && (t & A_UNDERLINE))
			tcap_outcmd(tp, S_ULE, tp->tcap->strs.ule);

		if ((a & A_BOLD)      && (t & A_BOLD))
			tcap_outcmd(tp, S_BDE, tp->tcap->strs.bde);

		if ((a & A_BLINK)     && (t & A_BLINK))
			tcap_outcmd(tp, S_BLE, tp->tcap->strs.ble);

		if ((a & A_DIM)       && (t & A_DIM))
			tcap_outcmd(tp, S_DME, tp->tcap->strs.dme);

		if ((a & A_INVIS)     && (t & A_INVIS))
			tcap_outcmd(tp, S_IVE, tp->tcap->strs.ive);

		if ((a & A_PROTECT)   && (t & A_PROTECT))
			tcap_outcmd(tp, S_PRE, tp->tcap->strs.pre);
	}
}

/*------------------------------------------------------------------------
 * tcap_set_color() - set color attributes
 */
static void tcap_set_color (TERMINAL *tp, attr_t a)
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
			tcap_outparm(tp, S_CS, bg, fg);
		}
		else
		{
			tcap_outparm(tp, S_FG, fg, 0);
			tcap_outparm(tp, S_BG, bg, 0);
		}
	}
}

/*------------------------------------------------------------------------
 * tcap_outattr() - set screen attributes
 */
int tcap_outattr (TERMINAL *tp, attr_t a, int c)
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
	if (tp == 0)
		return (-1);

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
	 * now do the attributes
	 */
	TERMINAL_LOCK(tp);
	{
		/*----------------------------------------------------------------
		 * check if we are switching from color->mono or mono->color
		 */
		if (old_is_color != new_is_color && tp->scrn->color_flag)
		{
			if (old_is_color)
			{
				attr_t r;
				attr_t s;

				/*--------------------------------------------------------
				 * switching color->mono
				 *
				 * Switch color to "std" mono-type color (e.g. white/black).
				 */
				tcap_get_default_attrs(tp, &r, &s, TRUE, TRUE);

				tcap_set_color(tp, r);

				/*--------------------------------------------------------
				 * set reverse on/off
				 * (in hopes this will set the emulator to mono)
				 */
				tcap_outcmd(tp, S_STS, tp->tcap->strs.sts);
				tcap_outcmd(tp, S_STE, tp->tcap->strs.ste);
			}
			else
			{
				/*--------------------------------------------------------
				 * switching mono->color
				 *
				 * Turn off all attributes so we have a "normal" screen.
				 */
				tcap_set_mono(tp, FALSE, o, o, TRUE);
			}
		}

		/*----------------------------------------------------------------
		 * store new attribute as current
		 */
		tp->scrn->curr_attr = a;

		/*----------------------------------------------------------------
		 * get diffs between old & new attrs
		 */
		t = o ^ a;

		/*----------------------------------------------------------------
		 * First do any offs if in mono mode
		 */
		if (! old_is_color)
		{
			tcap_set_mono(tp, FALSE, o, t, char_set_changed);
		}

		/*----------------------------------------------------------------
		 * Do any alt char switching first, since brain-dead
		 * SCO XENIX ansi driver resets color on font change.
		 */
		{
			/*------------------------------------------------------------
			 * first do the offs
			 */
			if (o & A_ALTCHARSET)
			{
				f = (a & A_ALTCHARSET) ? tp->tcap->alt_font_tbl[c] : 0;

				if (tp->scrn->alt_chars_on && tp->scrn->curr_alt_font != f)
				{
					if (tp->scrn->curr_alt_font != 0)
					{
						tcap_outcmd(tp,
							tcap_alt_font_cmds[tp->scrn->curr_alt_font].off,
							tp->tcap->strs.ane[tp->scrn->curr_alt_font-1]);
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
						tcap_outcmd(tp,
							tcap_reg_font_cmds[tp->scrn->curr_reg_font].off,
							tp->tcap->strs.rne[tp->scrn->curr_reg_font-1]);
					}
					tp->scrn->curr_reg_font = 0;
					char_set_changed = TRUE;
				}
			}

			/*------------------------------------------------------------
			 * now do the ons
			 */
			if (a & A_ALTCHARSET)
			{
				f = tp->tcap->alt_font_tbl[c];

				if (f && tp->scrn->curr_alt_font != f)
				{
					if (f != 0)
					{
						tcap_outcmd(tp, tcap_alt_font_cmds[f].on,
							tp->tcap->strs.ans[f-1]);
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
						tcap_outcmd(tp, tcap_reg_font_cmds[f].on,
							tp->tcap->strs.rns[f-1]);
					}
					tp->scrn->curr_reg_font = f;
					char_set_changed = TRUE;
				}
			}
		}

		/*----------------------------------------------------------------
		 * now do any color processing if we have color
		 */
		if (new_is_color)
		{
			/*------------------------------------------------------------
			 * output new color if changed, or
			 * if we are switching to or from alt-char set, or
			 * if fonts are different.
			 */
			if (t & A_COLOR || char_set_changed)
			{
				tcap_set_color(tp, a);
			}
		}

		/*----------------------------------------------------------------
		 * Now do any ons if in mono mode
		 */
		if (! new_is_color)
		{
			tcap_set_mono(tp, TRUE, a, t, char_set_changed);
		}

		/*----------------------------------------------------------------
		 * Now if any attribute changes happened & we are in alt-char mode,
		 * re-issue the alt cmd (since brain-dead eterm (linux) loses it).
		 */
		if (t != 0 && tp->scrn->alt_chars_on == TRUE)
		{
			if (tp->scrn->curr_alt_font != 0)
			{
				tcap_outcmd(tp, tcap_alt_font_cmds[tp->scrn->curr_alt_font].on,
					tp->tcap->strs.ans[tp->scrn->curr_alt_font-1]);
			}
		}
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_outpos() - set screen position
 */
int tcap_outpos (TERMINAL *tp, int row, int col)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		if (row != tp->scrn->curr_row || col != tp->scrn->curr_col)
		{
			if (row < 0 || row >= tp->tcap->ints.maxrows ||
				col < 0 || col >= tp->tcap->ints.maxcols)
			{
				rc = -1;
			}
			else
			{
				tcap_outparm(tp, S_CM, row, col);

				tp->scrn->curr_row = row;
				tp->scrn->curr_col = col;

				rc = 0;
			}
		}
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_eval_parm() - evaluate a parameterized expression
 */
char * tcap_eval_parm (const TERMINAL *tp, char *buf, SCRN_CMD cmd,
	int p1, int p2)
{
	TCAP_DATA *	td	= tp->tcap;
	char *		s;
	int			t;

	switch (cmd)
	{
	/*--------------------------------------------------------------------
	 * move cursor
	 */
	case S_CM:
		t = td->pdata.cm.type;
		s = td->pdata.cm.str;
		break;

	/*--------------------------------------------------------------------
	 * color string
	 */
	case S_CS:
		t = td->pdata.cs.type;
		s = td->pdata.cs.str;
		if (td->pdata.cs.data)
		{
			p1 = td->ints.color_bg_map[p1];
			p2 = td->ints.color_fg_map[p2];
		}
		break;

	/*--------------------------------------------------------------------
	 * foreground color
	 */
	case S_FG:
		t = td->pdata.fg.type;
		s = td->pdata.fg.str;
		if (td->pdata.fg.data)
		{
			p1 = td->ints.color_fg_map[p1];
		}
		break;

	/*--------------------------------------------------------------------
	 * background color
	 */
	case S_BG:
		t = td->pdata.bg.type;
		s = td->pdata.bg.str;
		if (td->pdata.bg.data)
		{
			p1 = td->ints.color_bg_map[p1];
		}
		break;

	/*--------------------------------------------------------------------
	 * invalid cmd
	 */
	default:
		t = -1;
		s = 0;
		break;
	}

	if (! is_cmd_pres(s) || *s == 0)
	{
		return ((char *)"");
	}

	return tcap_db_calc(t, buf, s, p1, p2);
}

/*------------------------------------------------------------------------
 * tcap_get_valid_acs() - check if a char is a valid alt-char
 */
int tcap_get_valid_acs (const TERMINAL *tp, int c)
{
	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (tp == 0)
		return (FALSE);

	/*--------------------------------------------------------------------
	 * get font number for this char
	 */
	c = tp->tcap->alt_font_tbl[c & 0xff];

	/*--------------------------------------------------------------------
	 * if font is 0, then nothing needs to be sent, but it is valid
	 */
	if (c == 0)
		return (TRUE);

	/*--------------------------------------------------------------------
	 * if user set no-alt-chars, then it is not valid
	 */
	if (! tp->tcap->bools.ala)
		return (FALSE);

	/*--------------------------------------------------------------------
	 * if both on & off strings exist for this font, then it is valid
	 */
	if (is_cmd_pres(tp->tcap->strs.ans[c-1]) &&
	    is_cmd_pres(tp->tcap->strs.ane[c-1]))
	{
		return (TRUE);
	}

	return (FALSE);
}

/*------------------------------------------------------------------------
 * tcap_set_valid_acs() - set if a char is a valid alt-char
 */
int tcap_set_valid_acs (TERMINAL *tp, int bf)
{
	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (tp == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * store new flag
	 */
	TERMINAL_LOCK(tp);
	{
		tp->tcap->bools.ala = bf;
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_curs_set() - set cursor mode
 */
int tcap_curs_set (TERMINAL *tp, int vis)
{
	int old_vis;
	int rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (tp == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * if new-visibility == old-visibility, then we're done
	 */
	old_vis = tp->scrn->visibility;

	if (vis == old_vis)
	{
		return (old_vis);
	}

	TERMINAL_LOCK(tp);
	{
		/*----------------------------------------------------------------
		 * set new visibility mode
		 */
		rc = 0;
		switch (vis)
		{
		case VISIBILITY_OFF:
			if (! is_cmd_pres(tp->tcap->strs.vi))
				rc = -1;
			tcap_outcmd(tp, S_VI, tp->tcap->strs.vi);
			break;

		case VISIBILITY_ON:
			if (! is_cmd_pres(tp->tcap->strs.ve))
				rc = -1;
			tcap_outcmd(tp, S_VE, tp->tcap->strs.ve);
			break;

		case VISIBILITY_VERY_ON:
			if (! is_cmd_pres(tp->tcap->strs.vs))
				rc = -1;
			tcap_outcmd(tp, S_VS, tp->tcap->strs.vs);
			break;

		default:
			rc = -1;
			break;
		}

		/*----------------------------------------------------------------
		 * cache current mode if successful in changing it
		 */
		if (rc == 0)
			tp->scrn->visibility = vis;
	}
	TERMINAL_UNLOCK(tp);

	return (old_vis);
}

/*------------------------------------------------------------------------
 * tcap_delay_output() - output delay chars for a calculated delay
 */
int tcap_delay_output (TERMINAL *tp, int ms, int (*outrtn)(TERMINAL *tp, int c))
{
	int (*rtn)(TERMINAL *tp, int c) = (outrtn == 0 ? tcap_outchar : outrtn);
	int n;
	int i;
	int cps;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (tp == 0)
		return (-1);

	if (ms < 0)
		return (-1);

	if (ms == 0)
		return (0);

	/*--------------------------------------------------------------------
	 * get chars-per-sec
	 *
	 * Note that the baud-rate is bits-per-sec, which includes a start
	 * and stop bit, so 10 bits = 1 char.
	 */
	cps = tcap_get_baud_rate(tp) / 10;

	/*--------------------------------------------------------------------
	 * now calculate number of pad-chars to output
	 */
	n = (ms * cps) / 1000;
	if (n == 0)
		return (0);

	/*--------------------------------------------------------------------
	 * now output them
	 */
	TERMINAL_LOCK(tp);
	{
		for (i=0; i<n; i++)
		{
			(*rtn)(tp, 0);
		}
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_out_debug() - start/stop screen debugging
 */
int tcap_out_debug (TERMINAL *tp, const char *path, int flag, int text)
{
	int rc;

	if (tp == 0)
		return (FALSE);

	TERMINAL_LOCK(tp);
	{
#if V_WINDOWS
		if (tp->tcap->db_type == TCAP_DB_WINDOWS)
			text = TRUE;
#endif
		tp->scrn->debug_scrn_text	= text;

		if (path == 0)
		{
			if (*tp->scrn->debug_scrn_path == 0)
				path = text ? TCAP_SCRN_LOG_TXT : TCAP_SCRN_LOG_BIN;
			else
				path = tp->scrn->debug_scrn_path;
		}
		strcpy(tp->scrn->debug_scrn_path, path);

		if (flag)
		{
			if (tp->scrn->debug_scrn_fp == 0)
			{
				tp->scrn->debug_scrn_mode	= TRUE;
				tp->scrn->debug_scrn_fp		= fopen(path, "w");

				if (tp->scrn->mode)
					tcap_set_window_title(tp, 0);
			}
		}
		else
		{
			if (tp->scrn->debug_scrn_fp != 0)
			{
				fclose(tp->scrn->debug_scrn_fp);
				tp->scrn->debug_scrn_fp = 0;

				if (tp->scrn->mode)
					tcap_set_window_title(tp, 0);
			}
		}

		rc = (tp->scrn->debug_scrn_fp != 0);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_get_curr_row() - get current row number
 */
int tcap_get_curr_row (const TERMINAL *tp)
{
	if (tp == 0)
		return (-1);

	return (tp->scrn->curr_row);
}

/*------------------------------------------------------------------------
 * tcap_get_curr_col() - get current col number
 */
int tcap_get_curr_col (const TERMINAL *tp)
{
	if (tp == 0)
		return (-1);

	return (tp->scrn->curr_col);
}

/*------------------------------------------------------------------------
 * tcap_get_curr_attr() - get current attributes
 */
attr_t tcap_get_curr_attr (const TERMINAL *tp)
{
	if (tp == 0)
		return (A_UNSET);

	return (tp->scrn->curr_attr);
}

/*------------------------------------------------------------------------
 * tcap_set_window_title() - set window title
 *
 * Note: we always add any debug log info
 */
int tcap_set_window_title (TERMINAL *tp, const char *s)
{
	if (tp == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * We only send the title if we have both a TTS & TTE string.
	 */
	if (! is_cmd_pres(tp->tcap->strs.tts) || ! is_cmd_pres(tp->tcap->strs.tte))
		return (-1);

	/*--------------------------------------------------------------------
	 * OK - let's do it
	 */
	TERMINAL_LOCK(tp);
	{
		char	new_title[128];

		/*----------------------------------------------------------------
		 * A NULL string implies we output the cached string.
		 */
		if (s == 0)
		{
			/*------------------------------------------------------------
			 * use cached string
			 */
			s = tp->scrn->win_title;
		}
		else
		{
			/*------------------------------------------------------------
			 * cache real string
			 */
			strncpy(tp->scrn->win_title, s, sizeof(tp->scrn->win_title));
		}

		/*----------------------------------------------------------------
		 * Check if we are appending anything
		 */
		if (*s != 0)
		{
			if (tp->scrn->debug_keys_fp != 0 ||
				tp->scrn->debug_scrn_fp != 0)
			{
				strcpy(new_title, s);
				s = new_title;

				strcat(new_title, " [logs:");

				if (tp->scrn->debug_keys_fp != 0)
				{
					strcat(new_title, " ");
					strcat(new_title, tp->scrn->debug_keys_text ?
						TCAP_KEYS_LOG_TXT : TCAP_KEYS_LOG_BIN);
				}

				if (tp->scrn->debug_scrn_fp != 0)
				{
					strcat(new_title, " ");
					strcat(new_title, tp->scrn->debug_scrn_text ?
						TCAP_SCRN_LOG_TXT : TCAP_SCRN_LOG_BIN);
				}

				strcat(new_title, "]");
			}
		}

		/*----------------------------------------------------------------
		 * send the title string
		 */
#if V_WINDOWS
		if (tp->tcap->db_type == TCAP_DB_WINDOWS)
		{
			tcap_outcmd(tp, S_TTS, s);
			tcap_outcmd(tp, S_TTE, 0);
		}
		else
#endif
		{
			tcap_outcmd(tp, S_TTS, tp->tcap->strs.tts);
			tcap_outs(tp, s);
			tcap_outcmd(tp, S_TTE, tp->tcap->strs.tte);
			tcap_outflush(tp);
		}
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_clear_screen() - clear the screen using current attributes
 */
int tcap_clear_screen (TERMINAL *tp)
{
	int i;
	int j;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		/*----------------------------------------------------------------
		 * use any easy methods if possible
		 */
		if (is_cmd_pres(tp->tcap->strs.cl))
		{
			/*------------------------------------------------------------
			 * clear-screen cmd is present
			 */
			tcap_outcmd(tp, S_CL, tp->tcap->strs.cl);
		}
		else if (is_cmd_pres(tp->tcap->strs.cd))
		{
			/*------------------------------------------------------------
			 * clear-to-end-of-screen cmd is present
			 */
			tcap_outpos(tp, 0, 0);
			tcap_outcmd(tp, S_CD, tp->tcap->strs.cd);
		}
		else if (is_cmd_pres(tp->tcap->strs.ce))
		{
			/*------------------------------------------------------------
			 * clear-to-end-of-line cmd is present
			 */
			for (i=0; i<tp->tcap->ints.maxrows; i++)
			{
				tcap_outpos(tp, i, 0);
				tcap_outcmd(tp, S_CE, tp->tcap->strs.ce);
			}
		}
		else
		{
			/*------------------------------------------------------------
			 * just blank the whole friggin screen
			 */
			for (i=0; i<tp->tcap->ints.maxrows; i++)
			{
				tcap_outpos(tp, i, 0);
				for (j=0; j<tp->tcap->ints.maxcols; j++)
					tcap_outc(tp, ' ');
			}
		}
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_need_ins_final() - check whether final write call needed for
 * last col of last line
 */
int tcap_need_ins_final (const TERMINAL *tp)
{
	if (tp == 0)
		return (FALSE);

	return (tp->tcap->bools.ifc);
}

/*------------------------------------------------------------------------
 * tcap_write_final_char() - output final char via insert logic
 */
int tcap_write_final_char (TERMINAL *tp, int cpc, attr_t cpa,
	int chc, attr_t cha)
{
	if (tp == 0)
		return (-1);

	if (! is_cmd_pres(tp->tcap->strs.ic))
		return (-1);

	TERMINAL_LOCK(tp);
	{
		tcap_outpos(tp, tp->tcap->ints.maxrows-1, tp->tcap->ints.maxcols-2);
		tcap_outattr(tp, cha, chc);
		tcap_outc(tp, chc);
		tcap_outpos(tp, tp->tcap->ints.maxrows-1, tp->tcap->ints.maxcols-2);
		tcap_outcmd(tp, S_IC, tp->tcap->strs.ic);
		tcap_outattr(tp, cpa, cpc);
		tcap_outc(tp, cpc);
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}
