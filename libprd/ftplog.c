/*------------------------------------------------------------------------
 * log an ftp node
 */
#include "libprd.h"

struct ftplog_info
{
	TREE *curr_dir_tree;
	DBLK *curr_dir;
	TREE *sib_list;
	NBLK *curr_node;
};
typedef struct ftplog_info FTPLOG_INFO;

static int ftp_logrtn (FTP_NODE *f, FTP_STAT *ftp_stat, void *data)
{
	FTPLOG_INFO *fi = (FTPLOG_INFO *)data;
	struct stat *s = &ftp_stat->stbuf;
	DBLK *dir_ptr;
	FBLK *file_blk;
	BLIST *l;
	BLIST *x;
	TREE *new_tree;
	TREE *t;
	DBLK *d;
	SYM_ENTRY *sym;

	if (strcmp(ftp_stat->file_name, ".") == 0)
	{
		make_ftp_xstat(s, &fi->curr_dir->stbuf);
		fi->curr_dir->flags = 0;
		return (0);
	}

	if (strcmp(ftp_stat->file_name, "..") == 0)
	{
		t = tparent(fi->curr_dir_tree);
		if (t)
		{
			d = (DBLK *)tid(t);
			make_ftp_xstat(s, &d->stbuf);
		}
		return (0);
	}

	if (S_ISDIR(s->st_mode))
	{
			dir_ptr = make_ftp_dblk(ftp_stat->file_name, s);
			if (! dir_ptr)
				return (-1);
			new_tree = TNEW(dir_ptr);
			if (new_tree == 0)
			{
				errmsg(ER_IM, "", ERR_ANY);
				return (-1);
			}
			dir_ptr->dir_tree = new_tree;
			tparent(new_tree) = fi->curr_dir_tree;
			tright(new_tree)  = fi->sib_list;
			fi->sib_list      = new_tree;
			return (0);
	}

	if (! S_ISREG(s->st_mode))
	{
		if (! S_ISLNK(s->st_mode))
			s->st_size = 0;		/* just to be sure */
	}

	file_blk = make_ftp_fblk(ftp_stat->file_name, s);
	if (! file_blk)
		return (0);

	/* if sym-link, add to list */
	if (S_ISLNK(s->st_mode))
	{
		file_blk->sym_name = STRDUP(ftp_stat->sym_name);

		sym = (SYM_ENTRY *)MALLOC(sizeof(SYM_ENTRY));
		if (sym)
		{
			x = BNEW(sym);
			if (x)
			{
				strcpy(sym->sym_name, ftp_stat->sym_name);
				sym->sym_file = (void *)file_blk;
				f->symfiles = bappend(x, f->symfiles);
			}
			else
			{
				FREE(sym);
			}
		}
	}

	file_blk->dir = (DBLK *)tid(fi->curr_dir_tree);
	fi->curr_node->node_total_count++;
	if (!check_for_dup_inode((BLIST *)fi->curr_node->link_list, file_blk,
		0, 0))
	{
		fi->curr_node->node_total_bytes += s->st_size;
		fi->curr_dir->dir_size += s->st_size;
	}
	else
	{
		if (!check_for_dup_inode(fi->curr_dir->flist, file_blk, 0, 0))
			fi->curr_dir->dir_size += s->st_size;
	}

	x = BNEW(file_blk);
	if (!x)
	{
		errmsg(ER_IM, "", ERR_ANY);
		return (-1);
	}

	/*
	 * Add entry to flist.
	 * Maybe we should insert it in sort order here?
	 */

	fi->curr_dir->flist = bappend(fi->curr_dir->flist, x);

	/* If it has a link, enter it into the link list for the node */

	if (s->st_nlink > 1)
	{
		l = BNEW(file_blk);
		if (l)
		{
			bnext(l) = fi->curr_node->link_list;
			fi->curr_node->link_list = l;
		}
	}

	return (0);
}

static TREE *process_ftp_dir (NBLK *n, TREE *tdir, FTPLOG_INFO *fi)
{
	char pathname[MAX_PATHLEN];
	FTP_NODE *fn;
	BLIST *b;
	int i;
	SYM_ENTRY *s;
	int match_count;
	int match_bytes;


	fn = (FTP_NODE *)n->node_sub_blk;

	fi->curr_node = n;
	fi->curr_dir_tree = tdir;
	fi->curr_dir = (DBLK *)tid(tdir);
	dirtree_to_dirname(tdir, pathname);

	fi->sib_list = 0;

	/* check if this dir is a symlink */

	for (b=fn->symdirs; b; b=bnext(b))
	{
		s = (SYM_ENTRY *)bid(b);
		if (s->sym_file == (void *)fi->curr_dir)
			break;
	}
	if (b)
	{
		strcpy(pathname, s->sym_name);
	}

	if (ftp_log_dir(fn, pathname, ftp_logrtn, fi))
	{
			fi->curr_dir->flags = D_CANT_LOG;
	}

	/*
	 * now process any symlinks
	 */
	for (b=fi->curr_dir->flist; b; b=bnext(b))
	{
		FBLK *f = (FBLK *)bid(b);

		if (S_ISLNK(f->stbuf.st_mode) && f->sym_name != 0)
		{
			struct stat ftp_stat;
			char old_path[MAX_PATHLEN];
			char sym_path[MAX_PATHLEN];

			if (fn_is_path_absolute(f->sym_name))
			{
				strcpy(old_path, f->sym_name);
			}
			else
			{
				strcpy(old_path, pathname);
				fn_append_filename_to_dir(old_path, f->sym_name);
			}

			*sym_path = 0;
			if (ftp_get_syminfo(fn, old_path, sym_path, &ftp_stat) == 0)
			{
				if (*sym_path != 0)
				{
					FREE(f->sym_name);
					f->sym_name = STRDUP(sym_path);
				}
				f->sym_mode = ftp_stat.st_mode;
			}
		}
	}

	/*
	 * Finished reading directory, now get mlists, showall lists
	 */

	fi->curr_dir->flist = bsort(fi->curr_dir->flist, compare_files);
	for (i=0; i<gbl(scr_cur_count); i++)
	{
		/* process any file spec for this view */

		if (gbl(scr_stat)[i].file_spec_pres)
		{
			match_count = 0;
			match_bytes = 0;
			fi->curr_dir->mlist[i] = get_mlist(fi->curr_dir->flist,
				gbl(scr_stat)[i].file_spec, &match_count, &match_bytes);
			if (i == gbl(scr_cur_no))
			{
				n->node_match_count += match_count;
				n->node_match_bytes += match_bytes;
			}
		}
		else
		{
			fi->curr_dir->mlist[i] = fi->curr_dir->flist;
		}
	}

	if (fi->curr_dir->flist)
	{
		b = BDUP(fi->curr_dir->flist);
		if (b)
			n->showall_flist = merge_lists(b, n->showall_flist);
	}

	fi->sib_list = sort_dirs(fi->sib_list);

	return (fi->sib_list);
}

static BLIST *log_ftp_dir (NBLK *n, BLIST *dptr)
{
	FTPLOG_INFO		ftplog_info;
	FTPLOG_INFO *	fi = &ftplog_info;
	BLIST *b;
	BLIST *bn;
	BLIST *bp;
	BLIST *bt;
	BLIST *br;
	BLIST *bl;
	TREE *ct;
	TREE *t;
	TREE *tn;
	TREE *td;
	TREE *tt;
	DBLK *d1;
	DBLK *d2;
	int i;
	int l1;
	int l2;
	char *n1;
	char *n2;

	ct = (TREE *)bid(dptr);
	clean_dir(ct, n, FALSE);
	d1 = (DBLK *)tid(ct);
	d1->dir_size = 0;

	/* make sure any sub-dirs are in alphabetical order */

	if (tleft(ct))
	{
		tleft(ct) = sort_dirs(tleft(ct));
		make_dir_list(n);
		dptr = bfind(n->dir_list, ct);
	}

	t = process_ftp_dir(n, ct, fi);

	/*
	 * Process sub-directories found.
	 *
	 * loop thru sub-dir list and list of sub-dirs in dir_list
	 *   if entry in t but not in dir_list, add new directory
	 *   if entry in dir_list but not in t, throw it away
	 *   if in both, check if it has a new date (changed)
	 */

	l1 = tdepth(ct)+1;			/* level sub-dirs must be at */
	bp = dptr;
	b  = bnext(bp);
	bl = 0;
	/*
	 * t points to current dir in dirs-read list
	 * b points to next entry in the dir-list
	 */
	while (t || b)
	{
		td = 0;
		l2 = 0;
		d2 = 0;
		if (b)
		{
			td = (TREE *)bid(b);
			l2 = tdepth(td);
			if (l2 < l1)		/* at next sibling or uncle */
			{
				bl = b;
				b = 0;
				continue;
			}
			else if (l2 > l1)		/* at nephew */
			{
				b = bnext(b);
				continue;
			}
		}
		if (b)
		{
			d2 = (DBLK *)bid(td);
			n2 = FULLNAME(d2);
		}
		else
		{
			n2 = 0;
		}

		if (t)
		{
			tn = tright(t);
			d1 = (DBLK *)tid(t);
			n1 = FULLNAME(d1);
		}
		else
		{
			tn = 0;
			n1 = 0;
		}

		if (!n1)
			i = 1;
		else if (!n2)
			i = -1;
		else
			i = strcmp(n1, n2);

		if (i < 0)			/* new sub_dir found */
		{
			/* get new bilist entry for dir_list */
			bn = BNEW(t);
			if (bn == 0)
			{
				errmsg(ER_IM, "", ERR_ANY);
				dblk_free((DBLK *)tid(t));
				TFREE(t, FALSE);
				t = tn;
			}
			else
			{
				/* hook entry into dirlist */
				tparent(t) = ct;
				td = (TREE *)bid(bp);	/* tree of entry just above us */
				if (td == ct)			/* first sub-dir */
				{
					tright(t) = tleft(td);
					tleft(td) = t;
				}
				else
				{
					tright(t)  = tright(td);
					tright(td) = t;
				}
				if (b)
					binsert(bprev(b), bn);
				else if (bl)
					binsert(bprev(bl), bn);
				else
					bappend(bp, bn);
				t = tn;
				bp = bn;
				b = bnext(bn);
			}
		}
		else if (i == 0)		/* same sub-dir found */
		{
			if (d1->stbuf.st_mtime > d2->stbuf.st_mtime)
			{
				/* mark as changed if logged */
				if (!d2->flags)
					d2->flags |= D_CHANGED;
			}
			make_ftp_xstat(&d2->stbuf, &d1->stbuf);
			dblk_free((DBLK *)tid(t));
			TFREE(t, FALSE);
			t  = tn;
			bp = b;
			b  = bnext(b);
		}
		else /* if (i > 0) */		/* old sub-dir no longer exists */
		{
			/* scan for last sub-dir under this dir */
			for (bt=b; (bn=bnext(bt)) != 0; bt=bnext(bt))
			{
				tt = (TREE *)bid(bn);
				if (tdepth(tt) <= l2)
					break;
			}
			b = bn;
			/* loop backwards, removing sub-dirs as we go */
			for (; bt!=bp; bt=br)
			{
				br = bprev(bt);
				tt = (TREE *)bid(bt);
				clean_dir(tt, n, FALSE);
				del_dir_from_node(bt, n);
			}
		}
	}

	d1 = (DBLK *)tid(ct);
	for (i=0; i<gbl(scr_cur_count); i++)
	{
		if (gbl(scr_stat)[i].file_spec_pres)
		{
			b = BDUP(d1->mlist[i]);
			n->showall_mlist[i] = merge_lists(b, n->showall_mlist[i]);
		}
		else
			n->showall_mlist[i] = n->showall_flist;
	}
	if (!gbl(scr_stat)[gbl(scr_cur_no)].file_spec_pres)
	{
		n->node_match_count = n->node_total_count;
		n->node_match_bytes = n->node_total_bytes;
	}

	return (dptr);
}

void relog_ftp_dir (int recurse)
{
	int i;
	int l;
	BLIST *b;
	BLIST *bt;
	TREE *t;
	NBLK *n;

	/* must be in dir or file mode (not showall or global) */

	if (!(gbl(scr_cur)->command_mode == m_dir ||
		gbl(scr_cur)->command_mode == m_file))
	{
		return;
	}

	/* get node & entry in dir_list in node */

	n = get_root_of_dir(gbl(scr_cur)->cur_dir_tree);
	if (n->dir_disp_list[gbl(scr_cur_no)])
		bt = bfind(n->dir_list, gbl(scr_cur)->cur_dir_tree);
	else
		bt = gbl(scr_cur)->dir_cursor;

	bt = log_ftp_dir(n, bt);

	if (gbl(scr_cur)->command_mode == m_dir)
	{
		disp_node_stats(gbl(scr_cur)->cur_root);
	}

	/* recurse thru sub-dirs if flag on */

	if (recurse)
	{
		l = tdepth(gbl(scr_cur)->cur_dir_tree);
		for (b=bnext(bt); b; b=bnext(b))
		{
			if (xchk_kbd() == KEY_ESCAPE)
				break;
			t = (TREE *)bid(b);
			if (tdepth(t) <= l)
				break;
			b = log_ftp_dir(n, b);
			if (gbl(scr_cur)->command_mode == m_dir)
			{
				disp_node_stats(gbl(scr_cur)->cur_root);
			}
		}
	}

	/* rebuild all directory lists */

#if 0
	make_dir_list(n);
#endif
	for (i=0; i<gbl(scr_cur_count); i++)
	{
		if (n->dir_disp_list[i])
			hide_dirs_in_node(n, i);
	}

	/* set pointers & counters */

	set_top_dir();

	/* if in file display, check if still files to display */

	if (gbl(scr_cur)->command_mode != m_dir)
	{
		if (!(gbl(scr_cur)->cur_dir)->mlist[gbl(scr_cur_no)])
		{
			gbl(scr_cur)->in_small_window = FALSE;
			gbl(scr_cur)->command_mode = m_dir;
		}
	}

	if (gbl(scr_cur)->command_mode == m_dir)
	{
		disp_dir_tree();
		do_dir_scroll_bar();
		hilite_dir(ON);
		disp_node_stats(gbl(scr_cur)->cur_root);
		disp_cur_dir();
	}
	else if (gbl(scr_cur)->in_small_window)
	{
		disp_dir_tree();
		do_dir_scroll_bar();
		hilite_dir(OFF);
		left_arrow(gbl(scr_cur)->cur_dir_win);
		wrefresh(gbl(scr_cur)->cur_dir_win);
		disp_cur_file();
	}
	start_file_display();
	if (gbl(scr_cur)->command_mode != m_dir)
		hilite_file(ON);
}
