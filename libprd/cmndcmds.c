/*------------------------------------------------------------------------
 * regular "dir" navigation cmds
 */
#include "libprd.h"

static int do_dir_cmdf (int c)
{
	int i;

	switch (c)
	{
	case RESET:
	final:
		do_dir_scroll_bar();
		hilite_dir(ON);
		(gbl(scr_cur)->cur_dir_tree) = (TREE *)bid(gbl(scr_cur)->dir_cursor);
		gbl(scr_cur)->cur_dir = (DBLK *)tid(gbl(scr_cur)->cur_dir_tree);
		if (!gbl(scr_cur)->in_dest_dir)
		{
			(gbl(scr_cur)->cur_root)->cur_dir_tree =
				(gbl(scr_cur)->cur_dir_tree);
			dirtree_to_dirname((gbl(scr_cur)->cur_dir_tree),
				gbl(scr_cur)->path_name);
			disp_path_line();
			disp_cur_dir();
			disp_drive_info((gbl(scr_cur)->cur_dir)->stbuf.st_dev, 0);
			check_small_window();
		}
		return (0);

	case '2':
	case ' ':
	case KEY_DOWN:
	down_one:
		if (gbl(scr_cur)->cur_dir_no == gbl(scr_cur)->numdirs-1)
			return (0);
		hilite_dir(OFF);
		gbl(scr_cur)->cur_dir_no++;
		gbl(scr_cur)->dir_cursor = bnext(gbl(scr_cur)->dir_cursor);
		gbl(scr_cur)->cur_dir_line++;
		if (gbl(scr_cur)->cur_dir_line > gbl(scr_cur)->max_dir_line)
		{
			gbl(scr_cur)->cur_dir_line = gbl(scr_cur)->max_dir_line;
			gbl(scr_cur)->top_dir = bnext(gbl(scr_cur)->top_dir);
			wmove(gbl(scr_cur)->cur_dir_win, 0, 0);
			wdeleteln(gbl(scr_cur)->cur_dir_win);
			wmove(gbl(scr_cur)->cur_dir_win, gbl(scr_cur)->max_dir_line, 0);
			disp_dir_line(gbl(scr_cur)->dir_cursor);
		}
		goto final;

	case '8':
	case KEY_UP:
		if (gbl(scr_cur)->cur_dir_no == 0)
			return (0);
		hilite_dir(OFF);
		gbl(scr_cur)->cur_dir_no--;
		gbl(scr_cur)->dir_cursor = bprev(gbl(scr_cur)->dir_cursor);
		gbl(scr_cur)->cur_dir_line--;
		if (gbl(scr_cur)->cur_dir_line < 0)
		{
			gbl(scr_cur)->cur_dir_line = 0;
			gbl(scr_cur)->top_dir = gbl(scr_cur)->dir_cursor;
			wmove(gbl(scr_cur)->cur_dir_win, 0, 0);
			winsertln(gbl(scr_cur)->cur_dir_win);
			disp_dir_line(gbl(scr_cur)->dir_cursor);
		}
		goto final;

	case '4':
	case KEY_LEFT:
		i = tdepth((TREE *)bid(gbl(scr_cur)->dir_cursor))-1;
		if (i <= 0)
			return (0);
		hilite_dir(OFF);
	up_to_level:
		while (TRUE)
		{
			gbl(scr_cur)->dir_cursor = bprev(gbl(scr_cur)->dir_cursor);
			gbl(scr_cur)->cur_dir_line--;
			gbl(scr_cur)->cur_dir_no--;
			if (gbl(scr_cur)->cur_dir_line < 0)
			{
				gbl(scr_cur)->cur_dir_line = 0;
				gbl(scr_cur)->top_dir = gbl(scr_cur)->dir_cursor;
			}
			if (i == tdepth((TREE *)bid(gbl(scr_cur)->dir_cursor)))
				break;
		}
		disp_dir_tree();
		goto final;

	case '6':
	case KEY_RIGHT:
		if (tright(gbl(scr_cur)->cur_dir_tree) != 0)
			goto down_one;
		return (0);

	case KEY_TAB:
		if (tright(gbl(scr_cur)->cur_dir_tree) == 0)
			return (0);
		hilite_dir(OFF);
		i = tdepth((gbl(scr_cur)->cur_dir_tree));
		while (1)
		{
			gbl(scr_cur)->cur_dir_no++;
			gbl(scr_cur)->dir_cursor = bnext(gbl(scr_cur)->dir_cursor);
			gbl(scr_cur)->cur_dir_line++;
			if (gbl(scr_cur)->cur_dir_line > gbl(scr_cur)->max_dir_line)
			{
				gbl(scr_cur)->cur_dir_line = gbl(scr_cur)->max_dir_line;
				gbl(scr_cur)->top_dir = bnext(gbl(scr_cur)->top_dir);
			}
			if (i == tdepth((TREE *)bid(gbl(scr_cur)->dir_cursor)))
				break;
		}
		disp_dir_tree();
		goto final;

	case KEY_BS:
		if (tparent(gbl(scr_cur)->cur_dir_tree) == 0)
			return (0);
		if (tleft(tparent(gbl(scr_cur)->cur_dir_tree)) ==
			gbl(scr_cur)->cur_dir_tree)
		{
			return (0);
		}
		hilite_dir(OFF);
		i = tdepth((gbl(scr_cur)->cur_dir_tree));
		goto up_to_level;

	case '9':
	case KEY_PGUP:
		if (!gbl(scr_cur)->cur_dir_no)
			return (0);
		hilite_dir(OFF);
		if (gbl(scr_cur)->cur_dir_line)
		{
			while (gbl(scr_cur)->cur_dir_line)
			{
				gbl(scr_cur)->cur_dir_line--;
				gbl(scr_cur)->cur_dir_no--;
				gbl(scr_cur)->dir_cursor = bprev(gbl(scr_cur)->dir_cursor);
			}
			goto final;
		}
		for (i=0; i<=gbl(scr_cur)->max_dir_line; i++)
		{
			if (!gbl(scr_cur)->cur_dir_no)
				break;
			gbl(scr_cur)->dir_cursor = bprev(gbl(scr_cur)->dir_cursor);
			gbl(scr_cur)->cur_dir_no--;
		}
		gbl(scr_cur)->top_dir = gbl(scr_cur)->dir_cursor;
		disp_dir_tree();
		goto final;

	case '3':
	case KEY_PGDN:
		if (gbl(scr_cur)->cur_dir_no == gbl(scr_cur)->numdirs-1)
			return (0);
		hilite_dir(OFF);
		if (gbl(scr_cur)->cur_dir_line < gbl(scr_cur)->max_dir_line)
		{
			while (gbl(scr_cur)->cur_dir_line<gbl(scr_cur)->max_dir_line &&
				bnext(gbl(scr_cur)->dir_cursor))
			{
				gbl(scr_cur)->cur_dir_line++;
				gbl(scr_cur)->cur_dir_no++;
				gbl(scr_cur)->dir_cursor = bnext(gbl(scr_cur)->dir_cursor);
			}
			goto final;
		}
		for (i=0; i<=gbl(scr_cur)->max_dir_line; i++)
		{
			if (! bnext(gbl(scr_cur)->dir_cursor))
				break;
			gbl(scr_cur)->top_dir = bnext(gbl(scr_cur)->top_dir);
			gbl(scr_cur)->dir_cursor = bnext(gbl(scr_cur)->dir_cursor);
			gbl(scr_cur)->cur_dir_no++;
		}
		disp_dir_tree();
		gbl(scr_cur)->cur_dir_line = gbl(scr_cur)->max_dir_line;
		goto final;

	case '7':
	case KEY_HOME:
		if (!gbl(scr_cur)->cur_dir_no)
			return (0);
		hilite_dir(OFF);
		if (gbl(scr_cur)->cur_dir_no != gbl(scr_cur)->cur_dir_line)
		{
			gbl(scr_cur)->top_dir = gbl(scr_cur)->first_dir;
			disp_dir_tree();
		}
		gbl(scr_cur)->cur_dir_line = 0;
		gbl(scr_cur)->cur_dir_no = 0;
		gbl(scr_cur)->dir_cursor = gbl(scr_cur)->first_dir;
		goto final;

	case '1':
	case KEY_END:
		if (gbl(scr_cur)->cur_dir_no == gbl(scr_cur)->numdirs-1)
			return (0);
		hilite_dir(OFF);
		while (bnext(gbl(scr_cur)->dir_cursor))
		{
			gbl(scr_cur)->cur_dir_line++;
			gbl(scr_cur)->dir_cursor = bnext(gbl(scr_cur)->dir_cursor);
		}
		gbl(scr_cur)->cur_dir_no = gbl(scr_cur)->numdirs-1;
		if (gbl(scr_cur)->cur_dir_line <= gbl(scr_cur)->max_dir_line)
			goto final;
		gbl(scr_cur)->cur_dir_line = gbl(scr_cur)->max_dir_line;
		for (gbl(scr_cur)->top_dir=gbl(scr_cur)->dir_cursor, i=0;
			i<gbl(scr_cur)->max_dir_line; i++)
		{
			gbl(scr_cur)->top_dir = bprev(gbl(scr_cur)->top_dir);
		}
		disp_dir_tree();
		goto final;

	default:
		return (c);
	}
}

static int do_dir_cmds (int c)
{
	int i;

	switch (c)
	{
	case RESET:
	final:
		hilite_dir(ON);
		(gbl(scr_cur)->cur_dir_tree) = (TREE *)bid(gbl(scr_cur)->dir_cursor);
		(gbl(scr_cur)->cur_dir) = (DBLK *)tid(gbl(scr_cur)->cur_dir_tree);
		if (!gbl(scr_cur)->in_dest_dir)
		{
			(gbl(scr_cur)->cur_root)->cur_dir_tree =
				(gbl(scr_cur)->cur_dir_tree);
			dirtree_to_dirname((gbl(scr_cur)->cur_dir_tree),
				gbl(scr_cur)->path_name);
			disp_path_line();
			disp_cur_dir();
			disp_drive_info((gbl(scr_cur)->cur_dir)->stbuf.st_dev, 0);
			check_small_window();
		}
		return (0);

	case '2':
	case ' ':
	case KEY_DOWN:
	down_one:
		if (gbl(scr_cur)->cur_dir_no == gbl(scr_cur)->numdirs-1)
			return (0);
		hilite_dir(OFF);
		gbl(scr_cur)->cur_dir_no++;
		gbl(scr_cur)->dir_cursor = bnext(gbl(scr_cur)->dir_cursor);
		disp_dir_tree();
		goto final;

	case '8':
	case KEY_UP:
		if (gbl(scr_cur)->cur_dir_no == 0)
			return (0);
		hilite_dir(OFF);
		gbl(scr_cur)->cur_dir_no--;
		gbl(scr_cur)->dir_cursor = bprev(gbl(scr_cur)->dir_cursor);
		disp_dir_tree();
		goto final;

	case '4':
	case KEY_LEFT:
		i = tdepth((TREE *)bid(gbl(scr_cur)->dir_cursor))-1;
		if (i <= 0)
			return (0);
		hilite_dir(OFF);
	up_to_level:
		while (1)
		{
			gbl(scr_cur)->dir_cursor = bprev(gbl(scr_cur)->dir_cursor);
			gbl(scr_cur)->cur_dir_no--;
			if (i == tdepth((TREE *)bid(gbl(scr_cur)->dir_cursor)))
				break;
		}
		disp_dir_tree();
		goto final;

	case '6':
	case KEY_RIGHT:
		if (tright(gbl(scr_cur)->cur_dir_tree) != 0)
			goto down_one;
		return (0);

	case KEY_TAB:
		if (tright(gbl(scr_cur)->cur_dir_tree) == 0)
			return (0);
		hilite_dir(OFF);
		i = tdepth((gbl(scr_cur)->cur_dir_tree));
		while (1)
		{
			gbl(scr_cur)->cur_dir_no++;
			gbl(scr_cur)->dir_cursor = bnext(gbl(scr_cur)->dir_cursor);
			if (i == tdepth((TREE *)bid(gbl(scr_cur)->dir_cursor)))
				break;
		}
		disp_dir_tree();
		goto final;

	case KEY_BS:
		if (tparent(gbl(scr_cur)->cur_dir_tree) == 0)
			return (0);
		if (tleft(tparent(gbl(scr_cur)->cur_dir_tree)) ==
			gbl(scr_cur)->cur_dir_tree)
		{
			return (0);
		}
		hilite_dir(OFF);
		i = tdepth((gbl(scr_cur)->cur_dir_tree));
		goto up_to_level;

	case '9':
	case KEY_PGUP:
		if (!gbl(scr_cur)->cur_dir_no)
			return (0);
		hilite_dir(OFF);
		for (i=0; i<=gbl(scr_cur)->max_dir_line; i++)
		{
			gbl(scr_cur)->dir_cursor = bprev(gbl(scr_cur)->dir_cursor);
			gbl(scr_cur)->cur_dir_no--;
			if (! bprev(gbl(scr_cur)->dir_cursor))
				break;
		}
		disp_dir_tree();
		goto final;

	case '3':
	case KEY_PGDN:
		if (gbl(scr_cur)->cur_dir_no == gbl(scr_cur)->numdirs-1)
			return (0);
		hilite_dir(OFF);
		for (i=0; i<=gbl(scr_cur)->max_dir_line; i++)
		{
			gbl(scr_cur)->dir_cursor = bnext(gbl(scr_cur)->dir_cursor);
			gbl(scr_cur)->cur_dir_no++;
			if (! bnext(gbl(scr_cur)->dir_cursor))
				break;
		}
		disp_dir_tree();
		goto final;

	case '7':
	case KEY_HOME:
		if (!gbl(scr_cur)->cur_dir_no)
			return (0);
		hilite_dir(OFF);
		gbl(scr_cur)->cur_dir_no = 0;
		gbl(scr_cur)->dir_cursor = gbl(scr_cur)->first_dir;
		disp_dir_tree();
		goto final;

	case '1':
	case KEY_END:
		if (gbl(scr_cur)->cur_dir_no == gbl(scr_cur)->numdirs-1)
			return (0);
		hilite_dir(OFF);
		while (bnext(gbl(scr_cur)->dir_cursor))
			gbl(scr_cur)->dir_cursor = bnext(gbl(scr_cur)->dir_cursor);
		gbl(scr_cur)->cur_dir_no = gbl(scr_cur)->numdirs-1;
		disp_dir_tree();
		goto final;

	default:
		return (c);
	}
}

int do_dir_cmd (int c)
{
	if (gbl(scr_cur)->dir_scroll)
		return (do_dir_cmds(c));
	else
		return (do_dir_cmdf(c));
}
