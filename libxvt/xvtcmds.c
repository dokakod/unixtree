/*------------------------------------------------------------------------
 * table of emulator entries
 */
#include "xvtcommon.h"

const CMD_TBL *	xvt_cmds[] =
{
	&xvt_vt52_cmdtbl,
	&xvt_vt100_cmdtbl,
	&xvt_vt200_cmdtbl,
	&xvt_vt300_cmdtbl,
	&xvt_xterm_cmdtbl,

	0
};
