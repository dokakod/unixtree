/*------------------------------------------------------------------------
 * process the "compact" cmd
 */
#include "libprd.h"

static int link_dir (BLIST *dptr, char *save_dirname, char *tempname)
{
	BLIST *b;
	TREE *t;
	DBLK *d;
	FBLK *f;
	char *old_n;
	char *new_n;
	char old_name[MAX_PATHLEN];
	char new_name[MAX_PATHLEN];

	dptr_to_dirname(dptr, old_name);
	strcpy(FULLNAME(gbl(scr_cur)->cur_dir), fn_basename(tempname));
	dptr_to_dirname(dptr, new_name);
	strcpy(FULLNAME(gbl(scr_cur)->cur_dir), save_dirname);
	fn_terminate_dirname(old_name);
	fn_terminate_dirname(new_name);
	old_n = old_name+strlen(old_name);
	new_n = new_name+strlen(new_name);
	t = (TREE *)bid(dptr);
	d = (DBLK *)tid(t);
	for (b=d->flist; b; b=bnext(b))
	{
		f = (FBLK *)bid(b);
		strcpy(old_n, FULLNAME(f));
		strcpy(new_n, FULLNAME(f));
		if (os_link(old_name, new_name))
			return (1);
	}
	return (0);
}

static void del_files_in_dir (BLIST *dptr)
{
	BLIST *b;
	TREE *t;
	DBLK *d;
	FBLK *f;
	char name[MAX_PATHLEN];
	char *p;

	dptr_to_dirname(dptr, name);
	fn_terminate_dirname(name);
	p = name+strlen(name);
	t = (TREE *)bid(dptr);
	d = (DBLK *)tid(t);
	for (b=d->flist; b; b=bnext(b))
	{
		f = (FBLK *)bid(b);
		strcpy(p, FULLNAME(f));
		os_file_delete(name);
	}
}

void compact (void)
{
	BLIST *first_dir;
	BLIST *last_dir;
	struct stat stbuf;
	int c;
	int i;
	char dir_name[MAX_PATHLEN];
	char full_name[MAX_PATHLEN];
	BLIST *b;
	TREE *last_dir_tree;
	TREE *t;
	DBLK *d;
	char old_name[MAX_PATHLEN];
	char new_name[MAX_PATHLEN];
	char tempname[MAX_PATHLEN];
	char save_dirname[MAX_PATHLEN];

	if (get_first_last(&first_dir, &last_dir, TRUE))
		goto bail;

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_compact_cdn));
	zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_dir));
	wrefresh(gbl(win_commands));
	if (check_sub_node(FALSE))
		goto bail;
	c = yesno_msg(msgs(m_compact_ctn));
	if (c)
		goto bail;

	/*	create new sub directory tree in parent */

	dirtree_to_dirname(gbl(scr_cur)->cur_dir_tree, full_name);
	os_make_temp_name(tempname, fn_dirname(full_name, dir_name), NULL);
	strcpy(save_dirname, FULLNAME(gbl(scr_cur)->cur_dir));
	strcpy(FULLNAME(gbl(scr_cur)->cur_dir), fn_basename(tempname));
	dirtree_to_dirname(gbl(scr_cur)->cur_dir_tree, full_name);
	if (os_dir_make(full_name) == -1)
		goto bail;
	os_chmod(full_name, (gbl(scr_cur)->cur_dir)->stbuf.st_mode);
	os_chown(full_name, (gbl(scr_cur)->cur_dir)->stbuf.st_uid,
		(gbl(scr_cur)->cur_dir)->stbuf.st_gid);

	/* create copies of all sub-directories underneath */

	if (first_dir != last_dir)
	{
		for (b=bnext(first_dir); b; b=bnext(b))
		{
			t = (TREE *)bid(b);
			dirtree_to_dirname(t, full_name);
			if (os_dir_make(full_name) == -1)
				goto bail;
			d = (DBLK *)tid(t);
			os_chmod(full_name, d->stbuf.st_mode);
			os_chown(full_name, d->stbuf.st_uid, d->stbuf.st_gid);
			if (b == last_dir)
				break;
		}
	}
	strcpy(FULLNAME(gbl(scr_cur)->cur_dir), save_dirname);

	/*	link all files in each directory to new sub node */
	/*	note - that files will be stored in the order displayed */

	for (b=first_dir; ; b=bnext(b))
	{
		link_dir(b, save_dirname, tempname);
		if (b == last_dir)
			break;
	}

	/*	delete all files in original directories & delete dir */
	/*	note - we go backwards so as to empty sub-dirs as we go */

	for (b=last_dir; ; b=bprev(b))
	{
		del_files_in_dir(b);
		dptr_to_dirname(b, old_name);
		os_dir_delete(old_name);
		if (b == first_dir)
			break;
	}

	/*	rename new directory to old name */

	dirtree_to_dirname(gbl(scr_cur)->cur_dir_tree, old_name);
	strcpy(FULLNAME(gbl(scr_cur)->cur_dir), fn_basename(tempname));
	dirtree_to_dirname(gbl(scr_cur)->cur_dir_tree, new_name);
	strcpy(FULLNAME(gbl(scr_cur)->cur_dir), save_dirname);
	os_dir_rename(new_name, old_name);

	/*
	 * Note that at this point all tree structures & directory lists
	 * are all correct.  We haven't changed the tree at all.
	 */

	/*	update actual size of all directories and displays */

	last_dir_tree = (TREE *)bid(last_dir);
	d = 0;
	for (i=gbl(scr_cur)->cur_dir_line, b=gbl(scr_cur)->dir_cursor; ;
		b=bnext(b))
	{
		dptr_to_dirname(b, old_name);
		t = 0;
		if (os_stat(old_name, &stbuf) == 0)
		{
			t = (TREE *)bid(b);
			d = (DBLK *)tid(t);
			d->stbuf.st_size = stbuf.st_size;
			d->stbuf.st_mtime = stbuf.st_mtime;
		}
		if (i <= gbl(scr_cur)->max_dir_line)
		{
			if (gbl(scr_cur)->dir_fmt == fmt_ddate ||
				gbl(scr_cur)->dir_fmt == fmt_dsize)
			{
				wmove(gbl(scr_cur)->cur_dir_win, i, 0);
				disp_dir_line(b);
				wrefresh(gbl(scr_cur)->cur_dir_win);
				i++;
			}
		}
		if (t == last_dir_tree)
			break;
	}

	if (gbl(scr_cur)->dir_fmt == fmt_ddate ||
		gbl(scr_cur)->dir_fmt == fmt_dsize)
	{
		hilite_dir(ON);		/* this assumes the entry is actually recreated */
	}

	if (d)
		disp_drive_info(d->stbuf.st_dev, 1);			/* update free space */

	/* and fall into ... */

bail:
	/* unhilite all sub-directories */

	if (first_dir != 0 && last_dir != 0)
	{
		i = gbl(scr_cur)->cur_dir_line;
		if (first_dir != last_dir)
		{
			for (b=bnext(first_dir); b; b=bnext(b))
			{
				if (++i <= gbl(scr_cur)->max_dir_line)
					hilite_the_dir(OFF, b, i);
				if (b == last_dir)
					break;
			}
		}
	}
}
