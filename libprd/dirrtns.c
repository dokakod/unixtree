/*------------------------------------------------------------------------
 * directory routines
 */
#include "libprd.h"

void do_make_dir (void)
{
	char input_str[MAX_FILELEN];
	int i;
	char *p, *q;
	TREE *t;
	DBLK *d;
	BLIST *b;
	NBLK *n;
	char new_name[MAX_PATHLEN];
	char new_dirname[MAX_FILELEN];

	bang(msgs(m_dirrtns_entdir));
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	p = msgs(m_dirrtns_maksub);
	q = msgs(m_dirrtns_as2);
	xaddstr(gbl(win_commands), p);
	zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_dir));
	wmove(gbl(win_commands), 1, display_len(p)-display_len(q));
	waddstr(gbl(win_commands), q);
	wrefresh(gbl(win_commands));
	*input_str = 0;
	i = xgetstr(gbl(win_commands), input_str, XGL_NEWNAME, MAX_FILELEN, 0,
		XG_FILENAME);
	if (i <= 0)
		return;
	make_file_name(input_str, FULLNAME(gbl(scr_cur)->cur_dir), new_dirname);
	strcpy(new_name, gbl(scr_cur)->path_name);
	fn_append_dirname_to_dir(new_name, new_dirname);
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_dirrtns_makdir));
	waddstr(gbl(win_commands), new_dirname);
	wrefresh(gbl(win_commands));

	/* create new dir */

	i = os_dir_make(new_name);
	if (i == -1)
	{
		errsys(ER_CMD);
		return;
	}

	/* check if already in tree */

	t = pathname_to_dirtree(new_name);
	if (t)
	{
		if (i == 0)
		{
			n = get_root_of_dir(t);
			b = bfind(n->dir_list, t);
			zap_dir(b);
			d = (DBLK *)tid(t);
			d->flags = 0;
		}
	}
	else
	{
		t = add_dir(new_dirname, new_name, gbl(scr_cur)->cur_dir_tree);
		if (i == 1)
		{
			d = (DBLK *)tid(t);
			d->flags |= D_NOT_LOGGED;
		}
	}
	set_top_dir();
	disp_dir_tree();
	hilite_dir(ON);
}

void set_top_dir (void)
{
	NBLK *n;
	int i;

	n = gbl(scr_cur)->cur_root;
	if (n->dir_disp_list[gbl(scr_cur_no)])
		gbl(scr_cur)->first_dir = n->dir_disp_list[gbl(scr_cur_no)];
	else
		gbl(scr_cur)->first_dir = n->dir_list;
	gbl(scr_cur)->dir_cursor =
		bfind(gbl(scr_cur)->first_dir, gbl(scr_cur)->cur_dir_tree);
	gbl(scr_cur)->cur_dir_no =
		bindex(gbl(scr_cur)->first_dir, gbl(scr_cur)->dir_cursor);
	gbl(scr_cur)->numdirs    = bcount(gbl(scr_cur)->first_dir);
	if (!gbl(scr_cur)->dir_scroll)
	{
		if (gbl(scr_cur)->cur_dir_line > gbl(scr_cur)->cur_dir_no)
			gbl(scr_cur)->cur_dir_line = gbl(scr_cur)->cur_dir_no;
		else if (gbl(scr_cur)->cur_dir_line < 0)
			gbl(scr_cur)->cur_dir_line = 0;

		gbl(scr_cur)->top_dir = gbl(scr_cur)->dir_cursor;
		for (i=gbl(scr_cur)->cur_dir_line; i; i--)
			gbl(scr_cur)->top_dir = bprev(gbl(scr_cur)->top_dir);
	}
}

TREE *add_dir (const char *name, const char *fullpath, TREE *p)
{
	struct stat stbuf;
	NBLK *n;
	DBLK *d;
	TREE *t;

	if (os_stat(fullpath, &stbuf))
		return (0);

	/* check if dir already in tree */

	for (t=tleft(p); t; t=tright(t))
	{
		d = (DBLK *)tid(t);
		if (strcmp(name, FULLNAME(d)) == 0)
			return (t);
	}

	/* make new DBLK for new directory */

	d = fs_make_dblk(name, &stbuf);
	if (!d)
	{
		errmsg(ER_IM, "", ERR_ANY);
		return (0);
	}
	d->flags = 0;		/* delete NOT_LOGGED flag */

	/* make tree block for new directory */

	t = TNEW(d);
	if (!t)
	{
		dblk_free(d);
		errmsg(ER_IM, "", ERR_ANY);
		return (0);
	}
	d->dir_tree = t;

	/* hook new tree entry into tree structure */

	add_dir_to_parent(t, p);

	/* update dir lists in node block */

	n = get_root_of_dir(p);
	fix_dir_list(n);

	return (t);
}

void fix_dir_list (NBLK *n)
{
	int i;

	make_dir_list(n);
	for (i=0; i<gbl(scr_cur_count); i++)
		if (n->dir_disp_list[i])
			hide_dirs_in_node(n, i);
}

void add_dir_to_parent (TREE *t, TREE *p)
{
	tparent(t) = p;
	tleft(p) = tinsertrf(tleft(p), t, compare_dirs);
}

void do_delete_dir (void)
{
	NBLK *n;

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_dirrtns_delsub));
	zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_dir));
	wrefresh(gbl(win_commands));

	/* check if top dir in node */

	if (gbl(scr_cur)->dir_cursor == gbl(scr_cur)->first_dir)
	{
		errmsg(ER_CDRN, "", ERR_ANY);
		return;
	}

	/* check if directory is empty */

	if (tleft(gbl(scr_cur)->cur_dir_tree) || (gbl(scr_cur)->cur_dir)->flist)
	{
		errmsg(ER_DNE, "", ERR_ANY);
		return;
	}

	/* check if directory is logged */

	if ((gbl(scr_cur)->cur_dir)->flags & D_NOT_LOGGED ||
		(gbl(scr_cur)->cur_dir)->flags & D_CANT_LOG)
	{
		errmsg(ER_DNL, "", ERR_ANY);
		return;
	}

	if (opt(prompt_for_delete))
	{
		if (yesno_msg(msgs(m_dirrtns_deldir)))
			return;
	}
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_dirrtns_del));
	zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_dir));
	wrefresh(gbl(win_commands));

	/* delete from file system */

	if (os_dir_delete(gbl(scr_cur)->path_name))
	{
		errsys(ER_CDD);
		return;
	}

	/* delete from structure */

	hilite_dir(OFF);			/* turn it off before it's gone */

	n = gbl(scr_cur)->cur_root;
	del_dir_from_node(gbl(scr_cur)->dir_cursor, n);

	/* fix display */

	if (n->dir_disp_list[gbl(scr_cur_no)])
		gbl(scr_cur)->first_dir = n->dir_disp_list[gbl(scr_cur_no)];
	else
		gbl(scr_cur)->first_dir = n->dir_list;
	gbl(scr_cur)->cur_dir_no--;
	gbl(scr_cur)->dir_cursor =
		bnth(gbl(scr_cur)->first_dir, gbl(scr_cur)->cur_dir_no);
	gbl(scr_cur)->numdirs--;
	gbl(scr_cur)->cur_dir_tree = (TREE *)bid(gbl(scr_cur)->dir_cursor);
	gbl(scr_cur)->cur_dir = (DBLK *)tid(gbl(scr_cur)->cur_dir_tree);
	if (!gbl(scr_cur)->dir_scroll)
	{
		if (gbl(scr_cur)->cur_dir_line)
			gbl(scr_cur)->cur_dir_line--;
		else
			gbl(scr_cur)->top_dir = gbl(scr_cur)->dir_cursor;
	}
	disp_dir_tree();
	disp_drive_info((gbl(scr_cur)->cur_dir)->stbuf.st_dev, 1);
}

void del_dir_from_node (BLIST *b, NBLK *n)
{
	TREE *t;
	TREE *p;
	DBLK *d;
	int i;

	t = (TREE *)bid(b);
	d = (DBLK *)tid(t);
	/* note - the flist & mlists better be empty!!! */
	dblk_free(d);
	p = tparent(t);			/* parent */
	tleft(p) = TDELETER(tleft(p), t, FALSE);	/* delete t from sib list */
	n->dir_list = BDELETE(n->dir_list, b, FALSE);
	for (i=0; i<gbl(scr_cur_count); i++)
	{
		if (n->dir_disp_list[i])
		{
			n->dir_disp_list[i] = BDELETE(n->dir_disp_list[i],
				bfind(n->dir_disp_list[i], t), FALSE);
		}
	}
}

void do_rename_dir (void)
{
	char new_name[MAX_PATHLEN];
	char new_dirname[MAX_FILELEN];
	char input_str[MAX_FILELEN];
	int i;
	char *p, *q;

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	p = msgs(m_dirrtns_rendir);
	q = msgs(m_dirrtns_as2);
	xaddstr(gbl(win_commands), p);
	zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_dir));
	wrefresh(gbl(win_commands));
	if (gbl(scr_cur)->dir_cursor == gbl(scr_cur)->first_dir)
	{
		errmsg(ER_CRRN, "", ERR_ANY);
		return;
	}

	bang(msgs(m_dirrtns_entdir));
	wmove(gbl(win_commands), 1, display_len(p)-display_len(q));
	waddstr(gbl(win_commands), q);
	wrefresh(gbl(win_commands));
	strcpy(input_str, FULLNAME(gbl(scr_cur)->cur_dir));
	i = xgetstr(gbl(win_commands), input_str, XGL_RENAME, MAX_FILELEN, 0,
		XG_FILENAME);
	if (i <= 0)
		return;

	make_file_name(input_str, FULLNAME(gbl(scr_cur)->cur_dir), new_dirname);
	strcpy(new_name, gbl(scr_cur)->path_name);
	strcpy(fn_basename(new_name), new_dirname);
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_dirrtns_ren));
	zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_dir));
	waddstr(gbl(win_commands), msgs(m_dirrtns_as1));
	waddstr(gbl(win_commands), new_dirname);
	wrefresh(gbl(win_commands));
	if  (strcmp(gbl(scr_cur)->path_name, new_name) == 0)
	{
		errmsg(ER_CRSN, "", ERR_ANY);
		return;
	}
	if (os_dir_rename(gbl(scr_cur)->path_name, new_name))
	{
		errsys(ER_CRD);
		return;
	}

	strcpy(gbl(scr_cur)->path_name, new_name);
	disp_path_line();
	dblk_change_name(gbl(scr_cur)->cur_dir, new_dirname);
	disp_cur_dir();
	hilite_dir(ON);
}

/* low level dir routines */

int make_dir_path (const char *path)
{
	char fullname[MAX_PATHLEN];
	char dir_part[MAX_FILELEN];
	char *p;
	int i;
	int n;

	if (!fn_is_path_absolute(path))
		return (-1);

	fn_rootname(path, fullname);
	n = 0;
	while (TRUE)
	{
		p = fn_get_nth_dir(path, n++, dir_part);
		if (!p)
			break;
		fn_append_dirname_to_dir(fullname, p);
		i = is_it_a_dir(fullname, 0);
		if (i == 1)
		{
			return (-1);
		}
		else if (i == -1)
		{
			if (os_dir_make(fullname) == -1)
			{
				errsys(ER_CMD);
				return (-1);
			}
		}
	}

	return (0);
}
