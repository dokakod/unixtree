/*------------------------------------------------------------------------
 *	Date conversion routines
 */
#include "utcommon.h"

/*------------------------------------------------------------------------
 * month/day table
 */
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

static const int month_day[] =
{
	0,
	JAN,
	JAN + FEB,
	JAN + FEB + MAR,
	JAN + FEB + MAR + APR,
	JAN + FEB + MAR + APR + MAY,
	JAN + FEB + MAR + APR + MAY + JUN,
	JAN + FEB + MAR + APR + MAY + JUN + JUL,
	JAN + FEB + MAR + APR + MAY + JUN + JUL + AUG,
	JAN + FEB + MAR + APR + MAY + JUN + JUL + AUG + SEP,
	JAN + FEB + MAR + APR + MAY + JUN + JUL + AUG + SEP + OCT,
	JAN + FEB + MAR + APR + MAY + JUN + JUL + AUG + SEP + OCT + NOV
};

/*------------------------------------------------------------------------
 *  datecvt() - convert time values to a time_t
 *
 *  Usage:  date = datecvt (yr,mo,dy,hh,mm,ss)
 *          time_t date;    returned date in internal format
 *          int    mo;      month number (1-12)
 *          int    dy;      day number (1-31)
 *          int    yr;      year number (-1900)
 *          int    hh;      hour (0-23)
 *          int    mm;      minute (0-59)
 *          int    ss;      second (0-59)
 */
time_t datecvt (int yr, int mo, int dy, int hh, int mm, int ss)
{
	int days;
	int secs;

	/*--------------------------------------------------------------------
	 * process year
	 */
	yr %= 100;						/* strip century */
	if (yr < 70)					/* after year 2000 */
		yr += 100;
	days  = (yr - 70) * 365;		/* convert years to days */
	days += (yr - 69) / 4;			/* add prev years' leap days */

	/*--------------------------------------------------------------------
	 * process month
	 */
	days += month_day[mo-1];		/* add prev months' days */
	if ((yr % 4) == 0 && mo > 2)	/* check if leap year & after feb */
		days++;

	/*--------------------------------------------------------------------
	 * process day
	 */
	days += dy - 1;					/* add number of days */

	/*--------------------------------------------------------------------
	 * convert to seconds
	 */
	secs = ((days * 24 + hh) * 60 + mm) * 60 + ss;

	/*--------------------------------------------------------------------
	 * convert to GMT
	 */
	secs += os_get_gmt();

	/*--------------------------------------------------------------------
	 * adjust for daylight savings time
	 */
	secs += os_get_dst(secs);

	return (secs);
}
