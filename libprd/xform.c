/*------------------------------------------------------------------------
 * num-string transformation routines
 */
#include "libprd.h"

char *xform (char *buf, long num)
{
	int j, i, d;

	for (i=0; i<11; i++)
		buf[i] = ' ';
	buf[11] = 0;
	j = 10;

	/* note num must be positive */

	if (num <= 0)
	{
		buf[j] = '0';
		return (buf);
	}

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

	i = 0;
	while (num)
	{
		if (i > 0 && (i % 3) == 0)
			buf[j--] = opt(num_sep_char);
		d = num%10;
		buf[j--] = '0'+d;
		num = (num-d)/10;
		i++;
	}
	return (buf);
}

char *xform2 (char *buf, int num, int len)
{
	int j, i, d;

	for (i=0; i<len; i++)
		buf[i] = ' ';
	buf[i] = 0;

	/* note num must be positive */

	if (num <= 0)
	{
		buf[len-1] = '0';
		return (buf);
	}

	i = 0;
	j = len-1;
	while (num)
	{
		d = num%10;
		buf[j--] = '0'+d;
		num = (num-d)/10;
		i++;
	}
	return (buf);
}

char *xform3 (char *buf, int num)
{
	char *p;

	p = xform(buf, num);
	while (*p == ' ')
		p++;
	return (p);
}
