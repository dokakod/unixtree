/*------------------------------------------------------------------------
 * process the "prune" cmd
 */
#include "libprd.h"

static int are_dirs_ok_to_prune (BLIST *first, BLIST *last)
{
	BLIST *b;
	TREE *t;
	DBLK *d;

	for (b=first; b; b=bnext(b))
	{
		t = (TREE *)bid(b);
		d = (DBLK *)tid(t);

		/* check if root of node */
		if (! bprev(b))
		{
			errmsg(ER_CDIRN, "", ERR_ANY);
			return (-1);
		}

		/* check if directory is logged */
		if (d->flags & D_NOT_LOGGED)
		{
			errmsg(ER_TDNL, "", ERR_ANY);
			return (-1);
		}

		/* check for hidden dirs */
		if (tleft(t) && (! bnext(b) || ((TREE *)bid(bnext(b))) != tleft(t)))
		{
			errmsg(ER_SCHD, "", ERR_ANY);
			return (-1);
		}

		/* check for permission */
		if (! can_we_write(&d->stbuf))
		{
			errmsg(ER_NWPF, FULLNAME(d), ERR_ANY);
			return (-1);
		}

		if (b == last)
			break;
	}

	return (0);
}

static int wipeout_dir (BLIST *b)
{
	char pathname[MAX_PATHLEN];
	TREE *t;
	TREE *p;
	DBLK *d;
	NBLK *n;
	int i;
	BLIST *bt;

	n = gbl(scr_cur)->cur_root;
	t = (TREE *)bid(b);
	d = (DBLK *)tid(t);
	b = bfind(n->dir_list, t);
	wmove(gbl(win_commands), 0, 0);
	wclrtoeol(gbl(win_commands));
	xaddstr(gbl(win_commands), msgs(m_prune_pd));
	zaddstr(gbl(win_commands), FULLNAME(d));
	wrefresh(gbl(win_commands));
	if (clean_dir(t, n, TRUE))
		return (1);
	dirtree_to_dirname(t, pathname);
	if (os_dir_delete(pathname))
		return (1);
	n->dir_list = BDELETE(n->dir_list, b, FALSE);
	for (i=0; i<gbl(scr_cur_count); i++)
	{
		if (n->dir_disp_list[i])
		{
			bt = bfind(n->dir_disp_list[i], t);
			if (bt)
				n->dir_disp_list[i] = BDELETE(n->dir_disp_list[i], bt, FALSE);
		}
	}
	dblk_free(d);
	p = tparent(t);
	tleft(p) = TDELETER(tleft(p), t, FALSE);
	return (0);
}

static void prune_node (BLIST *first, BLIST *last)
{
	BLIST *b;
	BLIST *x;

	for (b=last; ; b=x)
	{
		if (xchk_kbd() == KEY_ESCAPE)
			break;
		x = bprev(b);
		if (wipeout_dir(b))
			break;
		if (b == first)
			break;
	}
}

static void prune_first_to_last (BLIST *first, BLIST *last)
{
	if ((TREE *)bid(first) == gbl(scr_cur)->cur_dir_tree)
	{
		gbl(scr_cur)->dir_cursor = bprev(gbl(scr_cur)->dir_cursor);
		gbl(scr_cur)->cur_dir_tree = (TREE *)bid(gbl(scr_cur)->dir_cursor);
		gbl(scr_cur)->cur_dir = (DBLK *)tid(gbl(scr_cur)->cur_dir_tree);
		gbl(scr_cur)->cur_dir_no--;
		if (gbl(scr_cur)->cur_dir_line)
			gbl(scr_cur)->cur_dir_line--;
	}
	prune_node(first, last);
}

void prune (void)
{
	int cd_is_hilited;
	BLIST *first_dir;
	BLIST *last_dir;
	BLIST *b;
	NBLK *n;
	int i;
	int doing_cd;

	if (get_first_last(&first_dir, &last_dir, FALSE))
		return;
	cd_is_hilited = (first_dir != last_dir);

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_prune_pn));
	zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_dir));
	wrefresh(gbl(win_commands));
	if (yesno_msg(msgs(m_prune_ptn)))
		goto bail;
	if (yesno_msg(msgs(m_prune_air)))
		goto bail;

	doing_cd = ((TREE *)bid(first_dir) == gbl(scr_cur)->cur_dir_tree);
	prune_first_to_last(first_dir, last_dir);

	if (!doing_cd)
	{
		n = gbl(scr_cur)->cur_root;
		if (n->dir_disp_list[gbl(scr_cur_no)])
		{
			first_dir = last_dir =
				bfind(n->dir_list, gbl(scr_cur)->cur_dir_tree);
		}
		else
		{
			first_dir = last_dir = gbl(scr_cur)->dir_cursor;
		}

		set_top_dir();
		disp_dir_tree();
		do_dir_scroll_bar();
		hilite_dir(ON);
		werase(gbl(win_commands));
		wmove(gbl(win_commands), 0, 0);
		xaddstr(gbl(win_commands), msgs(m_prune_pn));
		zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_dir));
		wrefresh(gbl(win_commands));
		if (!yesno_msg(msgs(m_prune_ptn)))
		{
			if (!are_dirs_ok_to_prune(first_dir, last_dir))
				prune_first_to_last(first_dir, last_dir);
		}
	}

	set_top_dir();
	dptr_to_dirname(gbl(scr_cur)->dir_cursor, gbl(scr_cur)->path_name);
	disp_path_line();
	disp_dir_tree();
	do_dir_scroll_bar();
	hilite_dir(ON);
	check_small_window();
	disp_drive_info((gbl(scr_cur)->cur_dir)->stbuf.st_dev, 1);
	disp_node_stats(gbl(scr_cur)->cur_root);
	disp_cur_dir();
	return;

bail:
	if (!cd_is_hilited)
	{
		hilite_dir(ON);
		i = gbl(scr_cur)->cur_dir_line;
		for (b=first_dir; b; b=bnext(b))
		{
			if (++i <= gbl(scr_cur)->max_dir_line)
				hilite_the_dir(OFF, b, i);
			if (b == last_dir)
				break;
		}
	}
}

int get_first_last (BLIST **first, BLIST **last, int inc_first)
{
	BLIST *f_dir;
	BLIST *l_dir;
	BLIST *b;
	BLIST *x;
	BLIST *dc;
	NBLK *n;
	TREE *t;
	int l;
	int i;

	*first = *last = 0;

	n = gbl(scr_cur)->cur_root;
	if (n->dir_disp_list[gbl(scr_cur_no)])
		dc = bfind(n->dir_list, gbl(scr_cur)->cur_dir_tree);
	else
		dc = gbl(scr_cur)->dir_cursor;

	l = tdepth(gbl(scr_cur)->cur_dir_tree);
	x = bnext(dc);
	t = tleft(gbl(scr_cur)->cur_dir_tree);

	/* check if at end of list & sub-dirs exist */

	if (t && !x)
	{
		errmsg(ER_SCHD, "", ERR_ANY);
		return (-1);
	}

	/* check if first sub-dir exists & is not next in list */

	if (t && x && t != (TREE *)bid(x))
	{
		errmsg(ER_SCHD, "", ERR_ANY);
		return (-1);
	}

	if (x && tdepth((TREE *)bid(x)) > l)
	{
		/*
		 * If directory has sub-dirs, find first & last dir in sub-node
		 */

		if (inc_first)
			f_dir = dc;
		else
			f_dir = x;
		l_dir = 0;
		for (b=f_dir; b; b=bnext(b))
		{
			t = (TREE *)bid(b);
			/* check if at next dir at same level as cur_dir */
			if (!inc_first || b != f_dir)
			{
				if (tdepth(t) <= l)
					break;
			}
			l_dir = b;
		}
		if (!l_dir)
		{
			return (-1);
		}
		if (are_dirs_ok_to_prune(f_dir, l_dir))
			return (-1);
		if (!inc_first)
		{
			hilite_dir(OFF);
			left_arrow(gbl(scr_cur)->cur_dir_win);
			wrefresh(gbl(scr_cur)->cur_dir_win);
		}
		i = gbl(scr_cur)->cur_dir_line;
		if (!inc_first)
			i++;
		for (b=f_dir; b; b=bnext(b))
		{
			if (i <= gbl(scr_cur)->max_dir_line)
				hilite_the_dir(ON, b, i);
			i++;
			if (b == l_dir)
				break;
		}
	}
	else
	{
		/*
		 * Dir has no sub-dirs.  Set first & last dir to current dir
		 */

		f_dir = l_dir = dc;
		if (are_dirs_ok_to_prune(f_dir, l_dir))
			return (-1);
	}

	*first = f_dir;
	*last  = l_dir;

	return (0);
}

int clean_dir (TREE *t, NBLK *n, int del_flag)
{
	char pathname[MAX_PATHLEN];
	BLIST *b;
	BLIST *x;
	DBLK *d;
	FBLK *f;
	FBLK *g;
	BLIST *l;
	BLIST *lt;
	char *p = 0;
	int i;

	if (del_flag)
	{
		dirtree_to_dirname(t, pathname);
		fn_terminate_dirname(pathname);
		p = pathname+strlen(pathname);
	}

	d = (DBLK *)tid(t);
	for (b=d->flist; b; b=bnext(b))
	{
		f = (FBLK *)bid(b);

		if (del_flag)
		{
			strcpy(p, FULLNAME(f));
			if (os_file_delete(pathname))
			{
				i = errsys(ER_CU);
				return (i);
			}
		}

		if (f->stbuf.st_nlink > 1)
		{
			for (l=n->link_list; l; l=lt)
			{
				lt = bnext(l);
				g = (FBLK *)bid(l);
				if (g != f)
				{
					if (del_flag &&
					    f->stbuf.st_dev == g->stbuf.st_dev &&
				    	f->stbuf.st_ino == g->stbuf.st_ino)
						g->stbuf.st_nlink--;
				}
				else
				{
					n->link_list = BDELETE(n->link_list, l, FALSE);
				}
			}
		}

		n->node_total_count--;

		if (!check_for_dup_inode(n->showall_flist, f, 0, 0))
			n->node_total_bytes -= f->stbuf.st_size;

		if (gbl(scr_cur)->file_spec_pres)
		{
			if (match(FULLNAME(f), gbl(scr_cur)->file_spec))
			{
				n->node_match_count--;
				if (! check_for_dup_inode(n->showall_mlist[gbl(scr_cur_no)],
					f, 0,0))
				{
					n->node_match_bytes -= f->stbuf.st_size;
				}
			}
		}
		else
		{
			n->node_match_count = n->node_total_count;
			n->node_match_bytes = n->node_total_bytes;
		}

		if (is_file_tagged(f))
		{
			n->node_tagged_count--;
			if (!check_for_dup_inode(n->showall_flist, f, 0, 1))
			{
				n->node_tagged_bytes  -= f->stbuf.st_size;
				n->node_tagged_blocks -=
					file_size_in_blks(f, f->stbuf.st_size);
			}
		}

		x = bfind(n->showall_flist, f);
		n->showall_flist = BDELETE(n->showall_flist, x, FALSE);

		for (i=0; i<gbl(scr_cur_count); i++)
		{
			if (gbl(scr_stat)[i].file_spec_pres)
			{
				x = bfind(n->showall_mlist[i], f);
				n->showall_mlist[i] = BDELETE(n->showall_mlist[i], x, FALSE);
			}
			else
			{
				n->showall_mlist[i] = n->showall_flist;
			}
		}
		fblk_free(f);
	}

	d->flist = BSCRAP(d->flist, FALSE);

	for (i=0; i<gbl(scr_cur_count); i++)
	{
		if (gbl(scr_stat)[i].file_spec_pres)
			d->mlist[i] = BSCRAP(d->mlist[i], FALSE);
		else
			d->mlist[i] = 0;
	}

	return (0);
}
