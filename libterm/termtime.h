/*------------------------------------------------------------------------
 * time_val struct
 */
#ifndef TERMTIME_H
#define TERMTIME_H

#include <sys/types.h>

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * time_val struct (same as struct timeval)
 *
 * We use this interface, since Unix only defines "struct timeval"
 * (in sys/time.h) and Windows only defines "TIMEVAL" (in winsock.h).
 *
 * Note that, although the struct defines microseconds, the actual
 * data returned may not be that accurate.
 */
struct time_val
{
	time_t	tv_sec;			/* seconds		*/
	time_t	tv_usec;		/* microseconds	*/
};
typedef struct time_val TIME_VAL;

/*------------------------------------------------------------------------
 * function prototypes
 */
extern void		term_timeval		(TIME_VAL *pt);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TERMTIME_H */
