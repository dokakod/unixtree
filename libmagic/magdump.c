/*
 * print.c - debugging printout routines
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

#define SZOF(a)	(sizeof(a) / sizeof(a[0]))


/*
 * Print a string containing C character escapes.
 */
static void
showstr (FILE *fp, const char *s, int len)
{
	register char	c;

	for (;;)
	{
		c = *s++;
		if (len == -1)
		{
			if (c == '\0')
				break;
		}
		else
		{
			if (len-- == 0)
				break;
		}

		if (c >= 040 && c <= 0176)	/* TODO isprint && !iscntrl */
		{
			fputc(c, fp);
		}
		else
		{
			fputc('\\', fp);
			switch (c) {

			case '\n':
				fputc('n', fp);
				break;

			case '\r':
				fputc('r', fp);
				break;

			case '\b':
				fputc('b', fp);
				break;

			case '\t':
				fputc('t', fp);
				break;

			case '\f':
				fputc('f', fp);
				break;

			case '\v':
				fputc('v', fp);
				break;

			default:
				fprintf(fp, "%.3o", c & 0377);
				break;
			}
		}
	}
}

void
mag_dump (MAGIC *m)
{
	static const char *typ[] =
	{
		"invalid",
		"byte",
		"string",
		"short",
		"beshort",
		"leshort",
		"long",
		"belong",
		"lelong"
	};

	fputc('[', stdout);
	if (m->flag & ALSO)
		fprintf(stdout, "&");
	fprintf(stdout, ">>>>>>>> %d" + 8 - (m->cont_level & 7),
	    m->offset);

	if (m->flag & INDIR)
	{
		fprintf(stdout, "(%s,%d),",
		    (m->in.type < SZOF(typ)) ? typ[m->in.type] : "*bad*",
		    m->in.offset);
	}

	fprintf(stdout, " %s%s", (m->flag & UNSIGNED) ? "u" : "",
	    (m->type < SZOF(typ)) ? typ[m->type] : "*bad*");

	if (m->mask != ~((unsigned int)0))
		fprintf(stdout, " & %.8x", m->mask);

	fprintf(stdout, ",%c", m->reln);

	if (m->reln != 'x')
	{
		switch (m->type)
		{
		case BYTE:
		case SHORT:
		case LONG:
		case LESHORT:
		case LELONG:
		case BESHORT:
		case BELONG:
			fprintf(stdout, "%d", m->value.l);
			break;
		case STRING:
			showstr(stdout, m->value.s, -1);
			break;
		default:
			fputs("*bad*", stdout);
			break;
		}
	}

	fprintf(stdout, ",\"%s\"]\n", m->desc);
}
