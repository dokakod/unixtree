/*------------------------------------------------------------------------
 * This routine handles both compare directories &
 * compare files.
 */
#include "libprd.h"

struct cmp_info
{
	int	cmp_identical;
	int	cmp_unique;
	int	cmp_newer;
	int	cmp_older;
	int	cmp_same;

	FBLK *	file_to_cmp;
	char	path1[MAX_PATHLEN];
	char	path2[MAX_PATHLEN];
};
typedef struct cmp_info CMP_INFO;

static void cmp_dirs (CMP_INFO *cmpi, DBLK *d1, DBLK *d2)
{
	BLIST *b1;
	BLIST *b2;
	FBLK *f1;
	FBLK *f2;
	int tag_it;
	int frow;
	int fcol;

	for (b1=d1->flist; b1; b1=bnext(b1))
	{
		f1 = (FBLK *)bid(b1);
		f2 = 0;
		for (b2=d2->flist; b2; b2=bnext(b2))
		{
			f2 = (FBLK *)bid(b2);
			if (strcmp(FULLNAME(f1), FULLNAME(f2)) == 0)
				break;
		}

		tag_it = FALSE;
		if (!b2)
		{
			if (cmpi->cmp_unique)
				tag_it = TRUE;
		}
		else
		{
			if (cmpi->cmp_newer && (f1->stbuf.st_mtime > f2->stbuf.st_mtime))
				tag_it = TRUE;
			if (cmpi->cmp_older && (f1->stbuf.st_mtime < f2->stbuf.st_mtime))
				tag_it = TRUE;
			/*
			 * a and a' are files are identical if:
			 *   names are the same (already true)
			 *   sizes are the same
			 *   modification time of a is not later than  a'
			 *   modification time of a' is not later than the
			 *     creation time of a' (Note that if cp copies a to a'
			 *     then all times of a' are same, but we usually set the
			 *     atime & mtime to that of a, thus making mtime
			 *     less than ctime for a')
			 */
			if (cmpi->cmp_identical &&
				(f1->stbuf.st_size  == f2->stbuf.st_size) &&
				(f1->stbuf.st_mtime <= f2->stbuf.st_mtime) &&
				(f2->stbuf.st_mtime <= f2->stbuf.st_ctime) )
				tag_it = TRUE;
		}

		if (tag_it)
		{
			tag_file(f1);
			if (is_file_displayed(f1, &frow, &fcol))
				redisplay_file(f1, frow, fcol);
		}
	}
}

static int compare_file (void *data)
{
	CMP_INFO *cmpi = (CMP_INFO *)data;
	int c;
	int i;
	int fd1, fd2;
	unsigned char buf1[BUFSIZ];
	unsigned char buf2[BUFSIZ];
	int same;

	fblk_to_pathname(gbl(scr_cur)->cur_file, cmpi->path2);
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmp_cmpfil));
	if (gbl(scr_cur)->command_mode == m_file)
		zaddstr(gbl(win_commands), FULLNAME(cmpi->file_to_cmp));
	else
		zaddstr(gbl(win_commands), cmpi->path1);
	wmove(gbl(win_commands), 1, 0);
	waddstr(gbl(win_commands), msgs(m_cmp_wfil));
	if (gbl(scr_cur)->command_mode == m_file)
		zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_file));
	else
		zaddstr(gbl(win_commands), cmpi->path2);
	wrefresh(gbl(win_commands));

	/* check if same file */

	if (cmpi->file_to_cmp == gbl(scr_cur)->cur_file)
	{
		same = TRUE;
		goto set_tag;
	}

	/* check if links to each other */

	if (cmpi->file_to_cmp->stbuf.st_ino ==
			(gbl(scr_cur)->cur_file)->stbuf.st_ino &&
		cmpi->file_to_cmp->stbuf.st_dev ==
			(gbl(scr_cur)->cur_file)->stbuf.st_dev)
	{
		same = TRUE;
		goto set_tag;
	}

	/* check if same length */

	if (cmpi->file_to_cmp->stbuf.st_size !=
		(gbl(scr_cur)->cur_file)->stbuf.st_size)
	{
		same = FALSE;
		goto set_tag;
	}

	/* OK, we gotta read it */

	fd1 = os_open(cmpi->path1, O_RDONLY, 0666);
	if (fd1 == -1)
	{
		/* can't open file - treat as different */
		same = FALSE;
		goto set_tag;
	}

	fd2 = os_open(cmpi->path2, O_RDONLY, 0666);
	if (fd2 == -1)
	{
		os_close(fd1);
		/* can't open file - treat as different */
		same = FALSE;
		goto set_tag;
	}

	same = FALSE;
	while (1)
	{
		c = os_read(fd1, (char *)buf1, sizeof(buf1));
		if (c == 0)
		{
			same = TRUE;
			break;
		}
		if (c == -1)
		{
			/* i/o error - treat as different */
			break;
		}
		c = os_read(fd2, (char *)buf2, sizeof(buf2));
		if (c == -1)
		{
			/* i/o error - treat as different */
			break;
		}
		for (i=0; i<c; i++)
			if (buf1[i] != buf2[i])
				break;
		if (i < c)
			break;
	}
	os_close(fd1);
	os_close(fd2);

set_tag:
	if (same != cmpi->cmp_same)
		untag_file(gbl(scr_cur)->cur_file);
	return (1);
}

void do_cmp_dirs (void)
{
	CMP_INFO	cmp_info;
	CMP_INFO *	cmpi = &cmp_info;
	int c;
	TREE *dtptr;

	if (!(gbl(scr_cur)->cur_dir)->flist)
		return;

	cmpi->cmp_identical	= TRUE;
	cmpi->cmp_unique	= FALSE;
	cmpi->cmp_newer		= FALSE;
	cmpi->cmp_older		= FALSE;
	cmpi->cmp_same		= TRUE;

	bang("");
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmp_cmpdir));
	xaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_dir));
	xaddstr(gbl(win_commands), msgs(m_cmp_with));
	wrefresh(gbl(win_commands));
	strcpy(cmpi->path2, gbl(scr_cur)->path_name);
	c = do_dest_dir(cmpi->path2, FALSE, &dtptr);
	if (c < 0)
	{
		disp_cmds();
		return;
	}
	/* check if same directory specified */
	if (dtptr == gbl(scr_cur)->cur_dir_tree)
	{
		errmsg(ER_CCCD, "", ERR_ANY);
		disp_cmds();
		return;
	}

	/* ask for type of compare to do */

	while (TRUE)
	{
		wmove(gbl(win_commands), 1, 0);
		xaddstr(gbl(win_commands), msgs(m_cmp_tfta));

		xcaddstr(gbl(win_commands), CMDS_CMP_IDENTICAL, msgs(m_cmp_id));
		waddstr(gbl(win_commands), " ");
		if (cmpi->cmp_identical)
			xaddstr(gbl(win_commands), msgs(m_cmp_yes));
		else
			xaddstr(gbl(win_commands), msgs(m_cmp_no));

		xcaddstr(gbl(win_commands), CMDS_CMP_UNIQUE, msgs(m_cmp_un));
		waddstr(gbl(win_commands), " ");
		if (cmpi->cmp_unique)
			xaddstr(gbl(win_commands), msgs(m_cmp_yes));
		else
			xaddstr(gbl(win_commands), msgs(m_cmp_no));

		xcaddstr(gbl(win_commands), CMDS_CMP_NEWER, msgs(m_cmp_new));
		waddstr(gbl(win_commands), " ");
		if (cmpi->cmp_newer)
			xaddstr(gbl(win_commands), msgs(m_cmp_yes));
		else
			xaddstr(gbl(win_commands), msgs(m_cmp_no));

		xcaddstr(gbl(win_commands), CMDS_CMP_OLDER, msgs(m_cmp_old));
		waddstr(gbl(win_commands), " ");
		if (cmpi->cmp_older)
			xaddstr(gbl(win_commands), msgs(m_cmp_yes));
		else
			xaddstr(gbl(win_commands), msgs(m_cmp_no));
		wrefresh(gbl(win_commands));

		werase(gbl(win_message));
		ret_ok();
		esc_msg();
		bangnc(msgs(m_cmp_bang));

		while (TRUE)
		{
			c = xgetch(gbl(win_message));
			c = TO_LOWER(c);
			if (c == KEY_RETURN ||
				c == KEY_ESCAPE ||
				c == cmds(CMDS_CMP_IDENTICAL) ||
				c == cmds(CMDS_CMP_UNIQUE) ||
				c == cmds(CMDS_CMP_NEWER) ||
				c == cmds(CMDS_CMP_OLDER) )
				break;
		}

		if (c == KEY_ESCAPE)
		{
			disp_cmds();
			return;
		}
		else if (c == KEY_RETURN)
			break;
		else if (c == cmds(CMDS_CMP_IDENTICAL))
			cmpi->cmp_identical = !cmpi->cmp_identical;
		else if (c == cmds(CMDS_CMP_UNIQUE))
			cmpi->cmp_unique = !cmpi->cmp_unique;
		else if (c == cmds(CMDS_CMP_NEWER))
			cmpi->cmp_newer = !cmpi->cmp_newer;
		else if (c == cmds(CMDS_CMP_OLDER))
			cmpi->cmp_older = !cmpi->cmp_older;
	}

	cmp_dirs(cmpi, gbl(scr_cur)->cur_dir, (DBLK *)tid(dtptr));
	disp_cmds();
}

void do_comp_files (void)
{
	CMP_INFO	cmp_info;
	CMP_INFO *	cmpi = &cmp_info;
	int c;

	if (check_tag_count())
		return;

	cmpi->cmp_identical	= TRUE;
	cmpi->cmp_unique	= FALSE;
	cmpi->cmp_newer		= FALSE;
	cmpi->cmp_older		= FALSE;
	cmpi->cmp_same		= TRUE;

	cmpi->file_to_cmp = gbl(scr_cur)->cur_file;
	fblk_to_pathname(gbl(scr_cur)->cur_file, cmpi->path1);

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_cmp_cmpfil));
	waddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_file));
	xaddstr(gbl(win_commands), msgs(m_cmp_wtf));
	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_cmp_kftta));
	xcaddstr(gbl(win_commands), CMDS_CMP_SAME, msgs(m_cmp_same));
	xaddstr(gbl(win_commands), msgs(m_cmp_or));
	xcaddstr(gbl(win_commands), CMDS_CMP_DIFFERENT, msgs(m_cmp_diff));
	wrefresh(gbl(win_commands));
	bang(msgs(m_cmp_bang2));

	while (TRUE)
	{
		c = xgetch(gbl(win_message));
		c = TO_LOWER(c);
		if (c == KEY_ESCAPE)
		{
			disp_cmds();
			return;
		}
		if (c == KEY_RETURN)
			break;
		if (c == cmds(CMDS_CMP_SAME))
		{
			cmpi->cmp_same = TRUE;
			break;
		}
		if (c == cmds(CMDS_CMP_DIFFERENT))
		{
			cmpi->cmp_same = FALSE;
			break;
		}
	}

	traverse(compare_file, cmpi);
}
