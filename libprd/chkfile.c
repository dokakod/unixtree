/*------------------------------------------------------------------------
 * routines to check a file
 */
#include "libprd.h"

void check_the_file (const char *pathname)
{
	char dir_name[MAX_PATHLEN];

	check_for_file(fn_basename(pathname), fn_dirname(pathname, dir_name));
}

void check_for_file (const char *file_name, const char *dir_name)
{
	TREE *t;
	DBLK *d;
	NBLK *n;

	t = path_to_dt(dir_name, N_FS);
	if (!t)
		return;

	d = (DBLK *)tid(t);
	if (d->flags & (D_NOT_LOGGED | D_CANT_LOG))
		return;

	n = get_root_of_dir(t);

	if (n->node_type == N_FS)
	{
		disp_drive_info(d->stbuf.st_dev, -1);
	}

	add_file_to_dir(file_name, dir_name, t, FALSE);
}

void add_file_to_dir (const char *file_name, const char *dir_name,
	TREE *dir, int flag)
{
	char buffer[MAX_PATHLEN];
	NBLK *n;
	NBLK *r;
	DBLK *d;
	FBLK *f;
	BLIST *b;
	BLIST *l;
	struct stat stbuf;
	int m[NUM_SPLIT_WINS];
	int i;

	d = (DBLK *)tid(dir);				/* get pointer to dir block */
	update_dir(d);						/* update directory stats */

	strcpy(buffer, dir_name);				/* make new pathname */
	fn_append_filename_to_dir(buffer, file_name);

	if (flag)
	{
		if (os_stat(buffer, &stbuf))			/* if not there, forget it */
			return;
	}
	else
	{
		if (os_lstat(buffer, &stbuf))		/* if not there, forget it */
			return;
	}

	n = get_root_of_dir(dir);
	r = gbl(scr_cur)->cur_root;

	/*
	 *  Update all statistics
	 */

	for (i=0; i<gbl(scr_cur_count); i++)
	{
		if (gbl(scr_stat)[i].file_spec_pres)
			m[i] = match(file_name, gbl(scr_stat)[i].file_spec);
		else
			m[i] = 1;
	}

	f = 0;								/* shut up cc */
	for (b=d->flist; b; b=bnext(b))		/* check if file already in it */
	{
		f = (FBLK *)bid(b);
		if (strcmp(FULLNAME(f), file_name) == 0)
			break;
	}

	if (b)
	{
		/* file already in directory */

		if (!check_for_dup_inode(d->flist, f, 0, 0))
			d->dir_size -= f->stbuf.st_size;	/* subtract old size */
		n->node_total_count--;
		if (!check_for_dup_inode(n->showall_flist, f, 0, 0))
			n->node_total_bytes -= f->stbuf.st_size;
		if (m[gbl(scr_cur_no)])
		{
			n->node_match_count--;
			if (! check_for_dup_inode(n->showall_mlist[gbl(scr_cur_no)], f,
				0, 0))
			{
				n->node_match_bytes -= f->stbuf.st_size;
			}
		}

		if (is_file_tagged(f))
		{
			n->node_tagged_count--;
			if (!check_for_dup_inode(n->showall_flist, f, 0, 1))
			{
				n->node_tagged_bytes -= f->stbuf.st_size;
				n->node_tagged_blocks -=
					file_size_in_blks(f, f->stbuf.st_size);
			}
		}

		if (r->node_type == n->node_type)
		{
			switch (gbl(scr_cur)->command_mode)
			{
			case m_file:
				if (gbl(scr_cur)->cur_dir != d)
					break;
				/*FALLTHROUGH*/
			case m_showall:
			case m_level_showall:
			case m_tag_showall:
			case m_global:
			case m_tag_global:
				gbl(scr_cur)->numfiles--;
				gbl(scr_cur)->dir_total_count--;
				if (!check_for_dup_inode(gbl(scr_cur)->base_file, f, 0, 0))
					gbl(scr_cur)->dir_total_bytes -= f->stbuf.st_size;
				if (m[gbl(scr_cur_no)])
				{
					gbl(scr_cur)->dir_match_count--;
					if (!check_for_dup_inode(gbl(scr_cur)->first_file, f,
						0, 0))
					{
						gbl(scr_cur)->dir_match_bytes -= f->stbuf.st_size;
					}
				}
				if (is_file_tagged(f))
				{
					gbl(scr_cur)->dir_tagged_count--;
					if (!check_for_dup_inode(gbl(scr_cur)->base_file, f, 0, 1))
					{
						gbl(scr_cur)->dir_tagged_bytes -= f->stbuf.st_size;
						gbl(scr_cur)->dir_tagged_blocks -= file_size_in_blks(f,
							f->stbuf.st_size);
					}
				}
			}
		}

		make_xstat(&stbuf, &f->stbuf);
		fblk_sym(f);
	}
	else
	{
		/* file not in directory */

		f = fs_make_fblk(file_name, &stbuf);
		if (!f)
			return;
		f->dir = d;
		fblk_sym(f);

		d->flist          = put_file_in_list(f, d->flist);
		n->showall_flist  = put_file_in_list(f, n->showall_flist);
		if (stbuf.st_nlink > 1)
		{
			l = BNEW(f);
			if (l)
			{
				n->link_list = bappend(n->link_list, l);
			}
		}

		for (i=0; i<gbl(scr_cur_count); i++)
		{
			if (gbl(scr_stat)[i].file_spec_pres)
			{
				if (m[i])
				{
					d->mlist[i]          = put_file_in_list(f, d->mlist[i]);
					n->showall_mlist[i]  =
						put_file_in_list(f, n->showall_mlist[i]);
				}
			}
			else
			{
				d->mlist[i]         = d->flist;
				n->showall_mlist[i] = n->showall_flist;
			}

			if (gbl(scr_stat)[i].command_mode == m_global ||
		    	(gbl(scr_stat)[i].command_mode == m_level_showall &&
			    !(d->hidden & gbl(scr_stat)[i].tag_mask)))
			{
				gbl(scr_stat)[i].global_flist =
					put_file_in_list(f, gbl(scr_stat)[i].global_flist);
				if (gbl(scr_stat)[i].file_spec_pres)
				{
					if (m[i])
					{
						gbl(scr_stat)[i].global_mlist =
							put_file_in_list(f, gbl(scr_stat)[i].global_mlist);
					}
				}
				else
				{
					gbl(scr_stat)[i].global_mlist =
						gbl(scr_stat)[i].global_flist;
				}
			}
		}
	}

	if (!check_for_dup_inode(d->flist, f, 0, 0))
		d->dir_size += f->stbuf.st_size;
	n->node_total_count++;
	if (!check_for_dup_inode(n->showall_flist, f, 0, 0))
		n->node_total_bytes += f->stbuf.st_size;
	if (m[gbl(scr_cur_no)])
	{
		n->node_match_count++;
		if (!check_for_dup_inode(n->showall_mlist[gbl(scr_cur_no)], f, 0, 0))
			n->node_match_bytes += f->stbuf.st_size;
	}
	if (is_file_tagged(f))
	{
		n->node_tagged_count++;
		if (!check_for_dup_inode(n->showall_flist, f, 0, 1))
		{
			n->node_tagged_bytes += f->stbuf.st_size;
			n->node_tagged_blocks += file_size_in_blks(f, f->stbuf.st_size);
		}
	}

	if (n->node_type == r->node_type)
	{
		switch (gbl(scr_cur)->command_mode)
		{
			case m_dir:
			case m_file:
				gbl(scr_cur)->base_file  = (gbl(scr_cur)->cur_dir)->flist;
				gbl(scr_cur)->first_file =
					(gbl(scr_cur)->cur_dir)->mlist[gbl(scr_cur_no)];
				break;
			case m_showall:
				gbl(scr_cur)->base_file  =
					(gbl(scr_cur)->cur_root)->showall_flist;
				gbl(scr_cur)->first_file =
					(gbl(scr_cur)->cur_root)->showall_mlist[gbl(scr_cur_no)];
				break;
			case m_level_showall:
			case m_global:
			case m_tag_showall:
			case m_tag_global:
				gbl(scr_cur)->base_file  = gbl(scr_cur)->global_flist;
				gbl(scr_cur)->first_file = gbl(scr_cur)->global_mlist;
				break;
		}
		gbl(scr_cur)->numfiles = bcount(gbl(scr_cur)->first_file);
		if (gbl(scr_cur)->numfiles <= 1)
			gbl(scr_cur)->top_file = gbl(scr_cur)->first_file;

		switch (gbl(scr_cur)->command_mode)
		{
		case m_dir:
			check_small_window();
			disp_node_stats(gbl(scr_cur)->cur_root);
			break;
		case m_file:
			if (gbl(scr_cur)->cur_dir != d)
				break;
			/*FALLTHROUGH*/
		case m_showall:
		case m_level_showall:
		case m_tag_showall:
		case m_global:
		case m_tag_global:
			gbl(scr_cur)->dir_total_count++;
			if (!check_for_dup_inode(gbl(scr_cur)->base_file, f, 0, 0))
				gbl(scr_cur)->dir_total_bytes += stbuf.st_size;
			if (m[gbl(scr_cur_no)])
			{
				gbl(scr_cur)->dir_match_count++;
				if (!check_for_dup_inode(gbl(scr_cur)->first_file, f, 0, 0))
					gbl(scr_cur)->dir_match_bytes += stbuf.st_size;
			}
			disp_dir_stats();
			gbl(scr_cur)->cur_file_no =
				bindex(gbl(scr_cur)->first_file, gbl(scr_cur)->file_cursor);
			i = gbl(scr_cur)->cur_file_no -
				(gbl(scr_cur)->cur_file_line + gbl(scr_cur)->cur_file_col *
				(gbl(scr_cur)->max_file_line+1));
			gbl(scr_cur)->top_file = bnth(gbl(scr_cur)->first_file, i);
			disp_file_list();
			hilite_file(ON);
			disp_cur_file();
			break;
		}
	}
}
