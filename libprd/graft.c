/*------------------------------------------------------------------------
 * process the "graft" cmd
 */
#include "libprd.h"

void graft (void)
{
	char old_name[MAX_PATHLEN];
	char new_name[MAX_PATHLEN];
	TREE *target;
	BLIST *first_dir;
	BLIST *last_dir;
	int c;
	int i;
	char full_name[MAX_PATHLEN];
	TREE *last_dir_tree;
	BLIST *b;
	TREE *t;
	DBLK *d;
	NBLK *n;

	if (check_sub_node(TRUE))
		return;

	if (get_first_last(&first_dir, &last_dir, TRUE))
	{
		return;
	}
	last_dir_tree = (TREE *)bid(last_dir);

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_graft_grasub));
	strcpy(old_name, gbl(scr_cur)->path_name);
	zaddstr(gbl(win_commands), old_name);
	wrefresh(gbl(win_commands));
	if (yesno_msg(msgs(m_graft_granod)))
	{
		i = gbl(scr_cur)->cur_dir_line;
		for (b=bnext(first_dir); b; b=bnext(b))
		{
			if (++i <= gbl(scr_cur)->max_dir_line)
				hilite_the_dir(OFF, b, i);
			else
				break;
			t = (TREE *)bid(b);
			if (t == last_dir_tree)
				break;
		}
		return;
	}
	bang("");
	strcpy(new_name, old_name);
	wmove(gbl(win_commands), 1, 0);
	waddstr(gbl(win_commands), msgs(m_graft_tosub));
	wrefresh(gbl(win_commands));
	c = do_dest_dir(new_name, FALSE, &target);
	if (c < 0)
		return;

	/* check if target dir is in same node */

	if (get_root_of_dir(target) != gbl(scr_cur)->cur_root)
	{
		errmsg(ER_DIDN, "", ERR_ANY);
		return;
	}

	/* check if target dir is in same filesystem */

	d = (DBLK *)tid(target);
	if (d->stbuf.st_dev != (gbl(scr_cur)->cur_dir)->stbuf.st_dev)
	{
		errmsg(ER_NODD, "", ERR_ANY);
		return;
	}

	/* check if we have permissions */

	if (check_dir(d))
		return;

	/* check if target diff from source */

	if (target == gbl(scr_cur)->cur_dir_tree)
	{
		errmsg(ER_CGTY, "", ERR_ANY);
		return;
	}

	/* check if target contained in source sub-node */

	if (first_dir != last_dir)
	{
		for (b=bnext(first_dir); b; b=bnext(b))
		{
			t = (TREE *)bid(b);
			if (target == t)
			{
				errmsg(ER_CGIY, "", ERR_ANY);
				return;
			}
			if (t == last_dir_tree)
				break;
		}
	}

	/* check if target is logged */

	if (d->flags & D_NOT_LOGGED)
	{
		errmsg(ER_TDNL, "", ERR_ANY);
		return;
	}

	/*	create new sub node tree */

#if 1
	dirtree_to_dirname(target, full_name);
	fn_append_dirname_to_dir(full_name, FULLNAME(gbl(scr_cur)->cur_dir));
	c = os_dir_rename(gbl(scr_cur)->path_name, full_name);
	if (c != 0)
	{
		errsys(ER_CRD);
		return;
	}
#else
	t = tparent(gbl(scr_cur)->cur_dir_tree);
	if (!t)
	{
		*old_name = 0;
		i = 0;
	}
	else
	{
		dirtree_to_dirname(t, old_name);
		i = fn_num_subdirs(old_name);
	}
	for (b=first_dir; b; b=bnext(b))
	{
		/* create name new_name/sub_node_name */
		strcpy(full_name, new_name);
		dptr_to_dirname(b, old_name);
		for (j=i; ; j++)
		{
			np = fn_get_nth_dir(old_name, j);
			if (!np)
				break;
			fn_append_dirname_to_dir(full_name, np);
		}
		if (make_dir(full_name) == -1)
			return;
		t = (TREE *)bid(b);
		d = (DBLK *)tid(t);
		i_chmod(full_name, d->stbuf.st_mode);
		i_chown(full_name, d->stbuf.st_uid, d->stbuf.st_gid);
		if (t == last_dir_tree)
			break;
	}

	/*	link all files in each directory to new sub node */
	/*	note - files will be stored in the order displayed */

	for (b=first_dir; ; b=bnext(b))
	{
		t = (TREE *)bid(b);
		d = (DBLK *)tid(t);
		strcpy(full_name, new_name);
		dptr_to_dirname(b, old_name);
		for (j=i; ; j++)
		{
			np = fn_get_nth_dir(old_name, j);
			if (!np)
				break;
			fn_append_dirname_to_dir(full_name, np);
		}
		fn_terminate_dirname(full_name);
		np = full_name + strlen(full_name);
		for (x=d->flist; x; x=bnext(x))
		{
			f = (FBLK *)bid(x);
			fblk_to_pathname(f, old_name);
			strcpy(np, FULLNAME(f));
			if (i_link(old_name, full_name))
			{
				errmsg(ER_CL, old_name, ERR_ANY);
				return;
			}
		}
		if (t == last_dir_tree)
			break;
	}

	/*	delete all files in original directories & delete dir */
	/*	note - we go backwards so as to empty sub-dirs as we go */

	for (; ; b=bprev(b))
	{
		del_files_in_dir(b);
		dptr_to_dirname(b, full_name);
		delete_dir(full_name);
		if (b == first_dir)
			break;
	}

	/*	update actual size of all directories and displays */

	for (b=first_dir; b; b=bnext(b))
	{
		t = (TREE *)bid(b);
		dirtree_to_dirname(t, old_name);
		if (i_stat(old_name, &stbuf) == 0)
		{
			d = (DBLK *)tid(t);
			d->stbuf.st_size = stbuf.st_size;
			d->stbuf.st_mtime = stbuf.st_mtime;
		}
		if (t == last_dir_tree)
			break;
	}
#endif

	/* remove node from old place in tree */

	t = tparent(gbl(scr_cur)->cur_dir_tree);
	tleft(t) = tremover(tleft(t), gbl(scr_cur)->cur_dir_tree);

	/* insert node in new place in tree */

	add_dir_to_parent(gbl(scr_cur)->cur_dir_tree, target);

	/* make new dir lists */

	n = gbl(scr_cur)->cur_root;
	fix_dir_list(n);

	set_top_dir();

	/* update display */
	/* note that node statistics don't change */

	disp_dir_tree();
	do_dir_scroll_bar();
	hilite_dir(ON);
	disp_drive_info(d->stbuf.st_dev, 1);			/* update free space */
}

int check_sub_node (int graft_flag)
{
	char abs_name[MAX_PATHLEN];
	BLIST *b;
	TREE *t;
	DBLK *d;
	int lev;
	int c;

	/*	check if cur dir is top level node */

	if (gbl(scr_cur)->dir_cursor == gbl(scr_cur)->first_dir)
	{
		if (graft_flag)
			c = errmsg(ER_CGRN, "", ERR_ANY);
		else
			c = errmsg(ER_CCRN, "", ERR_ANY);
		return (c);
	}

	/*	check if parent directory is writable & searchable */

	t = tparent(gbl(scr_cur)->cur_dir_tree);
	d = (DBLK *)tid(t);
	c = check_dir(d);
	if (c)
		return (c);

	/*	check if cwd is within node */

	dptr_to_dirname(gbl(scr_cur)->dir_cursor, abs_name);
	if (fn_is_dir_in_dirname(abs_name, gbl(pgm_cwd)))
	{
		c = errmsg(ER_CCI, FULLNAME(gbl(scr_cur)->cur_dir), ERR_ANY);
		return (c);
	}

	/*	make sure we have permission to read, write & exec in all sub dirs */
	/* and that they are not mount points. */

	c = check_dir(gbl(scr_cur)->cur_dir);
	if (c)
		return (c);
	lev = tdepth((TREE *)bid(gbl(scr_cur)->dir_cursor));
	for (b=bnext(gbl(scr_cur)->dir_cursor); b; b=bnext(b))
	{
		t = (TREE *)bid(b);
		if (tdepth(t) <= lev)
			break;
		if (tleft(t) && (! bnext(b) || ((TREE *)bid(bnext(b))) != tleft(t)))
		{
			errmsg(ER_NCHD, "", ERR_ANY);
			return (1);
		}
		d = (DBLK *)tid(t);
		c = check_dir(d);
		if (c)
			return (c);
	}
	return (0);
}

int check_dir (DBLK *d)
{
	int c;

	if (! (can_we_read(&d->stbuf) &&
		can_we_write(&d->stbuf) && can_we_exec(&d->stbuf)) )
	{
		c = errmsg(ER_NPFD, FULLNAME(d), ERR_ANY);
		return (c);
	}
	if (d->stbuf.st_dev != (gbl(scr_cur)->cur_dir)->stbuf.st_dev)
	{
		c = errmsg(ER_CCMP, FULLNAME(d), ERR_ANY);
		return (c);
	}
	return (0);
}
