/*------------------------------------------------------------------------
 * process the "global" cmd
 */
#include "libprd.h"

static BLIST *add_global (NBLK *n)
{
	BLIST *b;
	BLIST *m;
	BLIST *x;
	BLIST *y;
	FBLK *f;

	m = 0;
	y = 0;
	for (b=n->showall_flist; b; b=bnext(b))
	{
		f = (FBLK *)bid(b);
		if (gbl(scr_cur)->command_mode == m_tag_global)
			if (!is_file_tagged(f))
				continue;
		x = BNEW(f);
		if (!x)
		{
			errmsg(ER_IM, "", ERR_ANY);
			break;
		}
		if (y)
		{
			bprev(x) = y;
			bnext(y) = x;
		}
		else
		{
			m = x;
		}
		y = x;
	}
	return (m);
}

void do_global (void)
{
	BLIST *b;
	NBLK *n;
	int ntype;

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	waddstr(gbl(win_commands), msgs(m_global_sgl));
	wrefresh(gbl(win_commands));
	bang("");

	gbl(scr_cur)->dir_total_count   = 0;
	gbl(scr_cur)->dir_total_bytes   = 0;
	gbl(scr_cur)->dir_match_count   = 0;
	gbl(scr_cur)->dir_match_bytes   = 0;
	gbl(scr_cur)->dir_tagged_count  = 0;
	gbl(scr_cur)->dir_tagged_bytes  = 0;
	gbl(scr_cur)->dir_tagged_blocks = 0;
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

	ntype = (gbl(scr_cur)->cur_root)->node_type;
	n = 0;				/* shut up cc */
	for (b=gbl(nodes_list); b; b=bnext(b))
	{
		n = (NBLK *)bid(b);
		if (n->node_type != ntype)
			continue;

		gbl(scr_cur)->dir_total_count += n->node_total_count;
		gbl(scr_cur)->dir_total_bytes += n->node_total_bytes;
		gbl(scr_cur)->dir_match_count += n->node_match_count;
		gbl(scr_cur)->dir_match_bytes += n->node_match_bytes;
		gbl(scr_cur)->dir_tagged_count += n->node_tagged_count;
		gbl(scr_cur)->dir_tagged_bytes += n->node_tagged_bytes;
		gbl(scr_cur)->dir_tagged_blocks += n->node_tagged_blocks;
		if (!n->dir_disp_list[gbl(scr_cur_no)])
		{
			gbl(scr_cur)->global_flist = merge_lists(add_global(n),
				gbl(scr_cur)->global_flist);
		}
		else
		{
			gbl(scr_cur)->global_flist = merge_lists(get_files_to_level(n),
				gbl(scr_cur)->global_flist);
		}
	}

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

	if (!gbl(scr_cur)->global_flist)
	{
		gbl(scr_cur)->command_mode = m_dir;
		gbl(scr_cur)->cmd_sub_mode = m_reg;
		disp_cmds();
	}
	gbl(scr_cur)->base_file = gbl(scr_cur)->global_flist;
	gbl(scr_cur)->first_file = gbl(scr_cur)->global_mlist;
	if (n->dir_disp_list[gbl(scr_cur_no)])
		setup_dir_stats();
}
