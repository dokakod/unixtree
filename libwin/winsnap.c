/*------------------------------------------------------------------------
 * "snap" a picture of the current screen
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * win_key_snap() - store a snapshot of the current screen
 *
 * This routine is usually called when a KEY_SNAP key is read in
 *
 * This routine ALWAYS returns 0
 */
int win_key_snap (int key, void *data)
{
	SCREEN *s	= (SCREEN *)data;

	win_snap(SCR_TRMSCR(s), 0);
	return (0);
}

/*------------------------------------------------------------------------
 * win_snap() - take a snapshot of a window
 */
int win_snap (WINDOW *win, const char *path)
{
	FILE *fp;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * open save file in append mode
	 */
	if (path == 0 || *path == 0)
		path = WIN_SNAP_FILE;

	fp = fopen(path, "ab");
	if (fp == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * save current screen image
	 */
	putwin(win, fp);

	/*--------------------------------------------------------------------
	 * close the file
	 */
	fclose(fp);

	return (OK);
}
