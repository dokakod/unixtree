/*------------------------------------------------------------------------
 * archive logging routines
 */
#include "libprd.h"

/*------------------------------------------------------------------------
 * log an archive
 */
void log_archive (void)
{
	int rc;

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_archive_arch));
	if (get_arch_device(gbl(scr_cur)->path_name, FALSE))
	{
		disp_cmds();
		return;
	}

	if (get_arch_name())
	{
		disp_cmds();
		return;
	}

	if (determine_arch_type())
	{
		werase(gbl(win_commands));
		wmove(gbl(win_commands), 0, 0);
		xaddstr(gbl(win_commands), msgs(m_archive_arch));
		if (get_arch_type())
		{
			disp_cmds();
			return;
		}
	}

	switch (ai->a_type)
	{
	case A_TAR:		rc = log_tar();		break;
	case A_BAR:		rc = log_bar();		break;
	case A_CPIO:	rc = log_cpio();	break;
	case A_ZTAR:	rc = log_ztar();	break;
	default:		rc = -1;			break;
	}

	if (rc == 0)
	{
		gbl(scr_cur)->cur_dir_no   = 0;
		gbl(scr_cur)->cur_dir_line = 0;
		gbl(scr_cur)->cur_dir_tree = (gbl(scr_cur)->cur_root)->root;
		gbl(scr_cur)->cur_dir = (DBLK *)tid(gbl(scr_cur)->cur_dir_tree);
		set_top_dir();
		set_top_file();
		fn_rootname(NULL, gbl(scr_cur)->path_name);
		gbl(scr_cur)->command_mode = m_dir;
		gbl(scr_cur)->cmd_sub_mode = m_reg;
	}

	setup_display();
	disp_cmds();
}

/*------------------------------------------------------------------------
 * display the "directory" of an archive
 */
void disp_arch_dirs (TREE *dir)
{
	NBLK *n;
	BLIST *b;
	TREE *t;
	DBLK *d;
	int i;

	n = gbl(scr_cur)->cur_root;
	fix_dir_list(n);
	i = 0;
	for (b=n->dir_list; b; b=bnext(b))
	{
		t = (TREE *)bid(b);
		if (t == dir)
			break;
		i++;
	}
	if (!b)
		return;
	d = (DBLK *)tid(t);

	gbl(scr_cur)->cur_dir_no = i;
	gbl(scr_cur)->dir_cursor = b;
	gbl(scr_cur)->cur_dir = d;
	if (i <= gbl(scr_cur)->max_dir_line)
	{
		gbl(scr_cur)->top_dir = n->dir_list;
		gbl(scr_cur)->cur_dir_line = i;
	}
	else
	{
		gbl(scr_cur)->top_dir =
			bnth(n->dir_list, i-gbl(scr_cur)->max_dir_line);
		gbl(scr_cur)->cur_dir_line = gbl(scr_cur)->max_dir_line;
	}
	disp_dir_tree();
}
