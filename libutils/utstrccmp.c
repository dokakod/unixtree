/*------------------------------------------------------------------------
 * case-insensitive string compare routines
 *
 *	strccmp()	- compare two strings
 *	strccmpn()	- compare two strings for the length of the first
 */
#include "utcommon.h"

#define IGNORE_CHAR(c)	(isspace((c)) || (c) == '-' || (c) == '_')

/*------------------------------------------------------------------------
 * strccmp() - compare two strings, ignoring case
 */
int strccmp (const char *s1, const char *s2)
{
	while (*s1 && *s2)
	{
		register int c = (tolower(*s1) - tolower(*s2));

		if (c)
			return (c);

		s1++;
		s2++;
	}

	return (tolower(*s1) - tolower(*s2));
}

/*------------------------------------------------------------------------
 * strccmpn() - compare two strings for length of the first, ignoring case
 */
int strccmpn (const char *s1, const char *s2)
{
	while (*s1)
	{
		register int c = (tolower(*s1) - tolower(*s2));

		if (c)
			return (c);

		s1++;
		s2++;
	}

	return (0);
}

/*------------------------------------------------------------------------
 * strxcmpn() - compare two strings, ignoring case & white-space, -, & _
 */
int strxcmp (const char *s1, const char *s2)
{
	while (TRUE)
	{
		register int c;

		if (IGNORE_CHAR(*s1))
		{
			s1++;
			continue;
		}

		if (IGNORE_CHAR(*s2))
		{
			s2++;
			continue;
		}

		if (*s1 == 0 || *s2 == 0)
			break;

		c = (tolower(*s1) - tolower(*s2));
		if (c)
			return (c);

		s1++;
		s2++;
	}

	return (tolower(*s1) - tolower(*s2));
}

/*------------------------------------------------------------------------
 * strxcmpn() - compare two strings for length of the first, ignoring case
 */
int strxcmpn (const char *s1, const char *s2)
{
	while (*s1)
	{
		register int c;

		if (IGNORE_CHAR(*s1))
		{
			s1++;
			continue;
		}

		if (IGNORE_CHAR(*s2))
		{
			s2++;
			continue;
		}

		c = (tolower(*s1) - tolower(*s2));
		if (c)
			return (c);

		s1++;
		s2++;
	}

	return (0);
}
