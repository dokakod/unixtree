/*
 * ASCII magic -- file types that we know based on keywords
 * that can appear anywhere in the file.
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

/*
 * These tokens are here because they can appear anywhere in
 * the first HOWMANY bytes, while tokens in /etc/magic must
 * appear at fixed offsets into the file. Don't make HOWMANY
 * too high unless you have a very fast CPU.
 */

/* these types are used to index the table 'types': keep em in sync! */
#define	L_C		0		/* C */
#define	L_CC	1		/* C++ */
#define	L_FORT	2		/* Fortran */
#define	L_MAKE	3		/* Makefiles */
#define	L_PLI	4		/* PL/1 */
#define	L_MACH	5		/* some kinda assembler */
#define	L_ENG	6		/* English */
#define	L_PAS	7		/* Pascal */
#define	L_MAIL	8		/* Electronic mail */
#define	L_NEWS	9		/* Usenet Netnews */
#define	L_JAVA	10		/* Java code */
#define	L_HTML	11		/* HTML */
#define	L_BCPL	12		/* BCPL */

static const char *types[] =
{
		"C program text",
		"C++ program text",
		"FORTRAN program text",
		"make commands text" ,
		"PL/1 program text",
		"assembler program text",
		"English text",
		"Pascal program text",
		"mail text",
		"news text",
		"Java program text",
		"HTML document text",
		"BCPL program text",
		0
};

/*
 * XXX - how should we distinguish Java from C++?
 * The trick used in a Debian snapshot, of having "extends" or "implements"
 * as tags for Java, doesn't work very well, given that those keywords
 * are often preceded by "class", which flags it as C++.
 *
 * Perhaps we need to be able to say
 *
 *	If "class" then
 *
 *		if "extends" or "implements" then
 *			Java
 *		else
 *			C++
 *	endif
 *
 * Or should we use other keywords, such as "package" or "import"?
 * Unfortunately, Ada95 uses "package", and Modula-3 uses "import",
 * although I infer from the language spec at
 *
 *	http://www.research.digital.com/SRC/m3defn/html/m3.html
 *
 * that Modula-3 uses "IMPORT" rather than "import", i.e. it must be
 * in all caps.
 *
 * So, for now, we go with "import".  We must put it before the C++
 * stuff, so that we don't misidentify Java as C++.  Not using "package"
 * means we won't identify stuff that defines a package but imports
 * nothing; hopefully, very little Java code imports nothing (one of the
 * reasons for doing OO programming is to import as much as possible
 * and write only what you need to, right?).
 *
 * Unfortunately, "import" may cause us to misidentify English text
 * as Java, as it comes after "the" and "The".  Perhaps we need a fancier
 * heuristic to identify Java?
 */
struct names
{
	const char *name;
	short type;
};

static const struct names names[] =
{
		/* These must be sorted by eye for optimal hit rate */
		/* Add to this list only after substantial meditation */
		{ "import",			L_JAVA },
		{ "\"libhdr\"",		L_BCPL },
		{ "\"LIBHDR\"",		L_BCPL },
		{ "//",				L_CC },
		{ "template",		L_CC },
		{ "virtual",		L_CC },
		{ "class",			L_CC },
		{ "public:",		L_CC },
		{ "private:",		L_CC },
		{ "/*",				L_C },	/* must precede "The", "the", etc. */
		{ "#include",		L_C },
		{ "char",			L_C },
		{ "The",			L_ENG },
		{ "the",			L_ENG },
		{ "double",			L_C },
		{ "extern",			L_C },
		{ "float",			L_C },
		{ "struct",			L_C },
		{ "union",			L_C },
		{ "CFLAGS",			L_MAKE },
		{ "LDFLAGS",		L_MAKE },
		{ "all:",			L_MAKE },
		{ ".PRECIOUS",		L_MAKE },
	/* Too many files of text have these words in them.  Find another way
	 * to recognize Fortrash.
	 */
#	ifdef	NOTDEF
		{ "subroutine",		L_FORT },
		{ "function",		L_FORT },
		{ "block",			L_FORT },
		{ "common",			L_FORT },
		{ "dimension",		L_FORT },
		{ "integer",		L_FORT },
		{ "data",			L_FORT },
#	endif	/*NOTDEF*/
		{ ".ascii",			L_MACH },
		{ ".asciiz",		L_MACH },
		{ ".byte",			L_MACH },
		{ ".even",			L_MACH },
		{ ".globl",			L_MACH },
		{ ".text",			L_MACH },
		{ "clr",			L_MACH },
		{ "(input,",		L_PAS },
		{ "dcl",			L_PLI },
		{ "Received:",		L_MAIL },
		{ ">From",			L_MAIL },
		{ "Return-Path:",	L_MAIL },
		{ "Cc:",			L_MAIL },
		{ "Newsgroups:",	L_NEWS },
		{ "Path:",			L_NEWS },
		{ "Organization:",	L_NEWS },
		{ "href=",			L_HTML },
		{ "HREF=",			L_HTML },
		{ "<body",			L_HTML },
		{ "<BODY",			L_HTML },
		{ NULL,				0 }
};

#define NNAMES ((sizeof(names)/sizeof(struct names)) - 1)

int
mag_ascii (unsigned char *buf, int nbytes, char *result)
{
	int i, has_escapes = 0;
	unsigned char *s;
	char nbuf[HOWMANY+1];	/* one extra for terminating '\0' */
	char *token;
	const struct names *p;

	/*
	 * Do the tar test first, because if the first file in the tar
	 * archive starts with a dot, we can confuse it with an nroff file.
	 */
	if (mag_tar(buf, nbytes, result) != 0)
		return 1;

	/*
	 * for troff, look for . + letter + letter or .\";
	 * this must be done to disambiguate tar archives' ./file
	 * and other trash from real troff input.
	 */
	if (*buf == '.')
	{
		unsigned char *tp = buf + 1;

		while (isascii(*tp) && isspace(*tp))
			++tp;	/* skip leading whitespace */
		if ((isascii(*tp) && (isalnum(*tp) || *tp=='\\') &&
		    isascii(tp[1]) && (isalnum(tp[1]) || tp[1] == '"')))
		{
			strcat(result, "troff or preprocessor input text");
			return 1;
		}
	}

	if ((*buf == 'c' || *buf == 'C') &&
	    isascii(buf[1]) && isspace(buf[1]))
	{
		strcat(result, "fortran program text");
		return 1;
	}

	/* Make sure we are dealing with ascii text before looking for tokens */
	for (i = 0; i < nbytes; i++)
	{
		if (!isascii(buf[i]))
			return 0;	/* not all ASCII */
	}

	/* look for tokens from names.h - this is expensive! */
	/* make a copy of the buffer here because strtok() will destroy it */
	s = (unsigned char*) memcpy(nbuf, buf, nbytes);
	s[nbytes] = '\0';
	has_escapes = (memchr(s, '\033', nbytes) != NULL);

	while ((token = strtok((char *) s, " \t\n\r\f")) != NULL)
	{
		s = NULL;	/* make strtok() keep on tokin' */
		for (p = names; p < names + NNAMES; p++)
		{
			if (strcmp(p->name, token) == 0)
			{
				strcat(result, types[p->type]);
				if (has_escapes)
					strcat(result, " (with escape sequences)");
				return 1;
			}
		}
	}

	/* all else fails, but it is ASCII... */
	strcat(result, "ASCII text");
	if (has_escapes)
	{
		strcat(result, " (with escape sequences)");
	}

	return 1;
}
