/*
 * regcomp and regexec
 *
 *	Copyright (c) 1986 by University of Toronto.
 *	Written by Henry Spencer.  Not derived from licensed software.
 *
 *	Permission is granted to anyone to use this software for any
 *	purpose on any computer system, and to redistribute it freely,
 *	subject to the following restrictions:
 *
 *	1. The author is not responsible for the consequences of use of
 *		this software, no matter how awful, even if they arise
 *		from defects in it.
 *
 *	2. The origin of this software must not be misrepresented, either
 *		by explicit claim or by omission.
 *
 *	3. Altered versions must be plainly marked as such, and must not
 *		be misrepresented as being the original software.
 *
 * Beware that some of this code is subtly aware of the way operator
 * precedence is structured in regular expressions.  Serious changes in
 * regular-expression syntax might require a total rethink.
 *
 * This code has been seriously hacked.
 */
#include "utcommon.h"
#include <setjmp.h>

/*
 * The "internal use only" fields in regexp.h are present to pass info from
 * compile to execute that permits the execute phase to run lots faster on
 * simple cases.  They are:
 *
 * regstart	char that must begin a match; '\0' if none obvious
 * reganch	is the match anchored (at beginning-of-line only)?
 * regmust	string (pointer into program) that match must include, or NULL
 * regmlen	length of regmust string
 *
 * Regstart and reganch permit very fast decisions on suitable starting points
 * for a match, cutting down the work a lot.  Regmust permits fast rejection
 * of lines that cannot possibly match.  The regmust tests are costly enough
 * that regcomp() supplies a regmust only if the r.e. contains something
 * potentially expensive (at present, the only such thing detected is * or +
 * at the start of the r.e., which can involve a lot of backup).  Regmlen is
 * supplied because the test in regexec() needs it and regcomp() is computing
 * it anyway.
 */

/*
 * Structure for regexp "program".  This is essentially a linear encoding
 * of a nondeterministic finite-state machine (aka syntax charts or
 * "railroad normal form" in parsing technology).  Each node is an opcode
 * plus a "next" pointer, possibly plus an operand.  "Next" pointers of
 * all nodes except BRANCH implement concatenation; a "next" pointer with
 * a BRANCH on both ends of it is connecting two alternatives.  (Here we
 * have one of the subtle syntax dependencies:  an individual BRANCH (as
 * opposed to a collection of them) is never concatenated with anything
 * because of operator precedence.)  The operand of some types of node is
 * a literal string; for others, it is a node leading into a sub-FSM.  In
 * particular, the operand of a BRANCH node is the first node of the branch.
 * (NB this is *not* a tree structure:  the tail of the branch connects
 * to the thing following the set of BRANCHes.)  The opcodes are:
 */

/* definition	number	   opnd?	meaning */
#define	END		0		/* no		End of program. */
#define	BOL		1		/* no		Match "" at beginning of line. */
#define	EOL		2		/* no		Match "" at end of line. */
#define	ANY		3		/* no		Match any one character. */
#define	ANYOF	4		/* str		Match any character in this string. */
#define	ANYBUT	5		/* str		Match any character not in this string. */
#define	BRANCH	6		/* node		Match this alternative, or the next... */
#define	BACK	7		/* no		Match "", "next" ptr points backward. */
#define	EXACTLY	8		/* str		Match this string. */
#define	NOTHING	9		/* no		Match empty string. */
#define	STAR	10		/* node		Match this thing 0 or more times */
#define	PLUS	11		/* node		Match this thing 1 or more times */
#define	OPEN	20		/* no		Mark this point in input as start of #n. */
						/*			OPEN+1 is number 1, etc. */
#define	CLOSE	30		/* no		Analogous to OPEN. */

/*
 * The first byte of the regexp internal "program" is actually this magic
 * number; the start node begins in the second byte.
 */
#define	REG_MAGIC	0xb2

/*
 * Opcode notes:
 *
 * BRANCH	The set of branches constituting a single choice are hooked
 *		together with their "next" pointers, since precedence prevents
 *		anything being concatenated to any individual branch.  The
 *		"next" pointer of the last BRANCH in a choice points to the
 *		thing following the whole choice.  This is also where the
 *		final "next" pointer of each individual branch points; each
 *		branch starts with the operand node of a BRANCH node.
 *
 * BACK		Normal "next" pointers all implicitly point forward; BACK
 *		exists to make loop structures possible.
 *
 * STAR,PLUS	'?', and complex '*' and '+', are implemented as circular
 *		BRANCH structures using BACK.  Simple cases (one character
 *		per match) are implemented with STAR and PLUS for speed
 *		and to minimize recursive plunges.
 *
 * OPEN,CLOSE	...are numbered at compile time.
 */

/*
 * A node is one char of opcode followed by two chars of "next" pointer.
 * "Next" pointers are stored as two 8-bit pieces, high order first.  The
 * value is a positive offset from the opcode of the node containing it.
 * An operand, if any, simply follows the node.  (Note that much of the
 * code generation knows about this implicit relationship.)
 *
 * Using two bytes for the "next" pointer is vast overkill for most things,
 * but allows patterns to get big without disasters.
 */
#define	OP(p)	(*(p))
#define	NEXT(p)	(((*((p)+1)&0377)<<8) + (*((p)+2)&0377))
#define	OPERAND(p)	((p) + 3)

/*
 * See regmagic.h for one further detail of program structure.
 */


/*
 * Utility definitions.
 */
#ifndef CHARBITS
#define	UCHARAT(p)	((int)*(unsigned char *)(p))
#else
#define	UCHARAT(p)	((int)*(p)&CHARBITS)
#endif

#define	ISMULT(c)	((c) == '*' || (c) == '+' || (c) == '?')
#define	META		"^$.[()|?+*\\"

/*
 * Flags to be passed up and down.
 */
#define	HASWIDTH	01	/* Known never to match null string. */
#define	SIMPLE		02	/* Simple enough to be STAR/PLUS operand. */
#define	SPSTART		04	/* Starts with * or +. */
#define	WORST		0	/* Worst case. */

/*
 * Global work variables for regcomp().
 */
typedef struct comp_data
{
	const char *	regparse;		/* Input-scan pointer. */
	int				regnpar;		/* () count. */
	char *			regcode;		/* Code-emit pointer; REG_DUMMY = don't. */
	int				regsize;		/* Code size. */
	int				reg_errno;		/* error number */
	jmp_buf			jmpbuf;			/* error jmp buf */
} COMP_DATA;

#define REG_DUMMY	((char *)-1)

/*
 * Forward declarations for regcomp()'s friends.
 */
static char *reg(COMP_DATA *, int, int *);
static char *regbranch(COMP_DATA *, int *);
static char *regpiece(COMP_DATA *, int *);
static char *regatom(COMP_DATA *, int *);
static char *regnode(COMP_DATA *, int);
static char *regnext(char *);
static void regc(COMP_DATA *, int);
static void reginsert(COMP_DATA *, int, char *);
static void regtail(COMP_DATA *cd, char *, char *);
static void regoptail(COMP_DATA *cd, char *, char *);

/*
** defines & variables needed for modifications
*/
#define REGMAX	256			/* max chars to search */

#if 0
char *reg_msgs[] =
{
	"insufficient memory",				/*  0 */
	"damaged match string",				/*  1 */
	"damaged regexp fed to regsub",		/*  2 */
	"NULL parameter",					/*  3 */
	"internal error",					/*  4 */
	"corrupted pointers",				/*  5 */
	"corrupted memory",					/*  6 */
	"corrupted program",				/*  7 */
	"trailing \\",						/*  8 */
	"unmatched []",						/*  9 */
	"invalid [] range",					/* 10 */
	"nested *+?",						/* 11 */
	"*+ operand could be empty",		/* 12 */
	"*+? follows nothing",				/* 13 */
	"junk on end",						/* 14 */
	"unmatched ()",						/* 15 */
	"too many ()",						/* 16 */
	"regexp too big",					/* 17 */
};
#endif

#define REG_CASE(c, reg_case)	(reg_case ? tolower(c) : (c))

static void regerror (COMP_DATA *cd, int err)
{
	cd->reg_errno = err;

	longjmp(cd->jmpbuf, 1);
}

/*
 *	xstrchr - just like the library strchr except it stops
 *	at 0, nl, or after REGMAX chars
 */

static const char *xstrchr (const char *s, int c, int reg_case)
{
	const char *regmax = s+REGMAX;

	c = REG_CASE(c, reg_case);

	for (; *s && *s!='\n' && s<regmax; s++)
	{
		if (REG_CASE(*s, reg_case) == c)
			return (s);
	}

	return (0);
}

static int xstrncmp (const char *s, const char *t, int n, int reg_case)
{
	for (; n; n--)
	{
		int d = (REG_CASE(*s, reg_case) - REG_CASE(*t, reg_case));

		if (d != 0)
			return (d);

		s++;
		t++;
	}

	return (0);
}

/*
 - regcomp - compile a regular expression into internal code
 *
 * We can't allocate space until we know how big the compiled form will be,
 * but we can't compile it (and thus know how big it is) until we've got a
 * place to put the code.  So we cheat:  we compile it twice, once with code
 * generation turned off and size counting turned on, and once "for real".
 * This also means that we don't allocate space until we are sure that the
 * thing really will compile successfully, and we never have to move the
 * code and thus invalidate pointers into it.  (Note that it has to be in
 * one piece because free() must be able to free it all.)
 *
 * Beware that the optimization-preparation code in here knows about some
 * of the structure of the compiled regexp.
 */
REGEXP *
ut_regcomp (const char *exp, int nocase, int *rc)
{
	COMP_DATA comp_data;
	COMP_DATA *cd = &comp_data;
	REGEXP *r;
	char *scan;
	char *longest;
	int len;
	int flags;

	cd->reg_errno = 0;
	if (setjmp(cd->jmpbuf))
	{
		*rc = cd->reg_errno;
		return ((REGEXP *)NULL);
	}

	if (!exp)
	{
		regerror(cd, 3);		/* NULL parameter */
	}

	/* First pass: determine size, legality. */
	cd->regparse = exp;
	cd->regnpar = 1;
	cd->regsize = 0L;
	cd->regcode = REG_DUMMY;
	regc(cd, REG_MAGIC);
	if (!reg(cd, 0, &flags))
	{
		*rc = cd->reg_errno;
		return ((REGEXP *)NULL);
	}

	/* Small enough for pointer-storage convention? */
	if (cd->regsize >= 32767L)		/* Probably could be 65535L. */
	{
		regerror(cd, 17);		/* regexp too big */
	}

	/* Allocate space. */
	r = (REGEXP *)MALLOC(sizeof(REGEXP) + (unsigned)cd->regsize);
	if (!r)
	{
		regerror(cd, 0);		/* insufficient memory */
	}

	/* Second pass: emit code. */
	cd->regparse = exp;
	cd->regnpar = 1;
	cd->regcode = r->program;
	regc(cd, REG_MAGIC);
	if (!reg(cd, 0, &flags))
	{
		*rc = cd->reg_errno;
		return ((REGEXP *)NULL);
	}

	/* Dig out information for optimizations. */
	r->regstart = 0;	/* Worst-case defaults. */
	r->reganch = 0;
	r->regmust = 0;
	r->regmlen = 0;
	scan = r->program+1;			/* First BRANCH. */
	if (OP(regnext(scan)) == END)		/* Only one top-level choice. */
	{
		scan = OPERAND(scan);

		/* Starting-point info. */
		if (OP(scan) == EXACTLY)
			r->regstart = *OPERAND(scan);
		else if (OP(scan) == BOL)
			r->reganch++;

		/*
		 * If there's something expensive in the r.e., find the
		 * longest literal string that must appear and make it the
		 * regmust.  Resolve ties in favor of later strings, since
		 * the regstart check works with the beginning of the r.e.
		 * and avoiding duplication strengthens checking.  Not a
		 * strong reason, but sufficient in the absence of others.
		 */
		if (flags & SPSTART)
		{
			longest = 0;
			len = 0;
			for (; scan; scan = regnext(scan))
			{
				if (OP(scan) == EXACTLY && (int)strlen(OPERAND(scan)) >= len)
				{
					longest = OPERAND(scan);
					len = strlen(OPERAND(scan));
				}
			}
			r->regmust = longest;
			r->regmlen = len;
		}
	}

	r->igncase = nocase;

	*rc = 0;
	return (r);
}

/*
 - reg - regular expression, i.e. main body or parenthesized thing
 *
 * Caller must absorb opening parenthesis.
 *
 * Combining parenthesis handling with the base level of regular expression
 * is a trifle forced, but the need to tie the tails of the branches to what
 * follows makes it hard to avoid.
 */
static char *
reg (COMP_DATA *cd, int paren, int *flagp)
{
	char *ret;
	char *br;
	char *ender;
	int parno = 0;
	int flags;

	*flagp = HASWIDTH;	/* Tentatively. */

	/* Make an OPEN node, if parenthesized. */
	if (paren)
	{
		if (cd->regnpar >= NSUBEXP)
		{
			regerror(cd, 16);		/* too many () */
		}
		parno = cd->regnpar;
		cd->regnpar++;
		ret = regnode(cd, OPEN+parno);
	}
	else
	{
		ret = 0;
	}

	/* Pick up the branches, linking them together. */
	br = regbranch(cd, &flags);
	if (!br)
		return (0);
	if (ret)
		regtail(cd, ret, br);	/* OPEN -> first. */
	else
		ret = br;
	if (!(flags&HASWIDTH))
		*flagp &= ~HASWIDTH;
	*flagp |= flags&SPSTART;
	while (*cd->regparse == '|')
	{
		cd->regparse++;
		br = regbranch(cd, &flags);
		if (!br)
			return (0);
		regtail(cd, ret, br);	/* BRANCH -> BRANCH. */
		if (!(flags&HASWIDTH))
			*flagp &= ~HASWIDTH;
		*flagp |= flags&SPSTART;
	}

	/* Make a closing node, and hook it on the end. */
	ender = regnode(cd, (paren) ? CLOSE+parno : END);
	regtail(cd, ret, ender);

	/* Hook the tails of the branches to the closing node. */
	for (br = ret; br; br = regnext(br))
		regoptail(cd, br, ender);

	/* Check for proper termination. */
	if (paren && *cd->regparse++ != ')')
	{
		regerror(cd, 15);		/* unmatched () */
	}
	else if (!paren && *cd->regparse != '\0')
	{
		if (*cd->regparse == ')')
		{
			regerror(cd, 15);		/* unmatched () */
		}
		else
		{
			regerror(cd, 14);		/* junk on end */
		}
		/*NOTREACHED*/
	}

	return (ret);
}

/*
 - regbranch - one alternative of an | operator
 *
 * Implements the concatenation operator.
 */
static char *
regbranch (COMP_DATA *cd, int *flagp)
{
	char *ret;
	char *chain;
	char *latest;
	int flags;

	*flagp = WORST;		/* Tentatively. */

	ret = regnode(cd, BRANCH);
	chain = 0;
	while (*cd->regparse != '\0' &&
		   *cd->regparse != '|'  &&
		   *cd->regparse != ')')
	{
		latest = regpiece(cd, &flags);
		if (!latest)
			return (0);
		*flagp |= flags&HASWIDTH;
		if (!chain)		/* First piece. */
			*flagp |= flags&SPSTART;
		else
			regtail(cd, chain, latest);
		chain = latest;
	}
	if (!chain)		/* Loop ran zero times. */
		regnode(cd, NOTHING);

	return (ret);
}

/*
 - regpiece - something followed by possible [*+?]
 *
 * Note that the branching code sequences used for ? and the general cases
 * of * and + are somewhat optimized:  they use the same NOTHING node as
 * both the endmarker for their branch list and the body of the last branch.
 * It might seem that this node could be dispensed with entirely, but the
 * endmarker role is not redundant.
 */
static char *
regpiece (COMP_DATA *cd, int *flagp)
{
	char *ret;
	char op;
	char *next;
	int flags;

	ret = regatom(cd, &flags);
	if (!ret)
		return (0);

	op = *cd->regparse;
	if (!ISMULT(op)) {
		*flagp = flags;
		return (ret);
	}

	if (!(flags&HASWIDTH) && op != '?')
	{
		regerror(cd, 12);		/* *+ operand could be empty */
	}
	*flagp = (op != '+') ? (WORST|SPSTART) : (WORST|HASWIDTH);

	if (op == '*' && (flags & SIMPLE))
	{
		reginsert(cd, STAR, ret);
	}
	else if (op == '*')
	{
		/* Emit x* as (x&|), where & means "self". */
		reginsert(cd, BRANCH, ret);			/* Either x */
		regoptail(cd, ret, regnode(cd, BACK));		/* and loop */
		regoptail(cd, ret, ret);			/* back */
		regtail(cd, ret, regnode(cd, BRANCH));		/* or */
		regtail(cd, ret, regnode(cd, NOTHING));		/* null. */
	}
	else if (op == '+' && (flags&SIMPLE))
	{
		reginsert(cd, PLUS, ret);
	}
	else if (op == '+')
	{
		/* Emit x+ as x(&|), where & means "self". */
		next = regnode(cd, BRANCH);			/* Either */
		regtail(cd, ret, next);
		regtail(cd, regnode(cd, BACK), ret);		/* loop back */
		regtail(cd, next, regnode(cd, BRANCH));		/* or */
		regtail(cd, ret, regnode(cd, NOTHING));		/* null. */
	}
	else if (op == '?')
	{
		/* Emit x? as (x|) */
		reginsert(cd, BRANCH, ret);			/* Either x */
		regtail(cd, ret, regnode(cd, BRANCH));		/* or */
		next = regnode(cd, NOTHING);		/* null. */
		regtail(cd, ret, next);
		regoptail(cd, ret, next);
	}
	cd->regparse++;
	if (ISMULT(*cd->regparse))
	{
		regerror(cd, 11);		/* nested *?+ */
	}

	return (ret);
}

/*
 - regatom - the lowest level
 *
 * Optimization:  gobbles an entire sequence of ordinary characters so that
 * it can turn them into a single node, which is smaller to store and
 * faster to run.  Backslashed characters are exceptions, each becoming a
 * separate node; the code is simpler that way and it's not worth fixing.
 */
static char *
regatom (COMP_DATA *cd, int *flagp)
{
	char *ret = 0;
	int flags;

	*flagp = WORST;		/* Tentatively. */

	switch (*cd->regparse++)
	{
	case '^':
		ret = regnode(cd, BOL);
		break;
	case '$':
		ret = regnode(cd, EOL);
		break;
	case '.':
		ret = regnode(cd, ANY);
		*flagp |= HASWIDTH|SIMPLE;
		break;
	case '[':
		{
			int class_ptr;
			int class_end;

			if (*cd->regparse == '^')	/* Complement of range. */
			{
				ret = regnode(cd, ANYBUT);
				cd->regparse++;
			}
			else
			{
				ret = regnode(cd, ANYOF);
			}
			if (*cd->regparse == ']' || *cd->regparse == '-')
				regc(cd, *cd->regparse++);
			while (*cd->regparse != '\0' && *cd->regparse != ']')
			{
				if (*cd->regparse == '-')
				{
					cd->regparse++;
					if (*cd->regparse == ']' || *cd->regparse == '\0')
					{
						regc(cd, '-');
					}
					else
					{
						class_ptr = UCHARAT(cd->regparse-2)+1;
						class_end = UCHARAT(cd->regparse);
						if (class_ptr > class_end+1)
						{
							regerror(cd, 10);		/* invalid [] range */
						}

						for (; class_ptr <= class_end; class_ptr++)
							regc(cd, class_ptr);
						cd->regparse++;
					}
				}
				else
				{
					regc(cd, *cd->regparse++);
				}
			}
			regc(cd, '\0');
			if (*cd->regparse != ']')
			{
				regerror(cd, 9);		/* unmatched [] */
			}
			cd->regparse++;
			*flagp |= HASWIDTH|SIMPLE;
		}
		break;
	case '(':
		ret = reg(cd, 1, &flags);
		if (!ret)
			return (0);
		*flagp |= flags&(HASWIDTH|SPSTART);
		break;
	case '\0':
	case '|':
	case ')':
		regerror(cd, 4);		/* internal error */
		break;
	case '?':
	case '+':
	case '*':
		regerror(cd, 13);		/* ?+* follows nothing */
		break;
	case '\\':
		if (*cd->regparse == '\0')
		{
			regerror(cd, 8);		/* trailing \\ */
		}
		ret = regnode(cd, EXACTLY);
		regc(cd, *cd->regparse++);
		regc(cd, '\0');
		*flagp |= HASWIDTH|SIMPLE;
		break;
	default:
		{
			int len;
			char ender;

			cd->regparse--;
			len = strcspn(cd->regparse, META);
			if (len <= 0)
			{
				regerror(cd, 4);		/* internal error */
			}
			ender = *(cd->regparse+len);
			if (len > 1 && ISMULT(ender))
				len--;		/* Back off clear of ?+* operand. */
			*flagp |= HASWIDTH;
			if (len == 1)
				*flagp |= SIMPLE;
			ret = regnode(cd, EXACTLY);
			while (len > 0)
			{
				regc(cd, *cd->regparse++);
				len--;
			}
			regc(cd, '\0');
		}
		break;
	}

	return (ret);
}

/*
 - regnode - emit a node
 */
static char *			/* Location. */
regnode (COMP_DATA *cd, int op)
{
	char *ret;
	char *ptr;

	ret = cd->regcode;
	if (ret == REG_DUMMY)
	{
		cd->regsize += 3;
		return (ret);
	}

	ptr = ret;
	*ptr++ = op;
	*ptr++ = '\0';		/* Null "next" pointer. */
	*ptr++ = '\0';
	cd->regcode = ptr;

	return (ret);
}

/*
 - regc - emit (if appropriate) a byte of code
 */
static void
regc (COMP_DATA *cd, int b)
{
	if (cd->regcode != REG_DUMMY)
		*cd->regcode++ = b;
	else
		cd->regsize++;
}

/*
 - reginsert - insert an operator in front of already-emitted operand
 *
 * Means relocating the operand.
 */
static void
reginsert (COMP_DATA *cd, int op, char *opnd)
{
	char *src;
	char *dst;
	char *place;

	if (cd->regcode == REG_DUMMY)
	{
		cd->regsize += 3;
		return;
	}

	src = cd->regcode;
	cd->regcode += 3;
	dst = cd->regcode;
	while (src > opnd)
		*--dst = *--src;

	place = opnd;		/* Op node, where operand used to be. */
	*place++ = op;
	*place++ = '\0';
	*place++ = '\0';
}

/*
 - regtail - set the next-pointer at the end of a node chain
 */
static void
regtail (COMP_DATA *cd, char *p, char *val)
{
	char *scan;
	char *temp;
	int offset;

	if (p == REG_DUMMY)
		return;

	/* Find last node. */
	scan = p;
	for (;;)
	{
		temp = regnext(scan);
		if (!temp)
			break;
		scan = temp;
	}

	if (OP(scan) == BACK)
		offset = scan - val;
	else
		offset = val - scan;
	*(scan+1) = (offset>>8)&0377;
	*(scan+2) = offset&0377;
}

/*
 - regoptail - regtail on operand of first argument; nop if operandless
 */
static void
regoptail (COMP_DATA *cd, char *p, char *val)
{
	/* "Operandless" and "op != BRANCH" are synonymous in practice. */
	if (p == 0 || p == REG_DUMMY || OP(p) != BRANCH)
		return;
	regtail(cd, OPERAND(p), val);
}

/*
 * regexec and friends
 */

/*
 * work variables for regexec().
 */
typedef struct exec_data
{
	const char *	reginput;		/* String-input pointer. */
	const char *	regbol;			/* Beginning of input, for ^ check. */
	const char **	regbegp;		/* Pointer to startp array. */
	const char **	regendp;		/* Ditto for endp. */
	const char *	reg_last;		/* ptr to last char in match */
	int				reg_case;		/* TRUE if case insensitive */
} EXEC_DATA;

/*
 - regrepeat - repeatedly match something simple, report how many
 */
static int
regrepeat (EXEC_DATA *ed, char *p)
{
	int count = 0;
	const char *scan;
	char *opnd;

	scan = ed->reginput;
	opnd = OPERAND(p);
	switch (OP(p)) {
	case ANY:
		count = strlen(scan);
		scan += count;
		break;
	case EXACTLY:
		while (REG_CASE(*opnd, ed->reg_case) == REG_CASE(*scan, ed->reg_case))
		{
			count++;
			scan++;
		}
		break;
	case ANYOF:
		while (*scan != '\0' && xstrchr(opnd, *scan, ed->reg_case))
		{
			count++;
			scan++;
		}
		break;
	case ANYBUT:
		while (*scan != '\0' && !xstrchr(opnd, *scan, ed->reg_case))
		{
			count++;
			scan++;
		}
		break;
	default:		/* Oh dear.  Called inappropriately. */
		count = 0;	/* Best compromise. */
		break;
	}
	ed->reginput = scan;

	return (count);
}

/*
 - regmatch - main matching routine
 *
 * Conceptually the strategy is simple:  check to see whether the current
 * node matches, call self recursively to see whether the rest matches,
 * and then act accordingly.  In practice we make some effort to avoid
 * recursion, in particular by going through "ordinary" nodes (that don't
 * need to know whether the rest of the match failed) by a loop instead of
 * by recursion.
 */
static int			/* 0 failure, 1 success */
regmatch (EXEC_DATA *ed, char *prog)
{
	char *scan;	/* Current node. */
	char *next;		/* Next node. */

	scan = prog;
	while (scan)
	{
		next = regnext(scan);

		switch (OP(scan))
		{
		case BOL:
			if (ed->reginput != ed->regbol)
				return (0);
			break;
		case EOL:
			if (*ed->reginput != '\0')
				return (0);
			break;
		case ANY:
			if (*ed->reginput == '\0')
				return (0);
			ed->reginput++;
			break;
		case EXACTLY:
			{
				int len;
				char *opnd;

				opnd = OPERAND(scan);
				/* Inline the first character, for speed. */
				if (REG_CASE(*opnd, ed->reg_case) !=
					REG_CASE(*ed->reginput, ed->reg_case))
				{
					return (0);
				}

				len = strlen(opnd);
				if (len > 1 &&
					xstrncmp(opnd, ed->reginput, len, ed->reg_case) != 0)
				{
					return (0);
				}
				ed->reginput += len;
			}
			break;
		case ANYOF:
			if (*ed->reginput == '\0' ||
				!xstrchr(OPERAND(scan), *ed->reginput, ed->reg_case))
			{
				return (0);
			}
			ed->reginput++;
			break;
		case ANYBUT:
			if (*ed->reginput == '\0' ||
				xstrchr(OPERAND(scan), *ed->reginput, ed->reg_case))
			{
				return (0);
			}
			ed->reginput++;
			break;
		case NOTHING:
			break;
		case BACK:
			break;
		case OPEN+1:
		case OPEN+2:
		case OPEN+3:
		case OPEN+4:
		case OPEN+5:
		case OPEN+6:
		case OPEN+7:
		case OPEN+8:
		case OPEN+9:
			{
				int no;
				const char *save;

				no = OP(scan) - OPEN;
				save = ed->reginput;

				if (regmatch(ed, next))
				{
					/*
					 * Don't set startp if some later
					 * invocation of the same parentheses
					 * already has.
					 */
					if (!ed->regbegp[no])
						ed->regbegp[no] = save;
					return (1);
				}
				return (0);
			}

		case CLOSE+1:
		case CLOSE+2:
		case CLOSE+3:
		case CLOSE+4:
		case CLOSE+5:
		case CLOSE+6:
		case CLOSE+7:
		case CLOSE+8:
		case CLOSE+9:
			{
				int no;
				const char *save;

				no = OP(scan) - CLOSE;
				save = ed->reginput;

				if (regmatch(ed, next))
				{
					/*
					 * Don't set endp if some later
					 * invocation of the same parentheses
					 * already has.
					 */
					if (!ed->regendp[no])
						ed->regendp[no] = save;
					return (1);
				}

				return (0);
			}

		case BRANCH:
			{
				const char *save;

				if (OP(next) != BRANCH)		/* No choice. */
				{
					next = OPERAND(scan);	/* Avoid recursion. */
				}
				else
				{
					do {
						save = ed->reginput;
						if (regmatch(ed, OPERAND(scan)))
							return (1);
						ed->reginput = save;
						scan = regnext(scan);
					} while (scan && OP(scan) == BRANCH);
					return (0);
					/*NOTREACHED*/
				}
			}
			break;

		case STAR:
		case PLUS:
			{
				char nextch;
				int no;
				const char *save;
				int minplus;

				/*
				 * Lookahead to avoid useless match attempts
				 * when we know what character comes next.
				 */
				nextch = '\0';
				if (OP(next) == EXACTLY)
					nextch = *OPERAND(next);
				minplus = (OP(scan) == STAR) ? 0 : 1;
				save = ed->reginput;
				no = regrepeat(ed, OPERAND(scan));
				while (no >= minplus)
				{
					/* If it could work, try it. */
					if (nextch == '\0' || *ed->reginput == nextch)
					{
						if (regmatch(ed, next))
							return (1);
					}
					/* Couldn't or didn't -- back up. */
					no--;
					ed->reginput = save + no;
				}
			}
			return (0);
		case END:
			return (1);	/* Success! */
		default:
			return (0);
		}

		scan = next;
	}

	/*
	 * We get here only if there's trouble -- normally "case END" is
	 * the terminating point.
	 */
	return (0);
}

/*
 - regtry - try match at specific point
 */
static int			/* 0 failure, 1 success */
regtry (EXEC_DATA *ed, REGEXP *prog, const char *string)
{
	int i;
	const char **sp;
	const char **ep;

	ed->reginput = string;
	ed->regbegp  = prog->startp;
	ed->regendp  = prog->endp;

	sp = prog->startp;
	ep = prog->endp;

	for (i = NSUBEXP; i > 0; i--)
	{
		*sp++ = 0;
		*ep++ = 0;
	}

	if (regmatch(ed, prog->program + 1))
	{
		prog->startp[0] = string;
		prog->endp[0] = ed->reginput;
		return (1);
	}

	return (0);
}

/*
 - regnext - dig the "next" pointer out of a node
 */
static char *
regnext (char *p)
{
	int offset;

	if (p == REG_DUMMY)
		return (0);

	offset = NEXT(p);
	if (offset == 0)
		return (0);

	if (OP(p) == BACK)
		return (p-offset);
	else
		return (p+offset);
}

/*
 - regexec - match a regexp against a string
 */
const char *ut_regexec (REGEXP *prog, const char *string, const char **endp)
{
	EXEC_DATA	exec_data;
	EXEC_DATA *	ed = &exec_data;
	const char *s;

	/* Be paranoid... */
	if (prog == 0 || string == 0)
	{
		return (0);
	}

	/* Check validity of program. */
	if (UCHARAT(prog->program) != REG_MAGIC)
	{
		return (0);
	}

	ed->reg_case = prog->igncase;
	ed->reg_last = 0;

	/* If there is a "must appear" string, look for it. */
	if (prog->regmust)
	{
		s = string;
		while ((s = xstrchr(s, prog->regmust[0], ed->reg_case)))
		{
			if (xstrncmp(s, prog->regmust, prog->regmlen, ed->reg_case) == 0)
			{
				break;	/* Found it. */
			}
			s++;
		}
		if (s == 0)			/* Not present. */
			return (0);
	}

	/* Mark beginning of line for ^ . */
	ed->regbol = string;

	/* Simplest case:  anchored match need be tried only once. */
	if (prog->reganch)
	{
		if (regtry(ed, prog, string))
		{
			if (endp != 0)
				*endp = ed->reg_last;
			return (string);
		}
		else
		{
			return (0);
		}
	}

	/* Messy cases:  unanchored match. */
	s = string;
	if (prog->regstart != '\0')
	{
		/* We know what char it must start with. */
		while ((s = xstrchr(s, prog->regstart, ed->reg_case)))
		{
			if (regtry(ed, prog, s))
			{
				if (endp != 0)
					*endp = ed->reg_last;
				return (s);
			}
			s++;
		}
	}
	else
	{
		/* We don't -- general case. */
		do {
			if (regtry(ed, prog, s))
			{
				if (endp != 0)
					*endp = ed->reg_last;
				return (s);
			}
		} while (*s++ != '\0');
	}

	/* Failure. */
	return (0);
}
