/*------------------------------------------------------------------------
 * routines to check a directory
 */
#include "libprd.h"

static TREE *add_path_to_node (const char *path, NBLK *n, int *new_dir_added)
{
	char fullpath[MAX_PATHLEN];
	TREE *t;
	TREE *tn;
	DBLK *d;
	char *p;
	int i;
	struct stat stbuf;
	char dir_part[MAX_FILELEN];

	*new_dir_added = FALSE;

	/* check if path is an absolute name */

	if (!fn_is_path_absolute(path))
		return (0);

	/*
	 * check if node can contain path &
	 * set p to point to part of path past root
	 */

	if (strcmp(n->root_name, path) == 0)
		return (n->root);

	if (!fn_is_dir_in_dirname(n->root_name, path))
		return (0);

	i = fn_num_subdirs(n->root_name);
	t = n->root;
	strcpy(fullpath, n->root_name);
	while (TRUE)
	{
		/*	loop thru siblings looking for next dir portion */

		p = fn_get_nth_dir(path, i, dir_part);
		if (!p)
			return (t);

		for (tn=tleft(t); tn; tn=tright(tn))
		{
			d = (DBLK *)tid(tn);
			if (strcmp(p, FULLNAME(d)) == 0)
			{
				if (d->flags & D_NOT_LOGGED)
					return (0);
				break;
			}
		}
		if (!tn)
			break;
		fn_append_dirname_to_dir(fullpath, p);
		t = tn;
		i++;
	}

	/* t points to parent of sub-path p */

	while (TRUE)
	{
		p = fn_get_nth_dir(path, i++, dir_part);
		if (!p)
			break;
		fn_append_dirname_to_dir(fullpath, p);
		if (os_stat(fullpath, &stbuf) == -1)
			return (0);
		if (! S_ISDIR(stbuf.st_mode))
			return (0);
		d = fs_make_dblk(p, &stbuf);
		if (d == 0)
			return (0);
		d->flags = 0;
		tn = TNEW(d);
		if (tn == 0)
		{
			dblk_free(d);
			return (0);
		}
		d->dir_tree = tn;
		add_dir_to_parent(tn, t);
		*new_dir_added = TRUE;
		t = tn;
	}

	return (t);
}

int chk_dir_in_tree (const char *path)
{
	NBLK *n;
	TREE *t;
	BLIST *b;
	int new_dir_added;

	n = 0;				/* shut up cc */
	for (b=gbl(nodes_list); b; b=bnext(b))
	{
		n = (NBLK *)bid(b);
		if (n->node_type != N_FS)
			continue;
		if (fn_is_dir_in_dirname(n->root_name, path))
			break;
	}
	if (!b)
		return (-1);
	t = add_path_to_node(path, n, &new_dir_added);
	if (!t)
		return (-1);
	if (new_dir_added)
		return (1);
	return (0);
}
