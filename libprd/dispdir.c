/*------------------------------------------------------------------------
 * display directory window
 */
#include "libprd.h"

int get_max_dir_level (WINDOW *win)
{
	return ((getmaxx(win) - OUR_MAXFN - 1) / 3) -1;
}

static char *disp_owner (BLIST *nodeptr, char *buf)
{
	TREE *t;
	DBLK *d;
	NBLK *n;
	char *s;

	t = (TREE *)bid(nodeptr);
	d = (DBLK *)tid(t);
	n = get_root_of_dir(t);

	if (n->node_type == N_FTP)
		s = get_ftp_owner(n, d->stbuf.st_uid);
	else
		s = get_owner(d->stbuf.st_uid, buf);

	return (s);
}

static char *disp_group (BLIST *nodeptr, char *buf)
{
	TREE *t;
	DBLK *d;
	NBLK *n;
	char *s;

	t = (TREE *)bid(nodeptr);
	d = (DBLK *)tid(t);
	n = get_root_of_dir(t);

	if (n->node_type == N_FTP)
		s = get_ftp_group(n, d->stbuf.st_gid);
	else
		s = get_group(d->stbuf.st_gid, buf);

	return (s);
}

static int fw_len (const char *s)
{
	int l = 0;

	for (; *s; s++)
	{
		if (iscntrl(*s))
			l++;
		l++;
	}

	return (l);
}

static int dir_type (BLIST *dptr)
{
	TREE *t;
	DBLK *d;

	t = (TREE *)bid(dptr);
	d = (DBLK *)tid(t);
	if (tleft(t) && (! bnext(dptr) || ((TREE *)bid(bnext(dptr))) != tleft(t)))
		return (pgm_const(hidden_dir_char));
	else if (d->flags & (D_NOT_LOGGED | D_CANT_LOG))
		return (pgm_const(unlogged_dir_char));
	else if (d->flags & D_CHANGED)
		return (pgm_const(changed_dir_char));

	return (' ');
}

void disp_dir_line (BLIST *nodeptr)
{
	BLIST *b;
	TREE *dt;
	TREE *t;
	int i;
	int k;
	int l;
	int n;
	DBLK *d;
	DBLK *e;
	long s;
	char *p;
	time_t tt;
	int code = CMDS_COMMON_MOUSEPOS;
	char nbuf[16];
	char dl[24];

	dt = (TREE *)bid(nodeptr);
	d  = (DBLK *)tid(dt);

	waddch(gbl(scr_cur)->cur_dir_win, dir_type(nodeptr));

	switch (gbl(scr_cur)->dir_fmt)
	{
	case fmt_dname:
		n = 0;
		if (tparent(dt))
		{
			l = tdepth(dt);
			t = dt;
			k = 0;
			for (i=gbl(scr_cur)->max_dir_level+1; i<l; i++)
			{
				t = tparent(t);
				k++;
			}
			l -= k;
			t = tparent(t);
			for (l--; l; l--)
			{
				wmove(gbl(scr_cur)->cur_dir_win,
					getcury(gbl(scr_cur)->cur_dir_win), (l * 3) - 2);
				if (tright(t) != 0)
					wbox_chr(gbl(scr_cur)->cur_dir_win, B_VE, B_SVSH);
				else
					waddch(gbl(scr_cur)->cur_dir_win, ' ');
				waddstr(gbl(scr_cur)->cur_dir_win, "  ");
				t = tparent(t);
				n += 3;
			}

			wmove(gbl(scr_cur)->cur_dir_win,
				getcury(gbl(scr_cur)->cur_dir_win), n+1);
			if (k == 0)
			{
				if (tright(dt) != 0)
					wbox_chr(gbl(scr_cur)->cur_dir_win, B_ML, B_SVSH);
				else
					wbox_chr(gbl(scr_cur)->cur_dir_win, B_BL, B_SVSH);
				wbox_chr(gbl(scr_cur)->cur_dir_win, B_HO, B_SVSH);
			}
			else
			{
				waddstr(gbl(scr_cur)->cur_dir_win, "..");
			}
			wbox_chr(gbl(scr_cur)->cur_dir_win, B_HO, B_SVSH);
			n += 3;
		}

		k = fw_len(FULLNAME(d)) + 1;
		if (k > (gbl(scr_cur)->dir_disp_len - n))
			k = (gbl(scr_cur)->dir_disp_len - n);

		disp_fw_str_m(gbl(scr_cur)->cur_dir_win, FULLNAME(d),
			k, code, FALSE);
		break;

	case fmt_dsize:
		disp_fw_str_m(gbl(scr_cur)->cur_dir_win, FULLNAME(d),
			gbl(scr_cur)->dir_disp_len, code, TRUE);
		wcaddstr(gbl(scr_cur)->cur_dir_win, xform2(nbuf, tdepth(dt), 2), code);
		wcaddch (gbl(scr_cur)->cur_dir_win, ' ', code);
		if (gbl(scr_is_split))
		{
			wcaddstr(gbl(scr_cur)->cur_dir_win,
				xform(nbuf, bcount(d->flist))+5, code);
			wcaddch (gbl(scr_cur)->cur_dir_win, ' ', code);
			wcaddstr(gbl(scr_cur)->cur_dir_win,
				xform(nbuf, d->dir_size), code);
		}
		else
		{
			wcaddstr(gbl(scr_cur)->cur_dir_win,
				xform(nbuf, bcount(d->flist))+5, code);
			wcaddch (gbl(scr_cur)->cur_dir_win, ' ', code);
			wcaddstr(gbl(scr_cur)->cur_dir_win,
				xform(nbuf, d->dir_size), code);
			wcaddch (gbl(scr_cur)->cur_dir_win, ' ', code);
			s = d->dir_size;
			l = tdepth(dt);
			if ((gbl(scr_cur)->cur_root)->dir_disp_list[gbl(scr_cur_no)])
				b = bfind((gbl(scr_cur)->cur_root)->dir_list, dt);
			else
				b = nodeptr;
			for (b=bnext(b); b; b=bnext(b))
			{
				t = (TREE *)bid(b);
				if (tdepth(t) <= l)
					break;
				e = (DBLK *)tid(t);
				s += e->dir_size;
			}
			wcaddstr(gbl(scr_cur)->cur_dir_win, xform(nbuf, s), code);
			wcaddch (gbl(scr_cur)->cur_dir_win, ' ', code);
		}
		break;

	case fmt_ddate:
		switch (opt(date_type))
		{
		case date_modified:
			tt = d->stbuf.st_mtime;
			break;
		case date_accessed:
			tt = d->stbuf.st_atime;
			break;
		case date_created:
			tt = d->stbuf.st_ctime;
			break;
		}

		disp_fw_str_m(gbl(scr_cur)->cur_dir_win, FULLNAME(d),
			gbl(scr_cur)->dir_disp_len, code, TRUE);
		wcaddstr(gbl(scr_cur)->cur_dir_win, xform2(nbuf, tdepth(dt), 2), code);
		wcaddch (gbl(scr_cur)->cur_dir_win, ' ', code);
		if (gbl(scr_is_split))
		{
			wcaddstr(gbl(scr_cur)->cur_dir_win,
				xform(nbuf, d->stbuf.st_size)+2, code);
			wcaddch (gbl(scr_cur)->cur_dir_win, ' ', code);
			p = date_to_str(dl, tt);
			p[8] = 0;
			wcaddstr(gbl(scr_cur)->cur_dir_win, p, code);
		}
		else
		{
			wcaddstr(gbl(scr_cur)->cur_dir_win,
				xform(nbuf, d->stbuf.st_size)+2, code);
			wcaddch (gbl(scr_cur)->cur_dir_win, ' ', code);
			wcaddstr(gbl(scr_cur)->cur_dir_win, date_to_str(dl, tt), code);
			wcaddch (gbl(scr_cur)->cur_dir_win, ' ', code);
		}
		break;

	case fmt_dperms:
		disp_fw_str_m(gbl(scr_cur)->cur_dir_win, FULLNAME(d),
			gbl(scr_cur)->dir_disp_len, code, TRUE);
		wcaddstr(gbl(scr_cur)->cur_dir_win, xform2(nbuf, tdepth(dt), 2), code);
		wcaddch(gbl(scr_cur)->cur_dir_win, ' ', code);
		if (gbl(scr_is_split))
		{
			wcaddstr(gbl(scr_cur)->cur_dir_win,
				perm_str(d->stbuf.st_mode, nbuf), code);
			wcaddch (gbl(scr_cur)->cur_dir_win, ' ', code);
			wcaddstr(gbl(scr_cur)->cur_dir_win, disp_owner(nodeptr, nbuf),
				code);
		}
		else
		{
			wcaddstr(gbl(scr_cur)->cur_dir_win,
				xform2(nbuf, d->stbuf.st_ino, 5), code);
			wcaddch (gbl(scr_cur)->cur_dir_win, ' ', code);
			wcaddstr(gbl(scr_cur)->cur_dir_win,
				perm_str(d->stbuf.st_mode, nbuf), code);
			wcaddch (gbl(scr_cur)->cur_dir_win, ' ', code);
			wcaddstr(gbl(scr_cur)->cur_dir_win, disp_owner(nodeptr, nbuf),
				code);
			wcaddch (gbl(scr_cur)->cur_dir_win, ' ', code);
			wcaddstr(gbl(scr_cur)->cur_dir_win, disp_group(nodeptr, nbuf),
				code);
			wcaddch (gbl(scr_cur)->cur_dir_win, ' ', code);
		}
		break;
	}
}

static void disp_dir_tree_no_refresh (void)
{
	BLIST *b;
	int l;

	werase(gbl(scr_cur)->cur_dir_win);
	if (gbl(scr_cur)->dir_scroll)
	{
		if (gbl(scr_cur)->cur_dir_no < gbl(scr_cur)->cur_dir_line)
		{
			b = gbl(scr_cur)->first_dir;
			l = gbl(scr_cur)->cur_dir_line-gbl(scr_cur)->cur_dir_no;
		}
		else
		{
			for (b=gbl(scr_cur)->dir_cursor, l=gbl(scr_cur)->cur_dir_line;
				l; l--)
			{
				b = bprev(b);
			}
		}
	}
	else
	{
		b = gbl(scr_cur)->top_dir;
		l = 0;
	}
	for (; l<=gbl(scr_cur)->max_dir_line; l++)
	{
		wmove(gbl(scr_cur)->cur_dir_win, l, 0);
		disp_dir_line(b);
		b = bnext(b);
		if (!b)
			break;
	}
}

void disp_dir_tree (void)
{
	disp_dir_tree_no_refresh();
	wrefresh(gbl(scr_cur)->cur_dir_win);
}

void hilite_dir (int on_off)
{
	hilite_the_dir(on_off, gbl(scr_cur)->dir_cursor,
		gbl(scr_cur)->cur_dir_line);
}

void hilite_the_dir (int on_off, BLIST *dptr, int line)
{
	int l;
	TREE *t;
	DBLK *d;
	int code = CMDS_COMMON_MOUSEPOS;

	t = (TREE *)bid(dptr);
	d = (DBLK *)tid(t);
	l = tdepth(t);
	switch (gbl(scr_cur)->dir_fmt)
	{
	case fmt_dname:
		if (l > gbl(scr_cur)->max_dir_level)
			l = gbl(scr_cur)->max_dir_level+1;
		wmove(gbl(scr_cur)->cur_dir_win, line, 0);
		waddch(gbl(scr_cur)->cur_dir_win, dir_type(dptr));
		wmove(gbl(scr_cur)->cur_dir_win, line, l*3);
		wclrtoeol(gbl(scr_cur)->cur_dir_win);
		if (on_off)
			wstandout(gbl(scr_cur)->cur_dir_win);
		if (l)
			wbox_chr(gbl(scr_cur)->cur_dir_win, B_HO, B_SVSH);
		else
			waddch(gbl(scr_cur)->cur_dir_win, dir_type(dptr));

		{
			int i;
			int n;

			n = fw_len(FULLNAME(d));
			i = getmaxx(gbl(scr_cur)->cur_dir_win) - (l+1)*3;
			if (n < i)
				i = n;
			i++;
			disp_fw_str_m(gbl(scr_cur)->cur_dir_win, FULLNAME(d),
				i, code, FALSE);
		}
		if (on_off)
			wstandend(gbl(scr_cur)->cur_dir_win);
		break;

	case fmt_dsize:
	case fmt_ddate:
	case fmt_dperms:
		wmove(gbl(scr_cur)->cur_dir_win, line, 0);
		wclrtoeol(gbl(scr_cur)->cur_dir_win);
		if (on_off)
			wstandout(gbl(scr_cur)->cur_dir_win);
		disp_dir_line(dptr);
		if (on_off)
			wstandend(gbl(scr_cur)->cur_dir_win);
		break;
	}
	wrefresh(gbl(scr_cur)->cur_dir_win);
}

void dir_scroll_bar_init (void)
{
	scrollbar(gbl(win_border), gbl(scr_cur)->cur_dir_win, SCRLBAR_VL, -1, 0);
	gbl(scr_cur)->dir_scroll_bar_displayed = TRUE;
	gbl(scr_cur)->dir_scroll_bar = -1;
}

void dir_scroll_bar_remove (void)
{
	int i;
	int y;
	int x;

	y = getbegy(gbl(scr_cur)->cur_dir_win) - getbegy(gbl(win_border));
	x = getbegx(gbl(scr_cur)->cur_dir_win) - getbegx(gbl(win_border));
	for (i=0; i<getmaxy(gbl(scr_cur)->cur_dir_win); i++)
	{
		wmove(gbl(win_border), y+i, x-1);
		wbox_chr(gbl(win_border), B_VE, pgm_const(border_type));
	}
	wrefresh(gbl(win_border));
	gbl(scr_cur)->dir_scroll_bar = -1;
	gbl(scr_cur)->dir_scroll_bar_displayed = FALSE;
}

void dir_scroll_bar_off (void)
{
	if (gbl(scr_cur)->dir_scroll_bar >= 0)
	{
		scrollbar(gbl(win_border), gbl(scr_cur)->cur_dir_win, SCRLBAR_VL, -1,
			gbl(scr_cur)->numdirs);
		gbl(scr_cur)->dir_scroll_bar = -1;
	}
}

void do_dir_scroll_bar (void)
{
	if (!gbl(scr_cur)->dir_scroll_bar_displayed)
		dir_scroll_bar_init();

	if (gbl(scr_cur)->cur_dir_no != gbl(scr_cur)->dir_scroll_bar)
	{
		scrollbar(gbl(win_border), gbl(scr_cur)->cur_dir_win, SCRLBAR_VL,
			gbl(scr_cur)->cur_dir_no, gbl(scr_cur)->numdirs);
		gbl(scr_cur)->dir_scroll_bar = gbl(scr_cur)->cur_dir_no;
		wrefresh(gbl(win_border));
	}
}
