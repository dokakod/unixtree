/*------------------------------------------------------------------------
 * process the "filespec" cmd
 */
#include "libprd.h"

static int add_file_spec (void)
{
	int match_count;
	int match_bytes;
	BLIST *b;
	BLIST *x;
	TREE *t;
	NBLK *n;
	DBLK *d;

	if (gbl(scr_cur)->file_spec_pres)
	{
		for (b=gbl(nodes_list); b; b=bnext(b))
		{
			n = (NBLK *)bid(b);
			match_count = 0;
			match_bytes = 0;
			for (x=n->dir_list; x; x=bnext(x))
			{
				t = (TREE *)bid(x);
				d = (DBLK *)tid(t);
				d->mlist[gbl(scr_cur_no)] =
					get_mlist(d->flist, gbl(scr_cur)->file_spec,
						&match_count, &match_bytes);
			}
			n->node_match_count = match_count;
			n->node_match_bytes = match_bytes;
			n->showall_mlist[gbl(scr_cur_no)] = get_mlist(n->showall_flist,
				gbl(scr_cur)->file_spec, 0, 0);
		}

		switch (gbl(scr_cur)->command_mode)
		{
		case m_dir:
		case m_file:
		case m_showall:
			break;
		case m_level_showall:
		case m_global:
		case m_tag_showall:
		case m_tag_global:
			gbl(scr_cur)->global_mlist =
				get_mlist(gbl(scr_cur)->global_flist,
					gbl(scr_cur)->file_spec, 0, 0);
			break;
		}
	}

	return (0);
}

static void set_file_spec (void)
{
	BLIST *b;
	FBLK *f;

	rm_file_spec();

	/* MUST CHANGE */
	if (gbl(scr_cur)->file_spec[0] &&
		strcmp(gbl(scr_cur)->file_spec, fn_all()) != 0)
	{
		gbl(scr_cur)->file_spec_pres = TRUE;
	}

	disp_file_box();
	add_file_spec();
	set_first_file(gbl(scr_cur_no));
	if (gbl(scr_cur)->file_spec_pres)
	{
		gbl(scr_cur)->dir_match_count = 0;
		gbl(scr_cur)->dir_match_bytes = 0;
		for (b=gbl(scr_cur)->first_file; b; b=bnext(b))
		{
			f = (FBLK *)bid(b);
			gbl(scr_cur)->dir_match_count++;
			if (!check_for_dup_inode(gbl(scr_cur)->first_file, f, 1, 0))
				gbl(scr_cur)->dir_match_bytes += f->stbuf.st_size;
		}
	}
	else
	{
		gbl(scr_cur)->dir_match_count = gbl(scr_cur)->dir_total_count;
		gbl(scr_cur)->dir_match_bytes = gbl(scr_cur)->dir_total_bytes;
	}
	gbl(scr_cur)->top_file = gbl(scr_cur)->first_file;
	gbl(scr_cur)->numfiles = bcount(gbl(scr_cur)->first_file);
	if (gbl(scr_cur)->command_mode == m_dir)
	{
		disp_node_stats(gbl(scr_cur)->cur_root);
		werase(gbl(scr_cur)->cur_file_win);
		disp_file_list();
	}
	else
	{
		disp_dir_stats();
		if (gbl(scr_cur)->numfiles)
		{
			locate_cur_file();
			if (gbl(scr_cur)->command_mode != m_file)
			{
				fblk_to_dirname(gbl(scr_cur)->cur_file,
					gbl(scr_cur)->path_name);
				disp_path_line();
			}
			werase(gbl(scr_cur)->cur_file_win);
			disp_file_list();
			hilite_file(ON);
		}
	}
}

void do_file_spec (void)
{
	char input_str[MAX_FILELEN];
	int i;
	char *p;

	bang(msgs(m_filespec_entfil));
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	waddstr(gbl(win_commands), msgs(m_filespec_filspc));
	wrefresh(gbl(win_commands));
	*input_str = 0;
	i = xgetstr(gbl(win_commands), input_str, XGL_FILESPEC, MAX_FILELEN, 0,
		XG_FILESPEC);
	if (i < 0)
		return;
	if (i == 0)
	{
		gbl(scr_cur)->invert_file_spec = FALSE;
		strcpy(gbl(scr_cur)->file_spec, fn_all());
		waddstr(gbl(win_commands), gbl(scr_cur)->file_spec);
		wrefresh(gbl(win_commands));
	}
	else if (i == 1 && *input_str == '-')
	{
		if (gbl(scr_cur)->file_spec_pres)
			gbl(scr_cur)->invert_file_spec = !gbl(scr_cur)->invert_file_spec;
		else
			return;
	}
	else
	{
		i = 0;
		for (p=input_str; *p; p++)
		{
			if (*p == '^')
			{
				if (i == 0)
				{
					i = 1;
					break;
				}
			}
			if (*p == '[')
				i++;
			else if (*p == ']')
				i--;
			if (i<0 || i>1)
				break;
		}
		if (i)
		{
			errmsg(ER_IFS, "", ERR_ANY);
			return;
		}
		strcpy(gbl(scr_cur)->file_spec, input_str);
		gbl(scr_cur)->invert_file_spec = FALSE;
	}
	set_file_spec();
}

void locate_cur_file (void)
{
	int i;

	/* find file in file list */

	gbl(scr_cur)->file_cursor =
		bfind(gbl(scr_cur)->first_file, gbl(scr_cur)->cur_file);

	/* if not there, set pointers to top of list */

	if (!gbl(scr_cur)->file_cursor)
	{
		gbl(scr_cur)->file_cursor   = gbl(scr_cur)->first_file;
		gbl(scr_cur)->top_file      = gbl(scr_cur)->first_file;
		if (gbl(scr_cur)->file_cursor)
			gbl(scr_cur)->cur_file  = (FBLK *)bid(gbl(scr_cur)->file_cursor);
		gbl(scr_cur)->cur_file_no   = 0;
		gbl(scr_cur)->cur_file_line = 0;
		gbl(scr_cur)->cur_file_col  = 0;
	}
	else
	{
		gbl(scr_cur)->cur_file_no =
			bindex(gbl(scr_cur)->first_file, gbl(scr_cur)->file_cursor);

		/* num files on scrn */
		i = (gbl(scr_cur)->max_file_line+1)*(gbl(scr_cur)->max_file_col+1);

		if (gbl(scr_cur)->cur_file_no < i)
		{
			gbl(scr_cur)->top_file      = gbl(scr_cur)->first_file;
			gbl(scr_cur)->cur_file_line = gbl(scr_cur)->cur_file_no %
										(gbl(scr_cur)->max_file_line+1);
			gbl(scr_cur)->cur_file_col  = gbl(scr_cur)->cur_file_no /
										(gbl(scr_cur)->max_file_line+1);
		}
		else if (gbl(scr_cur)->cur_file_no >= gbl(scr_cur)->numfiles-i)
		{
			i = gbl(scr_cur)->numfiles-i;
			gbl(scr_cur)->top_file = bnth(gbl(scr_cur)->first_file, i);
			gbl(scr_cur)->cur_file_line = (gbl(scr_cur)->cur_file_no-i) %
										(gbl(scr_cur)->max_file_line+1);
			gbl(scr_cur)->cur_file_col  = (gbl(scr_cur)->cur_file_no-i) /
										(gbl(scr_cur)->max_file_line+1);
		}
		else
		{
			gbl(scr_cur)->cur_file_line = gbl(scr_cur)->max_file_line/2;
			gbl(scr_cur)->cur_file_col  = gbl(scr_cur)->max_file_col /2;
			i = gbl(scr_cur)->cur_file_no - gbl(scr_cur)->cur_file_line -
				gbl(scr_cur)->cur_file_col* (gbl(scr_cur)->max_file_line+1);
			gbl(scr_cur)->top_file = bnth(gbl(scr_cur)->first_file, i);
		}
	}
}

void rm_file_spec (void)
{
	BLIST *b;
	BLIST *x;
	TREE *t;
	NBLK *n;
	DBLK *d;

	if (gbl(scr_cur)->file_spec_pres)
	{
		for (b=gbl(nodes_list); b; b=bnext(b))
		{
			n = (NBLK *)bid(b);
			for (x=n->dir_list; x; x=bnext(x))
			{
				t = (TREE *)bid(x);
				d = (DBLK *)tid(t);
				BSCRAP(d->mlist[gbl(scr_cur_no)], FALSE);
				d->mlist[gbl(scr_cur_no)] = d->flist;
			}
			BSCRAP(n->showall_mlist[gbl(scr_cur_no)], FALSE);
			n->showall_mlist[gbl(scr_cur_no)] = n->showall_flist;
			n->node_match_count = n->node_total_count;
			n->node_match_bytes = n->node_total_bytes;
		}
		BSCRAP(gbl(scr_cur)->global_mlist, FALSE);
		gbl(scr_cur)->global_mlist = gbl(scr_cur)->global_flist;
		gbl(scr_cur)->file_spec_pres = FALSE;
	}
}

BLIST *get_mlist (BLIST *flist, const char *pattern,
	int *count, int *bytes)
{
	BLIST *b;
	BLIST *t;
	BLIST *l;
	BLIST *m;
	FBLK *f;

	b = 0;
	m = 0;
	for (l=flist; l; l=bnext(l))
	{
		f = (FBLK *)bid(l);
		if (match(FULLNAME(f), pattern))
		{
			if (count != 0)
				*count += 1;

			if (bytes != 0)
			{
				if (!check_for_dup_inode(m, f, 1, 0))
					*bytes += f->stbuf.st_size;
			}

			t = BNEW(f);
			if (t == 0)
			{
				m = BSCRAP(m, FALSE);
				errmsg(ER_IM, "", ERR_ANY);
				break;
			}

			if (b)
			{
				bprev(t) = b;
				bnext(b) = t;
			}
			else
			{
				m = t;
			}
			b = t;
		}
	}
	return (m);
}

void set_first_file (int n)
{
	DSTAT *cp;

	cp = &gbl(scr_stat)[n];

	switch (cp->command_mode)
	{
		case m_dir:
		case m_file:
			cp->base_file  = (cp->cur_dir)->flist;
			cp->first_file = (cp->cur_dir)->mlist[n];
			break;

		case m_showall:
			cp->base_file  = (cp->cur_root)->showall_flist;
			cp->first_file = (cp->cur_root)->showall_mlist[n];
			break;

		case m_level_showall:
		case m_global:
		case m_tag_showall:
		case m_tag_global:
			cp->base_file  = cp->global_flist;
			cp->first_file = cp->global_mlist;
			break;
	}

	cp->numfiles = bcount(cp->first_file);
	if (cp->numfiles <= 1)
		cp->top_file = cp->first_file;
}
