/*------------------------------------------------------------------------
 * termcap/terminfo/Windows terminal database routines
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * table of tcap database names
 */
struct tcap_db
{
	int				type;		/* database type	*/
	const char *	name;		/* database name	*/
	const char *	desc;		/* description		*/
};
typedef struct tcap_db TCAP_DB;

static const TCAP_DB tcap_dbs[] =
{
	/*--------------------------------------------------------------------
	 * Common entries
	 */
	{ TCAP_DB_AUTO,		"auto",			"auto-detect  database"		},
	{ TCAP_DB_TERMFILES,"termfiles",	"use term-files only"		},

	/*--------------------------------------------------------------------
	 * Unix entries
	 */
#if V_UNIX
	{ TCAP_DB_TERMINFO,	"terminfo",		"use terminfo database"		},
	{ TCAP_DB_TERMCAP,	"termcap",		"use termcap  database"		},
#endif

	/*--------------------------------------------------------------------
	 * Windows entries
	 */
#if V_WINDOWS
	{ TCAP_DB_WINDOWS,	"windows",		"use windows console"		},
#endif

	{ -1, 0, 0 }
};

#define NUM_DBS		( (sizeof(tcap_dbs) / sizeof(*tcap_dbs)) - 1 )

/*------------------------------------------------------------------------
 * tcap_db_name_to_val() - convert a database name to a number
 */
int tcap_db_name_to_val (const char *name)
{
	const TCAP_DB *	t;

	if (! is_cmd_pres(name))
		return (-1);

	for (t = tcap_dbs; t->type >= 0; t++)
	{
		if (tcap_trmcmp(name, t->name) == 0)
			return (t->type);
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * tcap_db_val_to_name() - convert a database value to a name
 */
const char *tcap_db_val_to_name (int type)
{
	const TCAP_DB *	t;

	for (t = tcap_dbs; t->type >= 0; t++)
	{
		if (type == t->type)
			return (t->name);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_db_val_to_desc() - convert a database value to a description
 */
const char *tcap_db_val_to_desc (int type)
{
	const TCAP_DB *	t;

	for (t = tcap_dbs; t->type >= 0; t++)
	{
		if (type == t->type)
			return (t->desc);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_db_num_to_name() - convert a database number to a name
 */
const char *tcap_db_num_to_name (int num)
{
	const TCAP_DB *	t;

	if (num < 0 || num >= NUM_DBS)
		return (0);

	t = tcap_dbs + num;

	return (t->name);
}

/*------------------------------------------------------------------------
 * tcap_db_num_to_desc() - convert a database number to a description
 */
const char *tcap_db_num_to_desc (int num)
{
	const TCAP_DB *	t;

	if (num < 0 || num >= NUM_DBS)
		return (0);

	t = tcap_dbs + num;

	return (t->desc);
}

/*------------------------------------------------------------------------
 * tcap_db_set_type() - set terminal database type
 */
int tcap_db_set_type (int db_type)
{
	const TCAP_DB *	t;

	for (t = tcap_dbs; t->type >= 0; t++)
	{
		if (db_type == t->type)
		{
			tcap_db_type = db_type;
			return (0);
		}
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * tcap_db_get_type() - get terminal database type
 */
int tcap_db_get_type (void)
{
	return (tcap_db_type);
}

/*========================================================================
 * terminfo processing
 */

/*------------------------------------------------------------------------
 * tcap_ti_load_term() - fill tcap_data struct from database
 */
static int tcap_ti_load_term (TCAP_DATA *td, const char *term)
{
	const TC_DATA *	tc;
	const char *	s;
	int				i;
	int				c;
	int				rc;

	/*--------------------------------------------------------------------
	 * load terminfo database
	 */
	rc = tcap_tigetent(td->db_buf, term, 0);
	if (rc)
		return (-1);

	/*--------------------------------------------------------------------
	 * Go through the termcap data array
	 * and look up all entries that have an associated terminfo
	 * name (trmc_tiname) in the terminfo database.
	 */
	for (tc=tcap_termcap_data; tc->trmc_label; tc++)
	{
		char **	cp;
		int *	ip;
		int *	bp;

		if (tc->trmc_tiname)
		{
			switch (tc->trmc_flags & TC_TYPE)
			{
			case TC_NUM:
				c = tcap_tigetnum(td->db_buf, tc->trmc_tiname);
				if (c >= 0)
				{
					ip = TCAP_TC_NUM_SP(td, tc);
					*ip = c;
				}
				break;

			case TC_BLN:
				c = tcap_tigetflag(td->db_buf, tc->trmc_tiname);
				if (c >= 0)
				{
					bp = TCAP_TC_BLN_SP(td, tc);
					*bp = c;
				}
				break;

			case TC_STR:
				s = tcap_tigetstr(td->db_buf, tc->trmc_tiname);
				if (is_cmd_pres(s))
				{
					cp = TCAP_TC_STR_SP(td, tc);
					if (is_cmd_pres(*cp))
						FREE(*cp);
					*cp = tcap_trm_dup_str(s);
				}
				break;
			}
		}
	}

	/*--------------------------------------------------------------------
	 *	Go through the termcap keys array, and add
	 *	any entries we find in the termcap database.
	 */
	for (i=0; tcap_tc_keys[i].kf_tiname; i++)
	{
		s = tcap_tigetstr(td->db_buf, tcap_tc_keys[i].kf_tiname);
		if (is_cmd_pres(s))
			tcap_kf_addkey(td, s, tcap_tc_keys[i].kf_code);
	}

	/*--------------------------------------------------------------------
	 *	Set database type
	 */
	td->db_type = TCAP_DB_TERMINFO;

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_ti_dump_term() - dump the terminfo buffer
 */
static int tcap_ti_dump_term (const char *term, FILE *fp)
{
	char	buf[TCAP_BUFLEN];
	int		rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (term == 0 || *term == 0 || fp == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * load the buffer
	 */
	rc = tcap_tigetent(buf, term, 0);
	if (rc)
		return (-1);

	/*--------------------------------------------------------------------
	 * dump the buffer
	 */
	tcap_tidump(buf, term, fp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_ti_calc() - evaluate a parameterized string
 */
static char *tcap_ti_calc (char *buf, const char *s, int p1, int p2)
{
	return tcap_db_goto(TCAP_DB_TERMINFO, buf, s, p1, p2);
}

#if 0
/*------------------------------------------------------------------------
 * tcap_ti_outs() - output an evaluated parameterized string
 */
static int tcap_ti_outs (TERMINAL *tp, const char *str,
	int (*outrtn)(TERMINAL *tp, int c))
{
	int rc;

	if (tp == 0 || outrtn == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = tcap_tiputs(tp, str, outrtn);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}
#endif

/*========================================================================
 * termcap processing
 */

/*------------------------------------------------------------------------
 * tcap_tc_load_term() - fill tcap_data struct from database
 */
static int tcap_tc_load_term (TCAP_DATA *td, const char *term)
{
	const TC_DATA *	tc;
	const char *	s;
	int				i;
	int				c;
	int				rc;

	/*--------------------------------------------------------------------
	 * load termcap entry into buffer
	 */
	rc = tcap_tcgetent(td->db_buf, term, 0);
	if (rc == -1)
		return (-1);

	/*--------------------------------------------------------------------
	 * Go through the termcap data array
	 * and look up all entries that have an associated termcap
	 * name (trmc_tcname) in the termcap database.
	 */
	for (tc=tcap_termcap_data; tc->trmc_label; tc++)
	{
		char **	cp;
		int *	ip;
		int *	bp;

		if (tc->trmc_tcname)
		{
			switch (tc->trmc_flags & TC_TYPE)
			{
			case TC_NUM:
				c = tcap_tcgetnum(td->db_buf, tc->trmc_tcname);
				if (c >= 0)
				{
					ip = TCAP_TC_NUM_SP(td, tc);
					*ip = c;
				}
				break;

			case TC_BLN:
				c = tcap_tcgetflag(td->db_buf, tc->trmc_tcname);
				if (c >= 0)
				{
					bp = TCAP_TC_BLN_SP(td, tc);
					*bp = c;
				}
				break;

			case TC_STR:
				s = tcap_tcgetstr(td->db_buf, tc->trmc_tcname);
				if (is_cmd_pres(s))
				{
					cp = TCAP_TC_STR_SP(td, tc);
					if (is_cmd_pres(*cp))
						FREE(*cp);
					*cp = tcap_trm_dup_str(s);
				}
				break;
			}
		}
	}

	/*--------------------------------------------------------------------
	 *	Go through the termcap keys array, and add
	 *	any entries we find in the termcap database.
	 */
	for (i=0; tcap_tc_keys[i].kf_tcname; i++)
	{
		s = tcap_tcgetstr(td->db_buf, tcap_tc_keys[i].kf_tcname);
		if (is_cmd_pres(s))
			tcap_kf_addkey(td, s, tcap_tc_keys[i].kf_code);
	}

	/*--------------------------------------------------------------------
	 *	Set database type
	 */
	td->db_type = TCAP_DB_TERMCAP;

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_tc_dump_term() - dump the termcap buffer
 */
static int tcap_tc_dump_term (const char *term, FILE *fp)
{
	char	buf[TCAP_BUFLEN];
	int		rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (term == 0 || *term == 0 || fp == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * load the buffer
	 */
	rc = tcap_tcgetent(buf, term, 0);
	if (rc)
		return (-1);

	/*--------------------------------------------------------------------
	 * dump the buffer
	 */
	tcap_tcdump(buf, term, fp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_tc_calc() - evaluate a parameterized string
 */
static char *tcap_tc_calc (char *buf, const char *s, int p1, int p2)
{
	return tcap_db_goto(TCAP_DB_TERMCAP, buf, s, p1, p2);
}

#if 0
/*------------------------------------------------------------------------
 * tcap_tc_outs() - output an evaluated parameterized string
 */
static int tcap_tc_outs (TERMINAL *tp, const char *str,
	int (*outrtn)(TERMINAL *tp, int c))
{
	int rc;

	if (tp == 0 || outrtn == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = tcap_tcputs(tp, str, outrtn);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}
#endif

/*========================================================================
 * WINDOWS processing
 */

/*------------------------------------------------------------------------
 * tcap_nt_load_term() - fill tcap_data struct
 */
static int tcap_nt_load_term (TCAP_DATA *td, const char *term)
{
	if (term != 0 && strcmp(term, TCAP_TERM_WINDOWS) == 0)
	{
		/*----------------------------------------------------------------
		 * termcap values
		 */
		td->ints.maxcols	= 80;
		td->ints.maxrows	= 25;

		td->bools.ifc		= 0;

		/*----------------------------------------------------------------
		 * move cursor
		 */
		td->parms.cm		= tcap_trm_dup_str("");

		/*----------------------------------------------------------------
		 * beep
		 */
		td->strs.bl			= tcap_trm_dup_str("");

		/*----------------------------------------------------------------
		 * set cursor type
		 */
		td->strs.ve			= tcap_trm_dup_str("");
		td->strs.vi			= tcap_trm_dup_str("");
		td->strs.vs			= tcap_trm_dup_str("");

		/*----------------------------------------------------------------
		 * set window title
		 */
		td->strs.tts		= tcap_trm_dup_str("");
		td->strs.tte		= tcap_trm_dup_str("");

		/*----------------------------------------------------------------
		 * attributes
		 */
		td->strs.sts		= tcap_trm_dup_str("");
		td->strs.ste		= tcap_trm_dup_str("");

		/*----------------------------------------------------------------
		 * alt char sets
		 */
		td->strs.ans[0]		= tcap_trm_dup_str("");
		td->strs.ane[0]		= tcap_trm_dup_str("");

		td->strs.ans[1]		= tcap_trm_dup_str("");
		td->strs.ane[1]		= tcap_trm_dup_str("");

		/*----------------------------------------------------------------
		 * mouse
		 */
		td->strs.mbeg		= tcap_trm_dup_str("");
		td->strs.mend		= tcap_trm_dup_str("");
		td->strs.mstr		= tcap_trm_dup_str("");

		/*----------------------------------------------------------------
		 * colors
		 */
		td->parms.csm		= tcap_trm_dup_str("");

		/*----------------------------------------------------------------
		 *	Set database type
		 */
		td->db_type = TCAP_DB_WINDOWS;

		return (0);
	}
	else
	{
		return (-1);
	}
}

/*------------------------------------------------------------------------
 * tcap_nt_dump_term() - dump the termcap buffer
 */
static int tcap_nt_dump_term (const char *term, FILE *fp)
{
	return (-1);
}

/*------------------------------------------------------------------------
 * tcap_nt_calc() - evaluate a parameterized string
 */
static char *tcap_nt_calc (char *buf, const char *s, int p1, int p2)
{
	return ((char *)"");
}

#if 0
/*------------------------------------------------------------------------
 * tcap_nt_outs() - output an evaluated parameterized string
 */
static int tcap_nt_outs (TERMINAL *tp, const char *str,
	int (*outrtn)(TERMINAL *tp, int c))
{
	if (tp == 0 || outrtn == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		for (; *str; str++)
			(*outrtn)(tp, *str);
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}
#endif

/*========================================================================
 * actual routines
 */

/*------------------------------------------------------------------------
 * tcap_db_load_term() - fill tcap_data struct
 */
int tcap_db_load_term (int db_type, TERMINAL *tp, const char *term)
{
	TCAP_DATA *	td = tp->tcap;

	switch (db_type)
	{
	case TCAP_DB_AUTO:
#if V_UNIX
		if (tcap_ti_load_term(td, term) == 0)
			return (0);
		if (tcap_tc_load_term(td, term) == 0)
			return (0);
#endif
#if V_WINDOWS
		if (tcap_nt_load_term(td, term) == 0)
			return (0);
#endif
		return (-1);

	case TCAP_DB_TERMCAP:	return tcap_tc_load_term(td, term);
	case TCAP_DB_TERMINFO:	return tcap_ti_load_term(td, term);
	case TCAP_DB_WINDOWS:	return tcap_nt_load_term(td, term);
	case TCAP_DB_TERMFILES:	return (-1);

	default:				return (-1);
	}
}

/*------------------------------------------------------------------------
 * tcap_db_dump_term() - dump the database
 */
int tcap_db_dump_term (int db_type, const char *term, FILE *fp)
{
	switch (db_type)
	{
	case TCAP_DB_AUTO:
#if V_UNIX
		if (tcap_ti_dump_term(term, fp) == 0)
			return (0);
		if (tcap_tc_dump_term(term, fp) == 0)
			return (0);
#endif
#if V_WINDOWS
		if (tcap_nt_dump_term(term, fp) == 0)
			return (0);
#endif
		return (-1);

	case TCAP_DB_TERMCAP:	return tcap_tc_dump_term(term, fp);
	case TCAP_DB_TERMINFO:	return tcap_ti_dump_term(term, fp);
	case TCAP_DB_WINDOWS:	return tcap_nt_dump_term(term, fp);
	case TCAP_DB_TERMFILES:	return (-1);

	default:				return (-1);
	}
}

/*------------------------------------------------------------------------
 * tcap_db_calc() - evaluate a parameterized string
 */
char *tcap_db_calc (int db_type, char *buf, const char *s, int p1, int p2)
{
	switch (db_type)
	{
	case TCAP_DB_TERMCAP:	return tcap_tc_calc(buf, s, p1, p2);
	case TCAP_DB_TERMINFO:	return tcap_ti_calc(buf, s, p1, p2);
	case TCAP_DB_WINDOWS:	return tcap_nt_calc(buf, s, p1, p2);
	default:				return ((char *)"");
	}
}

/*------------------------------------------------------------------------
 * tcap_db_outs() - output an evaluated parameterized string
 *
 * This routine outputs a string which *may* contain padding information.
 * The syntax of padding info differs between termcap & terminfo, so
 * we deal with both types here (fortunately, the two are compatible).
 *
 * The syntaxes used are:
 *
 *	termcap		Padding is specified in milliseconds as a numeric value
 *				at the start of a string.  It may be followed by a '*'
 *				to indicate that the padding is relative to the number
 *				of lines, but we ignore it.  The padding delay is output
 *				*after* the string is sent.
 *
 *	terminfo	Padding is specified in milliseconds as the sequence
 *				$<nnn[/][*]>.  It may appear anywhere in the string to
 *				indicate a padding delay to be output at that place in
 *				the string.  The number may be followed by a '*' to
 *				indicate that the padding is relative to the number of
 *				lines, and/or by a '/' to indicate that the padding is
 *				mandatory.  We ignore both indicators and just output
 *				the padding at that place in the string.
 */
int tcap_db_outs (TERMINAL *tp, const char *str,
	int (*outrtn)(TERMINAL *tp, int c))
{
#if 0
	switch (tp->tcap->db_type)
	{
	case TCAP_DB_TERMCAP:	return tcap_tc_outs(tp, str, outrtn);
	case TCAP_DB_TERMINFO:	return tcap_ti_outs(tp, str, outrtn);
	case TCAP_DB_WINDOWS:	return tcap_nt_outs(tp, str, outrtn);
	default:				return (-1);
	}
#else
	const unsigned char *	s;
	int						init_pad = 0;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (outrtn == 0)
		return (-1);

	if (! is_cmd_pres(str) || *str == 0)
		return (0);

	s = (const unsigned char *)str;

	/*--------------------------------------------------------------------
	 * collect any initial padding (termcap syntax)
	 */
	if (isdigit(*s))
	{
		for (; *s; s++)
		{
			if (! isdigit(*s))
				break;

			init_pad = (init_pad * 10) + (*s - '0');
		}

		if (*s == '*')
			s++;
	}

	/*--------------------------------------------------------------------
	 * output the string
	 */
	TERMINAL_LOCK(tp);
	{
		/*----------------------------------------------------------------
		 * output the rest of the string
		 */
		for (; *s; s++)
		{
			/*------------------------------------------------------------
			 * check for padding (terminfo syntax)
			 */
			if (s[0] == '$' && s[1] == '<')
			{
				int	pad;

				/*--------------------------------------------------------
				 * get pad value
				 */
				pad = 0;
				for (s += 2; *s; s++)
				{
					if (! isdigit(*s))
						break;

					pad = (pad * 10) + (*s - '0');
				}

				for (; *s; s++)
				{
					if (*s == '>')
						break;
				}

				/*--------------------------------------------------------
				 * output the padding
				 */
				tcap_delay_output(tp, pad, outrtn);

				continue;
			}

			/*------------------------------------------------------------
			 * output the character
			 */
			(*outrtn)(tp, *s);
		}

		/*----------------------------------------------------------------
		 * now output any initial padding found (termcap syntax)
		 */
		if (init_pad > 0)
		{
			tcap_delay_output(tp, init_pad, outrtn);
		}
	}
	TERMINAL_UNLOCK(tp);

	return (0);
#endif
}

/*========================================================================
 * low-level routines
 */

/*------------------------------------------------------------------------
 * tcap_db_bufptr() - get pointer to database buffer
 */
char * tcap_db_bufptr (TERMINAL *tp)
{
	if (tp == 0)
		return (0);

	return (tp->tcap->db_buf);
}

/*------------------------------------------------------------------------
 * tcap_db_dbtype() - get database type of buffer
 */
int tcap_db_dbtype (TERMINAL *tp)
{
	if (tp == 0)
		return (-1);

	return (tp->tcap->db_type);
}

/*------------------------------------------------------------------------
 * tcap_db_getent() - get database entry
 */
int tcap_db_getent (int db_type, char *bp, const char *term)
{
	switch (db_type)
	{
	case TCAP_DB_TERMCAP:	return tcap_tcgetent(bp, term, 0);
	case TCAP_DB_TERMINFO:	return tcap_tigetent(bp, term, 0);
	case TCAP_DB_WINDOWS:	return (-1);
	default:				return (-1);
	}
}

/*------------------------------------------------------------------------
 * tcap_db_getflag() - get boolean from a database
 */
int tcap_db_getflag (int db_type, const char *bp, const char *id)
{
	switch (db_type)
	{
	case TCAP_DB_TERMCAP:	return tcap_tcgetflag(bp, id);
	case TCAP_DB_TERMINFO:	return tcap_tigetflag(bp, id);
	case TCAP_DB_WINDOWS:	return (-1);
	default:				return (-1);
	}
}

/*------------------------------------------------------------------------
 * tcap_db_getnum() - get number from a database
 */
int tcap_db_getnum (int db_type, const char *bp, const char *id)
{
	switch (db_type)
	{
	case TCAP_DB_TERMCAP:	return tcap_tcgetnum(bp, id);
	case TCAP_DB_TERMINFO:	return tcap_tigetnum(bp, id);
	case TCAP_DB_WINDOWS:	return (-1);
	default:				return (-1);
	}
}

/*------------------------------------------------------------------------
 * tcap_db_getstr() - get string from a database
 */
const char * tcap_db_getstr (int db_type, const char *bp, const char *id)
{
	switch (db_type)
	{
	case TCAP_DB_TERMCAP:	return tcap_tcgetstr(bp, id);
	case TCAP_DB_TERMINFO:	return tcap_tigetstr(bp, id);
	case TCAP_DB_WINDOWS:	return ("");
	default:				return ("");
	}
}

/*------------------------------------------------------------------------
 * tcap_db_goto() - get parameterized string
 */
char * tcap_db_goto (int db_type, char *buf, const char *str, int p1, int p2)
{
	int p[9];
	int i;

	for (i=0; i<9; i++)
		p[i] = 0;

	switch (db_type)
	{
	case TCAP_DB_TERMCAP:
		p[0] = p1;
		p[1] = p2;
		return tcap_tcgoto(buf, str, p);

	case TCAP_DB_TERMINFO:
		p[0] = p1;
		p[1] = p2;
		return tcap_tigoto(buf, str, p);

	case TCAP_DB_WINDOWS:
		return ((char *)"");

	default:
		return ((char *)"");
	}
}

/*------------------------------------------------------------------------
 * tcap_db_dumpent() - dump a database buffer
 */
void tcap_db_dump (int db_type, const char *bp, const char *term, FILE *fp)
{
	switch (db_type)
	{
	case TCAP_DB_TERMCAP:	tcap_tcdump(bp, term, fp);	break;
	case TCAP_DB_TERMINFO:	tcap_tidump(bp, term, fp);	break;
	case TCAP_DB_WINDOWS:								break;
	default:											break;
	}
}
