/*------------------------------------------------------------------------
 * filename matching routines
 */
#include "oscommon.h"

/*------------------------------------------------------------------------
 * Note that this evalutes "c" many times.
 */
#define FOLD(c)	((flags & FNM_CASEFOLD) && isupper(c) ? tolower(c) : (c))

/*------------------------------------------------------------------------
 * Match STRING against the filename pattern PATTERN, returning zero if
 * it matches, nonzero if not.
 */
int
os_fn_match (const char *pattern, const char *string, int flags)
{
	const char *p = pattern;
	const char *n = string;
	char c;

	while ((c = *p++) != '\0')
	{
		c = FOLD(c);

		switch (c)
		{
		case '?':
			if (*n == '\0')
				return FNM_NOMATCH;
			else if ((flags & FNM_PATHNAME) && *n == '/')
				return FNM_NOMATCH;
			else if ((flags & FNM_PERIOD) && *n == '.' &&
			    (n == string || ((flags & FNM_PATHNAME) && n[-1] == '/')))
				return FNM_NOMATCH;
			break;

		case '\\':
			if (!(flags & FNM_NOESCAPE))
			{
				c = *p++;
				c = FOLD(c);
			}
			if (FOLD(*n) != c)
				return FNM_NOMATCH;
			break;

		case '*':
			if ((flags & FNM_PERIOD) && *n == '.' &&
			    (n == string || ((flags & FNM_PATHNAME) && n[-1] == '/')))
				return FNM_NOMATCH;

			for (c = *p++; c == '?' || c == '*'; c = *p++, n++)
				if (((flags & FNM_PATHNAME) && *n == '/') ||
				    (c == '?' && *n == '\0'))
					return FNM_NOMATCH;

			if (c == '\0')
				return 0;

			{
				char c1 = (!(flags & FNM_NOESCAPE) && c == '\\') ? *p : c;
				c1 = FOLD(c1);
				for (--p; *n != '\0'; n++)
					if ((c == '[' || FOLD(*n) == c1) &&
					    os_fn_match(p, n, flags & ~FNM_PERIOD) == 0)
						return 0;
				return FNM_NOMATCH;
			}

		case '[':
			{
				/* Nonzero if the sense of the character class is inverted.  */
				register int notc;

				if (*n == '\0')
					return FNM_NOMATCH;

				if ((flags & FNM_PERIOD) && *n == '.' &&
				    (n == string || ((flags & FNM_PATHNAME) && n[-1] == '/')))
					return FNM_NOMATCH;

				notc = (*p == '!' || *p == '^');
				if (notc)
					p++;

				c = *p++;
				for (;;)
				{
					char cstart = c;
					char cend   = c;

					if (!(flags & FNM_NOESCAPE) && c == '\\')
						cstart = cend = *p++;

					cstart = cend = FOLD(cstart);

					if (c == '\0')
						/* [ (unterminated) loses.  */
						return FNM_NOMATCH;

					c = *p++;
					c = FOLD(c);

					if ((flags & FNM_PATHNAME) && c == '/')
						/* [/] can never match.  */
						return FNM_NOMATCH;

					if (c == '-' && *p != ']')
					{
						cend = *p++;
						if (!(flags & FNM_NOESCAPE) && cend == '\\')
							cend = *p++;
						if (cend == '\0')
							return FNM_NOMATCH;
						cend = FOLD(cend);

						c = *p++;
					}

					if (FOLD(*n) >= cstart && FOLD(*n) <= cend)
						goto matched;

					if (c == ']')
						break;
				}
				if (!notc)
					return FNM_NOMATCH;
				break;

matched:
				;
				/* Skip the rest of the [...] that already matched.  */
				while (c != ']')
				{
					if (c == '\0')
						/* [... (unterminated) loses.  */
						return FNM_NOMATCH;

					c = *p++;
					if (!(flags & FNM_NOESCAPE) && c == '\\')
						/* XXX 1003.2d11 is unclear if this is right.  */
						p++;
				}
				if (notc)
					return FNM_NOMATCH;
			}
			break;

		default:
			if (c != FOLD(*n))
				return FNM_NOMATCH;
		}

		n++;
	}

	if (*n == '\0')
		return 0;

	if ((flags & FNM_LEADING_DIR) && *n == '/')
	{
		/* FNM_LEADING_DIR flag says that "foo*" matches "foobar/frobozz". */
		return 0;
	}

	return FNM_NOMATCH;
}
