/*------------------------------------------------------------------------
 * display file window
 */
#include "libprd.h"

static char *disp_owner (FBLK *f, char *buf)
{
	NBLK *n;
	char *s;

	n = get_root_of_file(f);

	if (n->node_type == N_FTP)
		s = get_ftp_owner(n, f->stbuf.st_uid);
	else
		s = get_owner(f->stbuf.st_uid, buf);

	return (s);
}

static char *disp_group (FBLK *f, char *buf)
{
	NBLK *n;
	char *s;

	n = get_root_of_file(f);

	if (n->node_type == N_FTP)
		s = get_ftp_group(n, f->stbuf.st_gid);
	else
		s = get_group(f->stbuf.st_gid, buf);

	return (s);
}

static char * format_size (FBLK *f, char *buf)
{
	int mode;
	char nbuf[12];

	mode = f->stbuf.st_mode & S_IFMT;
	switch (mode)
	{
	case S_IFCHR:
	case S_IFBLK:
		strcpy(buf, "    ");
		strcat(buf, xform2(nbuf, get_major(&f->stbuf), 3));
		strcat(buf, ",");
		strcat(buf, xform2(nbuf, get_minor(&f->stbuf), 3));
		break;
	default:
		strcpy(buf, xform(nbuf, f->stbuf.st_size));
		break;
	}

	return (buf);
}

void disp_file_line (FBLK *f)
{
	char file_line[256];
	char *l = file_line;
	int ftype;
	char *p;
	time_t tt;
	int code = CMDS_COMMON_MOUSEPOS;
	attr_t save_attr = A_UNSET;
	attr_t file_attr = A_UNSET;
	char nbuf[16];
	char dl[24];

	/*--------------------------------------------------------------------
	 * First we create the entire line to be displayed
	 */

	/*--------------------------------------------------------------------
	 * Display file-type-char, name, tag & space.
	 * These are common to all file displays (split & non-split).
	 */
	switch (f->stbuf.st_mode & S_IFMT)
	{
	case S_IFBLK:	ftype = pgm_const(blk_display_char);	break;
	case S_IFCHR:	ftype = pgm_const(chr_display_char);	break;
	case S_IFIFO:	ftype = pgm_const(ffo_display_char);	break;
	case S_IFNAM:	ftype = pgm_const(nam_display_char);	break;
	case S_IFLNK:	ftype = (f->sym_mode == 0) ?
							pgm_const(syd_display_char) :
							pgm_const(sym_display_char);	break;
	case S_IFSOCK:	ftype = pgm_const(sck_display_char);	break;
	case S_IFDOOR:	ftype = pgm_const(dor_display_char);	break;
	default:		ftype = pgm_const(reg_display_char);	break;
	}

	*l++ = ftype;
	l+= fill_fw_str(l, FULLNAME(f), gbl(scr_cur)->file_disp_len);
	if (is_file_tagged(f))
		*l++ = pgm_const(tag_disp_char);
	else
		*l++ = ' ';
	*l++ = ' ';

	switch (gbl(scr_cur)->file_fmt)
	{
	case fmt_fname1:
	case fmt_fname3:
		break;

	case fmt_fsize:
		l += istrcpy(l, format_size(f, nbuf));
		break;

	case fmt_fdate:
		switch (opt(date_type))
		{
		case date_modified:
			tt = f->stbuf.st_mtime;
			break;
		case date_accessed:
			tt = f->stbuf.st_atime;
			break;
		case date_created:
			tt = f->stbuf.st_ctime;
			break;
		}

		if (gbl(scr_is_split))
		{
			l += istrcpy(l, xform2(nbuf, f->stbuf.st_nlink, 3));
			*l++ = ' ';
			l += istrcpy(l, xform2(nbuf, f->stbuf.st_ino, 5));
			*l++ = ' ';
			p = date_to_str(dl, tt);
			p[8] = 0;				/* date only, no time */
			l += istrcpy(l, p);
		}
		else
		{
			l += istrcpy(l, format_size(f, nbuf));
			*l++ = ' ';
			l += istrcpy(l, xform2(nbuf, f->stbuf.st_nlink, 3));
			*l++ = ' ';
			l += istrcpy(l, xform2(nbuf, f->stbuf.st_ino, 5));
			*l++ = ' ';
			l += istrcpy(l, date_to_str(dl, tt));
		}
		break;

	case fmt_fperms:
		if (gbl(scr_is_split))
		{
			l += istrcpy(l, perm_str(f->stbuf.st_mode, nbuf));
			*l++ = ' ';
			l += istrcpy(l, disp_owner(f, nbuf));
		}
		else
		{
			l += istrcpy(l,  format_size(f, nbuf));
			*l++ = ' ';
			l += istrcpy(l, perm_str(f->stbuf.st_mode, nbuf));
			*l++ = ' ';
			l += istrcpy(l, disp_owner(f, nbuf));
			*l++ = ' ';
			l += istrcpy(l, disp_group(f, nbuf));
		}
		break;
	}
	*l = 0;

	/*--------------------------------------------------------------------
	 * Now we display the line
	 */

	/*--------------------------------------------------------------------
	 * check if doing file colors & file not highlighted
	 */
	if (! getstmode(gbl(scr_cur)->cur_file_win) )
	{
		file_attr = get_file_attr(gbl(scr_cur)->cur_file_win, f);
		if (file_attr != A_UNSET)
		{
			save_attr = wattrget(gbl(scr_cur)->cur_file_win);
			if ((file_attr & (FA_RAINBOW | FA_RANDOM)) == 0)
				wattrset(gbl(scr_cur)->cur_file_win, file_attr);
		}
	}

	if (file_attr == A_UNSET || (file_attr & (FA_RAINBOW | FA_RANDOM)) == 0)
	{
		/*----------------------------------------------------------------
		 * normal display
		 * (file-type char not displayed with mouse-code)
		 */
		waddch(gbl(scr_cur)->cur_file_win, *file_line);
		wcaddstr(gbl(scr_cur)->cur_file_win, file_line+1, code);
	}
	else
	{
		/*----------------------------------------------------------------
		 * "rainbow" or "random" display (each char in different color)
		 */
		int f = A_FG_CLRNUM(file_attr);
		int b = A_BG_CLRNUM(wattrget(gbl(scr_cur)->cur_file_win));
		int i;

		for (i=0; file_line[i]; i++)
		{
			if (file_attr & FA_RAINBOW)
			{
				/*--------------------------------------------------------
				 * rainbow:	bump to next color that is not same as bg
				 */
				if (f == b)
				{
					f++;
					if (f >= NUM_COLORS)
						f = 0;
				}
				wattrset(gbl(scr_cur)->cur_file_win, A_CLR(f, b));
				f++;
				if (f >= NUM_COLORS)
					f = 0;
			}
			else
			{
				/*--------------------------------------------------------
				 * random:	get a random color
				 */
				int	max_rand = 0x7fff;
				int	fg;

				for (fg = b; fg == b || fg == f; )
				{
					fg = (((rand() & max_rand) * NUM_COLORS) / max_rand);
				}
				wattrset(gbl(scr_cur)->cur_file_win, A_CLR(fg, b));
				f = fg;
			}

			/*------------------------------------------------------------
			 * file-type char not displayed with mouse-code
			 */
			if (i == 0)
				waddch(gbl(scr_cur)->cur_file_win, file_line[i]);
			else
				wcaddch(gbl(scr_cur)->cur_file_win, file_line[i], code);
		}
	}

	if (save_attr != A_UNSET)
		wattrset(gbl(scr_cur)->cur_file_win, save_attr);
}

static void disp_file_list_no_refresh (void)
{
	int l;
	int c;
	BLIST *b;
	FBLK *f;

	werase(gbl(scr_cur)->cur_file_win);
	if (gbl(scr_cur)->top_file)
	{
		l = 0;
		c = 0;
		for (b=gbl(scr_cur)->top_file; b; b=bnext(b))
		{
			f = (FBLK *)bid(b);
			wmove(gbl(scr_cur)->cur_file_win,
				l, c*gbl(scr_cur)->file_line_width);
			disp_file_line(f);
			if (++l > gbl(scr_cur)->max_file_line)
			{
				l = 0;
				c++;
				if (c > gbl(scr_cur)->max_file_col)
					break;
			}
		}
	}
	else
	{
		werase(gbl(scr_cur)->cur_file_win);
		wmove(gbl(scr_cur)->cur_file_win, 0, 0);
		if (gbl(scr_cur)->cur_dir->flags & D_CANT_LOG)
			waddstr(gbl(scr_cur)->cur_file_win, msgs(m_dispfile_cantlog));
		else if (gbl(scr_cur)->cur_dir->flags & D_NOT_LOGGED)
			waddstr(gbl(scr_cur)->cur_file_win, msgs(m_dispfile_notlog));
		else if (gbl(scr_cur)->cur_dir->flist)
			waddstr(gbl(scr_cur)->cur_file_win, msgs(m_dispfile_nomat));
		else
			waddstr(gbl(scr_cur)->cur_file_win, msgs(m_dispfile_nofil));
	}
}

void disp_file_list (void)
{
	disp_file_list_no_refresh();
	wrefresh(gbl(scr_cur)->cur_file_win);
}

void hilite_file (int on_off)
{
	wmove(gbl(scr_cur)->cur_file_win, gbl(scr_cur)->cur_file_line,
		gbl(scr_cur)->cur_file_col*gbl(scr_cur)->file_line_width);
	if (on_off)
		wstandout(gbl(scr_cur)->cur_file_win);
	disp_file_line(gbl(scr_cur)->cur_file);
	if (on_off)
		wstandend(gbl(scr_cur)->cur_file_win);
	wrefresh(gbl(scr_cur)->cur_file_win);
}

static const int  masks[] =
{
	S_IRUSR, S_IWUSR, S_IXUSR,
	S_IRGRP, S_IWGRP, S_IXGRP,
	S_IROTH, S_IWOTH, S_IXOTH
};

static const char names[] =
{
	'r',	'w',	'x',
	'r',	'w',	'x',
	'r',	'w',	'x'
};

char *perm_str (int mode, char *buf)
{
	int i;

	if (mode == -1)
		return (msgs(m_dispfile_noperms));

	for (i=0; i<sizeof(names); i++)
		if (mode & masks[i])
			buf[i] = names[i];
		else
			buf[i] = '-';

	/* special cases */

	if (mode & S_ISUID)
		buf[2] = (mode & S_IXUSR)? 's': 'S';
	if (mode & S_ISGID)
		buf[5] = (mode & S_IXGRP)? 's': 'S';
	if (mode & S_ISVTX)
		buf[8] = (mode & S_IXOTH)? 't': 'T';

	buf[9] = 0;

	return  (buf);
}

void check_small_window (void)
{
	gbl(scr_cur)->cur_file_win = gbl(scr_cur)->small_file_win;
	small_border(ON);
	start_file_display();
}

void start_file_display (void)
{
	gbl(scr_cur)->base_file   = (gbl(scr_cur)->cur_dir)->flist;
	gbl(scr_cur)->first_file  =
		(gbl(scr_cur)->cur_dir)->mlist[gbl(scr_cur_no)];
	gbl(scr_cur)->file_cursor = gbl(scr_cur)->first_file;
	gbl(scr_cur)->top_file = gbl(scr_cur)->file_cursor;
	if (gbl(scr_cur)->file_cursor)
		gbl(scr_cur)->cur_file = (FBLK *)bid(gbl(scr_cur)->file_cursor);
	gbl(scr_cur)->cur_file_no   = 0;
	gbl(scr_cur)->cur_file_line = 0;
	gbl(scr_cur)->cur_file_col  = 0;
	gbl(scr_cur)->numfiles = bcount(gbl(scr_cur)->first_file);
	gbl(scr_cur)->max_file_line = getmaxy(gbl(scr_cur)->cur_file_win)-1;
	werase(gbl(scr_cur)->cur_file_win);
	disp_file_list();
	if (gbl(scr_cur)->numfiles)
		do_file_scroll_bar();
	else
		file_scroll_bar_remove();
}

void file_scroll_bar_init (void)
{
	scrollbar(gbl(win_border), gbl(scr_cur)->cur_file_win, SCRLBAR_VL, -1, 0);
	gbl(scr_cur)->file_scroll_bar_displayed = TRUE;
	gbl(scr_cur)->file_scroll_bar = -1;
}

void file_scroll_bar_remove (void)
{
	int i;
	int y;
	int x;

	y = getbegy(gbl(scr_cur)->cur_file_win) - getbegy(gbl(win_border));
	x = getbegx(gbl(scr_cur)->cur_file_win) - getbegx(gbl(win_border));
	for (i=0; i<getmaxy(gbl(scr_cur)->cur_file_win); i++)
	{
		wmove(gbl(win_border), y+i, x-1);
		wbox_chr(gbl(win_border), B_VE, pgm_const(border_type));
	}
	wrefresh(gbl(win_border));
	gbl(scr_cur)->file_scroll_bar_displayed = FALSE;
	gbl(scr_cur)->file_scroll_bar = -1;
}

void do_file_scroll_bar (void)
{
	if (!gbl(scr_cur)->numfiles)
		return;

	if (!gbl(scr_cur)->file_scroll_bar_displayed)
		file_scroll_bar_init();

	if (gbl(scr_cur)->cur_file_no != gbl(scr_cur)->file_scroll_bar)
	{
		scrollbar(gbl(win_border), gbl(scr_cur)->cur_file_win, SCRLBAR_VL,
			gbl(scr_cur)->cur_file_no, gbl(scr_cur)->numfiles);
		gbl(scr_cur)->file_scroll_bar = gbl(scr_cur)->cur_file_no;
		wrefresh(gbl(win_border));
	}
}
