/*------------------------------------------------------------------------
 * terminal characteristics routines
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * tcap_set_term_db() - set term database type
 */
int tcap_set_term_db (const char *term_db_type)
{
	int type;

	if (! is_cmd_pres(term_db_type))
	{
		return (-1);
	}

	type = tcap_db_name_to_val(term_db_type);
	if (type < 0)
	{
		return (-1);
	}

	return tcap_db_set_type(type);
}

/*------------------------------------------------------------------------
 * tcap_get_term_db() - get term database type
 */
const char * tcap_get_term_db (void)
{
	int	type = tcap_db_get_type();

	return tcap_db_val_to_name(type);
}

/*------------------------------------------------------------------------
 * tcap_get_term_db_name() - get term database name
 */
const char * tcap_get_term_db_name (int num)
{
	return tcap_db_num_to_name(num);
}

/*------------------------------------------------------------------------
 * tcap_get_term_db_desc() - get term database desc
 */
const char * tcap_get_term_db_desc (int num)
{
	return tcap_db_num_to_desc(num);
}

/*------------------------------------------------------------------------
 * tcap_dump_term() - dump a term database
 */
int tcap_dump_term (const char *term, FILE *fp)
{
	return tcap_db_dump_term(tcap_db_get_type(), term, fp);
}

/*------------------------------------------------------------------------
 * tcap_set_icon() - set icon for display
 *
 * icon data is as follows:
 *
 *		ICBS esc-sequence
 *		width
 *		height
 *		((width * height) / 8) data bytes ("height" lines of "width" bits)
 *		ICBE esc-seq
 *
 *		width	one byte  encoded as (width  + ' ')
 *		height	one byte  encoded as (height + ' ')
 *		data	two bytes encoded as ((b >> 4) + ' ', (b & 0x0f) + ' ')
 */
int tcap_set_icon (TERMINAL *tp, int width, int height,
	const unsigned char *bits)
{
	if (tp == 0 ||
		width  <= 0 || width  > 64 ||
		height <= 0 || height > 64 ||
		bits == 0)
	{
		return (-1);
	}

	if (! is_cmd_pres(tp->tcap->strs.icbs) ||
	    ! is_cmd_pres(tp->tcap->strs.icbe))
	{
		return (0);
	}

	TERMINAL_LOCK(tp);
	{
		int i;
		int bytes = (width * height) / 8;

		tcap_outcmd(tp, S_ICBS, tp->tcap->strs.icbs);

		tcap_outch(tp, width  + ' ', FALSE);
		tcap_outch(tp, height + ' ', FALSE);

		for (i = 0; i < bytes; i++)
		{
			int	l = bits[i] >> 4;
			int r = bits[i]  & 0x0f;

			tcap_outch(tp, l + ' ', FALSE);
			tcap_outch(tp, r + ' ', FALSE);
		}

		tcap_outcmd(tp, S_ICBE, tp->tcap->strs.icbe);
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_set_evt_rtn() - set an event routine to be called
 */
int tcap_set_evt_rtn (TERMINAL *tp,
	TCAP_EVENT_RTN *rtn,
	void *data)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_screen_dev_evt(tp->term, rtn, data);
	}
	TERMINAL_UNLOCK(tp);

	if (rc)
		return (-1);

#ifdef SIGTSTP
	/*--------------------------------------------------------------------
	 * If we are using an event routine, we are not under control of
	 * a shell, so lose our job-control stuff (since there's no shell
	 * to restore it).
	 */
	if (data != 0)
		sys_sig_set(SIGTSTP, SIG_DEFAULT, 0);
#endif

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_set_inp_rtn() - set an input routine to be called
 */
int tcap_set_inp_rtn (TERMINAL *tp,
	TCAP_INPUT_RTN *rtn,
	void *data)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_screen_dev_inp(tp->term, rtn, data);
	}
	TERMINAL_UNLOCK(tp);

	if (rc)
		return (-1);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_set_out_rtn() - set an output routine to be called
 */
int tcap_set_out_rtn (TERMINAL *tp,
	TCAP_OUTPUT_RTN *rtn,
	void *data)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = term_screen_dev_out(tp->term, rtn, data);
	}
	TERMINAL_UNLOCK(tp);

	if (rc)
		return (-1);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_get_window_env() - get whether in a window or not
 */
int tcap_get_window_env (void)
{
	return term_window_check();
}

/*------------------------------------------------------------------------
 * tcap_get_disp_in_win() - get whether in a window or not
 */
int tcap_get_disp_in_win (const TERMINAL *tp)
{
	if (tp == 0)
		return (FALSE);

	return (tp->scrn->disp_in_window);
}

/*------------------------------------------------------------------------
 * tcap_get_orig_attr() - get original screen attribute if possible
 */
attr_t tcap_get_orig_attr (const TERMINAL *tp)
{
	if (tp == 0)
		return (A_UNSET);

#if V_WINDOWS
	if (tp->scrn->orig_attr == A_UNSET &&
	    tp->tcap->db_type   == TCAP_DB_WINDOWS)
	{
		int fg;
		int bg;
		int	rc;

		rc = term_win_get_color(tp->term, &fg, &bg);
		if (rc == 0)
		{
			tp->scrn->orig_attr = A_CLR(fg, bg);
		}
	}
#endif

	return (tp->scrn->orig_attr);
}

/*------------------------------------------------------------------------
 * tcap_get_termname() - get value of TERM env variable
 */
const char *tcap_get_termname (const TERMINAL *tp)
{
	return getenv("TERM");
}

/*------------------------------------------------------------------------
 * tcap_get_termtype() - get name of TERM used to load a TERMINAL struct
 */
const char *tcap_get_termtype (const TERMINAL *tp)
{
	if (tp == 0)
		return ("?");

	return (tp->tcap->tty_type);
}

/*------------------------------------------------------------------------
 * tcap_get_longname() - get description of terminal type
 */
const char *tcap_get_longname (const TERMINAL *tp)
{
	if (tp == 0)
		return ("?");

	if (is_cmd_pres(tp->tcap->strs.desc))
		return (tp->tcap->strs.desc);
	else
		return (tp->tcap->tty_type);
}

/*------------------------------------------------------------------------
 * tcap_get_ttyname() - get name of device for a terminal
 */
const char *tcap_get_ttyname (const TERMINAL *tp)
{
	if (tp == 0)
		return ("?");

	return term_get_tty_name(tp->term);
}

/*------------------------------------------------------------------------
 * tcap_get_max_rows() - get max rows on screen
 */
int tcap_get_max_rows (const TERMINAL *tp)
{
	if (tp == 0)
		return (0);

	return (tp->tcap->ints.maxrows);
}

/*------------------------------------------------------------------------
 * tcap_get_max_cols() - get max cols on screen
 */
int tcap_get_max_cols (const TERMINAL *tp)
{
	if (tp == 0)
		return (0);

	return (tp->tcap->ints.maxcols);
}

/*------------------------------------------------------------------------
 * tcap_get_tab_size() - get tab width for a screen
 */
int tcap_get_tab_size (const TERMINAL *tp)
{
	int ts = 8;

	if (tp != 0)
	{
		if (tp->tcap->ints.tabsize != 0)
			ts = tp->tcap->ints.tabsize;
	}

	return (ts);
}

/*------------------------------------------------------------------------
 * tcap_get_has_ic() - check whether ic/dc capability is present
 */
int tcap_get_has_ic (const TERMINAL *tp)
{
	if (tp == 0)
		return (FALSE);

	if (is_cmd_pres(tp->tcap->strs.ic) && is_cmd_pres(tp->tcap->strs.dc))
		return (TRUE);
	else
		return (FALSE);
}

/*------------------------------------------------------------------------
 * tcap_get_has_il() - check whether il/dl capability is present
 */
int tcap_get_has_il (const TERMINAL *tp)
{
	if (tp == 0)
		return (FALSE);

	if (is_cmd_pres(tp->tcap->strs.al) && is_cmd_pres(tp->tcap->strs.dl))
		return (TRUE);
	else
		return (FALSE);
}

/*------------------------------------------------------------------------
 * tcap_get_termattrs() - get mask of attributes available
 */
attr_t tcap_get_termattrs (const TERMINAL *tp)
{
	attr_t ch = 0;

	if (tp == 0)
		return (ch);

	if ( is_cmd_pres(tp->tcap->strs.sts) )	ch |= A_STANDOUT;
	if ( is_cmd_pres(tp->tcap->strs.uls) )	ch |= A_UNDERLINE;
	if ( is_cmd_pres(tp->tcap->strs.bds) )	ch |= A_BOLD;
	if ( is_cmd_pres(tp->tcap->strs.bls) )	ch |= A_BLINK;
	if ( is_cmd_pres(tp->tcap->strs.dms) )	ch |= A_DIM;
	if ( is_cmd_pres(tp->tcap->strs.ivs) )	ch |= A_INVIS;
	if ( is_cmd_pres(tp->tcap->strs.prs) )	ch |= A_PROTECT;

	return (ch);
}

/*------------------------------------------------------------------------
 * tcap_get_cmdstr() - get cmd string associated with a TC_DATA entry
 */
static int tcap_get_cmdstr (const TERMINAL *tp, const TC_DATA *tc, char *buf)
{
	const TCAP_DATA *	td	= tp->tcap;
	const char **		cp;
	const int *			ip;
	const int *			bp;

	switch (tc->trmc_flags & TC_TYPE)
	{
	case TC_NUM:
		ip = TCAP_TC_NUM_GP(td, tc);
		sprintf(buf, "%d\n", *ip);
		break;

	case TC_BLN:
		bp = TCAP_TC_BLN_GP(td, tc);
		sprintf(buf, "%d\n", *bp);
		break;

	case TC_STR:
		cp = TCAP_TC_STR_GP(td, tc);
		if (! is_cmd_pres(*cp))
			*buf = 0;
		else
			strcpy(buf, *cp);
		break;

	default:
		*buf = 0;
		break;
	}

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_get_cmdstr_by_enum() - get cmd string associated with an enum
 */
int tcap_get_cmdstr_by_enum (const TERMINAL *tp, SCRN_CMD cmd, char *buf)
{
	const TC_DATA *		tc;
	const TCAP_DATA *	td;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0 || buf == 0)
		return (-1);

	td = tp->tcap;

	*buf = 0;

	/*--------------------------------------------------------------------
	 * special case of database type
	 */
	if (cmd == S_DB)
	{
		sprintf(buf, "%s\n", tcap_db_num_to_name(td->db_type));
		return (0);
	}

	/*--------------------------------------------------------------------
	 * find entry in tcap table
	 */
	for (tc=tcap_termcap_data; tc->trmc_label; tc++)
	{
		if (tc->trmc_cmd == cmd)
			break;
	}

	if (tc->trmc_label == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * return entry
	 */
	return tcap_get_cmdstr(tp, tc, buf);
}

/*------------------------------------------------------------------------
 * tcap_get_cmdstr_by_name() - get cmd string associated with a name
 */
int tcap_get_cmdstr_by_name (const TERMINAL *tp, const char *name,
	SCRN_CMD *pcmd, char *buf)
{
	const TC_DATA *		tc;
	const KF_TABLE *	kf;
	const TCAP_DATA *	td;
	int					key;
	int					clr;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0 || buf == 0 || pcmd == 0 || name == 0 || *name == 0)
		return (-1);

	td = tp->tcap;

	*pcmd	= S_ZERO;
	*buf	= 0;

	/*--------------------------------------------------------------------
	 * special case of database type
	 */
	if (tcap_trmcmp(name, tcap_keyword_dbtype) == 0)
	{
		*pcmd = S_DB;
		return tcap_get_cmdstr_by_enum(tp, S_DB, buf);
	}

	/*--------------------------------------------------------------------
	 * find entry in tcap table
	 */
	for (tc=tcap_termcap_data; tc->trmc_label; tc++)
	{
		if (tc->trmc_label  && tcap_trmcmp(name, tc->trmc_label)  == 0)
			break;

		if (tc->trmc_tcname && tcap_trmcmp(name, tc->trmc_tcname) == 0)
			break;

		if (tc->trmc_tiname && tcap_trmcmp(name, tc->trmc_tiname) == 0)
			break;
	}

	/*--------------------------------------------------------------------
	 * if tcap entry found, return it
	 */
	if (tc->trmc_label != 0)
	{
		*pcmd = tc->trmc_cmd;
		return tcap_get_cmdstr(tp, tc, buf);
	}

	/*--------------------------------------------------------------------
	 * if entry not found, check if it is a function key name
	 */
	key = -1;

	for (kf=tcap_tc_keys; kf->kf_tcname; kf++)
	{
		if (tcap_trmcmp(kf->kf_tcname, name) == 0 ||
		    tcap_trmcmp(kf->kf_tiname, name) == 0)
		{
			key = kf->kf_code;
			break;
		}
	}

	if (key == -1)
		key = tcap_get_key_value(name);

	/*--------------------------------------------------------------------
	 * if function key name, look up its value
	 */
	if (key != -1)
	{
		KFUNC *	k;

		for (k=td->kfuncs; k; k=k->kf_next)
		{
			if (k->kf_code == key)
			{
				strcpy(buf, k->kf_str);
				break;
			}
		}

		return (0);
	}

	/*--------------------------------------------------------------------
	 * if not a function key, check if color name
	 */
	clr = tcap_get_fg_num_by_name(name);
	if (clr != -1)
	{
		if (is_cmd_pres(td->strs.clr_fg[clr]))
			strcpy(buf, td->strs.clr_fg[clr]);

		return (0);
	}

	clr = tcap_get_bg_num_by_name(name);
	if (clr != -1)
	{
		if (is_cmd_pres(td->strs.clr_bg[clr]))
			strcpy(buf, td->strs.clr_bg[clr]);

		return (0);
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * tcap_get_cmdstr_by_str() - get cmd string associated with a string
 *
 * returns:
 *		-1	error
 *		 0	got parameter string in buf
 *		 1	got parameter values in p1 & p2
 */
int tcap_get_cmdstr_by_str (const TERMINAL *tp, const char *str,
	SCRN_CMD *pcmd, char *buf, int *pp1, int *pp2)
{
	const TC_DATA *	tc;
	SCRN_CMD		cmd;
	char			name[128];
	char			data[128];
	char *			n;
	const char **	vp;
	int				p1;
	int				p2;
	int				rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (tp == 0 || str == 0 || pcmd == 0 || buf == 0 || pp1 == 0 || pp2 == 0)
	{
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * clear args
	 */
	*pcmd	= S_ZERO;
	*buf	= 0;
	*pp1	= 0;
	*pp2	= 0;

	p1		= 0;
	p2		= 0;

	/*--------------------------------------------------------------------
	 * collect name part
	 */
	strcpy(name, str);
	n = strchr(name, ':');
	if (n != 0)
		*n++ = 0;
	else
		n = (char *)"";

	/*--------------------------------------------------------------------
	 * lookup name
	 */
	if (tcap_trmcmp(name, "fg") == 0)
	{
		cmd = S_FG;
	}
	else if (tcap_trmcmp(name, "bg") == 0)
	{
		cmd = S_BG;
	}
	else
	{
		tc = tcap_tc_find_entry(name);
		if (tc == 0)
			return (-1);
		cmd	= tc->trmc_cmd;
	}

	/*--------------------------------------------------------------------
	 * decode entry
	 */
	switch (cmd)
	{
	case S_CM:
		p1 = atoi(n);
		n = strchr(n, ',');
		if (n != 0)
			p2 = atoi(n+1);
		rc = 1;
		break;

	case S_CS:
		strcpy(data, n);
		n = strchr(data, '/');
		if (n == 0)
			n = (char *)"";
		else
			*n++ = 0;
		p2 = tcap_get_fg_num_by_name(data);
		p1 = tcap_get_bg_num_by_name(n);
		rc = 1;
		break;

	case S_FG:
		p1 = tcap_get_fg_num_by_name(n);
		cmd = S_FG;
		rc = 1;
		break;

	case S_BG:
		p1 = tcap_get_bg_num_by_name(n);
		cmd = S_BG;
		rc = 1;
		break;

	default:
		vp = TCAP_TC_STR_GP(tp->tcap, tc);
		if (is_cmd_pres(*vp))
			strcpy(buf, *vp);
		rc = 0;
		break;
	}

	/*--------------------------------------------------------------------
	 * store results
	 */
	*pcmd	= cmd;
	*pp1	= p1;
	*pp2	= p2;

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_get_default_attrs() - get default attributes
 */
int tcap_get_default_attrs (const TERMINAL *tp, attr_t *pr, attr_t *ps,
	int use_color, int use_win)
{
	attr_t orig;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0 || pr == 0 || ps == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * figure out attributes
	 */
	orig = tcap_get_orig_attr(tp);

	if (use_color && tcap_has_colors(tp))
	{

		if (orig != A_UNSET)
		{
			int f = A_FG_CLRNUM(orig);
			int b = A_BG_CLRNUM(orig);

			*pr	= orig;
			*ps	= A_CLR(b, f);
		}
		else
		{
			/*----------------------------------------------------
			 * if in a window, set to black-on-ltwhite
			 * if not,         set to white-on-black
			 */
			if (use_win && tcap_get_disp_in_win(tp))
			{
				*pr	= A_CLR(COLOR_BLACK,   COLOR_LTWHITE);
				*ps	= A_CLR(COLOR_LTWHITE, COLOR_BLACK);
			}
			else
			{
				*pr	= A_CLR(COLOR_LTWHITE, COLOR_BLACK);
				*ps	= A_CLR(COLOR_BLACK,   COLOR_LTWHITE);
			}
		}
	}
	else
	{
		*pr	= A_NORMAL;
		*ps	= A_REVERSE;
	}

	return (0);
}
