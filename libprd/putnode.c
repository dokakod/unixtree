/*------------------------------------------------------------------------
 *	Routine to save a node structure to a file
 *
 *	Format of save file:
 *		magic number	4 bytes
 *		sort type		4 bytes
 *		sort order		4 bytes
 *		node block:
 *			rootname 1024 bytes
 *			nodename   64 bytes
 *			node-type	4 bytes
 *			node-flags	4 bytes
 *			num dirs	4 bytes
 *			num files	4 bytes
 *			total size	4 bytes
 *		node sub-block if present:
 *		bar, tar, cpio, ztar:
 *			name       12 bytes
 *			path     1024 bytes
 *			dev-type	4 bytes
 *			blkfactor	4 bytes
 *			vol-size	4 bytes
 *			arch-size	4 bytes
 *			num-vols	4 bytes
 *		dir blocks (num-dirs):
 *			name	  256 bytes
 *			level		4 bytes
 *			stat buf   44 bytes
 *			log time	4 bytes
 *			dir size	4 bytes
 *			flags		4 bytes
 *		file blocks (num-files):
 *			name	  256 bytes
 *			dir-no		4 bytes
 *			stat buf   44 bytes
 *			arch loc	4 bytes
 *
 */
#include "libprd.h"

static void put_stbuf (FILE *fp, struct stat *s)
{
	put_4byte(fp, s->st_dev);
	put_4byte(fp, s->st_ino);
	put_4byte(fp, s->st_mode);
	put_4byte(fp, s->st_nlink);
	put_4byte(fp, s->st_uid);
	put_4byte(fp, s->st_gid);
	put_4byte(fp, get_rdev(s));
	put_4byte(fp, s->st_size);
	put_4time(fp, s->st_atime);
	put_4time(fp, s->st_mtime);
	put_4time(fp, s->st_ctime);
}

static void save_node_to_fp (NBLK *n, FILE *fp)
{
	BLIST *b;
	BLIST *x;
	TREE *t;
	DBLK *d;
	FBLK *f;
	ABLK *a;
	char name[MAX_FILELEN];
	int num_dirs;
	int lev;
	int l;

	/* output magic number to identify ckp file */

	put_4byte(fp, N_MAGIC);
	put_4byte(fp, opt(sort_type));
	put_4byte(fp, opt(sort_order));

	/* node info */

	num_dirs = bcount(n->dir_list);

	fwrite(n->root_name, sizeof(n->root_name), 1, fp);
	fwrite(n->node_name, sizeof(n->node_name), 1, fp);
	put_4byte(fp, n->node_type);
	put_4byte(fp, n->node_flags);
	put_4byte(fp, num_dirs);
	put_4byte(fp, n->node_total_count);
	put_4byte(fp, n->node_total_bytes);

	/* node sub-blk info */

	switch (n->node_type)
	{
	case N_FS:
		break;

	case N_ARCH:
		a = (ABLK *)n->node_sub_blk;
		fwrite(a->arch_name, sizeof(a->arch_name), 1, fp);
		fwrite(a->arch_devname, sizeof(a->arch_devname), 1, fp);
		put_4byte(fp, a->arch_type);
		put_4byte(fp, a->arch_dev_type);
		put_4byte(fp, a->arch_blkfactor);
		put_4byte(fp, a->arch_volsize);
		put_4byte(fp, a->arch_size);
		put_4byte(fp, a->arch_numvols);
		break;

	case N_FTP:
		break;
	}

	/* dir info */

	for (b=n->dir_list; b; b=bnext(b))
	{
		t = (TREE *)bid(b);
		d = (DBLK *)tid(t);
		lev = tdepth(t);

		memset(name, 0, sizeof(name));
		strcpy(name, FULLNAME(d));

		fwrite(name, MAX_FILELEN, 1, fp);
		put_4byte(fp, lev);
		put_stbuf(fp, &d->stbuf);
		put_4time(fp, d->log_time);
		put_4byte(fp, d->dir_size);
		put_4byte(fp, d->flags);
	}

	for (b=n->showall_flist; b; b=bnext(b))
	{
		f = (FBLK *)bid(b);

		memset(name, 0, sizeof(name));
		strcpy(name, FULLNAME(f));
		fwrite(name, MAX_FILELEN, 1, fp);

		t = f->dir->dir_tree;
		l = 0;
		for (l=0, x=n->dir_list; x; l++, x=bnext(x))
		{
			if (bid(x) == (void *)t)
				break;
		}
		put_4byte(fp, l);
		put_stbuf(fp, &f->stbuf);
	}
}

void do_save_node (void)
{
	char node_path[MAX_PATHLEN];
	int c;
	struct stat stbuf;
	char input_str[MAX_PATHLEN];
	const char *open_mode = "wb";
	FILE *fp;

	bang(msgs(m_putnode_bang));
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_putnode_save));
	wrefresh(gbl(win_commands));
	*input_str = 0;
	c = xgetstr(gbl(win_commands), input_str, XGL_NODE_PATH, MAX_PATHLEN, 0,
		XG_FILEPATH);
	if (c <= 0)
	{
		disp_cmds();
		return;
	}
	fn_get_abs_path(gbl(scr_cur)->path_name, input_str, node_path);

	c = os_stat(node_path, &stbuf);
	if (c == 0)
	{
		if (! S_ISREG(stbuf.st_mode))
		{
			errmsg(ER_CWTD, "", ERR_ANY);
			disp_cmds();
			return;
		}
		else
		{
			c = errmsg(ER_FERA, "", ERR_CHAR);
			if (c < 0 ||
				c == cmds(CMDS_NOPE1) || c == cmds(CMDS_NOPE2))
			{
				disp_cmds();
				return;
			}
			if (c == cmds(CMDS_APPEND))
				open_mode = "ab";
			c = -1;
		}
	}

	fp = fopen(node_path, open_mode);
	if (!fp)
	{
		errmsg(ER_COF, "", ERR_ANY);
		disp_cmds();
		return;
	}

	save_node_to_fp(gbl(scr_cur)->cur_root, fp);

	fclose(fp);

	disp_cmds();
}

int putnode (void)
{
	FILE *fp;
	NBLK *n;
	BLIST *b;
	char put_path[MAX_PATHLEN];
	char filename[MAX_FILELEN];

	strcpy(filename, pgmi(m_pgm_program));
	fn_set_ext(filename, pgm_const(ckp_ext));

	strcpy(put_path, gbl(pgm_home));
	fn_append_filename_to_dir(put_path, filename);
	fn_resolve_pathname(put_path);

	if (make_home_dir())
		return (-1);

	fp = fopen(put_path, "wb");
	if (!fp)
		return (-1);
	check_the_file(put_path);

	for (b=gbl(nodes_list); b; b=bnext(b))
	{
		n = (NBLK *)bid(b);
		if (n->node_type == N_FS)
			save_node_to_fp(n, fp);
	}

	fclose(fp);
	return (0);
}
