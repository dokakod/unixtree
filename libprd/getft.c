/*------------------------------------------------------------------------
 * get a file-tree (log a regular node)
 */
#include "libprd.h"

int getft (NBLK *n, struct stat *pstbuf)
{
	TREE *root;
	TREE *curr_node;					/* current tree node */
	TREE *sib_list;
	TREE *tt;
	DBLK *dir_ptr;
	DBLK *cdir;
	BLIST *cdir_ptr;
	BLIST *b;
	BLIST *sub_list;
	int i;
	int dirno;
	int c;
	int log_loop;

	/*
	 *	Initialize root structure as
	 *	a single directory not logged.
	 */

	dir_ptr = fs_make_dblk(n->root_name, pstbuf);
	if (! dir_ptr)
		return (-1);

	root = TNEW(dir_ptr);
	if (!root)
	{
		c = errmsg(ER_IM, "", ERR_ANY);
		return (c);
	}
	dir_ptr->dir_tree = root;
	n->root = root;
	n->node_type = N_FS;

	/* check if read-only */

	if (ml_is_readonly_filesys(gbl(mount_list), pstbuf->st_dev))
		n->node_flags |= N_READONLY;

	cdir_ptr = BNEW(root);
	if (!cdir_ptr)
	{
		c = errmsg(ER_IM, "", ERR_ANY);
		return (c);
	}
	n->dir_list = cdir_ptr;

	/*
	 * Setup display for reading node
	 */

	strcpy(gbl(scr_cur)->path_name, n->root_name);
	disp_path_line();
	if (! gbl(scr_is_split) &&
		! opt(wide_screen)  &&
		! gbl(scr_cur)->in_dest_dir)
	{
		disp_file_box();
		disp_drive_info(pstbuf->st_dev, 1);
		disp_node_stats(n);
		werase(gbl(win_current));
		wrefresh(gbl(win_current));
	}
	if (! gbl(scr_cur)->in_dest_dir)
	{
		werase(gbl(win_commands));
		/* put fortune logic here */
		wrefresh(gbl(win_commands));
	}

	dirno = 0;
	if (opt(scroll_during_logging))
	{
		werase(gbl(scr_cur)->cur_dir_win);
#if 0
		if (n->dir_list)
		{
			dirno = bcount(n->dir_list);
			if (dirno > gbl(scr_cur)->max_dir_line)
				i = dirno-gbl(scr_cur)->max_dir_line-1;
			else
				i = 0;
			t = bnth(n->dir_list, i);
			for (k=0, b=t; b && i<dirno; i++, b=bnext(b))
			{
				wmove(gbl(scr_cur)->cur_dir_win, k++, 0);
				disp_dir_line(b);
			}
		}
#endif
		wrefresh(gbl(scr_cur)->cur_dir_win);
	}

	log_loop = TRUE;
	for (; log_loop && cdir_ptr; cdir_ptr=bnext(cdir_ptr))
	{
		if (xchk_kbd() == KEY_ESCAPE)
			break;
		curr_node = (TREE *)bid(cdir_ptr);
		cdir = (DBLK *)tid(curr_node);
		if (opt(scroll_during_logging))
		{
			if (dirno > gbl(scr_cur)->max_dir_line)
			{
				dirno = 0;
				werase(gbl(scr_cur)->cur_dir_win);
			}
			wmove(gbl(scr_cur)->cur_dir_win, dirno, 0);
			disp_dir_line(cdir_ptr);
			wrefresh(gbl(scr_cur)->cur_dir_win);
		}
		if (!gbl(scr_is_split) &&
			! opt(wide_screen) &&
			! gbl(scr_cur)->in_dest_dir)
		{
			char dbuf[12];

			disp_drive_info(cdir->stbuf.st_dev, 0);
			wmove(gbl(win_stats), 2, 8);
			wstandout(gbl(win_stats));
			waddstr(gbl(win_stats), xform(dbuf, n->node_total_count));
			wstandend(gbl(win_stats));
			wrefresh(gbl(win_stats));
			if (! opt(scroll_during_logging))
			{
				werase(gbl(win_current));
				wmove(gbl(win_current), 0, 1);
				waddstr(gbl(win_current), msgs(m_getft_readir));
				wmove(gbl(win_current), 1, 1);
				zaddstr(gbl(win_current), FULLNAME(cdir));
				wrefresh(gbl(win_current));
			}
		}

		sib_list = process_dir(n, curr_node, TRUE);

		/*
		 *  Now process the sub-directories
		 */

		if (sib_list)
		{
			tleft(curr_node) = sib_list;
			sub_list = 0;
			for (tt=tleft(curr_node); tt; tt=tright(tt))
			{
				b = BNEW(tt);
				if (!b)
				{
					c = errmsg(ER_IM, "", ERR_ANY);
					return (c);
				}
				sub_list = bappend(sub_list, b);
			}
			binsert(cdir_ptr, sub_list);
		}

		if (opt(scroll_during_logging))
		{
			wmove(gbl(scr_cur)->cur_dir_win, dirno, 0);
			disp_dir_line(cdir_ptr);
			wrefresh(gbl(scr_cur)->cur_dir_win);
			dirno++;
		}

		if (opt(logging_method) != log_auto)
			log_loop = FALSE;
	}

	for (i=0; i<gbl(scr_cur_count); i++)
	{
		if (gbl(scr_stat)[i].file_spec_pres)
		{
			n->showall_mlist[i] = get_mlist(n->showall_flist,
				gbl(scr_stat)[i].file_spec, 0, 0);
		}
		else
		{
			n->showall_mlist[i] = n->showall_flist;
		}
	}
	if (!gbl(scr_stat)[gbl(scr_cur_no)].file_spec_pres)
	{
		n->node_match_count = n->node_total_count;
		n->node_match_bytes = n->node_total_bytes;
	}
	return (0);
}

DBLK *fs_make_dblk (const char *pathname, struct stat *pstbuf)
{
	char rootname[MAX_PATHLEN];
	const char *name;
	DBLK *d;
	int i;

	d = dblk_make();
	if (! d)
	{
		errmsg(ER_IM, "", ERR_ANY);
		return (0);
	}

	name = fn_basename(pathname);
	if (*name == 0)
	{
		fn_rootname(pathname, rootname);
		name = rootname;
	}

	dblk_set_name(d, name);
	d->flags = D_NOT_LOGGED;
	d->log_time = 0;
	d->hidden = 0;
	make_xstat(pstbuf, &d->stbuf);
	d->dir_size = 0;
	d->flist = 0;
	for (i=0; i<gbl(scr_cur_count); i++)
		d->mlist[i] = 0;
	return (d);
}

FBLK *fs_make_fblk (const char *name, struct stat *pstbuf)
{
	FBLK *f;

	f = fblk_make();
	if (! f)
	{
		errmsg(ER_IM, "", ERR_ANY);
		return (0);
	}
	fblk_set_name(f, name);
	f->archive_vol = -1;
	f->archive_loc = -1;
	f->tagged = 0;
	make_xstat(pstbuf, &f->stbuf);
	f->dir = 0;
	f->sym_name = 0;
	f->sym_mode = 0;
	return (f);
}

/*
 * Process a directory
 *
 * This routine will read the directory, create the lists
 * of files found in the DBLK and return a list (actually
 * a TREE chain) of directories found.
 */

TREE *process_dir (NBLK *n, TREE *curr_node, int flag)
{
	char	filename[MAX_FILELEN];
	char	pathname[MAX_PATHLEN];
	char	fullpath[MAX_PATHLEN];
	char	sym_path[MAX_PATHLEN];
	TREE *	new_tree;
	TREE *	sib_list;
	TREE *	p;
	DBLK *	dir_ptr;
	DBLK *	cdir;
	DBLK *	d;
	FBLK *	file_blk;
	BLIST *	b;
	BLIST *	x;
	BLIST *	last_file;
	BLIST *	l;
	char *	e;
	int		i;
	int		exclude_it = FALSE;
	int		sym_mode;
	struct stat stbuf;

	for (p=curr_node; tparent(p); p=tparent(p))
		;
	d = (DBLK *)tid(p);				/* root directory */
	cdir = (DBLK *)tid(curr_node);
	dirtree_to_dirname(curr_node, pathname);
	sib_list = 0;
	last_file = 0;

	if (flag)
	{
		for (b=cmdopt(exclude_list); b; b=bnext(b))
		{
			e = (char *)bid(b);
			if (strcmp(FULLNAME(cdir), e) == 0)
			{
				exclude_it = TRUE;
				break;
			}
		}
	}

	if (! exclude_it &&
		can_we_read(&cdir->stbuf) &&
		(opt(cont_at_mount_points) ||
			(d->stbuf.st_dev == cdir->stbuf.st_dev)) &&
	    (opt(log_network_files) ||
			! ml_is_remote_filesys(gbl(mount_list), cdir->stbuf.st_dev)) )
	{
		UTDIR *utdir;
		int rc;

		cdir->flags = 0;
		cdir->log_time = time(0);

		if (! gbl(scr_cur)->in_dest_dir &&
			d->stbuf.st_dev != cdir->stbuf.st_dev)
		{
			disp_drive_info(cdir->stbuf.st_dev, 0);
		}

		utdir = os_dir_open(pathname);
		if (utdir != 0)
		{
			while (TRUE)
			{
				rc = os_dir_read(utdir, filename);
				if (rc)
					break;

				strcpy(fullpath, pathname);
				fn_append_filename_to_dir(fullpath, filename);
				if (os_lstat(fullpath, &stbuf))
					continue;

				*sym_path = 0;
				sym_mode = 0;
				if (S_ISLNK(stbuf.st_mode))
				{
					struct stat sym_stat;

					rc = os_readlink(fullpath, sym_path, sizeof(sym_path));
					if (rc > 0)
					{
						sym_path[rc] = 0;
						rc = os_stat(fullpath, &sym_stat);
						if (rc == 0)
						{
							sym_mode = sym_stat.st_mode;
						}
					}

					if (opt(resolve_sym_links))
					{
						if (check_symlink(fullpath) == 0)
						{
							if (os_stat(fullpath, &sym_stat) == 0)
							{
								memcpy(&stbuf, &sym_stat, sizeof(stbuf));
								*sym_path = 0;
								sym_mode = 0;
							}
						}
					}
				}

				if (S_ISDIR(stbuf.st_mode))
				{
					/*
					 * Got directory entry
					 */

					dir_ptr = fs_make_dblk(filename, &stbuf);
					if (! dir_ptr)
						goto done;
					new_tree = TNEW(dir_ptr);
					if (new_tree == 0)
					{
						errmsg(ER_IM, "", ERR_ANY);
						goto done;
					}
					dir_ptr->dir_tree = new_tree;
					tparent(new_tree) = curr_node;
					tright(new_tree)  = sib_list;
					sib_list          = new_tree;
				}
				else
				{
					/*
					 * Got non-directory
					 */

					if (! S_ISREG(stbuf.st_mode))
					{
						if (! S_ISLNK(stbuf.st_mode))
						{
							stbuf.st_size = 0;		/* just to be sure */
						}
					}
					file_blk = fs_make_fblk(filename, &stbuf);
					if (! file_blk)
						goto done;
					if (*sym_path != 0)
					{
						file_blk->sym_name = STRDUP(sym_path);
						file_blk->sym_mode = sym_mode;
					}
					file_blk->dir = (DBLK *)tid(curr_node);
					n->node_total_count++;
					if (!check_for_dup_inode((BLIST *)n->link_list, file_blk,
						0, 0))
					{
						n->node_total_bytes += stbuf.st_size;
						cdir->dir_size += stbuf.st_size;
					}
					else
					{
						if (!check_for_dup_inode(cdir->flist, file_blk, 0, 0))
							cdir->dir_size += stbuf.st_size;
					}
					x = BNEW(file_blk);
					if (!x)
					{
						errmsg(ER_IM, "", ERR_ANY);
						goto done;
					}

					/*
					 * Add entry to flist.
					 * Maybe we should insert it in sort order here?
					 */

					if (last_file)
					{
						bprev(x) = last_file;
						bnext(last_file) = x;
					}
					else
					{
						cdir->flist = x;
					}
					last_file = x;

					/* If it has a link, enter it into the link list
						for the node */

					if (stbuf.st_nlink > 1)
					{
						l = BNEW(file_blk);
						if (l)
						{
							bnext(l) = n->link_list;
							n->link_list = l;
						}
					}
				}
			}
			os_dir_close(utdir);
		}
		else
		{
			cdir->flags = D_CANT_LOG;
		}
	}
	else
	{
		if (! can_we_read(&cdir->stbuf))
			cdir->flags = D_CANT_LOG;
	}

	/*
	 * Finished reading directory, now get mlists, showall lists
	 */

	cdir->flist = bsort(cdir->flist, compare_files);
	for (i=0; i<gbl(scr_cur_count); i++)
	{
		/* process any file spec for this view */

		if (gbl(scr_stat)[i].file_spec_pres)
		{
			int match_count = 0;
			int match_bytes = 0;

			cdir->mlist[i] = get_mlist(cdir->flist, gbl(scr_stat)[i].file_spec,
				&match_count, &match_bytes);
			if (i == gbl(scr_cur_no))
			{
				n->node_match_count += match_count;
				n->node_match_bytes += match_bytes;
			}
		}
		else
		{
			cdir->mlist[i] = cdir->flist;
		}
	}
	if (cdir->flist)
	{
		b = BDUP(cdir->flist);
		if (b)
			n->showall_flist = merge_lists(b, n->showall_flist);
	}

done:
	sib_list = sort_dirs(sib_list);
	return (sib_list);
}

void make_xstat (struct stat *pstbuf, struct stat *xbuf)
{
	xbuf->st_dev   = pstbuf->st_dev;
	xbuf->st_ino   = pstbuf->st_ino;
	xbuf->st_mode  = pstbuf->st_mode;
	xbuf->st_nlink = pstbuf->st_nlink;
	xbuf->st_uid   = pstbuf->st_uid;
	xbuf->st_gid   = pstbuf->st_gid;
	set_rdev(xbuf, get_rdev(pstbuf));
	xbuf->st_size  = pstbuf->st_size;
	xbuf->st_atime = pstbuf->st_atime;
	xbuf->st_mtime = pstbuf->st_mtime;
	xbuf->st_ctime = pstbuf->st_ctime;
	check_owner(pstbuf->st_uid);
	check_group(pstbuf->st_gid);
}

int is_it_a_dir (const char *directory, struct stat *pstbuf)
{
	struct stat tmp_stbuf;
	int i;

	if (pstbuf == 0)
		pstbuf = &tmp_stbuf;

	i = os_stat(directory, pstbuf);
	if (i)
		return (-1);		/* does not exist */

	if (! S_ISDIR(pstbuf->st_mode))
		return (1);			/* is not a directory */

	return (0);
}

int is_directory (const char *directory, struct stat *pstbuf)
{
	int i;

	i = is_it_a_dir(directory, pstbuf);
	if (i == 0)
		return (0);
	werase(gbl(win_message));
	wmove(gbl(win_message), 0, 0);
	zaddstr(gbl(win_message), directory);
	if (i == -1)
		waddstr(gbl(win_message), msgs(m_getft_dne));
	else
		waddstr(gbl(win_message), msgs(m_getft_ind));
	wrefresh(gbl(win_message));
	return (anykey());
}
