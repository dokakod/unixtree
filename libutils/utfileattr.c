/*------------------------------------------------------------------------
 * process fileattr files
 *
 * entries:
 *
 *	atime		{<=|<|=|!=|>|>=}nnn		{attr|+|-}
 *	ctime		{<=|<|=|!=|>|>=}nnn		{attr|+|-}
 *	mtime		{<=|<|=|!=|>|>=}nnn		{attr|+|-}
 *	inode		{<=|<|=|!=|>|>=}nnn		{attr|+|-}
 *	links		{<=|<|=|!=|>|>=}nnn		{attr|+|-}
 *	size		{<=|<|=|!=|>|>=}nnn		{attr|+|-}
 *	perms		ugsrwxrwxrwx			{attr|+|-}
 *	owner		[-]{nnn|aaa}			{attr|+|-}
 *	group		[-]{nnn|aaa}			{attr|+|-}
 *	type		[-]{r|b|c|d|l|p|f|s|u}	{attr|+|-}
 *	filename	[-]<file-spec>			{attr|+|-}
 *	directory	[-]<path-name>			{attr|+|-}
 */
#include "utcommon.h"

/*------------------------------------------------------------------------
 * keyword structs
 */
struct kwds
{
	int		type;
	const char *	name;
};
typedef struct kwds KWDS;

static const KWDS spec_kwds[] =
{
	{ FA_ATTR_ATIME,	"atime" },
	{ FA_ATTR_CTIME,	"ctime" },
	{ FA_ATTR_MTIME,	"mtime" },
	{ FA_ATTR_INODE,	"inode" },
	{ FA_ATTR_LINKS,	"links" },
	{ FA_ATTR_SIZE,		"size" },
	{ FA_ATTR_PERMS,	"perms" },
	{ FA_ATTR_OWNER,	"owner" },
	{ FA_ATTR_GROUP,	"group" },
	{ FA_ATTR_TYPE,		"type" },
	{ FA_ATTR_NAME,		"filename" },
	{ FA_ATTR_DIR,		"directory" },

	{ 0, 0 }
};

static const KWDS type_kwds[] =
{
	{ FA_TYPE_REG,		"reg" },
	{ FA_TYPE_CHR,		"chr" },
	{ FA_TYPE_BLK,		"blk" },
	{ FA_TYPE_LINK,		"link" },
	{ FA_TYPE_FIFO,		"fifo" },
	{ FA_TYPE_PIPE,		"pipe" },
	{ FA_TYPE_SOCK,		"sock" },
	{ FA_TYPE_DOOR,		"door" },
	{ FA_TYPE_UNK,		"unknown" },

	{ 0, 0 }
};

static const KWDS comp_kwds[] =
{
	{ FA_COMP_LE,		"<=" },
	{ FA_COMP_LT,		"<"  },
	{ FA_COMP_GE,		">=" },
	{ FA_COMP_GT,		">"  },
	{ FA_COMP_EQ,		"="  },
	{ FA_COMP_EQ,		"==" },
	{ FA_COMP_NE,		"!=" },

	{ 0, 0 }
};

/*------------------------------------------------------------------------
 * find a kwd entry by name
 */
static const KWDS *
find_kwd_by_name (const KWDS *kwds, const char *name, int first)
{
	const KWDS *k;

	for (k=kwds; k->type; k++)
	{
		if (first)
		{
			if (tolower(*name) == *k->name)
				return (k);
		}
		else
		{
			if (strccmp(name, k->name) == 0)
				return (k);
		}
	}

	return (0);
}

/*------------------------------------------------------------------------
 * find a kwd entry by type
 */
static const KWDS *
find_kwd_by_type (const KWDS *kwds, int type)
{
	const KWDS *k;

	for (k=kwds; k->type; k++)
	{
		if (type == k->type)
			return (k);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * load list from an attr file
 */
BLIST *
fileattr_load (const char *path, int *errs)
{
	FILE *		fp;
	BLIST *		flist	= 0;
	int			linecnt	= 0;
	int			err_cnt	= 0;

	if (path == 0 || *path == 0)
	{
		if (errs != 0)
			*errs = 1;
		return (0);
	}

	if (strcmp(path, "-") == 0)
	{
		fp = stdin;
	}
	else
	{
		fp = fopen(path, "r");
		if (fp == 0)
		{
			if (errs != 0)
				*errs = 1;
			return (0);
		}
	}

	while (TRUE)
	{
		BLIST *			b;
		FILEATTR		fa;
		FILEATTR *		pfa;
		const KWDS *	k;
		char			line[BUFSIZ];
		char *			np;
		char *			dp;
		const char *	ap;
		char *			l;
		int				n;

		/*----------------------------------------------------------------
		 * get next line & skip blank/comment lines
		 */
		if (fgets(line, sizeof(line), fp) == 0)
			break;
		strip(line);

		linecnt++;

		if (*line == 0 || *line == '#')
			continue;

		/*----------------------------------------------------------------
		 * breakup line into parts
		 */
		np = line;
		for (l=np; *l; l++)
		{
			if (isspace(*l))
				break;
		}
		if (*l == 0)
		{
			if (errs != 0)
			{
				fprintf(stderr, "fc line %d: only one field\n", linecnt);
			}
			err_cnt++;
			continue;
		}
		*l++ = 0;

		for (; *l; l++)
		{
			if (! isspace(*l))
				break;
		}
		if (*l == 0)
		{
			if (errs != 0)
			{
				fprintf(stderr, "fc line %d: only one field\n", linecnt);
			}
			err_cnt++;
			continue;
		}
		dp = l;

		for (l=dp; *l; l++)
		{
			if (isspace(*l))
				break;
		}
		if (*l == 0)
		{
			if (errs != 0)
			{
				fprintf(stderr, "fc line %d: only two fields\n", linecnt);
			}
			err_cnt++;
			continue;
		}
		*l++ = 0;

		for (; *l; l++)
		{
			if (! isspace(*l))
				break;
		}
		if (*l == 0)
		{
			if (errs != 0)
			{
				fprintf(stderr, "fc line %d: only two fields\n", linecnt);
			}
			err_cnt++;
			continue;
		}
		ap = l;

		for (; *l; l++)
		{
			if (isspace(*l))
				break;
		}
		*l = 0;

		/*----------------------------------------------------------------
		 * check for valid spec
		 */
		k = find_kwd_by_name(spec_kwds, np, TRUE);
		if (k == 0)
		{
			if (errs != 0)
			{
				fprintf(stderr, "fc line %d: invalid type %s\n", linecnt, np);
			}
			err_cnt++;
			continue;
		}

		fa.type = k->type;

		/*----------------------------------------------------------------
		 * evaluate attr
		 */
		if (strcmp(ap, "-") == 0)
		{
			fa.more = FALSE;
			fa.attr = A_UNSET;
		}
		else if (strcmp(ap, "+") == 0)
		{
			fa.more = TRUE;
			fa.attr = A_UNSET;
		}
		else
		{
			attr_t funny = 0;

			fa.more = FALSE;

			if (strcmp(ap, "rainbow") == 0)
			{
				ap = "*0";
			}
			else if (strcmp(ap, "random") == 0)
			{
				funny = FA_RANDOM;
				ap = "0";
			}

			if (*ap == '*')
			{
				funny = FA_RAINBOW;
				ap++;
			}

			if (isdigit(*ap))
			{
				n = atoi(ap);

				if (n >= NUM_COLORS)
				{
					if (errs != 0)
					{
						fprintf(stderr,
							"fc line %d: invalid color number %d\n",
							linecnt, n);
					}
					err_cnt++;
					continue;
				}
			}
			else
			{
				n = get_fg_num_by_name(ap);

				if (n < 0)
				{
					if (errs != 0)
					{
						fprintf(stderr,
							"fc line %d: invalid color name %s\n",
							linecnt, ap);
					}
					err_cnt++;
					continue;
				}
			}

			n = get_fg_value_by_num(n);
			fa.attr = A_FG_CLRVAL(n) | funny;
		}

		/*----------------------------------------------------------------
		 * evaluate data part
		 */
		switch (fa.type)
		{
		case FA_ATTR_ATIME:
		case FA_ATTR_CTIME:
		case FA_ATTR_MTIME:
		case FA_ATTR_INODE:
		case FA_ATTR_LINKS:
		case FA_ATTR_SIZE:
			{
				for (k=comp_kwds; k->type; k++)
				{
					n = strlen(k->name);

					if (strncmp(k->name, dp, n) == 0)
						break;
				}
				if (k->type == 0)
				{
					if (errs != 0)
					{
						fprintf(stderr,
							"fc line %d: invalid comparison op %s\n",
							linecnt, dp);
					}
					err_cnt++;
					continue;
				}

				fa.data.nums.comp = k->type;

				dp += n;
				fa.data.nums.data = atox(dp);
			}
			break;

		case FA_ATTR_PERMS:
			{
				int bit = 1;

				fa.data.perms.and_mask = 0;
				fa.data.perms.or_mask  = 0;

				n = strlen(dp);
				if (n > 12)
				{
					if (errs != 0)
					{
						fprintf(stderr,
							"fc line %d: too many bits (%d) specified\n",
							linecnt, n);
					}
					err_cnt++;
					continue;
				}

				for (n--; n>=0; n--)
				{
					switch (dp[n])
					{
					case '0':
						fa.data.perms.and_mask |=  bit;
						fa.data.perms.or_mask  &= ~bit;
						break;

					case '1':
						fa.data.perms.and_mask |=  bit;
						fa.data.perms.or_mask  |=  bit;
						break;

					default:
						fa.data.perms.and_mask &= ~bit;
						fa.data.perms.or_mask  &= ~bit;
						break;
					}

					bit <<= 1;
				}
			}
			break;

		case FA_ATTR_OWNER:
			{
				if (*dp == '-')
				{
					fa.data.nums.comp = FA_COMP_NE;
					dp++;
				}
				else
				{
					fa.data.nums.comp = FA_COMP_EQ;
				}

				if (isdigit(*dp))
				{
					n = atoi(dp);
				}
				else
				{
					n = os_get_usr_id_from_name(dp);
					if (n < 0)
					{
						if (errs != 0)
						{
							fprintf(stderr,
								"fc line %d: invalid owner name %s\n",
								linecnt, dp);
						}
						err_cnt++;
						continue;
					}
				}
				fa.data.nums.data = n;
			}
			break;

		case FA_ATTR_GROUP:
			{
				if (*dp == '-')
				{
					fa.data.nums.comp = FA_COMP_NE;
					dp++;
				}
				else
				{
					fa.data.nums.comp = FA_COMP_EQ;
				}

				if (isdigit(*dp))
				{
					n = atoi(dp);
				}
				else
				{
					n = os_get_grp_id_from_name(dp);
					if (n < 0)
					{
						if (errs != 0)
						{
							fprintf(stderr,
								"fc line %d: invalid group name %s\n",
								linecnt, dp);
						}
						err_cnt++;
						continue;
					}
				}
				fa.data.nums.data = n;
			}
			break;

		case FA_ATTR_TYPE:
			{
				if (*dp == '-')
				{
					fa.data.types.allb = TRUE;
					dp++;
				}
				else
				{
					fa.data.types.allb = FALSE;
				}

				k = find_kwd_by_name(type_kwds, dp, TRUE);
				if (k == 0)
				{
					if (errs != 0)
					{
						fprintf(stderr,
							"fc line %d: invalid file type %s\n",
							linecnt, dp);
					}
					err_cnt++;
					continue;
				}

				fa.data.types.type = k->type;
				switch (k->type)
				{
				case FA_TYPE_REG:	fa.data.types.mode = S_IFREG;	break;
				case FA_TYPE_CHR:	fa.data.types.mode = S_IFCHR;	break;
				case FA_TYPE_BLK:	fa.data.types.mode = S_IFBLK;	break;
				case FA_TYPE_LINK:	fa.data.types.mode = S_IFLNK;	break;
				case FA_TYPE_FIFO:	fa.data.types.mode = S_IFIFO;	break;
				case FA_TYPE_PIPE:	fa.data.types.mode = S_IFNAM;	break;
				case FA_TYPE_SOCK:	fa.data.types.mode = S_IFSOCK;	break;
				case FA_TYPE_UNK:	fa.data.types.mode = S_IFLNK;	break;
				default:			continue;
				}
			}
			break;

		case FA_ATTR_NAME:
		case FA_ATTR_DIR:
			{
				if (*dp == '-')
				{
					fa.data.names.allb = TRUE;
					dp++;
				}
				else
				{
					fa.data.names.allb = FALSE;
				}

				strcpy(fa.data.names.spec, dp);
			}
			break;
		}

		/*----------------------------------------------------------------
		 * got a valid entry.  save it in the list
		 */
		pfa = (FILEATTR *)MALLOC(sizeof(*pfa));
		if (pfa == 0)
		{
			if (errs != 0)
			{
				fprintf(stderr,
					"fc line %d: cannot allocate entry\n",
					linecnt);
			}
			err_cnt++;
			continue;
		}
		memcpy(pfa, &fa, sizeof(*pfa));

		b = BNEW(pfa);
		if (b == 0)
		{
			if (errs != 0)
			{
				fprintf(stderr,
					"fc line %d: cannot allocate list entry\n",
					linecnt);
			}
			err_cnt++;
			FREE(pfa);
			continue;
		}

		flist = bappend(flist, b);
	}

	if (fp != stdin)
		fclose(fp);

	if (errs != 0)
		*errs = err_cnt;

	return (flist);
}

/*------------------------------------------------------------------------
 * save a list to an attr file
 */
int
fileattr_save (const char *path, BLIST *flist)
{
	FILE *	fp;
	BLIST *	l;

	if (path == 0 || *path == 0)
	{
		return (-1);
	}

	if (strcmp(path, "-") == 0)
	{
		fp = stdout;
	}
	else
	{
		fp = fopen(path, "w");
		if (fp == 0)
			return (-1);
	}

	for (l=flist; l; l=bnext(l))
	{
		FILEATTR *		fa		= (FILEATTR *)bid(l);
		const KWDS *	spec	= find_kwd_by_type(spec_kwds, fa->type);
		const KWDS *	subk;
		const char *	name	= spec->name;
		const char *	id;
		char			id_buf[24];
		char			line[MAX_PATHLEN];
		int				i;

		switch (fa->type)
		{
		case FA_ATTR_ATIME:
		case FA_ATTR_CTIME:
		case FA_ATTR_MTIME:
		case FA_ATTR_INODE:
		case FA_ATTR_LINKS:
		case FA_ATTR_SIZE:
			{
				id = "";
				i = fa->data.nums.data;

				if ((i % (1024 * 1024)) == 0)
				{
					i /= (1024 * 1024);
					id = "m";
				}
				else if ((i % 1024) == 0)
				{
					i /= 1024;
					id = "k";
				}

				subk = find_kwd_by_type(comp_kwds, fa->data.nums.comp);
				sprintf(line, "%-16s %s%d%s",
					name,
					subk->name,
					i, id);
			}
			break;

		case FA_ATTR_PERMS:
			{
				sprintf(line, "%-16s ",
					name);
				for (i=11; i>=0; i--)
				{
					int bit = 1 << i;

					if (fa->data.perms.and_mask & bit)
					{
						if (fa->data.perms.or_mask & bit)
							strcat(line, "1");
						else
							strcat(line, "0");
					}
					else
					{
						strcat(line, "-");
					}
				}
			}
			break;

		case FA_ATTR_OWNER:
			{
				id = os_get_usr_name_from_id(fa->data.nums.data, id_buf);
				if (id == 0)
				{
					sprintf(line, "%-16s %s%d",
						name,
						(fa->data.nums.comp == FA_COMP_EQ ? "" : "-"),
						fa->data.nums.data);
				}
				else
				{
					sprintf(line, "%-16s %s%s",
						name,
						(fa->data.nums.comp == FA_COMP_EQ ? "" : "-"),
						id);
				}
			}
			break;

		case FA_ATTR_GROUP:
			{
				id = os_get_grp_name_from_id(fa->data.nums.data, id_buf);
				if (id == 0)
				{
					sprintf(line, "%-16s %s%d",
						name,
						(fa->data.nums.comp == FA_COMP_EQ ? "" : "-"),
						fa->data.nums.data);
				}
				else
				{
					sprintf(line, "%-16s %s%s",
						name,
						(fa->data.nums.comp == FA_COMP_EQ ? "" : "-"),
						id);
				}
			}
			break;

		case FA_ATTR_TYPE:
			{
				subk = find_kwd_by_type(type_kwds, fa->data.types.type);
				sprintf(line, "%-16s %s%s",
					name,
					(fa->data.types.allb ? "-" : ""),
					subk->name);
			}
			break;

		case FA_ATTR_NAME:
		case FA_ATTR_DIR:
			{
				sprintf(line, "%-16s %s%s",
					name,
					(fa->data.names.allb ? "-" : ""),
					fa->data.names.spec);
			}
			break;

		default:
			continue;
		}

		strcat(line, " ");
		for (i = strlen(line); i<32; i++)
			strcat(line, " ");

		if (fa->more)
		{
			strcat(line, "+");
		}
		else
		{
			if (fa->attr == A_UNSET)
			{
				strcat(line, "-");
			}
			else
			{
				if (fa->attr & FA_RANDOM)
				{
					strcat(line, "random");
				}
				else if (fa->attr & FA_RAINBOW)
				{
					if (A_FG_CLRNUM(fa->attr) == COLOR_BLACK)
					{
						strcat(line, "rainbow");
					}
					else
					{
						strcat(line, "*");
						strcat(line,
							get_fg_name_by_num(A_FG_CLRNUM(fa->attr)));
					}
				}
				else
				{
					strcat(line, get_fg_name_by_num(A_FG_CLRNUM(fa->attr)));
				}
			}
		}

		fprintf(fp, "%s\n", line);
	}

	if (fp != stdout)
		fclose(fp);

	return (0);
}

/*------------------------------------------------------------------------
 * free a list
 */
BLIST *
fileattr_free (BLIST *flist)
{
	return BSCRAP(flist, TRUE);
}
