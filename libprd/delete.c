/*------------------------------------------------------------------------
 * process the "delete" cmd
 */
#include "libprd.h"

struct delete_info
{
	int do_ask;
};
typedef struct delete_info DELETE_INFO;

static int delete_cur_file (void *data)
{
	DELETE_INFO *dli = (DELETE_INFO *)data;
	char buffer[MAX_PATHLEN];
	int i;
	int c;
	int writable;

	werase(gbl(win_commands));

	
	writable = can_we_write(&gbl(scr_cur)->cur_file->stbuf);
	if (dli->do_ask || ! writable)
	{
		wmove(gbl(win_commands), 0, 0);
		xaddstr(gbl(win_commands), msgs(m_delete_delfil1));
		zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_file));
		wrefresh(gbl(win_commands));

		werase(gbl(win_message));
		esc_msg();
		wmove(gbl(win_message), 0, 0);
		if (! writable)
		{
			xaddstr(gbl(win_message), errs(ER_NPW));
			xaddstr(gbl(win_message), " ");
		}
		xaddstr(gbl(win_message), msgs(m_delete_deltfl));
		c = yesno();
		werase(gbl(win_message));
		wrefresh(gbl(win_message));

		if (c < 0)
			return (-1);
		if (c)
			return (1);
	}
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_delete_delfil2));
	zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_file));
	wrefresh(gbl(win_commands));
	fblk_to_pathname((gbl(scr_cur)->cur_file), buffer);
	if (os_file_delete(buffer))
	{
		c = errsys(ER_CDF);
		return (c);
	}
	i = remove_file(gbl(scr_cur)->cur_file, TRUE);
	return (i);
}

int do_delete (void)
{
	DELETE_INFO		delete_info;
	DELETE_INFO *	dli = &delete_info;

	dli->do_ask = opt(prompt_for_delete);
	return (delete_cur_file(dli));
}

void do_del_files (void)
{
	DELETE_INFO		delete_info;
	DELETE_INFO *	dli = &delete_info;
	int ask;

	if (check_tag_count())
		return;

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_delete_delall));
	wrefresh(gbl(win_commands));
	if (opt(prompt_for_delete))
	{
		ask = yesno_msg(msgs(m_delete_condel));
		if (ask < 0)
			return;
		dli->do_ask = 1-ask;					/* yesno() returns 0 for yes */
	}
	else
	{
		dli->do_ask = 0;
	}
	traverse(delete_cur_file, dli);
}

int remove_file (FBLK *f, int for_real)
{
	BLIST *l;
	FBLK *g;
	NBLK *n;
	DBLK *d;
	int c;
	int i;
	int file_in_disp_list;
	int frow;
	int fcol;

	n = get_root_of_file(f);
	d = f->dir;

	/* adjust stats */

	if (!check_for_dup_inode(d->flist, f, 0, 0))
		d->dir_size -= f->stbuf.st_size;

	if (bfind(gbl(scr_cur)->base_file, f))
	{
		gbl(scr_cur)->dir_total_count--;
		if (!check_for_dup_inode(gbl(scr_cur)->base_file, f, 0, 0))
			gbl(scr_cur)->dir_total_bytes -= f->stbuf.st_size;
	}

	file_in_disp_list = FALSE;
	if (bfind(gbl(scr_cur)->first_file, f))
	{
		file_in_disp_list = TRUE;
		gbl(scr_cur)->numfiles--;
		gbl(scr_cur)->dir_match_count--;
		if (!check_for_dup_inode(gbl(scr_cur)->first_file, f, 0, 0))
			gbl(scr_cur)->dir_match_bytes -= f->stbuf.st_size;
	}

	n->node_total_count--;
	if (!check_for_dup_inode(n->showall_flist, f, 0, 0))
		n->node_total_bytes -= f->stbuf.st_size;

	n->node_match_count--;
	if (!check_for_dup_inode(n->showall_mlist[gbl(scr_cur_no)], f, 0, 0))
		n->node_match_bytes -= f->stbuf.st_size;

	untag_file(f);

	/* check for links */

	if (f->stbuf.st_nlink > 1)
	{
		l = bfind(n->link_list, f);
		n->link_list = BDELETE(n->link_list, l, FALSE);
		if (for_real)
		{
			for (l=n->link_list; l; l=bnext(l))
			{
				g = (FBLK *)bid(l);
				if (g->stbuf.st_ino == f->stbuf.st_ino &&
				    g->stbuf.st_dev == f->stbuf.st_dev)
				{
					g->stbuf.st_nlink--;
					if (gbl(scr_cur)->file_fmt == fmt_fdate)
						if (is_file_displayed(g, &frow, &fcol))
							redisplay_file(g, frow, fcol);
				}
			}
		}
	}

	/* update screen & pointers */

	if (file_in_disp_list)
	{
		if (gbl(scr_cur)->numfiles)
		{
			/* blank out extra entry in list if showing */

			c = gbl(scr_cur)->cur_file_no - gbl(scr_cur)->cur_file_line -
				gbl(scr_cur)->cur_file_col * (gbl(scr_cur)->max_file_line+1);
			if ((gbl(scr_cur)->max_file_line+1) *
					(gbl(scr_cur)->max_file_col+1) >
				gbl(scr_cur)->numfiles-c)
			{
				c = gbl(scr_cur)->numfiles-c;
				if (c > 0)
				{
					wmove(gbl(scr_cur)->cur_file_win,
						(c % (gbl(scr_cur)->max_file_line+1)),
						(c / (gbl(scr_cur)->max_file_line+1)) *
						gbl(scr_cur)->file_line_width);
					wclrtoeol(gbl(scr_cur)->cur_file_win);
				}
			}

			if (f == gbl(scr_cur)->cur_file)
			{
				if (bnext(gbl(scr_cur)->file_cursor))
				{
					if (gbl(scr_cur)->file_cursor == gbl(scr_cur)->top_file)
						gbl(scr_cur)->top_file = bnext(gbl(scr_cur)->top_file);
					gbl(scr_cur)->file_cursor =
						bnext(gbl(scr_cur)->file_cursor);
				}
				else
				{
					gbl(scr_cur)->cur_file_no--;
					if (gbl(scr_cur)->file_cursor == gbl(scr_cur)->top_file)
					{
						c = (gbl(scr_cur)->max_file_line+1) *
							(gbl(scr_cur)->max_file_col+1);
						if (c > gbl(scr_cur)->numfiles)
							c = gbl(scr_cur)->numfiles;
						gbl(scr_cur)->cur_file_line--;
						gbl(scr_cur)->file_cursor =
							bprev(gbl(scr_cur)->file_cursor);
						for (i=0; i<c; i++)
						{
							gbl(scr_cur)->top_file =
								bprev(gbl(scr_cur)->top_file);
							gbl(scr_cur)->cur_file_line++;
							if (gbl(scr_cur)->cur_file_line >
								gbl(scr_cur)->max_file_line)
							{
								gbl(scr_cur)->cur_file_line = 0;
								gbl(scr_cur)->cur_file_col++;
							}
						}
					}
					else
					{
						gbl(scr_cur)->file_cursor =
							bprev(gbl(scr_cur)->file_cursor);
						gbl(scr_cur)->cur_file_line--;
						if (gbl(scr_cur)->cur_file_line < 0)
						{
							gbl(scr_cur)->cur_file_line =
								gbl(scr_cur)->max_file_line;
							gbl(scr_cur)->cur_file_col--;
						}
					}
				}
				gbl(scr_cur)->cur_file =
					(FBLK *)bid(gbl(scr_cur)->file_cursor);
			}
		}
		else
		{
			gbl(scr_cur)->top_file = 0;
		}
	}

	/* we don't know which list file_cursor is in */

	d->flist         = BDELETE(d->flist, bfind(d->flist, f), FALSE);
	n->showall_flist = BDELETE(n->showall_flist, bfind(n->showall_flist, f),
		FALSE);
	for (i=0; i<gbl(scr_cur_count); i++)
	{
		switch (gbl(scr_stat)[i].command_mode)
		{
		case m_level_showall:
		case m_global:
		case m_tag_showall:
		case m_tag_global:
			gbl(scr_stat)[i].global_flist =
				BDELETE(gbl(scr_stat)[i].global_flist,
					bfind(gbl(scr_stat)[i].global_flist, f), FALSE);

			if (gbl(scr_stat)[i].file_spec_pres)
			{
				gbl(scr_stat)[i].global_mlist =
					BDELETE(gbl(scr_stat)[i].global_mlist,
						bfind(gbl(scr_stat)[i].global_mlist, f), FALSE);
			}
			else
			{
				gbl(scr_stat)[i].global_mlist = gbl(scr_stat)[i].global_flist;
			}
		}
		if (gbl(scr_stat)[i].file_spec_pres)
		{
			d->mlist[i] = BDELETE(d->mlist[i], bfind(d->mlist[i], f), FALSE);
			n->showall_mlist[i] = BDELETE(n->showall_mlist[i],
				bfind(n->showall_mlist[i], f), FALSE);
		}
		else
		{
			d->mlist[i]         = d->flist;
			n->showall_mlist[i] = n->showall_flist;
		}
	}
	set_first_file(gbl(scr_cur_no));
	update_dir(f->dir);
	fblk_free(f);

	disp_drive_info((gbl(scr_cur)->cur_dir)->stbuf.st_dev, 1);
	disp_dir_stats();
	if (gbl(scr_cur)->numfiles)
	{
		if (gbl(scr_cur)->command_mode != m_file)
		{
			fblk_to_dirname(gbl(scr_cur)->cur_file, gbl(scr_cur)->path_name);
			disp_path_line();
		}
		disp_file_list();
		return (0);
	}
	else
	{
		disp_file_list();
		return (-1);
	}
}
