/*------------------------------------------------------------------------
 * misc utilities
 */
#include "libprd.h"

TREE *pathname_to_dirtree (const char *pathname)
{
	int ntype;

	ntype = (gbl(scr_cur)->cur_root)->node_type;
	return (path_to_dt(pathname, ntype));
}

TREE *path_to_dt (const char *pathname, int ntype)
{
	BLIST *b;
	NBLK *n;

	/* find node path is in */

	for (b=gbl(nodes_list); b; b=bnext(b))
	{
		n = (NBLK *)bid(b);
		if (n->node_type != ntype)
			continue;

		if (fn_is_dir_in_dirname(n->root_name, pathname))
			break;
	}
	if (!b)
		return (0);

	return (path_to_tree(pathname, n));
}

TREE *path_to_tree (const char *path, NBLK *n)
{
	TREE *t;
	TREE *tn;
	DBLK *d;
	char *p;
	int i;

	i = fn_num_subdirs(n->root_name);
	t = n->root;
	while (TRUE)
	{
		char dir_part[MAX_FILELEN];

		/*	loop thru siblings looking for next dir portion */

		p = fn_get_nth_dir(path, i++, dir_part);
		if (!p)
			return (t);

		for (tn=tleft(t); tn; tn=tright(tn))
		{
			d = (DBLK *)tid(tn);
			if (strcmp(p, FULLNAME(d)) == 0)
				break;
		}
		if (!tn)
			break;
		t = tn;
	}
	return (0);
}

void dptr_to_dirname (BLIST *dptr, char *line)
{
	dirtree_to_dirname((TREE *)bid(dptr), line);
}

static void add_tree_to_path (TREE *t, char *line)
{
	TREE *p = tparent(t);

	/*--------------------------------------------------------------------
	 * append sub-dir if not root of node
	 */
	if (p != 0)
	{
		DBLK *d;

		/*----------------------------------------------------------------
		 * recurse up to root
		 */
		add_tree_to_path(p, line);

		/*----------------------------------------------------------------
		 * now append this name on the way down
		 */
		d = (DBLK *)tid(t);
		fn_append_dirname_to_dir(line, FULLNAME(d));
	}
}

void dirtree_to_dirname (TREE *t, char *line)
{
	NBLK *n;

	n = get_root_of_dir(t);
	strcpy(line, n->root_name);
	add_tree_to_path(t, line);
}

void dblk_to_dirname (DBLK *d, char *line)
{
	dirtree_to_dirname(d->dir_tree, line);
}

void fblk_to_dirname (FBLK *f, char *line)
{
	dblk_to_dirname(f->dir, line);
}

void fblk_to_pathname (FBLK *f, char *path)
{
	fblk_to_dirname(f, path);
	fn_append_filename_to_dir(path, FULLNAME(f));
}

NBLK *get_root_of_file (FBLK *fptr)
{
	NBLK *n;

	switch (gbl(scr_cur)->command_mode)
	{
	case m_dir:
	case m_file:
	case m_showall:
	case m_tag_showall:
	case m_level_showall:
			n = gbl(scr_cur)->cur_root;
			break;
	case m_global:
	case m_tag_global:
			n = get_root_of_dir(fptr->dir->dir_tree);
			break;
	default:
			n = 0;
			break;
	}
	return (n);
}

NBLK *get_root_of_dir (TREE *t)
{
	TREE *p = troot(t);
	BLIST *b;

	for (b=gbl(nodes_list); b; b=bnext(b))
	{
		NBLK *n = (NBLK *)bid(b);

		if (n->root == p)
			return (n);
	}

	return (gbl(scr_cur)->cur_root);
}

int fblk_fs_type (FBLK *f)
{
	NBLK *n = get_root_of_file(f);

	if (n == 0)
		return (-1);

	return (n->node_type);
}

int dblk_fs_type (DBLK *d)
{
	NBLK *n = get_root_of_dir(d->dir_tree);

	if (n == 0)
		return (-1);

	return (n->node_type);
}

int is_file_displayed (FBLK *f, int *frow, int *fcol)
{
	BLIST *b;
	int trow	= 0;
	int tcol	= 0;

	for (b=gbl(scr_cur)->top_file; b; b=bnext(b))
	{
		if (f == (FBLK *)bid(b))
		{
			*frow = trow;
			*fcol = tcol;
			return (TRUE);
		}

		if (++trow > gbl(scr_cur)->max_file_line)
		{
			trow = 0;
			if (++tcol > gbl(scr_cur)->max_file_col)
				return (FALSE);
		}
	}

	return (FALSE);
}

void redisplay_file (FBLK *f, int frow, int fcol)
{
	wmove(gbl(scr_cur)->cur_file_win,
		frow, fcol * gbl(scr_cur)->file_line_width);
	disp_file_line(f);
	wrefresh(gbl(scr_cur)->cur_file_win);
}

static int is_dir_displayed (TREE *t, int *dlinep, BLIST **dptrp)
{
	int dline;
	BLIST *dptr;

	if (!(gbl(scr_cur)->command_mode == m_dir ||
	    (gbl(scr_cur)->command_mode == m_file &&
		gbl(scr_cur)->in_small_window)))
	{
		return (0);
	}

	if (gbl(scr_cur)->dir_fmt == fmt_dname ||
		gbl(scr_cur)->dir_fmt == fmt_dsize)
	{
		return (0);
	}

	if (t == (gbl(scr_cur)->cur_dir_tree))
	{
		*dptrp  = gbl(scr_cur)->dir_cursor;
		*dlinep = gbl(scr_cur)->cur_dir_line;
		return (1);
	}

	if (gbl(scr_cur)->dir_scroll)
	{
		if (gbl(scr_cur)->cur_dir_no < gbl(scr_cur)->cur_dir_line)
		{
			dptr  = gbl(scr_cur)->first_dir;
			dline = gbl(scr_cur)->cur_dir_line-gbl(scr_cur)->cur_dir_no;
		}
		else
		{
			for (dptr=gbl(scr_cur)->dir_cursor,
				dline=gbl(scr_cur)->cur_dir_line; dline;
				dline--)
			{
				dptr = bprev(dptr);
			}
		}
	}
	else
	{
		dptr  = gbl(scr_cur)->top_dir;
		dline = 0;
	}

	for (; dline<=gbl(scr_cur)->max_dir_line; dline++)
	{
		if (t == (TREE *)bid(dptr))
		{
			*dlinep = dline;
			*dptrp  = dptr;
			return (1);
		}
		dptr = bnext(dptr);
	}

	return (0);
}

static void redisplay_dir (TREE *t)
{
	int dline;
	BLIST *dptr;

	if (is_dir_displayed(t, &dline, &dptr))
	{
		wmove(gbl(scr_cur)->cur_dir_win, dline, 0);
		disp_dir_line(dptr);
		wrefresh(gbl(scr_cur)->cur_dir_win);
	}
}

void update_dir (DBLK *d)
{
	struct stat stbuf;
	char fullname[MAX_PATHLEN];

	dblk_to_dirname(d, fullname);
	if (os_stat(fullname, &stbuf) == 0)
	{
		d->stbuf.st_size  = stbuf.st_size;
		d->stbuf.st_mtime = stbuf.st_mtime;
		d->stbuf.st_atime = stbuf.st_atime;
		d->stbuf.st_ctime = stbuf.st_ctime;
	}

	redisplay_dir(d->dir_tree);
}

long sum_files_in_list (BLIST *file_list, int check_tag)
{
	BLIST *b;
	FBLK *f;
	FBLK *g;
	long sum;
	BLIST *links;
	BLIST *l;
	BLIST *k;

	sum = 0;
	links = 0;
	for (b=file_list; b; b=bnext(b))
	{
		f = (FBLK *)bid(b);
		if (check_tag && !is_file_tagged(f))
			continue;
		if (f->stbuf.st_nlink > 1)
		{
			for (l=links; l; l=bnext(l))
			{
				g = (FBLK *)bid(l);
				if (g->stbuf.st_dev == f->stbuf.st_dev &&
				    g->stbuf.st_ino == f->stbuf.st_ino)
					break;
			}
			k = BNEW(f);
			if (k)
			{
				links = bappend(links, k);
			}
			if (l)
				continue;
		}
		sum += f->stbuf.st_size;
	}
	links = BSCRAP(links, FALSE);
	return (sum);
}

int check_for_dup_inode (BLIST *file_list, FBLK *file, int mode,
	int check_tag)
{
	BLIST *b;
	FBLK *f;
	dev_t dev;
	ino_t ino;

	if (file->stbuf.st_nlink == 1)
		return (FALSE);
	dev = file->stbuf.st_dev;
	ino = file->stbuf.st_ino;
	for (b=file_list; b; b=bnext(b))
	{
		f = (FBLK *)bid(b);
		if (f == file)
		{
			if (mode)
				return (FALSE);
			else
				continue;
		}
		if (f->stbuf.st_nlink == 1)
			continue;
		if (check_tag && !is_file_tagged(f))
			continue;
		if (f->stbuf.st_dev == dev && f->stbuf.st_ino == ino)
			return (TRUE);
	}
	return (FALSE);
}

long file_size_in_blks (FBLK *f, long size)
{
	return ( ((size + 1023) / 1024) * 1024 );
}

/*  this routine is designed to be called from config() and default() */

int set_wide (const void *valp)
{
	int wide = *(const int *)valp;

	if (gbl(scr_in_config) && !gbl(scr_is_split))
	{
		if (wide)
		{
			gbl(scr_cur)->cur_dir_win      = gbl(win_dir_wide);
			gbl(scr_cur)->cur_dest_dir_win = gbl(win_dest_dir_wide);
			gbl(scr_cur)->small_file_win   = gbl(win_small_file_wide);
			gbl(scr_cur)->large_file_win   = gbl(win_large_file_wide);
		}
		else
		{
			gbl(scr_cur)->cur_dir_win      = gbl(win_dir_reg);
			gbl(scr_cur)->cur_dest_dir_win = gbl(win_dest_dir_reg);
			gbl(scr_cur)->small_file_win   = gbl(win_small_file_reg);
			gbl(scr_cur)->large_file_win   = gbl(win_large_file_reg);
		}
		gbl(scr_cur)->cur_dest_dir_win = gbl(win_dest_dir_reg);
		gbl(scr_cur)->cur_path_line    = gbl(win_path_line_reg);
		if (gbl(scr_cur)->command_mode == m_dir ||
			gbl(scr_cur)->in_small_window)
		{
			gbl(scr_cur)->cur_file_win = (wide ?
				gbl(win_small_file_wide) : gbl(win_small_file_reg));
		}
		else
		{
			gbl(scr_cur)->cur_file_win = (wide ?
				gbl(win_large_file_wide) : gbl(win_large_file_reg));
		}

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
		if (gbl(scr_cur)->command_mode == m_dir ||
		   (gbl(scr_cur)->command_mode == m_file &&
			gbl(scr_cur)->in_small_window))
		{
			gbl(scr_cur)->cur_file_win = gbl(scr_cur)->small_file_win;
		}
		else
		{
			gbl(scr_cur)->cur_file_win = gbl(scr_cur)->large_file_win;
		}

		gbl(scr_cur)->max_dir_line  = getmaxy(gbl(scr_cur)->cur_dir_win)-1;
		gbl(scr_cur)->max_dir_level =
			get_max_dir_level(gbl(scr_cur)->cur_dir_win);
		gbl(scr_cur)->max_file_line = getmaxy(gbl(scr_cur)->cur_file_win)-1;
		set_file_disp();
		set_dir_disp();
	}
	return (0);
}

void clock_rtn (WINDOW *win, void *data,
	int yr, int mo, int da, int hh, int mm, int ss, int wday, int yday)
{
	char dl[24];
	char *d;

	d = tm_to_str(dl, yr, mo, da, hh, mm, ss);
	if (opt(time_fmt) == time_24)
		d[17] = 0;

	wmove(win, 0, 0);
	waddstr(win, d);
}
