/*------------------------------------------------------------------------
 * process the "treespec" cmd
 */
#include "libprd.h"

void treespec (void)
{
#if 0
	char input_str[MAX_PATHLEN];
	int c;

	bang(msgs(m_treespec_prompt));

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_treespec_ts));

	strcpy(input_str, gbl(scr_cur)->path_name);
	c = xgetstr(gbl(win_commands), input_str, XGL_TREESPEC, MAX_PATHLEN,
		0, XG_FILEPATH);
	if (c < 0)
	{
		disp_cmds();
		return;
	}

	disp_cmds();
#endif
}
