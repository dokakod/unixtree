/*------------------------------------------------------------------------
 * Clock routines
 */
#include "termcommon.h"

/*------------------------------------------------------------------------
 * term_clock_init() - initialize clock processing
 */
void term_clock_init (int interval, CLOCK_RTN *rtn, void *data)
{
	if (interval != 0)
	{
		term_alarm_set(ALARM_CLOCK, interval, rtn, data,
			FALSE, TRUE, FALSE);
	}
	else
	{
		term_alarm_clr(ALARM_CLOCK);
	}
}

/*------------------------------------------------------------------------
 * term_clock_clear() - clear clock entry
 */
void term_clock_clear (void)
{
	term_alarm_clr(ALARM_CLOCK);
}

/*------------------------------------------------------------------------
 * term_clock_set() - set clock running to on or off
 */
void term_clock_set (int bf)
{
	if (bf)
		term_clock_on();
	else
		term_clock_off();
}

/*------------------------------------------------------------------------
 * term_clock_on() - turn clock on
 */
void term_clock_on (void)
{
	if (term_alarm_check(ALARM_CLOCK) && ! term_alarm_active(ALARM_CLOCK))
	{
		term_alarm_trip(ALARM_CLOCK, time(0));
		term_alarm_on(ALARM_CLOCK);
	}
}

/*------------------------------------------------------------------------
 * term_clock_off() - turn clock off
 */
void term_clock_off (void)
{
	if (term_alarm_check(ALARM_CLOCK) && term_alarm_active(ALARM_CLOCK))
	{
		term_alarm_trip(ALARM_CLOCK, 0);
		term_alarm_off(ALARM_CLOCK);
	}
}

/*------------------------------------------------------------------------
 * term_clock_active() - just suspend clock running
 */
int term_clock_active (int bf)
{
	int active = term_alarm_active(ALARM_CLOCK);

	if (bf != active)
	{
		if (bf)
			term_alarm_on(ALARM_CLOCK);
		else
			term_alarm_off(ALARM_CLOCK);
	}

	return (active);
}

/*------------------------------------------------------------------------
 * term_clock_check() - check if clock is on
 */
int term_clock_check (void)
{
	return term_alarm_active(ALARM_CLOCK);
}

/*------------------------------------------------------------------------
 * term_clock_trip() - call the clock routine
 */
void term_clock_trip (void)
{
	term_alarm_trip(ALARM_CLOCK, time(0));
}
