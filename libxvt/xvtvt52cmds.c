/*------------------------------------------------------------------------
 * vt52 terminal emulator commands
 */
#include "xvtcommon.h"
#include "xvtvt52.h"

#define te		(&(td->esc))

/*------------------------------------------------------------------------
 * do any initialization
 */
void xvt_vt52_init (TERMDATA *td)
{
	/*--------------------------------------------------------------------
	 * setup all key structs
	 */  
	td->keys_fn		= xvt_vt52_keys_fn_reg;
	td->keys_ct		= xvt_vt52_keys_ct_reg;
	td->keys_ms		= xvt_vt52_keys_ms_reg;
	td->keys_kp		= xvt_vt52_keys_kp_reg;
	td->keys_pf		= xvt_vt52_keys_pf_reg;
	td->keys_st_up	= xvt_vt52_keys_st_up;
	td->keys_st_dn	= xvt_vt52_keys_st_dn;

	/*--------------------------------------------------------------------
	 * reset all screens & set current screen as reg screen
	 */
	xvt_scrn_reset(td->reg_scrn);
	xvt_scrn_reset(td->alt_scrn);
	td->cur_scrn = td->reg_scrn;

	/*--------------------------------------------------------------------
	 * setup all char sets
	 */
	CUR_SCRN_GCS(td, CHAR_SET_G0) = GCS_US;
	CUR_SCRN_GCS(td, CHAR_SET_G1) = GCS_ALT;
	CUR_SCRN_GCS(td, CHAR_SET_G2) = GCS_US;
	CUR_SCRN_GCS(td, CHAR_SET_G3) = GCS_US;

	/*--------------------------------------------------------------------
	 * set window title back
	 */
	xvt_term_set_win_title(td, td->org_label);
	xvt_term_set_ico_title(td, td->org_label);

	/*--------------------------------------------------------------------
	 * unmap & unlock the keyboard
	 */
	OPTION_SET(td, SM_KbdLocked,   FALSE);
	OPTION_SET(td, SM_BSsendsDel,  FALSE);
	OPTION_SET(td, SM_DELsendsDel, FALSE);
	OPTION_SET(td, SM_AutoNL,      FALSE);
	OPTION_SET(td, SM_LocalEcho,   FALSE);

	/*--------------------------------------------------------------------
	 * force screen to mono mode
	 */
	xvt_term_set_scrn_mode(td, FALSE);

	/*--------------------------------------------------------------------
	 * now clear the screen
	 */
	xvt_curscrn_clear(td, 0);
}

/*------------------------------------------------------------------------
 * output a character to the screen
 */
void xvt_vt52_char_output (TERMDATA *td, int ch)
{
	CELL *	cp;

	/*--------------------------------------------------------------------
	 * debug output
	 */
	{
		char	buf[2];

		buf[0] = ch;
		buf[1] = 0;
		xvt_log_output_chr(td, buf);
	}

	/*--------------------------------------------------------------------
	 * adjust character according to character set
	 */
	ch = xvt_gcs_get_char(CUR_SCRN_GCS(td, CUR_SCRN_CS(td)), ch);

	/*--------------------------------------------------------------------
	 * now display it
	 */
	if (! CUR_SCRN_INS(td))
	{
		/*----------------------------------------------------------------
		 * check if this will wrap to next line
		 *
		 * Note that if the previous char displayed was at the far right
		 * of the screen, we postpone moving to the next line (and possibly
		 * scrolling) until the next char is output.
		 */
		if (CUR_SCRN_X(td) >= CUR_SCRN_COLS(td))
		{
			if (OPTION_GET(td, SM_WrapAround))
			{
				/*--------------------------------------------------------
				 * if autoprint is on, print this line before moving on
				 */
				if (OPTION_GET(td, SM_PrintAuto))
					xvt_scrn_print_line(CUR_SCRN(td), td->prt, CUR_SCRN_Y(td));

				/*--------------------------------------------------------
				 * bump to next line
				 */
				CUR_SCRN_X(td) = 0;
				CUR_SCRN_Y(td)++;

				/*-----------------------------------------------------------
				 * now check if we have to scroll
				 */
				if (CUR_SCRN_Y(td) >= CUR_SCRN_ROWS(td))
				{
					CUR_SCRN_Y(td)--;
					xvt_curscrn_lines_ins(td, 1);
				}
			}
			else
			{
				CUR_SCRN_X(td)--;
			}
		}

		/*----------------------------------------------------------------
		 * display it if it doesn't match char already on screen
		 */
		if (CUR_SCRN_Y(td) > CUR_SCRN_LOCK(td))
		{
			cp = CUR_SCRN_CHAR(td, CUR_SCRN_Y(td), CUR_SCRN_X(td));

			if (cp->at != CUR_SCRN_AT(td) ||
				cp->bg != CUR_SCRN_BG(td) ||
				cp->fg != CUR_SCRN_FG(td) ||
				cp->ch != ch)
			{
				cp->ch = (unsigned char)ch;
				cp->fg = CUR_SCRN_FG(td);
				cp->bg = CUR_SCRN_BG(td);
				cp->at = CUR_SCRN_AT(td);

				xvt_term_disp_char(td, CUR_SCRN_X(td), CUR_SCRN_Y(td), cp->ch,
					cp->fg, cp->bg, cp->at);
			}
		}
	}
	else
	{
		if (CUR_SCRN_Y(td) > CUR_SCRN_LOCK(td))
		{
			xvt_curscrn_chars_ins(td, 1);

			cp = CUR_SCRN_CHAR(td, CUR_SCRN_Y(td), CUR_SCRN_X(td));
			cp->ch = (unsigned char)ch;
			cp->fg = CUR_SCRN_FG(td);
			cp->bg = CUR_SCRN_BG(td);
			cp->at = CUR_SCRN_AT(td);

			xvt_term_disp_char(td, CUR_SCRN_X(td), CUR_SCRN_Y(td), cp->ch,
				cp->fg, cp->bg, cp->at);
		}
	}

	/*--------------------------------------------------------------------
	 * bump x cursor position
	 */
	CUR_SCRN_X(td)++;

	/*--------------------------------------------------------------------
	 * check margin bell
	 */
	if (td->read_pending && OPTION_GET(td, SM_MarginBell))
	{
		if (CUR_SCRN_X(td) == (CUR_SCRN_COLS(td) - td->xd->mbcols))
			xvt_term_bell(td);
	}

	/*--------------------------------------------------------------------
	 * now check out position again
	 */
#if 0
	if (CUR_SCRN_X(td) == CUR_SCRN_COLS(td))
	{
		if (OPTION_GET(td, SM_WrapAround))
		{
			xvt_vt52_CTL_LF(td);
			CUR_SCRN_X(td) = 0;
		}
		else
		{
			CUR_SCRN_X(td)--;
		}
	}
#endif

	td->read_pending = FALSE;
}

/*------------------------------------------------------------------------
 * process printing
 */
void xvt_vt52_char_print (TERMDATA *td, int ch)
{
	/*--------------------------------------------------------------------
	 * if currently reading an esc-seq, cache this char into the buffer
	 */
	if (td->prt_mode == PRT_MODE_ESC)
	{
		te->esc_chars[te->esc_char_cnt++] = ch;
		te->esc_chars[te->esc_char_cnt  ] = 0;

		/*----------------------------------------------------------------
		 * an alpha char will end esc-seq processing
		 */
		if (isalpha(ch))
		{
			/*------------------------------------------------------------
			 * check if this is an end-printing esc-seq.
			 */
			if (strcmp(te->esc_chars, "\033X") == 0)
			{
				te->reading_esc_seq	= FALSE;

				xvt_log_output_seq(td, "stop print pass-thru");
				if (! OPTION_GET(td, SM_PrintAuto))
					xvt_term_printer_close(td);
				td->prt_mode = PRT_MODE_NONE;
			}
			else
			{
				td->prt_mode = PRT_MODE_NORM;
				xvt_prt_output_str(td->prt, te->esc_chars);
			}

			te->esc_chars[0]	= 0;
			te->esc_char_cnt	= 0;
			te->vt_mode			= VT52_TOP;
		}

		return;
	}

	/*--------------------------------------------------------------------
	 * not reading an esc-seq, check if this will start one
	 */
	if (ch == 0x1b)
	{
		te->esc_chars[te->esc_char_cnt++] = ch;
		te->esc_chars[te->esc_char_cnt  ] = 0;
		td->prt_mode = PRT_MODE_ESC;
		return;
	}

	/*--------------------------------------------------------------------
	 * just a reg char
	 */
	xvt_prt_output_chr(td->prt, ch);
}

/*========================================================================
 * control char routines
 *========================================================================
 */

/*------------------------------------------------------------------------
 * BEL		ring the bell
 */
void xvt_vt52_CTL_BEL (TERMDATA *td)
{
	xvt_term_bell(td);
}

/*------------------------------------------------------------------------
 * BS		backspace
 */
void xvt_vt52_CTL_BS (TERMDATA *td)
{
	if (CUR_SCRN_X(td) > 0)
	{
		if (CUR_SCRN_X(td) >= CUR_SCRN_COLS(td))
			CUR_SCRN_X(td) = CUR_SCRN_COLS(td) - 1;
		CUR_SCRN_X(td)--;
	}
	else if (OPTION_GET(td, SM_ReverseWrap))
	{
		if (CUR_SCRN_Y(td) > 0)
		{
			CUR_SCRN_Y(td)--;
			CUR_SCRN_X(td) = CUR_SCRN_COLS(td) - 1;
		}
	}
}

/*------------------------------------------------------------------------
 * HT		horizontal tab (just move, no writing of chars)
 */
void xvt_vt52_CTL_HT (TERMDATA *td)
{
	int x;

	for (x=CUR_SCRN_X(td)+1; x<CUR_SCRN_COLS(td); x++)
	{
		if (CUR_SCRN_TABSTOP(td, x))
			break;
	}

	if (x < CUR_SCRN_COLS(td))
		CUR_SCRN_X(td) = x;
	else
		CUR_SCRN_X(td) = CUR_SCRN_COLS(td) - 1;
}

/*------------------------------------------------------------------------
 * LF		line-feed
 */
void xvt_vt52_CTL_LF (TERMDATA *td)
{
	/*--------------------------------------------------------------------
	 * if autoprint is on, print this line before moving on
	 */
	if (OPTION_GET(td, SM_PrintAuto))
		xvt_scrn_print_line(CUR_SCRN(td), td->prt, CUR_SCRN_Y(td));

	/*--------------------------------------------------------------------
	 * now move to next line
	 */
	CUR_SCRN_Y(td)++;
	if (CUR_SCRN_Y(td) >= CUR_SCRN_ROWS(td))
	{
		CUR_SCRN_Y(td)--;
		xvt_curscrn_lines_ins(td, 1);
	}
}

/*------------------------------------------------------------------------
 * CR		carriage return
 */
void xvt_vt52_CTL_CR (TERMDATA *td)
{
	CUR_SCRN_X(td) = 0;
}

/*========================================================================
 * TOP mode commands
 *========================================================================
 */

/*------------------------------------------------------------------------
 * CUU	move the cursor up
 */
void xvt_vt52_TOP_CUU (TERMDATA *td)
{
	xvt_log_output_seq(td, "up 1 line");

	CUR_SCRN_Y(td) -= 1;
	if (CUR_SCRN_Y(td) < 0)
		CUR_SCRN_Y(td) = 0;
}

/*------------------------------------------------------------------------
 * CUD	move the cursor down
 */
void xvt_vt52_TOP_CUD (TERMDATA *td)
{
	xvt_log_output_seq(td, "down 1 line");

	CUR_SCRN_Y(td) += 1;
	if (CUR_SCRN_Y(td) >= CUR_SCRN_ROWS(td))
		CUR_SCRN_Y(td) = CUR_SCRN_ROWS(td) - 1;
}

/*------------------------------------------------------------------------
 * CUF	move the cursor right
 */
void xvt_vt52_TOP_CUF (TERMDATA *td)
{
	xvt_log_output_seq(td, "right 1 char");

	CUR_SCRN_X(td) += 1;
	if (CUR_SCRN_X(td) >= CUR_SCRN_COLS(td))
		CUR_SCRN_X(td) = CUR_SCRN_COLS(td) - 1;
}

/*------------------------------------------------------------------------
 * CUB	move the cursor left
 */
void xvt_vt52_TOP_CUB (TERMDATA *td)
{
	xvt_log_output_seq(td, "left 1 char");

	CUR_SCRN_X(td) -= 1;
	if (CUR_SCRN_X(td) < 0)
		CUR_SCRN_X(td) = 0;
}

/*------------------------------------------------------------------------
 * CUH	move the cursor home
 */
void xvt_vt52_TOP_CUH (TERMDATA *td)
{
	xvt_log_output_seq(td, "move to (0,0)");

	CUR_SCRN_X(td) = 0;
	CUR_SCRN_Y(td) = 0;
}

/*------------------------------------------------------------------------
 * TOP_RI			index up (reverse index)
 */
void xvt_vt52_TOP_RI (TERMDATA *td)
{
	xvt_log_output_seq(td, "index up");

	if (CUR_SCRN_Y(td) <= 0)
		xvt_curscrn_scroll(td, 1);
	else
		CUR_SCRN_Y(td)--;
}

/*------------------------------------------------------------------------
 * ED	erase display (with current attrs)
 *
 * The current cursor position is not changed.
 */
void xvt_vt52_TOP_ED (TERMDATA *td)
{
	/*--------------------------------------------------------------------
	 * clear from cursor to EOS
	 */
	xvt_log_output_seq(td, "clear-to-eos");

	xvt_curscrn_chars_clr(td, 1, 0);
	if (CUR_SCRN_Y(td) < CUR_SCRN_ROWS(td) - 1)
	{
		xvt_curscrn_lines_clr(td, CUR_SCRN_Y(td) + 1,
			(CUR_SCRN_ROWS(td) - (CUR_SCRN_Y(td) + 1)), 0);
	}
}

/*------------------------------------------------------------------------
 * EL	erase line (with current attrs)
 *
 * The current cursor position is not changed.
 */
void xvt_vt52_TOP_EL (TERMDATA *td)
{
	/*--------------------------------------------------------------------
	 * clear to EOL
	 */
	xvt_log_output_seq(td, "clear-to-eol");
	xvt_curscrn_chars_clr(td, 1, 0);
}

/*------------------------------------------------------------------------
 * TOP_DA			return terminal ID
 */
void xvt_vt52_TOP_DA (TERMDATA *td)
{
	xvt_log_output_seq(td, "send device attributes");
	xvt_term_write_report(td, "\033/Z");
}

/*------------------------------------------------------------------------
 * DECKPAM	application keypad
 */
void xvt_vt52_TOP_DECKPAM (TERMDATA *td)
{
	xvt_log_output_seq(td, "application keypad on");
	td->keys_kp = xvt_vt52_keys_kp_reg;
}

/*------------------------------------------------------------------------
 * DECKPNM	normal keypad
 */
void xvt_vt52_TOP_DECKPNM (TERMDATA *td)
{
	xvt_log_output_seq(td, "application keypad off");
	td->keys_kp = xvt_vt52_keys_kp_cur;
}

/*------------------------------------------------------------------------
 * DECANM	switch to ANSI mode
 */
void xvt_vt52_TOP_DECANM (TERMDATA *td)
{
	xvt_log_output_seq(td, "switch to %s mode", XVT_S_OPT_TERMINAL);
	xvt_emul_set_tbl(td, XVT_S_OPT_TERMINAL);
}

/*------------------------------------------------------------------------
 * DECSAP	autoprint on
 */
void xvt_vt52_TOP_DECSAP (TERMDATA *td)
{
	xvt_log_output_seq(td, "start autoprint mode");
	xvt_term_printer_open(td);
	OPTION_SET(td, SM_PrintAuto, TRUE);
}

/*------------------------------------------------------------------------
 * DECEAP	autoprint off
 */
void xvt_vt52_TOP_DECEAP (TERMDATA *td)
{
	xvt_log_output_seq(td, "end autoprint mode");
	if (td->prt_mode == PRT_MODE_NONE)
		xvt_term_printer_close(td);
	OPTION_SET(td, SM_PrintAuto, FALSE);
}

/*------------------------------------------------------------------------
 * DECSPC	start print controller mode
 */
void xvt_vt52_TOP_DECSPC (TERMDATA *td)
{
	xvt_log_output_seq(td, "start print controller mode");
	xvt_term_printer_open(td);
	td->prt_mode = PRT_MODE_NORM;
}

/*------------------------------------------------------------------------
 * DECEPC	end print controller mode
 */
void xvt_vt52_TOP_DECEPC (TERMDATA *td)
{
	xvt_log_output_seq(td, "end print controller mode");
	if (! OPTION_GET(td, SM_PrintAuto))
		xvt_term_printer_close(td);
	td->prt_mode = PRT_MODE_NONE;
}

/*------------------------------------------------------------------------
 * DECPSC	print screen
 */
void xvt_vt52_TOP_DECPSC (TERMDATA *td)
{
	xvt_log_output_seq(td, "print screen");
	xvt_term_screen_print(td, FALSE, FALSE);
}

/*------------------------------------------------------------------------
 * DECPCL	print current line
 */
void xvt_vt52_TOP_DECPCL (TERMDATA *td)
{
	xvt_log_output_seq(td, "print current line");
	xvt_term_screen_print_line(td);
}

/*------------------------------------------------------------------------
 * SGM		start graphics mode
 */
void xvt_vt52_TOP_SGM (TERMDATA *td)
{
	xvt_log_output_seq(td, "start graphics mode");
	CUR_SCRN_CS(td) = CHAR_SET_G1;
}

/*------------------------------------------------------------------------
 * EGM		end graphics mode
 */
void xvt_vt52_TOP_EGM (TERMDATA *td)
{
	xvt_log_output_seq(td, "end graphics mode");
	CUR_SCRN_CS(td) = CHAR_SET_G0;
}

/*========================================================================
 * PRM mode commands
 *========================================================================
 */

/*------------------------------------------------------------------------
 * CUP		move to col;row
 */
void xvt_vt52_PRM_CUP (TERMDATA *td)
{
	int y	= te->data_chars[0] - ' ';
	int x	= te->data_chars[1] - ' ';

	xvt_log_output_seq(td, "move to (%d,%d)", x, y);

	if (x < 0)					x = 0;
	if (x >= CUR_SCRN_COLS(td))	x = CUR_SCRN_COLS(td) - 1;

	if (y < 0)					y = 0;
	if (y >= CUR_SCRN_ROWS(td))	y = CUR_SCRN_ROWS(td) - 1;

	CUR_SCRN_X(td) = x;
	CUR_SCRN_Y(td) = y;
}
