/*------------------------------------------------------------------------
 * process the "showall" cmd
 */
#include "libprd.h"

static BLIST *add_showall (NBLK *n)
{
	BLIST *b;
	BLIST *c;
	BLIST *x;
	BLIST *m;
	FBLK *f;

	m = 0;
	x = 0;
	for (b=n->showall_flist; b; b=bnext(b))
	{
		f = (FBLK *)bid(b);
		if (is_file_tagged(f))
		{
			c = BNEW(f);
			if (!c)
			{
				errmsg(ER_IM, "", ERR_ANY);
				break;
			}

			if (x)
			{
				bprev(c) = x;
				bnext(x) = c;
			}
			else
			{
				m = c;
			}
			x = c;
		}
	}

	return (m);
}

void do_showall (void)
{
	NBLK *n;

	n = gbl(scr_cur)->cur_root;

	gbl(scr_cur)->dir_total_count   = n->node_total_count;
	gbl(scr_cur)->dir_total_bytes   = n->node_total_bytes;
	gbl(scr_cur)->dir_match_count   = n->node_match_count;
	gbl(scr_cur)->dir_match_bytes   = n->node_match_bytes;
	gbl(scr_cur)->dir_tagged_count  = n->node_tagged_count;
	gbl(scr_cur)->dir_tagged_bytes  = n->node_tagged_bytes;
	gbl(scr_cur)->dir_tagged_blocks = n->node_tagged_blocks;
	gbl(scr_cur)->base_file         = n->showall_flist;
	gbl(scr_cur)->first_file        = n->showall_mlist[gbl(scr_cur_no)];
	gbl(scr_cur)->numfiles          = bcount(gbl(scr_cur)->first_file);

	if (gbl(scr_cur)->global_mlist != gbl(scr_cur)->global_flist)
	{
		gbl(scr_cur)->global_mlist  =
			BSCRAP(gbl(scr_cur)->global_mlist, FALSE);
	}
	else
	{
		gbl(scr_cur)->global_mlist  = 0;
	}
	gbl(scr_cur)->global_flist      =
		BSCRAP(gbl(scr_cur)->global_flist, FALSE);

	if (n->dir_disp_list[gbl(scr_cur_no)])
	{
		if (gbl(scr_cur)->command_mode == m_showall)
			gbl(scr_cur)->command_mode = m_level_showall;
		gbl(scr_cur)->numfiles = 0;
		gbl(scr_cur)->global_flist = get_files_to_level(n);
		if (gbl(scr_cur)->file_spec_pres)
		{
			gbl(scr_cur)->global_mlist = get_mlist(gbl(scr_cur)->global_flist,
				gbl(scr_cur)->file_spec, 0, 0);
		}
		else
		{
			gbl(scr_cur)->global_mlist = gbl(scr_cur)->global_flist;
		}
		gbl(scr_cur)->base_file  = gbl(scr_cur)->global_flist;
		gbl(scr_cur)->first_file = gbl(scr_cur)->global_mlist;
		setup_dir_stats();
	}
	else if (gbl(scr_cur)->command_mode == m_tag_showall)
	{
		gbl(scr_cur)->global_flist = add_showall(n);
		if (gbl(scr_cur)->file_spec_pres)
		{
			gbl(scr_cur)->global_mlist = get_mlist(gbl(scr_cur)->global_flist,
				gbl(scr_cur)->file_spec, 0, 0);
		}
		else
		{
			gbl(scr_cur)->global_mlist = gbl(scr_cur)->global_flist;
		}
		gbl(scr_cur)->numfiles = bcount(gbl(scr_cur)->global_mlist);
		gbl(scr_cur)->base_file  = gbl(scr_cur)->global_flist;
		gbl(scr_cur)->first_file = gbl(scr_cur)->global_mlist;
	}
}

BLIST *get_files_to_level (NBLK *n)
{
	BLIST *b;
	BLIST *c;
	BLIST *x;
	BLIST *m;
	DBLK *d;
	FBLK *f;

	m = 0;
	x = 0;
	for (b=n->showall_flist; b; b=bnext(b))
	{
		f = (FBLK *)bid(b);
		d = f->dir;
		if (!(d->hidden & gbl(scr_cur)->tag_mask))
		{
			if (!(gbl(scr_cur)->command_mode == m_tag_showall ||
			    gbl(scr_cur)->command_mode == m_tag_global) ||
			    is_file_tagged(f))
			{
				c = BNEW(f);
				if (!c)
				{
					errmsg(ER_IM, "", ERR_ANY);
					break;
				}

				if (x)
				{
					bprev(c) = x;
					bnext(x) = c;
				}
				else
				{
					m = c;
				}
				x = c;
				gbl(scr_cur)->numfiles++;
			}
		}
	}

	return (m);
}
