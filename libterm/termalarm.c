/*------------------------------------------------------------------------
 * Alarm routines
 */
#include "termcommon.h"

/*------------------------------------------------------------------------
 * alarm structure
 */
struct alarm
{
	int			interval;			/* interval for signalling		*/
	time_t		clock;				/* next signal time				*/
	ALARM_RTN *	funcptr;			/* callback routine to call		*/
	void *		data;				/* user data to pass to CB rtn	*/
	int			on_heap;			/* TRUE if data is on the heap	*/
	int			returns;			/* TRUE if CB rtn returns		*/
	int			running;			/* TRUE if alarm is running		*/
};
typedef struct alarm ALARM;

static ALARM term_alarms[NUM_ALARMS] = { 0 };

/*------------------------------------------------------------------------
 * need this forward reference for Windows
 */
static void term_alarm_reset (int sig, void *data);

#if V_WINDOWS
/*------------------------------------------------------------------------
 * sub-thread routine for Windows
 */
static HANDLE	term_event		= INVALID_HANDLE_VALUE;
static int		term_interval	= 0;

void __cdecl term_alarm_thread (void *foo)
{
	BOOL	bLoop = TRUE;

	while (bLoop && term_interval != 0)
	{
		DWORD wait_event;

		wait_event = WaitForSingleObject(term_event, term_interval * 1000);
		switch (wait_event)
		{
		case WAIT_TIMEOUT:
			term_alarm_reset(1, 0);
			break;

		default:
			bLoop = FALSE;
			break;
		}
	}

	CloseHandle(term_event);
	term_event = INVALID_HANDLE_VALUE;
}

static int alarm (int interval)
{
	int	old_interval = term_interval;

	/*--------------------------------------------------------------------
	 * if interval is 0, we turn off the alarm mechanism
	 */
	term_interval = interval;

	if (interval == 0)
	{
		SetEvent(term_event);
	}
	else
	{
		if (term_event == INVALID_HANDLE_VALUE)
			term_event = CreateEvent(0, TRUE, FALSE, 0);

		if (term_event != INVALID_HANDLE_VALUE)
		{
			_beginthread(term_alarm_thread, 0, 0);
		}
	}

	return (old_interval);
}
#endif

/*------------------------------------------------------------------------
 * term_alarm_reset() - internal routine to signal any ready entries &
 * restart the alarm mechanism
 */
static void term_alarm_reset (int sig, void *data)
{
	time_t		clock			= time(0);
	int			new_interval	= 0;
	ALARM_RTN *	defer			= 0;
	void *		udata			= 0;
	int			i;

	/*--------------------------------------------------------------------
	 * set signal handler if pseudo-call
	 */
#ifdef SIGALRM
	if (sig == 0)
		sys_sig_set(SIGALRM, term_alarm_reset, 0);
#endif

	/*--------------------------------------------------------------------
	 *	Loop through alarm array & process each entry
	 *	that is active and expired.
	 *	Note that more than one alarm can expire at the
	 *	same time, but only one at a time can have (returns == FALSE).
	 */
	for (i=0; i<NUM_ALARMS; i++)
	{
		ALARM *a = term_alarms + i;

		/*----------------------------------------------------------------
		 * skip empty entries
		 */
		if (! a->running)
			continue;

		/*----------------------------------------------------------------
		 * sig == 0 means alarm_on() or alarm_off() called us.
		 * sig == SIGALARM is a real alarm call
		 */
		if (sig && a->clock <= clock)
		{
			if (a->returns)
			{
				/*--------------------------------------------------------
				 * reset entry for next time
				 */
				a->clock = clock + a->interval;

				/*--------------------------------------------------------
				 * now call it
				 */
				(*a->funcptr)(clock, a->data);
			}
			else
			{
				/*--------------------------------------------------------
				 * cache entry for later
				 */
				defer = a->funcptr;
				udata = a->data;

				/*--------------------------------------------------------
				 * clear entry (a no-return entry is signalled only once)
				 */
				a->interval	= 0;
				a->clock	= 0;
				a->funcptr	= 0;
				a->data		= 0;
				a->on_heap	= FALSE;
				a->returns	= FALSE;
				a->running	= FALSE;
			}
		}

		/*----------------------------------------------------------------
		 *	Find shortest interval to reset alarm for
		 */
		{
			int interval = a->clock - clock;

			/*------------------------------------------------------------
			 * allow for clock ticking while we fiddle about
			 */
			if (interval <= 0)
				interval = 1;

			/*------------------------------------------------------------
			 * check if this is shorter than prev
			 */
			if (new_interval == 0 || interval < new_interval)
				new_interval = interval;
		}
	}

	/*--------------------------------------------------------------------
	 *	restart timer if anything to wait for
	 */
	if (new_interval > 0)
	{
		alarm(new_interval);
	}

	/*--------------------------------------------------------------------
	 *	Call deferred alarm function if applicable
	 */
	if (defer != 0)
	{
		(*defer)(clock, udata);
		/*NOTREACHED*/
	}
}

/*------------------------------------------------------------------------
 * term_alarm_set() - add an entry to the alarm table
 */
int term_alarm_set (int atype, int interval, ALARM_RTN *funcptr, void *data,
	int on_heap, int returns, int start)
{
	ALARM *	a;

	/*--------------------------------------------------------------------
	 * check for invalid alarm number
	 */
	if (atype < 0 || atype >= NUM_ALARMS)
		return (-1);

	a = term_alarms + atype;

#if V_WINDOWS
	/*--------------------------------------------------------------------
	 * a no-return entry is not allowed in WINDOWS
	 */
	if (! returns)
		return (-1);
#endif

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (data == 0)
		on_heap = FALSE;

	if (on_heap && ! returns)
		return (-1);

	if (interval <= 0 || funcptr == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * convert ms to seconds
	 */
	interval += 999;
	interval /= 1000;

	/*--------------------------------------------------------------------
	 * stop alarm from running
	 */
	alarm(0);

	/*--------------------------------------------------------------------
	 * check if entry is already in use
	 */
	if (a->interval)
	{
		/*----------------------------------------------------------------
		 * free the data if on the heap
		 */
		if (a->on_heap)
			FREE(a->data);
	}

	/*--------------------------------------------------------------------
	 * store data in entry
	 */
	a->interval	= interval;
	a->clock	= time(0) + interval;
	a->funcptr	= funcptr;
	a->data		= data;
	a->on_heap	= on_heap;
	a->returns	= returns;
	a->running	= start;

	/*--------------------------------------------------------------------
	 * reset alarm
	 */
	term_alarm_reset(0, 0);

	return (0);
}

/*------------------------------------------------------------------------
 * term_alarm_clr() - remove an entry from the alarm table
 */
int term_alarm_clr (int atype)
{
	ALARM *	a;

	/*--------------------------------------------------------------------
	 * check for invalid alarm number
	 */
	if (atype < 0 || atype >= NUM_ALARMS)
		return (-1);

	a = term_alarms + atype;

	/*--------------------------------------------------------------------
	 * check if not active
	 */
	if (a->interval == 0)
		return (0);

	/*--------------------------------------------------------------------
	 * turn off alarms
	 */
	alarm(0);

	/*--------------------------------------------------------------------
	 * free the data if on the heap
	 */
	if (a->on_heap)
		FREE(a->data);

	/*--------------------------------------------------------------------
	 * clear the entry
	 */
	a->interval	= 0;
	a->clock	= 0;
	a->funcptr	= 0;
	a->data		= 0;
	a->on_heap	= FALSE;
	a->returns	= FALSE;
	a->running	= FALSE;

	/*--------------------------------------------------------------------
	 * now restart the alarms
	 */
	term_alarm_reset(0, 0);

	return (0);
}

/*------------------------------------------------------------------------
 * term_alarm_on() - turn on an alarm
 */
int term_alarm_on (int atype)
{
	ALARM *	a;

	/*--------------------------------------------------------------------
	 * check for invalid alarm number
	 */
	if (atype < 0 || atype >= NUM_ALARMS)
		return (-1);

	a = term_alarms + atype;

	/*--------------------------------------------------------------------
	 * check if already on
	 */
	if (a->running)
		return (0);

	/*--------------------------------------------------------------------
	 * turn off alarms
	 */
	alarm(0);

	/*--------------------------------------------------------------------
	 * set entry on
	 */
	a->running	= TRUE;

	/*--------------------------------------------------------------------
	 * now restart the alarms
	 */
	term_alarm_reset(0, 0);

	return (0);
}

/*------------------------------------------------------------------------
 * term_alarm_off() - turn off an alarm
 */
int term_alarm_off (int atype)
{
	ALARM *	a;

	/*--------------------------------------------------------------------
	 * check for invalid alarm number
	 */
	if (atype < 0 || atype >= NUM_ALARMS)
		return (-1);

	a = term_alarms + atype;

	/*--------------------------------------------------------------------
	 * turn off alarms
	 */
	alarm(0);

	/*--------------------------------------------------------------------
	 * set entry off
	 */
	a->running	= FALSE;

	/*--------------------------------------------------------------------
	 * now restart the alarms
	 */
	term_alarm_reset(0, 0);

	return (0);
}

/*------------------------------------------------------------------------
 * term_alarm_check() - check if an alarm entry is active
 */
int term_alarm_check (int atype)
{
	if (atype >= 0 && atype < NUM_ALARMS)
	{
		ALARM *	a	= term_alarms + atype;

		return (a->interval != 0);
	}

	return (FALSE);
}

/*------------------------------------------------------------------------
 * term_alarm_trip() - signal an alarm type if active
 */
int term_alarm_trip (int atype, time_t t)
{
	if (atype >= 0 && atype < NUM_ALARMS)
	{
		ALARM *	a	= term_alarms + atype;

		if (a->funcptr != 0)
			(*a->funcptr)(t, a->data);

		return (0);
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * term_alarm_active() - check if an alarm type is active
 */
int term_alarm_active (int atype)
{
	if (atype >= 0 && atype < NUM_ALARMS)
	{
		ALARM *	a	= term_alarms + atype;

		return (a->running);
	}

	return (FALSE);
}

/*------------------------------------------------------------------------
 * term_alarm_stop() - suspend alarm activity
 */
int term_alarm_stop (void)
{
	return (alarm(0));
}

/*------------------------------------------------------------------------
 * term_alarm_start() - restart alarm activity
 */
int term_alarm_start (int value)
{
	return (alarm(value));
}
