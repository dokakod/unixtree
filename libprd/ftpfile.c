/*------------------------------------------------------------------------
 * ftp file routines
 */
#include "libprd.h"

struct ftpfile_info
{
	char	pattern[MAX_PATHLEN];
	char	buffer[MAX_PATHLEN];
	char	old_name[MAX_PATHLEN];
	char	new_name[MAX_PATHLEN];
	int		do_ask;
};
typedef struct ftpfile_info FTPFILE_INFO;

static int ftp_delete_cur_file (void *data)
{
	FTPFILE_INFO *	fi = (FTPFILE_INFO *)data;
	int i;
	int c;
	NBLK *n;
	FTP_NODE *fn;

	n = get_root_of_file(gbl(scr_cur)->cur_file);
	fn = (FTP_NODE *)n->node_sub_blk;

	werase(gbl(win_commands));
	if (fi->do_ask)
	{
		wmove(gbl(win_commands), 0, 0);
		xaddstr(gbl(win_commands), msgs(m_delete_delfil1));
		zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_file));
		wrefresh(gbl(win_commands));
		c = yesno_msg(msgs(m_delete_deltfl));
		if (c < 0)
			return (-1);
		if (c)
			return (1);
	}
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_delete_delfil2));
	zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_file));
	wrefresh(gbl(win_commands));
	fblk_to_pathname((gbl(scr_cur)->cur_file), fi->buffer);

	if (ftp_delete_file(fn, fi->buffer))
	{
		c = errmsg(ER_CDF, "", ERR_ANY);
		return (c);
	}
	i = remove_file(gbl(scr_cur)->cur_file, TRUE);
	return (i);
}

int ftp_do_delete (void)
{
	FTPFILE_INFO	ftpfile_info;
	FTPFILE_INFO *	fi = &ftpfile_info;

	fi->do_ask = opt(prompt_for_delete);

	return (ftp_delete_cur_file(fi));
}

int ftp_do_tag_delete (void)
{
	FTPFILE_INFO	ftpfile_info;
	FTPFILE_INFO *	fi = &ftpfile_info;
	int ask;

	if (check_tag_count())
		return (0);

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_delete_delall));
	wrefresh(gbl(win_commands));
	if (opt(prompt_for_delete))
	{
		ask = yesno_msg(msgs(m_delete_condel));
		if (ask < 0)
			return (0);
		fi->do_ask = 1-ask;					/* yesno() returns 0 for yes */
	}
	else
	{
		fi->do_ask = 0;
	}
	traverse(ftp_delete_cur_file, fi);

	return (0);
}

static int ftp_rename_cur_file (void *data)
{
	FTPFILE_INFO *fi = (FTPFILE_INFO *)data;
	NBLK *n;
	FTP_NODE *fn;
	int c;

	n = get_root_of_file(gbl(scr_cur)->cur_file);
	fn = (FTP_NODE *)n->node_sub_blk;

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_rename_ren2));
	zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_file));
	waddstr(gbl(win_commands), msgs(m_rename_as1));
	zaddstr(gbl(win_commands), fi->buffer);
	wrefresh(gbl(win_commands));
	make_file_name(fi->pattern, FULLNAME(gbl(scr_cur)->cur_file), fi->buffer);
	fblk_to_dirname((gbl(scr_cur)->cur_file), fi->old_name);
	strcpy(fi->new_name, fi->old_name);
	fn_append_filename_to_dir(fi->old_name, FULLNAME(gbl(scr_cur)->cur_file));
	fn_append_filename_to_dir(fi->new_name, fi->buffer);
	if (ftp_rename_file(fn, fi->old_name, fi->new_name))
	{
		c = errsys(ER_CRF);
		return (c);
	}
	fblk_change_name(gbl(scr_cur)->cur_file, fi->buffer);
	hilite_file(ON);
	disp_cur_file();
	return (1);				/* for traverse() */
}

int ftp_do_rename (void)
{
	FTPFILE_INFO	ftpfile_info;
	FTPFILE_INFO *	fi = &ftpfile_info;
	int i;
	char *p, *q;

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	p = msgs(m_rename_ren1);
	q = msgs(m_rename_as2);
	xaddstr(gbl(win_commands), p);
	zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_file));
	bang(msgs(m_rename_efsfn));
	wmove(gbl(win_commands), 1, display_len(p)-display_len(q));
	waddstr(gbl(win_commands), q);
	wrefresh(gbl(win_commands));
	strcpy(fi->pattern, FULLNAME(gbl(scr_cur)->cur_file));
	i = xgetstr(gbl(win_commands), fi->pattern, XGL_PATTERN, MAX_FILELEN, 0,
		XG_FILENAME);
	if (i <= 0)
	{
		disp_cmds();
		return (0);
	}
	ftp_rename_cur_file(fi);
	disp_cmds();
	return (0);
}

int ftp_do_tag_rename (void)
{
	FTPFILE_INFO	ftpfile_info;
	FTPFILE_INFO *	fi = &ftpfile_info;
	int i;

	if (check_tag_count())
		return (0);

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_rename_ratf));
	bang(msgs(m_rename_efsfn));
	wmove(gbl(win_commands), 1, 0);
	waddstr(gbl(win_commands), msgs(m_rename_as2));
	wrefresh(gbl(win_commands));
	*fi->pattern = 0;
	i = xgetstr(gbl(win_commands), fi->pattern, XGL_PATTERN, MAX_FILELEN, 0,
		XG_FILENAME);
	if (i <= 0)
		return (0);
	traverse(ftp_rename_cur_file, fi);

	return (0);
}
