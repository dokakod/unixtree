/*------------------------------------------------------------------------
 * procedures for processing the alt-file-display & alt-dir-display cmds
 */
#include "libprd.h"

/*------------------------------------------------------------------------
 * tables of number of columns in file window for each type
 */
static const int max_file_col_no[]			= { 0,  2,  1,  0,  0 };
static const int split_max_file_col_no[]	= { 0,  1,  0,  0,  0 };

/*------------------------------------------------------------------------
 * tables of length of display line needed after filename part
 */
static const int file_line_width[]			= { 3,  3, 14, 42, 42 };
static const int split_file_line_width[]	= { 3,  3, 14, 21, 21 };

static const int dir_line_width[]			= { 0, 35, 32, 38 };
static const int split_dir_line_width[]		= { 0, 22, 22, 22 };

/*------------------------------------------------------------------------
 * process alt-file-display cmd
 */
void do_file_disp (void)
{
	gbl(scr_cur)->file_fmt++;
	gbl(scr_cur)->file_fmt %= 5;
	set_file_disp();
	werase(gbl(scr_cur)->cur_file_win);
	disp_file_list();
	do_file_scroll_bar();
}

/*------------------------------------------------------------------------
 * setup file display for given display mode
 */
void set_file_disp (void)
{
	int i;

	i = gbl(scr_cur)->file_fmt;
	if (gbl(scr_is_split))
	{
		if (gbl(scr_in_autoview))
		{
			gbl(scr_cur)->max_file_col = 0;
			gbl(scr_cur)->file_line_width =
				getmaxx(gbl(scr_cur)->cur_file_win);
			gbl(scr_cur)->file_disp_len = gbl(scr_cur)->file_line_width - 3;
		}
		else
		{
			gbl(scr_cur)->max_file_col = split_max_file_col_no[i];
			gbl(scr_cur)->file_line_width =
				( (getmaxx(gbl(scr_cur)->cur_file_win)-1) /
				(gbl(scr_cur)->max_file_col+1) );
			gbl(scr_cur)->file_disp_len = gbl(scr_cur)->file_line_width -
				split_file_line_width[i];
		}
	}
	else
	{
		if (gbl(scr_in_autoview))
		{
			gbl(scr_cur)->max_file_col = 0;
			gbl(scr_cur)->file_line_width =
				getmaxx(gbl(scr_cur)->cur_file_win);
			gbl(scr_cur)->file_disp_len = gbl(scr_cur)->file_line_width - 3;
		}
		else
		{
			gbl(scr_cur)->max_file_col = max_file_col_no[i];
			gbl(scr_cur)->file_line_width =
				( (getmaxx(gbl(scr_cur)->cur_file_win)-1) /
				(gbl(scr_cur)->max_file_col+1) );
			gbl(scr_cur)->file_disp_len = gbl(scr_cur)->file_line_width -
				file_line_width[i];
		}
	}

	while (gbl(scr_cur)->cur_file_col > gbl(scr_cur)->max_file_col)
	{
		gbl(scr_cur)->cur_file_col--;
		for (i=0; i<=gbl(scr_cur)->max_file_line; i++)
			gbl(scr_cur)->top_file = bnext(gbl(scr_cur)->top_file);
	}
}

/*------------------------------------------------------------------------
 * process alt-dir-display cmd
 */
void do_dir_disp (void)
{
	gbl(scr_cur)->dir_fmt++;
	gbl(scr_cur)->dir_fmt %= 4;
	set_dir_disp();
	if (gbl(scr_cur)->command_mode == m_dir ||
		(gbl(scr_cur)->command_mode == m_file &&
		gbl(scr_cur)->in_small_window))
	{
		disp_dir_tree();
	}
}

/*------------------------------------------------------------------------
 * setup dir display for given display mode
 */
void set_dir_disp (void)
{
	int i;

	i = gbl(scr_cur)->dir_fmt;
	if (gbl(scr_is_split))
	{
		gbl(scr_cur)->dir_disp_len =
			getmaxx(gbl(scr_cur)->cur_dir_win)-2-split_dir_line_width[i];
	}
	else
	{
		gbl(scr_cur)->dir_disp_len =
			getmaxx(gbl(scr_cur)->cur_dir_win)-2-dir_line_width[i];
	}
}
