/*------------------------------------------------------------------------
 * process the "what" cmd
 */
#include "libprd.h"

void do_what (void)
{
	char	path[MAX_PATHLEN];
	char	result[256];
	int		rc;

	/*--------------------------------------------------------------------
	 * load magic list if not done yet
	 */
	if (gbl(magic_list) == 0)
	{
		gbl(magic_list) = mag_parse(gbl(pgm_magic), FALSE);
	}

	/*--------------------------------------------------------------------
	 * get magic info for file
	 */
	strcpy(path, gbl(scr_cur)->path_name);
	fn_append_filename_to_dir(path, FULLNAME(gbl(scr_cur)->cur_file));

	rc = mag_check(gbl(magic_list), path, result);

	/*--------------------------------------------------------------------
	 * display info
	 */
	if (rc == 0)
	{
		werase(gbl(win_commands));
		wmove(gbl(win_commands), 0, 0);
		zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_file));
		wmove(gbl(win_commands), 1, 0);
		waddstr(gbl(win_commands), result);
		wrefresh(gbl(win_commands));

		bang("");
		anykey();
	}
	else
	{
		errmsg(ER_COF, result, ERR_ANY);
	}
}

void what_free (void)
{
	if (gbl(magic_list) != 0)
	{
		mag_free(gbl(magic_list));
		gbl(magic_list) = 0;
	}
}
