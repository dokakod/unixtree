/*------------------------------------------------------------------------
 * error msg routines
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * errmsg names & descriptions
 */
struct err_msg
{
	int				code;
	const char *	name;
	const char *	desc;
};
typedef struct err_msg ERR_MSG;

static const ERR_MSG tcap_err_msgs[] =
{
	{ TCAP_ERR_NONE,		"NONE",			"no error"					},
	{ TCAP_ERR_NOMEM,		"NOMEM",		"no memory"					},
	{ TCAP_ERR_NOTERM,		"NOTERM",		"no term variable"			},
	{ TCAP_ERR_CANTOPEN,	"CANTOPEN",		"can't open term/file"		},
	{ TCAP_ERR_NODBS,		"NODBS",		"database not specified"	},
	{ TCAP_ERR_NOTFND,		"NOTFND",		"no entry found for TERM"	},

	{ TCAP_ERR_NOCM,		"NOCM",			"no CM string found"		},
	{ TCAP_ERR_NOLINES,		"NOLINES",		"no LINES found"			},
	{ TCAP_ERR_NOCOLS,		"NOCOLS",		"no COLS found"				},
	{ TCAP_ERR_NOFG,		"NOFG",			"missing FG entry"			},
	{ TCAP_ERR_NOBG,		"NOBG",			"missing BG entry"			},

	{ TCAP_ERR_KEY_DUP,		"KEY_DUP",		"dup key definition"		},
	{ TCAP_ERR_KEY_PART,	"KEY_PART",		"partial string in def"		},
	{ TCAP_ERR_KEY_CONT,	"KEY_CONT",		"key contains other key"	},

	{ TCAP_ERR_UNKNOWN,		"UNKNOWN",		"unknown term-file entry"	},
	{ TCAP_ERR_NOTERMFILE,	"NOTERMFILE",	"no term-file found"		},
	{ TCAP_ERR_CYCLIC,		"CYCLIC",		"cyclic TERM references"	},
	{ TCAP_ERR_INV_SYNTAX,	"INV_SYNTAX",	"invalid syntax in entry"	},
	{ TCAP_ERR_INV_KEYWRD,	"INV_KEYWRD",	"invalid keyword in entry"	},
	{ TCAP_ERR_INV_DATA,	"INV_DATA",		"invalid data in entry"		},

	{ -1, 0, 0 }
};

/*------------------------------------------------------------------------
 * errmsg level names
 */
static const char * tcap_errmsg_lvls[] = { "?", "I", "W", "E" };

/*------------------------------------------------------------------------
 * tcap_errmsg_init() - initialize a struct
 */
void tcap_errmsg_init (TCAP_ERRS *te)
{
	if (te != 0)
	{
		te->msgs	= 0;
		te->num		= 0;
		te->max_lvl	= TCAP_ERR_L_NONE;
	}
}

/*------------------------------------------------------------------------
 * tcap_errmsg_clear() - clear a struct
 */
void tcap_errmsg_clear (TCAP_ERRS *te)
{
	if (te != 0)
	{
		TCAP_ERRMSG *	tl;
		TCAP_ERRMSG *	tn;

		for (tl=te->msgs; tl; tl=tn)
		{
			tn = tl->next;
			FREE(tl);
		}

		te->msgs	= 0;
		te->num		= 0;
		te->max_lvl	= TCAP_ERR_L_NONE;
	}
}

/*------------------------------------------------------------------------
 * tcap_errmsg_add() - add an entry to the list
 */
void tcap_errmsg_add (TCAP_ERRS *te, TCAP_ERRMSG *tm)
{
	if (te != 0 && tm != 0)
	{
		tm->next = 0;
		if (tm->level > TCAP_ERR_L_FATAL)
			tm->level = TCAP_ERR_L_FATAL;

		te->num++;
		if (tm->level > te->max_lvl)
			te->max_lvl = tm->level;

		if (te->msgs == 0)
		{
			te->msgs = tm;
		}
		else
		{
			TCAP_ERRMSG *	t;

			for (t=te->msgs; t->next; t=t->next)
				;
			t->next = tm;
		}
	}
}

/*------------------------------------------------------------------------
 * tcap_errmsg_add_fmt() - add a msg ala fprintf()
 */
void tcap_errmsg_add_fmt (TCAP_ERRS *te, int code, int level,
	const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	tcap_errmsg_add_var(te, code, level, fmt, args);
	va_end(args);
}

/*------------------------------------------------------------------------
 * tcap_errmsg_add_var() - add a msg ala vfprintf()
 */
void tcap_errmsg_add_var (TCAP_ERRS *te, int code, int level,
	const char *fmt, va_list args)
{
	TCAP_ERRMSG		tl;
	TCAP_ERRMSG *	tm;

	/*--------------------------------------------------------------------
	 * create the actual message
	 */
	vsprintf(tl.msg, fmt, args);

	/*--------------------------------------------------------------------
	 * now fill in rest of the entry
	 */
	tl.code		= code;
	tl.level	= level;

	/*--------------------------------------------------------------------
	 * If struct is not NULL, allocate an entry & add it to list
	 */
	tm = 0;
	if (te != 0)
	{
		tm = (TCAP_ERRMSG *)MALLOC(sizeof(*tm));
		if (tm != 0)
		{
			memcpy(tm, &tl, sizeof(*tm));
			tcap_errmsg_add(te, tm);
		}
	}

	/*--------------------------------------------------------------------
	 * At this point, if "te" is NULL or "tm" is null, we didn't
	 * add an entry to the list, so just print it.
	 */
	if (te == 0 || tm == 0)
		tcap_errmsg_print_entry(&tl, stderr, FALSE);
}

/*------------------------------------------------------------------------
 * tcap_errmsg_print_list() - print an error msg list
 */
void tcap_errmsg_print_list (const TCAP_ERRS *te, FILE *fp, int detail)
{
	if (te != 0)
	{
		const TCAP_ERRMSG *	tm;

		for (tm=te->msgs; tm; tm=tm->next)
			tcap_errmsg_print_entry(tm, fp, detail);
	}
}

/*------------------------------------------------------------------------
 * tcap_errmsg_print_entry() - print an error msg entry
 */
void tcap_errmsg_print_entry (const TCAP_ERRMSG *tm, FILE *fp, int detail)
{
	if (tm != 0 && fp != 0)
	{
		if (detail)
		{
			fprintf(fp, "%-10s:%s: %s\n",
				tcap_errmsg_name(tm->code),
				tcap_errmsg_lvls[tm->level],
				tm->msg);
		}
		else
		{
			fputs(tm->msg, fp);
			fputc('\n', fp);
		}

		fflush(fp);
	}
}

/*------------------------------------------------------------------------
 * tcap_errmsg_name() - get error code name
 * tcap_errmag_desc() - get error code description
 */
const char * tcap_errmsg_name (int code)
{
	const ERR_MSG *	e;

	for (e=tcap_err_msgs; e->code >= 0; e++)
	{
		if (e->code == code)
			return (e->name);
	}

	return ("?");
}

const char * tcap_errmsg_desc (int code)
{
	const ERR_MSG *	e;

	for (e=tcap_err_msgs; e->code >= 0; e++)
	{
		if (e->code == code)
			return (e->desc);
	}

	return ("?");
}
