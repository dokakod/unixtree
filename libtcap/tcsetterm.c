/*------------------------------------------------------------------------
 * TERMINAL setup routines
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * tcap_process_acs_string() - process an ACS string
 */
static void tcap_process_acs_str (TCAP_DATA *td, const char *str)
{
	const unsigned char *	s = (const unsigned char *)str;

	/*--------------------------------------------------------------------
	 * step through string 2 chars at a time
	 */
	for (; s[0] && s[1]; s+=2)
	{
		const ACS_CHRS *a;

		for (a=tcap_acs_chars; a->vt_char; a++)
		{
			if (a->vt_char == s[0])
				td->alt_tbl[a->pc_char] = s[1];
		}
	}
}

/*------------------------------------------------------------------------
 * tcap_chk_parms() - check all parameterized entries
 *
 * This routine should be table-driven, but right now we'll grunt it out.
 */
static int tcap_chk_parms (TERMINAL *tp)
{
	TCAP_DATA *			td	= tp->tcap;
	const PARAM_INFO *	pi;
	PARM_DATA *			pd	= &td->pdata;
	int					rc	= 0;

	/*--------------------------------------------------------------------
	 * all entries in param-info table
	 */
	for (pi=tcap_param_info; pi->cmd > 0; pi++)
	{
		const TC_DATA *	tc	= tcap_tc_find_cmd(pi->cmd);

		/*----------------------------------------------------------------
		 * lookup tcap-data entry for this cmd
		 */
		if (tc != 0)
		{
			char **	cp		= TCAP_TC_STR_SP(td, tc);
			int		bogus	= FALSE;

			/*------------------------------------------------------------
			 * if cmd is present, check if db-type must be defined for it
			 */
			if (is_cmd_pres(*cp))
			{
				if (pi->need && td->db_type <= 0)
				{
					tcap_errmsg_add_fmt(tp->tcap->errs,
						TCAP_ERR_NODBS, TCAP_ERR_L_FATAL,
						"database type not specified for %s",
						tc->trmc_label);
					bogus = TRUE;
					rc = -1;
				}

				/*--------------------------------------------------------
				 * set info into paramater if valid
				 */
				if (! bogus)
				{
					PARM_ENTRY *	pe = (PARM_ENTRY *)((char *)pd + pi->offs);

					pe->cmd		= pi->cmd;
					pe->use		= pi->use;
					pe->data	= pi->data;
					pe->str		= *cp;
					pe->type	= pi->type == 0 ?
									(pi->need ? td->db_type : 0) :
									(pi->type);
				}
			}
		}
	}

	/*--------------------------------------------------------------------
	 * error if no CM was specified
	 */
	if (! is_cmd_pres(pd->cm.str))
	{
		tcap_errmsg_add_fmt(tp->tcap->errs,
			TCAP_ERR_NOCM, TCAP_ERR_L_FATAL,
			"CM string not defined");
		rc = -1;
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_chk_color() - internal rtn to check to see if we have color capability
 *
 * We simply look to see if we have color setting strings defined.
 * We also do color string setup here.
 */
static int tcap_chk_color (TERMINAL *tp)
{
	TCAP_DATA *	td			= tp->tcap;
	int			is_color	= FALSE;
	int			i;
	int			have_cs		= FALSE;

	/*--------------------------------------------------------------------
	 * set max-colors if not set
	 */
	if (td->ints.mc == 0)
	{
		td->ints.mc = NUM_COLORS;
	}

	/*--------------------------------------------------------------------
	 * If we found "fg" definitions, expand them
	 */
	if (is_cmd_pres(td->pdata.fg.str))
	{
		/*----------------------------------------------------------------
		 * set max-fg if not set
		 */
		if (td->ints.max_fg == 0)
		{
			td->ints.max_fg = NUM_COLORS;
			if (td->ints.max_fg > td->ints.mc)
				td->ints.max_fg = td->ints.mc;
		}
		else
		{
			if (td->ints.max_fg < td->ints.mc)
				td->ints.max_fg = td->ints.mc;
		}

		/*----------------------------------------------------------------
		 * expand any empty fg entries
		 */
		for (i=0; i<td->ints.max_fg; i++)
		{
			if (! is_cmd_pres(td->strs.clr_fg[i]) )
			{
				char buf[128];
				char *	s;

				s = tcap_eval_parm(tp, buf, S_FG, i, 0);
				td->strs.clr_fg[i] = tcap_trm_dup_str(s);
			}
		}
	}

	/*--------------------------------------------------------------------
	 * If we found "bg" definitions, expand them
	 */
	if (is_cmd_pres(td->pdata.bg.str))
	{
		/*----------------------------------------------------------------
		 * set max-bg if not set
		 */
		if (td->ints.max_bg == 0)
		{
			td->ints.max_bg = NUM_COLORS;
			if (td->ints.max_bg > td->ints.mc)
				td->ints.max_bg = td->ints.mc;
		}
		else
		{
			if (td->ints.max_bg < td->ints.mc)
				td->ints.max_bg = td->ints.mc;
		}

		/*----------------------------------------------------------------
		 * expand any empty bg entries
		 */
		for (i=0; i<td->ints.max_bg; i++)
		{
			if (! is_cmd_pres(td->strs.clr_bg[i]) )
			{
				char buf[128];
				char *	s;

				s = tcap_eval_parm(tp, buf, S_BG, i, 0);
				td->strs.clr_bg[i] = tcap_trm_dup_str(s);
			}
		}
	}

	/*--------------------------------------------------------------------
	 * If we found a "cs" definition, set num colors found (fg & bg)
	 */
	if (is_cmd_pres(td->pdata.cs.str))
	{
		td->ints.max_fg	= td->ints.mc;
		td->ints.max_bg	= td->ints.mc;

		have_cs = TRUE;
	}

	/*--------------------------------------------------------------------
	 * if we have colors & NOT a CS string, check if all colors are there
	 */
	if (! have_cs && td->ints.max_fg > 0)
	{
		for (i=0; i<td->ints.max_fg; i++)
		{
			if (! is_cmd_pres(td->strs.clr_fg[i]))
			{
				tcap_errmsg_add_fmt(tp->tcap->errs,
					TCAP_ERR_NOFG, TCAP_ERR_L_FATAL,
					"Missing fg color definition for \"%s\"",
					tcap_get_fg_name_by_num(i));
				return (-1);
			}
		}

		for (i=0; i<td->ints.max_bg; i++)
		{
			if (! is_cmd_pres(td->strs.clr_bg[i]))
			{
				tcap_errmsg_add_fmt(tp->tcap->errs,
					TCAP_ERR_NOBG, TCAP_ERR_L_FATAL,
					"Missing bg color definition for \"%s\"",
					tcap_get_bg_name_by_num(i));
				return (-1);
			}
		}
	}

	/*--------------------------------------------------------------------
	 * check if user specified no color allowed
	 *
	 * We do this check after setting up all color strings, since the
	 * user can change this at run time.
	 */
	if (! td->bools.alc)
		return (FALSE);

	/*--------------------------------------------------------------------
	 * check if any fg color settings defined
	 */
	is_color = (td->ints.max_fg > 0);

	/*--------------------------------------------------------------------
	 * now check if we have fg colors but no bg colors
	 */
	if (is_color && td->ints.max_bg == 0)
	{
		/*----------------------------------------------------------------
		 * set a dummy empty bg entry
		 */
		td->strs.clr_bg[0]  = tcap_trm_dup_str("");
		td->ints.max_bg = 1;
	}

	return (is_color);
}

/*------------------------------------------------------------------------
 * tcap_load_term() - internal routine to load term files
 */
static int tcap_load_term (TERMINAL *tp, const char *term)
{
	TCAP_DATA *		td		= tp->tcap;
	const char *	path	= tcap_get_term_path();
	char			trc_path[MAX_PATHLEN];
	char			trm_path[MAX_PATHLEN];
	char			trc_file[MAX_FILELEN];
	char			trm_file[MAX_FILELEN];
	char			term1[64];
	char			term2[64];
	char *			t;
	int				r1;
	int				r2;

	/*--------------------------------------------------------------------
	 * check for "term" in form "term:alt-term"
	 */
	strcpy(term1, term);
	*term2 = 0;

	t = strchr(term1, ':');
	if (t != 0)
	{
		*t++ = 0;
		strcpy(term2, t);
	}

	term = term1;

	/*--------------------------------------------------------------------
	 * check path
	 */
	if (path == 0 || *path == 0)
	{
		path = getenv("TERMPATH");
	}

	/*--------------------------------------------------------------------
	 * check if any term files exist
	 */
	strcpy(trc_file, term);
	strcat(trc_file, ".trc");

	strcpy(trm_file, term);
	strcat(trm_file, ".trm");

	r1 = tcap_find_path(trc_file, path, trc_path);
	r2 = tcap_find_path(trm_file, path, trm_path);

	if (r1 == 0)
	{
		/*--------------------------------------------------------
		 * found compiled term file (always takes precedence)
		 */
		r1 = tcap_get_trc(tp, trc_path);
		if (r1 == -1)
			return (-1);
	}
	else
	{
		/*--------------------------------------------------------
		 * no compiled term file found - load database defaults &
		 * any term-file(s) found.
		 */

		/*--------------------------------------------------------
		 * try to load term database
		 */
		r1 = tcap_db_load_term(tcap_db_get_type(), tp, term);

		/*--------------------------------------------------------
		 * try to load term-file(s)
		 */
		if (r2 == 0 || td->defs != 0)
		{
			int	r3 = 0;

			r2 = 0;
			r3 = tcap_process_trm_file(tp, term, path, FALSE, 0);
			if (r3 == 0)
			{
				if (tcap_process_trm_file(tp, "user", path, FALSE, 0) > 0)
					r3 = -1;
			}

			/*----------------------------------------------------
			 * if errors in term-file, bail
			 */
			if (r3 != 0)
			{
				return (-1);
			}
		}

		/*--------------------------------------------------------
		 * If no term db & no term file are found, try the alternate
		 * term if present.
		 */
		if (r1 == -1 && r2 == -1)
		{
			if (*term2 != 0)
				r1 = tcap_load_term(tp, term2);

			if (r1 != 0)
			{
				tcap_errmsg_add_fmt(tp->tcap->errs,
					TCAP_ERR_NOTFND, TCAP_ERR_L_FATAL,
					"No term file or database entry found for term \"%s\"",
					term);
				return (-1);
			}
		}
	}

	/*--------------------------------------------------------------------
	 * process any parameterized entries
	 */
	r1 = tcap_chk_parms(tp);
	if (r1 != 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * if ttytype not set, set to "term"
	 */
	if (td->tty_type == 0)
	{
		td->tty_type = tcap_trm_dup_str(term);
	}

	/*--------------------------------------------------------------------
	 * add default dumb keys
	 */
	r1 = tcap_fk_add_dumb_keys(td);
	if (r1 == -1)
		return (-1);

	/*--------------------------------------------------------------------
	 * setup function keys
	 */
	r1 = tcap_fk_setup(td);
	if (r1 == -1)
		return (-1);

	/*--------------------------------------------------------------------
	 * setup prefix interval
	 */
	tcap_kbd_set_timeout(tp, TRUE);

	/*--------------------------------------------------------------------
	 * Check if color or mono
	 */
	r1 = tcap_chk_color(tp);
	if (r1 == -1)
		return (-1);
	tp->scrn->color_flag = r1;

	/*--------------------------------------------------------------------
	 * look for acs entry & process if found
	 */
	if (is_cmd_pres(td->strs.acs))
	{
		tcap_process_acs_str(td, td->strs.acs);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_setup_term() - load all database/term-file info into a TERMINAL
 */
static int tcap_setup_term (TERMINAL *tp, const char *term)
{
	TCAP_DATA *		td	= tp->tcap;
	int				rc	= 0;
	int				rows;
	int				cols;

	/*--------------------------------------------------------------------
	 * load any database and/or trmfile entries
	 */
	if (tcap_load_term(tp, term))
	{
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * check window size
	 */
	if (term_window_size_get(tp->term, &rows, &cols) == 0)
	{
		td->ints.maxrows = rows;
		td->ints.maxcols = cols;
	}

	/*--------------------------------------------------------------------
	 * override window size if requested
	 */
	if (tcap_get_use_env())
	{
		char *	p;
		int		n;

		p = getenv("LINES");
		if (p)
		{
			n = atoi(p);
			if (n > 0)
				td->ints.maxrows = n;
		}

		p = getenv("COLUMNS");
		if (p)
		{
			n = atoi(p);
			if (n > 0)
				td->ints.maxcols = n;
		}

		p = getenv("TABSIZE");
		if (p)
		{
			n = atoi(p);
			if (n > 0)
				td->ints.tabsize = n;
		}
	}

	/*--------------------------------------------------------------------
	 * check if window size not defined
	 */
	if (td->ints.maxrows <= 0)
	{
		tcap_errmsg_add_fmt(tp->tcap->errs,
			TCAP_ERR_NOLINES, TCAP_ERR_L_FATAL,
			"LINES not defined for term \"%s\"", term);
		rc = -1;
	}

	if (td->ints.maxcols <= 0)
	{
		tcap_errmsg_add_fmt(tp->tcap->errs,
			TCAP_ERR_NOCOLS, TCAP_ERR_L_FATAL,
			"COLS not defined for term \"%s\"", term);
		rc = -1;
	}

	/*--------------------------------------------------------------------
	 * use default tab-size if not specified
	 */
	if (td->ints.tabsize <= 0)
	{
		td->ints.tabsize = 8;
	}

	/*--------------------------------------------------------------------
	 * get original screen attributes if possible
	 */
	tp->scrn->orig_attr = tcap_get_orig_attr(tp);

	/*--------------------------------------------------------------------
	 * check if in window
	 */
	tp->scrn->disp_in_window = term_window_check();

	/*--------------------------------------------------------------------
	 * store mouse stuff
	 */
	{
		MOUSE_STATUS *	mp	= term_screen_dev_mouse(tp->term);

		mp->max_x	= td->ints.maxcols;
		mp->max_y	= td->ints.maxrows;
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_newterm() - create a new TERMINAL
 */
TERMINAL *tcap_newterm (const char *term, FILE *out_fp, FILE *inp_fp,
	TCAP_ERRS *ta)
{
	TERMINAL *	tp;
	int			inp_fd	= -1;
	int			out_fd = -1;

	/*--------------------------------------------------------------------
	 * get file descriptors
	 */
	if (inp_fp != 0)
		inp_fd = fileno(inp_fp);

	if (out_fp != 0)
		out_fd = fileno(out_fp);

	/*--------------------------------------------------------------------
	 * now do it
	 */
	tp = tcap_newscreen(term, 0, out_fd, inp_fd, ta);

	return (tp);
}

/*------------------------------------------------------------------------
 * tcap_newscreen() - create a new TERMINAL
 */
TERMINAL *tcap_newscreen (const char *term, const char **defs,
	int out_fd, int inp_fd,
	TCAP_ERRS *ta)
{
	TCAP_ERRS	errs;
	TCAP_ERRS *	te;
	TERMINAL *	tp;
	int			rc;

	/*--------------------------------------------------------------------
	 * initialize err msg struct
	 */
	te = (ta != 0 ? ta : &errs);
	tcap_errmsg_init(te);

	/*--------------------------------------------------------------------
	 * get value of TERM
	 */
	if (term == 0 || *term == 0)
		term = getenv("TERM");

	if (term == 0 || *term == 0)
	{
		if (tcap_db_get_type() == TCAP_DB_WINDOWS)
		{
			term = TCAP_TERM_WINDOWS;
		}
		else
		{
			tcap_errmsg_add_fmt(ta, TCAP_ERR_NOTERM, TCAP_ERR_L_FATAL,
				"No TERM variable specified");
			return (0);
		}
	}

	/*--------------------------------------------------------------------
	 * create TERMINAL struct
	 */
	tp = tcap_init_terminal();
	if (tp == 0)
	{
		tcap_errmsg_add_fmt(ta, TCAP_ERR_NOMEM, TCAP_ERR_L_FATAL,
			"Cannot allocate terminal struct");
		return (0);
	}

	tp->tcap->errs = te;
	tp->tcap->defs = defs;

	/*--------------------------------------------------------------------
	 * open TERMINAL devices
	 */
	rc = term_screen_dev_open(tp->term, inp_fd, out_fd);
	if (rc)
	{
		tcap_errmsg_add_fmt(tp->tcap->errs,
			TCAP_ERR_CANTOPEN, TCAP_ERR_L_FATAL,
			"Cannot open terminal");
		rc = -1;
	}

	/*--------------------------------------------------------------------
	 * now fill it in if no errors so far
	 */
	if (rc == 0)
		rc = tcap_setup_term(tp, term);

	/*--------------------------------------------------------------------
	 * if any errors, free it back up
	 */
	if (rc)
	{
		/*----------------------------------------------------------------
		 * now delete the screen
		 */
		tcap_delterminal(tp);
		tp = 0;
	}

	/*--------------------------------------------------------------------
	 * If the user didn't provide an error msg struct, we have to
	 * dump any msgs in our private struct & clear it.
	 *
	 * Note that, if we don't clear our private struct if it was
	 * used, we will have a memory leak.
	 */
	if (ta == 0)
	{
		tcap_errmsg_print_list(te, stderr, FALSE);
		tcap_errmsg_clear(te);
	}

	/*--------------------------------------------------------------------
	 * Now clear the struct pointer, since it either points to our
	 * automatic data or to the user's struct, which we know nothing
	 * about.
	 *
	 * Note that, just in case we try to output more msgs, this
	 * pointer being NULL will just result in the msg being printed.
	 */
	if (tp != 0)
		tp->tcap->errs = 0;

	return (tp);
}

/*------------------------------------------------------------------------
 * tcap_delterminal() - delete a TERMINAL struct
 */
void tcap_delterminal (TERMINAL *tp)
{
	if (tp != 0)
	{
		tcap_cexit(tp, TRUE);
		tcap_free_terminal(tp);
	}
}
