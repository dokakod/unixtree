/*------------------------------------------------------------------------
 *	header for alarm routines
 *
 *	These routines allow a user to have multiple alarms running,
 *	even though the O/S allows only one.
 */
#ifndef TERMALARM_H
#define TERMALARM_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * alarm callback routine type
 */
typedef void ALARM_RTN (time_t t, void *data);

/*------------------------------------------------------------------------
 * alarm types
 */
#define ALARM_KEY  		0			/* used by key input rtn (termkbd.c)	*/
#define ALARM_CLOCK		1			/* used by clock rtn (termclock.c)		*/
#define ALARM_PING		2			/* used to ping something (heartbeat)	*/
#define ALARM_USER		3			/* available for users					*/

#define NUM_ALARMS		4			/* number of alarms defined				*/

/*------------------------------------------------------------------------
 * function prototypes
 */
extern int		term_alarm_set		(int atype,
										int interval,
										ALARM_RTN *funcptr,
										void *data,
										int on_heap,
										int returns,
										int start);
extern int		term_alarm_clr		(int atype);

extern int		term_alarm_on		(int atype);
extern int		term_alarm_off		(int atype);

extern int		term_alarm_check	(int atype);
extern int		term_alarm_trip		(int atype, time_t t);
extern int		term_alarm_active	(int atype);

extern int		term_alarm_stop		(void);
extern int		term_alarm_start	(int value);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TERMALARM_H */
