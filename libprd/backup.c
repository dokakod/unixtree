/*------------------------------------------------------------------------
 * process the "backup" cmd
 */
#include "libprd.h"

static int ask_questions (void)
{
	char input_str[MAX_PATHLEN];
	char base_path[MAX_PATHLEN];
	struct stat stbuf;
	int c;

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	if (gbl(scr_cur)->cmd_sub_mode == m_reg)
	{
		xaddstr(gbl(win_commands), msgs(m_backup_bakfil));
		zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_file));
	}
	else
		xaddstr(gbl(win_commands), msgs(m_backup_bakall));
	wrefresh(gbl(win_commands));

	/* ask for archive device */

	ai->a_open_flags = O_WRONLY;
	dptr_to_dirname(gbl(scr_cur)->dir_cursor, base_path);
	c = get_arch_device(base_path, TRUE);
	if (c)
		return (c);

	/* ask for archive type */

	if (get_arch_type())
		return (-1);

	/* display "BACKUP ... to ..." */

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	if (gbl(scr_cur)->cmd_sub_mode == m_reg)
	{
		xaddstr(gbl(win_commands), msgs(m_backup_bakfil));
		zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_file));
	}
	else
		xaddstr(gbl(win_commands), msgs(m_backup_bakall));
	waddstr(gbl(win_commands), msgs(m_backup_to2));
	waddstr(gbl(win_commands), ai->a_device);
	wrefresh(gbl(win_commands));

	/* ask whether to append to end of file */

	switch (ai->a_dev_type)
	{
	case A_DEV_FILE:
		if (os_stat(ai->a_device, &stbuf) == 0 && ai->a_dev_type != A_ZTAR)
			c = 1;
		else
			c = 0;
		break;
	case A_DEV_NOSEEK:
		c = 0;
		break;
	case A_DEV_SEEK:
		c = 0;				/* set to 1 if append to devices OK */
		break;
	}
	if (c)
	{
		c = yesno_msg(msgs(m_backup_append));
		if (c < 0)
			return (-1);
		if (c == 0)
			ai->a_open_flags = O_RDWR;
	}

	/*
	 * Ask if rel pathnames or abs pathnames desired.
	 * If global mode, abs path names required.
	 */

	if (gbl(scr_cur)->command_mode == m_global ||
	    gbl(scr_cur)->command_mode == m_tag_global)
	{
		*ai->a_rel_name = 0;
	}
	else
	{
		wmove(gbl(win_commands), 1, 0);
		xaddstr(gbl(win_commands), msgs(m_backup_sto));
		waddstr(gbl(win_commands), "  ");
		xcaddstr(gbl(win_commands), CMDS_ARCH_ABS, msgs(m_backup_stoa));
		waddstr(gbl(win_commands), msgs(m_backup_or));
		xcaddstr(gbl(win_commands), CMDS_ARCH_REL, msgs(m_backup_stor));
		wrefresh(gbl(win_commands));
		werase(gbl(win_message));
		esc_msg();
		bangnc(msgs(m_backup_entar));
		while (TRUE)
		{
			c = xgetch(gbl(win_message));
			c = tolower(c);
			if (c == KEY_ESCAPE ||
				c == KEY_RETURN ||
			    c == cmds(CMDS_ARCH_REL) ||
				c == cmds(CMDS_ARCH_ABS))
				break;
		}
		bang("");
		if (c == KEY_ESCAPE || c == KEY_RETURN)
			return (-1);
		if (c == cmds(CMDS_ARCH_ABS))
		{
			*ai->a_rel_name = 0;
		}
		else
		{
			bang(msgs(m_backup_entbas));
			waddstr(gbl(win_message), "  ");
			fk_msg(gbl(win_message), CMDS_DEST_DIR, msgs(m_backup_f2pt));
			wrefresh(gbl(win_message));

			wmove(gbl(win_commands), 1, 0);
			wclrtoeol(gbl(win_commands));
			xaddstr(gbl(win_commands), msgs(m_backup_entnod));
			wrefresh(gbl(win_commands));
			strcpy(input_str, gbl(scr_cur)->path_name);
			c = xgetstr(gbl(win_commands), input_str, XGL_RELNODE, MAX_PATHLEN,
				0, XG_PATHNAME);
			if (c < 0)
				return (-1);
			fn_resolve_pathname(input_str);
			fn_get_abs_dir(gbl(scr_cur)->path_name, input_str, ai->a_rel_name);
			if (is_directory(ai->a_rel_name, 0))
				return (-1);
			fn_terminate_dirname(ai->a_rel_name);
		}
	}

	/* if bar file, ask if compression wanted */

	if (ai->a_dev_type == A_BAR && ai->a_open_flags == O_WRONLY)
	{
		c = yesno_msg(msgs(m_backup_askcmp));
		if (c < 0)
			return (-1);
		ai->a_compressed = (c == 0);
		if (ai->a_compressed)
			os_make_temp_name(ai->a_temp_name, gbl(pgm_tmp), NULL);
	}

	ai->a_buffer = (char *)MALLOC(ai->a_blkfactor*ai->a_blksize);
	if (!ai->a_buffer)
	{
		errmsg(ER_IM, "", ERR_ANY);
		return (-1);
	}
	ai->a_volno = 1;
	return (0);
}

static int tar_file (FBLK *f)
{
	long blks_needed;
	char path[MAX_PATHLEN];
	BLIST *l;
	FBLK *fl;
	int is_link;
	int c;
	int n;
	int fd = -1;
	int blks_to_read;
	int blks_left;
	int vols_needed;
	int vols_written;
	int num_to_read;
	int num_read_in;
	int blks_to_write;

	/* display message */

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_backup_backing));
	zaddstr(gbl(win_commands), FULLNAME(f));
	wrefresh(gbl(win_commands));

	/* if link, find file it's linked to */

	is_link = FALSE;
	fl = 0;
	if (gbl(scr_cur)->cmd_sub_mode == m_tag && (f->stbuf.st_nlink > 1))
	{
		for (l=ai->a_links; l; l=bnext(l))
		{
			fl = (FBLK *)bid(l);
			if (fl->stbuf.st_dev == f->stbuf.st_dev &&
				fl->stbuf.st_ino == f->stbuf.st_ino)
			{
				is_link = TRUE;
				break;
			}
		}
		l = BNEW(f);
		if (l)
		{
			ai->a_links = bappend(ai->a_links, l);
		}
	}

	/* determine number of blocks left on device */

	if (ai->a_volsize)
		blks_left = ai->a_volsize - ai->a_blk_count;
	else
		blks_left = 0;

	/* determine number of blocks needed for file */

	if (is_link || ! S_ISREG(f->stbuf.st_mode))
		blks_needed = 0;
	else
		blks_needed = (f->stbuf.st_size + ai->a_blksize-1) / ai->a_blksize;

	/*
	 *	ask for new volume if there is not enough space for file
	 *	or at least for the min # of blks.
	 */

	if (ai->a_blk_count && blks_left && (blks_needed+3 > blks_left))
	{
		if (is_link || blks_left < ARCH_MIN_BLKS)
		{
			if (tar_ask_for_volno(ai->a_name, ai->a_volno+1))
				return (-1);
		}
	}

	/* if link, just write header and quit */

	if (is_link)
	{
		c = tar_wr_link_hdr(f, fl, ai->a_rel_name);
		if (c == -1)
			return (-1);
		else
			return (1);
	}

	/* if symlink, do same */

	strcpy(path, gbl(scr_cur)->path_name);
	fn_append_filename_to_dir(path, FULLNAME(f));
	if (S_ISLNK(f->stbuf.st_mode))
	{
		char symname[MAX_PATHLEN];

		c = os_readlink(path, symname, sizeof(symname));
		if (c == -1)
		{
			c = errsys(ER_IOERR);
			return (c);
		}
		symname[c] = 0;
		c = tar_wr_sym_hdr(f, symname, ai->a_rel_name);
		if (c == -1)
			return (-1);
		else
			return (1);
	}

	/* check if regular file */

	if (! S_ISREG(f->stbuf.st_mode))
	{
		return (1);
	}

	/* determine number of blocks left on device (again) */

	if (ai->a_volsize)
		blks_left = ai->a_volsize - ai->a_blk_count;
	else
		blks_left = 0;

	/* open file to be archived */

	if (S_ISREG(f->stbuf.st_mode))
	{
		fd = os_open(path, O_RDONLY, 0666);
		if (fd < 0)
		{
			c = errsys(ER_COF);
			return (c);
		}
	}

	if (S_ISREG(f->stbuf.st_mode))
	{
		/* check if file will fit on current volume */

		if (blks_left && (blks_needed+3 > blks_left))
		{
			/* file will not fit on current volume */

			/* calculate number of vols needed */

			n = blks_needed - (blks_left-3);	/* blks needed after 1st vol */
			vols_needed = 1;
			vols_needed += n / (ai->a_volsize-3);
			if (n % (ai->a_volsize-3))
				vols_needed++;

			for (vols_written=1; vols_written<=vols_needed; vols_written++)
			{
				/* write out partial header */

				blks_left = ai->a_volsize - ai->a_blk_count;
				blks_to_write = blks_left-3;
				if (blks_to_write > blks_needed)
					blks_to_write = blks_needed;
				n = blks_to_write * ai->a_blksize;
				c = tar_wr_part_hdr(f, ai->a_rel_name, n, vols_written,
					vols_needed);
				if (c)
					return (c);
				blks_needed -= blks_to_write;

				/* write part of file */

				while (blks_to_write)
				{
					blks_to_read = ai->a_blkfactor - ai->a_rec_count;
					if (blks_to_read > blks_to_write)
						blks_to_read = blks_to_write;
					num_to_read = blks_to_read * ai->a_blksize;
					num_read_in = os_read(fd, ai->a_recp, num_to_read);
					if (num_read_in == -1)
					{
						os_close(fd);
						c = errsys(ER_IOERR);
						return (c);
					}
					else if (num_read_in == 0)
					{
						os_close(fd);
						c = errmsg(ER_UEOF, "", ERR_ANY);
						return (c);
					}
					else
					{
						n = (num_read_in + ai->a_blksize - 1) / ai->a_blksize;
						blks_to_write -= n;
						while (n--)
						{
							if (num_read_in < ai->a_blksize)
							{
								for (c=num_read_in; c<ai->a_blksize; c++)
									ai->a_recp[c] = 0;
							}
							c = tar_write_rec();
							if (c)
							{
								os_close(fd);
								return (c);
							}
							num_read_in -= ai->a_blksize;
						}
					}
				}
				if (vols_written < vols_needed)
				{
					if (tar_ask_for_volno(ai->a_name, ai->a_volno+1))
						return (-1);
				}
			}
		}
		else
		{
			/* file will fit on current volume */

			/* write out header */

			c = tar_wr_hdr(f, ai->a_rel_name);
			if (c)
				return (c);

			/* write file */

			while (blks_needed)
			{
				blks_to_read = ai->a_blkfactor - ai->a_rec_count;
				if (blks_to_read > blks_needed)
					blks_to_read = blks_needed;
				num_to_read = blks_to_read * ai->a_blksize;
				num_read_in = os_read(fd, ai->a_recp, num_to_read);
				if (num_read_in == -1)
				{
					os_close(fd);
					c = errsys(ER_IOERR);
					return (c);
				}
				else if (num_read_in == 0)
				{
					os_close(fd);
					c = errmsg(ER_UEOF, "", ERR_ANY);
					return (c);
				}
				else
				{
					n = (num_read_in + ai->a_blksize - 1) / ai->a_blksize;
					blks_needed -= n;
					while (n--)
					{
						if (num_read_in < ai->a_blksize)
						{
							for (c=num_read_in; c<ai->a_blksize; c++)
								ai->a_recp[c] = 0;
						}
						c = tar_write_rec();
						if (c)
						{
							os_close(fd);
							return (c);
						}
						num_read_in -= ai->a_blksize;
					}
				}
			}
		}
		os_close(fd);
	}

	return (1);					/* for traverse() */
}

static int bar_file (FBLK *f)
{
	long blks_needed;
	struct stat cstbuf;
	char path[MAX_PATHLEN];
	BLIST *l;
	FBLK *fl;
	int is_link;
	int c;
	int n;
	int fd;
	int blks_to_read;
	int blks_to_write;
	int blks_left;
	int vols_needed;
	int vols_written;
	int num_to_read;
	int num_read_in;
	long save_size;
	long file_size;
	int file_was_compressed;

	/* display message */

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_backup_backing));
	zaddstr(gbl(win_commands), FULLNAME(f));
	wrefresh(gbl(win_commands));

	/* if link, find file it's linked to */

	is_link = FALSE;
	fl = 0;
	if (gbl(scr_cur)->cmd_sub_mode == m_tag && (f->stbuf.st_nlink > 1))
	{
		for (l=ai->a_links; l; l=bnext(l))
		{
			fl = (FBLK *)bid(l);
			if (fl->stbuf.st_dev == f->stbuf.st_dev &&
				fl->stbuf.st_ino == f->stbuf.st_ino)
			{
				is_link = TRUE;
				break;
			}
		}
		l = BNEW(f);
		if (l)
		{
			ai->a_links = bappend(ai->a_links, l);
		}
	}

	/* determine number of blocks left on device */

	if (ai->a_volsize)
		blks_left = ai->a_volsize - ai->a_blk_count;
	else
		blks_left = 0;

	/* determine number of blocks needed for file */

	if (is_link || ! S_ISREG(f->stbuf.st_mode))
		blks_needed = 0;
	else
		blks_needed = (f->stbuf.st_size + ai->a_blksize-1) / ai->a_blksize;

	/*
	 *	ask for new volume if at end of vol
	 */

	if (ai->a_volsize && blks_left == 0)
	{
		if (bar_ask_for_volno(ai->a_name, ai->a_volno+1))
			return (-1);
		bar_wr_vol_hdr((long)0);
	}

	/* if link, just write header and quit */

	if (is_link)
	{
		c = bar_wr_link_hdr(f, fl, ai->a_rel_name);
		if (c == -1)
			return (-1);
		else
			return (1);
	}

	/* same if symlink */

	strcpy(path, gbl(scr_cur)->path_name);
	fn_append_filename_to_dir(path, FULLNAME(f));
	if (S_ISLNK(f->stbuf.st_mode))
	{
		char zname[MAX_PATHLEN];

		c = os_readlink(path, zname, sizeof(zname));
		if (c == -1)
		{
			c = errsys(ER_IOERR);
			return (c);
		}
		zname[c] = 0;
		c = bar_wr_sym_hdr(f, zname, ai->a_rel_name);
		if (c == -1)
			return (-1);
		else
			return (1);
	}

	/* check if regular file */

	if (! S_ISREG(f->stbuf.st_mode))
	{
		/* if not, just write header & return */

		c = bar_wr_hdr(f, ai->a_rel_name);
		if (c)
			return (c);
	}
	else
	{

		/* open file to be archived */

		/* check if file is to be compressed */

		file_was_compressed = FALSE;
		if (ai->a_compressed && f->stbuf.st_size)
		{
			x_comp(path, ai->a_temp_name, comp_lzw);
			strcpy(path, ai->a_temp_name);
			if (os_stat(path, &cstbuf))
			{
				return (-1);
			}
			file_was_compressed = TRUE;
		}

		fd = os_open(path, O_RDONLY, 0666);
		if (fd < 0)
		{
			c = errsys(ER_COF);
			return (c);
		}

		/* delete the open file, will go away at close time */

		if (file_was_compressed)
			os_file_delete(ai->a_temp_name);

		/* write out header */

		file_size = f->stbuf.st_size;
		if (file_was_compressed)
		{
			save_size = f->stbuf.st_size;
			f->stbuf.st_size = cstbuf.st_size;
			file_size = cstbuf.st_size;
			c = bar_wr_hdr(f, ai->a_rel_name);
			f->stbuf.st_size = save_size;
		}
		else
		{
			c = bar_wr_hdr(f, ai->a_rel_name);
		}
		if (c)
			return (c);

		/* determine number of blocks needed for file (again) */

		blks_needed = (file_size + ai->a_blksize-1) / ai->a_blksize;

		/* determine blocks left on volume (again) */

		if (ai->a_volsize)
			blks_left = ai->a_volsize - ai->a_blk_count;
		else
			blks_left = 0;

		/* write file */

		/* check if file will fit on current volume */

		if (blks_left && (blks_needed+2 > blks_left))
		{
			/* file will not fit on current volume */

			/* calculate number of volumes needed */

			n = blks_needed - blks_left;	/* number left after 1st vol */
			vols_needed = 1 + (n / (ai->a_volsize-1));
			if (n % (ai->a_volsize-1))
				vols_needed++;

			for (vols_written=0; vols_written<vols_needed; vols_written++)
			{
				/* if not 1st vol, ask for new vol & write hdr */

				if (vols_written)
				{
					blks_to_write = ai->a_volsize - 1;
					if (blks_to_write > blks_needed)
						blks_to_write = blks_needed;
					blks_needed -= blks_to_write;

					c = bar_ask_for_volno(ai->a_name, ai->a_volno+1);
					if (c < 0)
					{
						os_close(fd);
						return (-1);
					}

					c = bar_wr_vol_hdr((long)blks_to_write*ai->a_blksize);
					if (c < 0)
					{
						os_close(fd);
						return (-1);
					}
				}
				else
				{
					blks_to_write = blks_left;
					if (blks_to_write > blks_needed)
						blks_to_write = blks_needed;
					blks_needed -= blks_to_write;
				}

				/* write part of file */

				while (blks_to_write)
				{
					blks_to_read = ai->a_blkfactor - ai->a_rec_count;
					if (blks_to_read > blks_to_write)
						blks_to_read = blks_to_write;
					num_to_read = blks_to_read * ai->a_blksize;
					num_read_in = os_read(fd, ai->a_recp, num_to_read);
					if (num_read_in == -1)
					{
						os_close(fd);
						c = errsys(ER_IOERR);
						return (c);
					}
					else if (num_read_in == 0)
					{
						os_close(fd);
						c = errmsg(ER_UEOF, "", ERR_ANY);
						return (c);
					}
					else
					{
						n = (num_read_in + ai->a_blksize - 1) / ai->a_blksize;
						blks_to_write -= n;
						while (n--)
						{
							if (num_read_in < ai->a_blksize)
							{
								for (c=num_read_in; c<ai->a_blksize; c++)
									ai->a_recp[c] = 0;
							}
							c = bar_write_rec();
							if (c)
							{
								os_close(fd);
								return (c);
							}
							num_read_in -= ai->a_blksize;
						}
					}
				}
			}
		}
		else
		{
			/* file will fit on current volume */

			while (blks_needed)
			{
				blks_to_read = ai->a_blkfactor - ai->a_rec_count;
				if (blks_to_read > blks_needed)
					blks_to_read = blks_needed;
				num_to_read = blks_to_read * ai->a_blksize;
				num_read_in = os_read(fd, ai->a_recp, num_to_read);

				if (num_read_in == -1)
				{
					os_close(fd);
					c = errsys(ER_IOERR);
					return (c);
				}

				if (num_read_in == 0)
				{
					os_close(fd);
					if (!file_was_compressed)
						c = errmsg(ER_UEOF, "", ERR_ANY);
					else
						c = 1;
					return (c);
				}

				n = (num_read_in + ai->a_blksize - 1) / ai->a_blksize;
				blks_needed -= n;
				while (n--)
				{
					if (num_read_in < ai->a_blksize)
					{
						for (c=num_read_in; c<ai->a_blksize; c++)
							ai->a_recp[c] = 0;
					}
					c = bar_write_rec();
					if (c)
					{
						os_close(fd);
						return (c);
					}
					num_read_in -= ai->a_blksize;
				}
			}
		}
		os_close(fd);
	}

	return (1);					/* for traverse */
}

static int cpio_file (FBLK *f)
{
	char path[MAX_PATHLEN];
	int c;
	int fd = -1;
	long num_to_write;
	long num_to_read;
	long num_read_in;

	/* display message */

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_backup_backing));
	zaddstr(gbl(win_commands), FULLNAME(f));
	wrefresh(gbl(win_commands));

	/* open file to be archived */

	strcpy(path, gbl(scr_cur)->path_name);
	fn_append_filename_to_dir(path, FULLNAME(f));
	if (S_ISREG(f->stbuf.st_mode))
	{
		fd = os_open(path, O_RDONLY, 0666);
		if (fd < 0)
		{
			c = errsys(ER_COF);
			return (c);
		}
	}

	/* write out header */

	c = cpio_wr_hdr(f, ai->a_rel_name);
	if (c)
		return (c);

	/* write file */

	if (S_ISLNK(f->stbuf.st_mode))
	{
		c = os_readlink(path, ai->cpio_buffer, ai->cpio_bufsize);
		if (c == -1)
		{
			c = errsys(ER_IOERR);
			return (c);
		}
		else
		{
			c = cpio_write_rec(ai->cpio_buffer, c);
			if (c)
			{
				return (c);
			}
		}
	}
	else
	if (S_ISREG(f->stbuf.st_mode))
	{
		num_to_write = f->stbuf.st_size;
		while (num_to_write)
		{
			num_to_read = ai->cpio_bufsize;
			if (num_to_read > num_to_write)
				num_to_read = num_to_write;
			num_read_in = os_read(fd, ai->cpio_buffer, num_to_read);

			if (num_read_in == -1)
			{
				os_close(fd);
				c = errsys(ER_IOERR);
				return (c);
			}

			if (num_read_in == 0)
			{
				os_close(fd);
				c = errmsg(ER_UEOF, "", ERR_ANY);
				return (c);
			}

			c = cpio_write_rec(ai->cpio_buffer, num_read_in);
			if (c)
			{
				os_close(fd);
				return (c);
			}
			num_to_write -= num_read_in;
		}
		os_close(fd);
	}

	return (1);					/* for traverse */
}

static int ztar_file (FBLK *f)
{
	long blks_needed;
	char path[MAX_PATHLEN];
	BLIST *l;
	FBLK *fl;
	int is_link;
	int c;
	int n;
	int fd = -1;
	int blks_to_read;
	int blks_left;
	int vols_needed;
	int vols_written;
	int num_to_read;
	int num_read_in;
	int blks_to_write;

	/* display message */

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_backup_backing));
	zaddstr(gbl(win_commands), FULLNAME(f));
	wrefresh(gbl(win_commands));

	/* if link, find file it's linked to */

	is_link = FALSE;
	fl = 0;
	if (gbl(scr_cur)->cmd_sub_mode == m_tag && (f->stbuf.st_nlink > 1))
	{
		for (l=ai->a_links; l; l=bnext(l))
		{
			fl = (FBLK *)bid(l);
			if (fl->stbuf.st_dev == f->stbuf.st_dev &&
				fl->stbuf.st_ino == f->stbuf.st_ino)
			{
				is_link = TRUE;
				break;
			}
		}
		l = BNEW(f);
		if (l)
		{
			ai->a_links = bappend(ai->a_links, l);
		}
	}

	/* determine number of blocks left on device */

	if (ai->a_volsize)
		blks_left = ai->a_volsize - ai->a_blk_count;
	else
		blks_left = 0;

	/* determine number of blocks needed for file */

	if (is_link || ! S_ISREG(f->stbuf.st_mode))
		blks_needed = 0;
	else
		blks_needed = (f->stbuf.st_size + ai->a_blksize-1) / ai->a_blksize;

	/*
	 *	ask for new volume if there is not enough space for file
	 *	or at least for the min # of blks.
	 */

	if (ai->a_blk_count && blks_left && (blks_needed+3 > blks_left))
	{
		if (is_link || blks_left < ARCH_MIN_BLKS)
		{
			if (tar_ask_for_volno(ai->a_name, ai->a_volno+1))
				return (-1);
		}
	}

	/* if link, just write header and quit */

	if (is_link)
	{
		c = tar_wr_link_hdr(f, fl, ai->a_rel_name);
		if (c == -1)
			return (-1);
		else
			return (1);
	}

	/* if symlink, do same */

	strcpy(path, gbl(scr_cur)->path_name);
	fn_append_filename_to_dir(path, FULLNAME(f));
	if (S_ISLNK(f->stbuf.st_mode))
	{
		char symname[MAX_PATHLEN];

		c = os_readlink(path, symname, sizeof(symname));
		if (c == -1)
		{
			c = errsys(ER_IOERR);
			return (c);
		}
		symname[c] = 0;
		c = tar_wr_sym_hdr(f, symname, ai->a_rel_name);
		if (c == -1)
			return (-1);
		else
			return (1);
	}

	/* check if regular file */

	if (! S_ISREG(f->stbuf.st_mode))
	{
		return (1);
	}

	/* determine number of blocks left on device (again) */

	if (ai->a_volsize)
		blks_left = ai->a_volsize - ai->a_blk_count;
	else
		blks_left = 0;

	/* open file to be archived */

	if (S_ISREG(f->stbuf.st_mode))
	{
		fd = os_open(path, O_RDONLY, 0666);
		if (fd < 0)
		{
			c = errsys(ER_COF);
			return (c);
		}
	}

	if (S_ISREG(f->stbuf.st_mode))
	{
		/* check if file will fit on current volume */

		if (blks_left && (blks_needed+3 > blks_left))
		{
			/* file will not fit on current volume */

			/* calculate number of vols needed */

			n = blks_needed - (blks_left-3);	/* blks needed after 1st vol */
			vols_needed = 1;
			vols_needed += n / (ai->a_volsize-3);
			if (n % (ai->a_volsize-3))
				vols_needed++;

			for (vols_written=1; vols_written<=vols_needed; vols_written++)
			{
				/* write out partial header */

				blks_left = ai->a_volsize - ai->a_blk_count;
				blks_to_write = blks_left-3;
				if (blks_to_write > blks_needed)
					blks_to_write = blks_needed;
				n = blks_to_write * ai->a_blksize;
				c = tar_wr_part_hdr(f, ai->a_rel_name, n, vols_written,
					vols_needed);
				if (c)
					return (c);
				blks_needed -= blks_to_write;

				/* write part of file */

				while (blks_to_write)
				{
					blks_to_read = ai->a_blkfactor - ai->a_rec_count;
					if (blks_to_read > blks_to_write)
						blks_to_read = blks_to_write;
					num_to_read = blks_to_read * ai->a_blksize;
					num_read_in = os_read(fd, ai->a_recp, num_to_read);
					if (num_read_in == -1)
					{
						os_close(fd);
						c = errsys(ER_IOERR);
						return (c);
					}
					else if (num_read_in == 0)
					{
						os_close(fd);
						c = errmsg(ER_UEOF, "", ERR_ANY);
						return (c);
					}
					else
					{
						n = (num_read_in + ai->a_blksize - 1) / ai->a_blksize;
						blks_to_write -= n;
						while (n--)
						{
							if (num_read_in < ai->a_blksize)
							{
								for (c=num_read_in; c<ai->a_blksize; c++)
									ai->a_recp[c] = 0;
							}
							c = tar_write_rec();
							if (c)
							{
								os_close(fd);
								return (c);
							}
							num_read_in -= ai->a_blksize;
						}
					}
				}
				if (vols_written < vols_needed)
				{
					if (tar_ask_for_volno(ai->a_name, ai->a_volno+1))
						return (-1);
				}
			}
		}
		else
		{
			/* file will fit on current volume */

			/* write out header */

			c = tar_wr_hdr(f, ai->a_rel_name);
			if (c)
				return (c);

			/* write file */

			while (blks_needed)
			{
				blks_to_read = ai->a_blkfactor - ai->a_rec_count;
				if (blks_to_read > blks_needed)
					blks_to_read = blks_needed;
				num_to_read = blks_to_read * ai->a_blksize;
				num_read_in = os_read(fd, ai->a_recp, num_to_read);
				if (num_read_in == -1)
				{
					os_close(fd);
					c = errsys(ER_IOERR);
					return (c);
				}
				else if (num_read_in == 0)
				{
					os_close(fd);
					c = errmsg(ER_UEOF, "", ERR_ANY);
					return (c);
				}
				else
				{
					n = (num_read_in + ai->a_blksize - 1) / ai->a_blksize;
					blks_needed -= n;
					while (n--)
					{
						if (num_read_in < ai->a_blksize)
						{
							for (c=num_read_in; c<ai->a_blksize; c++)
								ai->a_recp[c] = 0;
						}
						c = tar_write_rec();
						if (c)
						{
							os_close(fd);
							return (c);
						}
						num_read_in -= ai->a_blksize;
					}
				}
			}
		}
		os_close(fd);
	}

	return (1);					/* for traverse() */
}

static int arch_file (void *data)
{
	int rc;

	switch (ai->a_dev_type)
	{
	case A_TAR:
			rc = tar_file(gbl(scr_cur)->cur_file);
			break;
	case A_BAR:
			rc = bar_file(gbl(scr_cur)->cur_file);
			break;
	case A_CPIO:
			rc = cpio_file(gbl(scr_cur)->cur_file);
			break;
	case A_ZTAR:
			rc = ztar_file(gbl(scr_cur)->cur_file);
			break;
	default:
			rc = -1;
			break;
	}
	return (rc);
}

void do_backup (void)
{
	if (ask_questions())
		return;

	if (arch_open())
	{
		FREE(ai->a_buffer);
		return;
	}
	if (ai->a_dev_type == A_CPIO)
	{
		ai->cpio_buffer = (char *)MALLOC(ai->cpio_bufsize);
		if (!ai->cpio_buffer)
		{
			FREE(ai->a_buffer);
			arch_close();
			errmsg(ER_IM, "", ERR_ANY);
			return;
		}
	}

	arch_file(0);

	switch (ai->a_dev_type)
	{
	case A_BAR:
		bar_wr_tlr();
		break;

	case A_TAR:
		break;

	case A_CPIO:
		cpio_wr_tlr();
		FREE(ai->cpio_buffer);
		break;

	case A_ZTAR:
		break;
	}
	arch_close();

	FREE(ai->a_buffer);
	if (ai->a_dev_type == A_DEV_FILE)
		check_the_file(ai->a_device);
}

void do_tag_backup (void)
{

	if (check_tag_count())
		return;
	if (ask_questions())
		return;
	ai->a_links = 0;

	if (arch_open())
	{
		FREE(ai->a_buffer);
		return;
	}
	if (ai->a_dev_type == A_CPIO)
	{
		ai->cpio_buffer = (char *)MALLOC(ai->cpio_bufsize);
		if (!ai->cpio_buffer)
		{
			FREE(ai->a_buffer);
			arch_close();
			errmsg(ER_IM, "", ERR_ANY);
			return;
		}
	}

	traverse(arch_file, 0);

	switch (ai->a_dev_type)
	{
	case A_BAR:
		bar_wr_tlr();
		break;

	case A_TAR:
		break;

	case A_CPIO:
		cpio_wr_tlr();
		FREE(ai->cpio_buffer);
		break;

	case A_ZTAR:
		break;
	}
	arch_close();

	FREE(ai->a_buffer);
	ai->a_links = BSCRAP(ai->a_links, FALSE);
	if (ai->a_dev_type == A_DEV_FILE)
		check_the_file(ai->a_device);
}
