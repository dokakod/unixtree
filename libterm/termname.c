/*------------------------------------------------------------------------
 * get name of tty device
 */
#include "termcommon.h"

/*------------------------------------------------------------------------
 * term_get_ttyname() - get name of tty device
 */
const char * term_get_tty_name (const TERM_DATA *t)
{
	const char *n;

	if (t == 0)
	{
		n = 0;
	}
	else
	{
#if V_UNIX
		n = ttyname(t->tty_out);
#else
		n = "con";
#endif
	}

	if (n == 0)
	{
		n = "?";
	}

	return (n);
}
