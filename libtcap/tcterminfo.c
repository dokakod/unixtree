/*------------------------------------------------------------------------
 * routines to process terminfo entries
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * terminfo header struct & data pointers
 */
struct ti_hdr
{
	short	magic;
	short	name_size;
	short	bool_count;
	short	nums_count;
	short	offs_count;
	short	strs_size;
};
typedef struct ti_hdr TI_HDR;

#define TI_NAMES(ti)	( (char  *)((char *)ti + sizeof(*ti)) )

#define TI_BOOLS(ti)	( (char  *)((char *)ti + sizeof(*ti) + ti->name_size) )

#define TI_NUMS(ti)		( (short *)((char *)ti + sizeof(*ti) + \
							((ti->name_size + ti->bool_count + 1) & ~1)) )

#define TI_OFFS(ti)		( (short *)(TI_NUMS(ti) + ti->nums_count) )

#define TI_STRS(ti)		( (char  *)(TI_OFFS(ti) + ti->offs_count) )

#define TI_MAGIC		0x011a		/* terminfo magic number */

/*------------------------------------------------------------------------
 * terminfo name arrays
 */
static const char * tcap_ti_bool_names[] =
{
	"bw",
	"am",
	"xsb",
	"xhp",
	"xenl",
	"eo",
	"gn",
	"hc",
	"km",
	"hs",
	"in",
	"da",
	"db",
	"mir",
	"msgr",
	"os",
	"eslok",
	"xt",
	"hz",
	"ul",
	"xon",
	"nxon",
	"mc5i",
	"chts",
	"nrrmc",
	"npc",
	"ndscr",
	"ccc",
	"bce",
	"hls",
	"xhpa",
	"crxm",
	"daisy",
	"xvpa",
	"sam",
	"cpix",
	"lpix",
	0
};

static const char * tcap_ti_num_names[] =
{
	"cols",
	"it",
	"lines",
	"lm",
	"xmc",
	"pb",
	"vt",
	"wsl",
	"nlab",
	"lh",
	"lw",
	"ma",
	"wnum",
	"colors",
	"pairs",
	"ncv",
	"bufsz",
	"spinv",
	"spinh",
	"maddr",
	"mjump",
	"mcs",
	"mls",
	"npins",
	"orc",
	"orl",
	"orhi",
	"orvi",
	"cps",
	"widcs",
	"btns",
	"bitwin",
	"bitype",
	0
};

static const char * tcap_ti_str_names[] =
{
	"cbt",
	"bel",
	"cr",
	"csr",
	"tbc",
	"clear",
	"el",
	"ed",
	"hpa",
	"cmdch",
	"cup",
	"cud1",
	"home",
	"civis",
	"cub1",
	"mrcup",
	"cnorm",
	"cuf1",
	"ll",
	"cuu1",
	"cvvis",
	"dch1",
	"dl1",
	"dsl",
	"hd",
	"smacs",
	"blink",
	"bold",
	"smcup",
	"smdc",
	"dim",
	"smir",
	"invis",
	"prot",
	"rev",
	"smso",
	"smul",
	"ech",
	"rmacs",
	"sgr0",
	"rmcup",
	"rmdc",
	"rmir",
	"rmso",
	"rmul",
	"flash",
	"ff",
	"fsl",
	"is1",
	"is2",
	"is3",
	"if",
	"ich1",
	"il1",
	"ip",
	"kbs",
	"ktbc",
	"kclr",
	"kctab",
	"kdch1",
	"kdl1",
	"kcud1",
	"krmir",
	"kel",
	"ked",
	"kf0",
	"kf1",
	"kf10",
	"kf2",
	"kf3",
	"kf4",
	"kf5",
	"kf6",
	"kf7",
	"kf8",
	"kf9",
	"khome",
	"kich1",
	"kil1",
	"kcub1",
	"kll",
	"knp",
	"kpp",
	"kcuf1",
	"kind",
	"kri",
	"khts",
	"kcuu1",
	"rmkx",
	"smkx",
	"lf0",
	"lf1",
	"lf10",
	"lf2",
	"lf3",
	"lf4",
	"lf5",
	"lf6",
	"lf7",
	"lf8",
	"lf9",
	"rmm",
	"smm",
	"nel",
	"pad",
	"dch",
	"dl",
	"cud",
	"ich",
	"indn",
	"il",
	"cub",
	"cuf",
	"rin",
	"cuu",
	"pfkey",
	"pfloc",
	"pfx",
	"mc0",
	"mc4",
	"mc5",
	"rep",
	"rs1",
	"rs2",
	"rs3",
	"rf",
	"rc",
	"vpa",
	"sc",
	"ind",
	"ri",
	"sgr",
	"hts",
	"wind",
	"ht",
	"tsl",
	"uc",
	"hu",
	"iprog",
	"ka1",
	"ka3",
	"kb2",
	"kc1",
	"kc3",
	"mc5p",
	"rmp",
	"acsc",
	"pln",
	"kcbt",
	"smxon",
	"rmxon",
	"smam",
	"rmam",
	"xonc",
	"xoffc",
	"enacs",
	"smln",
	"rmln",
	"kbeg",
	"kcan",
	"kclo",
	"kcmd",
	"kcpy",
	"kcrt",
	"kend",
	"kent",
	"kext",
	"kfnd",
	"khlp",
	"kmrk",
	"kmsg",
	"kmov",
	"knxt",
	"kopn",
	"kopt",
	"kprv",
	"kprt",
	"krdo",
	"kref",
	"krfr",
	"krpl",
	"krst",
	"kres",
	"ksav",
	"kspd",
	"kund",
	"kBEG",
	"kCAN",
	"kCMD",
	"kCPY",
	"kCRT",
	"kDC",
	"kDL",
	"kslt",
	"kEND",
	"kEOL",
	"kEXT",
	"kFND",
	"kHLP",
	"kHOM",
	"kIC",
	"kLFT",
	"kMSG",
	"kMOV",
	"kNXT",
	"kOPT",
	"kPRV",
	"kPRT",
	"kRDO",
	"kRPL",
	"kRIT",
	"kRES",
	"kSAV",
	"kSPD",
	"kUND",
	"rfi",
	"kf11",
	"kf12",
	"kf13",
	"kf14",
	"kf15",
	"kf16",
	"kf17",
	"kf18",
	"kf19",
	"kf20",
	"kf21",
	"kf22",
	"kf23",
	"kf24",
	"kf25",
	"kf26",
	"kf27",
	"kf28",
	"kf29",
	"kf30",
	"kf31",
	"kf32",
	"kf33",
	"kf34",
	"kf35",
	"kf36",
	"kf37",
	"kf38",
	"kf39",
	"kf40",
	"kf41",
	"kf42",
	"kf43",
	"kf44",
	"kf45",
	"kf46",
	"kf47",
	"kf48",
	"kf49",
	"kf50",
	"kf51",
	"kf52",
	"kf53",
	"kf54",
	"kf55",
	"kf56",
	"kf57",
	"kf58",
	"kf59",
	"kf60",
	"kf61",
	"kf62",
	"kf63",
	"el1",
	"mgc",
	"smgl",
	"smgr",
	"fln",
	"sclk",
	"dclk",
	"rmclk",
	"cwin",
	"wingo",
	"hup",
	"dial",
	"qdial",
	"tone",
	"pulse",
	"hook",
	"pause",
	"wait",
	"u0",
	"u1",
	"u2",
	"u3",
	"u4",
	"u5",
	"u6",
	"u7",
	"u8",
	"u9",
	"op",
	"oc",
	"initc",
	"initp",
	"scp",
	"setf",
	"setb",
	"cpi",
	"lpi",
	"chr",
	"cvr",
	"defc",
	"swidm",
	"sdrfq",
	"sitm",
	"slm",
	"smicm",
	"snlq",
	"snrmq",
	"sshm",
	"ssubm",
	"ssupm",
	"sum",
	"rwidm",
	"ritm",
	"rlm",
	"rmicm",
	"rshm",
	"rsubm",
	"rsupm",
	"rum",
	"mhpa",
	"mcud1",
	"mcub1",
	"mcuf1",
	"mvpa",
	"mcuu1",
	"porder",
	"mcud",
	"mcub",
	"mcuf",
	"mcuu",
	"scs",
	"smgb",
	"smgbp",
	"smglp",
	"smgrp",
	"smgt",
	"smgtp",
	"sbim",
	"scsd",
	"rbim",
	"rcsd",
	"subcs",
	"supcs",
	"docr",
	"zerom",
	"csnm",
	"kmous",
	"minfo",
	"reqmp",
	"getm",
	"setaf",
	"setab",
	"pfxl",
	"devt",
	"csin",
	"s0ds",
	"s1ds",
	"s2ds",
	"s3ds",
	"smglr",
	"smgtb",
	"birep",
	"binel",
	"bicr",
	"colornm",
	"defbi",
	"endbi",
	"setcolor",
	"slines",
	"dispc",
	"smpch",
	"rmpch",
	"smsc",
	"rmsc",
	"pctrm",
	"scesc",
	"scesa",
	"ehhlm",
	"elhlm",
	"elohlm",
	"erhlm",
	"ethlm",
	"evhlm",
	"sgr1",
	"slength",
	0
};

/*------------------------------------------------------------------------
 * list of terminfo directories to check
 */
static const char * tcap_ti_dirs[] =
{
	"/usr/share/lib/terminfo",
	"/usr/lib/terminfo",
	"/usr/share/terminfo",
	0
};

/*------------------------------------------------------------------------
 * stack operations
 */
#define	STK_SIZE		20		/* max entries in a stack */

struct stack
{
	int		cnt;				/* count of entries	*/
	int		nums[STK_SIZE];		/* stack entries	*/
};
typedef struct stack STACK;

#define STK_CLR(s)		( (s)->cnt = 0 )

#define STK_PUSH(s,n)	( (s)->cnt < STK_SIZE ? \
							(s)->nums[(s)->cnt++] = (n) : 0 )

#define STK_POP(s)		( (s)->cnt > 0 ? (s)->nums[--(s)->cnt] : 0 )

/*------------------------------------------------------------------------
 * fix a short if needed
 */
static void tcap_fix_2byte (short *tp)
{
	int	one				= 1;
	unsigned char *	op	= (unsigned char *)&one;

	/*--------------------------------------------------------------------
	 * The struct is always stored in "little-endian" format.
	 *
	 * If we are big-endian, swap the bytes.
	 */
	if (*op == 0)
	{
		unsigned char *	cp	= (unsigned char *)tp;
		unsigned char	c1	= cp[0];
		unsigned char	c2	= cp[1];

		cp[0] = c2;
		cp[1] = c1;
	}
}

/*------------------------------------------------------------------------
 * read in a terminfo struct
 */
static int tcap_ti_read (char *buf, const char *path)
{
	TI_HDR *	ti = (TI_HDR *)buf;
	FILE *		fp;
	short *		tp;
	int			size;
	int			i;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (buf == 0 || path == 0 || *path == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * open the file
	 */
	fp = fopen(path, "rb");
	if (fp == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * read in the buffer
	 */
	size = fread(buf, 1, TCAP_BUFLEN, fp);

	/*--------------------------------------------------------------------
	 * close the file
	 */
	fclose(fp);

	/*--------------------------------------------------------------------
	 * adjust the header
	 */
	tcap_fix_2byte(&ti->magic);
	tcap_fix_2byte(&ti->name_size);
	tcap_fix_2byte(&ti->bool_count);
	tcap_fix_2byte(&ti->nums_count);
	tcap_fix_2byte(&ti->offs_count);
	tcap_fix_2byte(&ti->strs_size);

	/*--------------------------------------------------------------------
	 * validate the header
	 */
	if (ti->magic != TI_MAGIC)
	{
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * fix the nums
	 */
	tp = TI_NUMS(ti);
	for (i = 0; i < ti->nums_count; i++)
	{
		tcap_fix_2byte(tp + i);
	}

	/*--------------------------------------------------------------------
	 * fix the string offsets
	 */
	tp = TI_OFFS(ti);
	for (i = 0; i < ti->offs_count; i++)
	{
		tcap_fix_2byte(tp + i);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * find a terminfo bool entry
 */
static int tcap_ti_bool (const char *buf, const char *name)
{
	const TI_HDR *	ti		= (const TI_HDR *)buf;
	const char *	bools	= TI_BOOLS(ti);
	int				i;

	/*--------------------------------------------------------------------
	 * look through name table for entry
	 */
	for (i=0; tcap_ti_bool_names[i]; i++)
	{
		if (strcmp(tcap_ti_bool_names[i], name) == 0)
		{
			if (i >= ti->bool_count)
				break;

			return (bools[i]);
		}
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * find a terminfo numeric entry
 */
static int tcap_ti_num (const char *buf, const char *name)
{
	const TI_HDR *	ti		= (const TI_HDR *)buf;
	const short *	nums	= TI_NUMS(ti);
	int				i;

	/*--------------------------------------------------------------------
	 * look through name table for entry
	 */
	for (i=0; tcap_ti_num_names[i]; i++)
	{
		if (strcmp(tcap_ti_num_names[i], name) == 0)
		{
			if (i >= ti->nums_count)
				break;

			return (nums[i]);
		}
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * find a terminfo string entry
 */
static const char * tcap_ti_str (const char *buf, const char *name)
{
	const TI_HDR *	ti		= (const TI_HDR *)buf;
	const short *	offs	= TI_OFFS(ti);
	const char *	strs	= TI_STRS(ti);
	int				i;

	/*--------------------------------------------------------------------
	 * special case of "desc"
	 */
	if (strcmp("desc", name) == 0)
	{
		const char *	n = TI_NAMES(ti);
		const char *	p;

		p = strrchr(n, '|');
		if (p != 0)
			n = p + 1;

		return (n);
	}

	/*--------------------------------------------------------------------
	 * look through name table for entry
	 */
	for (i=0; tcap_ti_str_names[i]; i++)
	{
		if (strcmp(tcap_ti_str_names[i], name) == 0)
		{
			if (i >= ti->offs_count)
				break;

			if (offs[i] == -1)
				return (0);

			return (strs + offs[i]);
		}
	}

	return ((char *)-1);
}

/*------------------------------------------------------------------------
 * parameterize an entry
 *
 * This routine does NOT support the static variable ops:
 *
 *	%g[A-Z]		get static var
 *	%P[A-Z]		put static var
 */
static char * tcap_ti_parm (char *buf, const char *str, const int *p)
{
	STACK			st;				/* regular stack */
	STACK			si;				/* if-then stack */
	STACK *			tp;				/* current stack pointer */
	int				params[26];		/* dynamic parameter array */
	char			fmt[128];		/* format string buffer */
	char			num[24];		/* number buffer */
	const char *	s;
	const char *	save_s;
	char *			f;
	char *			b;
	int				if_state;		/* result of if/then */
	int				n;
	int				n1;
	int				n2;
	int				pp[2];

	/*--------------------------------------------------------------------
	 * clear the stack & param array
	 */
	tp = &st;
	STK_CLR(tp);

	for (n=0; n<26; n++)
		params[n] = 0;

	/*--------------------------------------------------------------------
	 * cache parms
	 */
	pp[0] = p[0];
	pp[1] = p[1];

	/*--------------------------------------------------------------------
	 * now do it
	 */
	b = buf;

	for (s=str; *s; s++)
	{
		if (*s != '%')
		{
			*b++ = *s;
		}
		else if (s[1] == 0)
		{
			*b++ = *s;
		}
		else
		{
			s++;
			switch (*s)
			{
			/*------------------------------------------------------------
			 * %% - output a '%'
			 */
			case '%':
				*b++ = *s;
				break;

			/*------------------------------------------------------------
			 * %c - output pop as a char
			 */
			case 'c':
				*b++ = STK_POP(tp);
				break;

			/*------------------------------------------------------------
			 * %'c' - push char constant c (c may be escaped char)
			 */
			case '\'':
				n = 0;
				for (s++; *s; s++)
				{
					if (*s == '\'')
						break;

					if (*s != '\\')
					{
						n = *s;
					}
					else
					{
						s++;
						switch (*s)
						{
						case 'n':
							n = '\n';
							break;

						case 't':
							n = '\t';
							break;

						case 'r':
							n = '\r';
							break;

						case 'f':
							n = '\f';
							break;

						case 'b':
							n = '\b';
							break;

						case 's':
							n = ' ';
							break;

						case '\\':
							n = '\\';
							break;

						case '0':
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
							n = 0;
							for (; *s; s++)
							{
								if (*s < '0' || *s > '7')
								{
									s--;
									break;
								}
								else
								{
									n = (n * 8) + (*s - '0');
								}
							}
							break;

						default:
							n = *s;
							break;
						}
					}
				}
				STK_PUSH(tp, n);
				break;

			/*------------------------------------------------------------
			 * %{nn} - push decimal constant nn
			 */
			case '{':
				n = 0;
				for (s++; *s; s++)
				{
					if (! isdigit(*s))
						break;

					n = (n * 10) + (*s - '0');
				}
				for (; *s; s++)
				{
					if (*s == '}')
						break;
				}
				STK_PUSH(tp, n);
				break;

			/*------------------------------------------------------------
			 * %+ %- %* %/ %m - arithmetic (m is mod) -
			 *					push (pop int2 op pop int1)
			 */
			case '+':
				n1 = STK_POP(tp);
				n2 = STK_POP(tp);
				STK_PUSH(tp, n2 + n1);
				break;

			case '-':
				n1 = STK_POP(tp);
				n2 = STK_POP(tp);
				STK_PUSH(tp, n2 - n1);
				break;

			case '*':
				n1 = STK_POP(tp);
				n2 = STK_POP(tp);
				STK_PUSH(tp, n2 * n1);
				break;

			case '/':
				n1 = STK_POP(tp);
				n2 = STK_POP(tp);
				if (n1 == 0)
					STK_PUSH(tp, 0);
				else
					STK_PUSH(tp, n2 / n1);
				break;

			case 'm':
				n1 = STK_POP(tp);
				n2 = STK_POP(tp);
				if (n1 == 0)
					STK_PUSH(tp, 0);
				else
					STK_PUSH(tp, n2 % n1);
				break;

			/*------------------------------------------------------------
			 * %& %| %^ - bit operations - push (pop int2 op pop int1)
			 */
			case '&':
				n1 = STK_POP(tp);
				n2 = STK_POP(tp);
				STK_PUSH(tp, n2 & n1);
				break;

			case '|':
				n1 = STK_POP(tp);
				n2 = STK_POP(tp);
				STK_PUSH(tp, n2 | n1);
				break;

			case '^':
				n1 = STK_POP(tp);
				n2 = STK_POP(tp);
				STK_PUSH(tp, n2 ^ n1);
				break;

			/*------------------------------------------------------------
			 * %! %~ - unary operations - push (op pop)
			 */
			case '!':
				n1 = STK_POP(tp);
				STK_PUSH(tp, ! n1);
				break;

			case '~':
				n1 = STK_POP(tp);
				STK_PUSH(tp, ~ n1);
				break;

			/*------------------------------------------------------------
			 * %< %> %= - logical operations - push (pop int2 op pop int1)
			 */
			case '<':
				n1 = STK_POP(tp);
				n2 = STK_POP(tp);
				STK_PUSH(tp, n2 < n1);
				break;

			case '>':
				n1 = STK_POP(tp);
				n2 = STK_POP(tp);
				STK_PUSH(tp, n2 > n1);
				break;

			case '=':
				n1 = STK_POP(tp);
				n2 = STK_POP(tp);
				STK_PUSH(tp, n2 == n1);
				break;

			/*------------------------------------------------------------
			 * %A %O - and & or operations - push (pop int 2 op pop int1)
			 */
			case 'A':
				n1 = STK_POP(tp);
				n2 = STK_POP(tp);
				STK_PUSH(tp, n2 && n1);
				break;

			case 'O':
				n1 = STK_POP(tp);
				n2 = STK_POP(tp);
				STK_PUSH(tp, n2 || n1);
				break;

			/*------------------------------------------------------------
			 * %i - increment - increment parameters p1 & p2 by 1
			 */
			case 'i':
				pp[0]++;
				pp[1]++;
				break;

			/*------------------------------------------------------------
			 * %l - string length
			 */
			case 'l':
				sprintf(num, "%d", STK_POP(tp));
				STK_PUSH(tp, strlen(num));
				break;

			/*------------------------------------------------------------
			 * %p[1-2] - push parameter n
			 */
			case 'p':
				if (s[1] == 0)
					n = 0;
				else
					n = (*++s) - '0';
				if (n < 1 || n > 2)
					STK_PUSH(tp, 0);
				else
					STK_PUSH(tp, pp[n-1]);
				break;

			/*------------------------------------------------------------
			 * %P[a-z] - set dynamic variable - set var[a-z] to pop
			 * %P[A-Z] - set static  variable - ignored (after pop)
			 */
			case 'P':
				n = *s++;
				if (isalpha(n) && islower(n))
					params[n - 'a'] = STK_POP(tp);
				else
					STK_POP(tp);
				break;

			/*------------------------------------------------------------
			 * %g[a-z] - get dynamic variable - push var[a-z]
			 * %g[A-Z] - get static  variable - ignored (after push(0))
			 */
			case 'g':
				n = *s++;
				if (isalpha(n) && islower(n))
					STK_PUSH(tp, params[n - 'a']);
				else
					STK_PUSH(tp, 0);
				break;

			/*------------------------------------------------------------
			 * %? %t %e %; - if then else endif
			 */
			case '?':
				tp = &si;		/* set stack pointer to if-stack */
				STK_CLR(tp);
				if_state = FALSE;
				break;

			case 't':
				if_state = STK_POP(tp);
				if (! if_state)
				{
					/*----------------------------------------------------
					 * scan to next else or endif
					 */
					for (; *s; s++)
					{
						if (s[1] == '%' && (s[2] == 'e' || s[2] == ';'))
							break;
					}
				}
				break;

			case 'e':
				if (if_state)
				{
					/*----------------------------------------------------
					 * scan to endif
					 */
					for (; *s; s++)
					{
						if (s[1] == '%' && s[2] == ';')
							break;
					}
				}
				break;

			case ';':
				tp = &st;		/* restore stack pointer to reg stack */
				break;

			/*------------------------------------------------------------
			 * %[[:]flags][width][.prec]][doxXs] - printf format for pop
			 */
			default:
				save_s = s;

				if (*s == ':')
					s++;

				f = fmt;
				*f++ = '%';
				for (; *s; s++)
				{
					*f++ = *s;
					if (*s == 'd' ||
					    *s == 'o' ||
					    *s == 'x' ||
					    *s == 'X' ||
						*s == 's')
					{
						break;
					}
				}
				*f = 0;

				if (*s == 0)
				{
					s = save_s;
					break;
				}

			 	if (*s == 's')
				{
					sprintf(num, "%d", STK_POP(tp));
					b += sprintf(b, fmt, num);
				}
				else
				{
					b += sprintf(b, fmt, STK_POP(tp));
				}
				break;
			}
		}

		if (*s == 0)
			break;
	}
	*b = 0;

	return (buf);
}

/*------------------------------------------------------------------------
 * tcap_tigetent() - read in a term entry
 */
int tcap_tigetent (char *buf, const char *term, const char *path)
{
	char			ti_path[MAX_PATHLEN];
	const char **	ts;
	int				rc;

	/*--------------------------------------------------------------------
	 * find the proper path to use
	 */
	rc = FALSE;
	if (path == 0 || *path == 0)
		path = getenv("TERMINFO");

	if (path != 0 && *path != 0)
	{
		if (tcap_trmcmp(path, "none") == 0)
			return (-1);
	}

	if (path != 0 && *path != 0)
	{
		sprintf(ti_path, "%s/%c/%s", path, *term, term);
		rc = tcap_path_exists(ti_path);
	}

	if (! rc)
	{
		for (ts=tcap_ti_dirs; *ts; ts++)
		{
			sprintf(ti_path, "%s/%c/%s", *ts, *term, term);
			rc = tcap_path_exists(ti_path);
			if (rc)
				break;

			/*------------------------------------------------------------
			 * OSX location
			 */
			sprintf(ti_path, "%s/%02x/%s", *ts, *term, term);
			rc = tcap_path_exists(ti_path);
			if (rc)
				break;
		}
	}

	if (! rc)
		return (-1);

	/*--------------------------------------------------------------------
	 * now load the file
	 */
	rc = tcap_ti_read(buf, ti_path);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_tigetflag() - get a boolean entry from a terminfo buffer
 */
int tcap_tigetflag (const char *buf, const char *name)
{
	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (name == 0 || *name == 0)
		return (FALSE);

	return tcap_ti_bool(buf, name);
}

/*------------------------------------------------------------------------
 * tcap_tigetnum() - get a numeric entry from a terminfo buffer
 */
int tcap_tigetnum (const char *buf, const char *name)
{
	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (name == 0 || *name == 0)
		return (-1);

	return tcap_ti_num(buf, name);
}

/*------------------------------------------------------------------------
 * tcap_tigetstr() - get string entry from a terminfo buffer
 */
const char * tcap_tigetstr (const char *buf, const char *name)
{
	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (name == 0 || *name == 0)
		return (0);

	return tcap_ti_str(buf, name);
}

/*------------------------------------------------------------------------
 * tcap_tigoto() - parameterize an entry
 */
char * tcap_tigoto (char *buf, const char *str, const int *p)
{
	return tcap_ti_parm(buf, str, p);
}

/*------------------------------------------------------------------------
 * tcap_tiputs() - output a command string
 */
int tcap_tiputs (TERMINAL *tp, const char *str, int (*rtn)(TERMINAL *tp, int c))
{
	const unsigned char *	s;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0)
		return (-1);

	if (str == 0 || *str == 0)
		return (0);

	/*--------------------------------------------------------------------
	 * output the string
	 */
	s = (const unsigned char *)str;
	for (; *s; s++)
	{
		/*----------------------------------------------------------------
		 * check for delay parameter
		 */
		if (s[0] == '$' && s[1] == '<')
		{
			int	pad;

			/*------------------------------------------------------------
			 * get pad value
			 */
			pad = 0;
			for (s += 2; *s; s++)
			{
				if (! isdigit(*s))
					break;

				pad = (pad * 10) + (*s - '0');
			}

			for (; *s; s++)
			{
				if (*s == '>')
					break;
			}

			/*------------------------------------------------------------
			 * now output the padding
			 */
			tcap_delay_output(tp, pad, rtn);

			continue;
		}

		/*----------------------------------------------------------------
		 * output the character
		 */
		(*rtn)(tp, *s);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_tidump() - dump a terminfo buffer to a stream
 */
void tcap_tidump (const char *buf, const char *term, FILE *fp)
{
	const TI_HDR *	ti = (const TI_HDR *)buf;
	const char *	names;
	const char *	bools;
	const short *	nums;
	const short *	offs;
	const char *	strs;
	int				i;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (ti == 0)
		return;

	names	= TI_NAMES(ti);
	bools	= TI_BOOLS(ti);
	nums	= TI_NUMS(ti);
	offs	= TI_OFFS(ti);
	strs	= TI_STRS(ti);

	/*--------------------------------------------------------------------
	 * tell what we are dumping
	 */
	fprintf(fp, "---------------------------------\n");
	fprintf(fp, "dump of terminfo entry for \"%s\"\n", term);
	fprintf(fp, "\n");

	/*--------------------------------------------------------------------
	 * dump the header
	 */
	fprintf(fp, "header:\n");
	fprintf(fp, "  magic      = %04x\n", ti->magic);
	fprintf(fp, "  name_size  = %d\n",   ti->name_size);
	fprintf(fp, "  bool_count = %d\n",   ti->bool_count);
	fprintf(fp, "  nums_count = %d\n",   ti->nums_count);
	fprintf(fp, "  offs_count = %d\n",   ti->offs_count);
	fprintf(fp, "  strs_size  = %d\n",   ti->strs_size);
	fprintf(fp, "\n");

	/*--------------------------------------------------------------------
	 * dump the names
	 */
	fprintf(fp, "names = \"%s\"\n", names);
	fprintf(fp, "\n");

	/*--------------------------------------------------------------------
	 * dump the bools
	 */
	fprintf(fp, "bools:\n");
	for (i = 0; i < ti->bool_count; i++)
	{
		if (bools[i])
			fprintf(fp, "  %s\n", tcap_ti_bool_names[i]);
	}
	fprintf(fp, "\n");

	/*--------------------------------------------------------------------
	 * dump the nums
	 */
	fprintf(fp, "nums:\n");
	for (i = 0; i < ti->nums_count; i++)
	{
		if (nums[i] > 0)
			fprintf(fp, "  %-24s = %d\n", tcap_ti_num_names[i], nums[i]);
	}
	fprintf(fp, "\n");

	/*--------------------------------------------------------------------
	 * dump the strings
	 */
	fprintf(fp, "strs:\n");
	for (i = 0; i < ti->offs_count; i++)
	{
		char str[256];

		if (offs[i] >= 0)
		{
			fprintf(fp, "  %-24s = \"%s\"\n", tcap_ti_str_names[i],
				tcap_print_str(strs + offs[i], str));
		}
	}
	fprintf(fp, "\n");
}
