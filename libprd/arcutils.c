/*------------------------------------------------------------------------
 * archive utilities
 */
#include "libprd.h"

static ARCH_DEV * arch_find_entry_by_name (const char *name)
{
	BLIST *b;

	for (b=gbl(arch_dev_list); b; b=bnext(b))
	{
		ARCH_DEV *a = (ARCH_DEV *)bid(b);

		if (strcmp(name, a->d_name) == 0)
			return (a);
	}

	return (0);
}

static ARCH_DEV * arch_find_entry_by_device (const char *device)
{
	BLIST *b;

	for (b=gbl(arch_dev_list); b; b=bnext(b))
	{
		ARCH_DEV *a = (ARCH_DEV *)bid(b);

		if (strcmp(device, a->d_device) == 0)
			return (a);
	}

	return (0);
}

static void set_vals_from_arch_entry (ARCH_DEV *a)
{
	ai->a_blkfactor = a->d_blkfactor;
	if (!ai->a_blkfactor)
		ai->a_blkfactor = 1;

	ai->a_volsize = a->d_numblks * 2;

	ai->a_dev_type = (a->d_tape ? A_DEV_NOSEEK : A_DEV_SEEK);
}

static int xlate_arch_device (char *string, const char *cur_working_dir,
	int out)
{
	ARCH_DEV *a;
	struct stat stbuf;

	/* initialize volume info */

	ai->a_blksize	= 512;
	ai->a_volno		= 1;

	/*
	 *	If string is null or starts with a digit,
	 *	look for "archive0=xxxxxx" in a tar description file.
	 */

	if (*string == 0 || isdigit(*string))
	{
		a = arch_find_entry_by_name(string);
		if (a == 0)
		{
			errmsg(ER_TDNT, "", ERR_ANY);
			return (-1);
		}

		set_vals_from_arch_entry(a);
		fn_get_abs_path(cur_working_dir, a->d_device, ai->a_device);
	}
	else
	{
		fn_get_abs_path(cur_working_dir, string, ai->a_device);
		ai->a_blkfactor = 20;
		ai->a_volsize   = 0;
		if (os_stat(ai->a_device, &stbuf))
		{
			ai->a_dev_type = A_DEV_FILE;
			ai->a_volsize = 0;
			if (!out)			/* input? */
			{
				errmsg(ER_FDNE, "", ERR_ANY);
				return (-1);
			}
		}
		else
		{
			if (out)			/* output? */
			{
				if (S_ISREG(stbuf.st_mode))
				{
					ai->a_dev_type = A_DEV_FILE;
					ai->a_volsize = 0;
				}
				else if (S_ISCHR(stbuf.st_mode))
				{
					a = arch_find_entry_by_device(ai->a_device);
					if (a == 0)
						ai->a_dev_type = A_DEV_NOSEEK;
					else
						set_vals_from_arch_entry(a);
				}
				else
				{
					errmsg(ER_CATSF, "", ERR_ANY);
					return (-1);
				}
				if (!can_we_write(&stbuf))
				{
					errmsg(ER_NPTWF, "", ERR_ANY);
					return (-1);
				}
			}
			else
			{
				if (!can_we_read(&stbuf))
				{
					errmsg(ER_NPTRF, "", ERR_ANY);
					return (-1);
				}

				if (S_ISREG(stbuf.st_mode))
				{
					ai->a_dev_type = A_DEV_FILE;
					ai->a_volsize = 0;
				}
				else if (S_ISCHR(stbuf.st_mode))
				{
					a = arch_find_entry_by_device(ai->a_device);
					if (a == 0)
						ai->a_dev_type = A_DEV_NOSEEK;
					else
						set_vals_from_arch_entry(a);
				}
				else
				{
					errmsg(ER_CAFSF, "", ERR_ANY);
					return (-1);
				}
			}
		}
	}
	return (0);
}

static void make_phony_xstat (struct stat *xbuf)
{
	xbuf->st_dev   = 0;
	xbuf->st_ino   = 0;
	xbuf->st_mode  = (mode_t)-1;
	xbuf->st_nlink = 1;
	xbuf->st_uid   = -1;
	xbuf->st_gid   = -1;
	set_rdev(xbuf, 0);
	xbuf->st_size  = 0;
	xbuf->st_atime = (time_t) -1;
	xbuf->st_mtime = (time_t) -1;
	xbuf->st_ctime = (time_t) -1;

	check_owner(xbuf->st_uid);
	check_group(xbuf->st_gid);
}

static int disp_arc_dev (int n, int x)
{
	BLIST *b;
	ARCH_DEV *a;

	b = bnth(gbl(arch_dev_list), n);
	a = (ARCH_DEV *)bid(b);
	wmove(gbl(win_commands), 1, x);
	wclrtoeol(gbl(win_commands));
	waddstr(gbl(win_commands), a->d_device);
	wrefresh(gbl(win_commands));

	return (0);
}

static int extract_temp (FBLK *f)
{
	ABLK *	a = (ABLK *)(gbl(scr_cur)->cur_root)->node_sub_blk;
	int		rc;

	strcpy(ai->a_file_name, gbl(scr_cur)->path_name);
	fn_append_filename_to_dir(ai->a_file_name, FULLNAME(f));
	strcpy(ai->a_temp_name, gbl(pgm_tmp));
	fn_append_filename_to_dir(ai->a_temp_name, FULLNAME(f));

	switch (a->arch_type)
	{
	case A_TAR:
			rc = tar_extract_temp(f, ai->a_temp_name);
			break;

	case A_BAR:
			rc = bar_extract_temp(f, ai->a_temp_name);
			break;

	case A_CPIO:
			rc = cpio_extract_temp(f, ai->a_temp_name);
			break;

	case A_ZTAR:
			rc = ztar_extract_temp(f, ai->a_temp_name);
			break;

	default:
			rc = -1;
			break;
	}

	return (rc);
}

int arch_open (void)
{
	char arch_temp_dir[MAX_PATHLEN];
	int rc;

	switch (ai->a_dev_type)
	{
	case A_TAR:
			strcpy(ai->a_name, msgs(m_arcutils_tf));
			rc = tar_open();
			break;

	case A_BAR:
			strcpy(ai->a_name, msgs(m_arcutils_bf));
			rc = bar_open();
			if (rc == 0 &&
				ai->a_open_flags != O_RDONLY &&
				ai->a_open_flags != O_RDWR)
			{
				rc = bar_wr_vol_hdr((long)0);
			}
			break;

	case A_CPIO:
			strcpy(ai->a_name, msgs(m_arcutils_cf));
			rc = cpio_open();
			break;

	case A_ZTAR:
			strcpy(ai->a_name, msgs(m_arcutils_gf));
			fn_dirname(ai->a_device, arch_temp_dir);
			if (strcmp(arch_temp_dir, "/dev") == 0)
			{
				strcpy(arch_temp_dir, gbl(pgm_tmp));
				ai->a_comp_dev = TRUE;
			}
			else
			{
				ai->a_comp_dev = FALSE;
			}
			os_make_temp_name(ai->a_temp_comp, arch_temp_dir, NULL);
			strcpy(ai->a_save_comp, ai->a_device);
			if (ai->a_open_flags == O_RDONLY)
			{
				rc = x_decomp(ai->a_device, ai->a_temp_comp, comp_gzip);
				if (rc)
					return (-1);
			}
			strcpy(ai->a_device, ai->a_temp_comp);
			rc = tar_open();
			break;

	default:
			rc = -1;
			break;
	}
	return (rc);
}

int arch_close (void)
{
	int rc;

	switch (ai->a_dev_type)
	{
	case A_TAR:
			rc = tar_close(TRUE);
			break;

	case A_BAR:
			rc = bar_close(TRUE);
			break;

	case A_CPIO:
			rc = cpio_close(TRUE);
			break;

	case A_ZTAR:
			rc = tar_close(TRUE);
			if (rc == 0)
			{
				if (ai->a_open_flags != O_RDONLY)
				{
					if (ai->a_comp_dev)
					{
						rc = filecopy(ai->a_temp_comp, ai->a_save_comp, 0666);
					}
					else
					{
						rc = os_file_rename(ai->a_temp_comp, ai->a_save_comp);
					}
				}
				os_file_delete(ai->a_temp_comp);
			}
			break;

	default:
			rc = -1;
			break;
	}
	return (rc);
}

void do_arch_open (void)
{
	FBLK *f = gbl(scr_cur)->cur_file;

	if (! S_ISREG(f->stbuf.st_mode))
	{
		errmsg(ER_COSPF, "", ERR_ANY);
		return;
	}
	if (extract_temp(f))
		return;
	bang("");

	do_open_file(ai->a_temp_name);
	os_file_delete(ai->a_temp_name);
}

void do_extract (void)
{
	ABLK *a = (ABLK *)(gbl(scr_cur)->cur_root)->node_sub_blk;

	switch (a->arch_type)
	{
	case A_TAR:
			do_tar_extract();
			break;

	case A_BAR:
			do_bar_extract();
			break;

	case A_CPIO:
			do_cpio_extract();
			break;

	case A_ZTAR:
			do_ztar_extract();
			break;
	}
}

void do_tag_extract (void)
{
	ABLK *a = (ABLK *)(gbl(scr_cur)->cur_root)->node_sub_blk;

	if (check_tag_count())
		return;

	if (gbl(scr_cur)->command_mode == m_global ||
	    gbl(scr_cur)->command_mode == m_tag_global)
	{
		errmsg(ER_TENA, "", ERR_ANY);
		return;
	}

	switch (a->arch_type)
	{
	case A_TAR:
			do_tag_tar_extract();
			break;

	case A_BAR:
			do_tag_bar_extract();
			break;

	case A_CPIO:
			do_tag_cpio_extract();
			break;

	case A_ZTAR:
			do_tag_ztar_extract();
			break;
	}
}

void arc_print (void)
{
	FBLK *f = gbl(scr_cur)->cur_file;

	if (! S_ISREG(f->stbuf.st_mode))
	{
		errmsg(ER_CPSF, "", ERR_ANY);
		return;
	}
	if (extract_temp(f))
		return;
	bang("");

	print_file_with_hdr(FULLNAME(f), ai->a_temp_name, ai->a_file_name);
	os_file_delete(ai->a_temp_name);
}

void arc_view (void)
{
	FBLK *f = gbl(scr_cur)->cur_file;

	if (! S_ISREG(f->stbuf.st_mode))
	{
		errmsg(ER_CVSF, "", ERR_ANY);
		return;
	}
	if (extract_temp(f))
		return;

	strcpy(gbl(vfcb_fv)->v_dispname, ai->a_file_name);
	view_file(ai->a_temp_name);
	*gbl(vfcb_fv)->v_dispname = 0;
	os_file_delete(ai->a_temp_name);
}

/*
 *	This routine returns with the following variables:
 *		ai->a_blksize	block size
 *		ai->a_device[]	contains pathname
 *		ai->a_blkfactor	records in block (>= 1)
 *		ai->a_volsize	size of volume (in 512 byte blks) or
 *						0 if not known.
 *		ai->a_dev_type	tar device type
 */

int get_arch_device (const char *cur_working_dir, int out)
{
	char input_str[MAX_PATHLEN];
	int	arch_dev_x;
	int c;
	BLIST *b;
	ARCH_DEV *a;

get_dev:
	bang(msgs(m_arcutils_entdev));
	waddstr(gbl(win_message), "  ");
	fk_msg(gbl(win_message), CMDS_REG_FILE_ARCLIST, msgs(m_arcutils_f2list));
	wrefresh(gbl(win_message));

	wmove(gbl(win_commands), 1, 0);
	wclrtoeol(gbl(win_commands));
	if (out)
		xaddstr(gbl(win_commands), msgs(m_arcutils_todev));
	else
		xaddstr(gbl(win_commands), msgs(m_arcutils_frodev));
	wrefresh(gbl(win_commands));
	arch_dev_x = getcurx(gbl(win_commands));
	*input_str = 0;
	if (gbl(scr_cur)->command_mode == m_file)
	{
		if (gbl(scr_cur)->cur_file)
			strcpy(input_str, FULLNAME(gbl(scr_cur)->cur_file));
	}
	c = xgetstr(gbl(win_commands), input_str, XGL_DEVICE, MAX_PATHLEN, 0,
		XG_FILE_AB);
	if (c < 0)
		return (-1);

	if (c == cmds(CMDS_DEST_DIR))
	{
		WINDOW *w;

		if (gbl(arch_dev_list) == 0)
			arch_load_dev_tbl();

		if (gbl(arch_dev_list) == 0)
			goto get_dev;

		werase(gbl(win_message));
		esc_msg();
		bangnc(msgs(m_arcutils_seldev));
		if (gbl(scr_cur)->command_mode == m_dir ||
			gbl(scr_cur)->in_small_window)
		{
			w = gbl(scr_cur)->cur_dir_win;
		}
		else
		{
			w = gbl(scr_cur)->cur_file_win;
		}

		c = menupick(gbl(arch_dev_list), disp_arc_dev, w, arch_dev_x);
		bang("");
		if (c < 0)
			goto get_dev;
		b = bnth(gbl(arch_dev_list), c);
		a = (ARCH_DEV *)bid(b);
		strcpy(input_str, a->d_name);
	}

	c = xlate_arch_device(input_str, cur_working_dir, out);

	return (c);
}

int get_arch_type (void)
{
	int c;

	wmove(gbl(win_commands), 1, 0);
	wclrtoeol(gbl(win_commands));
	xaddstr(gbl(win_commands), msgs(m_arcutils_at));

	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_ARCH_BAR, msgs(m_arcutils_bar));

	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_ARCH_CPIO, msgs(m_arcutils_cpio));

	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_ARCH_TAR, msgs(m_arcutils_tar));

	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_ARCH_ZTAR, msgs(m_arcutils_ztar));

#if 0
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_ARCH_ZIP, msgs(m_arcutils_zip));
#endif

	wrefresh(gbl(win_commands));

	bang("");
	esc_msg();
	bangnc(msgs(m_arcutils_eat));

	while (TRUE)
	{
		c = xgetch(gbl(win_message));
		if (c == KEY_ESCAPE ||
			c == cmds(CMDS_ARCH_BAR) ||
			c == cmds(CMDS_ARCH_CPIO) ||
			c == cmds(CMDS_ARCH_TAR) ||
			c == cmds(CMDS_ARCH_ZTAR) ||
			c == cmds(CMDS_ARCH_ZIP) )
		break;
	}
	bang("");
	wmove(gbl(win_commands), 1, 0);
	wclrtoeol(gbl(win_commands));
	wrefresh(gbl(win_commands));

	if (c == KEY_ESCAPE)
		return (-1);

	if (c == cmds(CMDS_ARCH_BAR))
		ai->a_dev_type = A_BAR;
	else if (c == cmds(CMDS_ARCH_CPIO))
		ai->a_dev_type = A_CPIO;
	else if (c == cmds(CMDS_ARCH_TAR))
		ai->a_dev_type = A_TAR;
	else if (c == cmds(CMDS_ARCH_ZTAR))
		ai->a_dev_type = A_ZTAR;
	else
		return (-1);

	return (0);
}

TREE *add_phony_dblk_to_node (const char *path, NBLK *n, int *newdir)
{
	TREE *t;
	TREE *tn;
	DBLK *d;
	char *p;
	int i;
	char dir_part[MAX_FILELEN];

	/*
	 * note - it is assumed that the root of this node
	 * always is root
	 */

	*newdir = FALSE;
	t = n->root;
	i = 0;
	while (TRUE)
	{
		/*	loop thru siblings looking for next dir portion */

		p = fn_get_nth_dir(path, i, dir_part);
		if (!p)
			return (t);

		for (tn=tleft(t); tn; tn=tright(tn))
		{
			d = (DBLK *)tid(tn);
			if (strcmp(p, FULLNAME(d)) == 0)
				break;
		}
		if (!tn)
			break;
		t = tn;
		i++;
	}

	/* t points to parent of sub-path p */

	*newdir = TRUE;
	while (TRUE)
	{
		p = fn_get_nth_dir(path, i++, dir_part);
		if (!p)
			break;

		d = make_phony_dblk(p);
		if (d == 0)
			return (0);

		tn = TNEW(d);
		if (tn == 0)
		{
			dblk_free(d);
			return (0);
		}
		d->dir_tree = tn;

		add_dir_to_parent(tn, t);
		t = tn;
	}
	return (t);
}

DBLK *make_phony_dblk (const char *name)
{
	char rootname[MAX_PATHLEN];
	DBLK *d;
	int i;

	d = dblk_make();
	if (d == 0)
		return (0);

	if (*name == 0)
	{
		fn_rootname(NULL, rootname);
		name = rootname;
	}
	dblk_set_name(d, name);
	d->flags = D_PHONY;
	d->hidden = 0;
	make_phony_xstat(&d->stbuf);
	d->dir_size = 0;
	d->flist = 0;
	for (i=0; i<gbl(scr_cur_count); i++)
		d->mlist[i] = 0;
	return (d);
}

FBLK *make_phony_fblk (const char *name)
{
	FBLK *f;

	f = fblk_make();
	if (f == 0)
		return (0);
	fblk_set_name(f, name);
	f->archive_vol = ai->a_volno;
	f->archive_loc = ai->a_blk_count-1;
	f->tagged = 0;
	make_phony_xstat(&f->stbuf);
	f->dir = 0;
	f->sym_name = 0;
	f->sym_mode = 0;
	return (f);
}

void set_top_file (void)
{
	gbl(scr_cur)->cur_file_line = 0;
	gbl(scr_cur)->cur_file_col  = 0;
	gbl(scr_cur)->base_file = (gbl(scr_cur)->cur_dir)->flist;
	gbl(scr_cur)->first_file = (gbl(scr_cur)->cur_dir)->mlist[gbl(scr_cur_no)];
	gbl(scr_cur)->top_file = gbl(scr_cur)->first_file;
	gbl(scr_cur)->file_cursor = gbl(scr_cur)->top_file;
	gbl(scr_cur)->numfiles = bcount(gbl(scr_cur)->first_file);
	if (gbl(scr_cur)->first_file)
		gbl(scr_cur)->cur_file = (FBLK *)bid(gbl(scr_cur)->first_file);
}

int set_arch_buf (void)
{
	NBLK *n;
	ABLK *a;

	n = gbl(scr_cur)->cur_root;
	a = (ABLK *)n->node_sub_blk;
	*ai->a_name = 0;
	if (*a->arch_tmpname != 0)
		strcpy(ai->a_device, a->arch_tmpname);
	else
		strcpy(ai->a_device, a->arch_devname);
	ai->a_volsize   = a->arch_volsize;
	ai->a_blkfactor = a->arch_blkfactor;
	ai->a_dev_type  = a->arch_dev_type;
	ai->a_buffer = (char *)MALLOC(ai->a_blkfactor*ai->a_blksize);
	if (!ai->a_buffer)
	{
		errmsg(ER_IM, "", ERR_ANY);
		return (-1);
	}
	return (0);
}

int is_arch_path_rel (FBLK *f)
{
	char relpath[MAX_PATHLEN];
	TREE *p;
	TREE *t;
	NBLK *n;

	n = gbl(scr_cur)->cur_root;
	fn_rootname(NULL, relpath);
	fn_append_dirname_to_dir(relpath, ARCH_REL_DIR_NAME);
	t = path_to_tree(relpath, n);
	for (p=(f->dir)->dir_tree; p; p=tparent(p))
	{
		if (p == t)
			return (TRUE);
	}
	return (FALSE);
}

int get_arch_name (void)
{
	char *d;
	BLIST *b;
	NBLK *n;
	ABLK *a;
	time_t now;
	int i;
	int c;

	*ai->a_name = 0;
	while (!*ai->a_name)
	{
		bang(msgs(m_arcutils_entnm));
		wmove(gbl(win_commands), 1, 0);
		wclrtoeol(gbl(win_commands));
		xaddstr(gbl(win_commands), msgs(m_arcutils_arnm));
		wrefresh(gbl(win_commands));
		c = xgetstr(gbl(win_commands), ai->a_name, XGL_ARCH_NAME,
			ARCH_NAME_LEN, 0, XG_STRING);
		if (c < 0)
			return (-1);
		if (c == 0)
		{
			char dl[24];

			now = time((time_t *)NULL);
			d = date_2_str(dl, now);
			if (d[9] == ' ')
				d[9] = '0';
			for (i=0; i<8; i++)
				ai->a_name[i] = d[i+9];
			ai->a_name[i] = 0;
		}
		else
		{
			for (b=gbl(nodes_list); b; bnext(b))
			{
				n = (NBLK *)bid(b);
				if (n->node_type == N_ARCH)
				{
					a = (ABLK *)n->node_sub_blk;
					if (strcmp(a->arch_name, ai->a_name) == 0)
					{
						c = errmsg(ER_DUPNM, "", ERR_ANY);
						if (c < 0)
							return (-1);
						*ai->a_name = 0;
						break;
					}
				}
			}
		}
	}
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_archive_arch));
	waddstr(gbl(win_commands), ": ");
	waddstr(gbl(win_commands), ai->a_name);
	xaddstr(gbl(win_commands), msgs(m_arcutils_frodev));
	waddstr(gbl(win_commands), ai->a_device);
	wrefresh(gbl(win_commands));
	return (0);
}

int determine_arch_type (void)
{
	int fd;
	int l;
	char header[512];
	struct cpio_bin_hdr *cpio_bh;
	struct cpio_chr_hdr *cpio_ch;
	struct cpio_asc_hdr *cpio_ah;
	union tblock *tar_hdr;
	union bblock *bar_hdr;

	bang(msgs(m_arcutils_dat));

	fd = os_open(ai->a_device, O_RDONLY, 0666);
	if (fd == -1)
	{
		bang("");
		return (-1);
	}

	l = os_read(fd, header, sizeof(header));
	os_close(fd);

	if (l <= 0)
	{
		bang("");
		return (-1);
	}

	/* check if cpio header */

	cpio_bh = (struct cpio_bin_hdr *)header;
	if (cpio_bh->h_magic == CMN_BIN)
	{
		ai->a_type = A_CPIO;
		bang("");
		return (0);
	}

	if (cpio_bh->h_magic == CMN_BBS)
	{
		ai->a_type = A_CPIO;
		bang("");
		return (0);
	}

	cpio_ch = (struct cpio_chr_hdr *)header;
	if (strncmp(cpio_ch->c_magic, CMS_CHR, sizeof(cpio_ch->c_magic)) == 0)
	{
		ai->a_type = A_CPIO;
		bang("");
		return (0);
	}

	cpio_ah = (struct cpio_asc_hdr *)header;
	if (strncmp(cpio_ah->e_magic, CMS_ASC, sizeof(cpio_ah->e_magic)) == 0)
	{
		ai->a_type = A_CPIO;
		bang("");
		return (0);
	}

	if (strncmp(cpio_ah->e_magic, CMS_CRC, sizeof(cpio_ah->e_magic)) == 0)
	{
		ai->a_type = A_CPIO;
		bang("");
		return (0);
	}

	/* check if tar header */

	tar_hdr = (union tblock *)header;
	if (tar_is_hdr(tar_hdr))
	{
		ai->a_type = A_TAR;
		bang("");
		return (0);
	}

	/* check if bar vol header */

	bar_hdr = (union bblock *)header;
	if (bar_is_hdr(bar_hdr))
	{
		ai->a_type = A_BAR;
		bang("");
		return (0);
	}

	/* check if gztar vol header */

	if (*header == 0x1f)
	{
		ai->a_type = A_ZTAR;
		bang("");
		return (0);
	}

	/* cannot determine type */

	bang("");
	return (-1);
}

static ARCH_DEV *arch_parse_dev_entry (char *line)
{
	ARCH_DEV *	a;
	char		name[MAX_FILELEN];
	char		desc[MAX_FILELEN];
	char		dev[MAX_PATHLEN];
	int			blkfactor	= 1;
	int			numblks		= 0;
	int			tape		= FALSE;
	char *		s;
	char *		t;

	strip(line);
	if (*line == 0 || *line == '#')
		return (0);

	s = strchr(line, '=');
	if (s == 0)
		return (0);
	*s++ = 0;

	if (strccmpn("archive", line) != 0)
		return (0);

	strcpy(name, line + 7);

	t = dev;
	for (; *s; s++)
	{
		if (isspace(*s))
			break;
		*t++ = *s;
	}
	*t = 0;

	strcpy(desc, dev);

	if (*s == 0)
		goto gotit;

	blkfactor = stoi((const char **)&s);
	if (*s == 0)
		goto gotit;

	numblks = stoi((const char **)&s);
	if (*s == 0)
		goto gotit;

	for (s++; *s; s++)
	{
		if (! isspace(*s))
			break;
	}
	tape = (TO_LOWER(*s) == 'y');
	if (*s == 0)
		goto gotit;

	for (s++; *s; s++)
	{
		if (! isspace(*s) && *s != '#')
			break;
	}
	if (*s != 0)
		strcpy(desc, s);

gotit:
	a = (ARCH_DEV *)MALLOC(sizeof(*a));
	if (a != 0)
	{
		strcpy(a->d_name,   name);
		strcpy(a->d_desc,   desc);
		strcpy(a->d_device, dev);
		a->d_blkfactor = blkfactor;
		a->d_numblks   = numblks;
		a->d_tape      = tape;
	}

	return (a);
}

void arch_load_dev_tbl (void)
{
	FILE *		fp;
	char		line[BUFSIZ];
	char		filename[MAX_FILELEN];
	char		pathname[MAX_PATHLEN];
	char *		defpath;

	arch_free_dev_tbl();

	strcpy(filename, pgmi(m_pgm_program));
	fn_set_ext(filename, pgm_const(dev_ext));
	defpath = os_get_path(filename, gbl(pgm_path), pathname);
	if (defpath == 0)
		return;

	fp = fopen(defpath, "r");
	if (fp == 0)
		return;

	while (fgets(line, sizeof(line), fp))
	{
		ARCH_DEV *	a;

		a = arch_parse_dev_entry(line);
		if (a != 0)
		{
			BLIST *		l;

			l = BNEW(a);
			if (a == 0)
				FREE(a);
			else
				gbl(arch_dev_list) = bappend(gbl(arch_dev_list), l);
		}
	}

	fclose(fp);
}

void arch_free_dev_tbl (void)
{
	gbl(arch_dev_list) = BSCRAP(gbl(arch_dev_list), TRUE);
}
