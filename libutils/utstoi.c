/*------------------------------------------------------------------------
 *	string to integer routines
 */
#include "utcommon.h"

long stol (const char **in_str)
{
	const char *	str;
	long			num  = 0;
	int				is_neg = 0;

	str = *in_str;
	while (isspace(*str))			/* skip leading white space */
		str++;
	if (*str == '-')				/* check for minus sign */
	{
		is_neg = 1;
		str++;
	}
	while (isdigit(*str))
	{
		num *= 10;
		num += *str++ - '0';
	}
	*in_str = str;
	return (is_neg? -num: num);
}

int  stoi (const char **in_str)
{
	return ((int) stol(in_str));
}

int stoo (const char **in_str)
{
	int				num  = 0;
	int				dig;
	const char *	str;

	str = *in_str;
	while (isspace(*str))			/* skip leading white space */
		str++;

	while (isdigit(*str))
	{
		dig = *str++ - '0';
		if (dig >= 8)
		{
			num = -1;
			break;
		}
		num = (num * 8) + dig;
	}
	*in_str = str;
	return (num);
}

int stox (const char **in_str)
{
	int				n = stoi(in_str);
	const char *	s = *in_str;

	switch (tolower(*s))
	{
	case 'k':
		n *= 1024;
		s++;
		*in_str = s;
		break;

	case 'm':
		n *= (1024 * 1024);
		s++;
		*in_str = s;
		break;

	case 'g':
		n *= (1024 * 1024 * 1024);
		s++;
		*in_str = s;
		break;
	}

	return (n);
}

int atox (const char *str)
{
	return stox(&str);
}
