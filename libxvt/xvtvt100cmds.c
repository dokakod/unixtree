/*------------------------------------------------------------------------
 * vt100 terminal emulator commands
 */
#include "xvtcommon.h"
#include "xvtvt100.h"

#define te		(&(td->esc))

/*------------------------------------------------------------------------
 * misc other defines
 */
#define PLURAL(n)		(n == 1 ? "" : "s")

/*------------------------------------------------------------------------
 * do any initialization
 */
void xvt_vt100_init (TERMDATA *td)
{
	/*--------------------------------------------------------------------
	 * setup all char sets
	 */  
	CUR_SCRN_GCS(td, CHAR_SET_G0) = GCS_US;
	CUR_SCRN_GCS(td, CHAR_SET_G1) = GCS_US;
	CUR_SCRN_GCS(td, CHAR_SET_G2) = GCS_US;
	CUR_SCRN_GCS(td, CHAR_SET_G3) = GCS_US;

	/*--------------------------------------------------------------------
	 * setup all key structs
	 */  
	td->keys_fn		= xvt_vt100_keys_fn_reg;
	td->keys_ct		= xvt_vt100_keys_ct_reg;
	td->keys_ms		= xvt_vt100_keys_ms_reg;
	td->keys_kp		= xvt_vt100_keys_kp_reg;
	td->keys_pf		= xvt_vt100_keys_pf_reg;
	td->keys_st_up	= xvt_vt100_keys_st_up;
	td->keys_st_dn	= xvt_vt100_keys_st_dn;
}

/*------------------------------------------------------------------------
 * check column position for scrolling
 */
static void xvt_vt100_col_check (TERMDATA *td)
{
	/*--------------------------------------------------------------------
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
			/*------------------------------------------------------------
			 * if autoprint is on, print this line before moving on
			 */
			if (OPTION_GET(td, SM_PrintAuto))
				xvt_term_screen_print_line(td);

			/*------------------------------------------------------------
			 * bump to next line
			 */
			CUR_SCRN_X(td) = 0;
			CUR_SCRN_Y(td)++;

			/*------------------------------------------------------------
			 * now check if we have to scroll
			 */
			if (CUR_SCRN_Y(td) > CUR_SCRN_BOT(td))
			{
				CUR_SCRN_Y(td)--;
				xvt_curscrn_lines_ins(td, 1);
			}
			else if (CUR_SCRN_Y(td) >= CUR_SCRN_ROWS(td))
			{
				CUR_SCRN_Y(td)--;
			}
		}
		else
		{
			CUR_SCRN_X(td)--;
		}
	}
}

/*------------------------------------------------------------------------
 * output a character to the screen
 */
void xvt_vt100_char_output (TERMDATA *td, int ch)
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
	td->prev_char = ch;

	if (CUR_SCRN_TMPCS(td) != CHAR_SET_NONE)
	{
		ch = xvt_gcs_get_char(CUR_SCRN_GCS(td, CUR_SCRN_TMPCS(td)), ch);
		CUR_SCRN_TMPCS(td) = CHAR_SET_NONE;
	}
	else
	{
		ch = xvt_gcs_get_char(CUR_SCRN_GCS(td, CUR_SCRN_CS(td)), ch);
	}

	/*--------------------------------------------------------------------
	 * now display it
	 */
	if (! CUR_SCRN_INS(td))
	{
		xvt_vt100_col_check(td);

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
			xvt_vt100_CTL_LF(td);
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
void xvt_vt100_char_print (TERMDATA *td, int ch)
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
			if (strcmp(te->esc_chars, "\033[4i") == 0)
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
			te->vt_mode			= VT100_TOP;
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
 * static support functions
 *========================================================================
 */

/*------------------------------------------------------------------------
 * XVT_KBDMAP	set keyboard mapping
 */
static void xvt_vt100_kbdmap (TERMDATA *td, int state)
{
	if (state)
	{
		xvt_log_output_seq(td, "kbd-map %d on", state);

		/*----------------------------------------------------------------
		 * set kbd-mapping on
		 */
		OPTION_SET(td, SM_KbdMapMode, (state == 1 ? Kbd_MapSome : Kbd_MapAll));
		td->keys_ct = xvt_vt100_keys_ct_map;
		td->keys_pf = xvt_vt100_keys_pf_map;
		td->keys_ms = xvt_vt100_keys_ms_map;
		td->keys_kp = xvt_vt100_keys_kp_map;

		/*----------------------------------------------------------------
		 * We don't set kbd-state on yet because the user may not
		 * be ready yet. We'll let him get it the first time the
		 * kbd is read.
		 */
#if 0
		xvt_term_set_kbd_state(td, -1);
#endif
	}
	else
	{
		xvt_log_output_seq(td, "kbd-map off");

		/*----------------------------------------------------------------
		 * set kbd-state off
		 */
		xvt_term_set_kbd_state(td, 0);

		/*----------------------------------------------------------------
		 * set kbd-mapping off
		 */
		OPTION_SET(td, SM_KbdMapMode, Kbd_MapNone);
		td->keys_ct = xvt_vt100_keys_ct_reg;
		td->keys_pf = xvt_vt100_keys_pf_reg;
		td->keys_ms = xvt_vt100_keys_ms_reg;
		td->keys_kp = xvt_vt100_keys_kp_reg;
	}
}

/*------------------------------------------------------------------------
 * DECCKM	application cursor keys on|off
 */
static void xvt_vt100_DECCKM (TERMDATA *td, int bf)
{
	td->keys_kp = (bf ? xvt_vt100_keys_kp_apl : xvt_vt100_keys_kp_reg);
}

/*------------------------------------------------------------------------
 * DECNKM	application keypad keys on|off
 */
static void xvt_vt100_DECNKM (TERMDATA *td, int bf)
{
	td->keys_kp = (bf ? xvt_vt100_keys_kp_cur : xvt_vt100_keys_kp_reg);
}

/*------------------------------------------------------------------------
 * ICON		process icon data
 */
static void xvt_vt100_ICON_BITS (TERMDATA *td)
{
	int		w;
	int		h;
	int		i;
	int		b;
	char *	c;

	/*--------------------------------------------------------------------
	 * reading an icon
	 *
	 * Icon data is as follows:
	 *
	 *	width  (in pixels) (must be multiple of 8)
	 *	height (in pixels) (must be multiple of 8)
	 *	((width * height) / 8) data-bytes of pixel data.
	 *
	 *	width	one byte  encoded as (w + ' ')
	 *	height	one byte  encoded as (h + ' ')
	 *	data	two bytes encoded as ((b >> 4) + ' ', (b & 0x0f) + ' ')
	 *
	 * We impose a limit of 64 x 64 pixels on the size.
	 */
	if (te->data_cnt <= 2)
		return;

	w = (te->data_chars[0] - ' ');
	h = (te->data_chars[1] - ' ');

	if (w > 64)	w = 64;
	if (h > 64)	h = 64;

	b = (w * h) / 8;

	if (((b * 2) + 2) != te->data_cnt)
		return;

	c = te->data_chars + 2;

	/*--------------------------------------------------------------------
	 * store icon data
	 */
	td->icon_width	= w;
	td->icon_height	= h;

	for (i=0; i<b; i++)
	{
		int	l = (*c++ - ' ') << 4;
		int r = (*c++ - ' ');

		td->icon_bits[i] = (l | r);
	}

	/*--------------------------------------------------------------------
	 * now set it
	 */
	xvt_term_set_icon(td, td->icon_width, td->icon_height, td->icon_bits);
}

/*========================================================================
 * control char routines
 *========================================================================
 */

/*------------------------------------------------------------------------
 * ENQ		enquire terminal status
 */
void xvt_vt100_CTL_ENQ (TERMDATA *td)
{
	xvt_vt100_CSI_DA(td);
}

/*------------------------------------------------------------------------
 * BEL		ring the bell
 */
void xvt_vt100_CTL_BEL (TERMDATA *td)
{
	xvt_term_bell(td);
}

/*------------------------------------------------------------------------
 * BS		backspace
 */
void xvt_vt100_CTL_BS (TERMDATA *td)
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
void xvt_vt100_CTL_HT (TERMDATA *td)
{
	int x;

	xvt_vt100_col_check(td);

	for (x=CUR_SCRN_X(td) + 1; x<CUR_SCRN_COLS(td); x++)
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
void xvt_vt100_CTL_LF (TERMDATA *td)
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
	if (CUR_SCRN_Y(td) > CUR_SCRN_BOT(td))
	{
		CUR_SCRN_Y(td)--;
		xvt_curscrn_lines_ins(td, 1);
	}
	else if (CUR_SCRN_Y(td) >= CUR_SCRN_ROWS(td))
	{
		CUR_SCRN_Y(td)--;
	}

	/*--------------------------------------------------------------------
	 * check auto-nl mode
	 */
	if (OPTION_GET(td, SM_AutoNL))
		CUR_SCRN_X(td) = 0;
}

/*------------------------------------------------------------------------
 * CR		carriage return
 */
void xvt_vt100_CTL_CR (TERMDATA *td)
{
	CUR_SCRN_X(td) = 0;
}

/*------------------------------------------------------------------------
 * SO		alt-chars on (switch to G1)
 */
void xvt_vt100_CTL_SO (TERMDATA *td)
{
	CUR_SCRN_CS(td) = CHAR_SET_G1;
	xvt_log_output_seq(td, "select G1 char-set");
}

/*------------------------------------------------------------------------
 * SI		alt-chars off (switch to G0)
 */
void xvt_vt100_CTL_SI (TERMDATA *td)
{
	CUR_SCRN_CS(td) = CHAR_SET_G0;
	xvt_log_output_seq(td, "select G0 char-set");
}

/*========================================================================
 * TOP mode commands
 *========================================================================
 */

/*------------------------------------------------------------------------
 * DECSC	save cursor position
 */
void xvt_vt100_TOP_DECSC (TERMDATA *td)
{
	xvt_log_output_seq(td, "save-cursor");

	CUR_SCRN_SAVE_MODE(td)	= CUR_SCRN_MODE(td);
	CUR_SCRN_SAVE_FG(td)	= CUR_SCRN_FG(td);
	CUR_SCRN_SAVE_BG(td)	= CUR_SCRN_BG(td);
	CUR_SCRN_SAVE_AT(td)	= CUR_SCRN_AT(td);
	CUR_SCRN_SAVE_GCS(td, CHAR_SET_G0)	= CUR_SCRN_GCS(td, CHAR_SET_G0);
	CUR_SCRN_SAVE_GCS(td, CHAR_SET_G1)	= CUR_SCRN_GCS(td, CHAR_SET_G1);
	CUR_SCRN_SAVE_GCS(td, CHAR_SET_G2)	= CUR_SCRN_GCS(td, CHAR_SET_G2);
	CUR_SCRN_SAVE_GCS(td, CHAR_SET_G3)	= CUR_SCRN_GCS(td, CHAR_SET_G3);
	CUR_SCRN_SAVE_CS(td)	= CUR_SCRN_CS(td);
	CUR_SCRN_SAVE_X(td)		= CUR_SCRN_X(td);
	CUR_SCRN_SAVE_Y(td)		= CUR_SCRN_Y(td);
}

/*------------------------------------------------------------------------
 * DECRC	restore cursor position
 */
void xvt_vt100_TOP_DECRC (TERMDATA *td)
{
	xvt_log_output_seq(td, "restore-cursor");

	xvt_term_set_scrn_mode(td, CUR_SCRN_SAVE_MODE(td));

	CUR_SCRN_FG(td)		= CUR_SCRN_SAVE_FG(td);
	CUR_SCRN_BG(td)		= CUR_SCRN_SAVE_BG(td);
	CUR_SCRN_AT(td)		= CUR_SCRN_SAVE_AT(td);
	CUR_SCRN_GCS(td, CHAR_SET_G0)	= CUR_SCRN_SAVE_GCS(td, CHAR_SET_G0);
	CUR_SCRN_GCS(td, CHAR_SET_G1)	= CUR_SCRN_SAVE_GCS(td, CHAR_SET_G1);
	CUR_SCRN_GCS(td, CHAR_SET_G2)	= CUR_SCRN_SAVE_GCS(td, CHAR_SET_G2);
	CUR_SCRN_GCS(td, CHAR_SET_G3)	= CUR_SCRN_SAVE_GCS(td, CHAR_SET_G3);
	CUR_SCRN_CS(td)		= CUR_SCRN_SAVE_CS(td);
	CUR_SCRN_X(td)		= CUR_SCRN_SAVE_X(td);
	CUR_SCRN_Y(td)		= CUR_SCRN_SAVE_Y(td);
}

/*------------------------------------------------------------------------
 * DECKPAM	application keypad
 */
void xvt_vt100_TOP_DECKPAM (TERMDATA *td)
{
	xvt_log_output_seq(td, "application keypad on");
	xvt_vt100_DECNKM(td, TRUE);
}

/*------------------------------------------------------------------------
 * DECKPNM	normal keypad
 */
void xvt_vt100_TOP_DECKPNM (TERMDATA *td)
{
	xvt_log_output_seq(td, "application keypad off");
	xvt_vt100_DECNKM(td, FALSE);
}

/*------------------------------------------------------------------------
 * RIS		reset terminal
 */
void xvt_vt100_TOP_RIS (TERMDATA *td)
{
	xvt_log_output_seq(td, "reset terminal");

	/*--------------------------------------------------------------------
	 * reset all screens & set current screen as reg screen
	 */
	xvt_scrn_reset(td->reg_scrn);
	xvt_scrn_reset(td->alt_scrn);
	td->cur_scrn = td->reg_scrn;

	/*--------------------------------------------------------------------
	 * set window title back
	 */
	xvt_term_set_win_title(td, td->org_label);
	xvt_term_set_ico_title(td, td->org_label);

	/*--------------------------------------------------------------------
	 * set normal keypad & cursor
	 */
	xvt_vt100_DECCKM(td, FALSE);
	xvt_vt100_DECNKM(td, FALSE);

	/*--------------------------------------------------------------------
	 * unmap & unlock the keyboard
	 */
	xvt_vt100_kbdmap(td, 0);
	OPTION_SET(td, SM_KbdLocked,   FALSE);
	OPTION_SET(td, SM_BSsendsDel,  FALSE);
	OPTION_SET(td, SM_DELsendsDel, FALSE);
	OPTION_SET(td, SM_AutoNL,      FALSE);
	OPTION_SET(td, SM_LocalEcho,   FALSE);

	/*--------------------------------------------------------------------
	 * reset tty modes
	 */
	xvt_pty_setmode(td->pd, &td->pd->tty);

	/*--------------------------------------------------------------------
	 * reset all char sets
	 */
	CUR_SCRN_GCS(td, CHAR_SET_G0) = GCS_US;
	CUR_SCRN_GCS(td, CHAR_SET_G1) = GCS_US;
	CUR_SCRN_GCS(td, CHAR_SET_G2) = GCS_US;
	CUR_SCRN_GCS(td, CHAR_SET_G3) = GCS_US;

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
 * LS2		invoke G2 char set
 */
void xvt_vt100_TOP_LS2 (TERMDATA *td)
{
	xvt_log_output_seq(td, "select G2 char set");

	CUR_SCRN_CS(td) = CHAR_SET_G2;
}

/*------------------------------------------------------------------------
 * LS3		invoke G3 char set
 */
void xvt_vt100_TOP_LS3 (TERMDATA *td)
{
	xvt_log_output_seq(td, "select G3 char set");

	CUR_SCRN_CS(td) = CHAR_SET_G3;
}

/*------------------------------------------------------------------------
 * LS1R		invoke G1 char set as GR
 */
void xvt_vt100_TOP_LS1R (TERMDATA *td)
{
	xvt_log_output_seq(td, "select G1 char set as GR");

	CUR_SCRN_CS(td) = CHAR_SET_G1;
}

/*------------------------------------------------------------------------
 * LS2R		invoke G2 char set as GR
 */
void xvt_vt100_TOP_LS2R (TERMDATA *td)
{
	xvt_log_output_seq(td, "select G2 char set as GR");

	CUR_SCRN_CS(td) = CHAR_SET_G2;
}

/*------------------------------------------------------------------------
 * LS3R		invoke G3 char set as GR
 */
void xvt_vt100_TOP_LS3R (TERMDATA *td)
{
	xvt_log_output_seq(td, "select G3 char set as GR");

	CUR_SCRN_CS(td) = CHAR_SET_G3;
}

/*------------------------------------------------------------------------
 * TOP_DEVEND		end device processing
 */
void xvt_vt100_TOP_DEVEND (TERMDATA *td)
{
	switch (te->vt_type)
	{
	case DEV_DCS:
		xvt_log_output_seq(td, "DCS");
		break;

	case DEV_PM:
		xvt_log_output_seq(td, "PM");
		break;

	case DEV_SOS:
		xvt_log_output_seq(td, "SOS");
		break;

	case DEV_APC:
		xvt_log_output_seq(td, "APC");
		xvt_vt100_ICON_BITS(td);
		break;
	}
}

/*------------------------------------------------------------------------
 * TOP_DA			return terminal ID
 */
void xvt_vt100_TOP_DA (TERMDATA *td)
{
	xvt_log_output_seq(td, "send device attributes");
	xvt_term_write_report(td, "\033[?1;2c");
}

/*------------------------------------------------------------------------
 * TOP_CLL			cursor to lower-left of screen
 */
void xvt_vt100_TOP_CLL (TERMDATA *td)
{
	xvt_log_output_seq(td, "cursor to lower-left of screen");
	CUR_SCRN_X(td) = 0;
	CUR_SCRN_Y(td) = CUR_SCRN_ROWS(td) - 1;
}

/*------------------------------------------------------------------------
 * TOP_HTS			tab set
 */
void xvt_vt100_TOP_HTS (TERMDATA *td)
{
	xvt_log_output_seq(td, "set tab at cur col");
	xvt_scrn_tab_set(CUR_SCRN(td), SCRN_TAB_SET_CUR);
}

/*------------------------------------------------------------------------
 * TOP_SS2			single shift select of G2 char set
 */
void xvt_vt100_TOP_SS2 (TERMDATA *td)
{
	xvt_log_output_seq(td, "single shift select of G2 char set");
	CUR_SCRN_TMPCS(td) = CHAR_SET_G2;
}

/*------------------------------------------------------------------------
 * TOP_SS3			single shift select of G3 char set
 */
void xvt_vt100_TOP_SS3 (TERMDATA *td)
{
	xvt_log_output_seq(td, "single shift select of G3 char set");
	CUR_SCRN_TMPCS(td) = CHAR_SET_G3;
}

/*------------------------------------------------------------------------
 * TOP_IND			index down
 */
void xvt_vt100_TOP_IND (TERMDATA *td)
{
	xvt_log_output_seq(td, "index down");

	if (CUR_SCRN_Y(td) >= CUR_SCRN_BOT(td))
	{
		xvt_curscrn_scroll(td, -1);
	}
	else
	{
		CUR_SCRN_Y(td)++;
	}
}

/*------------------------------------------------------------------------
 * TOP_RI			index up (reverse index)
 */
void xvt_vt100_TOP_RI (TERMDATA *td)
{
	int top;

	xvt_log_output_seq(td, "index up");

	top = CUR_SCRN_TOP(td);
	if (top < CUR_SCRN_LOCK(td))
		top = CUR_SCRN_LOCK(td);

	if (CUR_SCRN_Y(td) <= top)
	{
		xvt_curscrn_scroll(td, 1);
	}
	else
	{
		CUR_SCRN_Y(td)--;
	}
}

/*------------------------------------------------------------------------
 * TOP_NEL			next line
 */
void xvt_vt100_TOP_NEL (TERMDATA *td)
{
	xvt_log_output_seq(td, "next line");
	xvt_vt100_CTL_LF(td);
	CUR_SCRN_X(td) = 0;
}

/*------------------------------------------------------------------------
 * TOP_MEML			memory lock
 */
void xvt_vt100_TOP_MEML (TERMDATA *td)
{
	xvt_log_output_seq(td, "memory lock");
	CUR_SCRN_LOCK(td) = CUR_SCRN_Y(td);
}

/*------------------------------------------------------------------------
 * TOP_MEMU			memory unlock
 */
void xvt_vt100_TOP_MEMU (TERMDATA *td)
{
	xvt_log_output_seq(td, "memory unlock");
	CUR_SCRN_LOCK(td) = -1;
}

/*------------------------------------------------------------------------
 * TOP_SPA			start protected area
 */
void xvt_vt100_TOP_SPA (TERMDATA *td)
{
	xvt_log_output_seq(td, "start protected area");
	CUR_SCRN_AT(td) |= A_ISOPROT;
}

/*------------------------------------------------------------------------
 * TOP_EPA			end protected area
 */
void xvt_vt100_TOP_EPA (TERMDATA *td)
{
	xvt_log_output_seq(td, "end protected area");
	CUR_SCRN_AT(td) &= ~A_ISOPROT;
}

/*------------------------------------------------------------------------
 * TOP_DECBI		back index
 */
void xvt_vt100_TOP_DECBI (TERMDATA *td)
{
	xvt_log_output_seq(td, "back index");

	if (CUR_SCRN_X(td) == 0)
	{
		xvt_curscrn_chars_ins(td, 1);
	}
	else
	{
		CUR_SCRN_X(td)--;
	}
}

/*------------------------------------------------------------------------
 * TOP_DECFI		forward index
 */
void xvt_vt100_TOP_DECFI (TERMDATA *td)
{
	xvt_log_output_seq(td, "forward index");

	if (CUR_SCRN_X(td) >= CUR_SCRN_COLS(td) - 1)
	{
		int x = CUR_SCRN_X(td);

		CUR_SCRN_X(td) = 0;
		xvt_curscrn_chars_del(td, 1);
		CUR_SCRN_X(td) = x;
	}
	else
	{
		CUR_SCRN_X(td)++;
	}
}

/*========================================================================
 * DEC mode commands
 *========================================================================
 */

/*------------------------------------------------------------------------
 * DECALN	DEC alignment test (fill screen with E's)
 */
void xvt_vt100_DEC_DECALN (TERMDATA *td)
{
	xvt_log_output_seq(td, "DECALN");
	xvt_curscrn_set(td, 'E');
}

/*========================================================================
 * GCS mode commands
 *========================================================================
 */

/*------------------------------------------------------------------------
 * GCS_CODE		designate a char set
 */
void xvt_vt100_GCS_CODE (TERMDATA *td)
{
	int c	= te->code;
	int	s	= xvt_gcs_get_gcs_code(c);

	if (s < 0)
	{
		xvt_log_output_seq(td, "Set G%d to %c: unknown: ignored",
			te->vt_type, c);
	}
	else
	{
		CUR_SCRN_GCS(td, te->vt_type) = s;
		xvt_log_output_seq(td, "Set G%d to %c: %s",
			te->vt_type, c, xvt_gcs_get_name(s));
	}
}

/*========================================================================
 * CSI mode commands
 *========================================================================
 */

/*------------------------------------------------------------------------
 * HPA	move the cursor to col x
 */
void xvt_vt100_CSI_HPA (TERMDATA *td)
{
	int x = te->n[0];

	if (--x < 0)	x = 0;

	xvt_log_output_seq(td, "move-cursor to col %d", x);

	if (x >= CUR_SCRN_COLS(td))	x = CUR_SCRN_COLS(td)-1;
	CUR_SCRN_X(td) = x;
}

/*------------------------------------------------------------------------
 * VPA	move the cursor to row y
 */
void xvt_vt100_CSI_VPA (TERMDATA *td)
{
	int y = te->n[0];

	if (--y < 0)	y = 0;

	xvt_log_output_seq(td, "move-cursor to row %d", y);

	if (y >= CUR_SCRN_ROWS(td))	y = CUR_SCRN_ROWS(td)-1;
	CUR_SCRN_Y(td) = y;
}

/*------------------------------------------------------------------------
 * CUP	move the cursor to (x,y)
 */
void xvt_vt100_CSI_CUP (TERMDATA *td)
{
	int y = te->n[0];
	int x = te->n[1];

	if (--y < 0)	y = 0;
	if (--x < 0)	x = 0;

	xvt_log_output_seq(td, "move-cursor to (%d,%d)", x, y);

	if (CUR_SCRN_ORG(td))
	{
		/*----------------------------------------------------------------
		 * "origin" mode means that the cursor is limited to moving
		 * within the scrolling region and line numbers are relative to
		 * the scrolling region.
		 */
		int nlines = (CUR_SCRN_BOT(td) - CUR_SCRN_TOP(td) + 1);

		if (y >= nlines)
			y = (nlines - 1);

		CUR_SCRN_Y(td) = CUR_SCRN_TOP(td) + y;
	}
	else
	{
		if (y >= CUR_SCRN_ROWS(td))
			y = CUR_SCRN_ROWS(td)-1;
		CUR_SCRN_Y(td) = y;
	}

	if (x >= CUR_SCRN_COLS(td))
		x = CUR_SCRN_COLS(td)-1;
	CUR_SCRN_X(td) = x;
}

/*------------------------------------------------------------------------
 * ECH	erase n chars (with current attrs)
 */
void xvt_vt100_CSI_ECH (TERMDATA *td)
{
	int n = te->n[0];

	if (n <= 0)
		n = 1;

	xvt_log_output_seq(td, "erase %d chars", n);
	xvt_curscrn_chars_era(td, n, A_ISOPROT);
}

/*------------------------------------------------------------------------
 * ED	erase display (with current attrs)
 *
 * The current cursor position is not changed.
 */
void xvt_vt100_CSI_ED (TERMDATA *td)
{
	int mode = te->n[0];

	switch (mode)
	{
	case 0:
		/*----------------------------------------------------------------
		 * clear from cursor to EOS
		 */
		xvt_log_output_seq(td, "clear-to-eos");

		xvt_curscrn_chars_clr(td, 1, A_ISOPROT);
		if (CUR_SCRN_Y(td) < CUR_SCRN_ROWS(td) - 1)
		{
			xvt_curscrn_lines_clr(td, CUR_SCRN_Y(td) + 1,
				(CUR_SCRN_ROWS(td) - (CUR_SCRN_Y(td) + 1)), A_ISOPROT);
		}
		break;

	case 1:
		/*----------------------------------------------------------------
		 * clear from BOS to cursor
		 */
		xvt_log_output_seq(td, "clear-from-bos");

		xvt_curscrn_chars_clr(td, -1, A_ISOPROT);
		if (CUR_SCRN_Y(td) > 0)
		{
			xvt_curscrn_lines_clr(td, 0, CUR_SCRN_Y(td), A_ISOPROT);
		}
		break;

	case 2:
		/*----------------------------------------------------------------
		 * clear entire screen
		 */
		xvt_log_output_seq(td, "clear-screen");

		xvt_curscrn_clear(td, A_ISOPROT);
		break;

	case 3:
		/*----------------------------------------------------------------
		 * erase saved lines
		 */
		xvt_log_output_seq(td, "erase saved lines: ignored");
		break;

	default:
		/*----------------------------------------------------------------
		 * invalid seq
		 */
		xvt_log_output_seq(td, "invalid ED mode %d", mode);
		break;
	}
}

/*------------------------------------------------------------------------
 * EL	erase line (with current attrs)
 *
 * The current cursor position is not changed.
 */
void xvt_vt100_CSI_EL (TERMDATA *td)
{
	int mode = te->n[0];

	switch (mode)
	{
	case 0:
		/*----------------------------------------------------------------
		 * clear to EOL
		 */
		xvt_log_output_seq(td, "clear-to-eol");
		xvt_curscrn_chars_clr(td, 1, A_ISOPROT);
		break;

	case 1:
		/*----------------------------------------------------------------
		 * clear from BOL
		 */
		xvt_log_output_seq(td, "clear-from-bol");
		xvt_curscrn_chars_clr(td, -1, A_ISOPROT);
		break;

	case 2:
		/*----------------------------------------------------------------
		 * clear entire line
		 */
		xvt_log_output_seq(td, "clear-entire-line");
		xvt_curscrn_chars_clr(td, 0, A_ISOPROT);
		break;

	default:
		/*----------------------------------------------------------------
		 * invalid mode
		 */
		xvt_log_output_seq(td, "invalid EL mode %d", mode);
		break;
	}
}

/*------------------------------------------------------------------------
 * CUU	move the cursor up
 */
void xvt_vt100_CSI_CUU (TERMDATA *td)
{
	int n = te->n[0];

	if (n == 0)
		n = 1;

	if (te->char_code == ' ')
	{
		xvt_vt100_CSI_SR(td);
		return;
	}

	xvt_log_output_seq(td, "up %d line%s", n, PLURAL(n));

	CUR_SCRN_Y(td) -= n;
	if (CUR_SCRN_Y(td) < 0)
		CUR_SCRN_Y(td) = 0;

	if (CUR_SCRN_ORG(td) && CUR_SCRN_Y(td) < CUR_SCRN_TOP(td))
		CUR_SCRN_Y(td) = CUR_SCRN_TOP(td);
}

/*------------------------------------------------------------------------
 * CPL	move the cursor up & go to col 1
 */
void xvt_vt100_CSI_CPL (TERMDATA *td)
{
	int n = te->n[0];

	if (n == 0)
		n = 1;

	xvt_log_output_seq(td, "up %d line%s & go to col 1", n, PLURAL(n));

	CUR_SCRN_Y(td) -= n;
	if (CUR_SCRN_Y(td) < 0)
		CUR_SCRN_Y(td) = 0;

	if (CUR_SCRN_ORG(td) && CUR_SCRN_Y(td) < CUR_SCRN_TOP(td))
		CUR_SCRN_Y(td) = CUR_SCRN_TOP(td);

	CUR_SCRN_X(td) = 0;
}

/*------------------------------------------------------------------------
 * CUD	move the cursor down
 */
void xvt_vt100_CSI_CUD (TERMDATA *td)
{
	int n = te->n[0];

	if (n == 0)
		n = 1;

	xvt_log_output_seq(td, "down %d line%s", n, PLURAL(n));

	CUR_SCRN_Y(td) += n;
	if (CUR_SCRN_Y(td) >= CUR_SCRN_ROWS(td))
		CUR_SCRN_Y(td) = CUR_SCRN_ROWS(td) - 1;

	if (CUR_SCRN_ORG(td) && CUR_SCRN_Y(td) > CUR_SCRN_BOT(td))
		CUR_SCRN_Y(td) = CUR_SCRN_BOT(td);
}

/*------------------------------------------------------------------------
 * CNL	move the cursor down & go to col 1
 */
void xvt_vt100_CSI_CNL (TERMDATA *td)
{
	int n = te->n[0];

	if (n == 0)
		n = 1;

	xvt_log_output_seq(td, "down %d line%s & go to col 1", n, PLURAL(n));

	CUR_SCRN_Y(td) += n;
	if (CUR_SCRN_Y(td) >= CUR_SCRN_ROWS(td))
		CUR_SCRN_Y(td) = CUR_SCRN_ROWS(td) - 1;

	if (CUR_SCRN_ORG(td) && CUR_SCRN_Y(td) > CUR_SCRN_BOT(td))
		CUR_SCRN_Y(td) = CUR_SCRN_BOT(td);

	CUR_SCRN_X(td) = 0;
}

/*------------------------------------------------------------------------
 * CUF	move the cursor right
 */
void xvt_vt100_CSI_CUF (TERMDATA *td)
{
	int n = te->n[0];

	if (n == 0)
		n = 1;

	xvt_log_output_seq(td, "right %d char%s", n, PLURAL(n));

	CUR_SCRN_X(td) += n;
	if (CUR_SCRN_X(td) >= CUR_SCRN_COLS(td))
		CUR_SCRN_X(td) = CUR_SCRN_COLS(td) - 1;
}

/*------------------------------------------------------------------------
 * CUB	move the cursor left
 */
void xvt_vt100_CSI_CUB (TERMDATA *td)
{
	int n = te->n[0];

	if (n == 0)
		n = 1;

	xvt_log_output_seq(td, "left %d char%s", n, PLURAL(n));

	CUR_SCRN_X(td) -= n;
	if (CUR_SCRN_X(td) < 0)
		CUR_SCRN_X(td) = 0;
}

/*------------------------------------------------------------------------
 * ICH	insert n characters at the current cursor position
 *
 * This moves the chars on the current line over one & inserts a
 * blank char at the current position (with current attributes).
 *
 * The current cursor position is not changed.
 */
void xvt_vt100_CSI_ICH (TERMDATA *td)
{
	int n = te->n[0];

	if (n < 1)
		n = 1;

	if (te->char_code == ' ')
	{
		xvt_vt100_CSI_SL(td);
		return;
	}

	xvt_log_output_seq(td, "insert %d char%s", n, PLURAL(n));
	xvt_curscrn_chars_ins(td, n);
}

/*------------------------------------------------------------------------
 * DCH	delete n chars from where the cursor is
 *
 * This moves the rest of the line to the left one char & inserts a
 * blank (with current attrs) at the end of the line.
 *
 * The current cursor position is not changed.
 */
void xvt_vt100_CSI_DCH (TERMDATA *td)
{
	int n = te->n[0];

	if (n == 0)
		n = 1;

	xvt_log_output_seq(td, "delete %d char%s", n, PLURAL(n));
	xvt_curscrn_chars_del(td, n);
}

/*------------------------------------------------------------------------
 * AL	add n lines at the current cursor position (with current attrs)
 *
 * This results in all rows above the current line to be "scrolled" up.
 * The cursor is moved to the start of the current line.
 */
void xvt_vt100_CSI_AL (TERMDATA *td)
{
	int n = te->n[0];

	if (n == 0)
		n = 1;

	xvt_log_output_seq(td, "add %d line%s", n, PLURAL(n));
	xvt_curscrn_lines_ins(td, n);
}

/*------------------------------------------------------------------------
 * DL	delete n lines where the cursor is on
 *
 * This will add a blank line at the bottom of the screen
 * (with the current attrs).
 */
void xvt_vt100_CSI_DL (TERMDATA *td)
{
	int n = te->n[0];

	if (n == 0)
		n = 1;

	xvt_log_output_seq(td, "delete %d line%s", n, PLURAL(n));
	xvt_curscrn_lines_del(td, n);
}

/*------------------------------------------------------------------------
 * SM 	set mode
 */
void xvt_vt100_CSI_SM (TERMDATA *td)
{
	int i;

	for (i=0; i<=te->n_index; i++)
	{
		int n = te->n[i];

		switch (n)
		{
		case 2:			/* keyboard action on */
			xvt_log_output_seq(td, "lock keyboard");
			OPTION_SET(td, SM_KbdLocked, TRUE);
			break;

		case 4:			/* insert mode */
			xvt_log_output_seq(td, "insert mode on");
			CUR_SCRN_INS(td) = TRUE;
			break;

		case 12:		/* send/receive off */
			xvt_log_output_seq(td, "Send/Receive off");
			OPTION_SET(td, SM_LocalEcho, FALSE);
			break;

		case 20:		/* auto-nl */
			xvt_log_output_seq(td, "auto-nl mode on");
			OPTION_SET(td, SM_AutoNL, TRUE);
			break;

		default:
			xvt_log_output_seq(td, "invalid SM opt %d", n);
			break;
		}
	}
}

/*------------------------------------------------------------------------
 * RM	reset mode
 */
void xvt_vt100_CSI_RM (TERMDATA *td)
{
	int i;

	for (i=0; i<=te->n_index; i++)
	{
		int n = te->n[i];

		switch (n)
		{
		case 2:			/* keyboard action off */
			xvt_log_output_seq(td, "unlock keyboard");
			OPTION_SET(td, SM_KbdLocked, FALSE);
			break;

		case 4:			/* insert mode */
			xvt_log_output_seq(td, "insert mode off");
			CUR_SCRN_INS(td) = FALSE;
			break;

		case 12:		/* send/receive on */
			xvt_log_output_seq(td, "Send/Receive on");
			OPTION_SET(td, SM_LocalEcho, TRUE);
			break;

		case 20:		/* auto-nl */
			xvt_log_output_seq(td, "auto-nl mode off");
			OPTION_SET(td, SM_AutoNL, FALSE);
			break;

		default:
			xvt_log_output_seq(td, "invalid RM opt %d", n);
			break;
		}
	}
}

/*------------------------------------------------------------------------
 * SGR_AT	set attribute
 */
static void xvt_vt100_SGR_AT (TERMDATA *td, int set, int n)
{
	/*--------------------------------------------------------------------
	 * REVERSE forces a switch to mono mode
	 */
	switch (n)
	{
	case A_REVERSE:
	case A_NORMAL:
		if (CUR_SCRN_MODE(td) == SCRN_MODE_COLOR)
		{
			xvt_log_output_seq(td, "switch to mono mode");
			xvt_term_set_scrn_mode(td, SCRN_MODE_MONO);
		}
		break;
	}

	/*--------------------------------------------------------------------
	 * change attribute
	 */
	if (n == 0)
	{
		CUR_SCRN_AT(td) = A_NORMAL;

		xvt_log_output_seq(td, "all attributes off");
	}
	else
	{
		if (set)
			CUR_SCRN_AT(td) |=  n;
		else
			CUR_SCRN_AT(td) &= ~n;

		{
			const char *at = "?";

			switch (n)
			{
			case A_BOLD:		at = "bold";		break;
			case A_UNDERLINE:	at = "underline";	break;
			case A_BLINK:		at = "blink";		break;
			case A_REVERSE:		at = "reverse";		break;
			case A_HIDDEN:		at = "hidden";		break;
			}

			xvt_log_output_seq(td, "attr %s %s", at,
				set ? "on" : "off");
		}
	}
}

/*------------------------------------------------------------------------
 * SGR_FG	set foreground color
 */
static void xvt_vt100_SGR_FG (TERMDATA *td, int fg)
{
	/*--------------------------------------------------------------------
	 * now switch to color mode if currently in mono
	 */
	if (CUR_SCRN_MODE(td) == SCRN_MODE_MONO)
	{
		xvt_log_output_seq(td, "switch to color mode");
		xvt_term_set_scrn_mode(td, SCRN_MODE_COLOR);
	}

	/*--------------------------------------------------------------------
	 * now if actually in color, set it
	 */
	if (CUR_SCRN_MODE(td) == SCRN_MODE_COLOR)
	{
		CUR_SCRN_FG(td) = color_code(fg);
	}

	/*--------------------------------------------------------------------
	 * debug output
	 */
	xvt_log_output_seq(td, "fg:%s", color_name(fg));
}

/*------------------------------------------------------------------------
 * SGR_BG	set background color
 */
static void xvt_vt100_SGR_BG (TERMDATA *td, int bg)
{
	/*--------------------------------------------------------------------
	 * now switch to color mode if currently in mono
	 */
	if (CUR_SCRN_MODE(td) == SCRN_MODE_MONO)
	{
		xvt_log_output_seq(td, "switch to color mode");
		xvt_term_set_scrn_mode(td, SCRN_MODE_COLOR);
	}

	/*--------------------------------------------------------------------
	 * now if actually in color, set it
	 */
	if (CUR_SCRN_MODE(td) == SCRN_MODE_COLOR)
	{
		CUR_SCRN_BG(td) = color_code(bg);
	}

	/*--------------------------------------------------------------------
	 * debug output
	 */
	xvt_log_output_seq(td, "bg:%s", color_name(bg));
}

/*------------------------------------------------------------------------
 * SGR		set graphic rendition (color/attributes)
 */
void xvt_vt100_CSI_SGR (TERMDATA *td)
{
	int i;

	for (i=0; i<=te->n_index; i++)
	{
		int n = te->n[i];

		switch (n)
		{
		/*----------------------------------------------------------------
		 * all mono attributes off
		 */
		case 0:
			xvt_vt100_SGR_AT(td, TRUE, 0);
			break;

		/*----------------------------------------------------------------
		 * mono attributes on
		 */
		case 1:		xvt_vt100_SGR_AT(td, TRUE,  A_BOLD);		break;
		case 4:		xvt_vt100_SGR_AT(td, TRUE,  A_UNDERLINE);	break;
		case 5:		xvt_vt100_SGR_AT(td, TRUE,  A_BLINK);		break;
		case 7:		xvt_vt100_SGR_AT(td, TRUE,  A_REVERSE);		break;
		case 8:		xvt_vt100_SGR_AT(td, TRUE,  A_HIDDEN);		break;

		/*----------------------------------------------------------------
		 * mono attributes off
		 */
		case 22:	xvt_vt100_SGR_AT(td, FALSE, A_BOLD);		break;
		case 24:	xvt_vt100_SGR_AT(td, FALSE, A_UNDERLINE);	break;
		case 25:	xvt_vt100_SGR_AT(td, FALSE, A_BLINK);		break;
		case 27:	xvt_vt100_SGR_AT(td, FALSE, A_REVERSE);		break;
		case 28:	xvt_vt100_SGR_AT(td, FALSE, A_HIDDEN);		break;

		/*----------------------------------------------------------------
		 * fg colors 0-7
		 */
		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:	xvt_vt100_SGR_FG(td, n - 30);			break;

		/*----------------------------------------------------------------
		 * default fg color
		 */
		case 39:	xvt_vt100_SGR_FG(td, XVT_CLR_FG);		break;

		/*----------------------------------------------------------------
		 * bg colors 0-7
		 */
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:	xvt_vt100_SGR_BG(td, n - 40);			break;

		/*----------------------------------------------------------------
		 * default bg color
		 */
		case 49:	xvt_vt100_SGR_BG(td, XVT_CLR_BG);		break;

		/*----------------------------------------------------------------
		 * fg colors 8-15 (aixterm)
		 */
		case 90:
		case 91:
		case 92:
		case 93:
		case 94:
		case 95:
		case 96:
		case 97:	xvt_vt100_SGR_FG(td, (n -  90) | XVT_CLR_BRIGHT);	break;

		/*----------------------------------------------------------------
		 * bg colors 8-15 (aixterm)
		 */
		case 100:
		case 101:
		case 102:
		case 103:
		case 104:
		case 105:
		case 106:
		case 107:	xvt_vt100_SGR_BG(td, (n - 100) | XVT_CLR_BRIGHT);	break;

		/*----------------------------------------------------------------
		 * unknown
		 */
		default:
			xvt_log_output_seq(td, "invalid SGR: %d", n);
			break;
		}
	}
}

/*------------------------------------------------------------------------
 * DECSTBM		set top & bottom margins
 */
void xvt_vt100_CSI_DECSTBM (TERMDATA *td)
{
	int p1 = te->n[0];
	int p2 = te->n[1];

	if (--p1 < 0)	p1 = 0;
	if (--p2 < 0)	p2 = 0;

	xvt_log_output_seq(td, "set-vert-scroll-region: %d,%d", p1, p2);

	xvt_curscrn_set_vmargins(td, p1, p2);

	/*--------------------------------------------------------------------
	 * move to top of scrolling region
	 */
	CUR_SCRN_X(td) = CUR_SCRN_LEFT(td);
	CUR_SCRN_Y(td) = CUR_SCRN_TOP(td);
}

/*------------------------------------------------------------------------
 * DECSLRM		set left & right margins
 */
void xvt_vt100_CSI_DECSLRM (TERMDATA *td)
{
	int p1 = te->n[0];
	int p2 = te->n[1];

	if (--p1 < 0)	p1 = 0;
	if (--p2 < 0)	p2 = 0;

	xvt_log_output_seq(td, "set-horz-scroll-region: %d,%d", p1, p2);

	xvt_curscrn_set_hmargins(td, p1, p2);

	/*--------------------------------------------------------------------
	 * move to top of scrolling region
	 */
	CUR_SCRN_X(td) = CUR_SCRN_LEFT(td);
	CUR_SCRN_Y(td) = CUR_SCRN_TOP(td);
}

/*------------------------------------------------------------------------
 * TERM			process terminal cmds
 */
void xvt_vt100_CSI_TERM (TERMDATA *td)
{
	int n = te->n[0];

	switch (n)
	{
	case 1:					/* un-iconify the window		*/
		xvt_log_output_seq(td, "iconify-off");
		xvt_term_set_icon_state(td, FALSE);
		break;

	case 2:					/* iconify the window			*/
		xvt_log_output_seq(td, "iconify-on");
		xvt_term_set_icon_state(td, TRUE);
		break;

	case 3:					/* set window pos to n[1], n[2]	*/
		if (te->n_index == 2)
		{
			xvt_log_output_seq(td, "set window position to (%d x %d)",
				te->n[1], te->n[2]);
			xvt_term_set_position(td, te->n[1], te->n[2]);
		}
		break;

	case 4:					/* set w/h to n[2], n[1] pixels	*/
		if (te->n_index == 2)
		{
			xvt_log_output_seq(td, "set window size to (%d x %d) pixels",
				te->n[2], te->n[1]);
			xvt_term_set_size(td, te->n[2], te->n[1]);
		}
		break;

	case 5:					/* raise window					*/
		xvt_log_output_seq(td, "raise window");
		xvt_term_raise(td);
		break;

	case 6:					/* lower window					*/
		xvt_log_output_seq(td, "lower window");
		xvt_term_lower(td);
		break;

	case 7:					/* refresh window				*/
		xvt_log_output_seq(td, "refresh window");
		xvt_term_repaint(td);
		break;

	case 8:					/* set w/h to n[2], n[1] chars	*/
		if (te->n_index == 2)
		{
			xvt_log_output_seq(td, "set window size to (%d x %d) chars",
				te->n[2], te->n[1]);
			xvt_term_resize(td, te->n[2], te->n[1]);
		}
		break;

	case 9:					/* maximize/restore window */
		n = te->n[1];
		switch (n)
		{
		case 0:				/* restore maximized window */
			xvt_log_output_seq(td, "restore maximized window");
			xvt_term_restore(td);
			break;

		case 1:				/* maximize window */
			xvt_log_output_seq(td, "maximize window");
			xvt_term_maximize(td);
			break;

		default:
			xvt_log_output_seq(td, "invalid window size cmd %d", n);
			break;
		}
		break;

	case XVT_CSI_TERM_CUR:	/* XVT: set pointer type to n[1]	*/
		n = te->n[1];
		xvt_log_output_seq(td, "set pointer type %d", n);
		xvt_term_set_pointer(td, n);
		break;

	case 11:				/* report win state				*/
		xvt_log_output_seq(td, "report window state");
		xvt_term_write_report(td, "\033[%dt",
			td->tw->iconified ? 2 : 1);
		break;

	case 13:				/* report win position			*/
		{
			int x, y;

			xvt_log_output_seq(td, "report window position");
			xvt_term_get_position(td, &x, &y);
			xvt_term_write_report(td, "\033[3;%d;%dt", x, y);
		}
		break;

	case 14:				/* report win size (pixels)		*/
		{
			int w, h;

			xvt_log_output_seq(td, "report window size (pixels)");
			xvt_term_get_size(td, &w, &h);
			xvt_term_write_report(td, "\033[4;%d;%dt", w, h);
		}
		break;

	case 18:				/* report win size (chars)		*/
		xvt_log_output_seq(td, "report window size (chars)");
		xvt_term_write_report(td, "\033[8;%d;%dt",
			td->xd->cur_rows, td->xd->cur_cols);
		break;

	case 20:				/* report ico label				*/
		xvt_log_output_seq(td, "report icon label");
		xvt_term_write_report(td, "\033]L%s\033\\",
			td->ico_label);
		break;

	case 21:				/* report win label				*/
		xvt_log_output_seq(td, "report window label");
		xvt_term_write_report(td, "\033]l%s\033\\",
			td->org_label);
		break;

	default:
		if (n >= 24)
		{
							/* set win height to n rows		*/
		
			xvt_log_output_seq(td, "set window height to %d", n);
			xvt_term_resize(td, -1, n);
		}
		break;
	}
}

/*------------------------------------------------------------------------
 * MC			process media copy cmds
 */
void xvt_vt100_CSI_MC (TERMDATA *td)
{
	int n = te->n[0];

	switch (n)
	{
	case 0:
		xvt_log_output_seq(td, "print-screen");
		xvt_term_screen_print(td, FALSE, OPTION_GET(td, SM_PrintScreenRegion));
		break;

	case 4:
		/*----------------------------------------------------------------
		 * we should never get here ...
		 */
		xvt_log_output_seq(td, "stop print pass-thru");
		if (! OPTION_GET(td, SM_PrintAuto))
			xvt_term_printer_close(td);
		td->prt_mode = PRT_MODE_NONE;
		break;

	case 5:
		xvt_log_output_seq(td, "start print pass-thru");
		xvt_term_printer_open(td);
		td->prt_mode = PRT_MODE_NORM;
		break;

	default:
		xvt_log_output_seq(td, "invalid MC cmd %d", n);
		break;
	}
}

/*------------------------------------------------------------------------
 * DA			device attributes report
 */
void xvt_vt100_CSI_DA (TERMDATA *td)
{
	xvt_log_output_seq(td, "send device attributes");
	if (te->char_code == '>')
	{
		xvt_term_write_report(td, "\033[>0;%s;0c", XVT_VERSION);
	}
	else if (te->char_code == '=')
	{
		xvt_term_write_report(td, "\033P!|00\033\\");
	}
	else
	{
		xvt_term_write_report(td, "\033[?1;2c");
	}
}

/*------------------------------------------------------------------------
 * DECREQT		request terminal parameters
 */
void xvt_vt100_CSI_DECREQT (TERMDATA *td)
{
	int n = te->n[0];

	xvt_log_output_seq(td, "request terminal parameters");
	xvt_term_write_report(td, "\033[%d;%d;%d;%d;%d;%d;%dx",
		n + 2,					/* ? */
		1,						/* no parity */
		1,						/* eight bits */
		128,					/* xmit 38.4k baud */
		128,					/* recv 38.4k baud */
		1,						/* clock multiplier ? */
		0						/* STP flags ? */
		);
}

/*------------------------------------------------------------------------
 * DECTST		invoke confidence report
 */
void xvt_vt100_CSI_DECTST (TERMDATA *td)
{
	int n = te->n[0];

	if (n != 2)
	{
		xvt_log_output_seq(td, "invalid DECTST cmd %d", n);
		return;
	}

	n = te->n[1];
	xvt_log_output_seq(td, "invoke confidence report %d", n);
	if (n == 0)
		xvt_vt100_TOP_RIS(td);
}

/*------------------------------------------------------------------------
 * DSR			device status report
 */
void xvt_vt100_CSI_DSR (TERMDATA *td)
{
	int n = te->n[0];

	switch (n)
	{
	case 5:						/* status report */
		xvt_log_output_seq(td, "status report");
		xvt_term_write_report(td, "\033[0n");
		break;

	case 6:						/* report cursor position */
		xvt_log_output_seq(td, "report cursor-position");
		xvt_term_write_report(td, "\033[%d;%dR",
			CUR_SCRN_Y(td) + 1,
			(CUR_SCRN_X(td) >= CUR_SCRN_COLS(td) ?
				CUR_SCRN_COLS(td) - 1 : CUR_SCRN_X(td)) + 1);
		break;

	case 7:						/* report display name */
		xvt_log_output_seq(td, "report display name");
		xvt_term_write_report(td, "%s\n", td->xd->display);
		break;

	case 8:						/* report version */
		xvt_log_output_seq(td, "report version");
		xvt_term_write_report(td, "%s\n", XVT_APPCLASS);
		break;

	default:
		xvt_log_output_seq(td, "invalid DSR cmd %d", n);
		break;
	}
}

/*------------------------------------------------------------------------
 * TBC			tab clear
 */
void xvt_vt100_CSI_TBC (TERMDATA *td)
{
	int n = te->n[0];

	switch (n)
	{
	case 0:			/* clear tab at cur col */
		xvt_log_output_seq(td, "clear tab at cur col");
		xvt_scrn_tab_set(CUR_SCRN(td), SCRN_TAB_CLR_CUR);
		break;

	case 3:			/* clear all tabs */
		xvt_log_output_seq(td, "clear all tabs");
		xvt_scrn_tab_set(CUR_SCRN(td), SCRN_TAB_CLR_ALL);
		break;

	default:
		xvt_log_output_seq(td, "invalid tab clear mode %d", n);
		break;
	}
}

/*------------------------------------------------------------------------
 * TBS			tab set
 */
void xvt_vt100_CSI_TBS (TERMDATA *td)
{
	int n = te->n[0];

	switch (n)
	{
	case 0:			/* set tab at cur col */
		xvt_log_output_seq(td, "set tab at cur col");
		xvt_scrn_tab_set(CUR_SCRN(td), SCRN_TAB_SET_CUR);
		break;

	case 2:			/* clear tab at cur col */
		xvt_log_output_seq(td, "clear tab at cur col");
		xvt_scrn_tab_set(CUR_SCRN(td), SCRN_TAB_CLR_CUR);
		break;

	case 5:			/* clear all tabs */
		xvt_log_output_seq(td, "clear all tabs");
		xvt_scrn_tab_set(CUR_SCRN(td), SCRN_TAB_CLR_ALL);
		break;

	default:
		xvt_log_output_seq(td, "invalid tab set mode %d", n);
		break;
	}
}

/*------------------------------------------------------------------------
 * CHT			tab move right
 */
void xvt_vt100_CSI_CHT (TERMDATA *td)
{
	int n = te->n[0];
	int x;

	if (n <= 0)
		n = 1;

	xvt_log_output_seq(td, "move %d tabstop%s right", n, PLURAL(n));

	for (; n>0; n--)
		xvt_vt100_CTL_HT(td);
}

/*------------------------------------------------------------------------
 * CBT			tab move left
 */
void xvt_vt100_CSI_CBT (TERMDATA *td)
{
	int n = te->n[0];
	int x;

	if (n <= 0)
		n = 1;

	xvt_log_output_seq(td, "move %d tabstop%s left", n, PLURAL(n));

	for (x=CUR_SCRN_X(td) - 1; n>0 && x >= 0; x--)
	{
		if (CUR_SCRN_TABSTOP(td, x))
		{
			if (--n == 0)
				break;
		}
	}

	if (x > 0)
		CUR_SCRN_X(td) = x;
	else
		CUR_SCRN_X(td) = 0;
}

/*------------------------------------------------------------------------
 * SU			scroll up n lines
 */
void xvt_vt100_CSI_SU (TERMDATA *td)
{
	int n = te->n[0];

	if (n <= 0)
		n = 1;

	xvt_log_output_seq(td, "scroll up %d line%s", n, PLURAL(n));
	xvt_curscrn_scroll(td, n);
}

/*------------------------------------------------------------------------
 * SD			scroll down n lines
 */
void xvt_vt100_CSI_SD (TERMDATA *td)
{
	int n = te->n[0];

	if (n <= 0)
		n = 1;

	xvt_log_output_seq(td, "scroll down %d line%s", n, PLURAL(n));
	xvt_curscrn_scroll(td, -n);
}

/*------------------------------------------------------------------------
 * SL			scroll left n chars
 */
void xvt_vt100_CSI_SL (TERMDATA *td)
{
	int sx	= CUR_SCRN_X(td);
	int sy	= CUR_SCRN_Y(td);
	int n	= te->n[0];
	int y;
	int top;
	int bot;

	if (n <= 0)
		n = 1;

	xvt_log_output_seq(td, "scroll left %d char%s", n, PLURAL(n));

	top = CUR_SCRN_TOP(td);
	bot = CUR_SCRN_BOT(td);

	if (top < CUR_SCRN_LOCK(td))
		top = CUR_SCRN_LOCK(td);

	for (y=top; y<=bot; y++)
	{
		CUR_SCRN_X(td) = 0;
		CUR_SCRN_Y(td) = y;
		xvt_curscrn_chars_del(td, n);
	}

	CUR_SCRN_X(td) = sx;
	CUR_SCRN_Y(td) = sy;
}

/*------------------------------------------------------------------------
 * SR			scroll right n chars
 */
void xvt_vt100_CSI_SR (TERMDATA *td)
{
	int sx	= CUR_SCRN_X(td);
	int sy	= CUR_SCRN_Y(td);
	int n	= te->n[0];
	int y;
	int top;
	int bot;

	if (n <= 0)
		n = 1;

	xvt_log_output_seq(td, "scroll right %d char%s", n, PLURAL(n));

	top = CUR_SCRN_TOP(td);
	bot = CUR_SCRN_BOT(td);

	if (top < CUR_SCRN_LOCK(td))
		top = CUR_SCRN_LOCK(td);

	for (y=top; y<=bot; y++)
	{
		CUR_SCRN_X(td) = 0;
		CUR_SCRN_Y(td) = y;
		xvt_curscrn_chars_ins(td, n);
	}

	CUR_SCRN_X(td) = sx;
	CUR_SCRN_Y(td) = sy;
}

/*------------------------------------------------------------------------
 * DECSTR		Soft reset
 */
void xvt_vt100_CSI_DECSTR (TERMDATA *td)
{
		xvt_log_output_seq(td, "soft reset");
}

/*------------------------------------------------------------------------
 * DECSCL		Set Compatability Level
 */
void xvt_vt100_CSI_DECSCL (TERMDATA *td)
{
	int	n1	= te->n[0];
	int	n2	= te->n[1];
	int	controls;
	int level;

	if (te->char_code == '!')
	{
		xvt_vt100_CSI_DECSTR(td);
		return;
	}

	switch (n2)
	{
	case 0:
	case 2:	controls = 8;	break;

	case 1:	controls = 7;	break;

	default:
		xvt_log_output_seq(td, "invalid DECSCL cmd (invalid 2nd parameter");
		return;
	}

	switch (n1)
	{
	case 61:	level = 100;	break;
	case 62:	level = 200;	break;
	case 63:	level = 300;	break;

	default:
		xvt_log_output_seq(td, "invalid DECSCL cmd (invalid 1st parameter)");
		return;
	}

	xvt_log_output_seq(td, "set VT%d mode with %d controls", level, controls);
}

/*------------------------------------------------------------------------
 * DECSCA		set char protection attribute
 */
void xvt_vt100_CSI_DECSCA (TERMDATA *td)
{
	int n = te->n[0];

	if (te->char_code != '"')
	{
		xvt_log_output_seq(td, "invalid DECSCA cmd");
		return;
	}

	switch (n)
	{
	case 0:
	case 2:				/* DECSED & DECSEL can erase */
		xvt_log_output_seq(td, "DECSED & DECSEL can erase");
		CUR_SCRN_AT(td) &= ~A_DECPROT;
		break;

	case 1:				/* DECSED & DECSEL cannot erase */
		xvt_log_output_seq(td, "DECSED & DECSEL cannot erase");
		CUR_SCRN_AT(td) |= A_DECPROT;
		break;

	default:
		xvt_log_output_seq(td, "invalid DECSCA cmd %d", n);
		break;
	}
}

/*------------------------------------------------------------------------
 * REP		repeat last char
 */
void xvt_vt100_CSI_REP (TERMDATA *td)
{
	int n = te->n[0];
	int c = td->prev_char;

	xvt_log_output_seq(td, "repeat last char %d time%s", n, PLURAL(n));
	if (c != 0)
	{
		while (n--)
			xvt_vt100_char_output(td, c);
	}
}

/*------------------------------------------------------------------------
 * DECDC	delete columns
 */
void xvt_vt100_CSI_DECDC (TERMDATA *td)
{
	int n = te->n[0];

	if (n == 0)
		n = 1;

	if (te->char_code != '\'')
	{
		xvt_log_output_seq(td, "invalid DECDC cmd");
		return;
	}

	xvt_log_output_seq(td, "delete %d column%s", n, PLURAL(n));
	xvt_curscrn_cols_del(td, n);
}

/*------------------------------------------------------------------------
 * DECIC	insert columns
 */
void xvt_vt100_CSI_DECIC (TERMDATA *td)
{
	int n = te->n[0];

	if (n == 0)
		n = 1;

	if (te->char_code != '\'')
	{
		xvt_log_output_seq(td, "invalid DECIC cmd");
		return;
	}

	xvt_log_output_seq(td, "insert %d column%s", n, PLURAL(n));
	xvt_curscrn_cols_ins(td, n);
}

/*========================================================================
 * PRI mode commands
 *========================================================================
 */

/*------------------------------------------------------------------------
 * DECMC 	media copy
 */
void xvt_vt100_PRI_DECMC (TERMDATA *td)
{
	int n = te->n[0];

	switch (n)
	{
	case 1:						/* print line containing cursor */
		xvt_log_output_seq(td, "print line containing cursor");
		xvt_term_screen_print_line(td);
		break;

	case 4:						/* autoprint mode off */
		xvt_log_output_seq(td, "autoprint mode off");
		if (td->prt_mode == PRT_MODE_NONE)
			xvt_term_printer_close(td);
		OPTION_SET(td, SM_PrintAuto, FALSE);
		break;

	case 5:						/* autoprint mode on */
		xvt_log_output_seq(td, "autoprint mode on");
		xvt_term_printer_open(td);
		OPTION_SET(td, SM_PrintAuto, TRUE);
		break;

	case 10:					/* print composed display */
		xvt_log_output_seq(td, "print composed display");
		xvt_term_screen_print(td, FALSE, FALSE);
		break;

	case 11:					/* print all pages */
		xvt_log_output_seq(td, "print all pages");
		xvt_term_screen_print(td, TRUE, OPTION_GET(td, SM_PrintScreenRegion));
		break;

	default:
		xvt_log_output_seq(td, "invalid MC cmd %d", n);
		break;
	}
}

/*------------------------------------------------------------------------
 * DECSED 	selective erase in display
 */
void xvt_vt100_PRI_DECSED (TERMDATA *td)
{
	int mode = te->n[0];

	switch (mode)
	{
	case 0:
		/*----------------------------------------------------------------
		 * clear from cursor to EOS
		 */
		xvt_log_output_seq(td, "selective clear-to-eos");

		xvt_curscrn_chars_clr(td, 1, A_DECPROT);
		if (CUR_SCRN_Y(td) < CUR_SCRN_ROWS(td) - 1)
		{
			xvt_curscrn_lines_clr(td, CUR_SCRN_Y(td) + 1,
				(CUR_SCRN_ROWS(td) - (CUR_SCRN_Y(td) + 1)), A_DECPROT);
		}
		break;

	case 1:
		/*----------------------------------------------------------------
		 * clear from BOS to cursor
		 */
		xvt_log_output_seq(td, "selective clear-from-bos");

		xvt_curscrn_chars_clr(td, -1, A_DECPROT);
		if (CUR_SCRN_Y(td) > 0)
		{
			xvt_curscrn_lines_clr(td, 0, CUR_SCRN_Y(td), A_DECPROT);
		}
		break;

	case 2:
		/*----------------------------------------------------------------
		 * clear entire screen
		 */
		xvt_log_output_seq(td, "selective clear-screen");

		xvt_curscrn_clear(td, A_DECPROT);
		break;

	default:
		/*----------------------------------------------------------------
		 * invalid seq
		 */
		xvt_log_output_seq(td, "invalid DECSED mode %d", mode);
		break;
	}
}

/*------------------------------------------------------------------------
 * DECSEL 	selective erase in line
 */
void xvt_vt100_PRI_DECSEL (TERMDATA *td)
{
	int mode = te->n[0];

	switch (mode)
	{
	case 0:
		/*----------------------------------------------------------------
		 * clear to EOL
		 */
		xvt_log_output_seq(td, "selective clear-to-eol");
		xvt_curscrn_chars_clr(td, 1, A_DECPROT);
		break;

	case 1:
		/*----------------------------------------------------------------
		 * clear from BOL
		 */
		xvt_log_output_seq(td, "selective clear-from-bol");
		xvt_curscrn_chars_clr(td, -1, A_DECPROT);
		break;

	case 2:
		/*----------------------------------------------------------------
		 * clear entire line
		 */
		xvt_log_output_seq(td, "selective clear-entire-line");
		xvt_curscrn_chars_clr(td, 0, A_DECPROT);
		break;

	default:
		/*----------------------------------------------------------------
		 * invalid mode
		 */
		xvt_log_output_seq(td, "invalid DECSEL mode %d", mode);
		break;
	}
}

/*------------------------------------------------------------------------
 * DECDSR 	device status report
 */
void xvt_vt100_PRI_DECDSR (TERMDATA *td)
{
	int n = te->n[0];

	switch (n)
	{
	case 15:					/* report printer status */
		xvt_log_output_seq(td, "report printer status");
		if (td->prt == 0)
			xvt_term_write_report(td, "\033[?11n");
		else
			xvt_term_write_report(td, "\033[?10n");
		break;

	case 25:					/* report UDK status */
		xvt_log_output_seq(td, "report UDK status");
		xvt_term_write_report(td, "\033[?20n");
		break;

	case 26:					/* report kbd status */
		xvt_log_output_seq(td, "report kbd status");
		xvt_term_write_report(td, "\033[?27;1n");	/* North American */
		break;

	case 53:					/* report locator status */
		xvt_log_output_seq(td, "report locator status");
		xvt_term_write_report(td, "\033[?53n");		/* no locator */
		break;

	default:
		xvt_log_output_seq(td, "invalid DSR cmd %d", n);
		break;
	}
}

/*------------------------------------------------------------------------
 * PROCESS 	process Private cmd
 */
static void xvt_vt100_PRI_PROCESS (TERMDATA *td, int n, Option_Cmd cmd)
{
	int old_value;
	int new_value;

	switch (n)
	{
	case 1:					/* DECCKM - application cursor keys */
		old_value = OPTION_GET(td, SM_AppCursorKeys);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "app-cursor-keys on");
			OPTION_SET(td, SM_AppCursorKeys, TRUE);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "app-cursor-keys off");
			OPTION_SET(td, SM_AppCursorKeys, FALSE);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save app-cursor-keys mode");
			OPTION_SAV(td, SM_AppCursorKeys);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore app-cursor-keys mode");
			OPTION_RES(td, SM_AppCursorKeys);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle app-cursor-keys mode");
			OPTION_TGL(td, SM_AppCursorKeys);
			break;
		}

		new_value = OPTION_GET(td, SM_AppCursorKeys);
		if (new_value != old_value)
		{
			xvt_vt100_DECCKM(td, new_value);
		}
		break;

	case 2:					/* DECANM - set charsets */
		/*----------------------------------------------------------------
		 * This cmd is special in that the set/reset do entirely
		 * different things.
		 */
		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "set char sets G0-G3 as USASCII");
			CUR_SCRN_GCS(td, CHAR_SET_G0) = GCS_US;
			CUR_SCRN_GCS(td, CHAR_SET_G1) = GCS_US;
			CUR_SCRN_GCS(td, CHAR_SET_G2) = GCS_US;
			CUR_SCRN_GCS(td, CHAR_SET_G3) = GCS_US;
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "switch to VT52 mode");
			xvt_emul_set_tbl(td, "vt52");
			break;
		}
		break;

	case 3:					/* DECCOLM - 80/132 column mode */
		old_value = OPTION_GET(td, SM_WideMode);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "set 132 column mode");
			OPTION_SET(td, SM_WideMode, TRUE);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "set 80 column mode");
			OPTION_SET(td, SM_WideMode, FALSE);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save 80/132 column mode");
			OPTION_SAV(td, SM_WideMode);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore 80/132 column mode");
			OPTION_RES(td, SM_WideMode);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle 80/132 column mode");
			OPTION_TGL(td, SM_WideMode);
			break;
		}

		if (OPTION_GET(td, SM_AllowWideMode))
		{
			new_value = OPTION_GET(td, SM_WideMode);
			if (new_value != old_value)
			{
				xvt_term_resize(td, new_value ? 132 : 80, -1);

				/*--------------------------------------------------------
				 * I wouldn't do this, but xterm does.
				 */
				CUR_SCRN_X(td) = 0;
				CUR_SCRN_Y(td) = 0;
			}
		}
		else
		{
			OPTION_SET(td, SM_WideMode, old_value);
		}
		break;

	case 4:					/* DECSCLM - scrolling mode */
		old_value = OPTION_GET(td, SM_SmoothScroll);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "smooth scrolling");
			OPTION_SET(td, SM_SmoothScroll, TRUE);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "jump scrolling");
			OPTION_SET(td, SM_SmoothScroll, FALSE);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save scrolling mode");
			OPTION_SAV(td, SM_SmoothScroll);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore scrolling mode");
			OPTION_RES(td, SM_SmoothScroll);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle scrolling mode");
			OPTION_TGL(td, SM_SmoothScroll);
			break;
		}

		new_value = OPTION_GET(td, SM_SmoothScroll);
		if (new_value != old_value)
		{
		}
		break;

	case 5:					/* DECSCNM - reverse video */
		old_value = OPTION_GET(td, SM_ReverseVideo);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "reverse-video on");
			OPTION_SET(td, SM_ReverseVideo, TRUE);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "reverse-video off");
			OPTION_SET(td, SM_ReverseVideo, FALSE);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save reverse-video mode");
			OPTION_SAV(td, SM_ReverseVideo);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore reverse-video mode");
			OPTION_RES(td, SM_ReverseVideo);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle reverse-video mode");
			OPTION_TGL(td, SM_ReverseVideo);
			break;
		}

		new_value = OPTION_GET(td, SM_ReverseVideo);
		if (new_value != old_value)
		{
			xvt_term_set_rev_video(td, new_value);
		}
		break;

	case 6:				/* DECOM - origin mode */
		old_value = OPTION_GET(td, SM_OriginMode);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "origin mode on");
			OPTION_SET(td, SM_OriginMode, TRUE);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "origin mode off");
			OPTION_SET(td, SM_OriginMode, FALSE);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save origin mode");
			OPTION_SAV(td, SM_OriginMode);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore origin mode");
			OPTION_RES(td, SM_OriginMode);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle origin mode");
			OPTION_TGL(td, SM_OriginMode);
			break;
		}

		new_value = OPTION_GET(td, SM_OriginMode);
		if (new_value != old_value)
		{
			CUR_SCRN_ORG(td) = new_value;

			/*------------------------------------------------------------
			 * Move to top of scrolling region
			 */
			CUR_SCRN_X(td) = 0;
			CUR_SCRN_Y(td) = CUR_SCRN_TOP(td);
		}
		break;

	case 7:				/* DECAWM - auto wrap-around */
		old_value = OPTION_GET(td, SM_WrapAround);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "wrap-around on");
			OPTION_SET(td, SM_WrapAround, TRUE);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "wrap-around off");
			OPTION_SET(td, SM_WrapAround, FALSE);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save wrap-around mode");
			OPTION_SAV(td, SM_WrapAround);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore wrap-around mode");
			OPTION_RES(td, SM_WrapAround);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle wrap-around mode");
			OPTION_TGL(td, SM_WrapAround);
			break;
		}

		new_value = OPTION_GET(td, SM_WrapAround);
		if (new_value != old_value)
		{
		}
		break;

	case 8:				/* DECARM - auto-repeat keys */
		old_value = OPTION_GET(td, SM_AutoRepeat);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "auto-repeat on");
			OPTION_SET(td, SM_AutoRepeat, TRUE);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "auto-repeat off");
			OPTION_SET(td, SM_AutoRepeat, FALSE);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save auto-repeat mode");
			OPTION_SAV(td, SM_AutoRepeat);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore auto-repeat mode");
			OPTION_RES(td, SM_AutoRepeat);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle auto-repeat mode");
			OPTION_TGL(td, SM_AutoRepeat);
			break;
		}

		new_value = OPTION_GET(td, SM_AutoRepeat);
		if (new_value != old_value)
		{
		}
		break;

	case 9:				/* X10 mouse reporting */
		old_value = OPTION_GET(td, SM_MouseMode);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "X10-mouse-reporting on");
			OPTION_SET(td, SM_MouseMode, Mouse_X10);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "X10-mouse-reporting off");
			OPTION_SET(td, SM_MouseMode, Mouse_None);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save X10-mouse-reporting mode");
			OPTION_SAV(td, SM_MouseMode);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore X10-mouse-reporting mode");
			OPTION_RES(td, SM_MouseMode);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle X10-mouse-reporting mode");
			OPTION_TGV(td, SM_MouseMode, Mouse_X10, Mouse_None);
			break;
		}

		new_value = OPTION_GET(td, SM_MouseMode);
		if (new_value != old_value)
		{
		}
		break;

	case 18:			/* DECPFF - print form-feed */
		xvt_log_output_seq(td, "print form-feed on");
		old_value = OPTION_GET(td, SM_PrintScreenFF);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "print-screen-ff on");
			OPTION_SET(td, SM_PrintScreenFF, TRUE);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "print-screen-ff off");
			OPTION_SET(td, SM_PrintScreenFF, FALSE);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save print-screen-ff mode");
			OPTION_SAV(td, SM_PrintScreenFF);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore print-screen-ff mode");
			OPTION_RES(td, SM_PrintScreenFF);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle print-screen-ff mode");
			OPTION_TGL(td, SM_PrintScreenFF);
			break;
		}

		new_value = OPTION_GET(td, SM_PrintScreenFF);
		if (new_value != old_value)
		{
		}
		break;

	case 19:			/* DECPEX - print screen area */
		old_value = OPTION_GET(td, SM_PrintScreenRegion);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "print full screen");
			OPTION_SET(td, SM_PrintScreenRegion, TRUE);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "print screen region");
			OPTION_SET(td, SM_PrintScreenRegion, FALSE);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save print-screen-area mode");
			OPTION_SAV(td, SM_PrintScreenRegion);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore print-screen-area mode");
			OPTION_RES(td, SM_PrintScreenRegion);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle print-screen-area mode");
			OPTION_TGL(td, SM_PrintScreenRegion);
			break;
		}

		new_value = OPTION_GET(td, SM_PrintScreenRegion);
		if (new_value != old_value)
		{
		}
		break;

	case 25:			/* DECTCEM - cursor mode */
		old_value = OPTION_GET(td, SM_ShowCursor);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "cursor on");
			OPTION_SET(td, SM_ShowCursor, TRUE);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "cursor off");
			OPTION_SET(td, SM_ShowCursor, FALSE);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save cursor mode");
			OPTION_SAV(td, SM_ShowCursor);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore cursor mode");
			OPTION_RES(td, SM_ShowCursor);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle cursor mode");
			OPTION_TGL(td, SM_ShowCursor);
			break;
		}

		new_value = OPTION_GET(td, SM_ShowCursor);
		if (new_value != old_value)
		{
			if (CUR_SCRN_CSV(td) != new_value)
			{
				CUR_SCRN_CSV(td) = new_value;
				xvt_term_set_txt_cursor(td, new_value);
			}
		}
		break;

	case 35:				/* special shift keys */
		old_value = OPTION_GET(td, SM_SpecialKeys);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "special-keys on");
			OPTION_SET(td, SM_SpecialKeys, TRUE);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "special-keys off");
			OPTION_SET(td, SM_SpecialKeys, FALSE);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save special-keys mode");
			OPTION_SAV(td, SM_SpecialKeys);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore special-keys mode");
			OPTION_RES(td, SM_SpecialKeys);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle special-keys mode");
			OPTION_TGL(td, SM_SpecialKeys);
			break;
		}

		new_value = OPTION_GET(td, SM_SpecialKeys);
		if (new_value != old_value)
		{
		}
		break;

	case 38:				/* DECTEK - Tektronix mode */
		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "enter Tektronix mode: ignored");
			break;
		}
		break;

	case 40:				/* 80-132 switch */
		old_value = OPTION_GET(td, SM_AllowWideMode);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "allow-80/132-switch on");
			OPTION_SET(td, SM_AllowWideMode, TRUE);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "allow-80/132-switch off");
			OPTION_SET(td, SM_AllowWideMode, FALSE);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save allow-80/132-switch mode");
			OPTION_SAV(td, SM_AllowWideMode);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore allow-80/132-switch mode");
			OPTION_RES(td, SM_AllowWideMode);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle allow-80/132-switch mode");
			OPTION_TGL(td, SM_AllowWideMode);
			break;
		}

		new_value = OPTION_GET(td, SM_AllowWideMode);
		if (new_value != old_value)
		{
		}
		break;

	case 44:				/* margin bell */
		old_value = OPTION_GET(td, SM_MarginBell);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "margin-bell on");
			OPTION_SET(td, SM_MarginBell, TRUE);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "margin-bell off");
			OPTION_SET(td, SM_MarginBell, FALSE);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save margin-bell mode");
			OPTION_SAV(td, SM_MarginBell);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore margin-bell mode");
			OPTION_RES(td, SM_MarginBell);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle margin-bell mode");
			OPTION_TGL(td, SM_MarginBell);
			break;
		}

		new_value = OPTION_GET(td, SM_MarginBell);
		if (new_value != old_value)
		{
		}
		break;

	case 45:				/* reverse wrap-around */
		old_value = OPTION_GET(td, SM_ReverseWrap);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "reverse-wraparound on");
			OPTION_SET(td, SM_ReverseWrap, TRUE);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "reverse-wraparound off");
			OPTION_SET(td, SM_ReverseWrap, FALSE);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save reverse-wraparound mode");
			OPTION_SAV(td, SM_ReverseWrap);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore reverse-wraparound mode");
			OPTION_RES(td, SM_ReverseWrap);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle reverse-wraparound mode");
			OPTION_TGL(td, SM_ReverseWrap);
			break;
		}

		new_value = OPTION_GET(td, SM_ReverseWrap);
		if (new_value != old_value)
		{
		}
		break;

	case 46:				/* logging */
		old_value = OPTION_GET(td, SM_Logging);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "logging on");
			OPTION_SET(td, SM_Logging, TRUE);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "logging off");
			OPTION_SET(td, SM_Logging, FALSE);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save logging mode");
			OPTION_SAV(td, SM_Logging);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore logging mode");
			OPTION_RES(td, SM_Logging);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle logging mode");
			OPTION_TGL(td, SM_Logging);
			break;
		}

		new_value = OPTION_GET(td, SM_Logging);
		if (new_value != old_value)
		{
			xvt_log_set(td, XVT_LOG_EVENTS, new_value);
		}
		break;

	case 47:				/* alt-buf */
		old_value = OPTION_GET(td, SM_AltBuffer);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "alt-buffer on");
			OPTION_SET(td, SM_AltBuffer, TRUE);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "alt-buffer off");
			OPTION_SET(td, SM_AltBuffer, FALSE);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save alt-buffer mode");
			OPTION_SAV(td, SM_AltBuffer);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore alt-buffer mode");
			OPTION_RES(td, SM_AltBuffer);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle alt-buffer mode");
			OPTION_TGL(td, SM_AltBuffer);
			break;
		}

		new_value = OPTION_GET(td, SM_AltBuffer);
		if (new_value != old_value)
		{
			if (new_value)
			{
				if (CUR_SCRN(td) == td->reg_scrn)
				{
					CUR_SCRN(td) = td->alt_scrn;
					xvt_term_repaint(td);
				}
			}
			else
			{
				if (CUR_SCRN(td) == td->alt_scrn)
				{
					CUR_SCRN(td) = td->reg_scrn;
					xvt_term_repaint(td);
				}
			}
		}
		break;

	case 66:				/* application keypad keys */
		old_value = OPTION_GET(td, SM_AppKeypadKeys);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "app-keypad-keys on");
			OPTION_SET(td, SM_AppKeypadKeys, TRUE);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "app-keypad-keys off");
			OPTION_SET(td, SM_AppKeypadKeys, FALSE);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save app-keypad-keys mode");
			OPTION_SAV(td, SM_AppKeypadKeys);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore app-keypad-keys mode");
			OPTION_RES(td, SM_AppKeypadKeys);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle app-keypad-keys mode");
			OPTION_TGL(td, SM_AppKeypadKeys);
			break;
		}

		new_value = OPTION_GET(td, SM_AppKeypadKeys);
		if (new_value != old_value)
		{
			xvt_vt100_DECNKM(td, new_value);
		}
		break;

	case 67:				/* BS sends DEL */
		old_value = OPTION_GET(td, SM_BSsendsDel);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "BS-sends-DEL on");
			OPTION_SET(td, SM_BSsendsDel, TRUE);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "BS-sends-DEL off");
			OPTION_SET(td, SM_BSsendsDel, FALSE);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save BS-sends-DEL mode");
			OPTION_SAV(td, SM_BSsendsDel);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore BS-sends-DEL mode");
			OPTION_RES(td, SM_BSsendsDel);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle BS-sends-DEL mode");
			OPTION_TGL(td, SM_BSsendsDel);
			break;
		}

		new_value = OPTION_GET(td, SM_BSsendsDel);
		if (new_value != old_value)
		{
		}
		break;

	case 1000:				/* X11 mouse reporting */
		old_value = OPTION_GET(td, SM_MouseMode);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "X11-mouse-reporting on");
			OPTION_SET(td, SM_MouseMode, Mouse_X11);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "X11-mouse-reporting off");
			OPTION_SET(td, SM_MouseMode, Mouse_None);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save X11-mouse-reporting mode");
			OPTION_SAV(td, SM_MouseMode);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore X11-mouse-reporting mode");
			OPTION_RES(td, SM_MouseMode);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle X11-mouse-reporting mode");
			OPTION_TGV(td, SM_MouseMode, Mouse_X11, Mouse_None);
			break;
		}

		new_value = OPTION_GET(td, SM_MouseMode);
		if (new_value != old_value)
		{
		}
		break;

	case 1001:				/* hilite mouse reporting */
		old_value = OPTION_GET(td, SM_MouseMode);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "hilite-mouse-reporting on");
			OPTION_SET(td, SM_MouseMode, Mouse_Hilite);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "hilite-mouse-reporting off");
			OPTION_SET(td, SM_MouseMode, Mouse_None);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save hilite-mouse-reporting mode");
			OPTION_SAV(td, SM_MouseMode);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore hilite-mouse-reporting mode");
			OPTION_RES(td, SM_MouseMode);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle hilite-mouse-reporting mode");
			OPTION_TGV(td, SM_MouseMode, Mouse_Hilite, Mouse_None);
			break;
		}

		new_value = OPTION_GET(td, SM_MouseMode);
		if (new_value != old_value)
		{
		}
		break;

	case 1002:				/* cell-motion mouse reporting */
		old_value = OPTION_GET(td, SM_MouseMode);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "cell-motion-mouse-reporting on");
			OPTION_SET(td, SM_MouseMode, Mouse_CellMotion);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "cell-motion-mouse-reporting off");
			OPTION_SET(td, SM_MouseMode, Mouse_None);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save cell-motion-mouse-reporting mode");
			OPTION_SAV(td, SM_MouseMode);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore cell-motion-mouse-reporting mode");
			OPTION_RES(td, SM_MouseMode);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle cell-motion-mouse-reporting mode");
			OPTION_TGV(td, SM_MouseMode, Mouse_CellMotion, Mouse_None);
			break;
		}

		new_value = OPTION_GET(td, SM_MouseMode);
		if (new_value != old_value)
		{
		}
		break;

	case 1003:				/* all-motion mouse reporting */
		old_value = OPTION_GET(td, SM_MouseMode);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "all-motion-mouse-reporting on");
			OPTION_SET(td, SM_MouseMode, Mouse_AllMotion);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "all-motion-mouse-reporting off");
			OPTION_SET(td, SM_MouseMode, Mouse_None);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save all-motion-mouse-reporting mode");
			OPTION_SAV(td, SM_MouseMode);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore all-motion-mouse-reporting mode");
			OPTION_RES(td, SM_MouseMode);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle all-motion-mouse-reporting mode");
			OPTION_TGV(td, SM_MouseMode, Mouse_AllMotion, Mouse_None);
			break;
		}

		new_value = OPTION_GET(td, SM_MouseMode);
		if (new_value != old_value)
		{
		}
		break;

	case 1004:				/* XVT mouse reporting */
		old_value = OPTION_GET(td, SM_MouseMode);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "xvt-mouse-reporting on");
			OPTION_SET(td, SM_MouseMode, Mouse_XVT);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "xvt-mouse-reporting off");
			OPTION_SET(td, SM_MouseMode, Mouse_None);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save xvt-mouse-reporting mode");
			OPTION_SAV(td, SM_MouseMode);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore xvt-mouse-reporting mode");
			OPTION_RES(td, SM_MouseMode);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle xvt-mouse-reporting mode");
			OPTION_TGV(td, SM_MouseMode, Mouse_XVT, Mouse_None);
			break;
		}

		new_value = OPTION_GET(td, SM_MouseMode);
		if (new_value != old_value)
		{
		}
		break;

	case 1010:				/* scroll to bottom on output	*/
		old_value = OPTION_GET(td, SM_ScrollOnOutput);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "scroll-on-output on");
			OPTION_SET(td, SM_ScrollOnOutput, TRUE);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "scroll-on-output off");
			OPTION_SET(td, SM_ScrollOnOutput, FALSE);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save scroll-on-output mode");
			OPTION_SAV(td, SM_ScrollOnOutput);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore scroll-on-output mode");
			OPTION_RES(td, SM_ScrollOnOutput);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle scroll-on-output mode");
			OPTION_TGL(td, SM_ScrollOnOutput);
			break;
		}

		new_value = OPTION_GET(td, SM_ScrollOnOutput);
		if (new_value != old_value)
		{
		}
		break;

	case 1011:				/* scroll to bottom on keypress	*/
		old_value = OPTION_GET(td, SM_ScrollOnKeypress);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "scroll-on-keypress on");
			OPTION_SET(td, SM_ScrollOnKeypress, TRUE);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "scroll-on-keypress off");
			OPTION_SET(td, SM_ScrollOnKeypress, FALSE);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save scroll-on-keypress mode");
			OPTION_SAV(td, SM_ScrollOnKeypress);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore scroll-on-keypress mode");
			OPTION_RES(td, SM_ScrollOnKeypress);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle scroll-on-keypress mode");
			OPTION_TGL(td, SM_ScrollOnKeypress);
			break;
		}

		new_value = OPTION_GET(td, SM_ScrollOnKeypress);
		if (new_value != old_value)
		{
		}
		break;

	case 1037:				/* DEL-sends-DEL mode */
		old_value = OPTION_GET(td, SM_DELsendsDel);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "DEL-sends-DEL on");
			OPTION_SET(td, SM_DELsendsDel, TRUE);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "DEL-sends-DEL off");
			OPTION_SET(td, SM_DELsendsDel, FALSE);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save DEL-sends-DEL mode");
			OPTION_SAV(td, SM_DELsendsDel);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore DEL-sends-DEL mode");
			OPTION_RES(td, SM_DELsendsDel);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle DEL-sends-DEL mode");
			OPTION_TGL(td, SM_DELsendsDel);
			break;
		}

		new_value = OPTION_GET(td, SM_DELsendsDel);
		if (new_value != old_value)
		{
		}
		break;

	/*----------------------------------------------------------------
	 * XVT-specific cmds
	 */
	case XVT_PRI_KBD_OFF:			/* XVT: kbd-map off */
	case XVT_PRI_KBD_ONE:			/* XVT: kbd-map = 1 */
	case XVT_PRI_KBD_TWO:			/* XVT: kbd-map = 2 */
		xvt_vt100_kbdmap(td, n - XVT_PRI_KBD_OFF);
		break;

	case XVT_PRI_LBLINFO:
		old_value = OPTION_GET(td, SM_LabelInfo);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "label-info on");
			OPTION_SET(td, SM_LabelInfo, TRUE);
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "label-info off");
			OPTION_SET(td, SM_LabelInfo, FALSE);
			break;

		case Option_Save:
			xvt_log_output_seq(td, "save label-info mode");
			OPTION_SAV(td, SM_LabelInfo);
			break;

		case Option_Restore:
			xvt_log_output_seq(td, "restore label-info mode");
			OPTION_RES(td, SM_LabelInfo);
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle label-info mode");
			OPTION_TGL(td, SM_LabelInfo);
			break;
		}

		new_value = OPTION_GET(td, SM_LabelInfo);
		if (new_value != old_value)
		{
			xvt_term_set_win_title(td, td->win_label);
		}
		break;

	case XVT_PRI_LOG_EVT:
	case XVT_PRI_LOG_ACT:
	case XVT_PRI_LOG_OUT:
	case XVT_PRI_LOG_SCR:
	case XVT_PRI_LOG_INP:
	case XVT_PRI_LOG_KEY:
		n -= XVT_PRI_LOGGING;
		old_value = xvt_log_is_on(td, n);

		switch (cmd)
		{
		case Option_Set:
			xvt_log_output_seq(td, "log %d on", n);
			new_value = TRUE;
			break;

		case Option_Reset:
			xvt_log_output_seq(td, "log %d off", n);
			new_value = FALSE;
			break;

		case Option_Toggle:
			xvt_log_output_seq(td, "toggle log %d mode", n);
			new_value = ! old_value;
			break;
		}

		if (new_value != old_value)
		{
			xvt_log_set(td, n, new_value);
		}
		break;

	default:
		xvt_log_output_seq(td, "invalid PRI set opt %d", n);
		break;
	}
}

/*------------------------------------------------------------------------
 * DECSET 	set private mode
 */
void xvt_vt100_PRI_DECSET (TERMDATA *td)
{
	int i;

	for (i=0; i<=te->n_index; i++)
	{
		int n = te->n[i];

		xvt_vt100_PRI_PROCESS(td, n, Option_Set);
	}
}

/*------------------------------------------------------------------------
 * DECRST	reset private mode
 */
void xvt_vt100_PRI_DECRST (TERMDATA *td)
{
	int i;

	for (i=0; i<=te->n_index; i++)
	{
		int n = te->n[i];

		xvt_vt100_PRI_PROCESS(td, n, Option_Reset);
	}
}

/*------------------------------------------------------------------------
 * DECSAV	save private mode
 */
void xvt_vt100_PRI_DECSAV (TERMDATA *td)
{
	int i;

	for (i=0; i<=te->n_index; i++)
	{
		int n = te->n[i];

		xvt_vt100_PRI_PROCESS(td, n, Option_Save);
	}
}

/*------------------------------------------------------------------------
 * DECRES	restore private mode
 */
void xvt_vt100_PRI_DECRES (TERMDATA *td)
{
	int i;

	for (i=0; i<=te->n_index; i++)
	{
		int n = te->n[i];

		xvt_vt100_PRI_PROCESS(td, n, Option_Restore);
	}
}

/*------------------------------------------------------------------------
 * DECTGL	toggle private mode
 */
void xvt_vt100_PRI_DECTGL (TERMDATA *td)
{
	int i;

	for (i=0; i<=te->n_index; i++)
	{
		int n = te->n[i];

		xvt_vt100_PRI_PROCESS(td, n, Option_Toggle);
	}
}

/*========================================================================
 * XVT mode commands
 *========================================================================
 */

/*------------------------------------------------------------------------
 * XVT_KBDMAP	set keyboard mapping
 */
void xvt_vt100_XVT_KBDMAP (TERMDATA *td)
{
	xvt_vt100_kbdmap(td, te->n[0]);
}

/*========================================================================
 * OSC mode commands
 *========================================================================
 */

/*------------------------------------------------------------------------
 * OSC_REPORT	report OSC info
 */
static void xvt_vt100_OSC_REPORT (TERMDATA *td)
{
	int n = te->n[0];

	/*--------------------------------------------------------------------
	 * now process the report request
	 */
	switch (n)
	{
	case 0:
		xvt_log_output_seq(td, "report win&icon-label");
		xvt_term_write_report(td, "\033]0;%s\007", td->win_label);
		break;

	case 1:
		xvt_log_output_seq(td, "report icon-label");
		xvt_term_write_report(td, "\033]1;%s\007", td->ico_label);
		break;

	case 2:
		xvt_log_output_seq(td, "report win-label");
		xvt_term_write_report(td, "\033]2;%s\007", td->win_label);
		break;

	case 4:
		xvt_log_output_seq(td, "report screen colors");
		{
			int i;

			for (i=0; i<XVT_CLR_NUM; i++)
			{
				xvt_term_write_report(td,
					"\033]4;%d;%s\007", i, xvt_term_get_color_name(td, i));
			}
		}
		break;

	case 10:
		xvt_log_output_seq(td, "report foreground color");
		xvt_term_write_report(td,
			"\033]10;%s\007", xvt_term_get_color_name(td, XVT_CLR_FG));
		break;

	case 11:
		xvt_log_output_seq(td, "report background color");
		xvt_term_write_report(td,
			"\033]11;%s\007", xvt_term_get_color_name(td, XVT_CLR_BG));
		break;

	case 12:
		xvt_log_output_seq(td, "report pointer color");
		xvt_term_write_report(td,
			"\033]12;%s\007", xvt_term_get_color_name(td, XVT_CLR_PR));
		break;

	case 13:
		xvt_log_output_seq(td, "report border color");
		xvt_term_write_report(td,
			"\033]13;%s\007", xvt_term_get_color_name(td, XVT_CLR_BD));
		break;

	case 14:
		xvt_log_output_seq(td, "report cursor color");
		xvt_term_write_report(td,
			"\033]14;%s\007", xvt_term_get_color_name(td, XVT_CLR_CR));
		break;

	case 46:
		xvt_log_output_seq(td, "report log path");
		xvt_term_write_report(td,
			"\033]46;%s\007", td->xd->events_path);
		break;

	case 50:
		xvt_log_output_seq(td, "report current font");
		xvt_term_write_report(td,
			"\033]50;%s\007", xvt_term_get_font_name(td, td->tw->cur_font_no));
		break;

	default:
		xvt_log_output_seq(td, "unknown OSC mode %d", n);
		break;
	}
}

/*------------------------------------------------------------------------
 * OSC_LABEL	OSC labels
 */
void xvt_vt100_OSC_LABEL (TERMDATA *td)
{
	int n = te->n[0];

	/*--------------------------------------------------------------------
	 * delete the trailing ^G
	 */
	te->data_chars[--te->data_cnt] = 0;

	/*--------------------------------------------------------------------
	 * check if report wanted
	 */
	if (strcmp(te->data_chars, "?") == 0)
	{
		xvt_vt100_OSC_REPORT(td);
		return;
	}

	/*--------------------------------------------------------------------
	 * now process the data
	 */
	switch (n)
	{
	case 0:
		xvt_log_output_seq(td, "win&icon-label:%s", te->data_chars);
		strcpy(td->win_label, te->data_chars);
		strcpy(td->ico_label, te->data_chars);
		xvt_term_set_win_title(td, td->win_label);
		xvt_term_set_ico_title(td, td->ico_label);
		break;

	case 1:
		xvt_log_output_seq(td, "icon-label:%s", te->data_chars);
		strcpy(td->ico_label, te->data_chars);
		xvt_term_set_ico_title(td, td->ico_label);
		break;

	case 2:
		xvt_log_output_seq(td, "win-label:%s", te->data_chars);
		strcpy(td->win_label, te->data_chars);
		xvt_term_set_win_title(td, td->win_label);
		break;

	case 4:
		{
			char *	s = te->data_chars;

			while (s != 0)
			{
				char *	p = strchr(s, ';');
				char *	e;
				int		clr_no;

				if (p == 0)
					break;

				*p++ = 0;
				e = strchr(p, ';');
				if (e != 0)
					*e++ = 0;

				clr_no = atoi(s);
				if (clr_no >= XVT_CLR_NUM)
				{
					xvt_log_output_seq(td, "invalid color number %d", clr_no);
				}
				else
				{
					if (strcmp(p, "?") == 0)
					{
						xvt_log_output_seq(td, "report color %d");
						xvt_term_write_report(td,
							"\033]4;%d;%s\007",
							clr_no, xvt_term_get_color_name(td, clr_no));
					}
					else
					{
						xvt_log_output_seq(td,
							"change color %d (%s) to \"%s\"",
							clr_no, color_name(clr_no), p);
						xvt_term_chg_color(td, clr_no, p);
					}
				}

				s = e;
			}
		}
		break;

	case 10:
		xvt_log_output_seq(td, "change fg color to %s", te->data_chars);
		xvt_term_chg_color(td, XVT_CLR_FG, te->data_chars);
		break;

	case 11:
		xvt_log_output_seq(td, "change bg color to %s", te->data_chars);
		xvt_term_chg_color(td, XVT_CLR_BG, te->data_chars);
		break;

	case 12:
		xvt_log_output_seq(td, "change pointer color to %s", te->data_chars);
		xvt_term_chg_color(td, XVT_CLR_PR, te->data_chars);
		break;

	case 13:
		xvt_log_output_seq(td, "change border color to %s", te->data_chars);
		xvt_term_chg_color(td, XVT_CLR_BD, te->data_chars);
		break;

	case 14:
		xvt_log_output_seq(td, "change cursor color to %s", te->data_chars);
		xvt_term_chg_color(td, XVT_CLR_CR, te->data_chars);
		break;

	case 46:
		if (*(te->data_chars) != 0)
		{
			int	log_no		= XVT_LOG_EVENTS;
			int	log_is_on	= xvt_log_is_on(td, log_no);

			xvt_log_output_seq(td, "set log path to \"%s\"", te->data_chars);
			if (log_is_on)
				xvt_log_set(td, log_no, FALSE);
			xvt_log_path(td, log_no, te->data_chars);
			if (log_is_on)
				xvt_log_set(td, log_no, TRUE);
		}
		break;

	case 50:
		if (*(te->data_chars) != 0)
		{
			char *	p = te->data_chars;
			int		d = 0;

			if (*p != '#')
			{
				xvt_log_output_seq(td, "set current font to \"%s\"", p);
				xvt_term_set_font_name(td, -1, p);
			}
			else
			{
				p++;

				if (*p == '+')
				{
					d = +1;
					p++;
				}
				else if (*p == '-')
				{
					d = -1;
					p++;
				}

				n = atoi(p);
				if (d == 0)
				{
					xvt_log_output_seq(td, "set font no to %d", n);
					xvt_term_set_font_no(td, n);
				}
				else if (d < 0)
				{
					if (n == 0)
						n = 1;
					xvt_log_output_seq(td, "lower font no by %d", n);
					xvt_term_chg_font_no(td, -n);
				}
				else /* (d > 0) */
				{
					if (n == 0)
						n = 1;
					xvt_log_output_seq(td, "raise font no by %d", n);
					xvt_term_chg_font_no(td, n);
				}
			}
		}
		break;

	default:
		xvt_log_output_seq(td, "unknown OSC mode %d", n);
		break;
	}
}

/*========================================================================
 * DEV mode commands
 *========================================================================
 */
