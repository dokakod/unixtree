/*------------------------------------------------------------------------
 * GMT / DST routines
 */
#include "oscommon.h"

/*------------------------------------------------------------------------
 * convert a tm struct to a time_t
 */
static time_t os_cvt_tm (struct tm *tm)
{
	time_t	t = 0;

	t += (tm->tm_year - 1900) * (365 * 24 * 60 * 60);
	t += (tm->tm_yday       ) * (      24 * 60 * 60);
	t += (tm->tm_hour       ) * (           60 * 60);
	t += (tm->tm_min        ) * (                60);
	t += (tm->tm_sec        );

	return (t);
}

/*------------------------------------------------------------------------
 * get GMT offset
 *
 * The problem here is that BSD & SYSV & Windows all do this
 * differently:
 *
 *	BSD		has a "tm_gmtoff" entry in the tm struct
 *	SYSV	has the external variable "timezone"
 *
 *	So, we just calculate the local-time & the gmt-time & subtract
 *	the two & thus avoid all O/S differences.
 *
 * Note that the GMT offset is a constant for all time values
 * (within a given timezone).
 */
time_t os_get_gmt (void)
{
	time_t		now	= time(0);
	time_t		lcl;
	time_t		gmt;
	struct tm *	tm;

	tm  = localtime(&now);
	lcl = os_cvt_tm(tm);

	tm  = gmtime(&now);
	gmt = os_cvt_tm(tm);

	return (lcl - gmt);
}

/*------------------------------------------------------------------------
 * get DST offset
 *
 * Note that the DST offset varies with the time given.
 *
 * Note also, that due to O/S differences, we just assume that,
 * if an offset is present, it is for 1 hour.
 */
time_t os_get_dst (time_t t)
{
	struct tm *	tmptr;

	tmptr = localtime(&t);
	if (tmptr->tm_isdst)
		return (3600);

	return (0);
}
