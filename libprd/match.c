/*------------------------------------------------------------------------
 * match filenames
 */
#include "libprd.h"

/*
 *	match names to a pattern
 *
 *	default pattern is file_spec.
 *	pattern is of form xxxx, xxxxx, ...
 *	where xxxxx may contain * ? and [chars]
 */

#define PATTERN_SEP(p)	((p) == ' ' || (p) == ',')

int match (const char *name, const char *pattern)
{
	int invert;

	while (*pattern)
	{
		invert = gbl(scr_cur)->invert_file_spec;
		if (*pattern == '-')			/* check for inversion */
		{
			invert = !invert;			/* invert invert */
			pattern++;
		}

		if (match_names(name, pattern))		/* check next pattern */
		{
			if (!invert)
				return (1);
		}
		else
		{
			if (invert)
				return (1);
		}

		/* skip to end of section of pattern */
		while (*pattern && !PATTERN_SEP(*pattern))
			pattern++;

		/* skip over any white space & separators */
		while (*pattern && PATTERN_SEP(*pattern))
			pattern++;
	}
	return (0);
}

/* check against a single pattern */
int match_names (const char *name, const char *pattern)
{
	char c;
	const char *e;
	const char *t;
	int no;

	for (; (c = *pattern) != 0 && !PATTERN_SEP(c); pattern++)
	{
		if (*name == 0)
		{
			if (c == '*' && (PATTERN_SEP(pattern[1]) || pattern[1] == 0) )
				return (1);

			return (0);
		}

		switch (c)
		{
		case '?':
					name++;
					break;
		case '*':
					/* look for last occurance of next char in string */

					/* return if * was last char in pattern */
					c = *++pattern;
					if (!c || PATTERN_SEP(c))
						return (1);
					/* scan fwd, looking for last occurance of c */
					t = 0;
					for (e=name; *e; e++)
					{
						if (*e == c)
							t = e;
					}
					if (!t)
						return (0);
					name = t+1;
					break;
		case '[':
					no = FALSE;
					c = *++pattern;
					if (c == '^')			/* check for negation */
					{
						no = TRUE;
						c = *++pattern;
					}
					while (c && !PATTERN_SEP(c))
					{
						if (c == ']')
						{
							if (no)
								break;
							else
								return (0);
						}
						if (no)
						{
							if (*name == c)
								return (0);
						}
						else
						{
							if (*name == c)
								break;
						}
						c = *++pattern;
					}
					while (c != ']')
						c = *++pattern;
					name++;
					break;
		default:
					if (*name++ != c)
						return (0);
					break;
		}
	}

	/* tar rtn could check a full pathname */
	if (*name && !fn_is_path_absolute(name))
		return (0);
	else
		return (1);
}
