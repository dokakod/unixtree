/*------------------------------------------------------------------------
 * produce help listings
 */
#include "xvtcommon.h"

/*------------------------------------------------------------------------
 * type names
 */
struct help_types
{
	int				type;
	const char *	name;
};
typedef struct help_types HELP_TYPES;

static const HELP_TYPES xvt_tk_help_types[] =
{
	{ XVT_TK_OPT_INV,	"invalid"	},
	{ XVT_TK_OPT_STR,	"string"	},
	{ XVT_TK_OPT_CLR,	"color"		},
	{ XVT_TK_OPT_FNT,	"font"		},
	{ XVT_TK_OPT_NUM,	"numeric"	},
	{ XVT_TK_OPT_BLN,	"boolean"	},
	{ XVT_TK_OPT_DBG,	"debug"		},
	{ XVT_TK_OPT_CMD,	"command"	},

	{ XVT_TK_OPT_ERR,	"error"		}
};

/*------------------------------------------------------------------------
 * get help type name
 */
static const char * xvt_tk_help_type_name (int type)
{
	const HELP_TYPES *	h;

	for (h=xvt_tk_help_types; h->type >= 0; h++)
	{
		if (h->type == type)
			break;
	}

	return (h->name);
}

/*------------------------------------------------------------------------
 * list all toolkit options
 *
 * type:
 *			XVT_TK_HELP_SHORT		short listing
 *			XVT_TK_HELP_LONG		long listing
 *			XVT_TK_HELP_DETAIL		detailed listing
 *
 * mode:
 *			XVT_TK_HELP_TASK		running in "task" mode
 *			XVT_TK_HELP_HERE		running in "here" mode
 */
void xvt_tk_help (XVT_DATA *xd, FILE *fp, int type, int mode)
{
	const TK_OPTIONS *	op;
	char				line[76];
	int					maxlen	= sizeof(line) - 1;
	int					len		= 0;
	int					htype	= -1;
	int					skip_short	= (TK_FLAG_NOTINHELP |
										TK_FLAG_NOHELP_0 |
										TK_FLAG_NOHELP_1);
	int					skip_long	= (TK_FLAG_NOTINHELP |
										TK_FLAG_NOHELP_1);
	int					skip_detail	= (TK_FLAG_NOTINHELP);

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (fp == 0)
		return;

	/*--------------------------------------------------------------------
	 * check type of output
	 */
	fprintf(fp, "Toolkit options:\n");

	switch (type)
	{
	case XVT_TK_HELP_SHORT:
		/*----------------------------------------------------------------
		 * short listing
		 */
		for (op=xvt_tk_options; op->opt_type; op++)
		{
			int l;

			/*------------------------------------------------------------
			 * check if this entry should be skipped
			 */
			if (op->opt_flags & skip_short)
				continue;

			if (op->opt_desc == 0)
				continue;

			if (mode == XVT_TK_HELP_HERE && (op->opt_flags & TK_FLAG_NOTHERE))
				continue;

			if (mode == XVT_TK_HELP_TASK && (op->opt_flags & TK_FLAG_NOTTASK))
				continue;

#if 0
			/*------------------------------------------------------------
			 * This will separate each option type.
			 * (Disabled because it makes the help output too long...)
			 */
			if (htype != op->opt_type)
			{
				if (len > 0)
				{
					fprintf(fp, "  %s\n", line);
					len = 0;
				}

				fprintf(fp, "  --%s options--\n",
					xvt_tk_help_type_name(op->opt_type));
			}
#endif

			/*------------------------------------------------------------
			 * print out buffer if needed
			 */
			l = strlen(op->opt_help) + 3;
			if ((len + l) > maxlen)
			{
				fprintf(fp, "  %s\n", line);
				len = 0;
			}

			/*------------------------------------------------------------
			 * add entry to buffer
			 */
			htype = op->opt_type;
			if (len > 0)
				line[len++] = ' ';
			line[len++] = '[';
			len += xvt_strcpy(line + len, op->opt_help);
			line[len++] = ']';
			line[len  ] = 0;
		}

		/*----------------------------------------------------------------
		 * print buffer if anything left in it
		 */
		if (len > 0)
			fprintf(fp, "  %s\n", line);

		break;

	case XVT_TK_HELP_LONG:
		/*----------------------------------------------------------------
		 * long listing
		 */
		for (op=xvt_tk_options; op->opt_type; op++)
		{
			/*------------------------------------------------------------
			 * check if this entry should be skipped
			 */
			if (op->opt_flags & skip_long)
				continue;

			if (op->opt_desc == 0)
				continue;

			if (mode == XVT_TK_HELP_HERE && (op->opt_flags & TK_FLAG_NOTHERE))
				continue;

			if (mode == XVT_TK_HELP_TASK && (op->opt_flags & TK_FLAG_NOTTASK))
				continue;

			/*------------------------------------------------------------
			 * print entry
			 */
			fprintf(fp, "  %-16s  %s\n",
				op->opt_help,
				op->opt_desc);

			/*------------------------------------------------------------
			 * print any extra lines associated with it
			 */
			if (op->opt_lines != 0)
			{
				const char ** l;

				for (l=op->opt_lines; *l; l++)
				{
					fprintf(fp, "  %-16s  %s\n", "", *l);
				}
			}
		}

		break;

	case XVT_TK_HELP_DETAIL:
		/*----------------------------------------------------------------
		 * detailed listing
		 */
		fprintf(fp,
			"  Option            Default       "
			"Resource name    Description\n");
		fprintf(fp,
			"------------------  ------------  "
			"--------------   ------------------------\n");

		for (op=xvt_tk_options; op->opt_type; op++)
		{
			const char *	opt_default;
			int				opt_default_num;
			char			opt_default_buf[24];

			/*------------------------------------------------------------
			 * check if this entry should be skipped
			 */
			if (op->opt_flags & skip_detail)
				continue;

			if (op->opt_desc == 0)
				continue;

			if (mode == XVT_TK_HELP_HERE && (op->opt_flags & TK_FLAG_NOTHERE))
				continue;

			if (mode == XVT_TK_HELP_TASK && (op->opt_flags & TK_FLAG_NOTTASK))
				continue;

			/*------------------------------------------------------------
			 * if option type changed, output header for it
			 */
			if (op->opt_type != htype)
			{
				if (htype != -1)
					fprintf(fp, "\n");

				htype = op->opt_type;
				fprintf(fp, "--%s options--\n", xvt_tk_help_type_name(htype));
			}

			/*------------------------------------------------------------
			 * get printable version of default
			 */
			switch (op->opt_type)
			{
			case XVT_TK_OPT_STR:
			case XVT_TK_OPT_CLR:
			case XVT_TK_OPT_FNT:
				opt_default = (op->opt_default == 0 ? "" : op->opt_default);
				break;

			case XVT_TK_OPT_NUM:
				opt_default_num = (int)op->opt_default;
				sprintf(opt_default_buf, "%d", opt_default_num);
				opt_default = opt_default_buf;
				break;

			case XVT_TK_OPT_BLN:
				opt_default_num = (int)op->opt_default;
				opt_default = (opt_default_num ? "on" : "off");
				break;

			case XVT_TK_OPT_DBG:
			case XVT_TK_OPT_CMD:
				opt_default = "";
				break;
			}

			if (xd != 0 && xd->appname != 0 &&
			    (op->opt_flags & TK_FLAG_APPOVR))
			{
				opt_default = xd->appname;
			}

			/*------------------------------------------------------------
			 * now print the entry
			 */
			fprintf(fp, "  %-16s  %-12s  %-16s %s\n",
				op->opt_help,
				opt_default,
				op->opt_resname == 0 ? "" : op->opt_resname,
				op->opt_desc);

			/*------------------------------------------------------------
			 * print any extra lines associated with it
			 */
			if (op->opt_lines != 0)
			{
				const char ** l;

				for (l=op->opt_lines; *l; l++)
				{
					fprintf(fp, "  %-16s  %-12s  %-16s %s\n",
						"", "", "", *l);
				}
			}
		}

		break;
	}
}
