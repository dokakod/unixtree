/*
 * softmagic - interpret variable magic from /etc/magic
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

static int mget		(VALUETYPE *, unsigned char *, MAGIC *, int);
static int mcheck	(VALUETYPE *, MAGIC *);
static int mprint	(VALUETYPE *, MAGIC *, char *);
static int mconvert	(VALUETYPE *, MAGIC *);

/*
 * Go through the whole list, stopping if you find a match.  Process all
 * the continuations of that match before returning.
 *
 * We support multi-level continuations:
 *
 *	At any time when processing a successful top-level match, there is a
 *	current continuation level; it represents the level of the last
 *	successfully matched continuation.
 *
 *	Continuations above that level are skipped as, if we see one, it
 *	means that the continuation that controls them - i.e, the
 *	lower-level continuation preceding them - failed to match.
 *
 *	Continuations below that level are processed as, if we see one,
 *	it means we've finished processing or skipping higher-level
 *	continuations under the control of a successful or unsuccessful
 *	lower-level continuation, and are now seeing the next lower-level
 *	continuation and should process it.  The current continuation
 *	level reverts to the level of the one we're seeing.
 *
 *	Continuations at the current level are processed as, if we see
 *	one, there's no lower-level continuation that may have failed.
 *
 *	If a continuation matches, we bump the current continuation level
 *	so that higher-level continuations are processed.
 */
int
mag_soft (MAGIC *magics, unsigned char *s, int nbytes, char *result)
{
	MAGIC *		m;
	int			cont_level = 0;
	int			need_separator = 0;
	VALUETYPE	p;
	int			tmpoff[MAXOFFS];
	int			oldoff = 0;
	int			goto_next;

	for (m=magics; m; )
	{
		goto_next = 0;

		/* if main entry matches, print it... */
		if (m->next != 0 && (m->next->flag & ALSO) )
		{
			if (! mget(&p, s, m, nbytes) || ! mcheck(&p, m))
			{
				m = m->next;
				goto_next = 1;
			}
			else
			{
				VALUETYPE pa;

				if (! mget(&pa, s, m->next, nbytes) || ! mcheck(&pa, m->next))
				{
					m = m->next;
					goto_next = 1;
				}
			}
		}
		else
		{
			if (! mget(&p, s, m, nbytes) || ! mcheck(&p, m))
				goto_next = 1;
		}

		if (goto_next)
		{
				/*
				 * main entry didn't match,
				 * flush its continuations
				 */
				for (m=m->next; m; m=m->next)
				{
					if (m->cont_level == 0)
						break;
				}
				continue;
		}

		tmpoff[cont_level] = mprint(&p, m, result);

		/*
		 * If we printed something, we'll need to print
		 * a blank before we print something else.
		 */
		if (m->desc[0])
			need_separator = 1;
		/* and any continuations that match */
		cont_level++;

		if (m->next != 0 && (m->next->flag & ALSO) )
			m = m->next;

		for (m=m->next; m && m->cont_level; m=m->next)
		{
			if (cont_level >= m->cont_level)
			{
				if (cont_level > m->cont_level)
				{
					/*
					 * We're at the end of the level
					 * "cont_level" continuations.
					 */
					cont_level = m->cont_level;
				}

				if (m->flag & ADD)
				{
					oldoff=m->offset;
					m->offset += tmpoff[cont_level-1];
				}

				if (mget(&p, s, m, nbytes) && mcheck(&p, m))
				{
					/*
					 * This continuation matched.
					 * Print its message, with
					 * a blank before it if
					 * the previous item printed
					 * and this item isn't empty.
					 */

					/* space if previous printed */
					if (need_separator &&
					    (m->nospflag == 0) &&
					    (m->desc[0] != '\0')
					    )
					{
						strcat(result, " ");
						need_separator = 0;
					}

					tmpoff[cont_level] = mprint(&p, m, result);
					if (m->desc[0])
						need_separator = 1;

					/*
					 * If we see any continuations
					 * at a higher level,
					 * process them.
					 */
					cont_level++;
				}

				if (m->flag & ADD)
				{
					m->offset = oldoff;
				}
			}
		}
		return 1;		/* all through */
	}
	return 0;			/* no match at all */
}

static int
mprint (VALUETYPE *p, MAGIC *m, char *result)
{
	unsigned int v;
	int t = 0 ;
	char *r = result + strlen(result);

	switch (m->type)
	{
	case BYTE:
		v = p->b;
		v = mag_signex(m, v) & m->mask;
		sprintf(r, m->desc, (unsigned char) v);
		t = m->offset + sizeof(char);
		break;

	case SHORT:
	case BESHORT:
	case LESHORT:
		v = p->h;
		v = mag_signex(m, v) & m->mask;
		sprintf(r, m->desc, (unsigned short) v);
		t = m->offset + sizeof(short);
		break;

	case LONG:
	case BELONG:
	case LELONG:
		v = p->l;
		v = mag_signex(m, v) & m->mask;
		sprintf(r, m->desc, (unsigned int) v);
		t = m->offset + sizeof(int);
		break;

	case STRING:
		if (m->reln == '=')
		{
			sprintf(r, m->desc, m->value.s);
			t = m->offset + strlen(m->value.s);
		}
		else
		{
			if (*m->value.s == '\0')
			{
				char *cp = strchr(p->s, '\n');
				if (cp)
					*cp = '\0';
			}
			sprintf(r, m->desc, p->s);
			t = m->offset + strlen(p->s);
		}
		break;

	default:
		break;
	}

	return(t);
}

/*
 * Convert the byte order of the data we are looking at
 */
static int
mconvert (VALUETYPE *p, MAGIC *m)
{
	switch (m->type)
	{
	case BYTE:
	case SHORT:
	case LONG:
		return 1;
	case STRING:
		{
			char *ptr;

			/* Null terminate and eat the return */
			p->s[sizeof(p->s) - 1] = '\0';
			if ((ptr = strchr(p->s, '\n')) != NULL)
				*ptr = '\0';
			return 1;
		}
	case BESHORT:
		p->h = (short)((p->hs[0]<<8)|(p->hs[1]));
		return 1;
	case BELONG:
		p->l = (int)
		    ((p->hl[0]<<24)|(p->hl[1]<<16)|(p->hl[2]<<8)|(p->hl[3]));
		return 1;
	case LESHORT:
		p->h = (short)((p->hs[1]<<8)|(p->hs[0]));
		return 1;
	case LELONG:
		p->l = (int)
		    ((p->hl[3]<<24)|(p->hl[2]<<16)|(p->hl[1]<<8)|(p->hl[0]));
		return 1;
	default:
		return 0;
	}
}

static int
mget (VALUETYPE *p, unsigned char *s, MAGIC *m, int nbytes)
{
	int offset = m->offset;

	if (offset + (int)sizeof(VALUETYPE) <= nbytes)
	{
		memcpy(p, s + offset, sizeof(VALUETYPE));
	}
	else
	{
		/*
		 * the usefulness of padding with zeroes eludes me, it
		 * might even cause problems
		 */
		int have = nbytes - offset;
		memset(p, 0, sizeof(VALUETYPE));
		if (have > 0)
			memcpy(p, s + offset, have);
	}

	if (m->flag & INDIR)
	{

		switch (m->in.type)
		{
		case BYTE:
			offset = p->b + m->in.offset;
			break;
		case BESHORT:
			offset = (short)((p->hs[0]<<8)|(p->hs[1]))+
			    m->in.offset;
			break;
		case LESHORT:
			offset = (short)((p->hs[1]<<8)|(p->hs[0]))+
			    m->in.offset;
			break;
		case SHORT:
			offset = p->h + m->in.offset;
			break;
		case BELONG:
			offset = (int)((p->hl[0]<<24)|(p->hl[1]<<16)|
			    (p->hl[2]<<8)|(p->hl[3]))+
			    m->in.offset;
			break;
		case LELONG:
			offset = (int)((p->hl[3]<<24)|(p->hl[2]<<16)|
			    (p->hl[1]<<8)|(p->hl[0]))+
			    m->in.offset;
			break;
		case LONG:
			offset = p->l + m->in.offset;
			break;
		}

		if (offset + (int)sizeof(VALUETYPE) > nbytes)
			return 0;

		memcpy(p, s + offset, sizeof(VALUETYPE));
	}

	if (!mconvert(p, m))
		return 0;
	return 1;
}

static int
mcheck (VALUETYPE *p, MAGIC *m)
{
	register unsigned int l = m->value.l;
	register unsigned int v;
	int matched;

	if ( (m->value.s[0] == 'x') && (m->value.s[1] == '\0') )
	{
		return 1;
	}


	switch (m->type)
	{
	case BYTE:
		v = p->b;
		break;

	case SHORT:
	case BESHORT:
	case LESHORT:
		v = p->h;
		break;

	case LONG:
	case BELONG:
	case LELONG:
		v = p->l;
		break;

	case STRING:
		l = 0;
		/* What we want here is:
		 * v = strncmp(m->value.s, p->s, m->vallen);
		 * but ignoring any nulls.  bcmp doesn't give -/+/0
		 * and isn't universally available anyway.
		 */
		v = 0;
		{
			register unsigned char *a = (unsigned char*)m->value.s;
			register unsigned char *b = (unsigned char*)p->s;
			register int len = m->vallen;

			while (--len >= 0)
				if ((v = *b++ - *a++) != '\0')
					break;
		}
		break;
	default:
		return 0;
	}

	v = mag_signex(m, v) & m->mask;

	switch (m->reln)
	{
	case 'x':
		matched = 1;
		break;

	case '!':
		matched = (v != l);
		break;

	case '=':
		matched = (v == l);
		break;

	case '>':
		if (m->flag & UNSIGNED)
		{
			matched = (v > l);
		}
		else
		{
			matched = ( (int) v > (int) l);
		}
		break;

	case '<':
		if (m->flag & UNSIGNED)
		{
			matched = (v < l);
		}
		else
		{
			matched = ( (int) v < (int) l);
		}
		break;

	case '&':
		matched = ( (v & l) == l);
		break;

	case '^':
		matched = ( (v & l) != l);
		break;

	default:
		matched = 0;
		break;
	}

	return matched;
}
