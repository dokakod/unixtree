/*------------------------------------------------------------------------
 * ftp node utilities
 */
#include "libprd.h"

int close_ftp_node (NBLK *n)
{
	FTP_NODE *f;

	if (n->node_type != N_FTP)
		return (-1);

	f = (FTP_NODE *)n->node_sub_blk;
	if (f)
	{
		ftp_close(f);
	}

	return (0);
}

int delete_ftp_node (NBLK *n)
{
	FTP_NODE *f;

	if (n->node_type != N_FTP)
		return (-1);

	f = (FTP_NODE *)n->node_sub_blk;
	if (f)
	{
		ftp_close(f);
		ftp_free_node(f);
		n->node_sub_blk = 0;
	}

	return (0);
}

static NBLK *create_ftp_node (char *host, char *user, char *passwd,
	void (*logmsg)(const char *msg), int (*errmsg)(const char *msg))
{
	NBLK *n;
	FTP_NODE *f;

	n = nblk_make();
	if (n == 0)
		return (0);
	n->node_type = N_FTP;

	f = ftp_make_node(host, user, passwd, logmsg, errmsg);
	if (f == (FTP_NODE *)NULL)
	{
		del_node(n);
		return (0);
	}

	strcpy(n->node_name, host);

	if (ftp_open(f, opt(ftp_timeout), opt(ftp_debug)))
	{
		del_node(n);
		return (0);
	}

	n->node_sub_blk = (void *)f;

	return (n);
}

NBLK *init_ftp_node (char *host, char *user, char *passwd,
	void (*logmsg)(const char *msg), int (*errmsg)(const char *msg))
{
	NBLK *n;
	TREE *root;
	DBLK *d;
	BLIST *b;

	n = create_ftp_node(host, user, passwd, logmsg, errmsg);
	if (n == 0)
		return (0);

	fn_rootname(NULL, n->root_name);

	d = make_phony_ftp_dblk(n->root_name);
	if (! d)
	{
		return (0);
	}
	d->flags = D_NOT_LOGGED;

	root = TNEW(d);
	if (! root)
	{
		return (0);
	}
	d->dir_tree = root;
	n->root = root;

	b = BNEW(root);
	if (b == 0)
	{
		return (0);
	}
	n->dir_list = b;


	b = BNEW(n);
	if (! b)
	{
		return (0);
	}

	gbl(nodes_list) = bappend(gbl(nodes_list), b);
	gbl(scr_cur)->cur_node = b;

	return (n);
}
