/*------------------------------------------------------------------------
 * ftp directory routines
 */
#include "libprd.h"

void ftp_do_make_dir (void)
{
	char new_name[MAX_PATHLEN];
	char new_dirname[MAX_FILELEN];
	char input_str[MAX_FILELEN];
	int i;
	char *p, *q;
	TREE *t;
	DBLK *d;
	BLIST *b;
	NBLK *n;
	FTP_NODE *fn;
	struct stat stbuf;
	char dir_name[MAX_PATHLEN];

	n = gbl(scr_cur)->cur_root;
	fn = (FTP_NODE *)n->node_sub_blk;

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

	i = ftp_make_dir(fn, new_name);
	if (i == -1)
	{
		errmsg(ER_CMD, "", ERR_ANY);
		return;
	}

	ftp_stat(fn, fn_basename(new_name), fn_dirname(new_name, dir_name),
		&stbuf);

	/* check if already in tree */

	t = path_to_tree(new_name, n);
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
		t = ftp_add_dir(new_dirname, &stbuf, gbl(scr_cur)->cur_dir_tree);
		d = (DBLK *)tid(t);
		d->flags |= D_NOT_LOGGED;
	}

	set_top_dir();
	disp_dir_tree();
	hilite_dir(ON);
}

void ftp_do_delete_dir (void)
{
	NBLK *n;
	FTP_NODE *fn;

	n = gbl(scr_cur)->cur_root;
	fn = (FTP_NODE *)n->node_sub_blk;

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

	if (ftp_delete_dir(fn, gbl(scr_cur)->path_name))
	{
		errsys(ER_CDD);
		return;
	}

	/* delete from structure */

	hilite_dir(OFF);			/* turn it off before it's gone */

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
