/*------------------------------------------------------------------------
 * process the "split/unsplit/toggle-split" cmds
 */
#include "libprd.h"

int next_scr_no (int scr_no)
{
	scr_no++;
	if (scr_no >= gbl(scr_cur_count))
		scr_no = 0;

	return (scr_no);
}

void do_split (void)
{
	BLIST *b;
	BLIST *x;
	BLIST *l;
	NBLK *n;
	FBLK *f;
	FBLK *g;
	DBLK *d;
	TREE *t;
	VFCB *v;
	int i;
	int is_dup;

	gbl(scr_is_split) = ! gbl(scr_is_split);
	if (gbl(scr_is_split))
	{
		/* split the screen */

		gbl(scr_cur_count) = gbl(scr_num_split_wins);
		for (i=0; i<gbl(scr_cur_count); i++)
		{
			gbl(scr_cur) = &gbl(scr_stat)[i];
			clone_screen(0, i);
			set_file_disp();
			set_dir_disp();
			if (gbl(scr_stat)[i].cur_root->node_type == N_FS &&
				gbl(vfcb_av)->v_fd)
			{
				v = gbl(vfcb_av_split)[i];
				strcpy(v->v_pathname, gbl(vfcb_av)->v_pathname);
				view_command(v, V_CMD_OPEN_RO);
				if (v->v_fd)
				{
					gbl(scr_stat)[i].av_file_displayed =
						gbl(scr_stat)[i].cur_file;
					v->v_mode    = gbl(vfcb_av)->v_mode;
					v->v_top_pos = gbl(vfcb_av)->v_top_pos;
					v->v_cur_pos = gbl(vfcb_av)->v_cur_pos;
					v->v_fnd_ptr = 0;
					v->v_end_ptr = 0;
					get_buffer(v);
				}
			}
		}
		if (gbl(scr_stat)[0].cur_root->node_type == N_FS && gbl(vfcb_av)->v_fd)
			view_command(gbl(vfcb_av), V_CMD_CLOSE);
		gbl(scr_cur_no) = 0;
		gbl(scr_cur) = &gbl(scr_stat)[0];
	}
	else
	{
		/* unsplit the screen */

		gbl(scr_info_displayed) = 0;

		for (i=0; i<gbl(scr_cur_count); i++)
		{
			if (i != gbl(scr_cur_no))
			{
				if (gbl(scr_stat)[i].global_flist)
				{
					gbl(scr_stat)[i].global_flist =
						BSCRAP(gbl(scr_stat)[i].global_flist, FALSE);
					if (gbl(scr_stat)[i].file_spec_pres)
					{
						gbl(scr_stat)[i].global_mlist =
							BSCRAP(gbl(scr_stat)[i].global_mlist, FALSE);
					}
				}
			}
		}

		for (b=gbl(nodes_list); b; b=bnext(b))
		{
			n = (NBLK *)bid(b);
			for (i=0; i<gbl(scr_cur_count); i++)
			{
				if (i == gbl(scr_cur_no))
					continue;
				if (gbl(scr_stat)[i].file_spec_pres)
					BSCRAP(n->showall_mlist[i], FALSE);
				BSCRAP(n->dir_disp_list[i], FALSE);
			}
			n->showall_mlist[0]  = n->showall_mlist[gbl(scr_cur_no)];
			n->dir_disp_list[0]  = n->dir_disp_list[gbl(scr_cur_no)];
			for (x=n->dir_list; x; x=bnext(x))
			{
				t = (TREE *)bid(x);
				d = (DBLK *)tid(t);
				if (d->hidden & gbl(scr_stat)[gbl(scr_cur_no)].tag_mask)
					d->hidden = 1;
				else
					d->hidden = 0;
				for (i=0; i<gbl(scr_cur_count); i++)
				{
					if (i == gbl(scr_cur_no))
						continue;
					if (gbl(scr_stat)[i].file_spec_pres)
						d->mlist[i] = BSCRAP(d->mlist[i], FALSE);
				}
				d->mlist[0] = d->mlist[gbl(scr_cur_no)];
			}
			n->node_total_bytes = 0;
			n->node_total_count = 0;
			n->node_tagged_count = 0;
			n->node_tagged_bytes = 0;
			n->node_tagged_blocks = 0;
			for (x=n->showall_flist; x; x=bnext(x))
			{
				n->node_total_count++;
				f = (FBLK *)bid(x);
				if (f->tagged & gbl(scr_stat)[gbl(scr_cur_no)].tag_mask)
				{
					f->tagged = 1;
					n->node_tagged_count++;
				}
				else
					f->tagged = 0;
				is_dup = FALSE;
				if (f->stbuf.st_nlink > 1)
				{
					/* is dup if not first occurance of link in list */
					for (l=n->link_list; l && (g=(FBLK *)bid(l)) != f;
						l=bnext(l))
					{
						if (g->stbuf.st_dev == f->stbuf.st_dev &&
				    		g->stbuf.st_ino == f->stbuf.st_ino)
						{
							is_dup = TRUE;
							break;
						}
					}
				}
				if (!is_dup)
				{
					n->node_total_bytes += f->stbuf.st_size;
					if (f->tagged)
					{
						n->node_tagged_bytes += f->stbuf.st_size;
						n->node_tagged_blocks += file_size_in_blks(f,
							f->stbuf.st_size);
					}
				}
			}
			if (gbl(scr_stat)[gbl(scr_cur_no)].file_spec_pres)
			{
				n->node_match_count = bcount(n->showall_mlist[0]);
				n->node_match_bytes = sum_files_in_list(n->showall_mlist[0],
					0);
			}
			else
			{
				n->node_match_count = n->node_total_count;
				n->node_match_bytes = n->node_total_bytes;
			}
		}
		if (gbl(scr_cur_no))
			copy_dstat(&gbl(scr_stat)[gbl(scr_cur_no)], &gbl(scr_stat)[0]);
		gbl(scr_cur_count) = 1;
		gbl(scr_cur) = &gbl(scr_stat)[0];
		gbl(scr_cur)->tag_mask            = 1;
		gbl(scr_cur)->cur_av_path_line    = gbl(win_av_path_line_reg);
		gbl(scr_cur)->cur_av_mode_win     = gbl(win_av_mode_reg);
		gbl(scr_cur)->cur_av_file_win     = gbl(win_av_file_reg);
		gbl(scr_cur)->cur_av_view_win     = gbl(win_av_view_reg);
		gbl(scr_cur)->cur_av_dest_dir_win = gbl(win_av_dest_dir_reg);
		gbl(scr_cur)->vfcb                = gbl(vfcb_av);
		gbl(scr_cur)->cur_path_line       = gbl(win_path_line_reg);
		if (opt(wide_screen))
		{
			gbl(scr_cur)->cur_dir_win         = gbl(win_dir_wide);
			gbl(scr_cur)->cur_dest_dir_win    = gbl(win_dest_dir_wide);
			gbl(scr_cur)->large_file_win      = gbl(win_large_file_wide);
			gbl(scr_cur)->small_file_win      = gbl(win_small_file_wide);
		}
		else
		{
			gbl(scr_cur)->cur_dir_win         = gbl(win_dir_reg);
			gbl(scr_cur)->cur_dest_dir_win    = gbl(win_dest_dir_reg);
			gbl(scr_cur)->large_file_win      = gbl(win_large_file_reg);
			gbl(scr_cur)->small_file_win      = gbl(win_small_file_reg);
		}
		if (gbl(scr_in_autoview))
		{
			gbl(scr_cur)->cur_dest_dir_win = gbl(win_av_dest_dir_reg);
			gbl(scr_cur)->cur_path_line    = gbl(win_av_path_line_reg);
			gbl(scr_cur)->cur_file_win     = gbl(win_av_file_reg);
		}
		else
		{
			gbl(scr_cur)->cur_dest_dir_win = gbl(win_dest_dir_reg);
			gbl(scr_cur)->cur_path_line    = gbl(win_path_line_reg);
			if (gbl(scr_cur)->command_mode == m_dir ||
				gbl(scr_cur)->in_small_window)
			{
				gbl(scr_cur)->cur_file_win = (opt(wide_screen) ?
					gbl(win_small_file_wide): gbl(win_small_file_reg));
			}
			else
			{
				gbl(scr_cur)->cur_file_win = (opt(wide_screen) ?
					gbl(win_large_file_wide): gbl(win_large_file_reg));
			}
			set_file_disp();
			set_dir_disp();
		}
		gbl(scr_cur)->max_file_line = getmaxy(gbl(scr_cur)->cur_file_win)-1;
		if (gbl(scr_cur)->command_mode == m_dir ||
			gbl(scr_cur)->command_mode == m_file)
		{
			wattrset (gbl(scr_cur)->large_file_win, gbl(scr_reg_hilite_attr));
			wstandset(gbl(scr_cur)->large_file_win, gbl(scr_reg_cursor_attr));
		}
		else
		{
			wattrset (gbl(scr_cur)->large_file_win, gbl(scr_sa_hilite_attr));
			wstandset(gbl(scr_cur)->large_file_win, gbl(scr_sa_cursor_attr));
		}
		wattrset (gbl(scr_cur)->cur_av_file_win,
			wattrget(gbl(scr_cur)->large_file_win));
		wstandset(gbl(scr_cur)->cur_av_file_win,
			wstandget(gbl(scr_cur)->large_file_win));
		v = gbl(vfcb_av_split)[gbl(scr_cur_no)];
		if ((gbl(scr_cur)->cur_root)->node_type == N_FS && gbl(vfcb_av)->v_fd)
		{
			strcpy(gbl(vfcb_av)->v_pathname, v->v_pathname);
			view_command(gbl(vfcb_av), V_CMD_OPEN_RO);
			if (gbl(vfcb_av)->v_fd)
			{
				gbl(vfcb_av)->v_mode = v->v_mode;
				gbl(vfcb_av)->v_top_pos = v->v_top_pos;
				gbl(vfcb_av)->v_cur_pos = v->v_cur_pos;
				get_buffer(gbl(vfcb_av));
			}
		}

		if ((gbl(scr_cur)->cur_root)->node_type == N_FS)
		{
			for (i=0; i<gbl(scr_num_split_wins); i++)
			{
				if (gbl(vfcb_av_split)[i]->v_fd)
					view_command(gbl(vfcb_av_split)[i], V_CMD_CLOSE);
			}
		}
		gbl(scr_cur_no) = 0;
	}
	setup_display();
}

void clone_screen (int from, int to)
{
	BLIST *b;
	BLIST *x;
	NBLK *n;
	FBLK *f;
	DBLK *d;
	TREE *t;

	/* copy structure contents */

	if (to)
		copy_dstat(&gbl(scr_stat)[from], &gbl(scr_stat)[to]);

	/* reset window pointers to split windows */

	gbl(scr_stat)[to].cur_av_path_line    = gbl(win_av_path_line_split)[to];
	gbl(scr_stat)[to].cur_av_mode_win     = gbl(win_av_mode_split)[to];
	gbl(scr_stat)[to].cur_av_file_win     = gbl(win_av_file_split)[to];
	gbl(scr_stat)[to].cur_av_view_win     = gbl(win_av_view_split)[to];
	gbl(scr_stat)[to].cur_av_dest_dir_win = gbl(win_av_dest_dir_split)[to];
	gbl(scr_stat)[to].vfcb                = gbl(vfcb_av_split)[to];
	gbl(scr_stat)[to].cur_dir_win         = gbl(win_dir_split)[to];
	gbl(scr_stat)[to].large_file_win      = gbl(win_large_file_split)[to];
	gbl(scr_stat)[to].small_file_win      = gbl(win_small_file_split)[to];

	if (gbl(scr_in_autoview))
	{
		gbl(scr_stat)[to].cur_path_line    = gbl(win_av_path_line_split)[to];
		gbl(scr_stat)[to].cur_dest_dir_win = gbl(win_av_dest_dir_split)[to];
		gbl(scr_stat)[to].cur_file_win     = gbl(win_av_file_split)[to];
	}
	else
	{
		gbl(scr_stat)[to].cur_path_line    = gbl(win_path_line_split)[to];
		gbl(scr_stat)[to].cur_dest_dir_win = gbl(win_dest_dir_split)[to];
		if (gbl(scr_stat)[to].command_mode == m_dir ||
			gbl(scr_stat)[to].in_small_window)
		{
			gbl(scr_stat)[to].cur_file_win = gbl(win_small_file_split)[to];
		}
		else
		{
			gbl(scr_stat)[to].cur_file_win = gbl(win_large_file_split)[to];
		}
	}

	if (gbl(scr_stat)[to].command_mode == m_dir ||
	    gbl(scr_stat)[to].command_mode == m_file)
	{
		wattrset (gbl(scr_stat)[to].large_file_win, gbl(scr_reg_hilite_attr));
		wstandset(gbl(scr_stat)[to].large_file_win, gbl(scr_reg_cursor_attr));
	}
	else
	{
		wattrset (gbl(scr_stat)[to].large_file_win, gbl(scr_sa_hilite_attr));
		wstandset(gbl(scr_stat)[to].large_file_win, gbl(scr_sa_cursor_attr));
	}
	wattrset (gbl(scr_stat)[to].cur_av_file_win,
		wattrget(gbl(scr_stat)[to].large_file_win));
	wstandset(gbl(scr_stat)[to].cur_av_file_win,
		wstandget(gbl(scr_stat)[to].large_file_win));

	gbl(scr_stat)[to].max_file_line =
		getmaxy(gbl(scr_stat)[to].cur_file_win)-1;
	if (gbl(scr_in_autoview))
	{
		if (gbl(scr_stat)[to].cur_file_line > gbl(scr_stat)[to].max_file_line)
		{
			gbl(scr_stat)[to].cur_file_line = 0;
			gbl(scr_stat)[to].top_file = gbl(scr_stat)[to].file_cursor;
		}
	}
	gbl(scr_stat)[to].tag_mask = 1 << to;

	/* check if global lists have to be duped */

	if (to && gbl(scr_stat)[to].global_flist)
	{
		gbl(scr_stat)[to].global_flist = BDUP(gbl(scr_stat)[to].global_flist);
		if (gbl(scr_stat)[to].file_spec_pres)
		{
			gbl(scr_stat)[to].global_mlist =
				get_mlist(gbl(scr_stat)[to].global_flist,
					gbl(scr_stat)[to].file_spec, 0, 0);
		}
		else
		{
			gbl(scr_stat)[to].global_mlist = gbl(scr_stat)[to].global_flist;
		}
	}

	/* adjust node dir lists */

	for (b=gbl(nodes_list); b; b=bnext(b))
	{
		n = (NBLK *)bid(b);
		if (n->dir_disp_list[from])
			n->dir_disp_list[to] = BDUP(n->dir_disp_list[from]);
		else
			n->dir_disp_list[to] = 0;
		if (gbl(scr_stat)[to].file_spec_pres)
			n->showall_mlist[to] = BDUP(n->showall_mlist[from]);
		else
			n->showall_mlist[to] = n->showall_flist;
		for (x=n->dir_list; x; x=bnext(x))
		{
			t = (TREE *)bid(x);
			d = (DBLK *)tid(t);
			if (d->hidden & gbl(scr_stat)[from].tag_mask)
				d->hidden |= gbl(scr_stat)[to].tag_mask;
			if (gbl(scr_stat)[to].file_spec_pres)
				d->mlist[to] = BDUP(d->mlist[from]);
			else
				d->mlist[to] = d->flist;
		}
		for (x=n->showall_flist; x; x=bnext(x))
		{
			f = (FBLK *)bid(x);
			if (f->tagged & gbl(scr_stat)[from].tag_mask)
				f->tagged |= gbl(scr_stat)[to].tag_mask;
		}
	}
}

void kill_screen (int num)
{
	BLIST *b;
	BLIST *x;
	NBLK *n;
	FBLK *f;
	DBLK *d;
	TREE *t;

	if (gbl(scr_stat)[num].global_flist)
	{
		BSCRAP(gbl(scr_stat)[num].global_flist, FALSE);
		if (gbl(scr_stat)[num].file_spec_pres)
			BSCRAP(gbl(scr_stat)[num].global_mlist, FALSE);
	}

	for (b=gbl(nodes_list); b; b=bnext(b))
	{
		n = (NBLK *)bid(b);
		if (gbl(scr_stat)[num].file_spec_pres)
			BSCRAP(n->showall_mlist[num], FALSE);
		BSCRAP(n->dir_disp_list[num], FALSE);

		for (x=n->dir_list; x; x=bnext(x))
		{
			t = (TREE *)bid(x);
			d = (DBLK *)tid(t);
			d->hidden &= ~gbl(scr_stat)[num].tag_mask;
			if (gbl(scr_stat)[num].file_spec_pres)
				BSCRAP(d->mlist[num], FALSE);
		}
		for (x=n->showall_flist; x; x=bnext(x))
		{
			n->node_total_count++;
			f = (FBLK *)bid(x);
			f->tagged &= ~gbl(scr_stat)[num].tag_mask;
		}
	}
}

void disp_splits (void)
{
	int i;

	if (opt(display_clock))
		win_clock_set(FALSE);
	split_main_border();
	for (i=0; i<gbl(scr_cur_count); i++)
	{
		if (i != gbl(scr_cur_no))
			is_screen_kosher(i);
		disp_split_window(i);
	}
}

void disp_split_window (int i)
{
	int save_sn;
	DSTAT *save_cd;

	save_sn = gbl(scr_cur_no);
	save_cd = gbl(scr_cur);
	gbl(scr_cur_no) = i;
	gbl(scr_cur) = &gbl(scr_stat)[i];
	disp_path_line();
	set_dir_disp();
	set_file_disp();
	if (gbl(scr_cur)->command_mode == m_dir || gbl(scr_cur)->in_small_window)
	{
		disp_dir_tree();
		dir_scroll_bar_init();
		do_dir_scroll_bar();
		if (gbl(scr_cur_no) == save_sn && gbl(scr_cur)->command_mode == m_dir)
			hilite_dir(ON);
		else
		{
			hilite_dir(OFF);		/* for positioning (LAZY) */
			left_arrow(gbl(scr_cur)->cur_dir_win);
			wrefresh(gbl(scr_cur)->cur_dir_win);
		}
		small_border(ON);
	}
	file_scroll_bar_init();
	if (gbl(scr_cur)->command_mode != m_dir)
	{
		werase(gbl(scr_cur)->cur_file_win);
		disp_file_list();
		do_file_scroll_bar();
		if (gbl(scr_cur_no) == save_sn)
			hilite_file(ON);
		else
		{
			hilite_file(OFF);
			waddch(gbl(scr_cur)->cur_file_win, pgm_const(selected_char));
			wrefresh(gbl(scr_cur)->cur_file_win);
		}
	}
	else
		check_small_window();
	disp_file_box();
	gbl(scr_cur_no) = save_sn;
	gbl(scr_cur) = save_cd;
}

void copy_dstat (DSTAT *d1, DSTAT *d2)
{
	d2->save_cmd_mode       = d1->save_cmd_mode;
	d2->save_file_fmt       = d1->save_file_fmt;
	d2->vfcb                = 0;

	strcpy(d2->file_spec, d1->file_spec);
	strcpy(d2->path_name, d1->path_name);

	d2->cur_root			= d1->cur_root;
	d2->cur_dir_tree		= d1->cur_dir_tree;
	d2->cur_dir				= d1->cur_dir;
	d2->cur_file			= d1->cur_file;
	d2->av_file_displayed	= 0;					/* NOTE!! <<<---- */

	d2->dir_total_count		= d1->dir_total_count;;
	d2->dir_total_bytes		= d1->dir_total_bytes;
	d2->dir_match_count		= d1->dir_match_count;
	d2->dir_match_bytes		= d1->dir_match_bytes;
	d2->dir_tagged_count	= d1->dir_tagged_count;
	d2->dir_tagged_bytes	= d1->dir_tagged_bytes;
	d2->dir_tagged_blocks	= d1->dir_tagged_blocks;

	d2->global_flist		= d1->global_flist;
	d2->global_mlist		= d1->global_mlist;
	d2->cur_node			= d1->cur_node;
	d2->first_dir			= d1->first_dir;
	d2->top_dir				= d1->top_dir;
	d2->base_file			= d1->base_file;
	d2->dir_cursor			= d1->dir_cursor;
	d2->first_file			= d1->first_file;
	d2->top_file			= d1->top_file;
	d2->file_cursor			= d1->file_cursor;

	d2->dir_scroll_bar_displayed = FALSE;
	d2->file_scroll_bar_displayed = FALSE;

	d2->dir_scroll_bar		= d1->dir_scroll_bar;
	d2->file_scroll_bar		= d1->file_scroll_bar;
	d2->file_spec_pres		= d1->file_spec_pres;
	d2->invert_file_spec	= d1->invert_file_spec;
	d2->in_dest_dir			= d1->in_dest_dir;
	d2->dir_scroll			= d1->dir_scroll;
	d2->dir_fmt				= d1->dir_fmt;
	d2->file_fmt			= d1->file_fmt;
	d2->in_small_window		= d1->in_small_window;
	d2->numdirs				= d1->numdirs;
	d2->max_dir_line		= d1->max_dir_line;
	d2->max_dir_level		= d1->max_dir_level;
	d2->cur_dir_no			= d1->cur_dir_no;
	d2->cur_dir_line		= d1->cur_dir_line;
	d2->numfiles			= d1->numfiles;
	d2->max_file_line		= d1->max_file_line;
	d2->max_file_col		= d1->max_file_col;
	d2->cur_file_line		= d1->cur_file_line;
	d2->cur_file_col		= d1->cur_file_col;
	d2->cur_file_no			= d1->cur_file_no;
	d2->file_line_width		= d1->file_line_width;
	d2->file_disp_len		= d1->file_disp_len;
	d2->dir_disp_len		= d1->dir_disp_len;
	d2->tag_mask            = 0;
	d2->command_mode		= d1->command_mode;
	d2->cmd_sub_mode		= d1->cmd_sub_mode;
}

static void toggle_to_screen (int n)
{
	if (gbl(scr_in_autoview))
	{
		if (gbl(scr_stat)[n].numfiles)
		{
			gbl(scr_cur_no) = n;
			gbl(scr_cur) = &gbl(scr_stat)[gbl(scr_cur_no)];
			if (! gbl(scr_av_focus))
				hilite_file(ON);
			disp_cmds();
		}
		return;
	}

	gbl(scr_cur_no) = n;
	gbl(scr_cur) = &gbl(scr_stat)[gbl(scr_cur_no)];

	/* figure out if display is kosher or not */

	is_screen_kosher(n);

	if (gbl(scr_in_autoview))
	{
	}
	else if (gbl(scr_cur)->command_mode == m_dir)
	{
		dptr_to_dirname(gbl(scr_cur)->dir_cursor, gbl(scr_cur)->path_name);
		disp_path_line();
		disp_dir_tree();
		hilite_dir(ON);
		check_small_window();
		disp_cmds();
	}
	else
	{
		if (gbl(scr_cur)->in_small_window)
		{
			disp_dir_tree();
			hilite_dir(OFF);
			left_arrow(gbl(scr_cur)->cur_dir_win);
			wrefresh(gbl(scr_cur)->cur_dir_win);
		}
		disp_file_list();
		do_file_scroll_bar();
		hilite_file(ON);
		waddch(gbl(scr_cur)->cur_file_win, ' ');
		wrefresh(gbl(scr_cur)->cur_file_win);
		disp_cmds();
	}
}

void toggle_split (int n)
{
	if (gbl(scr_is_split))
	{
		/* turn off hilites in old window */

		if (gbl(scr_cur)->command_mode == m_dir)
		{
			hilite_dir(OFF);
			left_arrow(gbl(scr_cur)->cur_dir_win);
			wrefresh(gbl(scr_cur)->cur_dir_win);
		}
		else
		{
			hilite_file(OFF);
			waddch(gbl(scr_cur)->cur_file_win, pgm_const(selected_char));
			wrefresh(gbl(scr_cur)->cur_file_win);
		}

		/* bump to next window if necessary */

		if (gbl(scr_in_autoview))
		{
			if (n == -1)
			{
				int i;

				n = gbl(scr_cur_no);
				for (i=0; i<gbl(scr_cur_count); i++)
				{
					n = next_scr_no(n);
					if (gbl(scr_stat)[n].numfiles)
						break;
				}
			}
			else
			{
				if (!gbl(scr_stat)[n].numfiles)
					n = gbl(scr_cur_no);
			}
		}
		else
		{
			if (n == -1)
			{
				n = next_scr_no(gbl(scr_cur_no));
			}
		}
		toggle_to_screen(n);
	}
}

void is_screen_kosher (int n)
{
	DSTAT *cp;
	int i;

	cp = &gbl(scr_stat)[n];

	/*
	 * Does the node still exist?
	 */

	cp->cur_node = bfind(gbl(nodes_list), cp->cur_root);
	if (cp->cur_node == 0)
	{
		cp->cur_node = gbl(nodes_list);
		setup_node();
	}

	/*
	 * Does the directory still exist?
	 */

	if ((cp->cur_root)->dir_disp_list[gbl(scr_cur_no)])
		cp->first_dir = (cp->cur_root)->dir_disp_list[gbl(scr_cur_no)];
	else
		cp->first_dir = (cp->cur_root)->dir_list;
	cp->numdirs = bcount(cp->first_dir);
	cp->dir_cursor = bfind(cp->first_dir, cp->cur_dir_tree);
	if (!cp->dir_cursor)
	{
		setup_node();
	}
	else
	{
		cp->cur_dir_no = bindex(cp->first_dir, cp->dir_cursor);
		if (!cp->dir_scroll)
		{
			if (cp->cur_dir_no < cp->cur_dir_line)
				cp->cur_dir_line = cp->cur_dir_no;
			cp->top_dir = cp->dir_cursor;
			for (i=cp->cur_dir_line; i; i--)
				cp->top_dir = bprev(cp->top_dir);
		}
	}

	/*
	 * Are we in dir mode?
	 */

	set_first_file(n);
	if (!cp->numfiles)
		cp->command_mode = m_dir;

	/*
	 * Does the file still exist?
	 */

	if (cp->command_mode != m_dir)
	{
		cp->file_cursor = bfind(cp->first_file, cp->cur_file);
		if (!cp->file_cursor)
		{
			cp->file_cursor = cp->first_file;
			cp->top_file = cp->file_cursor;
			cp->cur_file = (FBLK *)bid(cp->file_cursor);
			cp->cur_file_no = 0;
			cp->cur_file_line = 0;
			cp->cur_file_col = 0;
		}
		else
		{
			cp->cur_file_no = bindex(cp->first_file, cp->file_cursor);
			i = ((cp->max_file_line+1) * cp->cur_file_col) + cp->cur_file_line;
			if (cp->cur_file_no < i)
			{
				cp->top_file = cp->first_file;
				cp->cur_file_line = cp->cur_file_no % (cp->max_file_line+1);
				cp->cur_file_col  = cp->cur_file_no / (cp->max_file_line+1);
			}
			else
			{
				cp->top_file = cp->file_cursor;
				for (; i; i--)
					cp->top_file = bprev(cp->top_file);
			}
		}
	}
}
