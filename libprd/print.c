/*------------------------------------------------------------------------
 * process the "print" cmd
 */
#include "libprd.h"

PRINTER * printer_open (const char *name, char *msgbuf)
{
	PRINTER *		p;
	const char *	n;

	if (name == 0)
		name = "";

	switch (*name)
	{
	case '|':
	case '>':
		for (n = name+1; *n; n++)
		{
			if (! isspace(*n))
				break;
		}
		if (*n == 0)
			return (0);

		p = prt_open(*name == '|' ? P_PIPE : P_FILE, n, msgbuf);
		break;

	default:
		p = prt_open(P_SPOOL, name, msgbuf);
		break;
	}

	return (p);
}

static int do_eject (void)
{
	PRINT_INFO *	pi	= &gbl(print_info);
	int				i;

	for (i=pi->linecnt; i<opt(page_length); i++)
		prt_output_str(pi->prt, "\n");
	for (i=0; i<opt(page_margin); i++)
		prt_output_str(pi->prt, "\n");

	return (0);
}

static int do_header (const char *prefix, const char *name)
{
	PRINT_INFO *	pi	= &gbl(print_info);
	char			nambuf[MAX_PATHLEN];
	char			dbuf[12];
	int				i;
	int				l;

	/* get max width of header name */

	l = opt(page_width) - strlen(prefix) - 17 - strlen(msgs(m_print_pag)) - 3;

	strcpy(nambuf, name);
	nambuf[l] = 0;

	if (pi->linecnt)
		do_eject();
	prt_output_str(pi->prt, prefix);
	prt_output_str(pi->prt, nambuf);
	i = strlen(nambuf);
	for (; i<l; i++)
		prt_output_str(pi->prt, " ");
	prt_output_str(pi->prt, pi->date_string);
	prt_output_str(pi->prt, msgs(m_print_pag));
	prt_output_str(pi->prt, xform(dbuf, ++pi->page_no)+8);
	prt_output_str(pi->prt, "\n\n\n");
	pi->linecnt = 3;

	return (0);
}

static int do_node_header (void)
{
	do_header(msgs(m_print_nod), (gbl(scr_cur)->cur_root)->root_name);

	return (0);
}

static int do_file_header (const char *name)
{
	do_header(msgs(m_print_pat), name);

	return (0);
}

static void print_dir_level (TREE *t)
{
	PRINT_INFO *	pi	= &gbl(print_info);
	TREE *			p = tparent(t);

	/*--------------------------------------------------------------------
	 * print sub-dir if not root of node
	 */
	if (p != 0 && tparent(p) != 0)
	{
		/*----------------------------------------------------------------
		 * recurse up to root
		 */
		print_dir_level(p);

		/*----------------------------------------------------------------
		 * now print this sub-dir on the way down
		 */
		if (tright(p) != 0)
			prt_output_str(pi->prt, "|");
		else
			prt_output_str(pi->prt, " ");
		prt_output_str(pi->prt, "  ");
	}
}

static int print_dir_line (BLIST *dir_ptr)
{
	PRINT_INFO *	pi	= &gbl(print_info);
	TREE *			t;
	DBLK *			d;

	t = (TREE *)bid(dir_ptr);
	d = (DBLK *)tid(t);
	if (tparent(t) != 0)
	{
		print_dir_level(t);

		if (tright(t) != 0)
			prt_output_str(pi->prt, "|");
		else
			prt_output_str(pi->prt, "+");
		prt_output_str(pi->prt, "--");
	}
	prt_output_str(pi->prt, FULLNAME(d));
	prt_output_str(pi->prt, "\n");

	return (0);
}

static int make_file_line (char *line, FBLK *f)
{
	int		info_len = 67;		/* length of info part of line */
	int		name_len = opt(page_width) - info_len - 1;
	char *	p;
	int		i;
	char	nbuf[16];
	char	dl[24];

	p = FULLNAME(f);
	for (i=0; p[i]; i++)
	{
		if (i == name_len)
		{
			line[i] = pgm_const(too_long_char);
			break;
		}
		else
		{
			line[i] = p[i];
		}
	}
	line[i++] = ' ';
	for (; i<name_len-1; i++)
		line[i] = ' ';
	line[i] = 0;

	strcat(line, " ");
	strcat(line, xform(nbuf, f->stbuf.st_size));
	strcat(line, " ");
	strcat(line, date_to_str(dl, f->stbuf.st_mtime));
	strcat(line, " ");
	strcat(line, perm_str(f->stbuf.st_mode, nbuf));
	strcat(line, " ");
	strcat(line, get_owner(f->stbuf.st_uid, nbuf));
	strcat(line, " ");
	strcat(line, get_group(f->stbuf.st_gid, nbuf));

	return (0);
}

static int print_file_line (FBLK *f)
{
	PRINT_INFO *	pi	= &gbl(print_info);
	char			line[MAX_FILELEN + 60];

	make_file_line(line, f);
	prt_output_str(pi->prt, "      ");
	prt_output_str(pi->prt, line);
	prt_output_str(pi->prt, "\n");

	return (0);
}

static int print_the_file (void *data)
{
	char	path[MAX_PATHLEN];
	FBLK *	f = (gbl(scr_cur)->cur_file);

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_print_fil1));
	zaddstr(gbl(win_commands), FULLNAME(f));
	wrefresh(gbl(win_commands));

	fblk_to_pathname(f, path);
	return (print_path(path, path));
}

static int print_open (void)
{
	PRINT_INFO *	pi	= &gbl(print_info);
	char			msgbuf[256];
	struct stat		stbuf;

	if (*opt(print_filename) != 0)
	{
		if (fn_is_path_absolute(opt(print_filename)))
		{
			strcpy(pi->print_name, opt(print_filename));
		}
		else
		{
			strcpy(pi->print_name, gbl(pgm_cwd));
			fn_append_filename_to_dir(pi->print_name, opt(print_filename));
		}

		if (os_stat(pi->print_name, &stbuf) == 0)
		{
			if (S_ISDIR(stbuf.st_mode))
			{
				errmsg(ER_PFD, "", ERR_ANY);
				return (-1);
			}
			if (! can_we_write(&stbuf))
			{
				errmsg(ER_NPWPF, "", ERR_ANY);
				return (-1);
			}
		}
		pi->prt = prt_open(P_FILE, pi->print_name, msgbuf);
	}
	else
	{
		pi->prt = printer_open(opt(printer), msgbuf);
	}

	if (pi->prt == 0)
	{
		errmsg(ER_COP, msgbuf, ERR_ANY);
		return (-1);
	}

	return (0);
}

void print_close (int exit_flag)
{
	PRINT_INFO *	pi	= &gbl(print_info);
	char			msgbuf[256];
	int				rc;

	/*--------------------------------------------------------------------
	 * don't close if close-on-exit flag on & not at exit
	 */
	if (! exit_flag && opt(close_printer_at_exit))
		return;

	/*--------------------------------------------------------------------
	 * close the printer
	 */
	if (pi->prt != 0)
	{
		rc = prt_close(pi->prt, msgbuf);
		pi->prt = 0;

		if (rc && ! exit_flag)
		{
			errmsg(ER_COP, msgbuf, ERR_ANY);
		}
	}

	/*--------------------------------------------------------------------
	 * if not exiting, check the print filename
	 */
	if (! exit_flag)
	{
		if (*opt(print_filename) != 0)
			check_the_file(pi->print_name);
	}
}

void print_scrn (void)
{
	PRINT_INFO *	pi	= &gbl(print_info);

	if (pi->prt == 0)
		print_open();

	if (pi->prt != 0)
		print_win(curscr, pi->prt);

	print_close(FALSE);
}

void print_file (void)
{
	char	input_str[MAX_PATHLEN];
	char	buffer[MAX_PATHLEN];
	int		c;

	bang(msgs(m_print_entfil));
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_print_fil2));
	wrefresh(gbl(win_commands));
	strcpy(input_str, FULLNAME(gbl(scr_cur)->cur_file));
	c = xgetstr(gbl(win_commands), input_str, XGL_PRT_FILE, MAX_PATHLEN, 0,
		XG_FILEPATH);
	if (c <= 0)
	{
		disp_cmds();
		return;
	}

	fn_get_abs_path(gbl(scr_cur)->path_name, input_str, buffer);
	print_file_with_hdr(input_str, buffer, buffer);
}

void print_file_with_hdr (const char *file, const char *path, const char *hdr)
{
	if (print_init())
		return;

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_print_fil1));
	zaddstr(gbl(win_commands), file);
	wrefresh(gbl(win_commands));

	print_path(path, hdr);
	print_close(FALSE);
	disp_cmds();
}

int print_init (void)
{
	PRINT_INFO *	pi	= &gbl(print_info);

	date_to_str(pi->date_string, time(0));
	pi->page_no = 0;

	return (print_open());
}

void do_tag_print (void)
{
	PRINT_INFO *	pi	= &gbl(print_info);
	int				c;

	if (check_tag_count())
		return;

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_print_atf));
	wrefresh(gbl(win_commands));
	c = yesno_msg(msgs(m_print_pri));
	if (c)
		return;

	if (print_init())
		return;

	traverse(print_the_file, pi);

	print_close(FALSE);
	disp_cmds();
}

int print_path (const char *path, const char *hdr)
{
	PRINT_INFO *	pi	= &gbl(print_info);
	FILE *			inp;
	char			line[BUFSIZ];
	struct stat		stbuf;
	int				c;

	if (os_stat(path, &stbuf))
	{
		c = errmsg(ER_FDNE, "", ERR_ANY);
		return (c);
	}
	if (! S_ISREG(stbuf.st_mode))
	{
		c = errmsg(ER_CPSF, "", ERR_ANY);
		return (c);
	}
	if (!can_we_read(&stbuf))
	{
		c = errmsg(ER_NPTRF, "", ERR_ANY);
		return (c);
	}
	inp = fopen(path, "rb");
	if (!inp)
	{
		c = errsys(ER_COF);
		return (c);
	}
	pi->linecnt = 0;
	if (opt(page_length))
		do_file_header(hdr);
	while (fgets(line, sizeof(line), inp))
	{
		if (opt(page_length) && (pi->linecnt >= opt(page_length)))
			do_file_header(hdr);
		pi->linecnt++;
		prt_output_str(pi->prt, line);
	}
	do_eject();
	fclose(inp);

	return (1);
}

void do_dir_print (void)
{
	PRINT_INFO *	pi	= &gbl(print_info);
	int				c;
	long			size;
	time_t			now;
	char *			xptr;
	BLIST *			b;
	BLIST *			x;
	TREE *			t;
	DBLK *			d;
	FBLK *			f;
	char			path[MAX_PATHLEN];
	char			dbuf[12];

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_print_pc));
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_PRINT_CAT, msgs(m_print_cat));
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_PRINT_PATH, msgs(m_print_names));
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_PRINT_TREE, msgs(m_print_tree));
	wrefresh(gbl(win_commands));
	werase(gbl(win_message));
	esc_msg();
	bangnc(msgs(m_print_entpri));
	while (TRUE)
	{
		c = xgetch(gbl(win_message));
		c = TO_LOWER(c);
		if (c == KEY_ESCAPE ||
			c == KEY_RETURN ||
		    c == cmds(CMDS_PRINT_PATH) ||
			c == cmds(CMDS_PRINT_TREE) ||
			c == cmds(CMDS_PRINT_CAT))
			break;
	}
	bang("");
	if (c == KEY_ESCAPE || c == KEY_RETURN)
	{
		disp_cmds();
		return;
	}
	now = time((time_t *)0);
	date_to_str(pi->date_string, now);
	gbl(scr_cur_sub_cmd) = c;
	if (c == cmds(CMDS_PRINT_PATH))
	{
		gbl(scr_cur_sub_cmd) = CMDS_PRINT_PATH;
		werase(gbl(win_commands));
		wmove(gbl(win_commands), 0, 0);
		xaddstr(gbl(win_commands), msgs(m_print_pat));
		wrefresh(gbl(win_commands));
		if (print_open())
			return;
		pi->page_no = 0;
		pi->linecnt = 0;
		if (opt(page_length))
			do_node_header();
		for (b=gbl(scr_cur)->first_dir; b; b=bnext(b))
		{
			if (opt(page_length) && (pi->linecnt >= opt(page_length)-1))
				do_node_header();
			pi->linecnt++;
			dptr_to_dirname(b, path);
			prt_output_str(pi->prt, path);
			prt_output_str(pi->prt, "\n");
		}
		do_eject();
		print_close(FALSE);
	}
	else if (c == cmds(CMDS_PRINT_TREE))
	{
		gbl(scr_cur_sub_cmd) = CMDS_PRINT_TREE;
		werase(gbl(win_commands));
		wmove(gbl(win_commands), 0, 0);
		xaddstr(gbl(win_commands), msgs(m_print_dt));
		wrefresh(gbl(win_commands));
		if (print_open())
			return;
		pi->page_no = 0;
		pi->linecnt = 0;
		if (opt(page_length))
			do_node_header();
		for (b=gbl(scr_cur)->first_dir; b; b=bnext(b))
		{
			if (opt(page_length) && (pi->linecnt >= opt(page_length)-1))
				do_node_header();
			pi->linecnt++;
			print_dir_line(b);
		}
		do_eject();
		print_close(FALSE);
	}
	else if (c == cmds(CMDS_PRINT_CAT))
	{
		gbl(scr_cur_sub_cmd) = CMDS_PRINT_CAT;
		werase(gbl(win_commands));
		wmove(gbl(win_commands), 0, 0);
		xaddstr(gbl(win_commands), msgs(m_print_ctf));
		wrefresh(gbl(win_commands));
		if (print_open())
			return;
		pi->page_no = 0;
		pi->linecnt = 0;
		if (opt(page_length))
			do_node_header();
		if ((gbl(scr_cur)->cur_root)->node_tagged_count == 0)
		{
			prt_output_str(pi->prt, msgs(m_print_ntf));
			prt_output_str(pi->prt, "\n");
		}
		else
		{
			for (b=gbl(scr_cur)->first_dir; b; b=bnext(b))
			{
				t = (TREE *)bid(b);
				d = (DBLK *)tid(t);
				c = 0;
				size = 0;
				for (x=d->flist; x; x=bnext(x))
				{
					f = (FBLK *)bid(x);
					if (is_file_tagged(f))
					{
						c++;
						size += f->stbuf.st_size;
					}
				}
				if (c)
				{
					if (opt(page_length) &&
					    (pi->linecnt+4 >= opt(page_length)-1))
					{
						do_node_header();
					}
					prt_output_str(pi->prt, "\n");
					prt_output_str(pi->prt, msgs(m_print_pat));
					dptr_to_dirname(b, path);
					prt_output_str(pi->prt, path);
					prt_output_str(pi->prt, "\n");
					prt_output_str(pi->prt, "      ");
					xptr = xform(dbuf, c);
					while (*xptr == ' ')
						xptr++;
					prt_output_str(pi->prt, xptr);
					prt_output_str(pi->prt, msgs(m_print_tagfil));
					xptr = xform(dbuf, size);
					while (*xptr == ' ')
						xptr++;
					prt_output_str(pi->prt, xptr);
					prt_output_str(pi->prt, msgs(m_print_byt));
					prt_output_str(pi->prt, "\n\n");
					pi->linecnt += 4;
					for (x=d->flist; x; x=bnext(x))
					{
						f = (FBLK *)bid(x);
						if (is_file_tagged(f))
						{
							if (opt(page_length) &&
								(pi->linecnt >= opt(page_length)-1))
							{
								do_node_header();
							}
							pi->linecnt++;
							print_file_line(f);
						}
					}
				}
			}
		}
		do_eject();
		print_close(FALSE);
	}
	disp_cmds();
}
