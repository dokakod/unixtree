/*------------------------------------------------------------------------
 * string routines
 */
#include "tcapcommon.h"

#define IS_OCT(c)	((c) >= '0' && (c) <= '7')
#define get_hex(c)	(isdigit(c)? ((c)-'0') : (tolower((c))-'a'+10))

/*------------------------------------------------------------------------
 * tcap_print_str() - routine to make a string printable
 */
char *tcap_print_str (const char *s, char *buf)
{
	const unsigned char *	us	= (const unsigned char *)s;
	char *					p;

	for (p=buf; *us; us++)
	{
		if (*us < ' ')		/* NOT iscntrl() */
		{
			switch (*us)
			{
			case 0x1b:
				*p++ = '\\';
				*p++ = 'e';
				break;

			default:
				*p++ = '^';
				*p++ = *us + '@';
				break;
			}
		}
		else if (*us == 0x7f)
		{
			*p++ = '^';
			*p++ = '?';
		}
		else if (*us >= 0x80)
		{
			*p++ = '\\';
			*p++ = 'x';
			*p++ = "0123456789abcdef"[(*us >> 4) & 0x0f];
			*p++ = "0123456789abcdef"[(*us     ) & 0x0f];
		}
		else
		{
			*p++ = *us;
		}
	}
	*p = 0;

	return (buf);
}

/*------------------------------------------------------------------------
 * tcap_trmcmp() - compare two strings, ignoring space, case, "-", & '_"
 */
int tcap_trmcmp (const char *a, const char *b)
{
	while (TRUE)
	{
		int c;

		if (*a == '-' || *a == '_' || isspace(*a))
		{
			a++;
			continue;
		}

		if (*b == '-' || *b == '_' || isspace(*b))
		{
			b++;
			continue;
		}

		c = (tolower(*a) - tolower(*b));
		if (c)
			return (c);

		if (*a == 0)
			break;

		a++;
		b++;
	}

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_trmncmp() - compare two strings, ignoring space, case, "-", & '_"
 *
 * compare is for length of 1st string
 */
int tcap_trmncmp (const char *a, const char *b)
{
	while (TRUE)
	{
		int c;

		if (*a == '-' || *a == '_' || isspace(*a))
		{
			a++;
			continue;
		}

		if (*b == '-' || *b == '_' || isspace(*b))
		{
			b++;
			continue;
		}

		if (*a == 0)
			break;

		c = (tolower(*a) - tolower(*b));
		if (c)
			return (c);

		a++;
		b++;
	}

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_trans_term_str() - convert a string, changing all 0x00 to 0x80
 */
int tcap_trans_term_str (const char *str_in, unsigned char *str_out, int count)
{
	int i;
	int l;

	l = tcap_trans_str(str_in, str_out, count);
	for (i=0; i<l; i++)
	{
		if (str_out[i] == 0)
			str_out[i] = 0x80;
	}

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_trans_str() - convert a string
 */
int tcap_trans_str (const char *str_in, unsigned char *str_out, int count)
{
	unsigned char *	ctmp;
	const char *	cp;
	int				c;
	int				num;

	if (*str_in == '"')		/* remove opening quote, if any */
		str_in++;

	count--;
	for (cp=str_in, ctmp=str_out; *cp; cp++)
	{
		c = *cp;
		if (c == '"')
			break;

		switch (c)
		{
		case '^':						/* control char */
			if (*(cp+1) == 0 || *(cp+1) == '"')
			{
				*ctmp++ = (unsigned char)c;
				break;
			}
			c = *++cp;

			if (c == '?')
				*ctmp++ = 0x7f;
			else if (c == '^')
				*ctmp++ = '^';
			else
				*ctmp++ = c & 0x1f;
			break;

		case '\\':						/* escape char */
			if (*(cp+1) == 0 || *(cp+1) == '"')
			{
				*ctmp++ = (unsigned char)c;
				break;
			}
			c = *++cp;

			switch (c)
			{
			case '\\':					/* backslash */
					*ctmp++ = (unsigned char)c;
					break;

			case 'e':
			case 'E':
					*ctmp++ = 0x1b;		/* escape */
					break;

			case 'n':
					*ctmp++ = '\n';		/* newline */
					break;

			case 'r':
					*ctmp++ = '\r';		/* return */
					break;

			case 't':
					*ctmp++ = '\t';		/* tab */
					break;

			case 'b':
					*ctmp++ = '\b';		/* backspace */
					break;

			case 'f':
					*ctmp++ = '\f';		/* formfeed */
					break;

			case 's':
					*ctmp++ = ' ';		/* space */
					break;

			case 'x':
			case 'X':					/* hex char */
					if (isxdigit(*(cp+1)) && isxdigit(*(cp+2)))
					{
						num = (get_hex(*(cp+1))<<4) + get_hex(*(cp+2));
						cp += 2;
						*ctmp++ = (unsigned char)num;
					}
					else
						*ctmp++ = (unsigned char)c;
					break;

			default:
					if (! IS_OCT(c))
					{
						*ctmp++ = (unsigned char)c;
					}
					else				/* octal char */
					{
						num = c - '0';
						while (IS_OCT(*(cp+1)))
						{
							c = *++cp;
							num = (num * 8) + (c-'0');
						}
						*ctmp++ = (unsigned char)num;
					}
					break;
			}
			break;

		default:
			*ctmp++ = (unsigned char)c;
			break;
		}

		if (ctmp == str_out+count)
			break;
	}
	*ctmp = 0;

	return (ctmp - str_out);
}

/*------------------------------------------------------------------------
 * tcap_trm_dup_str() - duplicate a string on the heap
 */
char *tcap_trm_dup_str (const char *str)
{
	char *	s;
	int		l;

	if (str == 0)
		return (0);

	l = strlen(str);
	s = (char *)MALLOC(l+1);
	if (s != 0)
		strcpy(s, str);

	return (s);
}
