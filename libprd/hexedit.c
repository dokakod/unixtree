/*------------------------------------------------------------------------
 * process the "hexedit" cmd
 */
#include "libprd.h"

static int hexedit_file_init (const char *path)
{
	char origpath[MAX_PATHLEN];
	char temppath[MAX_PATHLEN];
	FILE *fp;
	struct stat stbuf;
	unsigned char *disp_buf;
	unsigned char *orig_buf;
	int rc;

	/*----------------------------------------------------------------
	 * check if file is compressed & we want auto-decompression
	 */
	*temppath = 0;
	gbl(hxcb)->x_comp = -1;
	if (opt(exp_comp_files))
		gbl(hxcb)->x_comp = x_is_file_compressed(path);

	/*----------------------------------------------------------------
	 * if comppressed, uncompress it if needed
	 */
	strcpy(origpath, path);

	if (gbl(hxcb)->x_comp != -1)
	{
		os_make_temp_name(temppath, gbl(pgm_tmp), NULL);
		if (x_decomp(origpath, temppath, gbl(hxcb)->x_comp))
			return (-1);

		path = temppath;
	}

	/*----------------------------------------------------------------
	 * stat the file & get size
	 */
	rc = os_stat(path, &stbuf);
	if (rc)
	{
		errmsg(ER_CSN, "", ERR_ANY);
		return (-1);
	}

	if (stbuf.st_size == 0)
	{
		return (-1);
	}

	/*----------------------------------------------------------------
	 * open the file
	 */
	fp = fopen(path, "rb");
	if (fp == 0)
	{
		errmsg(ER_COF, "", ERR_ANY);
		return (-1);
	}

	/*----------------------------------------------------------------
	 * allocate buffers
	 */
	disp_buf = (unsigned char *)MALLOC(stbuf.st_size);
	if (disp_buf == 0)
	{
		fclose(fp);
		errmsg(ER_IM, "", ERR_ANY);
		return (-1);
	}

	orig_buf = (unsigned char *)MALLOC(stbuf.st_size);
	if (orig_buf == 0)
	{
		fclose(fp);
		FREE(disp_buf);
		errmsg(ER_IM, "", ERR_ANY);
		return (-1);
	}

	/*----------------------------------------------------------------
	 * read in the file
	 */
	rc = fread(orig_buf, stbuf.st_size, 1, fp);
	fclose(fp);
	if (rc != 1)
	{
		FREE(orig_buf);
		FREE(disp_buf);
		errmsg(ER_CREAD, "", ERR_ANY);
		return (-1);
	}

	/*----------------------------------------------------------------
	 * we can now setup the edit
	 */
	memcpy(disp_buf, orig_buf, stbuf.st_size);

	strcpy(gbl(hxcb)->x_pathname, path);
	strcpy(gbl(hxcb)->x_dispname, origpath);
	strcpy(gbl(hxcb)->x_origname, origpath);

	gbl(hxcb)->x_filesize	= stbuf.st_size;
	gbl(hxcb)->x_top_offs	= 0;
	gbl(hxcb)->x_cur_offs	= 0;

	gbl(hxcb)->x_cur_y		= 0;
	gbl(hxcb)->x_cur_x		= 0;

	gbl(hxcb)->x_hex		= TRUE;
	gbl(hxcb)->x_dirty		= FALSE;
	gbl(hxcb)->x_left		= TRUE;

	gbl(hxcb)->x_disp_buf	= disp_buf;
	gbl(hxcb)->x_orig_buf	= orig_buf;

	return (0);
}

void hexedit_run (const char *path)
{
	/*--------------------------------------------------------------------
	 * setup display if we can
	 */
	if (hexedit_file_init(path))
		return;

	/*--------------------------------------------------------------------
	 * setup was successful, so enter hexedit-mode
	 */
	gbl(scr_in_hexedit) = TRUE;

	setup_display();
}

void hexedit_file_end (void)
{
	char origpath[MAX_PATHLEN];
	int was_dirty = gbl(hxcb)->x_dirty;

	/*----------------------------------------------------------------
	 * if file is changed, ask for confirmation of save
	 */
	if (was_dirty)
	{
		int c;

		werase(gbl(win_message));
		esc_msg();
		wmove (gbl(win_message), 0, 0);
		xaddstr(gbl(win_message), msgs(m_hex_save));
		c = yesno();
		if (c < 0)
			return;
		if (c > 0)
			was_dirty = FALSE;
	}

	/*----------------------------------------------------------------
	 * if file is changed, write it back out
	 */
	if (was_dirty)
	{
		FILE *fp;

		fp = fopen(gbl(hxcb)->x_pathname, "wb");
		if (fp == 0)
		{
		}
		else
		{
			fwrite(gbl(hxcb)->x_disp_buf, gbl(hxcb)->x_filesize, 1, fp);
			fclose(fp);

			if (gbl(hxcb)->x_comp != -1)
			{
				x_comp(gbl(hxcb)->x_pathname, gbl(hxcb)->x_origname,
					gbl(hxcb)->x_comp);
				os_file_delete(gbl(hxcb)->x_pathname);
			}

			strcpy(origpath, gbl(hxcb)->x_origname);
		}
	}

	/*----------------------------------------------------------------
	 * free the buffers
	 */
	FREE(gbl(hxcb)->x_disp_buf);
	FREE(gbl(hxcb)->x_orig_buf);

	/*----------------------------------------------------------------
	 * now clear the struct
	 */
	gbl(hxcb)->x_pathname[0]	= 0;
	gbl(hxcb)->x_dispname[0]	= 0;
	gbl(hxcb)->x_origname[0]	= 0;
	gbl(hxcb)->x_filesize		= 0;
	gbl(hxcb)->x_top_offs		= 0;
	gbl(hxcb)->x_cur_offs		= 0;
	gbl(hxcb)->x_cur_y			= 0;
	gbl(hxcb)->x_cur_x			= 0;
	gbl(hxcb)->x_hex			= TRUE;
	gbl(hxcb)->x_dirty			= FALSE;
	gbl(hxcb)->x_left			= TRUE;
	gbl(hxcb)->x_comp			= -1;
	gbl(hxcb)->x_disp_buf		= 0;
	gbl(hxcb)->x_orig_buf		= 0;

	/*----------------------------------------------------------------
	 * now reset env
	 */
	gbl(scr_in_hexedit) = FALSE;

	if (gbl(scr_in_fullview) || gbl(scr_in_autoview))
		open_av_file(gbl(scr_cur)->cur_file);
	setup_display();
	disp_cmds();

	/*----------------------------------------------------------------
	 * if file changed, reload its info
	 */
	if (was_dirty)
		check_the_file(origpath);
}

void hexedit_setup_display (void)
{
	win_clock_set(FALSE);
	fv_border();
	disp_path_line();
	werase(gbl(win_av_mode_reg));
	wrefresh(gbl(win_av_mode_reg));
	hexedit_redisplay();
	touchwin(stdscr);
	wrefresh(stdscr);
	disp_cmds();
}

void hexedit_set_win (HXCB *h, WINDOW *win)
{
	/*--------------------------------------------------------------------
	 * store new window pointer & calculate width/height/#-chars
	 */
	h->x_win	= win;

	h->x_width	= (((getmaxx(win) - 9) * 4) / 17) & ~3;
	h->x_lines	= getmaxy(win);
	h->x_chars	= (h->x_width * h->x_lines);

	/*--------------------------------------------------------------------
	 * adjust top-left offset to be a multiple of the width
	 */
	h->x_top_offs = (h->x_top_offs / h->x_width) * h->x_width;

	/*--------------------------------------------------------------------
	 * adjust top-left offset to be sure the current offset is displayed
	 */
	if (h->x_cur_offs < h->x_top_offs)
	{
		h->x_top_offs -= h->x_width;
		if (h->x_top_offs < 0)
			h->x_top_offs = 0;
	}
}

HXCB * hexedit_init (WINDOW *win)
{
	HXCB *h;

	/*--------------------------------------------------------------------
	 * allocate hxcb object
	 */
	h = (HXCB *)MALLOC(sizeof(*h));
	if (h == 0)
		return (0);

	/*--------------------------------------------------------------------
	 * clear the struct & set window parameters
	 */
	memset(h, 0, sizeof(*h));
	hexedit_set_win(h, win);

	return (h);
}

HXCB * hexedit_end (HXCB *h)
{
	/*--------------------------------------------------------------------
	 * free the struct if ever allocated
	 */
	if (h)
	{
		if (h->x_disp_buf != 0)
			FREE(h->x_disp_buf);

		if (h->x_orig_buf != 0)
			FREE(h->x_orig_buf);

		FREE(h);
	}

	return (0);
}
