/*------------------------------------------------------------------------
 * misc ftp utilities
 */
#include "libprd.h"

static int ftp_errmsg (const char *str)
{
	return err_message(str, "", ERR_ANY);
}

static void ftp_logmsg (const char *str)
{
	werase(gbl(win_message));
	wmove(gbl(win_message), 0, 0);
	waddstr(gbl(win_message), str);
	wrefresh(gbl(win_message));
}

static int ftp_get_stbuf (FTP_NODE *fn, FTP_STAT *ftp_stat, void *data)
{
	struct stat *stbuf = (struct stat *)data;

	memcpy(stbuf, &ftp_stat->stbuf, sizeof(*stbuf));
	return (0);
}

static TREE *put_dir_in_ftp_node (NBLK *n, const char *dir)
{
	TREE *t;
	TREE *tn;
	DBLK *d;
	char *p;
	int i;

	/*
	 * note - it is assumed that the root of this node
	 * always is root
	 */

	t = n->root;
	i = 0;
	while (TRUE)
	{
		char dir_part[MAX_FILELEN];

		/*	loop thru siblings looking for next dir portion */

		p = fn_get_nth_dir(dir, i, dir_part);
		if (!p)
			return (t);

		for (tn=tleft(t); tn; tn=tright(tn))
		{
			d = (DBLK *)tid(tn);
			if (strcmp(p, FULLNAME(d)) == 0)
				break;
		}
		if (!tn)
			break;
		t = tn;
		i++;
	}

	/* t points to parent of sub-path p */

	while (TRUE)
	{
		char dir_part[MAX_FILELEN];

		p = fn_get_nth_dir(dir, i++, dir_part);
		if (p == 0)
			break;

		d = make_phony_ftp_dblk(p);
		if (d == 0)
			return (0);
		tn = TNEW(d);
		if (tn == 0)
		{
			dblk_free(d);
			return (0);
		}
		d->dir_tree = tn;
		add_dir_to_parent(tn, t);
		t = tn;
	}
	return (t);
}

char *ftp_host (NBLK *n)
{
	FTP_NODE *f;

	if (n->node_type != N_FTP)
		return (0);

	f = (FTP_NODE *)n->node_sub_blk;

	return (ftp_hostname(f));
}

char *ftp_addr (NBLK *n)
{
	FTP_NODE *f;

	if (n->node_type != N_FTP)
		return (0);

	f = (FTP_NODE *)n->node_sub_blk;

	return (ftp_hostaddr(f));
}

int ftp_get_type (NBLK *n)
{
	FTP_NODE *f;

	if (n->node_type != N_FTP)
		return (0);

	f = (FTP_NODE *)n->node_sub_blk;

	return (ftp_server_type(f));
}

void ftp_setup_node (char *host, char *user, char *passwd, char *rdir)
{
	NBLK *n;
	FTP_NODE *f;
	TREE *t;
	char ftp_dir[MAX_PATHLEN];
	int rc;

	werase(gbl(win_commands));
	wrefresh(gbl(win_commands));
	bang("");

	n = init_ftp_node(host, user, passwd,
		opt(ftp_logging) ? ftp_logmsg : 0, ftp_errmsg);
	bang("");

	if (n == 0)
	{
		disp_cmds();
		return;
	}

	f = (FTP_NODE *)n->node_sub_blk;

	if (rdir != 0 && *rdir != 0)
	{
		rc = ftp_set_cwd(f, rdir);
		if (rc)
		{
			ftp_errmsg("cannot set remote dir");
			goto done;
		}
	}

	rc = ftp_get_cwd(f, ftp_dir);
	if (rc)
	{
		ftp_errmsg("cannot get remote dir");
		goto done;
	}

	t = put_dir_in_ftp_node(n, ftp_dir);
	fix_dir_list(n);

	setup_node();

	strcpy(gbl(scr_cur)->path_name, ftp_dir);
	gbl(scr_cur)->cur_dir_tree = t;
	gbl(scr_cur)->cur_dir = (DBLK *)tid(t);
	gbl(scr_cur)->first_dir = n->dir_list;
	gbl(scr_cur)->top_dir = gbl(scr_cur)->first_dir;
	gbl(scr_cur)->dir_cursor = bfind(gbl(scr_cur)->first_dir, t);
	gbl(scr_cur)->cur_dir_no =
		bindex(gbl(scr_cur)->first_file, gbl(scr_cur)->dir_cursor);
	gbl(scr_cur)->cur_dir_line = gbl(scr_cur)->cur_dir_no;

	disp_path_line();
	disp_drive_info(0, 0);

	relog_ftp_dir(FALSE);

	disp_cmds();
	return;

done:
	init_disp();
}

int ftp_stat (FTP_NODE *fn, const char *file_name, const char *dir_name,
	struct stat *stbuf)
{
	if (ftp_list_file(fn, dir_name, file_name, ftp_get_stbuf, stbuf))
		return (-1);

	return (0);
}

static void make_phony_ftp_xstat (struct stat *xbuf)
{
	xbuf->st_dev   = 0;
	xbuf->st_ino   = 0;
	xbuf->st_mode  = (mode_t)-1;
	xbuf->st_nlink = 1;
	xbuf->st_uid   = 0;
	xbuf->st_gid   = 0;
	set_rdev(xbuf, 0);
	xbuf->st_size  = 0;
	xbuf->st_atime = (time_t) -1;
	xbuf->st_mtime = (time_t) -1;
	xbuf->st_ctime = (time_t) -1;
}

DBLK *make_phony_ftp_dblk (const char *name)
{
	struct stat stbuf;

	make_phony_ftp_xstat(&stbuf);
	return (make_ftp_dblk(name, &stbuf));
}

DBLK *make_ftp_dblk (const char *name, struct stat *stbuf)
{
	char rootname[MAX_PATHLEN];
	DBLK *d;
	int i;

	d = dblk_make();
	if (! d)
		return (0);

	if (*name == 0)
	{
		fn_rootname(NULL, rootname);
		name = rootname;
	}

	dblk_set_name(d, name);
	d->flags = D_NOT_LOGGED;
	d->hidden = 0;
	make_ftp_xstat(stbuf, &d->stbuf);
	d->dir_size = 0;
	d->flist = 0;
	for (i=0; i<gbl(scr_cur_count); i++)
		d->mlist[i] = 0;
	return (d);
}

FBLK *make_ftp_fblk (const char *name, struct stat *stbuf)
{
	FBLK *f;

	f = fblk_make();
	if (! f)
		return (0);
	fblk_set_name(f, name);
	f->tagged = 0;
	make_ftp_xstat(stbuf, &f->stbuf);
	f->dir = 0;
	f->sym_name = 0;
	f->sym_mode = 0;
	return (f);
}

void make_ftp_xstat (struct stat *stbuf, struct stat *xbuf)
{
	xbuf->st_dev   = stbuf->st_dev;
	xbuf->st_ino   = stbuf->st_ino;
	xbuf->st_mode  = stbuf->st_mode;
	xbuf->st_nlink = stbuf->st_nlink;
	xbuf->st_uid   = stbuf->st_uid;
	xbuf->st_gid   = stbuf->st_gid;
	set_rdev(xbuf, get_rdev(stbuf));
	xbuf->st_size  = stbuf->st_size;
	xbuf->st_atime = stbuf->st_atime;
	xbuf->st_mtime = stbuf->st_mtime;
	xbuf->st_ctime = stbuf->st_ctime;
}

char *get_ftp_owner (NBLK *n, int uid)
{
	FTP_NODE *f;
	BLIST *b;
	char *s;

	f = (FTP_NODE *)n->node_sub_blk;

	s = (char *)"unknown";
	if (uid > 0)
	{
		b = bnth(f->owners, uid-1);
		if (b)
			s = (char *)bid(b);
	}

	return (s);
}

char *get_ftp_group (NBLK *n, int gid)
{
	FTP_NODE *f;
	BLIST *b;
	char *s;

	f = (FTP_NODE *)n->node_sub_blk;

	s = (char *)"unknown";
	if (gid > 0)
	{
		b = bnth(f->groups, gid-1);
		if (b)
			s = (char *)bid(b);
	}

	return (s);
}
