/*------------------------------------------------------------------------
 * process the "sort" cmd
 */
#include "libprd.h"

static int file_cmp (BLIST *a, BLIST *b)
{
	FBLK *fa = (FBLK *)bid(a);
	FBLK *fb = (FBLK *)bid(b);

	return compare_files(&fa, &fb);
}

static int sort_dir (DBLK *d)
{
	int i;

	d->flist = bsort(d->flist, compare_files);
	for (i=0; i<gbl(scr_cur_count); i++)
	{
		if (gbl(scr_stat)[i].file_spec_pres)
		{
			BSCRAP(d->mlist[i], FALSE);
			d->mlist[i] =
				get_mlist(d->flist, gbl(scr_stat)[i].file_spec, 0, 0);
		}
		else
		{
			d->mlist[i] = d->flist;
		}
	}

	return (0);
}

static void sort_node (NBLK *n)
{
	BLIST *b;
	DBLK *d;
	TREE *t;
	int i;

	n->showall_flist = BSCRAP(n->showall_flist, FALSE);
	for (b=n->dir_list; b; b=bnext(b))
	{
		t = (TREE *)bid(b);
		d = (DBLK *)tid(t);
		sort_dir(d);
		n->showall_flist = merge_lists(BDUP(d->flist), n->showall_flist);
	}
	for (i=0; i<gbl(scr_cur_count); i++)
	{
		if (gbl(scr_stat)[i].file_spec_pres)
		{
			BSCRAP(n->showall_mlist[i], FALSE);
			n->showall_mlist[i] = get_mlist(n->showall_flist,
				gbl(scr_stat)[i].file_spec, 0, 0);
		}
		else
		{
			n->showall_mlist[i] = n->showall_flist;
		}
	}
}

static int sort_nodes (void)
{
	BLIST *b;
	NBLK *n;

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_sort_sor));
	wrefresh(gbl(win_commands));

	for (b=gbl(nodes_list); b; b=bnext(b))
	{
		n = (NBLK *)bid(b);
		sort_node(n);
	}

	switch (gbl(scr_cur)->command_mode)
	{
	case m_showall:
	case m_tag_showall:
	case m_level_showall:
			do_showall();
			break;
	case m_global:
	case m_tag_global:
			do_global();
			break;
	}

	return (0);
}

int do_sort (void)
{
	int c;
	int old_type;
	int old_order;
	char *p;
	int lo, lp;

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	p = msgs(m_sort_sfdb);
	xaddstr(gbl(win_commands), p);
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_SORT_NAME, msgs(m_sort_name));
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_SORT_EXT, msgs(m_sort_ext));
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_SORT_DATE, msgs(m_sort_date));
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_SORT_INODE, msgs(m_sort_inode));
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_SORT_LINKS, msgs(m_sort_link));
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_SORT_SIZE, msgs(m_sort_size));
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_SORT_UNSORTED, msgs(m_sort_unsort));

	wmove(gbl(win_commands), 1, display_len(p));
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_SORT_ORDER, msgs(m_sort_ord));
	waddstr(gbl(win_commands), " ");
	lo = getcurx(gbl(win_commands));
	if (opt(sort_order) == sort_asc)
		waddstr(gbl(win_commands), msgs(m_sort_asc));
	else
		waddstr(gbl(win_commands), msgs(m_sort_des));
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_SORT_PATH, msgs(m_sort_path));
	waddstr(gbl(win_commands), " ");
	lp = getcurx(gbl(win_commands));
	if (opt(sort_by_path))
		waddstr(gbl(win_commands), msgs(m_sort_on));
	else
		waddstr(gbl(win_commands), msgs(m_sort_off));
	wrefresh(gbl(win_commands));

	werase(gbl(win_message));
	esc_msg();

	old_type  = opt(sort_type);
	old_order = opt(sort_order);
	while (TRUE)
	{
		bangnc(msgs(m_sort_entopt));
		switch (opt(sort_type))
		{
		case sort_date:
			waddch(gbl(win_message), cmds(CMDS_SORT_DATE));
			break;
		case sort_ext:
			waddch(gbl(win_message), cmds(CMDS_SORT_EXT));
			break;
		case sort_name:
			waddch(gbl(win_message), cmds(CMDS_SORT_NAME));
			break;
		case sort_inode:
			waddch(gbl(win_message), cmds(CMDS_SORT_INODE));
			break;
		case sort_links:
			waddch(gbl(win_message), cmds(CMDS_SORT_LINKS));
			break;
		case sort_size:
			waddch(gbl(win_message), cmds(CMDS_SORT_SIZE));
			break;
		case sort_unsorted:
			waddch(gbl(win_message), cmds(CMDS_SORT_UNSORTED));
			break;
		}
		waddch(gbl(win_message), '\b');
		wrefresh(gbl(win_message));

		c = xgetch(gbl(win_message));
		c = TO_LOWER(c);

		if (c == KEY_ESCAPE || c == KEY_RETURN)
			;

		else if (c == KEY_MOUSE)
			c = 0;

		else if (c == cmds(CMDS_SORT_DATE))
			opt(sort_type) = sort_date;

		else if (c == cmds(CMDS_SORT_EXT))
			opt(sort_type) = sort_ext;

		else if (c == cmds(CMDS_SORT_NAME))
			opt(sort_type) = sort_name;

		else if (c == cmds(CMDS_SORT_INODE))
			opt(sort_type) = sort_inode;

		else if (c == cmds(CMDS_SORT_LINKS))
			opt(sort_type) = sort_links;

		else if (c == cmds(CMDS_SORT_SIZE))
			opt(sort_type) = sort_size;

		else if (c == cmds(CMDS_SORT_UNSORTED))
			opt(sort_type) = sort_unsorted;

		else if (c == cmds(CMDS_SORT_ORDER))
		{
			if (opt(sort_order) == sort_asc)
			{
				wmove(gbl(win_commands), 1, lo);
				waddstr(gbl(win_commands), msgs(m_sort_des));
				wrefresh(gbl(win_commands));
				opt(sort_order) = sort_desc;
			}
			else
			{
				wmove(gbl(win_commands), 1, lo);
				waddstr(gbl(win_commands), msgs(m_sort_asc));
				wrefresh(gbl(win_commands));
				opt(sort_order) = sort_asc;
			}
			c = 0;
		}

		else if (c == cmds(CMDS_SORT_PATH))
		{
			if (opt(sort_by_path))
			{
				opt(sort_by_path) = OFF;
				wmove(gbl(win_commands), 1, lp);
				waddstr(gbl(win_commands), msgs(m_sort_off));
				wrefresh(gbl(win_commands));
			}
			else
			{
				opt(sort_by_path) = ON;
				wmove(gbl(win_commands), 1, lp);
				waddstr(gbl(win_commands), msgs(m_sort_on));
				wrefresh(gbl(win_commands));
			}
			c = 0;
		}

		else /* default */
			c = 0;

		if (c)
			break;
	}
	bang("");
	if (c == KEY_ESCAPE)
	{
		opt(sort_type)  = old_type;
		opt(sort_order) = old_order;
		return (-1);
	}

	if (opt(sort_type) != sort_unsorted)
		sort_nodes();
	set_first_file(gbl(scr_cur_no));
	return (0);
}

BLIST *put_file_in_list (FBLK *f, BLIST *file_list)
{
	BLIST *b;
	BLIST *x;

	b = BNEW(f);
	if (!b)
	{
		return (file_list);
	}

	x = file_list;
	if (x)
	{
		if (file_cmp(b, x) < 0)
		{
			bnext(b) = x;
			bprev(x) = b;
		}
		else
		{
			for (; bnext(x); x=bnext(x))
			{
				if (file_cmp(b, bnext(x)) < 0)
					break;
			}
			binsert(x, b);
			b = file_list;
		}
	}
	return (b);
}

BLIST *merge_lists (BLIST *one, BLIST *two)
{
	BLIST *three;
	BLIST *b;

	if (!one)
		return (two);
	if (!two)
		return (one);

	if (file_cmp(one, two) < 0)
	{
		three = one;
		one = bnext(one);
	}
	else
	{
		three = two;
		two = bnext(two);
	}

	b = three;
	while (TRUE)
	{
		if (!one)
		{
			bnext(b) = two;
			if (two)
				bprev(two) = b;
			break;
		}
		if (!two)
		{
			bnext(b) = one;
			if (one)
				bprev(one) = b;
			break;
		}
		if (file_cmp(one, two) < 0)
		{
			bnext(b) = one;
			bprev(one) = b;
			b = one;
			one = bnext(one);
		}
		else
		{
			bnext(b) = two;
			bprev(two) = b;
			b = two;
			two = bnext(two);
		}
	}
	return (three);
}
