/*------------------------------------------------------------------------
 * string functions
 */
#include "xvtcommon.h"

/*------------------------------------------------------------------------
 * compare two strings (case insensitive)
 */
int xvt_strccmp (const char *s1, const char *s2)
{
	if (s1 == 0)	s1 = "";
	if (s2 == 0)	s2 = "";

	while (*s1 && *s2)
	{
		int c = (tolower(*s1) - tolower(*s2));

		if (c)
			return (c);

		s1++;
		s2++;
	}

	return (tolower(*s1) - tolower(*s2));
}

/*------------------------------------------------------------------------
 * compare two strings (case insensitive), ignoring white-space, "-", & "_"
 */
int xvt_strccmpc (const char *s1, const char *s2)
{
	if (s1 == 0)	s1 = "";
	if (s2 == 0)	s2 = "";

	for (;;)
	{
		int c;

		if (*s1 == '-' || *s1 == '_' || isspace(*s1))
		{
			s1++;
			continue;
		}

		if (*s2 == '-' || *s2 == '_' || isspace(*s2))
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
 * compare two strings (case insensitive), ignoring white-space, "-", & "_"
 * Compare stops at end of first string.
 */
int xvt_strccmpcn (const char *s1, const char *s2)
{
	if (s1 == 0)	s1 = "";
	if (s2 == 0)	s2 = "";

	for (;;)
	{
		int c;

		if (*s1 == '-' || *s1 == '_' || isspace(*s1))
		{
			s1++;
			continue;
		}

		if (*s2 == '-' || *s2 == '_' || isspace(*s2))
		{
			s2++;
			continue;
		}

		c = (tolower(*s1) - tolower(*s2));
		if (c)
			return (c);

		if (*s1 == 0)
			break;

		s1++;
		s2++;
	}

	return (0);
}

/*------------------------------------------------------------------------
 * compare two strings (case insensitive) for n chars
 */
int xvt_strnccmp (const char *s1, const char *s2, int n)
{
	if (s1 == 0)	s1 = "";
	if (s2 == 0)	s2 = "";

	while (*s1 && *s2)
	{
		int c = (tolower(*s1) - tolower(*s2));

		if (c)
			return (c);

		if (n-- == 0)
			return (0);

		s1++;
		s2++;
	}

	return (n == 0 ? 0 : tolower(*s1) - tolower(*s2));
}

/*------------------------------------------------------------------------
 * compare two strings for length of first string (case insensitive)
 */
int xvt_strccmpn (const char *s1, const char *s2)
{
	if (s1 == 0)	s1 = "";
	if (s2 == 0)	s2 = "";

	while (*s1)
	{
		int c = (tolower(*s1) - tolower(*s2));

		if (c)
			return (c);

		s1++;
		s2++;
	}

	return (0);
}

/*------------------------------------------------------------------------
 * strip trailing (& optionally leading) white-space from a string
 */
char * xvt_strip (char *str, int do_leading)
{
	char *	lastnsp = str;
	char *	s;

	if (str == 0)
		return (0);

	if (do_leading)
	{
		for (; *str; str++)
		{
			if (! isspace(*str))
				break;
		}
	}

	for (s = str; *s; s++)
	{
		if (! isspace(*s))
			lastnsp = s + 1;
	}
	*lastnsp = 0;

	return (str);
}

/*------------------------------------------------------------------------
 * like strchr(), but doesn't check escaped chars or chars in quotes
 */
char * xvt_strchr (const char *s, int c)
{
	const char *	p;
	int				in_quotes	= FALSE;

	if (s == 0)
		return (0);

	for (p=s; *p; p++)
	{
		if (*p == '\\')
		{
			if (p[1] != 0)
				p++;
			continue;
		}

		if (in_quotes)
		{
			if (*p == '"')
				in_quotes = FALSE;
			continue;
		}

		if (*p == '"')
		{
			in_quotes = TRUE;
			continue;
		}

		if (*p == c)
			return ((char *)p);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * like strpbrk(), but doesn't check escaped chars or chars in quotes
 */
char * xvt_strpbrk (const char *s1, const char *s2)
{
	const char *	p;
	int				in_quotes	= FALSE;

	if (s1 == 0 || s2 == 0)
		return (0);

	for (p=s1; *p; p++)
	{
		if (*p == '\\')
		{
			if (p[1] != 0)
				p++;
			continue;
		}

		if (in_quotes)
		{
			if (*p == '"')
				in_quotes = FALSE;
			continue;
		}

		if (*p == '"')
		{
			in_quotes = TRUE;
			continue;
		}

		if (strchr(s2, *p) != 0)
			return ((char *)p);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * like strstr(), but case-insensitive
 */
char * xvt_strstr (const char *s1, const char *s2)
{
	const char *se;
	int			l1;
	int			l2;

	if (s1 == 0 || s2 == 0)
		return (0);

	l1	= strlen(s1);
	l2	= strlen(s2);

	if (l1 < l2)
		return (0);
	se = s1 + (l2 - l1 + 1);

	for (; s1 < se; s1++)
	{
		if (xvt_strccmpn(s2, s1) == 0)
			return ((char *)s1);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * like strcpy(), but doesn't copy quotes or leading/trailing white-space.
 * Returns # chars copied (not counting the NULL at the end).
 */
int xvt_strcpy (char *s1, const char *s2)
{
	char *	s		= s1;
	char *	lastnsp = s1;

	if (s1 == 0 || s2 == 0)
		return (0);

	for (; *s2; s2++)
	{
		if (! isspace(*s2))
			break;
	}

	for (; *s2; s2++)
	{
		if (*s2 == '\\')
		{
			if (s2[1] != 0)
			{
				*s++ = s2[1];
				if (! isspace(s2[1]))
					lastnsp = s;
			}
			s2++;
			continue;
		}

		if (*s2 == '"')
			continue;

		*s++ = *s2;
		if (! isspace(*s2))
			lastnsp = s;
	}
	*lastnsp = 0;

	return (lastnsp - s1);
}

/*------------------------------------------------------------------------
 * like strncpy(), but doesn't copy quotes or leading/trailing white-space.
 * Returns # chars copied (not counting the NULL at the end).
 */
int xvt_strncpy (char *s1, const char *s2, int n)
{
	char *	s		= s1;
	char *	lastnsp = s1;

	if (s1 == 0 || s2 == 0 || n <= 0)
		return (0);

	for (; *s2; s2++)
	{
		if (! isspace(*s2))
			break;
	}

	for (; *s2 && n > 0; s2++)
	{
		if (*s2 == '\\')
		{
			if (s2[1] != 0)
			{
				*s++ = s2[1];
				n--;
				if (! isspace(s2[1]))
					lastnsp = s;
			}
			s2++;
			continue;
		}

		if (*s2 == '"')
			continue;

		*s++ = *s2;
		n--;
		if (! isspace(*s2))
			lastnsp = s;
	}
	*lastnsp = 0;

	return (lastnsp - s1);
}

/*------------------------------------------------------------------------
 * convert a string to a bool value
 *
 * returns: TRUE / FALSE / -1 (error)
 */
static const char * xvt_str_bool_ons[] =
{
	"on",
	"yes",
	"true",

#if FUNNY
	"uh-huh",
	"OK",
	"all-right",
	"if-you-insist",
#endif

#if INTL
	"ja",
	"da",
	"si",
	"oui",
#endif

	0
};

static const char * xvt_str_bool_offs[] =
{
	"off",
	"no",
	"false",

#if FUNNY
	"uh-uh",
	"no-way",
#endif

#if INTL
	"nein",
	"nyet",
	"non",
#endif

	0
};

int xvt_str_to_bool (const char *str)
{
	const char **	s;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (str == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * skip over leading white-space & quotes
	 */
	for (; *str; str++)
	{
		if (isspace(*str) || *str == '"')
			continue;
		break;
	}

	if (*str == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * check if numeric
	 */
	if (isdigit(*str))
	{
		return atoi(str);
	}

	/*--------------------------------------------------------------------
	 * check on strings
	 */
	for (s = xvt_str_bool_ons; *s; s++)
	{
		if (xvt_strccmp(str, *s) == 0)
			return (TRUE);
	}

	/*--------------------------------------------------------------------
	 * check off strings
	 */
	for (s = xvt_str_bool_offs; *s; s++)
	{
		if (xvt_strccmp(str, *s) == 0)
			return (FALSE);
	}

	return (-1);
}
