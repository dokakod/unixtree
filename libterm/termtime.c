/*------------------------------------------------------------------------
 * timeval routines
 */
#include "termcommon.h"

#if V_WINDOWS
/*------------------------------------------------------------------------
 * chart of cumulative # days per month (for non-leap & leap years)
 *
 *               jan feb mar apr may  jun  jul  aug  sep  oct  nov  dec
 */
static const
int ydays_n[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

static const
int ydays_l[] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };
#endif

/*------------------------------------------------------------------------
 * term_timeval() - get current time in microseconds
 */
void term_timeval (TIME_VAL *pt)
{
#if V_UNIX
	{
		struct timeval tv;

		/*----------------------------------------------------------------
		 * get local time
		 */
		gettimeofday(&tv, 0);

		/*----------------------------------------------------------------
		 * convert to a TIME_VAL struct
		 */
		pt->tv_sec	= tv.tv_sec;
		pt->tv_usec	= tv.tv_usec;
	}
#else
	{
		SYSTEMTIME	st;
    	int			bLeap;
    	const int *	ydays;

		/*----------------------------------------------------------------
		 * get local time
		 */
		GetLocalTime(&st);

		/*----------------------------------------------------------------
		 * get whether leap year or not
		 */
		bLeap = ( (st.wYear % 4) == 0 );

		/*----------------------------------------------------------------
		 * get proper cumulative-days table
		 */
		ydays = (bLeap ? ydays_l : ydays_n);

		/*----------------------------------------------------------------
		 * do the conversion
		 */
		pt->tv_sec  = ((st.wYear - 1970)    ) * (365 * 24 * 60 * 60);
		pt->tv_sec += ((st.wYear - 1969) / 4) * (      24 * 60 * 60);
		pt->tv_sec += (ydays[st.wMonth - 1] ) * (      24 * 60 * 60);
		pt->tv_sec += (st.wDay - 1          ) * (      24 * 60 * 60);
		pt->tv_sec += (st.wHour             ) * (           60 * 60);
		pt->tv_sec += (st.wMinute           ) * (                60);
		pt->tv_sec += (st.wSecond           );

		pt->tv_usec = (st.wMilliseconds * 1000);
	}
#endif
}
