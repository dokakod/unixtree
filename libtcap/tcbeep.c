/*------------------------------------------------------------------------
 * sound routines
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * tcap_beep() - ring bell if possible or flash if possible
 */
int tcap_beep (TERMINAL *tp)
{
	if (tp == 0)
		return (-1);

	if (is_cmd_pres(tp->tcap->strs.bl))
		tcap_outcmd(tp, S_BL, tp->tcap->strs.bl);
	else if (is_cmd_pres(tp->tcap->strs.vb))
		tcap_outcmd(tp, S_VB, tp->tcap->strs.vb);
	else
		tcap_outc(tp, KEY_BL);

	tcap_outflush(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_flash() - flash if possible or ring bell if possible
 */
int tcap_flash (TERMINAL *tp)
{
	if (tp == 0)
		return (-1);

	if (is_cmd_pres(tp->tcap->strs.vb))
		tcap_outcmd(tp, S_VB, tp->tcap->strs.vb);
	else if (is_cmd_pres(tp->tcap->strs.bl))
		tcap_outcmd(tp, S_BL, tp->tcap->strs.bl);

	tcap_outflush(tp);

	return (0);
}
