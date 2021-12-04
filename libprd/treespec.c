/*------------------------------------------------------------------------
 * process the "treespec" cmd
 */
#include "libprd.h"

static void update_display(TREE *t)
{
	gbl(scr_cur)->cur_dir_tree = t;
	gbl(scr_cur)->cur_dir = (DBLK *)tid(t);
	set_top_dir();
	disp_dir_tree();
	do_dir_scroll_bar();
	hilite_dir(ON);
	// TODO add support for destination selection mode
	dirtree_to_dirname((gbl(scr_cur)->cur_dir_tree), gbl(scr_cur)->path_name);
	disp_path_line();
	disp_cur_dir();
	check_small_window();
	disp_drive_info((gbl(scr_cur)->cur_dir)->stbuf.st_dev, 0);
	disp_node_stats(gbl(scr_cur)->cur_root);
}

static TREE *log_path(const char *path)
{
	char fullpath[MAX_PATHLEN];
	BLIST *bsel;
	BLIST *b;
	NBLK *n;
	TREE *t;
	TREE *tn;
	DBLK *d;
	char *p;
	int i;
	char dir_part[MAX_FILELEN];

	/* check if path is an absolute name */

	if (!fn_is_path_absolute(path))
		return (0);

	// find appropriate node (look for node with longest path match)

	bsel = (0);
	for (b = gbl(nodes_list); b; b = bnext(b))
	{
		n = (NBLK *)bid(b);
		if (n->node_type != N_FS || !fn_is_dir_in_dirname(n->root_name, path))
			continue;
		if (bsel == (0) || strlen(n->root_name) > strlen(((NBLK *)bid(bsel))->root_name))
			bsel = b;
	}
	if (!bsel)
		return (0);

	// switch node

	if (gbl(scr_cur)->cur_node != bsel)
	{
		gbl(scr_cur)->cur_node = bsel;
		setup_node();
	}

	// log root

	n = (NBLK *)bid(bsel);
	t = n->root;
	d = (DBLK *)tid(t);
	if (d->flags & D_NOT_LOGGED)
		log_dir(n, bfind(n->dir_list, t));
	update_display(t);

	i = fn_num_subdirs(n->root_name);
	strcpy(fullpath, n->root_name);
	while (TRUE)
	{
		/*	loop thru siblings looking for next dir portion */

		p = fn_get_nth_dir(path, i, dir_part);
		if (!p)
			return (t);

		for (tn = tleft(t); tn; tn = tright(tn))
		{
			d = (DBLK *)tid(tn);
			if (strcmp(p, FULLNAME(d)) == 0)
				break;
		}
		if (!tn)
			break;

		if (d->flags & D_NOT_LOGGED)
			log_dir(n, bfind(n->dir_list, tn));

		fn_append_dirname_to_dir(fullpath, p);
		update_display(tn);

		t = tn;
		i++;
	}

	return tn;
}

void treespec(void)
{
	char input_str[MAX_PATHLEN];
	char line[MAX_PATHLEN];
	BLIST *b;
	TREE *t;
	DBLK *d;
	NBLK *n;
	int c;

	bang(msgs(m_treespec_prompt));

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_treespec_ts));

	strcpy(input_str, gbl(scr_cur)->path_name);
	c = xgetstr(gbl(win_commands), input_str, XGL_TREESPEC, MAX_PATHLEN,
				0, XG_FILEPATH);
	if (c < 0)
		return;

	// TODO try direct "jump" if node is already logged
	t = log_path(input_str);
	if (!t)
		return;

	update_display(t);
}
