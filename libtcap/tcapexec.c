/*------------------------------------------------------------------------
 * exec interface
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * tcap_exec_argv() - exec a cmd specified as an argv array
 */
int tcap_exec_argv (TERMINAL *tp, const char *dir, char **argv,
	int interactive, char *msgbuf)
{
	TERM_DATA *t = (tp == 0 ? 0 : tp->term);

	return term_exec_argv(t, dir, argv, interactive, msgbuf);
}

/*------------------------------------------------------------------------
 * tcap_exec_cmd() - exec a cmd specified as a cmd line
 */
int tcap_exec_cmd (TERMINAL *tp, const char *dir, const char *cmd,
	int interactive, char *msgbuf)
{
	TERM_DATA *t = (tp == 0 ? 0 : tp->term);

	return term_exec_cmd(t, dir, cmd, interactive, msgbuf);
}
