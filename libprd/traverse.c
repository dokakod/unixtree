/*------------------------------------------------------------------------
 * "traverse" all tagged files for a "tag-*" cmd
 */
#include "libprd.h"

/*
 *	traverse - traverse the file list and act on all tagged files
 *
 *  action() acts on each file and returns:
 *		-1 abort
 *		 0 continue at current file if tagged
 *		 1 continue at next tagged file
 */

/* ARGSUSED */
void traverse (int (*action)(void *data), void *data)
{
	BLIST *b;
	BLIST *old_top;
	FBLK *f;
	int advance;

	hilite_file(OFF);

	/*
	 * if top left is not the first file, check if any files
	 * prev to top_file are tagged
	 */

	if (gbl(scr_cur)->top_file == gbl(scr_cur)->first_file)
	{
		gbl(scr_cur)->cur_file_no = 0;				/* start at home */
		gbl(scr_cur)->cur_file_line = 0;
		gbl(scr_cur)->cur_file_col = 0;
		gbl(scr_cur)->file_cursor = gbl(scr_cur)->first_file;
		old_top = gbl(scr_cur)->top_file;
		gbl(scr_cur)->top_file = gbl(scr_cur)->file_cursor;
	}
	else
	{
		/* scan for any tagged files between first_file & top_file */

		gbl(scr_cur)->cur_file_no = -1;
		for (b=gbl(scr_cur)->first_file; b!=gbl(scr_cur)->top_file; b=bnext(b))
		{
			gbl(scr_cur)->cur_file_no++;
			if (is_file_tagged((FBLK *)bid(b)))
				break;
		}

		if (b != gbl(scr_cur)->top_file)
		{
			gbl(scr_cur)->top_file = b;
			gbl(scr_cur)->file_cursor = b;
			gbl(scr_cur)->cur_file_line = 0;
			gbl(scr_cur)->cur_file_col = 0;
			werase(gbl(scr_cur)->cur_file_win);
			disp_file_list();
			old_top = gbl(scr_cur)->top_file;
		}
		else
		{
			old_top = gbl(scr_cur)->top_file;
			gbl(scr_cur)->cur_file_line = 0;
			gbl(scr_cur)->cur_file_col = 0;
			gbl(scr_cur)->file_cursor = gbl(scr_cur)->top_file;
			gbl(scr_cur)->cur_file_no++;
		}
	}
	gbl(scr_cur)->cur_file = (FBLK *)bid(gbl(scr_cur)->file_cursor);

	advance = 0;
	while (TRUE)
	{
		/*  start at current file or next */

		if (advance)
			b = bnext(gbl(scr_cur)->file_cursor);
		else
			b = gbl(scr_cur)->file_cursor;

		/* now scan to next tagged file */

		for (; b; b=bnext(b))
		{
			f = (FBLK *)bid(b);
			if (is_file_tagged(f))
				break;
		}

		/* if at end, done */

		if (!b)
			break;

		/* unhilite last file */

		hilite_file(OFF);

		/* move cursor down to the found file */

		for (; gbl(scr_cur)->file_cursor!=b;
			gbl(scr_cur)->file_cursor=bnext(gbl(scr_cur)->file_cursor))
		{
			gbl(scr_cur)->cur_file_no++;
			gbl(scr_cur)->cur_file_line++;
			if (gbl(scr_cur)->cur_file_line > gbl(scr_cur)->max_file_line)
			{
				gbl(scr_cur)->cur_file_col++;
				gbl(scr_cur)->cur_file_line = 0;
				if (gbl(scr_cur)->cur_file_col > gbl(scr_cur)->max_file_col)
				{
					/*
					 * if we spill off the screen, redraw
					 * with the found file at top left
					 */
					gbl(scr_cur)->cur_file_line = 0;
					gbl(scr_cur)->cur_file_col = 0;
					gbl(scr_cur)->file_cursor = b;
					gbl(scr_cur)->top_file = b;
					break;
				}
			}
		}

		if (old_top != gbl(scr_cur)->top_file)
		{
			werase(gbl(scr_cur)->cur_file_win);
			disp_file_list();
			old_top = gbl(scr_cur)->top_file;
		}

		gbl(scr_cur)->cur_file = (FBLK *)bid(gbl(scr_cur)->file_cursor);
		hilite_file(ON);
		disp_cur_file();
		fblk_to_dirname(gbl(scr_cur)->cur_file, gbl(scr_cur)->path_name);
		disp_path_line();

		advance = (*action)(data);
		if ((advance < 0) || (xchk_kbd() == KEY_ESCAPE))
			break;
	}

	if (gbl(scr_cur)->numfiles)
	{
		hilite_file(ON);
		disp_cur_file();
	}
}
