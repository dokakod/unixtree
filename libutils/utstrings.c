/*------------------------------------------------------------------------
 * string utility routines
 */
#include "utcommon.h"

/*------------------------------------------------------------------------
 * istrcpy() - like strcpy() but returns # chars copied
 */
int istrcpy (char *buf, const char *str)
{
	int n = 0;

	for (; *str; str++)
	{
		*buf++ = *str;
		n++;
	}
	*buf = 0;

	return (n);
}

/*------------------------------------------------------------------------
 * str_lwr() - make a string all lower-case
 */
char *str_lwr (char *string)
{
	char *s;

	for (s=string; *s; s++)
		*s = tolower(*s);

	return (string);
}

/*------------------------------------------------------------------------
 * str_upr() - make a string all upper-case
 */
char *str_upr (char *string)
{
	char *s;

	for (s=string; *s; s++)
		*s = toupper(*s);

	return (string);
}

/*------------------------------------------------------------------------
 * strip() - strip trailing white space from a string
 */
char *strip (char *string)
{
	char *s;
	char *p = string;

	for (s=string; *s; s++)
	{
		if (! isspace(*s))
			p = s+1;
	}
	*p = 0;

	return (string);
}

/*------------------------------------------------------------------------
 * str_rem_ws() - remove leading & trailing white space from a string
 */
char * str_rem_ws (char *string)
{
	char *s;
	char *t;

	/*--------------------------------------------------------------------
	 * move to first non-white space char
	 */
	for (s=string; *s; s++)
	{
		if (! isspace(*s))
			break;
	}

	/*--------------------------------------------------------------------
	 * check for empty string
	 */
	if (*s == 0)
	{
		*string = 0;
		return (string);
	}

	/*--------------------------------------------------------------------
	 * strip trailing white-space
	 */
	strip(s);

	/*--------------------------------------------------------------------
	 * now copy chars over
	 */
	t = string;
	for (; *s; s++)
		*t++ = *s;
	*t = 0;

	return (string);
}

/*------------------------------------------------------------------------
 * str_expand_tabs() - expand tabs in a string
 */
int str_expand_tabs (const char *str, int tab_width, char *buf)
{
	const char *	s	= str;
	char *			b	= buf;
	int				i	= 0;

	for (; *s; s++)
	{
		int c = *(unsigned char *)s;

		if (iscntrl(c))
		{
			if (c == '\n' || c == '\r')
				continue;

			if (c != '\t')
			{
				*b++ = '.';
				i++;
			}
			else
			{
				*b++ = ' ';
				for (i++; (i % tab_width) != 0; i++)
					*b++ = ' ';
			}
		}
		else
		{
			*b++ = c;
			i++;
		}
	}
	*b = 0;

	return (b - buf);
}
