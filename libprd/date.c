/*------------------------------------------------------------------------
 *	Date processing routines
 *
 *	Entry points:
 *		date_to_str(time)
 *			converts time to "mm/dd/yy hh:mm pm" or "mm/dd/yy hh:mm:ss"
 *		date_2_str(time)
 *			converts time to "mm/dd/yy hh:mm:ss pm"
 *		tm_to_str(yr, mo, dy, hh, mm, ss)
 *			converts ints to "mm/dd/yy hh:mm:ss pm"
 *		cdate (string, month, day, year, hour, mn, sec)
 *			converts char string to ints
 *
 */
#include "libprd.h"

static const char *date_formats[] =
{
	"mdy",
	"dmy",
	"ymd"
};

#define JAN		31
#define FEB		28
#define MAR		31
#define APR		30
#define MAY		31
#define JUN		30
#define JUL		31
#define AUG		31
#define SEP		30
#define OCT		31
#define NOV		30
#define DEC		31

static const int days_of_month[] =
{
	JAN, FEB, MAR, APR, MAY, JUN, JUL, AUG, SEP, OCT, NOV, DEC
};

static int same_str (const char *a, const char *b)
{
	for (; *a; a++, b++)
		if (tolower(*a) != tolower(*b))
			return (0);
	return (1);
}

char *date_to_str (char *dl, time_t time)
{
	if (time == (time_t)(-1))
	{
		return (msgs(m_date_nodate));
	}

	date_2_str(dl, time);
	if (opt(time_fmt) == time_24)
		dl[17] = 0;
	else
		strcpy(dl+14, dl+17);
	return (dl);
}

char *date_2_str (char *dl, time_t time)
{
	struct tm *tmptr;
	int mo, dy, yr;
	int hh, mm, ss;

	tmptr = localtime(&time);
	mo = tmptr->tm_mon+1;
	dy = tmptr->tm_mday;
	yr = tmptr->tm_year;
	hh = tmptr->tm_hour;
	mm = tmptr->tm_min;
	ss = tmptr->tm_sec;

	return (tm_to_str(dl, yr, mo, dy, hh, mm, ss));
}

char *tm_to_str (char *dl, int yr, int mo, int dy, int hh, int mm, int ss)
{
	char *p;
	int i;
	int h;

	/* process date */

	p = dl;
	for (i=0; i<3; i++)
	{
		switch (date_formats[opt(date_fmt)][i])
		{
		case 'm':
			p[0] = mo/10 + '0';
			p[1] = mo%10 + '0';
			break;

		case 'd':
			p[0] = dy/10 + '0';
			p[1] = dy%10 + '0';
			break;

		case 'y':
			yr %= 100;
			p[0] = yr/10 + '0';
			p[1] = yr%10 + '0';
			break;
		}
		p[2] = opt(date_sep_char);
		p += 3;
	}
	dl[8] = ' ';

	/* process time */

	h = hh;
	if (opt(time_fmt) == time_12)		/* fix if 12 hour fmt */
	{
		if (h >= 12)
			h -= 12;
		if (h == 0)
			h = 12;
	}
	dl[ 9] = h/10 + '0';
	dl[10] = h%10 + '0';
	dl[11] = opt(time_sep_char);

	dl[12] = mm/10 + '0';
	dl[13] = mm%10 + '0';
	dl[14] = opt(time_sep_char);

	dl[15] = ss/10 + '0';
	dl[16] = ss%10 + '0';
	dl[17] = ' ';

	p = (hh < 12 ? msgs(m_date_am) : msgs(m_date_pm));
	dl[18] = p[0];
	dl[19] = p[1];
	dl[20] = 0;

	return (dl);
}

/*
 *  cdate.c
 *
 *  Routine to convert a date in char string format to integers
 *
 *  Usage: cdate(string, &year, &month, &day, &hour, &min, &sec)
 *
 *  Where: string = char string containing date
 *          month = returned month number (1-12)
 *            day = returned day number (1-31)
 *           year = returned year number (year-1900)
 *           hour = returned hour (0-23)
 *            min = returned minute (0-59)
 *            sec = returned second (0-59)
 *          cdate = 0 if no error found
 *
 *  The format of the string can be any of the following:
 *      Sept. 7, 1987
 *      September 7, 1987
 *      Sept. 7
 *      Sep 7 87
 *      9-7
 *      9/7
 *      9/7/87
 *      9 7 1987
 *      9-7-1987
 *      wednesday
 *      [next | last] monday
 *      yesterday | today | tomorrow
 *      etc.
 *
 *  and so forth.  The month must precede the day, and the year
 *  is optional.  If the year is not specified, the current year is
 *  used.
 *
 *  If the day of the week is specified, it may also be prefixed
 *  last or next.
 *
 *  Allowable abbreviations are as follows:
 *
 *      sun  sunday         jan january
 *      mon  monday         feb february
 *      tue  tuesday        mar march
 *      wed  wednesday      apr april
 *      thu  thursday       may may
 *      fri  friday         jun june
 *      sat  saturday       jul july
 *                          aug august
 *      yes yesterday       sep september
 *      tod today           oct october
 *      tom tomorrow        nov november
 *      now now             dec december
 *
 *      la  last
 *      ne  next
 *
 *  also, a null or blank string defaults to today.
 *
 *  Note that the returned time is 12:00 am unless now is specified.
 */

int cdate (char *string, int *year, int *month, int *day,
	int *hour, int *mn, int *sec)
{
	time_t thetime;
	struct tm *tmptr;
	int mm, dd, yy, wd, rd, ls;
	int usetime;
	int i;
	int j;

	usetime = 0;
	thetime = time((time_t *)0);
	tmptr   = localtime(&thetime);

	/*	Skip over any leading white space */

	while (isspace(*string))
		string++;

	/*	if null - return now */

	if (!*string)
	{
		mm = tmptr->tm_mon+1;
		dd = tmptr->tm_mday;
		yy = tmptr->tm_year;
		usetime = 1;
		goto got_date;
	}

	/*	Check for special keywords */

	if (!isdigit(*string))
	{
		/*	Look for yesterday, today, tomorrow, or now */

		rd = -1;
		     if (same_str(msgs(m_date_yes), string)) rd = 0;
		else if (same_str(msgs(m_date_tod), string)) rd = 1;
		else if (same_str(msgs(m_date_tom), string)) rd = 2;
		else if (same_str(msgs(m_date_now), string)) rd = 1, usetime = 1;
		if (rd != -1)
		{
			yy = tmptr->tm_year;
			mm = tmptr->tm_mon+1;
			dd = tmptr->tm_mday+(rd-1);
			switch (rd)
			{
			case 0: if (dd == 0)
					{
						mm--;
						if (mm == 0)
						{
							mm = 12;
							yy--;
						}
						dd = days_of_month[mm-1];
					}
					break;
			case 1: break;
			case 2: if (dd > days_of_month[mm-1])
					{
						dd = 1;
						mm++;
						if (mm > 12)
						{
							mm = 1;
							yy++;
						}
					}
					break;
			}
			goto got_date;
		}

		/*	Look for next, last, and Sunday to Saturday */

		wd = -1;
		ls =  0;
		if (same_str(msgs(m_date_last), string))
		{
			ls = -1;
			while (!isspace(*string))
				string++;
			while ( isspace(*string))
				string++;
		}
		else if (same_str(msgs(m_date_next), string))
		{
			ls = 1;
			while (!isspace(*string))
				string++;
			while ( isspace(*string))
				string++;
		}
		for (i=0; i<7; i++)
			if (same_str(msgs(m_date_days_00+i), string))
			{
				wd = i;
				break;
			}
		if (wd != -1)
		{
			if (wd < tmptr->tm_wday)
				wd += 7;
			dd = tmptr->tm_mday+(wd-tmptr->tm_wday);
			mm = tmptr->tm_mon+1;
			yy = tmptr->tm_year;

			if (ls == -1)
				dd -= 7;
			else if (ls == 1)
				if (wd == tmptr->tm_wday)
					dd += 7;

			if (dd > days_of_month[mm-1])
			{
				dd -= days_of_month[mm-1];
				mm++;
				if (mm > 12)
				{
					mm = 1;
					yy++;
				}
			}
			else if (dd < 1)
			{
				mm--;
				if (mm < 1)
				{
					mm = 12;
					yy--;
				}
				dd = days_of_month[mm-1]-dd;
			}
			goto got_date;
		}
	}

	/*	Look for valid date */

	mm = dd = yy = -1;
	for (j=0; j<3; j++)
	{
		if (!*string)
			break;
		switch (date_formats[opt(date_fmt)][j])
		{
		case 'd':					/*	get day */
				dd = atoi(string);
				if (dd < 1)
					goto error;
				break;

		case 'm':					/*	Get month */
				if (!isdigit(*string))
				{
					for (i=0; i<12; i++)
						if (same_str(msgs(m_date_months_00+i), string))
						{
							mm = i+1;
							break;
						}
					if (mm == -1)
						goto error;
					while (*string && !isspace(*string))
						string++;
				}
				else
				{
					mm = atoi(string);
					if (mm < 1 || mm > 12)
						goto error;
				}
				break;

		case 'y':						/* get year */
				yy = atoi(string);
				if (yy == 0)
					yy = tmptr->tm_year;
				yy %= 100;
				break;
		}

		/* skip over letters & numbers (to delimiter) */

		while (*string && (isalpha(*string) || isdigit(*string)))
			string++;

		/* skip over all consecutive delimiters (non-letters & numbers) */

		while (*string && !(isalpha(*string) || isdigit(*string)))
			string++;
	}

	/* check if all there */

	if (dd == -1 || mm == -1)
		goto error;
	if (yy == -1)
		yy = tmptr->tm_year % 100;

	/* check for 2/29 in leap year */

	if (dd > days_of_month[mm-1])
	{
		if (mm == 2 && dd == 29 && (yy % 4 == 0))
			;
		else
			goto error;
	}

got_date:
	*month = mm;
	*day   = dd;
	*year  = yy;
	if (usetime)
	{
		*hour = tmptr->tm_hour;
		*mn   = tmptr->tm_min;
		*sec  = tmptr->tm_sec;
	}
	else
	{
		*hour = 0;
		*mn   = 0;
		*sec  = 0;
	}
	return (0);

error:
	return (1);
}
