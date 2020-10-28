/*------------------------------------------------------------------------
 * node routines
 */
#include "libprd.h"

static int count_fs (void)
{
	BLIST *b;
	NBLK *n;
	int count = 0;

	for (b=gbl(nodes_list); b; b=bnext(b))
	{
		n = (NBLK *)bid(b);
		if (n->node_type == N_FS)
			count++;
	}
	return (count);
}

static void relog (void)
{
	BLIST *b;
	NBLK *n;
	int c;

	/* get current node */

	for (b=gbl(nodes_list); b; b=bnext(b))
	{
		n = (NBLK *)bid(b);
		if (n == gbl(scr_cur)->cur_root)
			break;
	}

	if (n->node_type != N_FS)
		return;

	/* ask if relog OK */

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_newnode_relnod));
	zaddstr(gbl(win_commands), n->root_name);
	wrefresh(gbl(win_commands));
	c = yesno_msg(msgs(m_newnode_rel));
	if (c)
	{
		disp_cmds();
		return;
	}

	del_node(n);

	gbl(scr_cur)->cur_node = b;
	gbl(scr_cur)->cur_root = n;
	read_node(n);
	init_disp();
	return;
}

static void log_ftp_node (char *host, char *user, char *passwd, char *rdir)
{
	int c;

	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_newnode_flog));

	bang(msgs(m_newnode_bhost));

	wmove(gbl(win_commands), 1, 0);
	wclrtoeol(gbl(win_commands));
	xaddstr(gbl(win_commands), msgs(m_newnode_host));
	wrefresh(gbl(win_commands));

	c = xgetstr(gbl(win_commands), host, XGL_HOST, 64, 0, XG_STRING);
	if (c <= 0)
	{
		disp_cmds();
		return;
	}

	bang(msgs(m_newnode_buser));

	wmove(gbl(win_commands), 1, 0);
	wclrtoeol(gbl(win_commands));
	xaddstr(gbl(win_commands), msgs(m_newnode_user));
	wrefresh(gbl(win_commands));

	if (! *user)
	{
		os_get_usr_name(user);
	}
	c = xgetstr(gbl(win_commands), user, XGL_USER, 16, 0, XG_FILENAME);
	if (c < 0)
	{
		disp_cmds();
		return;
	}

	if (c == 0)
	{
		char user_name[64];
		char comp_name[64];

		/*----------------------------------------------------------------
		 * set username to "anonymous"
		 * and passwd to "<user-name>@<computer-name>"
		 */
		strcpy(user, "anonymous");

		os_get_usr_name(user_name);
		os_get_hostname(comp_name);

		strcpy(passwd, user_name);
		strcat(passwd, "@");
		strcat(passwd, comp_name);
	}
	else
	{
		bang(msgs(m_newnode_bpasswd));

		wmove(gbl(win_commands), 1, 0);
		wclrtoeol(gbl(win_commands));
		xaddstr(gbl(win_commands), msgs(m_newnode_passwd));
		wrefresh(gbl(win_commands));

		c = xgetstr(gbl(win_commands), passwd, XGL_NONE, 16, 0, XG_PASSWD);
		if (c < 0)
		{
			disp_cmds();
			return;
		}
	}

	bang(msgs(m_newnode_brdir));

	wmove(gbl(win_commands), 1, 0);
	wclrtoeol(gbl(win_commands));
	xaddstr(gbl(win_commands), msgs(m_newnode_rdir));
	wrefresh(gbl(win_commands));

	c = xgetstr(gbl(win_commands), rdir, XGL_RDIR, MAX_PATHLEN, 0, XG_STRING);
	if (c < 0)
	{
		disp_cmds();
		return;
	}

	ftp_setup_node(host, user, passwd, rdir);
}

static void log_ftp (void)
{
	char host[64];
	char user[64];
	char passwd[64];
	char rdir[MAX_PATHLEN];

	*host   = 0;
	*user   = 0;
	*passwd = 0;
	*rdir   = 0;

	log_ftp_node(host, user, passwd, rdir);
}

static int new_log (void)
{
	int c;
	char node_name[MAX_FILELEN];
	char input_str[MAX_PATHLEN];
	char *node;
	char user[64];
	char passwd[64];

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_newnode_entlog));
	wrefresh(gbl(win_commands));
	*input_str = 0;
	c = xgetstr(gbl(win_commands), input_str, XGL_NODE, MAX_PATHLEN, 0,
		XG_PATHNAME);
	if (c <= 0)
	{
		disp_cmds();
		return (0);
	}

	node = fn_nodename(input_str, node_name);
	if (node)
	{
		*user = 0;
		*passwd = 0;
		log_ftp_node(node, user, passwd, input_str);
		return (0);
	}

	fn_resolve_pathname(input_str);
	if (do_new_log(input_str))
		disp_cmds();

	return (0);
}

BLIST *log_dir (NBLK *n, BLIST *dptr)
{
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
	struct stat stbuf;
	char pathname[MAX_PATHLEN];

	ct = (TREE *)bid(dptr);
	clean_dir(ct, n, FALSE);
	d1 = (DBLK *)tid(ct);
	dirtree_to_dirname(ct, pathname);
	os_stat(pathname, &stbuf);
	make_xstat(&stbuf, &d1->stbuf);
	d1->dir_size = 0;

	/* make sure any sub-dirs are in alphabetical order */

	if (tleft(ct))
	{
		tleft(ct) = sort_dirs(tleft(ct));
		make_dir_list(n);
		dptr = bfind(n->dir_list, ct);
	}

	t = process_dir(n, ct, FALSE);

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
			d2 = (DBLK *)tid(td);
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
			make_xstat(&d2->stbuf, &d1->stbuf);
			dblk_free((DBLK *)tid(t));
			TFREE(t, FALSE);
			t = tn;
			bp = b;
			b = bnext(b);
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

static int is_curdir_invalid (void)
{
	struct stat stbuf;

	if (os_stat(gbl(scr_cur)->path_name, &stbuf) == -1)
	{
		if (errno == ENOENT)
			return (TRUE);
		return (FALSE);
	}

	if ((gbl(scr_cur)->cur_dir)->log_time < stbuf.st_mtime)
		return (TRUE);
	return (FALSE);
}

void do_log (void)
{
	int c;

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xcaddstr(gbl(win_commands), CMDS_NODE_RELOG, msgs(m_newnode_relog));
	xaddstr(gbl(win_commands), msgs(m_newnode_exist));
	waddstr(gbl(win_commands), "  ");
	xaddstr(gbl(win_commands), msgs(m_newnode_loga));
	xcaddstr(gbl(win_commands), CMDS_NODE_NEW, msgs(m_newnode_new));
	xaddstr(gbl(win_commands), msgs(m_newnode_snode));
	waddstr(gbl(win_commands), "  ");
	xaddstr(gbl(win_commands), msgs(m_newnode_logan));
	xcaddstr(gbl(win_commands), CMDS_NODE_ARCHIVE, msgs(m_newnode_arch));
	xaddstr(gbl(win_commands), msgs(m_newnode_snode));
	waddstr(gbl(win_commands), "  ");
	xaddstr(gbl(win_commands), msgs(m_newnode_logan));
	xcaddstr(gbl(win_commands), CMDS_NODE_FTP, msgs(m_newnode_ftptype));
	xaddstr(gbl(win_commands), msgs(m_newnode_snode));

	wmove(gbl(win_commands), 1, 0);
	xcaddstr(gbl(win_commands), CMDS_NODE_LOAD, msgs(m_newnode_load));
	xaddstr(gbl(win_commands), msgs(m_newnode_load1));
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_NODE_SAVE, msgs(m_newnode_save));
	xaddstr(gbl(win_commands), msgs(m_newnode_exist));

	wrefresh(gbl(win_commands));
	werase(gbl(win_message));
	esc_msg();
	bangnc(msgs(m_newnode_entrn));
	while (TRUE)
	{
		c = xgetch(gbl(win_commands));
		c = TO_LOWER(c);
		if (c == KEY_ESCAPE ||
			c == KEY_RETURN ||
			c == cmds(CMDS_NODE_FTP) ||
			c == cmds(CMDS_NODE_SAVE) ||
			c == cmds(CMDS_NODE_LOAD) ||
			c == cmds(CMDS_NODE_RELOG) ||
			c == cmds(CMDS_NODE_NEW) ||
			c == cmds(CMDS_NODE_ARCHIVE) )
			break;
	}

	bang("");
	if (c == KEY_ESCAPE || c == KEY_RETURN)
	{
		disp_cmds();
		return;
	}

	if (c == cmds(CMDS_NODE_NEW))
	{
		gbl(scr_cur_sub_cmd) = CMDS_NODE_NEW;
		new_log();
	}
	else if (c == cmds(CMDS_NODE_RELOG))
	{
		gbl(scr_cur_sub_cmd) = CMDS_NODE_RELOG;
		relog();
	}
	else if (c == cmds(CMDS_NODE_ARCHIVE))
	{
		gbl(scr_cur_sub_cmd) = CMDS_NODE_ARCHIVE;
		log_archive();
	}
	else if (c == cmds(CMDS_NODE_FTP))
	{
		gbl(scr_cur_sub_cmd) = CMDS_NODE_FTP;
		log_ftp();
	}
	else if (c == cmds(CMDS_NODE_LOAD))
	{
		gbl(scr_cur_sub_cmd) = CMDS_NODE_LOAD;
		do_load_node();
	}
	else /* if (c == cmds(CMDS_NODE_SAVE)) */
	{
		gbl(scr_cur_sub_cmd) = CMDS_NODE_SAVE;
		do_save_node();
	}
}

void do_release (void)
{
	NBLK *n = gbl(scr_cur)->cur_root;
	ABLK *a;
	int c;
	int node_changed;
	int num_nodes;

	node_changed = FALSE;

	/* can't release node if only one */

	num_nodes = bcount(gbl(nodes_list));
	if (num_nodes == 1)
	{
		errmsg(ER_CRYON, "", ERR_ANY);
		return;
	}

	/* can't release node if only FS */

	if (n->node_type == N_FS && count_fs() == 1)
	{
		/* can't delete FS node if only one */
		errmsg(ER_CRYON, "", ERR_ANY);
		return;
	}

	/* ask if OK */

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_newnode_rlsnod));

	switch (n->node_type)
	{
	case N_FS:
		xaddstr(gbl(win_commands), msgs(m_newnode_node));
		zaddstr(gbl(win_commands), n->root_name);
		break;

	case N_ARCH:
		a = (ABLK *)n->node_sub_blk;
		switch (a->arch_type)
		{
		case A_BAR:
			xaddstr(gbl(win_commands), msgs(m_newnode_bar));
			xaddstr(gbl(win_commands), msgs(m_newnode_node));
			waddstr(gbl(win_commands), a->arch_name);
			break;

		case A_CPIO:
			xaddstr(gbl(win_commands), msgs(m_newnode_cpio));
			xaddstr(gbl(win_commands), msgs(m_newnode_node));
			waddstr(gbl(win_commands), a->arch_name);
			break;

		case A_TAR:
			xaddstr(gbl(win_commands), msgs(m_newnode_tar));
			xaddstr(gbl(win_commands), msgs(m_newnode_node));
			waddstr(gbl(win_commands), a->arch_name);
			break;

		case A_ZTAR:
			xaddstr(gbl(win_commands), msgs(m_newnode_ztar));
			xaddstr(gbl(win_commands), msgs(m_newnode_node));
			waddstr(gbl(win_commands), a->arch_name);
			break;
		}
		break;

	case N_FTP:
		xaddstr(gbl(win_commands), msgs(m_newnode_ftp));
		xaddstr(gbl(win_commands), msgs(m_newnode_node));
		waddstr(gbl(win_commands), ftp_host(n));
		break;
	}
	wrefresh(gbl(win_commands));
	c = yesno_msg(msgs(m_newnode_rls));
	if (c == 0)
	{
		BLIST *b = gbl(scr_cur)->cur_node;

		node_changed = TRUE;
		gbl(scr_cur)->cur_node = bnext(gbl(scr_cur)->cur_node);
		if (!gbl(scr_cur)->cur_node)
			gbl(scr_cur)->cur_node = gbl(nodes_list);

		del_node(n);
		nblk_free(n);
		gbl(nodes_list) = BDELETE(gbl(nodes_list), b, FALSE);
	}

	if (node_changed)
		init_disp();
	else
		disp_cmds();
}

int do_new_log (char *input_str)
{
	BLIST *b;
	TREE *t;
	NBLK *n;
	char path[MAX_PATHLEN];

	fn_resolve_pathname(input_str);
	fn_get_abs_dir(gbl(scr_cur)->path_name, input_str, path);
	if (is_directory(path, 0))
		return (-1);

	/* check if pathname is already in structure */

	t = path_to_dt(path, N_FS);
	if (t == 0)
		n = 0;
	else
		n = get_root_of_dir(t);

	if (n == 0)
	{
		/* check if path contains a logged node */

		for (b=gbl(nodes_list); b; b=bnext(b))
		{
			n = (NBLK *)bid(b);
			if (n->node_type == N_FS)
			{
				if (fn_is_dir_in_dirname(n->root_name, path))
				{
					break;
				}
			}
		}

		if (b == 0)
			n = 0;
	}

	if (n != 0)
	{
		if (n == gbl(scr_cur)->cur_root)
		{
			if ((!gbl(scr_cur)->in_dest_dir) &&
				(gbl(scr_cur)->cmd_sub_mode == m_alt))
			{
				n = 0;
			}
		}
	}

	if (n != 0)
	{
		if (t != 0)
			errmsg(ER_PCLN, "", ERR_ANY);
		else
			errmsg(ER_PWLN, "", ERR_ANY);

		return (-1);
	}

	if ((!gbl(scr_cur)->in_dest_dir) && (gbl(scr_cur)->cmd_sub_mode == m_alt))
	{
		n = gbl(scr_cur)->cur_root;
		del_node(n);
		nblk_free(n);
		gbl(nodes_list) =
			BDELETE(gbl(nodes_list), gbl(scr_cur)->cur_node, FALSE);
	}

	if (init_node(path) == 0)
		init_disp();

	return (0);
}

void relog_dir (int recurse)
{
	int i;
	int l;
	BLIST *b;
	BLIST *bt;
	TREE *t;
	DBLK *d;
	NBLK *n;
	char pathname[MAX_PATHLEN];

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

	/* check if current directory still exists */

	dirtree_to_dirname(gbl(scr_cur)->cur_dir_tree, pathname);
	if (is_it_a_dir(pathname, 0))
	{
		/* don't exist no mo */
		zap_dir(bt);
		recurse = FALSE;
	}
	else
	{
		bt = log_dir(n, bt);
	}

	if (! gbl(scr_cur)->in_dest_dir &&
	    gbl(scr_cur)->command_mode == m_dir && ! gbl(scr_is_split))
	{
		disp_drive_info((gbl(scr_cur)->cur_dir)->stbuf.st_dev, 0);
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
			b = log_dir(n, b);
			if (gbl(scr_cur)->command_mode == m_dir)
			{
				d = (DBLK *)tid(t);
				disp_drive_info(d->stbuf.st_dev, 0);
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

	if (gbl(scr_cur)->command_mode != m_dir && ! gbl(scr_cur)->in_dest_dir)
	{
		if (!(gbl(scr_cur)->cur_dir)->mlist[gbl(scr_cur_no)])
		{
			gbl(scr_cur)->in_small_window = FALSE;
			gbl(scr_cur)->command_mode = m_dir;
		}
		else
		{
			gbl(scr_cur)->base_file   = (gbl(scr_cur)->cur_dir)->flist;
			gbl(scr_cur)->file_cursor =
				(gbl(scr_cur)->cur_dir)->mlist[gbl(scr_cur_no)];
			gbl(scr_cur)->cur_file    = (FBLK *)bid(gbl(scr_cur)->file_cursor);
			gbl(scr_cur)->first_file  = gbl(scr_cur)->base_file;
		}
	}

	if (gbl(scr_cur)->command_mode == m_dir)
	{
		disp_dir_tree();
		do_dir_scroll_bar();
		hilite_dir(ON);
		if (! gbl(scr_cur)->in_dest_dir)
		{
			disp_drive_info((gbl(scr_cur)->cur_dir)->stbuf.st_dev, 1);
			disp_node_stats(gbl(scr_cur)->cur_root);
			disp_cur_dir();
		}
	}
	else
	{
		if (gbl(scr_cur)->in_small_window)
		{
			disp_dir_tree();
			do_dir_scroll_bar();
			if (! gbl(scr_cur)->in_dest_dir)
			{
				hilite_dir(OFF);
				left_arrow(gbl(scr_cur)->cur_dir_win);
				wrefresh(gbl(scr_cur)->cur_dir_win);
			}
			else
			{
				hilite_dir(ON);
			}
		}
		disp_drive_info((gbl(scr_cur)->cur_dir)->stbuf.st_dev, 1);
		disp_cur_file();
		setup_dir_stats();
		disp_dir_stats();
	}

	if (! gbl(scr_cur)->in_dest_dir)
	{
		start_file_display();
		if (gbl(scr_cur)->command_mode != m_dir)
		{
			hilite_file(ON);
		}
	}
}

void unlog_dir (void)
{
	NBLK *n;
	int i;
	BLIST *bt;

	n = get_root_of_dir(gbl(scr_cur)->cur_dir_tree);
	if (n->dir_disp_list[gbl(scr_cur_no)])
		bt = bfind(n->dir_list, gbl(scr_cur)->cur_dir_tree);
	else
		bt = gbl(scr_cur)->dir_cursor;
	zap_dir(bt);
	for (i=0; i<gbl(scr_cur_count); i++)
	{
		if (n->dir_disp_list[i])
			hide_dirs_in_node(n, i);
	}

	/* set pointers & counters */

	if (n->dir_disp_list[gbl(scr_cur_no)])
		gbl(scr_cur)->first_dir = n->dir_disp_list[gbl(scr_cur_no)];
	else
		gbl(scr_cur)->first_dir = n->dir_list;
	gbl(scr_cur)->dir_cursor =
		bfind(gbl(scr_cur)->first_dir, gbl(scr_cur)->cur_dir_tree);

	/* note that cur_dir_no, cur_dir_line shouldn't change */
	if (!gbl(scr_cur)->dir_scroll)
	{
		gbl(scr_cur)->top_dir = gbl(scr_cur)->dir_cursor;
		for (i=gbl(scr_cur)->cur_dir_line; i; i--)
			gbl(scr_cur)->top_dir = bprev(gbl(scr_cur)->top_dir);
	}
	gbl(scr_cur)->numdirs = bcount(gbl(scr_cur)->first_dir);

	disp_dir_tree();
	hilite_dir(ON);

	if (! gbl(scr_cur)->in_dest_dir)
	{
		disp_node_stats(gbl(scr_cur)->cur_root);
		disp_cur_dir();
		start_file_display();
	}
}

void zap_dir (BLIST *b)
{
	BLIST *bn;
	BLIST *bt;
	TREE *t;
	TREE *tt;
	NBLK *n;
	DBLK *d;
	int l;

	t = (TREE *)bid(b);
	n = get_root_of_dir(t);
	l = tdepth(t);
	d = (DBLK *)tid(t);
	clean_dir(t, n, FALSE);
	d->dir_size = 0;
	/* get last dir ptr in sub list */
	bt = b;
	for (bn=bnext(b); bn; bn=bnext(bn))
	{
		tt = (TREE *)bid(bn);
		if (tdepth(tt) <= l)
			break;
		bt = bn;
	}
	/* loop backwards thru list zapping dirs */
	for (; bt!=b; bt=bn)
	{
		bn = bprev(bt);
		tt = (TREE *)bid(bt);
		clean_dir(tt, n, FALSE);
		del_dir_from_node(bt, n);
	}
	d->flags = D_NOT_LOGGED;
}

void check_invalid_dir (void)
{
	if ((gbl(scr_cur)->cur_dir)->flags & D_NOT_LOGGED)
	{
		if (!((gbl(scr_cur)->cur_dir)->flags & D_CANT_LOG))
			relog_dir(FALSE);
	}
	else
	{
		switch (opt(autolog))
		{
		case auto_disabled:
			break;

		case auto_check:
			if (is_curdir_invalid())
			{
				(gbl(scr_cur)->cur_dir)->flags |= D_CHANGED;
				hilite_dir(ON);
			}
			break;

		case auto_log:
			if (is_curdir_invalid())
			{
				relog_dir(FALSE);
			}
			break;
		}
	}
}

TREE * sort_dirs (TREE *t)
{
	TREE *tn;

	t = tsortr(t, compare_dirs);

	for (tn=t; tn; tn=tright(tn))
	{
		DBLK *d = (DBLK *)tid(tn);

		d->dir_tree = tn;
	}

	return (t);
}
