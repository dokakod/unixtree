/*------------------------------------------------------------------------
 * process the "goto" cmd
 */
#include "libprd.h"

void goto_file (void)
{
	int c;
	int i;
	char input_str[MAX_FILELEN];
	BLIST *b;
	BLIST *be;
	FBLK *f;

	bang(msgs(m_goto_entfil));

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_goto_gotfil));
	wrefresh(gbl(win_commands));

	*input_str = 0;
	c = xgetstr(gbl(win_commands), input_str, XGL_GOTO_FILE, MAX_FILELEN, 0,
		XG_FILENAME);
	if (c <= 0)
		return;

	be = 0;
	f = 0;
	for (b=gbl(scr_cur)->first_file; b; b=bnext(b))
	{
		f = (FBLK *)bid(b);
		i = strncmp(input_str, FULLNAME(f), c);
		if (i < 0)
		{
			if (!be)
				be = b;
		}
		else if (i == 0)
		{
			break;
		}
	}

	if (!b)
		b = be;
	if (b)
	{
		int frow;
		int fcol;

		gbl(scr_cur)->cur_file = f;
		if (is_file_displayed(f, &frow, &fcol))
		{
			gbl(scr_cur)->file_cursor   = b;
			gbl(scr_cur)->cur_file_no   =
				bindex(gbl(scr_cur)->first_file, gbl(scr_cur)->file_cursor);
			gbl(scr_cur)->cur_file_line = frow;
			gbl(scr_cur)->cur_file_col  = fcol;
		}
		else
		{
			locate_cur_file();
		}
		fblk_to_dirname(gbl(scr_cur)->cur_file, gbl(scr_cur)->path_name);
		disp_path_line();
		disp_file_list();
		hilite_file(ON);
		do_file_scroll_bar();
	}
}

void goto_dir (void)
{
	char input_str[MAX_FILELEN];
	BLIST *b;
	TREE *t;
	DBLK *d;
	int n;
	int c;

	if (! gbl(scr_cur)->in_dest_dir)
	{
		bang(msgs(m_goto_entdir));
		werase(gbl(win_commands));
		wmove(gbl(win_commands), 0, 0);
		xaddstr(gbl(win_commands), msgs(m_goto_gotdir));
		wrefresh(gbl(win_commands));

		*input_str = 0;
		c = xgetstr(gbl(win_commands), input_str, XGL_GOTO_DIR, MAX_FILELEN, 0,
			 XG_FILENAME);
	}
	else
	{
		bang(msgs(m_goto_gotdir));

		*input_str = 0;
		c = xgetstr(gbl(win_message), input_str, XGL_GOTO_DIR, MAX_FILELEN, 0,
			 XG_FILENAME);
	}

	if (c <= 0)
		return;

	n = 1;
	for (b=bnext(gbl(scr_cur)->dir_cursor); b; b=bnext(b))
	{
		t = (TREE *)bid(b);
		d = (DBLK *)tid(t);

		if (strccmpn(input_str, FULLNAME(d)) == 0)
			break;

		n++;
	}

	if (b == 0)
		return;

	hilite_dir(OFF);
	gbl(scr_cur)->dir_cursor    = b;
	gbl(scr_cur)->cur_dir_tree  = t;
	gbl(scr_cur)->cur_dir       = d;
	gbl(scr_cur)->cur_dir_no   += n;
	gbl(scr_cur)->cur_dir_line += n;
	if (gbl(scr_cur)->cur_dir_line > gbl(scr_cur)->max_dir_line)
	{
		gbl(scr_cur)->top_dir      = b;
		gbl(scr_cur)->cur_dir_line = 0;
	}
	disp_dir_tree();
	do_dir_scroll_bar();
	hilite_dir(ON);

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
