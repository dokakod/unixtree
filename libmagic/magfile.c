/*
 * apprentice - make one pass through /etc/magic, learning its secrets.
 *
 * Copyright (c) Ian F. Darwin, 1987.
 * Written by Ian F. Darwin.
 *
 * This software is not subject to any license of the American Telephone
 * and Telegraph Company or of the Regents of the University of California.
 *
 * Permission is granted to anyone to use this software for any purpose on
 * any computer system, and to alter it and redistribute it freely, subject
 * to the following restrictions:
 *
 * 1. The author is not responsible for the consequences of use of this
 *    software, no matter how awful, even if they arise from flaws in it.
 *
 * 2. The origin of this software must not be misrepresented, either by
 *    explicit claim or by omission.  Since few users ever read sources,
 *    credits must appear in the documentation.
 *
 * 3. Altered versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.  Since few users
 *    ever read sources, credits must appear in the documentation.
 *
 * 4. This notice may not be removed or altered.
 */

#include "magcommon.h"
#include <stdarg.h>

#define	EATWS		{ \
				while (isascii((unsigned char) *l) && \
				       isspace((unsigned char) *l)) \
					l++; \
			}

#define LOWCASE(c)	(isupper((unsigned char) (c)) ? \
				tolower((unsigned char) (c)) : (c))


static MAGIC *	mag_parse_1	(MAGIC *magics, const char *magfile, int check);
static MAGIC *	parse		(MAGIC *magics, const char *magfile, int lineno,
								char *line, int check);
static int		getvalue	(MAGIC *, char **);
static int		hextoint	(int);
static char *	getstr		(char *, char *, int, int *);
static void		eatsize		(char **);

static void
magwarn (const char *file, int lineno, const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	fprintf(stdout, "%s, %d: ", file, lineno);
	vfprintf(stdout, fmt, va);
	va_end(va);
	fputc('\n', stdout);
}

/*------------------------------------------------------------------------
 * free magic list
 */
void
mag_free (MAGIC *magics)
{
	MAGIC *	m;
	MAGIC *	n;

	for (m=magics; m; m=n)
	{
		n = m->next;

		FREE(m);
	}
}

/*------------------------------------------------------------------------
 * parse all magic files into linked-list
 */
MAGIC *
mag_parse (const char *mag_path, int check)
{
	MAGIC *			magics = 0;
	char			magfile[256];
	const char *	p;
	char *			mfn;
	int				path_sep_char;

	/*--------------------------------------------------------------------
	 * determine the path separator char
	 */
	p = strchr(mag_path, ';');
	path_sep_char = (p != 0 ? ';' : ':');

	/*--------------------------------------------------------------------
	 * parse each filename in path
	 */
	for (p=mag_path; *p; p++)
	{
		/*----------------------------------------------------------------
		 * get next filename in path
		 */
		mfn = magfile;
		for (; *p; p++)
		{
			if (*p == path_sep_char)
				break;
			*mfn++ = *p;
		}
		*mfn = 0;

		/*----------------------------------------------------------------
		 * parse this file
		 */
		magics = mag_parse_1(magics, magfile, check);

		/*----------------------------------------------------------------
		 * break if last file in path
		 */
		if (*p == 0)
			break;
	}

	return (magics);
}

/*------------------------------------------------------------------------
 * parse one magic file into linked-list
 */
static MAGIC *
mag_parse_1 (MAGIC *magics, const char *magfile, int check)
{
	FILE *			fp;
	int				lineno;

	/*--------------------------------------------------------------------
	 * open the file
	 */
	fp = fopen(magfile, "r");
	if (fp == 0)
	{
		return (magics);
	}

	/*--------------------------------------------------------------------
	 * parse it
	 */
	for (lineno = 1; ; lineno++)
	{
		char	line[BUFSIZ];
		char *	lp;

		/*----------------------------------------------------------------
		 * read in next line
		 */
		lp = fgets(line, sizeof(line), fp);
		if (lp == 0)
			break;

		/*----------------------------------------------------------------
		 * strip newline at end
		 */
		lp[strlen(lp)-1] = 0;

		/*----------------------------------------------------------------
		 * skip comments & blank lines
		 */
		if (*lp == 0)
			continue;

		if (*lp == '#')
		{
			/*------------------------------------------------------------
			 * use entry if just commented out (This is to access ELF entries
			 * in /etc/magic which are commented out)
			 */
			if (lp[1] == '>' || lp[1] == '&' || isdigit(lp[1]))
				lp++;
			else
				continue;
		}

		/*----------------------------------------------------------------
		 * parse it
		 */
		magics = parse(magics, magfile, lineno, lp, check);
	}

	fclose(fp);

	return (magics);
}

/*------------------------------------------------------------------------
 * extend the sign bit if the comparison is to be signed
 */
unsigned int
mag_signex (MAGIC *m, unsigned int v)
{
	if (! (m->flag & UNSIGNED))
	{
		switch (m->type)
		{
		/*
		 * Do not remove the casts below.  They are
		 * vital.  When later compared with the data,
		 * the sign extension must have happened.
		 */
		case BYTE:
			v = (char) v;
			break;
		case SHORT:
		case BESHORT:
		case LESHORT:
			v = (short) v;
			break;
		case LONG:
		case BELONG:
		case LELONG:
			v = (int) v;
			break;
		case STRING:
			break;
		default:
			v = (unsigned int)-1;
			break;
		}
	}

	return v;
}

/*
 * parse one line from magic file, put into magic[index++] if valid
 */
static MAGIC *
parse (MAGIC *magics, const char *magfile, int lineno,
	char *l, int check)
{
	MAGIC	mt;
	MAGIC *	m;
	char *	t;
	char *	s;
	int		i = 0;

	/*--------------------------------------------------------------------
	 * clear temp magic entry
	 */
	m = &mt;
	memset(m, 0, sizeof(*m));

	/*--------------------------------------------------------------------
	 * get cont-level & flags
	 */
	if (*l == '&')
	{
		l++;
		m->flag |= ALSO;
	}
	else
	{
		while (*l == '>')
		{
			l++;
			m->cont_level++;
		}

		if (m->cont_level != 0 && *l == '(')
		{
			l++;
			m->flag |= INDIR;
		}

		if (m->cont_level != 0 && *l == '&')
		{
			l++;
			m->flag |= ADD;
		}
	}

	/*--------------------------------------------------------------------
	 * get offset, then skip over it
	 */
	m->offset = (int) strtoul(l, &t, 0);
	if (l == t)
	{
		if (check)
			magwarn(magfile, lineno, "offset %s invalid", l);
	}
	l = t;

	if (m->flag & INDIR)
	{
		m->in.type = LONG;
		m->in.offset = 0;

		/*
		 * read [.lbs][+-]nnnnn)
		 */
		if (*l == '.')
		{
			l++;
			switch (*l)
			{
			case 'l':
				m->in.type = LELONG;
				break;
			case 'L':
				m->in.type = BELONG;
				break;
			case 'h':
			case 's':
				m->in.type = LESHORT;
				break;
			case 'H':
			case 'S':
				m->in.type = BESHORT;
				break;
			case 'c':
			case 'b':
			case 'C':
			case 'B':
				m->in.type = BYTE;
				break;
			default:
				if (check)
				{
					magwarn(magfile, lineno,
						"indirect offset type %c invalid", *l);
				}
				break;
			}
			l++;
		}

		s = l;
		if (*l == '+' || *l == '-') l++;
		if (isdigit((unsigned char)*l))
		{
			m->in.offset = strtoul(l, &t, 0);
			if (*s == '-') m->in.offset = - m->in.offset;
		}
		else
		{
			t = l;
		}

		if (*t++ != ')')
		{
			if (check)
				magwarn(magfile, lineno, "missing ')' in indirect offset");
		}

		l = t;
	}


	while (isascii((unsigned char)*l) && isdigit((unsigned char)*l))
		++l;
	EATWS;

#define NCHAR		4
#define NBYTE		4
#define NSTRING 	6
#define NSHORT		5
#define NBESHORT	7
#define NLESHORT	7
#define NLONG		4
#define NBELONG		6
#define NLELONG		6

	if (*l == 'u')
	{
		++l;
		m->flag |= UNSIGNED;
	}

	/* get type, skip it */
	if (strncmp(l, "byte", NBYTE)==0 ||
	    strncmp(l, "char", NCHAR)==0)
	{
		m->type = BYTE;
		l += NBYTE;
	}
	else if (strncmp(l, "short", NSHORT)==0)
	{
		m->type = SHORT;
		l += NSHORT;
	}
	else if (strncmp(l, "long", NLONG)==0)
	{
		m->type = LONG;
		l += NLONG;
	}
	else if (strncmp(l, "string", NSTRING)==0)
	{
		m->type = STRING;
		l += NSTRING;
	}
	else if (strncmp(l, "beshort", NBESHORT)==0)
	{
		m->type = BESHORT;
		l += NBESHORT;
	}
	else if (strncmp(l, "belong", NBELONG)==0)
	{
		m->type = BELONG;
		l += NBELONG;
	}
	else if (strncmp(l, "leshort", NLESHORT)==0)
	{
		m->type = LESHORT;
		l += NLESHORT;
	}
	else if (strncmp(l, "lelong", NLELONG)==0)
	{
		m->type = LELONG;
		l += NLELONG;
	}
	else
	{
		if (check)
			magwarn(magfile, lineno, "type %s invalid", l);
		return (magics);
	}

	/* New-style anding: "0 byte&0x80 =0x80 dynamically linked" */
	if (*l == '&')
	{
		++l;
		m->mask = mag_signex(m, strtoul(l, &l, 0));
		eatsize(&l);
	}
	else
	{
		m->mask = ~0L;
	}
	EATWS;

	if (m->type == STRING)
	{
		m->reln = '=';
	}
	else
	{
		switch (*l)
		{
		case '>':
		case '<':
			/* Old-style anding: "0 byte &0x80 dynamically linked" */
		case '&':
		case '^':
		case '=':
			m->reln = *l;
			++l;
			break;
		case '!':
			m->reln = *l;
			++l;
			break;
		default:
			if (*l == 'x' && isascii((unsigned char)l[1]) &&
			    isspace((unsigned char)l[1]))
			{
				m->reln = *l;
				++l;
				goto GetDesc;
			}
			m->reln = '=';
			break;
		}
	}
	EATWS;

	if (getvalue(m, &l))
		return (magics);
	/*
	 * TODO finish this macro and start using it!
	 * #define offsetcheck {if (offset > HOWMANY-1)
	 *	magwarn(magfile, "offset too big"); }
	 */

	/*
	 * now get last part - the description
	 */
GetDesc:
	EATWS;
	if (l[0] == '\b')
	{
		++l;
		m->nospflag = 1;
	}
	else if ((l[0] == '\\') && (l[1] == 'b'))
	{
		++l;
		++l;
		m->nospflag = 1;
	}
	else
	{
		m->nospflag = 0;
	}

	while ((m->desc[i++] = *l++) != '\0' && i<MAXDESC)
		/* NULLBODY */;

	if (check)
	{
		mag_dump(m);
	}
	else
	{
		/*----------------------------------------------------------------
		 * add this entry to list
		 */
		m = (MAGIC *)MALLOC(sizeof(*m));
		if (m != 0)
		{
			memcpy(m, &mt, sizeof(*m));

			if (magics == 0)
			{
				magics = m;
			}
			else
			{
				MAGIC *	mp;

				for (mp=magics; mp->next; mp=mp->next)
					;
				mp->next = m;
			}
		}
	}

	return (magics);
}

/*
 * Read a numeric value from a pointer, into the value union of a magic
 * pointer, according to the magic type.  Update the string pointer to point
 * just after the number read.  Return 0 for success, non-zero for failure.
 */
static int
getvalue(MAGIC *m, char **p)
{
	int slen;

	if (m->type == STRING)
	{
		*p = getstr(*p, m->value.s, sizeof(m->value.s), &slen);
		m->vallen = slen;
	}
	else
	{
		if (m->reln != 'x')
		{
			m->value.l = mag_signex(m, strtoul(*p, p, 0));
			eatsize(p);
		}
	}

	return 0;
}

/*
 * Convert a string containing C character escapes.  Stop at an unescaped
 * space or tab.
 * Copy the converted version to "p", returning its length in *slen.
 * Return updated scan pointer as function result.
 */
static char *
getstr(char *s, char *p, int plen, int *slen)
{
	char	*origp = p;
	char	*pmax = p + plen - 1;
	register int	c;
	register int	val;

	while ((c = *s++) != '\0')
	{
		if (isspace((unsigned char) c))
			break;

		if (p >= pmax)
		{
			break;
		}

		if (c == '\\')
		{
			switch(c = *s++)
			{
			case '\0':
				goto out;

			default:
				*p++ = (char) c;
				break;

			case 'n':
				*p++ = '\n';
				break;

			case 'r':
				*p++ = '\r';
				break;

			case 'b':
				*p++ = '\b';
				break;

			case 't':
				*p++ = '\t';
				break;

			case 'f':
				*p++ = '\f';
				break;

			case 'v':
				*p++ = '\v';
				break;

				/* \ and up to 3 octal digits */
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				val = c - '0';
				c = *s++;  /* try for 2 */
				if (c >= '0' && c <= '7')
				{
					val = (val<<3) | (c - '0');
					c = *s++;  /* try for 3 */
					if(c >= '0' && c <= '7')
						val = (val<<3) | (c-'0');
					else
						--s;
				}
				else
				{
					--s;
				}
				*p++ = (char)val;
				break;

				/* \x and up to 2 hex digits */
			case 'x':
				val = 'x';	/* Default if no digits */
				c = hextoint(*s++);	/* Get next char */
				if (c >= 0)
				{
					val = c;
					c = hextoint(*s++);
					if (c >= 0)
						val = (val << 4) + c;
					else
						--s;
				}
				else
				{
					--s;
				}
				*p++ = (char)val;
				break;
			}
		}
		else
		{
			*p++ = (char)c;
		}
	}
out:
	*p = '\0';
	*slen = p - origp;
	return s;
}


/* Single hex char to int; -1 if not a hex char. */
static int
hextoint(int c)
{
	if (!isascii((unsigned char) c))	return -1;
	if (isdigit((unsigned char) c))		return c - '0';
	if ((c>='a')&&(c<='f'))			return c + 10 - 'a';
	if ((c>='A')&&(c<='F'))			return c + 10 - 'A';
	return -1;
}

/*
 * eatsize(): Eat the size spec from a number [eg. 10UL]
 */
static void
eatsize (char **p)
{
	char *l = *p;

	if (LOWCASE(*l) == 'u')
		l++;

	switch (LOWCASE(*l))
	{
	case 'l':    /* long */
	case 's':    /* short */
	case 'h':    /* short */
	case 'b':    /* char/byte */
	case 'c':    /* char/byte */
		l++;
		/*FALLTHROUGH*/
	default:
		break;
	}

	*p = l;
}
