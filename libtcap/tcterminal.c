/*------------------------------------------------------------------------
 * screen routines
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * tcap_init_terminal() - initialize a TERMINAL
 */
TERMINAL *tcap_init_terminal (void)
{
	TERMINAL *tp;

	tp = (TERMINAL *)MALLOC(sizeof(*tp));
	if (tp != 0)
	{
		tp->crit	= sysmutex_alloc       ();
		tp->tcap	= tcap_init_tcap_data  ();
		tp->scrn	= tcap_init_screen_data();
		tp->term	= term_screen_dev_init ();

		if (tp->crit == 0 ||
		    tp->tcap == 0 ||
		    tp->scrn == 0 ||
		    tp->term == 0)
		{
			tcap_free_terminal(tp);
			tp = 0;
		}
	}

	return (tp);
}

/*------------------------------------------------------------------------
 * tcap_free_terminal() - free a TERMINAL
 */
void tcap_free_terminal (TERMINAL *tp)
{
	if (tp != 0)
	{
		if (tp->crit != 0)	sysmutex_free        (tp->crit);
		if (tp->scrn != 0)	tcap_free_screen_data(tp->scrn);
		if (tp->tcap != 0)	tcap_free_tcap_data  (tp->tcap);
		if (tp->term != 0)	term_screen_dev_free (tp->term);

		FREE(tp);
	}
}
