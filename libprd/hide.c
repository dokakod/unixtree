/*------------------------------------------------------------------------
 * process the "hide" & "show" cmds
 */
#include "libprd.h"

static void set_dir_level (void)
{
	NBLK *n;
	int i;

	n = gbl(scr_cur)->cur_root;
	hide_dirs_in_node(n, gbl(scr_cur_no));
	if (! n->dir_disp_list[gbl(scr_cur_no)])
	{
		gbl(scr_cur)->first_dir = (gbl(scr_cur)->cur_root)->dir_list;
	}
	else
	{
		gbl(scr_cur)->first_dir =
			(gbl(scr_cur)->cur_root)->dir_disp_list[gbl(scr_cur_no)];
	}
	gbl(scr_cur)->dir_cursor =
		bfind(gbl(scr_cur)->first_dir, gbl(scr_cur)->cur_dir_tree);
	gbl(scr_cur)->cur_dir_no =
		bindex(gbl(scr_cur)->first_dir, gbl(scr_cur)->dir_cursor);

	if (!gbl(scr_cur)->dir_scroll)
	{
		if (gbl(scr_cur)->cur_dir_no < gbl(scr_cur)->cur_dir_line)
			gbl(scr_cur)->cur_dir_line = gbl(scr_cur)->cur_dir_no;

		for (gbl(scr_cur)->top_dir=gbl(scr_cur)->dir_cursor,
			i=gbl(scr_cur)->cur_dir_line;
			i; i--)
		{
			gbl(scr_cur)->top_dir = bprev(gbl(scr_cur)->top_dir);
		}
	}
	gbl(scr_cur)->numdirs = bcount(gbl(scr_cur)->first_dir);
}

void hide_sub_node (int onoff)
{
	BLIST *b;
	TREE *t;
	DBLK *d;
	int l;

	l = tdepth(gbl(scr_cur)->cur_dir_tree);
	b = bfind((gbl(scr_cur)->cur_root)->dir_list, gbl(scr_cur)->cur_dir_tree);
	for (b=bnext(b); b; b=bnext(b))
	{
		t = (TREE *)bid(b);
		if (tdepth(t) <= l)
			break;
		d = (DBLK *)tid(t);
		if (onoff)
			d->hidden |= gbl(scr_cur)->tag_mask;
		else
			d->hidden &= ~gbl(scr_cur)->tag_mask;
	}
	set_dir_level();
	disp_dir_tree();
	hilite_dir(ON);

	if (! gbl(scr_cur)->in_dest_dir)
	{
		disp_path_line();
		check_small_window();
	}
}

void hide_entire_node (int onoff)
{
	BLIST *b;
	TREE *t;
	DBLK *d;
	int l;

	l = tdepth(gbl(scr_cur)->cur_dir_tree);
	for (b=(gbl(scr_cur)->cur_root)->dir_list; b; b=bnext(b))
	{
		t = (TREE *)bid(b);
		if (tdepth(t) <= l)
			continue;
		d = (DBLK *)tid(t);
		if (onoff)
			d->hidden |= gbl(scr_cur)->tag_mask;
		else
			d->hidden &= ~gbl(scr_cur)->tag_mask;
	}
	set_dir_level();
	disp_path_line();
	disp_dir_tree();
	hilite_dir(ON);
	check_small_window();
}

void hide_dirs_in_node (NBLK *n, int s)
{
	BLIST *b;
	BLIST *y;
	TREE *t;
	DBLK *d;
	int hidden_dirs_found;
	int m;

	/* scrap previous list */

	n->dir_disp_list[s] = BSCRAP(n->dir_disp_list[s], FALSE);

	/* check if there actually are any hidden dirs */

	m = gbl(scr_stat)[s].tag_mask;
	hidden_dirs_found = FALSE;
	for (b=n->dir_list; b; b=bnext(b))
	{
		t = (TREE *)bid(b);
		d = (DBLK *)tid(t);
		if (d->hidden & m)
		{
			hidden_dirs_found = TRUE;
			break;
		}
	}

	if (!hidden_dirs_found)
		return;

	/* create list of non-hidden dirs */

	for (b=n->dir_list; b; b=bnext(b))
	{
		t = (TREE *)bid(b);
		d = (DBLK *)tid(t);
		if (!(d->hidden & m))
		{
			y = BNEW(t);
			if (!y)
			{
				errmsg(ER_IM, "", ERR_ANY);
				/* no memory - zap entire hidden list */
				n->dir_disp_list[s] = BSCRAP(n->dir_disp_list[s], FALSE);
				for (y=n->dir_list; y; y=bnext(y))
				{
					t = (TREE *)bid(b);
					d = (DBLK *)tid(t);
					d->hidden &= ~gbl(scr_stat)[s].tag_mask;
				}
				break;
			}
			n->dir_disp_list[s] = bappend(n->dir_disp_list[s], y);
		}
	}
}

void setup_dir_stats (void)
{
	BLIST *b;
	FBLK *f;

	gbl(scr_cur)->dir_total_count = bcount(gbl(scr_cur)->base_file);
	if (gbl(scr_cur)->command_mode == m_file)
	{
		gbl(scr_cur)->dir_total_bytes = (gbl(scr_cur)->cur_dir)->dir_size;
	}
	else
	{
		gbl(scr_cur)->dir_total_bytes =
			sum_files_in_list(gbl(scr_cur)->base_file, 0);
	}

	if (!gbl(scr_cur)->file_spec_pres)
	{
		gbl(scr_cur)->dir_match_count = gbl(scr_cur)->dir_total_count;
		gbl(scr_cur)->dir_match_bytes = gbl(scr_cur)->dir_total_bytes;
	}
	else
	{
		gbl(scr_cur)->dir_match_count = 0;
		gbl(scr_cur)->dir_match_bytes = 0;
	}
	gbl(scr_cur)->dir_tagged_count = 0;
	gbl(scr_cur)->dir_tagged_bytes = 0;
	gbl(scr_cur)->dir_tagged_blocks = 0;
	for (b=gbl(scr_cur)->first_file; b; b=bnext(b))
	{
		f = (FBLK *)bid(b);
		if (gbl(scr_cur)->file_spec_pres)
		{
			gbl(scr_cur)->dir_match_count++;
			if (!check_for_dup_inode(gbl(scr_cur)->first_file, f, 1, 0))
				gbl(scr_cur)->dir_match_bytes += f->stbuf.st_size;
		}
		if (is_file_tagged(f))
		{
			gbl(scr_cur)->dir_tagged_count++;
			if (!check_for_dup_inode(gbl(scr_cur)->first_file, f, 1, 1))
			{
				gbl(scr_cur)->dir_tagged_bytes += f->stbuf.st_size;
				gbl(scr_cur)->dir_tagged_blocks +=
					file_size_in_blks(f, f->stbuf.st_size);
			}
		}
	}
	gbl(scr_cur)->numfiles = bcount(gbl(scr_cur)->first_file);
}
