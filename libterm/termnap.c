/*------------------------------------------------------------------------
 * sleep for given milliseconds
 */
#include "termcommon.h"

/*------------------------------------------------------------------------
 * term_nap() - sleep for given milliseconds
 */
void term_nap (int ms)
{
#if V_UNIX
	struct timeval	tv;

	tv.tv_sec	= (ms / 1000);
	tv.tv_usec	= (ms % 1000) * 1000;
	select(0, 0, 0, 0, &tv);
#else
	Sleep(ms);
#endif
}
