/*------------------------------------------------------------------------
 * common commands
 */
#include "libprd.h"

/*------------------------------------------------------------------------
 * reg dir cmds
 */
void do_cmn_reg_dir_invert (void)
{
	invert_dir(gbl(scr_cur)->dir_cursor);
	disp_node_stats(gbl(scr_cur)->cur_root);
	disp_file_list();
}

void do_cmn_reg_dir_tag (void)
{
	tag_dir(gbl(scr_cur)->dir_cursor);
	disp_node_stats(gbl(scr_cur)->cur_root);
	disp_file_list();
}

void do_cmn_reg_dir_untag (void)
{
	untag_dir(gbl(scr_cur)->dir_cursor);
	disp_node_stats(gbl(scr_cur)->cur_root);
	disp_file_list();
}

void do_cmn_reg_dir_global (void)
{
	gbl(scr_cur)->command_mode = m_global;
	dir_scroll_bar_remove();
	file_scroll_bar_remove();
	do_global();
	setup_sa_display();
}

void do_cmn_reg_dir_showall(void)
{
	gbl(scr_cur)->command_mode = m_showall;
	dir_scroll_bar_remove();
	file_scroll_bar_remove();
	do_showall();
	setup_sa_display();
}

/*------------------------------------------------------------------------
 * tag dir cmds
 */
void do_cmn_tag_dir_invert (void)
{
	BLIST *b;

	for (b=gbl(scr_cur)->first_dir; b; b=bnext(b))
	{
		invert_dir(b);
	}

	disp_node_stats(gbl(scr_cur)->cur_root);
	disp_file_list();
}

void do_cmn_tag_dir_tag (void)
{
	BLIST *b;

	for (b=gbl(scr_cur)->first_dir; b; b=bnext(b))
	{
		tag_dir(b);
	}

	disp_node_stats(gbl(scr_cur)->cur_root);
	disp_file_list();
}

void do_cmn_tag_dir_untag (void)
{
	BLIST *b;

	for (b=gbl(scr_cur)->first_dir; b; b=bnext(b))
	{
		untag_dir(b);
	}

	disp_node_stats(gbl(scr_cur)->cur_root);
	disp_file_list();
}

void do_cmn_tag_dir_global (void)
{
	gbl(scr_cur)->command_mode = m_tag_global;
	dir_scroll_bar_remove();
	file_scroll_bar_remove();
	do_global();
	gbl(scr_cur)->base_file  = gbl(scr_cur)->global_flist;
	gbl(scr_cur)->first_file = gbl(scr_cur)->global_mlist;
	setup_sa_display();
}

void do_cmn_tag_dir_showall(void)
{
	gbl(scr_cur)->command_mode = m_tag_showall;
	dir_scroll_bar_remove();
	file_scroll_bar_remove();
	do_showall();
	setup_sa_display();
}

/*------------------------------------------------------------------------
 * alt dir cmds
 */
void do_cmn_alt_dir_invert (void)
{
	TAG_INFO	tag_info;
	TAG_INFO *	ti = &tag_info;
	int			c;

	c = ask_alt_tag(cmds(CMDS_COMMON_INVERT), ti);
	if (c != 0)
	{
		if (ti->tag_choice == cmds(CMDS_TAG_NODE))
		{
			invert_sub_node(ti);
		}
		else
		{
			BLIST *b;

			for (b=gbl(scr_cur)->first_dir; b; b=bnext(b))
			{
				alt_invert_dir(b, ti);
			}
		}

		disp_node_stats(gbl(scr_cur)->cur_root);
		disp_file_list();
	}
}

void do_cmn_alt_dir_tag (void)
{
	TAG_INFO	tag_info;
	TAG_INFO *	ti = &tag_info;
	int			c;

	c = ask_alt_tag(cmds(CMDS_COMMON_TAG), ti);
	if (c != 0)
	{
		if (ti->tag_choice == cmds(CMDS_TAG_NODE))
		{
			tag_sub_node(ti);
		}
		else
		{
			BLIST *b;

			for (b=gbl(scr_cur)->first_dir; b; b=bnext(b))
			{
				alt_tag_dir(b, ti);
			}
		}

		disp_node_stats(gbl(scr_cur)->cur_root);
		disp_file_list();
	}
}

void do_cmn_alt_dir_untag (void)
{
	TAG_INFO	tag_info;
	TAG_INFO *	ti = &tag_info;
	int			c;

	c = ask_alt_tag(cmds(CMDS_COMMON_UNTAG), ti);
	if (c != 0)
	{
		if (ti->tag_choice == cmds(CMDS_TAG_NODE))
		{
			untag_sub_node(ti);
		}
		else
		{
			BLIST *b;

			for (b=gbl(scr_cur)->first_dir; b; b=bnext(b))
			{
				alt_untag_dir(b, ti);
			}
		}

		disp_node_stats(gbl(scr_cur)->cur_root);
		disp_file_list();
	}
}

void do_cmn_alt_dir_sort (void)
{
	int	c;

	c = do_sort();
	if (c >= 0)
	{
		gbl(scr_cur)->cur_file_line = 0;
		gbl(scr_cur)->cur_file_col  = 0;
		gbl(scr_cur)->cur_file_no   = 0;
		gbl(scr_cur)->base_file     = (gbl(scr_cur)->cur_dir)->flist;
		gbl(scr_cur)->first_file    =
			(gbl(scr_cur)->cur_dir)->mlist[gbl(scr_cur_no)];
		gbl(scr_cur)->top_file      = gbl(scr_cur)->first_file;
		gbl(scr_cur)->file_cursor   = gbl(scr_cur)->first_file;

		disp_file_list();
		do_file_scroll_bar();
	}
}

/*------------------------------------------------------------------------
 * reg file cmds
 */
void do_cmn_reg_file_invert (void)
{
	invert_file(gbl(scr_cur)->cur_file);
	hilite_file(ON);
	disp_dir_stats();
}

void do_cmn_reg_file_tag (void)
{
	tag_file(gbl(scr_cur)->cur_file);
	hilite_file(ON);
	disp_dir_stats();
}

void do_cmn_reg_file_untag (void)
{
	untag_file(gbl(scr_cur)->cur_file);
	hilite_file(ON);
	disp_dir_stats();
}

/*------------------------------------------------------------------------
 * tag file cmds
 */
void do_cmn_tag_file_invert (void)
{
	BLIST *b;

	for (b=gbl(scr_cur)->first_file; b; b=bnext(b))
	{
		FBLK *f = (FBLK *)bid(b);

		invert_file(f);
	}

	disp_file_list();
	hilite_file(ON);
	disp_dir_stats();
}

void do_cmn_tag_file_tag (void)
{
	BLIST *b;

	for (b=gbl(scr_cur)->first_file; b; b=bnext(b))
	{
		FBLK *f = (FBLK *)bid(b);

		tag_file(f);
	}

	disp_file_list();
	hilite_file(ON);
	disp_dir_stats();
}

void do_cmn_tag_file_untag (void)
{
	BLIST *b;

	for (b=gbl(scr_cur)->first_file; b; b=bnext(b))
	{
		FBLK *f = (FBLK *)bid(b);

		untag_file(f);
	}

	disp_file_list();
	hilite_file(ON);
	disp_dir_stats();
}

/*------------------------------------------------------------------------
 * alt file cmds
 */
void do_cmn_alt_file_invert (void)
{
	TAG_INFO	tag_info;
	TAG_INFO *	ti = &tag_info;
	int			c;

	c = ask_alt_tag(cmds(CMDS_COMMON_INVERT), ti);
	if (c != 0)
	{
		if (ti->tag_choice == cmds(CMDS_TAG_NODE))
		{
			invert_sub_node(ti);
		}
		else
		{
			BLIST *b;

			for (b=gbl(scr_cur)->first_file; b; b=bnext(b))
			{
				FBLK *f = (FBLK *)bid(b);

				alt_invert_file(f, ti);
			}
		}

		disp_file_list();
		hilite_file(ON);
		disp_dir_stats();
	}
}

void do_cmn_alt_file_tag (void)
{
	TAG_INFO	tag_info;
	TAG_INFO *	ti = &tag_info;
	int			c;

	c = ask_alt_tag(cmds(CMDS_COMMON_TAG), ti);
	if (c != 0)
	{
		if (ti->tag_choice == cmds(CMDS_TAG_NODE))
		{
			tag_sub_node(ti);
		}
		else
		{
			BLIST *b;

			for (b=gbl(scr_cur)->first_file; b; b=bnext(b))
			{
				FBLK *f = (FBLK *)bid(b);

				alt_tag_file(f, ti);
			}
		}

		disp_file_list();
		hilite_file(ON);
		disp_dir_stats();
	}
}

void do_cmn_alt_file_untag (void)
{
	TAG_INFO	tag_info;
	TAG_INFO *	ti = &tag_info;
	int			c;

	c = ask_alt_tag(cmds(CMDS_COMMON_UNTAG), ti);
	if (c != 0)
	{
		if (ti->tag_choice == cmds(CMDS_TAG_NODE))
		{
			untag_sub_node(ti);
		}
		else
		{
			BLIST *b;

			for (b=gbl(scr_cur)->first_file; b; b=bnext(b))
			{
				FBLK *f = (FBLK *)bid(b);

				alt_untag_file(f, ti);
			}
		}

		disp_file_list();
		hilite_file(ON);
		disp_dir_stats();
	}
}

void do_cmn_alt_file_copytags (void)
{
	BLIST *	b;
	int		i;

	i = 1 << next_scr_no(gbl(scr_cur_no));
	for (b=gbl(scr_cur)->first_file; b; b=bnext(b))
	{
		FBLK *f = (FBLK *)bid(b);

		if (is_file_tagged(f))
			f->tagged |= i;
	}
}

void do_cmn_alt_file_sort (void)
{
	int	c;

	c = do_sort();
	if (c >= 0)
	{
		gbl(scr_cur)->cur_file_line = 0;
		gbl(scr_cur)->cur_file_col  = 0;
		gbl(scr_cur)->cur_file_no   = 0;
		gbl(scr_cur)->file_cursor   = gbl(scr_cur)->first_file;
		gbl(scr_cur)->top_file      = gbl(scr_cur)->file_cursor;
		gbl(scr_cur)->cur_file      = (FBLK *)bid(gbl(scr_cur)->file_cursor);

		fblk_to_dirname(gbl(scr_cur)->cur_file, gbl(scr_cur)->path_name);
		disp_path_line();
		disp_file_list();
		do_file_scroll_bar();
		hilite_file(ON);
	}
}

/*------------------------------------------------------------------------
 * misc cmds
 */
void do_cmn_next_node (void)
{
	if (bprev(gbl(scr_cur)->cur_node) || bnext(gbl(scr_cur)->cur_node))
	{
		gbl(scr_cur)->cur_node = bnextc(gbl(scr_cur)->cur_node);
		init_disp();
		if (gbl(scr_cur)->command_mode != m_dir)
			setup_file_display(0);
	}
}

void do_cmn_prev_node (void)
{
	if (bprev(gbl(scr_cur)->cur_node) || bnext(gbl(scr_cur)->cur_node))
	{
		gbl(scr_cur)->cur_node = bprevc(gbl(scr_cur)->cur_node);
		init_disp();
		if (gbl(scr_cur)->command_mode != m_dir)
			setup_file_display(0);
	}
}
