/*------------------------------------------------------------------------
 * device output routines
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * tcap_outflush() - flush any buffered output
 */
int tcap_outflush (TERMINAL *tp)
{
	int rc;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * now do the flush
	 */
	TERMINAL_LOCK(tp);
	{
		/*----------------------------------------------------------------
		 * write any buffered chars
		 */
		rc = term_screen_dev_write(tp->term, tp->scrn->out_buf,
			tp->scrn->out_buf_count);

		/*----------------------------------------------------------------
		 * set buffered char count to 0
		 */
		tp->scrn->out_buf_count = 0;

		/*----------------------------------------------------------------
		 * flush debug file
		 */
		if (tp->scrn->debug_scrn_fp != 0)
			fflush(tp->scrn->debug_scrn_fp);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_outch() - output a character (either data or cmd part)
 */
int tcap_outch (TERMINAL *tp, int c, int data)
{
	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * output the char
	 */
	TERMINAL_LOCK(tp);
	{
#if V_WINDOWS
		/*----------------------------------------------------------------
		 * Win98 has a bug in that if you automatically move to the last
		 * column, the system will "wrap" it to the first col of that row.
		 * So, to avoid that problem, if we are about to buffer a char in
		 * the last col, we "move" to that position first.
		 */
		if (tp->tcap->db_type == TCAP_DB_WINDOWS)
		{
			if (tp->scrn->curr_col == (tp->tcap->ints.maxcols - 1))
			{
				tcap_outparm(tp, S_CM, tp->scrn->curr_row, tp->scrn->curr_col);
			}
		}
#endif

		/*----------------------------------------------------------------
		 * buffer this char
		 */
		tp->scrn->out_buf[tp->scrn->out_buf_count++] = (unsigned char)c;

		/*----------------------------------------------------------------
		 * if buffer is full, flush it
		 */
		if (tp->scrn->out_buf_count == tp->scrn->out_buf_size)
			tcap_outflush(tp);

		/*----------------------------------------------------------------
		 * debug print
		 */
		if (tp->scrn->debug_scrn_fp != 0)
		{
			if (data || ! tp->scrn->debug_scrn_text)
				putc(c, tp->scrn->debug_scrn_fp);
			tp->scrn->debug_scrn_mode = ! data;

			/*------------------------------------------------------------
			 * If doing text debugging & we have AM & we are outputting
			 * data & we are at EOL, output an nl to avoid real long lines.
			 */
			if (data && tp->scrn->debug_scrn_text && tp->tcap->bools.am)
			{
				if (tp->scrn->curr_col == (tp->tcap->ints.maxcols - 1))
				{
					putc('\n', tp->scrn->debug_scrn_fp);
					tp->scrn->debug_scrn_mode = TRUE;
				}
			}
		}
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_outchar() - output a char from a parameterized cmd string
 */
int tcap_outchar (TERMINAL *tp, int c)
{
	return tcap_outch(tp, c, FALSE);
}

/*------------------------------------------------------------------------
 * tcap_outcmd() - output a cmd
 */
int tcap_outcmd (TERMINAL *tp, SCRN_CMD cmd, const char *s)
{
	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * bail if string not present
	 */
	if (! is_cmd_pres(s))
		return (-1);

	TERMINAL_LOCK(tp);
	{
		/*----------------------------------------------------------------
		 * debug print
		 */
		if (tp->scrn->debug_scrn_fp != 0 && tp->scrn->debug_scrn_text)
		{
			const char *p = tcap_tc_get_cmd(tp->tcap, cmd);

			if (p != 0)
			{
				switch (cmd)
				{
				case S_FG:
				case S_BG:
				case S_CM:
				case S_CS:
					break;

				default:
					if (! tp->scrn->debug_scrn_mode)
						fprintf(tp->scrn->debug_scrn_fp, "\n");
					tp->scrn->debug_scrn_mode = TRUE;
					fprintf(tp->scrn->debug_scrn_fp, "<%s>\n", p);
					break;
				}
				fflush(tp->scrn->debug_scrn_fp);
			}
		}

		/*----------------------------------------------------------------
		 * now output the cmd
		 */
#if V_WINDOWS
		if (tp->tcap->db_type == TCAP_DB_WINDOWS)
		{
			switch (cmd)
			{
			case S_BL:			/* bell */
				break;

			case S_VE:			/* cursor normal */
			case S_VS:			/* cursor very visible */
				term_win_set_cursor(tp->term, TRUE);
				break;

			case S_VI:			/* cursor invisible */
				term_win_set_cursor(tp->term, FALSE);
				break;

			case S_STS:			/* standout start */
				tcap_outparm(tp, S_CS,
					A_FG_CLRNUM(tp->scrn->orig_attr),
					A_BG_CLRNUM(tp->scrn->orig_attr));
				break;

			case S_STE:			/* standout stop */
				tcap_outparm(tp, S_CS,
					A_BG_CLRNUM(tp->scrn->orig_attr),
					A_FG_CLRNUM(tp->scrn->orig_attr));
				break;

			case S_MBEG:		/* mouse start */
				term_win_set_mouse(tp->term, TRUE);
				break;

			case S_MEND:		/* mouse stop */
				term_win_set_mouse(tp->term, FALSE);
				break;

			case S_TTS:			/* start window title */
				term_win_set_title(tp->term, s);
				break;

			case S_TTE:			/* end window title */
				break;
			}
		}
		else
#endif
		{
			if (*s)
			{
				tcap_outflush(tp);
				tcap_db_outs(tp, s, tcap_outchar);
			}
		}
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_outparm() - process a cmd which is parameterized
 */
int tcap_outparm (TERMINAL *tp, SCRN_CMD cmd, int p1, int p2)
{
	int	rc = -1;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * now do it
	 */
	TERMINAL_LOCK(tp);
	{
		/*----------------------------------------------------------------
		 * debug print
		 */
		if (tp->scrn->debug_scrn_fp != 0 && tp->scrn->debug_scrn_text)
		{
			const char *p = tcap_tc_get_cmd(tp->tcap, cmd);

			if (! tp->scrn->debug_scrn_mode)
				fprintf(tp->scrn->debug_scrn_fp, "\n");
			tp->scrn->debug_scrn_mode = TRUE;

			switch (cmd)
			{
			case S_CM:
				fprintf(tp->scrn->debug_scrn_fp, "<%s:%d,%d>\n",
					p, p1, p2);
				fflush(tp->scrn->debug_scrn_fp);
				break;

			case S_CS:
				fprintf(tp->scrn->debug_scrn_fp, "<%s:%s/%s>\n", p,
					tcap_get_fg_name_by_num(p2), tcap_get_bg_name_by_num(p1));
				fflush(tp->scrn->debug_scrn_fp);
				break;

			case S_FG:
				fprintf(tp->scrn->debug_scrn_fp, "<%s:%s>\n",
					p, tcap_get_fg_name_by_num(p1));
				fflush(tp->scrn->debug_scrn_fp);
				break;

			case S_BG:
				fprintf(tp->scrn->debug_scrn_fp, "<%s:%s>\n",
					p, tcap_get_bg_name_by_num(p1));
				fflush(tp->scrn->debug_scrn_fp);
				break;
			}
		}

		/*----------------------------------------------------------------
		 * parameterize the data & output it
		 */
#if V_WINDOWS
		if (tp->tcap->db_type == TCAP_DB_WINDOWS)
		{
			tcap_outflush(tp);

			switch (cmd)
			{
			case S_CM:			/* cursor move */
				rc = term_win_set_position(tp->term, p1, p2);
				break;

			case S_CS:			/* color set */
				rc = term_win_set_color(tp->term, p2, p1);
				break;
			}
		}
		else
#endif
		{
			char buf[128];

			switch (cmd)
			{
			case S_CM:
			case S_CS:
				tcap_eval_parm(tp, buf, cmd, p1, p2);
				rc = tcap_outcmd(tp, cmd, buf);
				break;

			case S_FG:
				rc = tcap_outcmd(tp, S_FG, tp->tcap->strs.clr_fg[p1]);
				break;

			case S_BG:
				rc = tcap_outcmd(tp, S_BG, tp->tcap->strs.clr_bg[p1]);
				break;
			}
		}
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}
