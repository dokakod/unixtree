/*------------------------------------------------------------------------
 * terminal emulator routines
 *
 *	This file processes all input from the "child", which may
 *	be actual characters to be displayed, or control/escape
 *	sequences to be processed.
 *
 *	The main entry point is xvt_emul_char_input(), which is called
 *	with each character read in from the child.  Note that this
 *	routine is essentially event-driven, so it maintains its current
 *	state about what it is currently doing.
 */

#include "xvtcommon.h"

/*------------------------------------------------------------------------
 * misc defines
 */
#define xvt_esc_clr(t)	memset(t, 0, sizeof(ESCINFO))

#define CMD_TBL(td)		((td)->cmd_tbl_ptr)
#define CMD_PTR(td)		(CMD_TBL(td)->esc_tbl + (td)->esc.vt_mode)

static void		xvt_emul_esc_seq	(TERMDATA *td, int c);
static void		xvt_emul_ctl_seq	(TERMDATA *td, int c);

/*------------------------------------------------------------------------
 * process an escape sequence
 *
 * This routine is called to start esc-seq processing and when
 */
static void xvt_emul_esc_seq (TERMDATA *td, int c)
{
	ESCINFO *			te	= &td->esc;
	const ESC_CMD *		ep; 
	const ESC_DATA *	ed;
	int					is_data;

	/*--------------------------------------------------------------------
	 * check if start of new ecape-sequence or a cancel
	 */
	switch (c)
	{
	case 0x1b:
		/*----------------------------------------------------------------
		 * Check if reading data which is terminated by an esc-seq.
		 * Otherwise, the ESC will trigger a new esc-seq and the
		 * current seq will be discarded.
		 */
		if (te->reading_esc_seq && CMD_PTR(td)->data == DATA_ESCAPE)
		{
			te->vt_mode			= 0;
			te->esc_char_cnt	= 0;
			te->esc_chars[0]	= 0;
			return;
		}

		/*----------------------------------------------------------------
		 * if in esc-seq, complain about it
		 */
		if (te->esc_char_cnt > 0)
		{
			xvt_log_output_seq(td, 
				"ignoring esc-seq \"\\e%s\"", te->esc_chars);
		}

		/*----------------------------------------------------------------
		 * restart esc-seq processing
		 */
		xvt_esc_clr(te);
		te->reading_esc_seq	= TRUE;
		return;

	case 0x18:
	case 0x1a:
		/*----------------------------------------------------------------
		 * cancel current escape sequence
		 */
		xvt_esc_clr(te);
		return;
	}

	/*--------------------------------------------------------------------
	 * process this char according to mode we are in
	 */
	ep = CMD_PTR(td);
	te->code = c;

	/*--------------------------------------------------------------------
	 * cache char in either esc-seq buffer or data buffer
	 */
	is_data = (ep->data != DATA_NONE);
	if (is_data)
	{
		/*----------------------------------------------------------------
		 * If this "data" seq uses params, treat char as data only if
		 * all params are collected.
		 */
		if (te->n_index >= ep->params)
		{
			if (te->data_cnt < (int)sizeof(te->data_chars)-1)
			{
				te->data_chars[te->data_cnt++]	= c;
				te->data_chars[te->data_cnt  ]	= 0;
			}
		}
		else
		{
			is_data = FALSE;
		}
	}

	if (! is_data)
	{
		if (te->esc_char_cnt < (int)sizeof(te->esc_chars)-1)
		{
			te->esc_chars[te->esc_char_cnt++] = c;
			te->esc_chars[te->esc_char_cnt]   = 0;
		}
	}

	/*--------------------------------------------------------------------
	 * Check if we should process params & if so,
	 * return if we found a param.
	 *
	 * Params are either a numeric value or a ";" which separates
	 * params.
	 */
	if (ep->params < 0)
	{
		/*----------------------------------------------------------------
		 * process parms as abcdef up to number of parms
		 */
		int n = -(ep->params);

		if (te->data_cnt < n)
		{
			te->data_chars[te->data_cnt++] = c;
			if (te->data_cnt < n)
				return;
			c = 0;
		}
	}
	else
	{
		/*----------------------------------------------------------------
		 * process parms as nnn;nnn;...
		 */
		if (te->n_index < ep->params)
		{
			if (isdigit(c))
			{
				te->n[te->n_index] = (10 * te->n[te->n_index]) + (c - '0');
				return;
			}

			if (c == ';')
			{
				te->n_index++;
				if (te->n_index >= ESC_NUM_PARAMS)
					te->n_index = ESC_NUM_PARAMS;
				return;
			}

			if (ep->codes != 0)
			{
				if (strchr(ep->codes, c) != 0)
				{
					te->char_code = c;
					return;
				}
			}
		}
	}

	/*--------------------------------------------------------------------
	 * process entries in table
	 */
	for (ed=ep->cmds; ed->code >= 0; ed++)
	{
		/*----------------------------------------------------------------
		 * we have the entry if char-codes match or code == 0
		 */
		if (ed->code == 0 || ed->code == c)
		{
			/*------------------------------------------------------------
			 * store type if specified
			 */
			if (ed->type >= 0)
				te->vt_type = ed->type;

			/*------------------------------------------------------------
			 * Check for change in mode, and return if it did.
			 */
			if (ed->mode >= 0)
			{
				te->vt_mode = ed->mode;
				return;
			}

			/*------------------------------------------------------------
			 * At this point, we *should* have a complete sequence.
			 * Dump it to the log if not a data entry or the start
			 * of a data entry.
			 */
			if (! is_data || te->data_cnt == 1)
			{
				xvt_log_output_seq(td, "%s: \"\\e%s\"",
					ep->name, te->esc_chars);
			}

			/*------------------------------------------------------------
			 * If description specified, log it. Normally, this is
			 * used for esc-seqs we don't process.
			 */
			if (ed->desc != 0)
			{
				xvt_log_output_seq(td, "%s", ed->desc);
			}

			/*------------------------------------------------------------
			 * if routine specified, call it
			 */
			if (ed->rtn != 0)
			{
				(ed->rtn)(td);
			}

			/*------------------------------------------------------------
			 * If neither "rtn" or "desc" were specified and
			 * "code" == 0, this means to just keep in esc-mode.
			 * (Normally at this time we end the esc-processing.)
			 */
			if (ed->code != 0 || ed->rtn != 0 || ed->desc != 0)
			{
				xvt_esc_clr(te);
				td->prev_char = 0;
			}

			return;
		}
	}

	/*--------------------------------------------------------------------
	 * unrecognized char - check if a control char
	 */
	if (c < ' ')
	{
		te->esc_chars[--te->esc_char_cnt] = 0;
		xvt_emul_ctl_seq(td, c);
		return;
	}

	/*--------------------------------------------------------------------
	 * unknown esc-seq
	 */
	xvt_log_output_seq(td, "ignoring %s esc-seq: \"\\e%s\"",
		ep->name, te->esc_chars);
	xvt_esc_clr(te);
	td->prev_char = 0;
}

/*------------------------------------------------------------------------
 * process a control character
 */
static void xvt_emul_ctl_seq (TERMDATA *td, int ch)
{
	/*--------------------------------------------------------------------
	 * debug output
	 */
	if (ch != 0x1b)
		xvt_log_output_chr(td, "^%c", ch + '@');

	if (ch == '\n')
		xvt_log_output_chr(td, "\n");

	/*--------------------------------------------------------------------
	 * process the control char
	 */
	if (ch == 0x1b)
	{
		xvt_emul_esc_seq(td, ch);
	}
	else
	{
		const CTL_CMD *	ct;

		for (ct = CMD_TBL(td)->ctl_tbl; ct->ctl_char >= 0; ct++)
		{
			if (ch == ct->ctl_char)
			{
				(ct->ctl_rtn)(td);
				break;
			}
		}
		td->prev_char = 0;
	}
}

/*------------------------------------------------------------------------
 * process an input char
 */
void xvt_emul_char_input (TERMDATA *td, int ch)
{
	ESCINFO *	te	= &td->esc;

	if (td->prt_mode != PRT_MODE_NONE)
	{
		(CMD_TBL(td)->prt_rtn)(td, ch);
	}
	else if (te->reading_esc_seq)
	{
		xvt_emul_esc_seq(td, ch);
	}
	else if (ch < ' ')
	{
		xvt_emul_ctl_seq(td, ch);
	}
	else
	{
		(CMD_TBL(td)->out_rtn)(td, ch);
	}
}

/*------------------------------------------------------------------------
 * process an input char for local echo
 */
void xvt_emul_local_echo (TERMDATA *td, int ch)
{
	if (ch >= ' ')
	{
		(CMD_TBL(td)->out_rtn)(td, ch);
	}
	else
	{
		switch (ch)
		{
		case '\r':
		case '\n':
		case '\t':
		case '\f':
		case '\v':
			xvt_emul_ctl_seq(td, ch);
			break;
		}
	}
}

/*------------------------------------------------------------------------
 * setup a command table
 *
 * If <td> is NULL, then this is a validate call only
 */
int xvt_emul_set_tbl (TERMDATA *td, const char *name)
{
	const CMD_TBL **	ctp;

	for (ctp = xvt_cmds; *ctp; ctp++)
	{
		const CMD_TBL *	ct	= *ctp;

		if (xvt_strccmp(name, ct->tbl_name) == 0)
		{
			if (td != 0)
			{
				CMD_TBL(td) = ct;
				(CMD_TBL(td)->init_rtn)(td);
			}
			return (0);
		}
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * get TERM type for terminal
 */
const char * xvt_emul_get_term (TERMDATA *td)
{
	return (CMD_TBL(td)->term_type);
}
