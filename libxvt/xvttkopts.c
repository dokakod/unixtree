/*------------------------------------------------------------------------
 * process xvt cmd-line X-toolkit options
 */
#include "xvtcommon.h"

/*------------------------------------------------------------------------
 * find a toolkit option in the list by option-string
 */
const TK_OPTIONS * xvt_tk_find_by_option (const char *option)
{
	const TK_OPTIONS *	op;

	for (op=xvt_tk_options; op->opt_type; op++)
	{
		if (op->opt_string != 0)
		{
			if (xvt_strccmp(op->opt_string, option) == 0)
				return (op);
		}
	}

	return (0);
}

/*------------------------------------------------------------------------
 * find a toolkit option in the list by resource-name
 */
const TK_OPTIONS * xvt_tk_find_by_resname (const char *resname)
{
	const TK_OPTIONS *	op;

	for (op=xvt_tk_options; op->opt_type; op++)
	{
		if (op->opt_resname != 0)
		{
			if (xvt_strccmp(op->opt_resname, resname) == 0)
				return (op);
		}
	}

	return (0);
}

/*------------------------------------------------------------------------
 * process a cmd-line option
 *
 * returns:
 *		-1	error
 *		 0	not a toolkit option
 *		 1	toolkit option - option used
 *		 2	toolkit option - option & arg used
 *
 *		XVT_TK_OPT_* in type
 */
int xvt_tk_opt_set (XVT_DATA *xd, const char *option, const char *arg,
	int resent, int *type)
{
	const TK_OPTIONS *	op;
	const char *		p;
	char				optbuf[128];
	int					flag = -1;
	int					rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (xd == 0 || option == 0)
		return (-1);

	for (; *option; option++)
	{
		if (! isspace(*option))
			break;
	}

	if (*option == 0)
	{
		if (type != 0)
			*type = XVT_TK_OPT_INV;
		return (0);
	}

	/*--------------------------------------------------------------------
	 * peel off leading +/-
	 */
	if (*option == '-')
	{
		flag = TRUE;
		option++;
	}
	else if (*option == '+')
	{
		flag = FALSE;
		option++;
	}

	/*--------------------------------------------------------------------
	 * if arg in form "xxx=yyy", point arg to "yyy" (space too)
	 */
	p = strpbrk(option, "=: ");
	if (p != 0)
	{
		const char *s;
		char *		t;

		for (t=optbuf, s=option; s<p; s++)
			*t++ = *s;
		*t = 0;
		option = optbuf;

		arg = (p + 1);
	}

	/*--------------------------------------------------------------------
	 * check if string matches a toolkit-option
	 *
	 * Note: if <resent> is TRUE, then it is a resource name.
	 * otherwise, it may be either.
	 */
	op = 0;
	if (! resent)
		op = xvt_tk_find_by_option(option);
	if (op == 0)
		op = xvt_tk_find_by_resname(option);

	if (op == 0)
	{
		if (type != 0)
			*type = XVT_TK_OPT_INV;
		return (0);
	}

	/*--------------------------------------------------------------------
	 * process option according to its type
	 */
	switch (op->opt_type)
	{
	case XVT_TK_OPT_STR:
	case XVT_TK_OPT_CLR:
	case XVT_TK_OPT_FNT:
		{
			const char ** pstr = (const char **)((char *)xd + op->opt_offset);

			/*------------------------------------------------------------
			 * We cache string args into our string buffer, to avoid
			 * problems with automatic data.
			 */
			if (arg != 0)
			{
				for (; *arg; arg++)
				{
					if (! isspace(*arg))
						break;
				}

				if (*arg == 0)
				{
					arg = "";
				}
				else
				{
					int n;

					n = xvt_strcpy(xd->tkbuf + xd->tkbuf_index, arg);
					arg = xd->tkbuf + xd->tkbuf_index;
					xd->tkbuf_index += (n + 1);
				}
			}

			*pstr = arg;
		}
		rc = (p == 0 ? 2 : 1);
		break;

	case XVT_TK_OPT_NUM:
		{
			int * pnum = (int *)((char *)xd + op->opt_offset);

			*pnum = (arg == 0 ? 0 : atoi(arg));
		}
		rc = (p == 0 ? 2 : 1);
		break;

	case XVT_TK_OPT_BLN:
		{
			int * pbln = (int *)((char *)xd + op->opt_offset);

			if (p != 0)
			{
				rc = xvt_str_to_bool(arg);

				if (rc != -1)
					*pbln = rc;
			}
			else if (flag != -1)
			{
				*pbln = flag;
			}
			else if (arg != 0)
			{
				rc = xvt_str_to_bool(arg);

				if (rc != -1)
					*pbln = rc;
			}
		}
		rc = (flag == -1 ? 2 : 1);
		break;

	case XVT_TK_OPT_DBG:
		if (arg != 0)
		{
			FILE **			pfp;
			char *			path;
			const char *	log;

			log = strchr(arg, ':');
			if (log != 0)
				log++;

			if      (xvt_strccmpn(XVT_D_OPT_EVENTS, arg) == 0)
			{
				pfp		= &xd->events_fp;
				path	= xd->events_path;
				if (log == 0)	log = XVT_D_OPT_EVENTS_LOG;
			}
			else if (xvt_strccmpn(XVT_D_OPT_ACTION, arg) == 0)
			{
				pfp		= &xd->action_fp;
				path	= xd->action_path;
				if (log == 0)	log = XVT_D_OPT_ACTION_LOG;
			}
			else if (xvt_strccmpn(XVT_D_OPT_OUTPUT, arg) == 0)
			{
				pfp		= &xd->output_fp;
				path	= xd->output_path;
				if (log == 0)	log = XVT_D_OPT_OUTPUT_LOG;
			}
			else if (xvt_strccmpn(XVT_D_OPT_SCREEN, arg) == 0)
			{
				pfp		= &xd->screen_fp;
				path	= xd->screen_path;
				if (log == 0)	log = XVT_D_OPT_SCREEN_LOG;
			}
			else if (xvt_strccmpn(XVT_D_OPT_INPKBD, arg) == 0)
			{
				pfp		= &xd->inpkbd_fp;
				path	= xd->inpkbd_path;
				if (log == 0)	log = XVT_D_OPT_INPKBD_LOG;
			}
			else if (xvt_strccmpn(XVT_D_OPT_KEYBRD, arg) == 0)
			{
				pfp		= &xd->keybrd_fp;
				path	= xd->keybrd_path;
				if (log == 0)	log = XVT_D_OPT_KEYBRD_LOG;
			}
			else if (xvt_strccmpn(XVT_D_OPT_RESFIL, arg) == 0)
			{
				pfp		= &xd->resfil_fp;
				path	= xd->resfil_path;
				if (log == 0)	log = XVT_D_OPT_RESFIL_LOG;
			}
			else if (xvt_strccmpn(XVT_D_OPT_TKOPTS, arg) == 0)
			{
				pfp		= &xd->tkopts_fp;
				path	= xd->tkopts_path;
				if (log == 0)	log = XVT_D_OPT_TKOPTS_LOG;
			}
			else
			{
				pfp		= 0;
				path	= 0;
			}

			if (pfp != 0)
			{
				if (strcmp(log, "-") == 0)
				{
					strcpy(path, log);
					*pfp = stderr;
				}
				else
				{
					xvt_path_resolve(path, log);
					*pfp = fopen(path, "w");
				}
			}
		}
		rc = (p == 0 ? 2 : 1);
		break;

	case XVT_TK_OPT_CMD:
		rc = 1;
		break;
	}

	if (type != 0)
		*type = op->opt_type;

	return (rc);
}

/*------------------------------------------------------------------------
 * process cmd-line X-toolkit options
 */
int xvt_tk_args (XVT_DATA *xd, int argc, char **argv)
{
	char **	new_argv;
	int		new_argc;
	int		check_tk;
	int		type;
	int		i;
	int		rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (xd == 0)
	{
		return (-1);
	}

	if (argc == 0 || argv == 0)
	{
		sprintf(xd->error_msg, "NULL argument");
		xd->error_num = XVT_ERR_INVARG;
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * allocate a new argv array the same size as the original
	 */
	new_argv = (char **)MALLOC((argc + 1) * sizeof(char *));
	if (new_argv == 0)
	{
		sprintf(xd->error_msg, "No memory");
		xd->error_num = XVT_ERR_NOMEM;
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * now copy all entries over, processing them as we go
	 */
	new_argc	= 0;
	check_tk	= TRUE;

	new_argv[new_argc++] = (char *)xvt_path_basename(argv[0]);

	for (i=1; i<argc; i++)
	{
		/*----------------------------------------------------------------
		 * get status of next option
		 *
		 * If it was a toolkit option, the XVT_DATA struct is updated.
		 */
		type = XVT_TK_OPT_INV;
		if (check_tk)
		{
			if (*argv[i] == '-' || *argv[i] == '+')
				rc = xvt_tk_opt_set(xd, argv[i], argv[i+1], FALSE, &type);
		}

		switch (type)
		{
		case XVT_TK_OPT_INV:
			/*------------------------------------------------------------
			 * not a toolkit option - just copy the arg
			 */
			new_argv[new_argc++] = argv[i];
			break;

		case XVT_TK_OPT_STR:
		case XVT_TK_OPT_CLR:
		case XVT_TK_OPT_FNT:
		case XVT_TK_OPT_NUM:
		case XVT_TK_OPT_DBG:
		case XVT_TK_OPT_BLN:
			/*------------------------------------------------------------
			 * toolkit option - bump the arg pointer
			 */
			i += (rc - 1);
			break;

		case XVT_TK_OPT_CMD:
			/*------------------------------------------------------------
			 * cmd option - just copy over rest of args after indicating
			 * end-of-arglist
			 */
			new_argv[new_argc++] = (char *)"--";
			check_tk = FALSE;
			break;
		}
	}
	new_argv[new_argc] = 0;

	/*--------------------------------------------------------------------
	 * store new values
	 */
	xd->argc = new_argc;
	xd->argv = new_argv;

	return (XVT_OK);
}

/*------------------------------------------------------------------------
 * process a toolkit option list
 */
int	xvt_tk_optlist (XVT_DATA *xd, const char *list)
{
	char		opt_buf[128];
	char *		p;
	const char *s;
	int			rc;
	int			rv;
	int			check_str	= FALSE;
	int			check_clr	= FALSE;
	int			check_num	= FALSE;
	int			type;

	/*--------------------------------------------------------------------
	 * validate args
	 */
	if (xd == 0)
		return (-1);

	if (list == 0 || *list == 0)
		return (XVT_OK);

	/*--------------------------------------------------------------------
	 * rip through the list
	 */
	s = list;
	rc = 0;
	while (*s != 0)
	{
		/*----------------------------------------------------------------
		 * collect next part
		 */
		p = opt_buf;
		for (; *s; s++)
		{
			if (*s == ';' || *s == ',')
				break;
			*p++ = *s;
		}
		*p = 0;

		if (*s != 0)
			s++;

		if (*opt_buf == 0)
			continue;

		/*----------------------------------------------------------------
		 * now parse this puppy
		 */
		rv = xvt_tk_opt_set(xd, opt_buf, 0, FALSE, &type);
		if (rv == XVT_TK_OPT_INV)
		{
			sprintf(xd->error_msg, "invalid toolkit option \"%s\"", opt_buf);
			xd->error_num = XVT_ERR_INVOPT;
			rc = -1;
			break;
		}

		/*----------------------------------------------------------------
		 * check if error-checking needed
		 */
		switch (type)
		{
		case XVT_TK_OPT_STR:	check_str = TRUE;	break;
		case XVT_TK_OPT_CLR:	check_clr = TRUE;	break;
		case XVT_TK_OPT_NUM:	check_num = TRUE;	break;
		}
	}

	/*--------------------------------------------------------------------
	 * now do any error checking
	 */
	if (rc == XVT_OK && check_clr)
		rc = xvt_rgb_check(xd);

	if (rc == XVT_OK && check_str)
	{
		if (xd->termmode != 0 && *xd->termmode != 0)
		{
			rc = xvt_tio_build(0, xd->termmode, FALSE, xd->error_msg);
			if (rc)
			{
				xd->error_num = XVT_ERR_INVOPT;
				rc = -1;
			}
		}
	}

	if (rc == XVT_OK && check_num)
	{
		if (xd->fontno < 0 || xd->fontno >= XVT_NUM_FONTS)
		{
			sprintf(xd->error_msg, "invalid font number %d", xd->fontno);
			xd->error_num = XVT_ERR_INVOPT;
			rc = -1;
		}
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * get string representation of a data entry
 *
 * returns: NULL if data not set
 */
char * xvt_tk_data_value (XVT_DATA *xd, const TK_OPTIONS *op,
	int use_dflt, char *buf)
{
	/*--------------------------------------------------------------------
	 * check if a resource name is present in the entry
	 */
	if (! use_dflt && op->opt_resname == 0)
		return (0);

	*buf = 0;

	/*--------------------------------------------------------------------
	 * process option according to its type
	 */
	switch (op->opt_type)
	{
	case XVT_TK_OPT_STR:
	case XVT_TK_OPT_CLR:
	case XVT_TK_OPT_FNT:
		{
			const char ** pstr = (const char **)((char *)xd + op->opt_offset);

			if (*pstr != 0)
			{
				strcpy(buf, *pstr);
				return (buf);
			}
			else if (use_dflt && op->opt_default != 0)
			{
				const char * s = 0;

				if (op->opt_flags & TK_FLAG_APPOVR)
				{
					if (xd->appname != 0)
						s = xd->appname;
				}

				if (op->opt_flags & TK_FLAG_ENVNAME)
				{
					const char *e = getenv(op->opt_default + 1);

					if (e != 0)
						s = e;
				}

				if (s == 0 && (op->opt_flags & TK_FLAG_PHONYDFLT) == 0)
					s = op->opt_default;

				if (s != 0)
				{
					sprintf(buf, "(%s)", s);
					return (buf);
				}
			}
		}
		break;

	case XVT_TK_OPT_NUM:
		{
			int * pnum = (int *)((char *)xd + op->opt_offset);

			if (*pnum != -1)
			{
				sprintf(buf, "%d", *pnum);
				return (buf);
			}
			else if (use_dflt)
			{
				sprintf(buf, "(%d)", (int)op->opt_default);
				return (buf);
			}
		}
		break;

	case XVT_TK_OPT_BLN:
		{
			int * pbln = (int *)((char *)xd + op->opt_offset);

			if (*pbln != -1)
			{
				strcpy(buf, *pbln ? "on" : "off");
				return (buf);
			}
			else if (use_dflt)
			{
				sprintf(buf, "(%s)", (int)op->opt_default ? "on" : "off");
				return (buf);
			}
		}
		break;

	case XVT_TK_OPT_DBG:
	case XVT_TK_OPT_CMD:
		break;
	}

	return (0);
}

/*------------------------------------------------------------------------
 * initialize all user config entries  in the XVT_DATA struct
 */
void xvt_tk_data_init (XVT_DATA *xd, int set_dflt)
{
	const TK_OPTIONS *	op;

	/*--------------------------------------------------------------------
	 * if not setting default values, start with a cleared-out struct
	 */
	if (! set_dflt)
		memset(xd, 0, sizeof(*xd));

	/*--------------------------------------------------------------------
	 * now process all entries
	 */
	for (op=xvt_tk_options; op->opt_type; op++)
	{
		switch (op->opt_type)
		{
		case XVT_TK_OPT_STR:
		case XVT_TK_OPT_CLR:
		case XVT_TK_OPT_FNT:
			{
				const char ** pstr =
					(const char **)((char *)xd + op->opt_offset);

				if (set_dflt)
				{
					if (*pstr == 0 && op->opt_default != 0)
					{
						const char * s = 0;

						if (op->opt_flags & TK_FLAG_APPOVR)
						{
							if (xd->appname != 0)
								s = xd->appname;
						}

						if (op->opt_flags & TK_FLAG_ENVNAME)
						{
							const char *e = getenv(op->opt_default + 1);

							if (e != 0)
								s = e;
						}

						if (s == 0 && (op->opt_flags & TK_FLAG_PHONYDFLT) == 0)
							s = op->opt_default;

						if (s != 0)
							*pstr = s;
					}
				}
				else
				{
					*pstr = 0;
				}
			}
			break;

		case XVT_TK_OPT_NUM:
			{
				int * pnum = (int *)((char *)xd + op->opt_offset);

				if (set_dflt)
				{
					if (*pnum == -1)
						*pnum = (int)op->opt_default;
				}
				else
				{
					*pnum = -1;
				}
			}
			break;

		case XVT_TK_OPT_BLN:
			{
				int * pbln = (int *)((char *)xd + op->opt_offset);

				if (set_dflt)
				{
					if (*pbln == -1)
						*pbln = (int)op->opt_default;
				}
				else
				{
					*pbln = -1;
				}
			}
			break;

		case XVT_TK_OPT_DBG:
		case XVT_TK_OPT_CMD:
			break;
		}
	}

	/*--------------------------------------------------------------------
	 * now set all log paths
	 */
	if (*xd->events_path == 0)	strcpy(xd->events_path, XVT_D_OPT_EVENTS_LOG);
	if (*xd->action_path == 0)	strcpy(xd->action_path, XVT_D_OPT_ACTION_LOG);
	if (*xd->output_path == 0)	strcpy(xd->output_path, XVT_D_OPT_OUTPUT_LOG);
	if (*xd->screen_path == 0)	strcpy(xd->screen_path, XVT_D_OPT_SCREEN_LOG);
	if (*xd->inpkbd_path == 0)	strcpy(xd->inpkbd_path, XVT_D_OPT_INPKBD_LOG);
	if (*xd->keybrd_path == 0)	strcpy(xd->keybrd_path, XVT_D_OPT_KEYBRD_LOG);
	if (*xd->resfil_path == 0)	strcpy(xd->resfil_path, XVT_D_OPT_RESFIL_LOG);
	if (*xd->tkopts_path == 0)	strcpy(xd->tkopts_path, XVT_D_OPT_TKOPTS_LOG);
}

/*------------------------------------------------------------------------
 * dump all options to a stream
 */
int xvt_tk_options_dump (XVT_DATA *xd, FILE *fp, int show_dflt)
{
	const TK_OPTIONS *	op;
	char				data_buf[256];
	char *				p;
	int					maxlen;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (xd == 0)
		return (-1);

	if (fp == 0)
	{
		sprintf(xd->error_msg, "NULL stream");
		xd->error_num = XVT_ERR_INVARG;
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * get max length of a options string
	 */
	maxlen = 0;
	for (op=xvt_tk_options; op->opt_type; op++)
	{
		if (op->opt_string != 0)
		{
			int l	= strlen(op->opt_string);

			if (l > maxlen)
				maxlen = l;
		}
	}

	/*--------------------------------------------------------------------
	 * process all entries
	 */
	for (op=xvt_tk_options; op->opt_type; op++)
	{
		p = xvt_tk_data_value(xd, op, show_dflt, data_buf);
		if (p != 0)
		{
			fprintf(fp, "%-*s : %s\n", maxlen, op->opt_string, p);
		}
	}

	return (XVT_OK);
}
