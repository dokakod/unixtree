/*------------------------------------------------------------------------
 * display various cmd menus
 */
#include "libprd.h"

static void cmds_hexedit (void)
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_hexc));
	ret_msg(gbl(win_commands));
	xcaddstr(gbl(win_commands), CMDS_RETURN, msgs(m_cmds_hexsave));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	fk_msg(gbl(win_commands), CMDS_HEXEDIT_TOGGLE, msgs(m_cmds_hextoggle));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	if (gbl(hxcb)->x_dirty)
	{
		fk_msg(gbl(win_commands), CMDS_HEXEDIT_UNDO, msgs(m_cmds_hexundo));
		waddstr(gbl(win_commands), msgs(m_pgm_separate));
	}
	fk_msg(gbl(win_commands), CMDS_HEXEDIT_MASK,
		opt(display_mask) ? msgs(m_cmds_hexunmask) : msgs(m_cmds_hexmask));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
}

static void cmds_diff (void)
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_diff));
	xcaddstr(gbl(win_commands), CMDS_DIFF_TOGGLE_COM, msgs(m_cmds_diffcom));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_DIFF_TOGGLE_NUM, msgs(m_cmds_diffnum));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_DIFF_NEXT_DIFF_1, msgs(m_cmds_diffnext));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_DIFF_PREV_DIFF_1, msgs(m_cmds_diffprev));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_QUIT, msgs(m_cmds_quit));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_DIFF_REPORT, msgs(m_cmds_report));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
}

static void cmds_reg_av (void)
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_avr));
	xcaddstr(gbl(win_commands), CMDS_COMMON_ATTRS, msgs(m_cmds_attrs));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_COPY, msgs(m_cmds_copy));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_DELETE, msgs(m_cmds_delete));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_EDIT, msgs(m_cmds_edit));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_FILESPEC, msgs(m_cmds_filespec));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_GOTO1, msgs(m_cmds_goto));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_VIEW_HEXEDIT, msgs(m_cmds_hexedit));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_INVERT, msgs(m_cmds_invert));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_DIFF, msgs(m_cmds_jfc));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_REG_FILE_LINK, msgs(m_cmds_link));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
	xcaddstr(gbl(win_commands), CMDS_REG_FILE_MOVE, msgs(m_cmds_move));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_PRINT, msgs(m_cmds_print));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_RENAME, msgs(m_cmds_rename));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_QUIT, msgs(m_cmds_quit));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_VIEW_SRCH_FWD, msgs(m_cmds_search));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_TAG, msgs(m_cmds_tag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_UNTAG, msgs(m_cmds_untag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_VIEW, msgs(m_cmds_view));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_VIEW_WINDOW, msgs(m_cmds_window));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_EXEC, msgs(m_cmds_exec));
}

static void cmds_alt_av (void)
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_ava));
	xcaddstr(gbl(win_commands), CMDS_COMMON_INVERT, msgs(m_cmds_invert));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_QUIT, msgs(m_cmds_quit));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_TAG, msgs(m_cmds_tag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_UNTAG, msgs(m_cmds_untag));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
}

static void cmds_tag_av (void)
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_avt));
	xcaddstr(gbl(win_commands), CMDS_COMMON_ATTRS, msgs(m_cmds_attrs));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_COPY, msgs(m_cmds_copy));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_DELETE, msgs(m_cmds_delete));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_INVERT, msgs(m_cmds_invert));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_REG_FILE_MOVE, msgs(m_cmds_move));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_PRINT, msgs(m_cmds_print));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
	xcaddstr(gbl(win_commands), CMDS_COMMON_RENAME, msgs(m_cmds_rename));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_TAG_FILE_SEARCH, msgs(m_cmds_search));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_TAG, msgs(m_cmds_tag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_UNTAG, msgs(m_cmds_untag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_VIEW, msgs(m_cmds_view));
}

static void cmds_reg_dir (void)
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_dirr));
	xcaddstr(gbl(win_commands), CMDS_COMMON_ATTRS, msgs(m_cmds_attrs));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_REG_DIR_COMPARE, msgs(m_cmds_cmp1));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_DELETE, msgs(m_cmds_delete));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_EDIT, msgs(m_cmds_edit));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_FILESPEC, msgs(m_cmds_filespec));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_REG_DIR_GLOBAL, msgs(m_cmds_global));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_INVERT, msgs(m_cmds_invert));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_REG_DIR_LINK, msgs(m_cmds_link));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_REG_DIR_MKDIR, msgs(m_cmds_make));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
	xcaddstr(gbl(win_commands), CMDS_COMMON_NODE, msgs(m_cmds_node));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_PRINT, msgs(m_cmds_print));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_QUIT, msgs(m_cmds_quit));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_RENAME, msgs(m_cmds_rename));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_REG_DIR_SHOWALL, msgs(m_cmds_showall));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_TAG, msgs(m_cmds_tag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_UNTAG, msgs(m_cmds_untag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_REG_DIR_AVAILABLE, msgs(m_cmds_avail));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_EXEC, msgs(m_cmds_exec));
	if (bnext(gbl(nodes_list)) || bprev(gbl(nodes_list)))
	{
		waddstr(gbl(win_commands), msgs(m_pgm_separate));
		xcaddstr(gbl(win_commands), CMDS_COMMON_NEXT_NODE_1,
			msgs(m_cmds_select));
	}
}

static void cmds_alt_dir (void)
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_dira));
	xcaddstr(gbl(win_commands), CMDS_ALT_DIR_COMPACT, msgs(m_cmds_compact));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_DIRDISP, msgs(m_cmds_dirdisp));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_FILEDISP, msgs(m_cmds_filedisp));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_ALT_DIR_GRAFT, msgs(m_cmds_graft));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_INVERT, msgs(m_cmds_invert));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_NODE, msgs(m_cmds_node));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
	xcaddstr(gbl(win_commands), CMDS_ALT_DIR_PRUNE, msgs(m_cmds_prune));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_QUIT, msgs(m_cmds_quit));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_ALT_DIR_RELEASE, msgs(m_cmds_release));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_ALT_DIR_SORT, msgs(m_cmds_sort));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_TAG, msgs(m_cmds_tag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_UNTAG, msgs(m_cmds_untag));
}

static void cmds_tag_dir (void)
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_dirt));
	xcaddstr(gbl(win_commands), CMDS_TAG_DIR_GLOBAL, msgs(m_cmds_global));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_INVERT, msgs(m_cmds_invall));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_TAG_DIR_SHOWALL, msgs(m_cmds_showtag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_TAG, msgs(m_cmds_tagall));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_UNTAG, msgs(m_cmds_untagall));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
}

static void cmds_reg_file (void)
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_filr));
	xcaddstr(gbl(win_commands), CMDS_COMMON_ATTRS, msgs(m_cmds_attrs));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_REG_FILE_BACKUP, msgs(m_cmds_backup));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_COPY, msgs(m_cmds_copy));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_DELETE, msgs(m_cmds_delete));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_EDIT, msgs(m_cmds_edit));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_FILESPEC, msgs(m_cmds_filespec));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_GOTO1, msgs(m_cmds_goto));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_INVERT, msgs(m_cmds_invert));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_DIFF, msgs(m_cmds_jfc));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_REG_FILE_LINK, msgs(m_cmds_link));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
	xcaddstr(gbl(win_commands), CMDS_REG_FILE_MOVE, msgs(m_cmds_move));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_NODE, msgs(m_cmds_node));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_REG_FILE_OPEN, msgs(m_cmds_open));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_PRINT, msgs(m_cmds_print));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_QUIT, msgs(m_cmds_quit));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_RENAME, msgs(m_cmds_rename));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_REG_FILE_RESTORE, msgs(m_cmds_restore));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_TAG, msgs(m_cmds_tag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_UNTAG, msgs(m_cmds_untag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_VIEW, msgs(m_cmds_view));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_EXEC, msgs(m_cmds_exec));
}

static void cmds_alt_file (void)
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_fila));
	xcaddstr(gbl(win_commands), CMDS_COMMON_COPY, msgs(m_cmds_copy));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_DIRDISP, msgs(m_cmds_dirdisp));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_FILEDISP, msgs(m_cmds_filedisp));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_INVERT, msgs(m_cmds_invert));
	if (S_ISLNK((gbl(scr_cur)->cur_file)->stbuf.st_mode))
	{
		waddstr(gbl(win_commands), msgs(m_pgm_separate));
		xcaddstr(gbl(win_commands), CMDS_ALT_FILE_LINK, msgs(m_cmds_link));
	}
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_NODE, msgs(m_cmds_node));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
	xcaddstr(gbl(win_commands), CMDS_COMMON_QUIT, msgs(m_cmds_quit));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_ALT_FILE_RELEASE, msgs(m_cmds_release));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_ALT_FILE_SORT, msgs(m_cmds_sort));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_TAG, msgs(m_cmds_tag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_UNTAG, msgs(m_cmds_untag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_ALT_FILE_WHAT, msgs(m_cmds_what));
}

static void cmds_tag_file (void)
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_filt));
	xcaddstr(gbl(win_commands), CMDS_COMMON_ATTRS, msgs(m_cmds_attrs));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_TAG_FILE_BACKUP, msgs(m_cmds_backup));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_COPY, msgs(m_cmds_copy));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_DELETE, msgs(m_cmds_delete));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_TAG_FILE_BATCH, msgs(m_cmds_script));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_INVERT, msgs(m_cmds_invall));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_TAG_FILE_LINK, msgs(m_cmds_link));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_TAG_FILE_MOVE, msgs(m_cmds_move));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
	xcaddstr(gbl(win_commands), CMDS_TAG_FILE_COMPARE, msgs(m_cmds_cmp2));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_PRINT, msgs(m_cmds_print));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_RENAME, msgs(m_cmds_rename));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands),  CMDS_TAG_FILE_SEARCH, msgs(m_cmds_search));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_TAG, msgs(m_cmds_tagall));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_UNTAG, msgs(m_cmds_untagall));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_VIEW, msgs(m_cmds_view));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_EXEC, msgs(m_cmds_exec));
}

static void cmds_reg_fv (void)
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_fvr));
	xcaddstr(gbl(win_commands), CMDS_VIEW_SRCH_BCK, msgs(m_cmds_srchbck));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_EDIT, msgs(m_cmds_edit));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_VIEW_HEXEDIT, msgs(m_cmds_hexedit));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_PRINT, msgs(m_cmds_print));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_QUIT, msgs(m_cmds_quit));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_VIEW_SRCH_FWD, msgs(m_cmds_srchfwd));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_VIEW_WINDOW, msgs(m_cmds_window));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
}

static void cmds_reg_arcf (void)
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_filr));
	xcaddstr(gbl(win_commands), CMDS_REG_FILE_EXTRACT, msgs(m_cmds_extract));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_FILESPEC, msgs(m_cmds_filespec));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_GOTO1, msgs(m_cmds_goto));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_INVERT, msgs(m_cmds_invert));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_NODE, msgs(m_cmds_node));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_REG_FILE_OPEN, msgs(m_cmds_open));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
	xcaddstr(gbl(win_commands), CMDS_COMMON_PRINT, msgs(m_cmds_print));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_QUIT, msgs(m_cmds_quit));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_TAG, msgs(m_cmds_tag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_UNTAG, msgs(m_cmds_untag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_VIEW, msgs(m_cmds_view));
}

static void cmds_alt_arcf (void)
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_fila));
	xcaddstr(gbl(win_commands), CMDS_COMMON_DIRDISP, msgs(m_cmds_dirdisp));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_FILEDISP, msgs(m_cmds_filedisp));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_INVERT, msgs(m_cmds_invert));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_NODE, msgs(m_cmds_node));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
	xcaddstr(gbl(win_commands), CMDS_ALT_FILE_RELEASE, msgs(m_cmds_release));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_ALT_FILE_SORT, msgs(m_cmds_sort));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_TAG, msgs(m_cmds_tag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_UNTAG, msgs(m_cmds_untag));
}

static void cmds_tag_arcf (void)
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_filt));
	xcaddstr(gbl(win_commands), CMDS_TAG_FILE_EXTRACT, msgs(m_cmds_extract));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_INVERT, msgs(m_cmds_invall));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_TAG, msgs(m_cmds_tagall));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_UNTAG, msgs(m_cmds_untagall));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
}

static void cmds_reg_arcd (void)
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_dirr));
	xcaddstr(gbl(win_commands), CMDS_COMMON_FILESPEC, msgs(m_cmds_filespec));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_REG_DIR_GLOBAL, msgs(m_cmds_global));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_INVERT, msgs(m_cmds_invert));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_NODE, msgs(m_cmds_node));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_PRINT, msgs(m_cmds_print));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_QUIT, msgs(m_cmds_quit));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
	xcaddstr(gbl(win_commands), CMDS_REG_DIR_SHOWALL, msgs(m_cmds_showall));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_TAG, msgs(m_cmds_tag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_UNTAG, msgs(m_cmds_untag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_REG_DIR_AVAILABLE, msgs(m_cmds_avail));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_EXEC, msgs(m_cmds_exec));
	if (bnext(gbl(nodes_list)) || bprev(gbl(nodes_list)))
	{
		waddstr(gbl(win_commands), msgs(m_pgm_separate));
		xcaddstr(gbl(win_commands), CMDS_COMMON_NEXT_NODE_1,
			msgs(m_cmds_select));
	}
}

static void cmds_alt_arcd (void)
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_dira));
	xcaddstr(gbl(win_commands), CMDS_COMMON_DIRDISP, msgs(m_cmds_dirdisp));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_FILEDISP, msgs(m_cmds_filedisp));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_INVERT, msgs(m_cmds_invert));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_NODE, msgs(m_cmds_node));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
	xcaddstr(gbl(win_commands), CMDS_ALT_DIR_RELEASE, msgs(m_cmds_release));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_ALT_DIR_SORT, msgs(m_cmds_sort));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_TAG, msgs(m_cmds_tag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_UNTAG, msgs(m_cmds_untag));
}

static void cmds_tag_arcd (void)
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_dirt));
	xcaddstr(gbl(win_commands), CMDS_TAG_DIR_GLOBAL, msgs(m_cmds_global));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_INVERT, msgs(m_cmds_invall));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_TAG_DIR_SHOWALL, msgs(m_cmds_showtag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_TAG, msgs(m_cmds_tagall));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_UNTAG, msgs(m_cmds_untagall));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
}

static void cmds_reg_ftpf (void)
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_filr));
	xcaddstr(gbl(win_commands), CMDS_COMMON_ATTRS, msgs(m_cmds_attrs));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_COPY, msgs(m_cmds_copy));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_DELETE, msgs(m_cmds_delete));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_FILESPEC, msgs(m_cmds_filespec));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_GOTO1, msgs(m_cmds_goto));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_INVERT, msgs(m_cmds_invert));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
	xcaddstr(gbl(win_commands), CMDS_COMMON_NODE, msgs(m_cmds_node));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_REG_FILE_OPEN, msgs(m_cmds_open));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_PRINT, msgs(m_cmds_print));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_QUIT, msgs(m_cmds_quit));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_RENAME, msgs(m_cmds_rename));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_TAG, msgs(m_cmds_tag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_UNTAG, msgs(m_cmds_untag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_VIEW, msgs(m_cmds_view));
	if (bnext(gbl(nodes_list)) || bprev(gbl(nodes_list)))
	{
		waddstr(gbl(win_commands), msgs(m_pgm_separate));
		xcaddstr(gbl(win_commands), CMDS_COMMON_NEXT_NODE_1,
			msgs(m_cmds_select));
	}
}

static void cmds_alt_ftpf (void)
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_fila));
	xcaddstr(gbl(win_commands), CMDS_COMMON_COPY, msgs(m_cmds_copy));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_DIRDISP, msgs(m_cmds_dirdisp));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_FILEDISP, msgs(m_cmds_filedisp));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_INVERT, msgs(m_cmds_invert));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_NODE, msgs(m_cmds_node));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
	xcaddstr(gbl(win_commands), CMDS_ALT_FILE_RELEASE, msgs(m_cmds_release));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_ALT_FILE_SORT, msgs(m_cmds_sort));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_TAG, msgs(m_cmds_tag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_UNTAG, msgs(m_cmds_untag));
}

static void cmds_tag_ftpf (void)
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_filt));
	xcaddstr(gbl(win_commands), CMDS_COMMON_ATTRS, msgs(m_cmds_attrs));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_COPY, msgs(m_cmds_copy));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_DELETE, msgs(m_cmds_delete));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_INVERT, msgs(m_cmds_invall));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
	xcaddstr(gbl(win_commands), CMDS_COMMON_PRINT, msgs(m_cmds_print));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_RENAME, msgs(m_cmds_rename));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_TAG, msgs(m_cmds_tagall));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_UNTAG, msgs(m_cmds_untagall));
}

static void cmds_reg_ftpd (void )
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_dirr));
	xcaddstr(gbl(win_commands), CMDS_COMMON_DELETE, msgs(m_cmds_delete));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_FILESPEC, msgs(m_cmds_filespec));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_REG_DIR_GLOBAL, msgs(m_cmds_global));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_INVERT, msgs(m_cmds_invert));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_REG_DIR_MKDIR, msgs(m_cmds_make));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_NODE, msgs(m_cmds_node));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
	xcaddstr(gbl(win_commands), CMDS_COMMON_PRINT, msgs(m_cmds_print));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_QUIT, msgs(m_cmds_quit));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_REG_DIR_SHOWALL, msgs(m_cmds_showall));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_TAG, msgs(m_cmds_tag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_UNTAG, msgs(m_cmds_untag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_EXEC, msgs(m_cmds_exec));
	if (bnext(gbl(nodes_list)) || bprev(gbl(nodes_list)))
	{
		waddstr(gbl(win_commands), msgs(m_pgm_separate));
		xcaddstr(gbl(win_commands), CMDS_COMMON_NEXT_NODE_1,
			msgs(m_cmds_select));
	}
}

static void cmds_alt_ftpd (void)
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_dira));
	xcaddstr(gbl(win_commands), CMDS_COMMON_DIRDISP, msgs(m_cmds_dirdisp));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_FILEDISP, msgs(m_cmds_filedisp));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_INVERT, msgs(m_cmds_invert));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_NODE, msgs(m_cmds_node));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
	xcaddstr(gbl(win_commands), CMDS_ALT_DIR_RELEASE, msgs(m_cmds_release));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_ALT_DIR_SORT, msgs(m_cmds_sort));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_TAG, msgs(m_cmds_tag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_UNTAG, msgs(m_cmds_untag));
}

static void cmds_tag_ftpd (void)
{
	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_dirt));
	xcaddstr(gbl(win_commands), CMDS_TAG_DIR_GLOBAL, msgs(m_cmds_global));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_INVERT, msgs(m_cmds_invall));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_TAG_DIR_SHOWALL, msgs(m_cmds_showtag));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_TAG, msgs(m_cmds_tagall));
	waddstr(gbl(win_commands), msgs(m_pgm_separate));
	xcaddstr(gbl(win_commands), CMDS_COMMON_UNTAG, msgs(m_cmds_untagall));

	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmds_cmds));
}

void disp_cmds (void)
{
	NBLK *n;

	n = gbl(scr_cur)->cur_root;

	/* hexedit mode */

	if (gbl(scr_in_hexedit))
	{
		wattrset (gbl(win_commands), gbl(scr_av_lolite_attr));
		wstandset(gbl(win_commands), gbl(scr_av_menu_attr));
		wattrset (gbl(win_message),  gbl(scr_av_lolite_attr));
		wstandset(gbl(win_message),  gbl(scr_av_menu_attr));
		cmds_hexedit();
	}

	/* Fullview mode */

	else if (gbl(scr_in_fullview))
	{
		wattrset (gbl(win_commands), gbl(scr_av_lolite_attr));
		wstandset(gbl(win_commands), gbl(scr_av_menu_attr));
		wattrset (gbl(win_message),  gbl(scr_av_lolite_attr));
		wstandset(gbl(win_message),  gbl(scr_av_menu_attr));
		cmds_reg_fv();
	}

	/* Autoview mode */

	else if (gbl(scr_in_autoview))
	{
		if (gbl(scr_av_focus))
		{
			wattrset (gbl(win_commands), gbl(scr_av_lolite_attr));
			wstandset(gbl(win_commands), gbl(scr_av_menu_attr));
		}
		else
		{
			if (gbl(scr_cur)->command_mode == m_dir ||
			    gbl(scr_cur)->command_mode == m_file)
			{
				wattrset (gbl(win_commands), gbl(scr_reg_lolite_attr));
				wstandset(gbl(win_commands), gbl(scr_reg_menu_attr));
			}
			else
			{
				wattrset (gbl(win_commands), gbl(scr_sa_lolite_attr));
				wstandset(gbl(win_commands), gbl(scr_sa_menu_attr));
			}
		}
		wattrset (gbl(win_message), wattrget(gbl(win_commands)));
		wstandset(gbl(win_message), wstandget(gbl(win_commands)));
		switch (gbl(scr_cur)->cmd_sub_mode)
		{
			case m_reg:	cmds_reg_av();
						break;
			case m_tag:	cmds_tag_av();
						break;
			case m_alt:	cmds_alt_av();
						break;
		}
	}

	/* diff mode */

	else if (gbl(scr_in_diffview))
	{
		wattrset (gbl(win_commands), gbl(scr_av_lolite_attr));
		wstandset(gbl(win_commands), gbl(scr_av_menu_attr));
		wattrset (gbl(win_message),  gbl(scr_av_lolite_attr));
		wstandset(gbl(win_message),  gbl(scr_av_menu_attr));
		cmds_diff();
	}

	/* Regular dir mode */

	else if (gbl(scr_cur)->command_mode == m_dir)
	{
		wattrset (gbl(win_commands), gbl(scr_reg_lolite_attr));
		wstandset(gbl(win_commands), gbl(scr_reg_menu_attr));
		wattrset (gbl(win_message),  gbl(scr_reg_lolite_attr));
		wstandset(gbl(win_message),  gbl(scr_reg_menu_attr));
		switch (n->node_type)
		{
		case N_FS:
			switch (gbl(scr_cur)->cmd_sub_mode)
			{
				case m_reg:	cmds_reg_dir();
							break;
				case m_tag:	cmds_tag_dir();
							break;
				case m_alt:	cmds_alt_dir();
							break;
			}
			break;

		case N_ARCH:
			switch (gbl(scr_cur)->cmd_sub_mode)
			{
				case m_reg:	cmds_reg_arcd();
							break;
				case m_tag:	cmds_tag_arcd();
							break;
				case m_alt:	cmds_alt_arcd();
							break;
			}
			break;

		case N_FTP:
			switch (gbl(scr_cur)->cmd_sub_mode)
			{
				case m_reg:	cmds_reg_ftpd();
							break;
				case m_tag:	cmds_tag_ftpd();
							break;
				case m_alt:	cmds_alt_ftpd();
							break;
			}
			break;
		}
	}

	/* Regular File mode */

	else
	{
		if (gbl(scr_cur)->command_mode == m_file)
		{
			wattrset (gbl(win_commands), gbl(scr_reg_lolite_attr));
			wstandset(gbl(win_commands), gbl(scr_reg_menu_attr));
		}
		else
		{
			wattrset (gbl(win_commands), gbl(scr_sa_lolite_attr));
			wstandset(gbl(win_commands), gbl(scr_sa_menu_attr));
		}
		wattrset (gbl(win_message), wattrget(gbl(win_commands)));
		wstandset(gbl(win_message), wstandget(gbl(win_commands)));
		switch (n->node_type)
		{
		case N_FS:
			switch (gbl(scr_cur)->cmd_sub_mode)
			{
				case m_reg:	cmds_reg_file();
							break;
				case m_tag:	cmds_tag_file();
							break;
				case m_alt:	cmds_alt_file();
							break;
			}
			break;

		case N_ARCH:
			switch (gbl(scr_cur)->cmd_sub_mode)
			{
				case m_reg:	cmds_reg_arcf();
							break;
				case m_tag:	cmds_tag_arcf();
							break;
				case m_alt:	cmds_alt_arcf();
							break;
			}
			break;

		case N_FTP:
			switch (gbl(scr_cur)->cmd_sub_mode)
			{
				case m_reg:	cmds_reg_ftpf();
							break;
				case m_tag:	cmds_tag_ftpf();
							break;
				case m_alt:	cmds_alt_ftpf();
							break;
			}
			break;
		}
	}
	wrefresh(gbl(win_commands));
}
