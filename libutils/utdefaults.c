/*------------------------------------------------------------------------
 *	default table processing
 */
#include "utcommon.h"

/*------------------------------------------------------------------------
 * check_yn() - check for 0/1, yes/no, on/off, true/false
 */
static int check_yn (const DEFT *tbl, int *val, const char *p)
{
	int		c;
	int		i;
	int		option = FALSE;
	int		got_it = FALSE;

	/*--------------------------------------------------------------------
	 * if digit, just treat non-zero/zero as TRUE/FALSE
	 */
	if (isdigit(*p))
	{
		c = atoi(p);
		if (c)
			option = TRUE;
		else
			option = FALSE;

		got_it = TRUE;
	}

	/*--------------------------------------------------------------------
	 * check all "yes" strings
	 */
	if (! got_it)
	{
		for (i=0; tbl->yes_tbl[i].f_str; i++)
		{
			if (strxcmp(p, (tbl->msg_rtn)(tbl->yes_tbl[i].f_str)) == 0 ||
			    strxcmp(p, (tbl->msg_rtn)(tbl->yes_tbl[i].m_str)) == 0)
			{
				option = TRUE;
				got_it = TRUE;
				break;
			}
		}
	}

	/*--------------------------------------------------------------------
	 * check all "no" strings
	 */
	if (! got_it)
	{
		for (i=0; tbl->nos_tbl[i].f_str; i++)
		{
			if (strxcmp(p, (tbl->msg_rtn)(tbl->nos_tbl[i].f_str)) == 0 ||
			    strxcmp(p, (tbl->msg_rtn)(tbl->nos_tbl[i].m_str)) == 0)
			{
				option = FALSE;
				got_it = TRUE;
				break;
			}
		}
	}

	/*--------------------------------------------------------------------
	 * store value if we got a valid entry
	 */
	if (got_it)
	{
		*val = option;
	}

	return (got_it ? 0 : -1);
}

/*------------------------------------------------------------------------
 * check_color() - check for a color name (fg[/bg])
 */
static int check_color (const DEFT *tbl, attr_t *val, const char *p)
{
	char			fgs[128];
	char			bgs[128];
	const char *	s;
	const char *	t;
	char *			t1;
	int				fgn;
	int				bgn;
	int				rc = 0;
	attr_t			option;
	int				i;

	/*--------------------------------------------------------------------
	 * split up string into fg & bg names
	 */
	s = strpbrk(p, "/,|;");
	if (s != 0)
	{
		t1 = fgs;
		for (t=p; t<s; t++)
			*t1++ = *t;
		*t1 = 0;
		strcpy(bgs, s+1);
	}
	else
	{
		strcpy(fgs, p);
		*bgs = 0;
	}

	/*--------------------------------------------------------------------
	 * process strings
	 */
	fgn = COLOR_BLACK;
	bgn = COLOR_BLACK;

	if (*fgs != 0)
	{
		if (isdigit(*fgs))
		{
			fgn = atoi(fgs);
		}
		else
		{
			int got_it	= FALSE;

			for (i=0; tbl->fg_names[i].f_str; i++)
			{
				if (strxcmp(fgs, (tbl->msg_rtn)(tbl->fg_names[i].f_str)) == 0 ||
			    	strxcmp(fgs, (tbl->msg_rtn)(tbl->fg_names[i].m_str)) == 0)
				{
					fgn = i;
					got_it = TRUE;
					break;
				}
			}

			if (! got_it)
				fgn = -1;
		}
	}

	if (*bgs != 0)
	{
		if (isdigit(*bgs))
		{
			bgn = atoi(bgs);
		}
		else
		{
			int got_it	= FALSE;

			for (i=0; tbl->bg_names[i].f_str; i++)
			{
				if (strxcmp(bgs, (tbl->msg_rtn)(tbl->bg_names[i].f_str)) == 0 ||
			    	strxcmp(bgs, (tbl->msg_rtn)(tbl->bg_names[i].m_str)) == 0)
				{
					bgn = i;
					got_it = TRUE;
					break;
				}
			}

			if (! got_it)
				bgn = -1;
		}
	}

	/*--------------------------------------------------------------------
	 * check if values valid
	 */
	if (fgn < 0 || fgn >= NUM_COLORS || bgn < 0 || bgn >= NUM_COLORS)
	{
		rc = -1;
	}

	/*--------------------------------------------------------------------
	 * store value if valid
	 */
	if (rc == 0)
	{
		option = A_FG_CLRVAL(get_fg_value_by_num(fgn)) |
				 A_BG_CLRVAL(get_bg_value_by_num(bgn));

		*val = option;
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * check_mono() - check for mono attribute string
 */
static int check_mono (const DEFT *tbl, attr_t *val, const char *p)
{
	int	rc = 0;
	int	option;
	int i;

	/*--------------------------------------------------------------------
	 * check for a number or name
	 */
	if (isdigit(*p))
	{
		option = atoi(p);
	}
	else
	{
		int got_it	= FALSE;

		for (i=0; tbl->attr_names[i].f_str; i++)
		{
			if (strxcmp(p, (tbl->msg_rtn)(tbl->attr_names[i].f_str)) == 0 ||
			    strxcmp(p, (tbl->msg_rtn)(tbl->attr_names[i].m_str)) == 0)
			{
				option = i;
				got_it = TRUE;
				break;
			}
		}

		if (! got_it)
			option = -1;
	}

	if (option < 0 || option >= NUM_MONO_ATTRS)
		rc = -1;

	/*--------------------------------------------------------------------
	 * store value if valid
	 */
	if (rc == 0)
	{
		*val = get_attr_value_by_num(option);
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * dflt_read() - read in a config file & process it
 */
int dflt_read (const DEFT *tbl, const char *filename, const char *syspath)
{
	FILE *			fp;
	char			pathname[MAX_PATHLEN];
	char			line[BUFSIZ];
	const DEFS *	ds;
	char *			s;
	char *			l;
	char *			p;
	int				len;
	int				rc;

	/*--------------------------------------------------------------------
	 * get pathname of file & open it
	 */
	p = os_get_path(filename, syspath, pathname);
	if (p == 0)
		return (-1);

	fp = fopen(p, "r");
	if (fp == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * read in all lines
	 */
	while (fgets(line, sizeof(line), fp))
	{
		/*----------------------------------------------------------------
		 * zap newline at end
		 */
		strip(line);

		/*----------------------------------------------------------------
		 * skip over any leading white space
		 */
		for (l=line; *l; l++)
		{
			if (! isspace(*l))
				break;
		}

		/*----------------------------------------------------------------
		 * ignore blank lines or comments
		 */
		if (*l == 0 || *l == '*' || *l == '#' || *l == ';')
			continue;

		/*----------------------------------------------------------------
		 * look for "="
		 */
		s = strchr(l, '=');
		if (s == 0)
		{
			/*------------------------------------------------------------
			 * complain about bogus line syntax here
			 */
			continue;
		}
		*s++ = 0;

		/*----------------------------------------------------------------
		 * skip leading white space in data
		 */
		for (; *s; s++)
		{
			if (! isspace(*s))
				break;
		}

		/*----------------------------------------------------------------
		 * check for quotes
		 */
		len = strlen(s);
		if (s[0] == '"' && s[len-1] == '"')
		{
			s[len-1] = 0;
			s++;
		}
		else if (s[0] == '\'' && s[len-1] == '\'')
		{
			s[len-1] = 0;
			s++;
		}

		/*----------------------------------------------------------------
		 * look for matching entry in def_struct
		 */
		for (ds=tbl->def_tbl; ds->type; ds++)
		{
			if (ds->f_str)
			{
				if (strxcmp(l, (tbl->msg_rtn)(ds->f_str)) == 0 ||
				    strxcmp(l, (tbl->msg_rtn)(ds->m_str)) == 0)
				{
					break;
				}
			}
		}

		if (ds->type == 0)
		{
			/*------------------------------------------------------------
			 * complain here about unknown keyword
			 */
			continue;
		}

		/*----------------------------------------------------------------
		 * process this entry
		 */
		rc = dflt_process_entry(tbl, ds, s);
		if (rc)
		{
			/*------------------------------------------------------------
			 * complain about invalid data value here
			 */
		}
	}

	/*--------------------------------------------------------------------
	 * close file
	 */
	fclose(fp);

	return (0);
}

/*------------------------------------------------------------------------
 * dflt_write() - save a table to a file
 */
int dflt_write (const DEFT *tbl, const char *pathname, int all)
{
	FILE *			fp;
	const DEFS *	ds;
	const char *	p;
	const char *	title;
	char			buf[128];
	int				l;
	int				max_len;
	int				first = TRUE;
	int				print_it;
	int				opened;

	/*--------------------------------------------------------------------
	 * open the file
	 */
	if (strcmp(pathname, "-") == 0)
	{
		fp = stdout;
		opened = FALSE;
	}
	else
	{
		fp = fopen(pathname, "w");
		if (fp == 0)
			return (-1);
		opened = TRUE;
	}

	/*--------------------------------------------------------------------
	 * loop thru struct & get longest char string
	 */
	max_len = 0;
	for (ds=tbl->def_tbl; ds->type; ds++)
	{
		if (ds->type != DEF_TITLE)
		{
			p = (tbl->msg_rtn)(ds->f_str);
			l = strlen(p);
			if (l > max_len)
				max_len = l;
		}
	}

	/*--------------------------------------------------------------------
	 * now process all entries
	 */
	title = 0;
	for (ds=tbl->def_tbl; ds->type; ds++)
	{
		print_it = FALSE;

		/*----------------------------------------------------------------
		 * format entry if it is to be written
		 */
		switch (ds->type)
		{
		case DEF_TITLE:
			title = (tbl->msg_rtn)(ds->f_str);
			break;

		case DEF_BOOL:
		case DEF_NUM:
		case DEF_ENUM:
		case DEF_CHAR:
		case DEF_COLOR:
		case DEF_MONO:
		case DEF_STR:
			if (all || ! dflt_is_opt_default(tbl, ds))
			{
				print_it = TRUE;
				dflt_format(tbl, ds, TRUE, buf);
			}
			break;

		case DEF_LIST:
			if (all || ! dflt_is_opt_default(tbl, ds))
			{
				BLIST *b = DFLT_LPTR(tbl, ds);

				if (bcount(b) > 1)
				{
					print_it = TRUE;
					dflt_format(tbl, ds, TRUE, buf);
				}
			}
			break;
		}

		/*----------------------------------------------------------------
		 * write entry if it is to be written
		 */
		if (print_it)
		{
			/*------------------------------------------------------------
			 * display title if first entry for this section
			 */
			if (title)
			{
				if (first)
					first = FALSE;
				else
					fputs("\n", fp);

				fputs("# --------------------------------------"
					"---------------------------------\n", fp);
				fputs("# ", fp);
				fputs(title, fp);
				fputs("\n", fp);
				fputs("#\n", fp);

				title = 0;
			}

			/*------------------------------------------------------------
			 * now output entry
			 */
			p = (tbl->msg_rtn)(ds->f_str);
			l = strlen(p);
			fputs(p, fp);
			for (; l<max_len+1; l++)
				fputc(' ', fp);
			fputs(" = ", fp);
			fputs(buf, fp);
			fputs("\n", fp);
		}
	}

	/*--------------------------------------------------------------------
	 * close the file
	 */
	if (opened)
		fclose(fp);

	return (0);
}

/*------------------------------------------------------------------------
 * dflt_process_entry() - process an entry
 */
int dflt_process_entry (const DEFT *tbl, const DEFS *ds, const char *p)
{
	int	rc;

	/*--------------------------------------------------------------------
	 * process entry based on entry type
	 */
	switch (ds->type)
	{
	case DEF_TITLE:
		rc = 0;
		break;

	case DEF_BOOL:
		{
			int		bval;

			rc = check_yn(tbl, &bval, p);
			if (rc == 0)
			{
				dflt_set_opt(tbl, ds, &bval);
			}
		}
		break;

	case DEF_CHAR:
		{
			int	cval;

			rc = 0;
			cval = *(const unsigned char *)p;
			dflt_set_opt(tbl, ds, &cval);
		}
		break;

	case DEF_ENUM:
		{
			const DEFC *	dc;
			int				l;

			rc = -1;
			dc = (const DEFC *)ds->limit;
			for (l=0; dc[l].f_str; l++)
			{
				if (strxcmp(p, (tbl->msg_rtn)(dc[l].f_str)) == 0 ||
			    	strxcmp(p, (tbl->msg_rtn)(dc[l].m_str)) == 0)
				{
					rc = 0;
					dflt_set_opt(tbl, ds, &l);
					break;
				}
			}
		}
		break;

	case DEF_LIST:
		{
			BLIST *	bl;
			BLIST *	b;
			int		l;

			rc = -1;
			bl = DFLT_LPTR(tbl, ds);
			for (l=0, b=bl; b; l++, b=bnext(b))
			{
				const char *	s = (const char *)bid(b);

				if (strxcmp(p, s) == 0)
				{
					rc = 0;
					dflt_set_opt(tbl, ds, &l);
					break;
				}
			}
		}
		break;

	case DEF_NUM:
		{
			int	nval;
			int	l;

			rc = 0;
			nval = atoi(p);
			if (ds->limit != 0 && (l = *(const int *)ds->limit) > 0)
			{
				if (nval <= l)
				{
					dflt_set_opt(tbl, ds, &nval);
				}
				else
				{
					rc = -1;
				}
			}
			else
			{
				dflt_set_opt(tbl, ds, &nval);
			}
		}
		break;

	case DEF_COLOR:
		{
			attr_t	aval;

			rc = check_color(tbl, &aval, p);
			if (rc == 0)
			{
				dflt_set_opt(tbl, ds, &aval);
			}
		}
		break;

	case DEF_MONO:
		{
			attr_t	aval;

			rc = check_mono(tbl, &aval, p);
			if (rc == 0)
			{
				dflt_set_opt(tbl, ds, &aval);
			}
		}
		break;

	case DEF_STR:
		{
			int	l;

			rc = 0;
			if (ds->limit != 0 && (l = *(const int *)ds->limit) > 0)
			{
				if ((int)strlen(p) < l)
					dflt_set_opt(tbl, ds, p);
				else
					rc = -1;
			}
			else
			{
				dflt_set_opt(tbl, ds, p);
			}
		}
		break;

	default:
		rc = 0;
		break;
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * dflt_set_opt() - set an opt value into an entry
 */
void dflt_set_opt (const DEFT *tbl, const DEFS *ds, const void *value)
{
	switch (ds->type)
	{
	case DEF_BOOL:
		{
			OPT_BOOL *	ob = (OPT_BOOL *)DFLT_OPTS(tbl,ds);

			if (ds->rtn == 0 || (*ds->rtn)(value) >= 0)
				ob->opts[OPT_OPT] = *(const int *)value;
		}
		break;

	case DEF_NUM:
	case DEF_CHAR:
	case DEF_ENUM:
		{
			OPT_INT *	oi = (OPT_INT *)DFLT_OPTS(tbl,ds);

			if (ds->rtn == 0 || (*ds->rtn)(value) >= 0)
				oi->opts[OPT_OPT] = *(const int *)value;
		}
		break;

	case DEF_LIST:
		{
			OPT_INT *	oi = (OPT_INT *)DFLT_OPTS(tbl,ds);
			int			n;

			n = *(const int *)value;
			if (ds->rtn == 0 || (n = (*ds->rtn)(value)) >= 0)
				oi->opts[OPT_OPT] = n;
		}
		break;

	case DEF_COLOR:
	case DEF_MONO:
		{
			OPT_ATTR *	oa = (OPT_ATTR *)DFLT_OPTS(tbl,ds);

			if (ds->rtn == 0 || (*ds->rtn)(value) >= 0)
				oa->opts[OPT_OPT] = *(const attr_t *)value;
		}
		break;

	case DEF_STR:
		{
			OPT_STR *	os = (OPT_STR *)DFLT_OPTS(tbl,ds);

			if (ds->rtn == 0 || (*ds->rtn)(value) >= 0)
				strcpy(os->opts[OPT_OPT], (const char *)value);
		}
		break;
	}
}

/*------------------------------------------------------------------------
 * dflt_set_def() - set an def value into an entry
 */
void dflt_set_def (const DEFT *tbl, const DEFS *ds, const void *value,
	int reset)
{
	switch (ds->type)
	{
	case DEF_BOOL:
		{
			OPT_BOOL *	ob = (OPT_BOOL *)DFLT_OPTS(tbl,ds);

			ob->opts[OPT_DEF] = *(const int *)value;
			if (reset)
				ob->opts[OPT_OPT] = *(const int *)value;
		}
		break;

	case DEF_NUM:
	case DEF_CHAR:
	case DEF_ENUM:
	case DEF_LIST:
		{
			OPT_INT *	oi = (OPT_INT *)DFLT_OPTS(tbl,ds);

			oi->opts[OPT_DEF] = *(const int *)value;
			if (reset)
				oi->opts[OPT_OPT] = *(const int *)value;
		}
		break;

	case DEF_COLOR:
	case DEF_MONO:
		{
			OPT_ATTR *	oa = (OPT_ATTR *)DFLT_OPTS(tbl,ds);

			oa->opts[OPT_DEF] = *(const attr_t *)value;
			if (reset)
				oa->opts[OPT_OPT] = *(const attr_t *)value;
		}
		break;

	case DEF_STR:
		{
			OPT_STR *	os = (OPT_STR *)DFLT_OPTS(tbl,ds);

			strcpy(os->opts[OPT_DEF], (const char *)value);
			if (reset)
				strcpy(os->opts[OPT_OPT], (const char *)value);
		}
		break;
	}
}

/*------------------------------------------------------------------------
 * dflt_is_opt_default() - check if an opt entry value is the default
 */
int dflt_is_opt_default (const DEFT *tbl, const DEFS *ds)
{
	switch (ds->type)
	{
	case DEF_BOOL:
		{
			OPT_BOOL *	ob = (OPT_BOOL *)DFLT_OPTS(tbl,ds);

			return (ob->opts[OPT_OPT] == ob->opts[OPT_DEF]);
		}

	case DEF_NUM:
	case DEF_CHAR:
	case DEF_ENUM:
	case DEF_LIST:
		{
			OPT_INT *	oi = (OPT_INT *)DFLT_OPTS(tbl,ds);

			return (oi->opts[OPT_OPT] == oi->opts[OPT_DEF]);
		}

	case DEF_COLOR:
	case DEF_MONO:
		{
			OPT_ATTR *	oa = (OPT_ATTR *)DFLT_OPTS(tbl,ds);

			return (oa->opts[OPT_OPT] == oa->opts[OPT_DEF]);
		}

	case DEF_STR:
		{
			OPT_STR *	os = (OPT_STR *)DFLT_OPTS(tbl,ds);

			return (strcmp(os->opts[OPT_OPT], os->opts[OPT_DEF]) == 0);
		}
	}

	return (FALSE);
}

/*------------------------------------------------------------------------
 * dflt_save_opt_entry() - cache an opt entry value into save slot
 */
void dflt_save_opt_entry (const DEFT *tbl, const DEFS *ds)
{
	switch (ds->type)
	{
	case DEF_BOOL:
		{
			OPT_BOOL *	ob = (OPT_BOOL *)DFLT_OPTS(tbl,ds);

			ob->opts[OPT_SAV] = ob->opts[OPT_OPT];
		}
		break;

	case DEF_NUM:
	case DEF_CHAR:
	case DEF_ENUM:
	case DEF_LIST:
		{
			OPT_INT *	oi = (OPT_INT *)DFLT_OPTS(tbl,ds);

			oi->opts[OPT_SAV] = oi->opts[OPT_OPT];
		}
		break;

	case DEF_COLOR:
	case DEF_MONO:
		{
			OPT_ATTR *	oa = (OPT_ATTR *)DFLT_OPTS(tbl,ds);

			oa->opts[OPT_SAV] = oa->opts[OPT_OPT];
		}
		break;

	case DEF_STR:
		{
			OPT_STR *	os = (OPT_STR *)DFLT_OPTS(tbl,ds);

			strcpy(os->opts[OPT_SAV], os->opts[OPT_OPT]);
		}
		break;
	}
}

/*------------------------------------------------------------------------
 * dflt_save_opts() - cache all opt entries
 */
void dflt_save_opts (const DEFT *tbl)
{
	const DEFS *ds;

	for (ds=tbl->def_tbl; ds->type; ds++)
		dflt_save_opt_entry(tbl, ds);
}

/*------------------------------------------------------------------------
 * dflt_restore_opt_entry() - restore an opt entry from saved value
 */
void dflt_restore_opt_entry (const DEFT *tbl, const DEFS *ds)
{
	switch (ds->type)
	{
	case DEF_BOOL:
		{
			OPT_BOOL *	ob = (OPT_BOOL *)DFLT_OPTS(tbl,ds);

			dflt_set_opt(tbl, ds, &ob->opts[OPT_SAV]);
		}
		break;

	case DEF_NUM:
	case DEF_CHAR:
	case DEF_ENUM:
	case DEF_LIST:
		{
			OPT_INT *	oi = (OPT_INT *)DFLT_OPTS(tbl,ds);

			dflt_set_opt(tbl, ds, &oi->opts[OPT_SAV]);
		}
		break;

	case DEF_COLOR:
	case DEF_MONO:
		{
			OPT_ATTR *	oa = (OPT_ATTR *)DFLT_OPTS(tbl,ds);

			dflt_set_opt(tbl, ds, &oa->opts[OPT_SAV]);
		}
		break;

	case DEF_STR:
		{
			OPT_STR *	os = (OPT_STR *)DFLT_OPTS(tbl,ds);

			dflt_set_opt(tbl, ds, os->opts[OPT_SAV]);
		}
		break;
	}
}

/*------------------------------------------------------------------------
 * dflt_restore_opts() - restore all opt entries from saved values
 */
void dflt_restore_opts (const DEFT *tbl)
{
	const DEFS *ds;

	for (ds=tbl->def_tbl; ds->type; ds++)
		dflt_restore_opt_entry(tbl, ds);
}

/*------------------------------------------------------------------------
 * dflt_init_opt_entry() - set an opt entry to its default value
 */
void dflt_init_opt_entry (const DEFT *tbl, const DEFS *ds)
{
	switch (ds->type)
	{
	case DEF_BOOL:
		{
			OPT_BOOL *	ob = (OPT_BOOL *)DFLT_OPTS(tbl,ds);

			dflt_set_opt(tbl, ds, &ob->opts[OPT_DEF]);
		}
		break;

	case DEF_NUM:
	case DEF_CHAR:
	case DEF_ENUM:
	case DEF_LIST:
		{
			OPT_INT *	oi = (OPT_INT *)DFLT_OPTS(tbl,ds);

			dflt_set_opt(tbl, ds, &oi->opts[OPT_DEF]);
		}
		break;

	case DEF_COLOR:
	case DEF_MONO:
		{
			OPT_ATTR *	oa = (OPT_ATTR *)DFLT_OPTS(tbl,ds);

			dflt_set_opt(tbl, ds, &oa->opts[OPT_DEF]);
		}
		break;

	case DEF_STR:
		{
			OPT_STR *	os = (OPT_STR *)DFLT_OPTS(tbl,ds);

			dflt_set_opt(tbl, ds,  os->opts[OPT_DEF]);
		}
		break;
	}
}

/*------------------------------------------------------------------------
 * dflt_init_opts() - set all opt entries to their default value
 */
void dflt_init_opts (const DEFT *tbl)
{
	const DEFS *ds;

	for (ds=tbl->def_tbl; ds->type; ds++)
		dflt_init_opt_entry(tbl, ds);
}

/*------------------------------------------------------------------------
 * dflt_init_def_entry() - set an entry's default value
 */
void dflt_init_def_entry (const DEFT *tbl, const DEFS *ds, int reset)
{
	const char *	p;

	/*--------------------------------------------------------------------
	 * if reset is TRUE, then we are "resetting" the defaults entries,
	 * and we want to change the options entry if the current value
	 * is not the old default.
	 */
	if (reset)
	{
		reset = dflt_is_opt_default(tbl, ds);
	}

	p = (tbl->msg_rtn)(ds->v_str);
	switch (ds->type)
	{
	case DEF_TITLE:
		break;

	case DEF_BOOL:
		{
			int		bval;
			int		rc;

			rc = check_yn(tbl, &bval, p);
			if (rc == 0)
			{
				dflt_set_def(tbl, ds, &bval, reset);
			}
		}
		break;

	case DEF_NUM:
		{
			int	nval;
			int l;

			nval = atoi(p);
			if (ds->limit != 0 && (l = *(const int *)ds->limit) > 0)
			{
				if (nval <= l)
				{
					dflt_set_def(tbl, ds, &nval, reset);
				}
			}
			else
			{
				dflt_set_def(tbl, ds, &nval, reset);
			}
		}
		break;

	case DEF_CHAR:
		{
			int cval;

			cval = *(const unsigned char *)p;
			dflt_set_def(tbl, ds, &cval, reset);
		}
		break;

	case DEF_ENUM:
		{
			const DEFC *	dc;
			int				l;

			dc = (const DEFC *)ds->limit;
			for (l=0; dc[l].f_str; l++)
			{
				if (strxcmp(p, (tbl->msg_rtn)(dc[l].f_str)) == 0 ||
			    	strxcmp(p, (tbl->msg_rtn)(dc[l].m_str)) == 0)
				{
					dflt_set_def(tbl, ds, &l, reset);
					break;
				}
			}
		}
		break;

	case DEF_LIST:
		{
			BLIST *	bl;
			BLIST *	b;
			int		l;

			bl = DFLT_LPTR(tbl, ds);
			for (l=0, b=bl; b; l++, b=bnext(b))
			{
				const char *	s = (const char *)bid(b);

				if (strxcmp(p, s) == 0)
				{
					dflt_set_def(tbl, ds, &l, reset);
					break;
				}
			}
		}
		break;

	case DEF_COLOR:
		{
			attr_t	aval;
			int		rc;

			rc = check_color(tbl, &aval, p);
			if (rc == 0)
			{
				dflt_set_def(tbl, ds, &aval, reset);
			}
		}
		break;

	case DEF_MONO:
		{
			attr_t	aval;
			int		rc;

			rc = check_mono(tbl, &aval, p);
			if (rc == 0)
			{
				dflt_set_def(tbl, ds, &aval, reset);
			}
		}
		break;

	case DEF_STR:
		{
			int	l;

			if (ds->limit != 0 && (l = *(const int *)ds->limit) > 0)
			{
				if ((int)strlen(p) < l)
				{
					dflt_set_def(tbl, ds, p, reset);
				}
			}
			else
			{
				dflt_set_def(tbl, ds, p, reset);
			}
		}
		break;
	}
}

/*------------------------------------------------------------------------
 * dflt_init_defs() - set all entries' default value
 */
void dflt_init_defs (const DEFT *tbl, int reset)
{
	const DEFS *ds;

	for (ds=tbl->def_tbl; ds->type; ds++)
		dflt_init_def_entry(tbl, ds, reset);
}

/*------------------------------------------------------------------------
 * dflt_format() - format an entry into a printable buffer
 */
void dflt_format (const DEFT *tbl, const DEFS *ds, int file, char *buf)
{
	switch (ds->type)
	{
	case DEF_BOOL:
		{
			OPT_BOOL *		ob;
			const DEFC *	dc;
			int				bv;

			ob = (OPT_BOOL *)DFLT_OPTS(tbl,ds);
			bv = ob->opts[OPT_OPT];
			dc = (bv ? tbl->yes_tbl : tbl->nos_tbl);
			strcpy(buf, (tbl->msg_rtn)(file ? dc[0].f_str : dc[0].m_str));
		}
		break;

	case DEF_NUM:
		{
			OPT_INT *	oi;
			int			nv;

			oi = (OPT_INT *)DFLT_OPTS(tbl,ds);
			nv = oi->opts[OPT_OPT];
			sprintf(buf, "%d", nv);
		}
		break;

	case DEF_CHAR:
		{
			OPT_INT *	oi;
			int			cv;
			int			i;

			oi = (OPT_INT *)DFLT_OPTS(tbl,ds);
			cv = oi->opts[OPT_OPT];
			i  = 0;
			if (file)
				buf[i++] = '\'';
			buf[i++] = cv;
			if (file)
				buf[i++] = '\'';
			buf[i] = 0;
		}
		break;

	case DEF_ENUM:
		{
			OPT_INT *		oi;
			const DEFC *	dc;
			int				nv;

			oi = (OPT_INT *)DFLT_OPTS(tbl,ds);
			nv = oi->opts[OPT_OPT];
			dc = (const DEFC *)ds->limit;
			strcpy(buf, (tbl->msg_rtn)(file ? dc[nv].f_str : dc[nv].m_str));
		}
		break;

	case DEF_LIST:
		{
			OPT_INT *		oi;
			BLIST *			bl;
			int				nv;
			const char *	s;

			oi = (OPT_INT *)DFLT_OPTS(tbl,ds);
			nv = oi->opts[OPT_OPT];
			bl = DFLT_LPTR(tbl, ds);
			bl = bnth(bl, nv);
			s = (const char *)bid(bl);
			strcpy(buf, s);
		}
		break;

	case DEF_COLOR:
		{
			OPT_ATTR *		oa;
			const DEFC *	df;
			const DEFC *	db;
			attr_t			ch;
			int				fg;
			int				bg;

			oa = (OPT_ATTR *)DFLT_OPTS(tbl,ds);
			ch = oa->opts[OPT_OPT];
			df = tbl->fg_names;
			db = tbl->bg_names;
			fg = get_fg_num_by_code(ch);
			bg = get_bg_num_by_code(ch);
			strcpy(buf, (tbl->msg_rtn)(file ? df[fg].f_str : df[fg].m_str));
			strcat(buf, "/");
			strcat(buf, (tbl->msg_rtn)(file ? db[bg].f_str : db[bg].m_str));
		}
		break;

	case DEF_MONO:
		{
			OPT_ATTR *		oa;
			const DEFC *	da;
			attr_t			ch;
			int				at;

			oa = (OPT_ATTR *)DFLT_OPTS(tbl,ds);
			ch = oa->opts[OPT_OPT];
			da = tbl->attr_names;
			at = get_attr_num_by_code(ch);
			strcpy(buf, (tbl->msg_rtn)(file ? da[at].f_str : da[at].m_str));
		}
		break;

	case DEF_STR:
		{
			OPT_STR *	os;

			os = (OPT_STR *)DFLT_OPTS(tbl,ds);
			*buf = 0;
			if (file)
				strcat(buf, "\"");
			strcat(buf, os->opts[OPT_OPT]);
			if (file)
				strcat(buf, "\"");
		}
		break;

	default:
		*buf = 0;
		break;
	}
}

/*------------------------------------------------------------------------
 * dflt_find_entry() - find an entry based on the option struct it points to
 */
const DEFS * dflt_find_entry (const DEFT *tbl, int opt_off)
{
	const DEFS *ds;

	for (ds=tbl->def_tbl; ds->type; ds++)
	{
		if (ds->type != DEF_TITLE && ds->opt_off == opt_off)
			return (ds);
	}

	return (0);
}
