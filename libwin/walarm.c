/*------------------------------------------------------------------------
 * Alarm routines
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * win_alarm_set() - add an entry to the alarm table
 */
int win_alarm_set (int atype, int interval, WIN_ALARM_RTN *funcptr, void *data,
	int on_heap, int returns, int start)
{
	return term_alarm_set(atype, interval, funcptr, data, on_heap, returns,
		start);
}

/*------------------------------------------------------------------------
 * win_alarm_clr() - remove an entry from the alarm table
 */
int win_alarm_clr (int atype)
{
	return term_alarm_clr(atype);
}

/*------------------------------------------------------------------------
 * win_alarm_on() - turn on an alarm
 */
int win_alarm_on (int atype)
{
	return term_alarm_on(atype);
}

/*------------------------------------------------------------------------
 * win_alarm_off() - turn off an alarm
 */
int win_alarm_off (int atype)
{
	return term_alarm_off(atype);
}

/*------------------------------------------------------------------------
 * win_alarm_check() - check if an alarm entry is active
 */
int win_alarm_check (int atype)
{
	return term_alarm_check(atype);
}

/*------------------------------------------------------------------------
 * win_alarm_trip() - signal an alarm type if active
 */
int win_alarm_trip (int atype, time_t t)
{
	return term_alarm_trip(atype, t);
}

/*------------------------------------------------------------------------
 * win_alarm_active() - check if an alarm type is active
 */
int win_alarm_active (int atype)
{
	return term_alarm_active(atype);
}

/*------------------------------------------------------------------------
 * win_alarm_stop() - suspend alarm activity
 */
int win_alarm_stop (void)
{
	return term_alarm_stop();
}

/*------------------------------------------------------------------------
 * win_alarm_start() - restart alarm activity
 */
int win_alarm_start (int value)
{
	return term_alarm_start(value);
}
