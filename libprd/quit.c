/*------------------------------------------------------------------------
 * quit processing
 */
#include "libprd.h"

static void delete_everything (void)
{
	BLIST *b;
	int i;

	/*--------------------------------------------------------------------
	 *	This code is to make sure we know everything by
	 *	removing it all and checking that nothing is left.
	 *
	 *	This routine must only be called after endwin().
	 */

	/*--------------------------------------------------------------------
	 * free menu list
	 */
	menu_free_memory();

	/*--------------------------------------------------------------------
	 * free archive device list
	 */
	arch_free_dev_tbl();

	/*--------------------------------------------------------------------
	 * free entry histories
	 */
	xg_tbl_free();

	/*--------------------------------------------------------------------
	 * free file-attr list
	 */
	fa_list_free();

	/*--------------------------------------------------------------------
	 * free resource file arrays
	 */
	free_res();

	/*--------------------------------------------------------------------
	 * free magic list
	 */
	what_free();

	/*--------------------------------------------------------------------
	 * free our windows & views
	 */
	delete_windows();

	gbl(vfcb_av) = view_end(gbl(vfcb_av));
	gbl(vfcb_fv) = view_end(gbl(vfcb_fv));
	for (i=0; i<gbl(scr_num_split_wins); i++)
	{
		BSCRAP(gbl(scr_stat)[i].global_flist, FALSE);
		if (gbl(scr_stat)[i].file_spec_pres)
			BSCRAP(gbl(scr_stat)[i].global_mlist, FALSE);
		gbl(vfcb_av_split)[i] = view_end(gbl(vfcb_av_split)[i]);
	}

	gbl(hxcb) = hexedit_end(gbl(hxcb));

	/*--------------------------------------------------------------------
	 * free our global lists
	 */
	gbl(mount_list) = ml_free_mounts(gbl(mount_list));

	for (b=gbl(nodes_list); b; b=bnext(b))
	{
		NBLK *n = (NBLK *)bid(b);

		del_node(n);
		nblk_free(n);
	}
	gbl(nodes_list) = BSCRAP(gbl(nodes_list), FALSE);

	gbl(owner_list) = owner_free(gbl(owner_list));
	gbl(group_list) = group_free(gbl(group_list));

	cmdopt(exclude_list) = BSCRAP(cmdopt(exclude_list), FALSE);

	/*--------------------------------------------------------------------
	 * free entities from utils library
	 */
	if (gbl(srch_re))
	{
		FREE(gbl(srch_re));
		gbl(srch_re) = 0;
	}

	/*--------------------------------------------------------------------
	 * free entities from curses library
	 */
	free_wins();

	/*--------------------------------------------------------------------
	 * free our argv & xvt struct
	 */
	xvt_data_free(gbl(xvt_data));

	/*--------------------------------------------------------------------
	 * finally, free our global vars struct
	 */
	FREE(gblvars_ptr);
	gblvars_ptr = 0;
}

int quit (void)
{
	char buf[128];

	if (! opt(prompt_for_quit))
		return (1);

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_quit_quit));
	wrefresh(gbl(win_commands));
	strcpy(buf, msgs(m_quit_ok));
	strcat(buf, package_name());
	if (yesno_msg(buf) == 0)
		return (1);

	disp_cmds();
	return (0);
}

int alt_quit (void)
{
	int c;

	if (quit() == 0)
		return (0);

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_quit_savckp));
	wrefresh(gbl(win_commands));
	bang("");
	if (putnode() == 0)
		return (1);

	c = errmsg(ER_CKP, "", ERR_YESNO);
	if (c == 0)
		return (1);

	disp_cmds();
	return (0);
}

void done_processing (void)
{
	BLIST *	b;

	/*--------------------------------------------------------------------
	 * clear out all nodes
	 */
	for (b=gbl(nodes_list); b; b=bnext(b))
	{
		NBLK *n;
		ABLK *a;

		n = (NBLK *)bid(b);
		switch (n->node_type)
		{
		case N_FS:
			break;

		case N_ARCH:
			a = (ABLK *)n->node_sub_blk;
			switch (a->arch_type)
			{
			case A_TAR:
			case A_BAR:
			case A_CPIO:
				break;

			case A_ZTAR:
				if (*a->arch_tmpname != 0)
					os_file_delete(a->arch_tmpname);
				break;
			}
			break;

		case N_FTP:
			close_ftp_node(n);
			break;
		}
	}

	/*--------------------------------------------------------------------
	 * normal end processing
	 */
	print_close(TRUE);			/* close out any printing */

	win_clock_set(FALSE);

	wattrset(stdscr, gbl(scr_exit_attr));
	clearok(curscr, TRUE);
	wclear(stdscr);
	wrefresh(stdscr);

	end_wins();

	/*--------------------------------------------------------------------
	 * save history entries
	 */
	xg_tbl_save();
}

void all_done (void)
{
	int		dbg_malloc = cmdopt(debug_malloc);

	/*--------------------------------------------------------------------
	 * close out everything
	 */
	done_processing();

	/*--------------------------------------------------------------------
	 * check memory if requested
	 */
	if (dbg_malloc > 0)
	{
		delete_everything();
		SYSMEM_MEMCHK(0, dbg_malloc > 1);
	}

	pgm_exit(0);
	/*NOTREACHED*/
}

void pgm_exit (int rc)
{
	exit(rc);
	/*NOTREACHED*/
}
