/*------------------------------------------------------------------------
 * process mouse-input cmds
 */
#include "libprd.h"

static int check_win_border (WINDOW *win)
{
	int my = mouse_y(win);
	int mx = mouse_x(win);

	if (mx >= getbegx(win) && mx < getbegx(win)+getmaxx(win))
	{
		if (my == getbegy(win)-1)
			return (1);
		if (my == getbegy(win)+getmaxy(win))
			return (2);
	}
	return (0);
}

int check_mouse_in_win (WINDOW *win, int n)
{
	int i;

	i = mouse_is_in_win(win);
	if (i)
		return (3);
	i = check_win_border(win);
	if (i)
		return (i);
	i = read_vert_scrollbar(win, n);
	if (i >= 0)
		return (4);
	return (0);
}

static int get_file_pos (void)
{
	int y;
	int x;
	int col;
	int pos;
	int width;
	WINDOW *win;

	win = gbl(scr_cur)->cur_file_win;
	width = gbl(scr_cur)->file_line_width;

	if (!mouse_is_in_win(win))
		return (-1);

	y = mouse_y(win) - getbegy(win);
	x = mouse_x(win) - getbegx(win);
	col = x / width;
	pos = col * getmaxy(win) + y;

	return (pos);
}

static void set_cur_dir (int n)
{
	int i;

	if (n == gbl(scr_cur)->cur_dir_line)
		return;
	hilite_dir(OFF);

	gbl(scr_cur)->dir_cursor = gbl(scr_cur)->top_dir;
	gbl(scr_cur)->cur_dir_no -= gbl(scr_cur)->cur_dir_line;
	gbl(scr_cur)->cur_dir_line = 0;
	for (i=0; i<n; i++)
	{
		if (bnext(gbl(scr_cur)->dir_cursor))
		{
			gbl(scr_cur)->dir_cursor = bnext(gbl(scr_cur)->dir_cursor);
			gbl(scr_cur)->cur_dir_line++;
			gbl(scr_cur)->cur_dir_no++;
		}
		else
		{
			break;
		}
	}

	do_dir_scroll_bar();
	hilite_dir(ON);
	gbl(scr_cur)->cur_dir_tree = (TREE *)bid(gbl(scr_cur)->dir_cursor);
	gbl(scr_cur)->cur_dir = (DBLK *)tid(gbl(scr_cur)->cur_dir_tree);

	if (! gbl(scr_cur)->in_dest_dir)
	{
		(gbl(scr_cur)->cur_root)->cur_dir_tree = (gbl(scr_cur)->cur_dir_tree);
		dirtree_to_dirname((gbl(scr_cur)->cur_dir_tree),
			gbl(scr_cur)->path_name);
		disp_path_line();
		disp_cur_dir();
		disp_drive_info((gbl(scr_cur)->cur_dir)->stbuf.st_dev, 0);
		check_small_window();
	}
}

static void find_file (int n)
{
	int i;
	BLIST *b;

	b = gbl(scr_cur)->top_file;
	for (i=0; i<n; i++)
	{
		if (!b)
			break;
		b = bnext(b);
	}
	if (!b)
		return;

	hilite_file(OFF);
	gbl(scr_cur)->file_cursor   = b;
	gbl(scr_cur)->cur_file      = (FBLK *)bid(gbl(scr_cur)->file_cursor);
	gbl(scr_cur)->cur_file_no   =
		bindex(gbl(scr_cur)->first_file, gbl(scr_cur)->file_cursor);
	gbl(scr_cur)->cur_file_line = n % (gbl(scr_cur)->max_file_line+1);
	gbl(scr_cur)->cur_file_col  = n / (gbl(scr_cur)->max_file_line+1);
	do_file_scroll_bar();
	hilite_file(ON);
	fblk_to_dirname(gbl(scr_cur)->cur_file, gbl(scr_cur)->path_name);
	disp_path_line();
	disp_cur_file();
	disp_drive_info((gbl(scr_cur)->cur_file)->stbuf.st_dev, 0);
}

static void set_dir_pos_from_sb (void)
{
	int i;
	int j;
	int top;

	j = read_vert_scrollbar(gbl(scr_cur)->cur_dir_win, gbl(scr_cur)->numdirs);
	if (j != gbl(scr_cur)->dir_scroll_bar)
	{
		hilite_dir(OFF);
		top = gbl(scr_cur)->cur_dir_no - gbl(scr_cur)->cur_dir_line;
		if (j >= top && j-top < gbl(scr_cur)->max_dir_line+1)
		{
			gbl(scr_cur)->cur_dir_no = j;
			gbl(scr_cur)->cur_dir_line = j - top;
			gbl(scr_cur)->dir_cursor = gbl(scr_cur)->first_dir;
			for (i=top; i<j; i++)
				gbl(scr_cur)->dir_cursor = bnext(gbl(scr_cur)->dir_cursor);
			gbl(scr_cur)->cur_dir_tree = (TREE *)bid(gbl(scr_cur)->dir_cursor);
			gbl(scr_cur)->cur_dir = (DBLK *)tid(gbl(scr_cur)->cur_dir_tree);
		}
		else
		{
			top = calculate_top_item(j, gbl(scr_cur)->numdirs,
				gbl(scr_cur)->max_dir_line+1);
			gbl(scr_cur)->top_dir = gbl(scr_cur)->first_dir;
			for (i=0; i<top; i++)
				gbl(scr_cur)->top_dir = bnext(gbl(scr_cur)->top_dir);
			gbl(scr_cur)->cur_dir_no = j;
			gbl(scr_cur)->cur_dir_line = j - top;
			gbl(scr_cur)->dir_cursor = gbl(scr_cur)->top_dir;
			for (i=top; i<j; i++)
				gbl(scr_cur)->dir_cursor = bnext(gbl(scr_cur)->dir_cursor);
			gbl(scr_cur)->cur_dir_tree = (TREE *)bid(gbl(scr_cur)->dir_cursor);
			gbl(scr_cur)->cur_dir = (DBLK *)tid(gbl(scr_cur)->cur_dir_tree);
			disp_dir_tree();
		}
		check_small_window();
		hilite_dir(ON);
		do_dir_scroll_bar();
		dptr_to_dirname(gbl(scr_cur)->dir_cursor, gbl(scr_cur)->path_name);
		disp_path_line();
		disp_file_box();
		disp_drive_info(0, 1);
		disp_node_stats(gbl(scr_cur)->cur_root);
		disp_cur_dir();
	}
}

static void set_file_pos_from_sb (void)
{
	int i;
	int j;
	int num;
	int top;

	j = read_vert_scrollbar(gbl(scr_cur)->cur_file_win,
		gbl(scr_cur)->numfiles);
	if (j != gbl(scr_cur)->file_scroll_bar && gbl(scr_cur)->top_file != 0)
	{
		hilite_file(OFF);
		top = gbl(scr_cur)->cur_file_no - gbl(scr_cur)->cur_file_line -
				((gbl(scr_cur)->max_file_line+1) * gbl(scr_cur)->cur_file_col);
		num = (gbl(scr_cur)->max_file_line+1) * (gbl(scr_cur)->max_file_col+1);
		if (j >= top && j-top < num)
		{
			gbl(scr_cur)->cur_file_no = j;
			gbl(scr_cur)->cur_file_line =
				(j - top) % (gbl(scr_cur)->max_file_line+1);
			gbl(scr_cur)->cur_file_col  =
				(j - top) / (gbl(scr_cur)->max_file_line+1);
			gbl(scr_cur)->file_cursor = gbl(scr_cur)->top_file;
			for (i=top; i<j; i++)
				gbl(scr_cur)->file_cursor = bnext(gbl(scr_cur)->file_cursor);
			gbl(scr_cur)->cur_file = (FBLK *)bid(gbl(scr_cur)->file_cursor);
		}
		else
		{
			top = calculate_top_item(j, gbl(scr_cur)->numfiles, num);
			gbl(scr_cur)->top_file = gbl(scr_cur)->first_file;
			for (i=0; i<top; i++)
				gbl(scr_cur)->top_file = bnext(gbl(scr_cur)->top_file);
			gbl(scr_cur)->cur_file_no = j;
			gbl(scr_cur)->cur_file_line =
				(j - top) % (gbl(scr_cur)->max_file_line+1);
			gbl(scr_cur)->cur_file_col  =
				(j - top) / (gbl(scr_cur)->max_file_line+1);
			gbl(scr_cur)->file_cursor = gbl(scr_cur)->top_file;
			for (i=top; i<j; i++)
				gbl(scr_cur)->file_cursor = bnext(gbl(scr_cur)->file_cursor);
			gbl(scr_cur)->cur_file = (FBLK *)bid(gbl(scr_cur)->file_cursor);
			disp_file_list();
		}
		hilite_file(ON);
		do_file_scroll_bar();
		fblk_to_dirname(gbl(scr_cur)->cur_file, gbl(scr_cur)->path_name);
		disp_path_line();
		disp_cur_file();
		disp_drive_info((gbl(scr_cur)->cur_file)->stbuf.st_dev, 0);
	}
}

int check_mouse_pos (void)
{
	int i;
	int j;
	int c;

	c = KEY_MOUSE;

	if (gbl(scr_in_fullview) || gbl(scr_in_hexedit))
	{
		j = check_win_border(gbl(win_full_view));
		if (j)
		{
			if (j == 1)
			{
				c = KEY_UP;
			}
			else if (j == 2)
			{
				c = KEY_DOWN;
			}
		}
		return (c);
	}

	if (gbl(scr_in_diffview))
	{
		return (0);
	}

	if (gbl(scr_is_split) && !gbl(scr_cur)->in_dest_dir)
	{
		if (gbl(scr_in_autoview))
		{
			for (i=0; i<gbl(scr_cur_count); i++)
			{
				if (check_mouse_in_win(gbl(scr_stat)[i].cur_av_file_win, 0) ||
					check_mouse_in_win(gbl(scr_stat)[i].cur_av_view_win, 0))
				{
					if (i != gbl(scr_cur_no))
						toggle_split(i);
					break;
				}
			}
		}
		else
		{
			for (i=0; i<gbl(scr_cur_count); i++)
			{
				if (check_mouse_in_win(gbl(scr_stat)[i].cur_dir_win, 0) ||
					check_mouse_in_win(gbl(scr_stat)[i].cur_file_win, 0))
				{
					if (i != gbl(scr_cur_no))
						toggle_split(i);
					break;
				}
			}
		}
	}

	/* else */			/* not split screen */
	{
		if (gbl(scr_in_autoview))
		{
			j = check_mouse_in_win(gbl(scr_cur)->cur_av_file_win,
				gbl(scr_cur)->numfiles);
			if (j)
			{
				if (gbl(scr_av_focus))
				{
					gbl(scr_av_focus) = FALSE;
					disp_cmds();
				}
				if (j == 3)
				{
					i = get_file_pos();
					j = gbl(scr_cur)->cur_file_line;
					if (i != j)
					{
						find_file(i);
					}
				}
				else if (j == 1)
				{
					c = KEY_UP;
				}
				else if (j == 2)
				{
					c = KEY_DOWN;
				}
				else if (j == 4)
				{
					set_file_pos_from_sb();
				}
			}
			else if (j = check_mouse_in_win(gbl(scr_cur)->cur_av_view_win, 0))
			{
				if (!gbl(scr_av_focus))
				{
					gbl(scr_av_focus) = TRUE;
					disp_cmds();
				}
				if (j == 3)
				{
				}
				else if (j == 1)
				{
					c = KEY_UP;
				}
				else if (j == 2)
				{
					c = KEY_DOWN;
				}
				else if (j == 4)
				{
				}
			}
		}
		else
		{
			if (gbl(scr_cur)->command_mode == m_dir)
			{
				j = check_mouse_in_win(gbl(scr_cur)->cur_dir_win,
					gbl(scr_cur)->numdirs);
				if (j)
				{
					if (j == 3)
					{
						i = mouse_y(gbl(scr_cur)->cur_dir_win) -
							getbegy(gbl(scr_cur)->cur_dir_win);
						if (i != gbl(scr_cur)->cur_dir_line)
						{
							set_cur_dir(i);
						}
					}
					else if (j == 1)
					{
						c = KEY_UP;
					}
					else if (j == 2)
					{
						c = KEY_DOWN;
					}
					else if (j == 4)
					{
						set_dir_pos_from_sb();
					}
				}
				else if (!gbl(scr_cur)->in_dest_dir &&
					(j = check_mouse_in_win(gbl(scr_cur)->cur_file_win,
						gbl(scr_cur)->numfiles)))
				{
					if (j == 3)
					{
						i = get_file_pos();
						setup_file_display(i);
					}
					else if (j == 1)
					{
						c = KEY_UP;
					}
					else if (j == 2)
					{
						c = KEY_DOWN;
					}
					else if (j == 4)
					{
						set_file_pos_from_sb();
					}
				}
			}
			else
			{
				j = check_mouse_in_win(gbl(scr_cur)->cur_file_win,
					gbl(scr_cur)->numfiles);
				if (j)
				{
					if (j == 3)
					{
						i = get_file_pos();
						j = (gbl(scr_cur)->max_file_line+1) *
							gbl(scr_cur)->cur_file_col +
								gbl(scr_cur)->cur_file_line;
						if (i != j)
						{
							find_file(i);
						}
					}
					else if (j == 1)
					{
						c = KEY_UP;
					}
					else if (j == 2)
					{
						c = KEY_DOWN;
					}
					else if (j == 4)
					{
						set_file_pos_from_sb();
					}
				}
				else if (gbl(scr_cur)->in_small_window &&
					(j = check_mouse_in_win(gbl(scr_cur)->cur_dir_win,
						gbl(scr_cur)->numdirs)))
				{
					gbl(scr_cur)->command_mode = m_dir;
					gbl(scr_cur)->in_small_window = FALSE;
					if (j == 3)
					{
						i = mouse_y(gbl(scr_cur)->cur_dir_win) -
							getbegy(gbl(scr_cur)->cur_dir_win);
						if (i != gbl(scr_cur)->cur_dir_line)
						{
							set_cur_dir(i);
						}
					}
					check_small_window();
					hilite_dir(ON);
					dptr_to_dirname(gbl(scr_cur)->dir_cursor,
						gbl(scr_cur)->path_name);
					disp_path_line();
					disp_file_box();
					disp_drive_info(0, 1);
					disp_node_stats(gbl(scr_cur)->cur_root);
					disp_cur_dir();
					disp_cmds();
					if (j == 1)
					{
						c = KEY_UP;
					}
					else if (j == 2)
					{
						c = KEY_DOWN;
					}
					else if (j == 4)
					{
						set_dir_pos_from_sb();
					}
				}
			}
		}
	}
	return (c);
}

int read_vert_scrollbar (WINDOW *win, int n)
{
	int y = mouse_y(win);
	int x = mouse_x(win);
	int s;

	if (x != getbegx(win)-1)
		return (-1);

	y -= getbegy(win);
	if (y < 0 || y >= getmaxy(win))
		return (-1);

	if (y == 0)
		s = 0;
	else if (y == getmaxy(win)-1)
		s = n-1;
	else
		s = (y * n) / getmaxy(win);

	return (s);
}

int calculate_top_item (int cur, int num, int lines)
{
	int top;

	if (cur+lines >= num)
	{
		top = num - lines;
	}
	else if (cur < lines)
	{
		top = 0;
	}
	else
	{
		top = cur;
	}
	return (top);
}
