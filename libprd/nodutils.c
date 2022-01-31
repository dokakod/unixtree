/*------------------------------------------------------------------------
 * node utilities
 */
#include "libprd.h"

void init_cd (void)
{
	gbl(scr_cur_count)	= 1;
	gbl(scr_cur_no)		= 0;
	gbl(scr_cur_dev)	= 0;
	gbl(scr_cur_free)	= -1;
	gbl(scr_cur)				= &gbl(scr_stat)[0];

	if (opt(wide_screen))
	{
		gbl(scr_cur)->cur_dir_win      = gbl(win_dir_wide);
		gbl(scr_cur)->cur_dest_dir_win = gbl(win_dest_dir_wide);
		gbl(scr_cur)->large_file_win   = gbl(win_large_file_wide);
		gbl(scr_cur)->small_file_win   = gbl(win_small_file_wide);
	}
	else
	{
		gbl(scr_cur)->cur_dir_win      = gbl(win_dir_reg);
		gbl(scr_cur)->cur_dest_dir_win = gbl(win_dest_dir_reg);
		gbl(scr_cur)->large_file_win   = gbl(win_large_file_reg);
		gbl(scr_cur)->small_file_win   = gbl(win_small_file_reg);
	}
	gbl(scr_cur)->cur_path_line        = gbl(win_path_line_reg);
	gbl(scr_cur)->cur_file_win         = gbl(scr_cur)->small_file_win;
	gbl(scr_cur)->cur_av_path_line     = gbl(win_av_path_line_reg);
	gbl(scr_cur)->cur_av_mode_win      = gbl(win_av_mode_reg);
	gbl(scr_cur)->cur_av_file_win      = gbl(win_av_file_reg);
	gbl(scr_cur)->cur_av_view_win      = gbl(win_av_view_reg);
	gbl(scr_cur)->cur_av_dest_dir_win  = gbl(win_av_dest_dir_reg);

	wattrset (gbl(win_large_file_reg), gbl(scr_reg_hilite_attr));
	wstandset(gbl(win_large_file_reg), gbl(scr_reg_cursor_attr));

	gbl(scr_cur)->vfcb             = gbl(vfcb_av);

	gbl(scr_cur)->dir_fmt          = opt(dir_fmt);
	gbl(scr_cur)->file_fmt         = opt(file_fmt);
	gbl(scr_cur)->max_dir_line     = getmaxy(gbl(scr_cur)->cur_dir_win)-1;
	gbl(scr_cur)->max_dir_level    =
		get_max_dir_level(gbl(scr_cur)->cur_dir_win);
	gbl(scr_cur)->max_file_line    = getmaxy(gbl(scr_cur)->cur_file_win)-1;
	gbl(scr_cur)->global_flist     = 0;
	gbl(scr_cur)->global_mlist     = 0;
	gbl(scr_cur)->in_dest_dir      = 0;
	gbl(scr_cur)->invert_file_spec = 0;
	gbl(scr_cur)->dir_scroll       = opt(dir_scroll);
	gbl(scr_cur)->tag_mask         = 0x01;
	gbl(scr_cur)->dir_scroll_bar_displayed  = FALSE;
	gbl(scr_cur)->file_scroll_bar_displayed = FALSE;
	gbl(scr_cur)->dir_scroll_bar   = -1;
	gbl(scr_cur)->file_scroll_bar  = -1;
	gbl(scr_cur)->in_small_window  = 0;
	gbl(scr_cur)->command_mode     = m_dir;
	gbl(scr_cur)->cmd_sub_mode     = m_reg;
	gbl(scr_cur)->file_spec_pres   = FALSE;
	strcpy(gbl(scr_cur)->file_spec, fn_all());
	gbl(scr_cur)->cur_dir_line     = pgm_const(default_dir_line);
	gbl(scr_cur)->cur_file_no      = 0;
	gbl(scr_cur)->cur_file_col     = 0;
	gbl(scr_cur)->cur_file_line    = 0;
	gbl(scr_cur)->cur_dir_no       = 0;

	set_file_disp();
	set_dir_disp();
}

int init_node (const char *directory)
{
	BLIST *b;
	NBLK *node;
	char fullpath[MAX_PATHLEN];

	fn_get_abs_dir(gbl(pgm_cwd), directory, fullpath);
	node = nblk_make();
	if (node == 0)
		return (1);

	strcpy(node->root_name, fullpath);
	node->node_type = N_FS;

	gbl(scr_cur)->cur_root  = node;
	if (read_node(node))
		return (1);

	b = BNEW(node);
	if (b == 0)
	{
		del_node(node);
		nblk_free(node);
		errmsg(ER_IM, "", ERR_ANY);
		return (1);
	}

	gbl(nodes_list) = bappend(gbl(nodes_list), b);
	gbl(scr_cur)->cur_node = b;
	return (0);
}

int read_node (NBLK *node)
{
	struct stat stbuf;
	int rc;

	if (is_directory(node->root_name, &stbuf))
		return (-1);

	if (gbl(scr_cur)->command_mode == m_dir || gbl(scr_cur)->in_small_window)
		dir_scroll_bar_remove();
	file_scroll_bar_remove();
	gbl(scr_cur)->cur_file_win = gbl(scr_cur)->small_file_win;
	gbl(scr_cur)->command_mode = m_dir;

	if (! opt(keep_file_spec))
	{
		rm_file_spec();
		strcpy(gbl(scr_cur)->file_spec, fn_all());
		disp_file_box();
	}

	/*--------------------------------------------------------------------
	 * display headline if not first time
	 */
	if (! gbl(scr_cur)->in_dest_dir && gbl(nodes_list) != 0)
	{
		if (! opt(scroll_during_logging))
		{
			werase(gbl(scr_cur)->cur_dir_win);
			headline();
			wrefresh(gbl(scr_cur)->cur_dir_win);
		}
		small_border(ON);
		disp_credits();
	}

	rc = getft(node, &stbuf);
	return (rc);
}

void del_node (NBLK *n)
{
	BLIST *b;
	BLIST *x;
	TREE *t;
	DBLK *d;
	FBLK *f;
	int i;

	for (b=n->dir_list; b; b=bnext(b))
	{
		t = (TREE *)bid(b);
		d = (DBLK *)tid(t);
		for (i=0; i<gbl(scr_cur_count); i++)
		{
			if (gbl(scr_stat)[i].file_spec_pres)
				BSCRAP(d->mlist[i], FALSE);
		}
		for (x=d->flist; x; x=bnext(x))
		{
			f = (FBLK *)bid(x);
			fblk_free(f);
		}
		BSCRAP(d->flist, FALSE);
		dblk_free(d);
		TFREE(t, FALSE);
	}

	switch (n->node_type)
	{
	case N_FS:
		break;

	case N_ARCH:
		{
			ABLK *a = (ABLK *)n->node_sub_blk;

			if (*a->arch_tmpname != 0)
				os_file_delete(a->arch_tmpname);
			ablk_free(a);
		}
		break;

	case N_FTP:
		delete_ftp_node(n);
		break;
	}

	n->node_type		= 0;
	n->node_flags		= 0;
	n->node_sub_blk		= 0;
	n->root				= 0;
	n->cur_dir_tree		= 0;
	n->dir_list			= BSCRAP(n->dir_list, FALSE);
	n->showall_flist	= BSCRAP(n->showall_flist, FALSE);
	n->link_list		= BSCRAP(n->link_list, FALSE);

	for (i=0; i<gbl(scr_cur_count); i++)
	{
		if (n->dir_disp_list[i])
			n->dir_disp_list[i] = BSCRAP(n->dir_disp_list[i], FALSE);

		if (gbl(scr_stat)[i].file_spec_pres)
			n->showall_mlist[i] = BSCRAP(n->showall_mlist[i], FALSE);
		else
			n->showall_mlist[i] = 0;
	}

	n->node_total_count		= 0;
	n->node_total_bytes		= 0;
	n->node_match_count		= 0;
	n->node_match_bytes		= 0;
	n->node_tagged_count	= 0;
	n->node_tagged_bytes	= 0;
	n->node_tagged_blocks	= 0;
}

void setup_node (void)
{
	int i;

	if (! gbl(scr_cur)->dir_scroll)
		gbl(scr_cur)->cur_dir_line = 0;
	gbl(scr_cur)->dir_scroll_bar_displayed = FALSE;
	gbl(scr_cur)->file_scroll_bar_displayed = FALSE;
	gbl(scr_cur)->dir_scroll_bar = -1;
	gbl(scr_cur)->file_scroll_bar = -1;
	gbl(scr_cur)->cur_root     = (NBLK *)bid(gbl(scr_cur)->cur_node);
	gbl(scr_cur)->cur_dir_no   = 0;
	gbl(scr_cur)->first_dir    =
		(gbl(scr_cur)->cur_root)->dir_disp_list[gbl(scr_cur_no)] ?
			(gbl(scr_cur)->cur_root)->dir_disp_list[gbl(scr_cur_no)] :
			(gbl(scr_cur)->cur_root)->dir_list;
	gbl(scr_cur)->top_dir      = gbl(scr_cur)->first_dir;
	gbl(scr_cur)->dir_cursor   = gbl(scr_cur)->first_dir;
	gbl(scr_cur)->cur_dir_tree = (TREE *)bid(gbl(scr_cur)->dir_cursor);
	gbl(scr_cur)->cur_dir      = (DBLK *)tid(gbl(scr_cur)->cur_dir_tree);
	gbl(scr_cur)->numdirs      = bcount(gbl(scr_cur)->first_dir);

	if (! opt(home_to_node) && (gbl(scr_cur)->cur_root)->cur_dir_tree != 0)
	{
		gbl(scr_cur)->cur_dir_tree = (gbl(scr_cur)->cur_root)->cur_dir_tree;
		gbl(scr_cur)->cur_dir      = (DBLK *)tid(gbl(scr_cur)->cur_dir_tree);
		gbl(scr_cur)->dir_cursor   =
			bfind(gbl(scr_cur)->first_dir, gbl(scr_cur)->cur_dir_tree);
		gbl(scr_cur)->cur_dir_no   =
			bindex(gbl(scr_cur)->first_dir, gbl(scr_cur)->dir_cursor);

		if (! gbl(scr_cur)->dir_scroll)
		{
			if (gbl(scr_cur)->cur_dir_no <= gbl(scr_cur)->max_dir_line)
			{
				gbl(scr_cur)->cur_dir_line = gbl(scr_cur)->cur_dir_no;
			}
			else
			{
				if (gbl(scr_cur)->cur_dir_no + gbl(scr_cur)->max_dir_line >=
					gbl(scr_cur)->numdirs-1)
				{
					i = gbl(scr_cur)->numdirs-1-gbl(scr_cur)->max_dir_line;
				}
				else
				{
					i = gbl(scr_cur)->cur_dir_no - pgm_const(default_dir_line);
				}

				for (gbl(scr_cur)->top_dir=gbl(scr_cur)->dir_cursor;
					i<gbl(scr_cur)->cur_dir_no; i++)
				{
					gbl(scr_cur)->cur_dir_line++;
					gbl(scr_cur)->top_dir = bprev(gbl(scr_cur)->top_dir);
				}
			}
		}
	}
	gbl(scr_cur)->command_mode		= m_dir;
	gbl(scr_cur)->cmd_sub_mode		= m_reg;
	gbl(scr_cur)->in_small_window	= FALSE;
	gbl(scr_cur)->tag_mask			= 0x01;
}

void init_disp (void)
{
	setup_node();
	if (gbl(scr_cur)->in_dest_dir)
	{
		disp_dir_tree();
		hilite_dir(ON);
		do_dir_scroll_bar();
	}
	else
	{
		dptr_to_dirname(gbl(scr_cur)->dir_cursor, gbl(scr_cur)->path_name);
		disp_path_line();
		if (! gbl(scr_is_split))
		{
			disp_drive_info((gbl(scr_cur)->cur_dir)->stbuf.st_dev, 1);
			disp_node_stats(gbl(scr_cur)->cur_root);
			disp_cur_dir();
		}
		disp_dir_tree();
		do_dir_scroll_bar();
		hilite_dir(ON);
		check_small_window();
		disp_cmds();
	}
}

static void set_cur_file (int n)
{
	int i;

	gbl(scr_cur)->file_cursor   = gbl(scr_cur)->first_file;
	for (i=0; i<n; i++)
	{
		if (bnext(gbl(scr_cur)->file_cursor))
			gbl(scr_cur)->file_cursor = bnext(gbl(scr_cur)->file_cursor);
		else
			break;
	}
	gbl(scr_cur)->cur_file      = (FBLK *)bid(gbl(scr_cur)->file_cursor);
	gbl(scr_cur)->cur_file_no   =
		bindex(gbl(scr_cur)->first_file, gbl(scr_cur)->file_cursor);
	gbl(scr_cur)->cur_file_line = i % (gbl(scr_cur)->max_file_line+1);
	gbl(scr_cur)->cur_file_col  = i / (gbl(scr_cur)->max_file_line+1);
}

void setup_file_display (int n)
{
	gbl(scr_cur)->base_file  = (gbl(scr_cur)->cur_dir)->flist;
	gbl(scr_cur)->first_file = (gbl(scr_cur)->cur_dir)->mlist[gbl(scr_cur_no)];
	if (! gbl(scr_cur)->first_file)
	{
		file_scroll_bar_remove();
		return;
	}
	gbl(scr_cur)->command_mode = m_file;
	setup_dir_stats();
	gbl(scr_cur)->top_file = gbl(scr_cur)->first_file;
	set_cur_file(n);

	if (opt(do_small_window) || gbl(scr_cur)->in_small_window)
	{
		gbl(scr_cur)->cur_file_win = gbl(scr_cur)->small_file_win;
		gbl(scr_cur)->max_file_line = getmaxy(gbl(scr_cur)->cur_file_win)-1;
		gbl(scr_cur)->in_small_window = 1;
		hilite_dir(OFF);
		left_arrow(gbl(scr_cur)->cur_dir_win);
		wrefresh(gbl(scr_cur)->cur_dir_win);
	}
	else
	{
		gbl(scr_cur)->cur_file_win = gbl(scr_cur)->large_file_win;
		wattrset (gbl(scr_cur)->cur_file_win, gbl(scr_reg_hilite_attr));
		wstandset(gbl(scr_cur)->cur_file_win, gbl(scr_reg_cursor_attr));
		gbl(scr_cur)->max_file_line = getmaxy(gbl(scr_cur)->cur_file_win)-1;
		gbl(scr_cur)->in_small_window = 0;
		small_border(OFF);
		werase(gbl(scr_cur)->cur_file_win);
		disp_file_list();
	}
	do_file_scroll_bar();
	hilite_file(ON);
	disp_dir_stats();
	disp_cur_file();
	disp_cmds();
}

void setup_sa_display (void)
{
	if (! gbl(scr_cur)->first_file)
	{
		gbl(scr_cur)->command_mode = m_dir;
		return;
	}

	gbl(scr_cur)->file_cursor     = gbl(scr_cur)->first_file;
	gbl(scr_cur)->top_file        = gbl(scr_cur)->file_cursor;
	gbl(scr_cur)->cur_file        = (FBLK *)bid(gbl(scr_cur)->first_file);
	gbl(scr_cur)->cur_file_no     = 0;
	gbl(scr_cur)->cur_file_line   = 0;
	gbl(scr_cur)->cur_file_col    = 0;
	gbl(scr_cur)->cur_file_win    = gbl(scr_cur)->large_file_win;
	wattrset (gbl(scr_cur)->cur_file_win, gbl(scr_sa_hilite_attr));
	wstandset(gbl(scr_cur)->cur_file_win, gbl(scr_sa_cursor_attr));
	gbl(scr_cur)->max_file_line   = getmaxy(gbl(scr_cur)->cur_file_win)-1;
	gbl(scr_cur)->in_small_window = 0;
	small_border(OFF);
	werase(gbl(scr_cur)->cur_file_win);
	disp_file_list();
	do_file_scroll_bar();
	hilite_file(ON);
	fblk_to_dirname(gbl(scr_cur)->cur_file, gbl(scr_cur)->path_name);
	disp_path_line();
	disp_file_box();
	disp_drive_info(0, 1);
	disp_dir_stats();
	disp_cur_file();
	disp_cmds();
}
