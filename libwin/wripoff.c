/*------------------------------------------------------------------------
 * ripoff routine
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * ripoffline() - ripoff a line from the top or bottom of the screen
 */
int ripoffline (int line, WIN_RIPOFF_RTN *init)
{
	return ripoffmline(line < 0 ? -1 : 1, init);
}

/*------------------------------------------------------------------------
 * ripoffmline() - ripoff n lines from the top or bottom of the screen
 */
int ripoffmline (int line, WIN_RIPOFF_RTN *init)
{
	RIPOFF_ENTRY *r;

	/*--------------------------------------------------------------------
	 * check if table is full
	 */
	if (win_ripoff_tbl.num_ents >= MAX_RIPOFFS)
		return (ERR);

	/*--------------------------------------------------------------------
	 * fill in next entry
	 */
	r = win_ripoff_tbl.entries + win_ripoff_tbl.num_ents++;

	if (line == 0)
		line = 1;

	r->line = line;
	r->init = init;

	/*--------------------------------------------------------------------
	 * bump top/bot counter
	 */
	if (line < 0)
		win_ripoff_tbl.num_bot -= line;
	else
		win_ripoff_tbl.num_top += line;

	return (OK);
}
