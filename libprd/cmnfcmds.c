/*------------------------------------------------------------------------
 * process file navigation cmds
 */
#include "libprd.h"

int do_file_cmd (int c)
{
	int i;
	int k;

	switch (c)
	{
	case '2':
	case ' ':
	case KEY_DOWN:
		if (gbl(scr_cur)->cur_file_no == gbl(scr_cur)->numfiles-1)
			return (0);
		hilite_file(OFF);
		gbl(scr_cur)->cur_file_no++;
		gbl(scr_cur)->file_cursor = bnext(gbl(scr_cur)->file_cursor);
		gbl(scr_cur)->cur_file_line++;
		if (gbl(scr_cur)->cur_file_line > gbl(scr_cur)->max_file_line)
		{
			gbl(scr_cur)->cur_file_line = 0;
			gbl(scr_cur)->cur_file_col++;
			if (gbl(scr_cur)->cur_file_col <= gbl(scr_cur)->max_file_col)
				goto final;
			gbl(scr_cur)->cur_file_col = gbl(scr_cur)->max_file_col;
			gbl(scr_cur)->cur_file_line = gbl(scr_cur)->max_file_line;
			gbl(scr_cur)->top_file = bnext(gbl(scr_cur)->top_file);
			if (gbl(scr_cur)->max_file_col)
				disp_file_list();
			else
			{
				wmove(gbl(scr_cur)->cur_file_win, 0, 0);
				wdeleteln(gbl(scr_cur)->cur_file_win);
				wmove(gbl(scr_cur)->cur_file_win,
					gbl(scr_cur)->cur_file_line, 0);
				disp_file_line(gbl(scr_cur)->cur_file);
			}
		}
	final:
		gbl(scr_cur)->cur_file = (FBLK *)bid(gbl(scr_cur)->file_cursor);
		do_file_scroll_bar();
		hilite_file(ON);
		fblk_to_dirname(gbl(scr_cur)->cur_file, gbl(scr_cur)->path_name);
		disp_path_line();
		disp_cur_file();
		disp_drive_info((gbl(scr_cur)->cur_file)->stbuf.st_dev, 0);
		return (0);

	case '8':
	case KEY_UP:
		if (gbl(scr_cur)->cur_file_no == 0)
			return (0);
		hilite_file(OFF);
		gbl(scr_cur)->cur_file_no--;
		gbl(scr_cur)->file_cursor = bprev(gbl(scr_cur)->file_cursor);
		gbl(scr_cur)->cur_file_line--;
		if (gbl(scr_cur)->cur_file_line < 0)
		{
			gbl(scr_cur)->cur_file_line = gbl(scr_cur)->max_file_line;
			gbl(scr_cur)->cur_file_col--;
			if (gbl(scr_cur)->cur_file_col >= 0)
				goto final;
			gbl(scr_cur)->cur_file_line = 0;
			gbl(scr_cur)->cur_file_col = 0;
			gbl(scr_cur)->top_file = bprev(gbl(scr_cur)->top_file);
			if (gbl(scr_cur)->max_file_col)
				disp_file_list();
			else
			{
				wmove(gbl(scr_cur)->cur_file_win, 0, 0);
				winsertln(gbl(scr_cur)->cur_file_win);
				disp_file_line(gbl(scr_cur)->cur_file);
			}
		}
		goto final;

	case '6':
	case KEY_RIGHT:
		if (gbl(scr_cur)->cur_file_no +
			(gbl(scr_cur)->max_file_line-gbl(scr_cur)->cur_file_line) >=
			gbl(scr_cur)->numfiles-1)
		{
			return (0);
		}

		hilite_file(OFF);
		if (gbl(scr_cur)->cur_file_no+(gbl(scr_cur)->max_file_line+1) <=
			gbl(scr_cur)->numfiles-1)
		{
			gbl(scr_cur)->cur_file_no += gbl(scr_cur)->max_file_line+1;
			for (i=0; i<gbl(scr_cur)->max_file_line+1; i++)
				gbl(scr_cur)->file_cursor = bnext(gbl(scr_cur)->file_cursor);
		}
		else
		{
			for (; bnext(gbl(scr_cur)->file_cursor);
				gbl(scr_cur)->file_cursor=bnext(gbl(scr_cur)->file_cursor))
			{
				gbl(scr_cur)->cur_file_line++;
			}
			gbl(scr_cur)->cur_file_line -= gbl(scr_cur)->max_file_line+1;
			gbl(scr_cur)->cur_file_no = gbl(scr_cur)->numfiles-1;
		}
		if (++(gbl(scr_cur)->cur_file_col) > gbl(scr_cur)->max_file_col)
		{
			gbl(scr_cur)->cur_file_col = gbl(scr_cur)->max_file_col;
			for (i=0; i<gbl(scr_cur)->max_file_line+1; i++)
				gbl(scr_cur)->top_file = bnext(gbl(scr_cur)->top_file);
			werase(gbl(scr_cur)->cur_file_win);
			disp_file_list();
		}
		goto final;

	case '4':
	case KEY_LEFT:
		if (gbl(scr_cur)->cur_file_no == gbl(scr_cur)->cur_file_line)
			return (0);
		hilite_file(OFF);
		if (gbl(scr_cur)->cur_file_col)
		{
			gbl(scr_cur)->cur_file_col--;
			gbl(scr_cur)->cur_file_no -= gbl(scr_cur)->max_file_line+1;
			for (i=0; i<gbl(scr_cur)->max_file_line+1; i++)
				gbl(scr_cur)->file_cursor = bprev(gbl(scr_cur)->file_cursor);
			goto final;
		}
		if (gbl(scr_cur)->cur_file_no-gbl(scr_cur)->cur_file_line >=
			gbl(scr_cur)->max_file_line+1)
		{
			for (i=0; i<gbl(scr_cur)->max_file_line+1; i++)
			{
				gbl(scr_cur)->top_file = bprev(gbl(scr_cur)->top_file);
				gbl(scr_cur)->file_cursor = bprev(gbl(scr_cur)->file_cursor);
			}
			gbl(scr_cur)->cur_file_no -= gbl(scr_cur)->max_file_line+1;
		}
		else
		{
			gbl(scr_cur)->file_cursor = gbl(scr_cur)->first_file;
			gbl(scr_cur)->top_file = gbl(scr_cur)->file_cursor;
			gbl(scr_cur)->cur_file_no = 0;
			gbl(scr_cur)->cur_file_line = 0;
		}
		werase(gbl(scr_cur)->cur_file_win);
		disp_file_list();
		goto final;

	case '9':
	case KEY_PGUP:
		if (!gbl(scr_cur)->cur_file_no)
			return (0);
		hilite_file(OFF);
		if (!gbl(scr_cur)->cur_file_line && !gbl(scr_cur)->cur_file_col)
		{
			for (i=0;
				i < (gbl(scr_cur)->max_file_line+1) *
					(gbl(scr_cur)->max_file_col+1);
				i++)
			{
				gbl(scr_cur)->cur_file_no--;
				gbl(scr_cur)->top_file = bprev(gbl(scr_cur)->top_file);
				if (! bprev(gbl(scr_cur)->top_file))
					break;
			}
			werase(gbl(scr_cur)->cur_file_win);
			disp_file_list();
		}
		else
		{
			gbl(scr_cur)->cur_file_no -= gbl(scr_cur)->cur_file_line +
				gbl(scr_cur)->cur_file_col * (gbl(scr_cur)->max_file_line+1);
		}
		gbl(scr_cur)->cur_file_col = 0;
		gbl(scr_cur)->cur_file_line = 0;
		gbl(scr_cur)->file_cursor = gbl(scr_cur)->top_file;
		goto final;

	case '3':
	case KEY_PGDN:
		if (gbl(scr_cur)->cur_file_no == gbl(scr_cur)->numfiles-1)
			return (0);
		hilite_file(OFF);
		if (gbl(scr_cur)->cur_file_no - gbl(scr_cur)->cur_file_line -
			(gbl(scr_cur)->cur_file_col*(gbl(scr_cur)->max_file_line+1)) +
			(gbl(scr_cur)->max_file_col+1)*(gbl(scr_cur)->max_file_line+1) >=
				gbl(scr_cur)->numfiles)
		{
			for (; bnext(gbl(scr_cur)->file_cursor);
				gbl(scr_cur)->file_cursor=bnext(gbl(scr_cur)->file_cursor))
			{
				if (++gbl(scr_cur)->cur_file_line >
					gbl(scr_cur)->max_file_line)
				{
					gbl(scr_cur)->cur_file_col++;
					gbl(scr_cur)->cur_file_line = 0;
				}
			}
			gbl(scr_cur)->cur_file_no = gbl(scr_cur)->numfiles-1;
		}
		else
		{
			if (gbl(scr_cur)->cur_file_line<gbl(scr_cur)->max_file_line ||
				gbl(scr_cur)->cur_file_col<gbl(scr_cur)->max_file_col)
			{
				k = (gbl(scr_cur)->max_file_line+1) *
						(gbl(scr_cur)->max_file_col+1) -
					(gbl(scr_cur)->cur_file_line+gbl(scr_cur)->cur_file_col *
						(gbl(scr_cur)->max_file_line+1));

				for (i=1; i<k; i++)
				{
					gbl(scr_cur)->file_cursor =
						bnext(gbl(scr_cur)->file_cursor);
					gbl(scr_cur)->cur_file_no++;
				}
				gbl(scr_cur)->cur_file_line = gbl(scr_cur)->max_file_line;
				gbl(scr_cur)->cur_file_col = gbl(scr_cur)->max_file_col;
			}
			else
			{
				gbl(scr_cur)->top_file = gbl(scr_cur)->file_cursor;
				gbl(scr_cur)->cur_file_line = 0;
				gbl(scr_cur)->cur_file_col = 0;
				for (i=1;
					i < (gbl(scr_cur)->max_file_line+1) *
						(gbl(scr_cur)->max_file_col+1);
					i++)
				{
					if (! bnext(gbl(scr_cur)->file_cursor))
						break;
					gbl(scr_cur)->file_cursor =
						bnext(gbl(scr_cur)->file_cursor);
					gbl(scr_cur)->cur_file_no++;
					if (++(gbl(scr_cur)->cur_file_line) >
						gbl(scr_cur)->max_file_line)
					{
						gbl(scr_cur)->cur_file_line = 0;
						gbl(scr_cur)->cur_file_col++;
					}
				}
				werase(gbl(scr_cur)->cur_file_win);
				disp_file_list();
			}
		}
		goto final;

	case '7':
	case KEY_HOME:
		if (!gbl(scr_cur)->cur_file_no)
			return (0);
		hilite_file(OFF);
		if (gbl(scr_cur)->top_file != gbl(scr_cur)->first_file)
		{
			gbl(scr_cur)->top_file = gbl(scr_cur)->first_file;
			werase(gbl(scr_cur)->cur_file_win);	/* just in case */
			disp_file_list();
		}
		gbl(scr_cur)->file_cursor = gbl(scr_cur)->first_file;
		gbl(scr_cur)->cur_file_no = 0;
		gbl(scr_cur)->cur_file_line = 0;
		gbl(scr_cur)->cur_file_col = 0;
		goto final;

	case '1':
	case KEY_END:
		if (gbl(scr_cur)->cur_file_no == gbl(scr_cur)->numfiles-1)
			return (0);
		hilite_file(OFF);
		if (gbl(scr_cur)->cur_file_no - gbl(scr_cur)->cur_file_line -
			(gbl(scr_cur)->cur_file_col*(gbl(scr_cur)->max_file_line+1)) +
			(gbl(scr_cur)->max_file_col+1)*(gbl(scr_cur)->max_file_line+1) >=
				gbl(scr_cur)->numfiles)
		{
			for (; bnext(gbl(scr_cur)->file_cursor);
				gbl(scr_cur)->file_cursor=bnext(gbl(scr_cur)->file_cursor))
			{
				if (++(gbl(scr_cur)->cur_file_line) >
					gbl(scr_cur)->max_file_line)
				{
					gbl(scr_cur)->cur_file_col++;
					gbl(scr_cur)->cur_file_line = 0;
				}
			}
		}
		else
		{
			for (; bnext(gbl(scr_cur)->file_cursor);
				gbl(scr_cur)->file_cursor=bnext(gbl(scr_cur)->file_cursor))
				;
			gbl(scr_cur)->top_file = gbl(scr_cur)->file_cursor;
			for (i=1;
				i < (gbl(scr_cur)->max_file_line+1) *
					(gbl(scr_cur)->max_file_col+1);
				i++)
			{
				gbl(scr_cur)->top_file = bprev(gbl(scr_cur)->top_file);
			}
			gbl(scr_cur)->cur_file_col = gbl(scr_cur)->max_file_col;
			gbl(scr_cur)->cur_file_line = gbl(scr_cur)->max_file_line;
			disp_file_list();
		}
		gbl(scr_cur)->cur_file_no = gbl(scr_cur)->numfiles-1;
		goto final;

	default:
		return (c);
	}
}
