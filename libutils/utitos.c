/*------------------------------------------------------------------------
 * num-string transformation routines
 */
#include "utcommon.h"

/*------------------------------------------------------------------------
 * itosf() - num to "xxx,xxx,xxx"
 */
char *itosf (char *buf, int num, int sep_char)
{
	int j, i, d;

	/*--------------------------------------------------------------------
	 * fill buffer with blanks
	 */
	for (i=0; i<11; i++)
		buf[i] = ' ';
	buf[11] = 0;
	j = 10;

	/*--------------------------------------------------------------------
	 * num must be positive
	 */
	if (num <= 0)
	{
		buf[j] = '0';
		return (buf);
	}

	/*--------------------------------------------------------------------
	 * if num is too big, convert to "k" or "m"
	 */
	if (num > 999999999L)
	{
		buf[j--] = 'k';
		num /= 1024L;
		if (num > 99999999L)
		{
			buf[j+1] = 'm';
			num /= 1024L;
		}
	}

	/*--------------------------------------------------------------------
	 * now convert the number
	 */
	i = 0;
	while (num)
	{
		if (i && (i % 3) == 0)
			buf[j--] = sep_char;
		d = num % 10;
		buf[j--] = '0' + d;
		num = (num - d) / 10;
		i++;
	}

	return (buf);
}

/*------------------------------------------------------------------------
 * itosfs() - same as itosf(), but returns pointer to first non-blank char
 */
char *itosfs (char *buf, int num, int sep_char)
{
	char *p;

	p = itosf(buf, num, sep_char);
	while (*p == ' ')
		p++;

	return (p);
}

/*------------------------------------------------------------------------
 * itosl() - convert num to fixed width string with no sep chars
 */
char *itosl (char *buf, int num, int len)
{
	int j, i, d;

	/*--------------------------------------------------------------------
	 * fill buffer with spaces
	 */
	for (i=0; i<len; i++)
		buf[i] = ' ';
	buf[i] = 0;

	/*--------------------------------------------------------------------
	 * num must be positive
	 */
	if (num <= 0)
	{
		buf[len-1] = '0';
		return (buf);
	}

	/*--------------------------------------------------------------------
	 * now convert the number
	 */
	i = 0;
	j = len-1;
	while (num)
	{
		d = num % 10;
		buf[j--] = '0' + d;
		num = (num - d) / 10;
		i++;
	}

	return (buf);
}

/*------------------------------------------------------------------------
 * itos() - convert num to string
 */
char *itos (char *buf, int num)
{
	char *b;
	char *t;

	/*--------------------------------------------------------------------
	 * num must be positive
	 */
	if (num < 0)
	{
		*buf = 0;
		return (buf);
	}

	/*--------------------------------------------------------------------
	 * special case of 0
	 */
	if (num == 0)
	{
		strcpy(buf, "0");
		return (buf);
	}

	/*--------------------------------------------------------------------
	 * do the chars in reverse order
	 */
	b = buf;
	while (num)
	{
		*b++ = (num % 10) + '0';
		num /= 10;
	}
	*b = 0;

	/*--------------------------------------------------------------------
	 * now reverse the string
	 */
	t = buf;
	b--;
	while (t < b)
	{
		char a = *t;

		*t = *b;
		*b = a;

		t++;
		b--;
	}

	return (buf);
}
