/*------------------------------------------------------------------------
 * make a dir list from a dir tree
 */
#include "libprd.h"

struct mdl
{
	BLIST *b;
	BLIST *start;
};

static void p (TREE *node, void *a)
{
	struct mdl *m = (struct mdl *)a;
	BLIST *t;

	t = BNEW(node);
	if (!t)
		return;

	if (m->b)
	{
		bprev(t) = m->b;
		bnext(m->b) = t;
	}
	else
	{
		m->start = t;
	}

	m->b = t;
}

void make_dir_list (NBLK *node_ptr)
{
	struct mdl m;

	node_ptr->dir_list = BSCRAP(node_ptr->dir_list, FALSE);
	m.b     = 0;
	m.start = 0;

	tpreorder(node_ptr->root, p, &m);
	node_ptr->dir_list = m.start;
}
