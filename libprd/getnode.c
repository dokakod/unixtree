/*------------------------------------------------------------------------
 *	Routine to get a node structure from a file
 */
#include "libprd.h"

static void get_stbuf (FILE *fp, struct stat *s)
{
	s->st_dev   = get_4byte(fp);
	s->st_ino   = get_4byte(fp);
	s->st_mode  = get_4byte(fp);
	s->st_nlink = get_4byte(fp);
	s->st_uid   = get_4byte(fp);
	s->st_gid   = get_4byte(fp);
	set_rdev(s, (int)get_4byte(fp));
	s->st_size  = get_4byte(fp);
	s->st_atime = get_4time(fp);
	s->st_mtime = get_4time(fp);
	s->st_ctime = get_4time(fp);

	check_owner(s->st_uid);
	check_group(s->st_gid);
}

static NBLK *get_node_from_fp (FILE *fp)
{
	struct stat stbuf;
	NBLK *n;
	BLIST *db;
	BLIST *fb;
	BLIST *ft;
	BLIST *sb;
	BLIST *sl;
	TREE *t;
	TREE *parent;
	DBLK *d;
	FBLK *f;
	BLIST *ln;
	ABLK *a;
	char name[MAX_FILELEN];
	int num_dirs;
	int num_files;
	long file_size;
	int l;
	int i;
	int j;
	int k;
	int cur_lev = 0;
	int dirno;
	long magic;
	struct dir_ptr
	{
		TREE	*tptr;
		BLIST 	*dptr;
	} *tlist;
	int dev;

	/* read magic number & check if OK */

	magic = get_4byte(fp);
	if (feof(fp))
		return (0);
	if (magic != N_MAGIC)
	{
		return (0);
	}

	/* get sort type & order */

	opt(sort_type)  = get_4byte(fp);
	opt(sort_order) = get_4byte(fp);

	/* create node */

	n = nblk_make();
	if (n == 0)
		return (0);

	/* node info */

	fread(n->root_name, sizeof(n->root_name), 1, fp);
	fread(n->node_name, sizeof(n->node_name), 1, fp);
	n->node_type  = get_4byte(fp);
	n->node_flags = get_4byte(fp);

	num_dirs      = get_4byte(fp);
	num_files     = get_4byte(fp);
	file_size     = get_4byte(fp);

	/* node sub-blk if present */

	switch (n->node_type)
	{
	case N_FS:
		break;

	case N_ARCH:
		a = ablk_make();
		if (a == 0)
		{
			nblk_free(n);
			return (0);
		}
		fread(a->arch_name, sizeof(a->arch_name), 1, fp);
		fread(a->arch_devname, sizeof(a->arch_devname), 1, fp);
		*a->arch_tmpname = 0;
		a->arch_type      = get_4byte(fp);
		a->arch_dev_type  = get_4byte(fp);
		a->arch_blkfactor = get_4byte(fp);
		a->arch_volsize   = get_4byte(fp);
		a->arch_size      = get_4byte(fp);
		a->arch_numvols   = get_4byte(fp);
		n->node_sub_blk = (void *)a;
		break;

	case N_FTP:
		break;
	}

	/* stat root name */

	dev = 0;
	if (n->node_type == N_FS)
	{
		if (os_stat(n->root_name, &stbuf))
		{
			errmsg(ER_NDE, "", ERR_ANY);
			n->node_flags |= N_READONLY;
		}
		else
		{
			dev = stbuf.st_dev;
		}
	}

	/* allocate tree pointer array */

	tlist = (struct dir_ptr *)MALLOC(num_dirs * sizeof(struct dir_ptr));

	/* setup display */

	gbl(scr_cur)->cur_root = n;
	strcpy(gbl(scr_cur)->path_name, n->root_name);
	disp_path_line();
	if (!gbl(scr_is_split) && ! opt(wide_screen))
	{
		if (dev)
			disp_drive_info(stbuf.st_dev, 0);
		disp_file_box();
		disp_node_stats(n);
		werase(gbl(win_current));
		wrefresh(gbl(win_current));
	}
#if 0
	werase(gbl(scr_cur)->cur_dir_win);
	wrefresh(gbl(scr_cur)->cur_dir_win);
	headline();
	disp_credits();
#endif

	/* read in dirs */

	parent = 0;
	k = 0;
	for (i=0; i<num_dirs; i++)
	{
		d = dblk_make();
		if (d == 0)
			break;

		t = TNEW(d);
		if (t == 0)
		{
			dblk_free(d);
			break;
		}
		d->dir_tree = t;

		tlist[k].tptr = t;
		tlist[k].dptr = 0;
		k++;

		db = BNEW(t);
		if (db == 0)
		{
			dblk_free(d);
			TFREE(t, FALSE);
			break;
		}

		fread(name, sizeof(name), 1, fp);
		dblk_set_name(d, name);

		l = get_4byte(fp);
		get_stbuf(fp, &d->stbuf);
		d->log_time = get_4time(fp);
		d->dir_size = get_4byte(fp);
		d->flags    = get_4byte(fp);
		d->hidden   = 0;
		d->flist    = 0;
		for (j=0; j<gbl(scr_cur_count); j++)
			d->mlist[j] = 0;

		n->dir_list = bappend(n->dir_list, db);

		if (!l)
		{
			n->root = t;
			cur_lev = 0;
			parent = t;
		}
		else
		{
			if (l == cur_lev)
			{
				tleft(parent) = tappendr(tleft(parent), t);
			}
			else if (l > cur_lev)
			{
				parent = (TREE *)bid(bprev(db));
				tleft(parent) = t;
				cur_lev++;
			}
			else /* (l < cur_lev) */
			{
				for (; cur_lev>l; cur_lev--)
					parent = tparent(parent);
				tleft(parent) = tappendr(tleft(parent), t);
			}
			tparent(t) = parent;
		}
	}

	/* read file blocks */

	sl = 0;
	for (i=0; i<num_files; i++)
	{
		if ((i % 100) == 0 && !gbl(scr_is_split) && ! opt(wide_screen))
		{
			char dbuf[12];

			wmove(gbl(win_stats), 2, 8);
			wstandout(gbl(win_stats));
			waddstr(gbl(win_stats), xform(dbuf, i));
			wstandend(gbl(win_stats));
			wrefresh(gbl(win_stats));
		}

		f = fblk_make();
		if (f == 0)
			break;

		fb = BNEW(f);
		if (fb == 0)
		{
			fblk_free(f);
			break;
		}

		sb = BNEW(f);
		if (sb == 0)
		{
			fblk_free(f);
			BFREE(fb, FALSE);
			break;
		}

		fread(name, sizeof(name), 1, fp);
		fblk_set_name(f, name);

		dirno = get_4byte(fp);
		t = tlist[dirno].tptr;
		d = (DBLK *)tid(t);

		get_stbuf(fp, &f->stbuf);
		f->archive_vol = -1;
		f->archive_loc = -1;
		f->tagged = 0;
		f->dir = d;
		f->sym_name = 0;
		f->sym_mode = 0;

		/* add file to dir file list */

		ft = tlist[dirno].dptr;
		if (!ft)
		{
			d->flist = fb;
		}
		else
		{
			bnext(ft) = fb;
			bprev(fb) = ft;
		}
		tlist[dirno].dptr = fb;
		for (j=0; j<gbl(scr_cur_count); j++)
			d->mlist[j] = d->flist;

		/* add file to showall file list */

		if (!sl)
		{
			n->showall_flist = sb;
		}
		else
		{
			bnext(sl) = sb;
			bprev(sb) = sl;
		}
		sl = sb;

		/* if link, add to link list */

		if ((n->node_type == N_FS && f->stbuf.st_nlink > 1) ||
			(n->node_type != N_FS && f->stbuf.st_nlink > 2))
		{
			ln = BNEW(f);
			if (ln)
			{
				/* prepend link to list */
				bnext(ln) = n->link_list;
				n->link_list = ln;
			}
		}
	}

	for (j=0; j<gbl(scr_cur_count); j++)
	{
		n->showall_mlist[j] = n->showall_flist;
		gbl(scr_stat)[j].file_spec_pres = FALSE;
		strcpy(gbl(scr_stat)[j].file_spec, fn_all());
	}

	n->node_total_count = num_files;
	n->node_total_bytes = file_size;
	n->node_match_count = num_files;
	n->node_match_bytes = file_size;

	FREE(tlist);

	return (n);
}

void do_load_node (void)
{
	int c;
	struct stat stbuf;
	char input_str[MAX_PATHLEN];
	FILE *fp;
	NBLK *n;
	BLIST *b;
	char node_path[MAX_PATHLEN];

	bang(msgs(m_getnode_bang));
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_getnode_load));
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
	if (c != 0)
	{
		errmsg(ER_FDNE, "", ERR_ANY);
		disp_cmds();
		return;
	}
	if (! S_ISREG(stbuf.st_mode))
	{
		errmsg(ER_COF, "", ERR_ANY);
		disp_cmds();
		return;
	}

	fp = fopen(node_path, "rb");
	if (!fp)
	{
		errmsg(ER_COF, "", ERR_ANY);
		disp_cmds();
		return;
	}

	b = 0;
	while (TRUE)
	{
		n = get_node_from_fp(fp);
		if (n == 0)
			break;
		b = BNEW(n);
		if (b)
		{
			gbl(nodes_list) = bappend(gbl(nodes_list), b);
		}
	}
	fclose(fp);

	if (b)
	{
		gbl(scr_cur)->cur_node = b;
		init_disp();
	}

	disp_cmds();
}

int getnode (void)
{
	char filename[MAX_FILELEN];
	char fullpath[MAX_PATHLEN];
	const char *p;
	FILE *fp;
	BLIST *b;
	NBLK *n;

	strcpy(filename, pgmi(m_pgm_program));
	fn_set_ext(filename, pgm_const(ckp_ext));

	p = cmdopt(ckp_path);
	if (p == 0 || *p == 0)
		p = filename;

	p = os_get_path(p, gbl(pgm_path), fullpath);
	if (p == 0)
		return (-1);

	fp = fopen(p, "rb");
	if (fp == 0)
		return (-1);

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_getnode_res));
	wrefresh(gbl(win_commands));

	while (TRUE)
	{
		n = get_node_from_fp(fp);
		if (n == 0)
			break;
		b = BNEW(n);
		if (b)
		{
			gbl(nodes_list) = bappend(gbl(nodes_list), b);
		}
	}

	fclose(fp);
	return (0);
}
