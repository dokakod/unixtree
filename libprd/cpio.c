/*------------------------------------------------------------------------
 * "cpio" file routines
 */
#include "libprd.h"

/*
 *	low level routines for reading/writing cpio files
 *
 *	entry points:
 *
 *		cpio_open()
 *		cpio_close(e)
 *		cpio_read_rec(b, l)
 *		cpio_write_rec(b, l)
 *		cpio_wr_hdr(f, r)
 *		cpio_wr_tlr()
 *		cpio_get_stbuf(s)
 *		cpio_skip_file()
 *		cpio_skip_bytes(n)
 *		cpio_is_valid_hdr()
 *		cpio_position(n, v, p)
 *		cpio_ask_for_volno(n, v)
 *		cpio_filename()
 */

static int we_are_little_endian (void)
{
	int one = 1;

	return (*(unsigned char *)&one == 1);
}

static long cpio_calc_chksum (void)
{
	long chksum;

	chksum = 0;
	return (chksum);
}

static int cpio_make_hdr (FBLK *f, char *rel_name)
{
	char path[MAX_PATHLEN];
	long chksum;
	int l;
	char *n;
	int c;

	switch (ai->a_hdr_type)
	{
	case cpio_hdr_type_bin:
		ai->cpio_hdr.bh.h_magic = CMN_BIN;
		ai->cpio_hdr.bh.h_dev   = f->stbuf.st_dev;
		ai->cpio_hdr.bh.h_ino   = f->stbuf.st_ino;
		ai->cpio_hdr.bh.h_mode  = f->stbuf.st_mode;
		ai->cpio_hdr.bh.h_uid   = f->stbuf.st_uid;
		ai->cpio_hdr.bh.h_gid   = f->stbuf.st_gid;
		ai->cpio_hdr.bh.h_nlink = f->stbuf.st_nlink;
		ai->cpio_hdr.bh.h_rdev  = get_rdev(&f->stbuf);
		memcpy(ai->cpio_hdr.bh.h_mtime,  &f->stbuf.st_mtime,
			sizeof(f->stbuf.st_mtime));
		memcpy(ai->cpio_hdr.bh.h_filesz, &f->stbuf.st_size,
			sizeof(f->stbuf.st_size));

		if (we_are_little_endian())
		{
			short foo;

			foo = ai->cpio_hdr.bh.h_mtime[0];
			ai->cpio_hdr.bh.h_mtime[0] = ai->cpio_hdr.bh.h_mtime[1];
			ai->cpio_hdr.bh.h_mtime[1] = foo;

			foo = ai->cpio_hdr.bh.h_filesz[0];
			ai->cpio_hdr.bh.h_filesz[0] = ai->cpio_hdr.bh.h_filesz[1];
			ai->cpio_hdr.bh.h_filesz[1] = foo;
		}

		memset(ai->cpio_hdr.bh.h_name, 0, sizeof(ai->cpio_hdr.bh.h_name));
		n = get_name(f, rel_name, path);
		if ((int)strlen(n) > HNAMLEN-1)
		{
			c = errmsg(ER_NAMTL, "", ERR_ANY);
			return (c);
		}
		strcpy(ai->cpio_hdr.bh.h_name, n);
		l = strlen(ai->cpio_hdr.bh.h_name)+1;
		ai->cpio_hdr.bh.h_namesz = l;

		ai->cpio_hdr_len = BINSZ + l;
		if (ai->cpio_hdr_len & 1)			/* force to even value */
			ai->cpio_hdr_len++;

		if (ai->cpio_hdr_is_bbs)
		{
			X_SWAP2(&ai->cpio_hdr.bh.h_magic);
			X_SWAP2(&ai->cpio_hdr.bh.h_dev);
			X_SWAP2(&ai->cpio_hdr.bh.h_ino);
			X_SWAP2(&ai->cpio_hdr.bh.h_mode);
			X_SWAP2(&ai->cpio_hdr.bh.h_uid);
			X_SWAP2(&ai->cpio_hdr.bh.h_gid);
			X_SWAP2(&ai->cpio_hdr.bh.h_nlink);
			X_SWAP2(&ai->cpio_hdr.bh.h_rdev);
			X_SWAP2(&ai->cpio_hdr.bh.h_mtime[0]);
			X_SWAP2(&ai->cpio_hdr.bh.h_mtime[1]);
			X_SWAP2(&ai->cpio_hdr.bh.h_namesz);
			X_SWAP2(&ai->cpio_hdr.bh.h_filesz[0]);
			X_SWAP2(&ai->cpio_hdr.bh.h_filesz[1]);
		}
		break;

	case cpio_hdr_type_chr:
		strncpy(ai->cpio_hdr.ch.c_magic, CMS_CHR,
			sizeof(ai->cpio_hdr.ch.c_magic));
		cvt_to_oct(f->stbuf.st_dev,  ai->cpio_hdr.ch.c_dev,
			sizeof(ai->cpio_hdr.ch.c_dev), 6);
		cvt_to_oct(f->stbuf.st_ino,  ai->cpio_hdr.ch.c_ino,
			sizeof(ai->cpio_hdr.ch.c_ino), 6);
		cvt_to_oct(f->stbuf.st_mode, ai->cpio_hdr.ch.c_mode,
			sizeof(ai->cpio_hdr.ch.c_mode), 6);
		cvt_to_oct(f->stbuf.st_uid,  ai->cpio_hdr.ch.c_uid,
			sizeof(ai->cpio_hdr.ch.c_uid), 6);
		cvt_to_oct(f->stbuf.st_gid,  ai->cpio_hdr.ch.c_gid,
			sizeof(ai->cpio_hdr.ch.c_gid), 6);
		cvt_to_oct(f->stbuf.st_nlink, ai->cpio_hdr.ch.c_nlink,
			sizeof(ai->cpio_hdr.ch.c_nlink), 6);
		cvt_to_oct(get_rdev(&f->stbuf), ai->cpio_hdr.ch.c_rdev,
			sizeof(ai->cpio_hdr.ch.c_rdev), 6);
		cvt_to_oct(f->stbuf.st_mtime, ai->cpio_hdr.ch.c_mtime,
			sizeof(ai->cpio_hdr.ch.c_mtime), 6);
		cvt_to_oct(f->stbuf.st_size, ai->cpio_hdr.ch.c_filesz,
			sizeof(ai->cpio_hdr.ch.c_filesz), 6);
		memset(ai->cpio_hdr.ch.c_name, 0, sizeof(ai->cpio_hdr.ch.c_name));
		n = get_name(f, rel_name, path);
		if ((int)strlen(n) > HNAMLEN-1)
		{
			c = errmsg(ER_NAMTL, "", ERR_ANY);
			return (c);
		}
		strcpy(ai->cpio_hdr.ch.c_name, n);
		l = strlen(ai->cpio_hdr.ch.c_name) + 1;
		cvt_to_oct(l, ai->cpio_hdr.ch.c_namesz,
			sizeof(ai->cpio_hdr.ch.c_namesz), 6);

		ai->cpio_hdr_len = CHRSZ + l;
		break;

	case cpio_hdr_type_asc:
	case cpio_hdr_type_crc:
		if (ai->a_hdr_type == cpio_hdr_type_asc)
		{
			strncpy(ai->cpio_hdr.ah.e_magic, CMS_ASC,
				sizeof(ai->cpio_hdr.ah.e_magic));
		}
		else
		{
			strncpy(ai->cpio_hdr.ah.e_magic, CMS_CRC,
				sizeof(ai->cpio_hdr.ah.e_magic));
		}
		cvt_to_hex(f->stbuf.st_ino,   ai->cpio_hdr.ah.e_ino,
			sizeof(ai->cpio_hdr.ah.e_ino));
		cvt_to_hex(f->stbuf.st_mode,  ai->cpio_hdr.ah.e_mode,
			sizeof(ai->cpio_hdr.ah.e_mode));
		cvt_to_hex(f->stbuf.st_uid,   ai->cpio_hdr.ah.e_uid,
			sizeof(ai->cpio_hdr.ah.e_uid));
		cvt_to_hex(f->stbuf.st_gid,   ai->cpio_hdr.ah.e_gid,
			sizeof(ai->cpio_hdr.ah.e_gid));
		cvt_to_hex(f->stbuf.st_nlink, ai->cpio_hdr.ah.e_nlink,
			sizeof(ai->cpio_hdr.ah.e_nlink));
		cvt_to_hex(f->stbuf.st_mtime, ai->cpio_hdr.ah.e_mtime,
			sizeof(ai->cpio_hdr.ah.e_mtime));
		cvt_to_hex(f->stbuf.st_size,  ai->cpio_hdr.ah.e_filesz,
			sizeof(ai->cpio_hdr.ah.e_filesz));
		cvt_to_hex(f->stbuf.st_dev >> 8,    ai->cpio_hdr.ah.e_maj,
			sizeof(ai->cpio_hdr.ah.e_maj));
		cvt_to_hex(f->stbuf.st_dev & 0xff,  ai->cpio_hdr.ah.e_min,
			sizeof(ai->cpio_hdr.ah.e_min));
		cvt_to_hex(get_major(&f->stbuf), ai->cpio_hdr.ah.e_rmaj,
			sizeof(ai->cpio_hdr.ah.e_rmaj));
		cvt_to_hex(get_minor(&f->stbuf), ai->cpio_hdr.ah.e_rmin,
			sizeof(ai->cpio_hdr.ah.e_rmin));

		memset(ai->cpio_hdr.ah.e_name, 0, sizeof(ai->cpio_hdr.ah.e_name));
		n = get_name(f, rel_name, path);
		if ((int)strlen(n) > ENAMLEN-1)
		{
			c = errmsg(ER_NAMTL, "", ERR_ANY);
			return (c);
		}
		strcpy(ai->cpio_hdr.ah.e_name, n);
		l = strlen(ai->cpio_hdr.ah.e_name) + 1;
		if (ai->a_hdr_type == cpio_hdr_type_crc)
			l = ((l + ASCSZ + 3) & ~3) - ASCSZ;
		cvt_to_hex(l, ai->cpio_hdr.ah.e_namesz,
			sizeof(ai->cpio_hdr.ah.e_namesz));

		if (ai->a_hdr_type == cpio_hdr_type_asc)
		{
			cvt_to_hex(0, ai->cpio_hdr.ah.e_chksum,
				sizeof(ai->cpio_hdr.ah.e_chksum));
		}
		else
		{
			chksum = cpio_calc_chksum();
			cvt_to_hex(chksum, ai->cpio_hdr.ah.e_chksum,
				sizeof(ai->cpio_hdr.ah.e_chksum));
		}

		ai->cpio_hdr_len = ASCSZ + l;
		break;
	}
	return (0);
}

static void cpio_make_tlr (void)
{
	long chksum;
	int l;
	long zero = 0;

	switch (ai->a_hdr_type)
	{
	case cpio_hdr_type_bin:
		ai->cpio_hdr.bh.h_magic = CMN_BIN;
		ai->cpio_hdr.bh.h_dev   = 0;
		ai->cpio_hdr.bh.h_ino   = 0;
		ai->cpio_hdr.bh.h_mode  = 0;
		ai->cpio_hdr.bh.h_uid   = 0;
		ai->cpio_hdr.bh.h_gid   = 0;
		ai->cpio_hdr.bh.h_nlink = 1;
		ai->cpio_hdr.bh.h_rdev  = 0;
		memcpy(ai->cpio_hdr.bh.h_mtime, &zero, sizeof(zero));
		memcpy(ai->cpio_hdr.bh.h_filesz, &zero, sizeof(zero));

		if (we_are_little_endian())
		{
			short foo;

			foo = ai->cpio_hdr.bh.h_mtime[0];
			ai->cpio_hdr.bh.h_mtime[0] = ai->cpio_hdr.bh.h_mtime[1];
			ai->cpio_hdr.bh.h_mtime[1] = foo;

			foo = ai->cpio_hdr.bh.h_filesz[0];
			ai->cpio_hdr.bh.h_filesz[0] = ai->cpio_hdr.bh.h_filesz[1];
			ai->cpio_hdr.bh.h_filesz[1] = foo;
		}

		memset(ai->cpio_hdr.bh.h_name, 0, sizeof(ai->cpio_hdr.bh.h_name));
		strcpy(ai->cpio_hdr.bh.h_name, CPIO_TRAILER);
		l = strlen(ai->cpio_hdr.bh.h_name)+1;
		ai->cpio_hdr.bh.h_namesz = l;

		ai->cpio_hdr_len = BINSZ + l;
		if (ai->cpio_hdr_len & 1)			/* force to even value */
			ai->cpio_hdr_len++;

		if (ai->cpio_hdr_is_bbs)
		{
			X_SWAP2(&ai->cpio_hdr.bh.h_magic);
			X_SWAP2(&ai->cpio_hdr.bh.h_dev);
			X_SWAP2(&ai->cpio_hdr.bh.h_ino);
			X_SWAP2(&ai->cpio_hdr.bh.h_mode);
			X_SWAP2(&ai->cpio_hdr.bh.h_uid);
			X_SWAP2(&ai->cpio_hdr.bh.h_gid);
			X_SWAP2(&ai->cpio_hdr.bh.h_nlink);
			X_SWAP2(&ai->cpio_hdr.bh.h_rdev);
			X_SWAP2(&ai->cpio_hdr.bh.h_mtime[0]);
			X_SWAP2(&ai->cpio_hdr.bh.h_mtime[1]);
			X_SWAP2(&ai->cpio_hdr.bh.h_namesz);
			X_SWAP2(&ai->cpio_hdr.bh.h_filesz[0]);
			X_SWAP2(&ai->cpio_hdr.bh.h_filesz[1]);
		}
		break;

	case cpio_hdr_type_chr:
		strncpy(ai->cpio_hdr.ch.c_magic, CMS_CHR,
			sizeof(ai->cpio_hdr.ch.c_magic));
		cvt_to_oct(0, ai->cpio_hdr.ch.c_dev,
			sizeof(ai->cpio_hdr.ch.c_dev), 6);
		cvt_to_oct(0, ai->cpio_hdr.ch.c_ino,
			sizeof(ai->cpio_hdr.ch.c_ino), 6);
		cvt_to_oct(0, ai->cpio_hdr.ch.c_mode,
			sizeof(ai->cpio_hdr.ch.c_mode), 6);
		cvt_to_oct(0, ai->cpio_hdr.ch.c_uid,
			sizeof(ai->cpio_hdr.ch.c_uid), 6);
		cvt_to_oct(0, ai->cpio_hdr.ch.c_gid,
			sizeof(ai->cpio_hdr.ch.c_gid), 6);
		cvt_to_oct(1, ai->cpio_hdr.ch.c_nlink,
			sizeof(ai->cpio_hdr.ch.c_nlink), 6);
		cvt_to_oct(0, ai->cpio_hdr.ch.c_rdev,
			sizeof(ai->cpio_hdr.ch.c_rdev), 6);
		cvt_to_oct(0, ai->cpio_hdr.ch.c_mtime,
			sizeof(ai->cpio_hdr.ch.c_mtime), 6);
		cvt_to_oct(0, ai->cpio_hdr.ch.c_filesz,
			sizeof(ai->cpio_hdr.ch.c_filesz), 6);
		memset(ai->cpio_hdr.ch.c_name, 0, sizeof(ai->cpio_hdr.ch.c_name));
		strcpy(ai->cpio_hdr.ch.c_name, CPIO_TRAILER);
		l = strlen(ai->cpio_hdr.ch.c_name) + 1;
		cvt_to_oct(l, ai->cpio_hdr.ch.c_namesz,
			sizeof(ai->cpio_hdr.ch.c_namesz), 6);

		ai->cpio_hdr_len = CHRSZ + l;
		break;

	case cpio_hdr_type_asc:
	case cpio_hdr_type_crc:
		if (ai->a_hdr_type == cpio_hdr_type_asc)
		{
			strncpy(ai->cpio_hdr.ah.e_magic, CMS_ASC,
				sizeof(ai->cpio_hdr.ah.e_magic));
		}
		else
		{
			strncpy(ai->cpio_hdr.ah.e_magic, CMS_CRC,
				sizeof(ai->cpio_hdr.ah.e_magic));
		}
		cvt_to_hex(0, ai->cpio_hdr.ah.e_ino,
			sizeof(ai->cpio_hdr.ah.e_ino));
		cvt_to_hex(0, ai->cpio_hdr.ah.e_mode,
			sizeof(ai->cpio_hdr.ah.e_mode));
		cvt_to_hex(0, ai->cpio_hdr.ah.e_uid,
			sizeof(ai->cpio_hdr.ah.e_uid));
		cvt_to_hex(0, ai->cpio_hdr.ah.e_gid,
			sizeof(ai->cpio_hdr.ah.e_gid));
		cvt_to_hex(1, ai->cpio_hdr.ah.e_nlink,
			sizeof(ai->cpio_hdr.ah.e_nlink));
		cvt_to_hex(0, ai->cpio_hdr.ah.e_mtime,
			sizeof(ai->cpio_hdr.ah.e_mtime));
		cvt_to_hex(0, ai->cpio_hdr.ah.e_filesz,
			sizeof(ai->cpio_hdr.ah.e_filesz));
		cvt_to_hex(0, ai->cpio_hdr.ah.e_maj,
			sizeof(ai->cpio_hdr.ah.e_maj));
		cvt_to_hex(0, ai->cpio_hdr.ah.e_min,
			sizeof(ai->cpio_hdr.ah.e_min));
		cvt_to_hex(0, ai->cpio_hdr.ah.e_rmaj,
			sizeof(ai->cpio_hdr.ah.e_rmaj));
		cvt_to_hex(0, ai->cpio_hdr.ah.e_rmin,
			sizeof(ai->cpio_hdr.ah.e_rmin));

		memset(ai->cpio_hdr.ah.e_name, 0, sizeof(ai->cpio_hdr.ah.e_name));
		strcpy(ai->cpio_hdr.ah.e_name, CPIO_TRAILER);
		l = strlen(ai->cpio_hdr.ah.e_name) + 1;
		if (ai->a_hdr_type == cpio_hdr_type_crc)
			l = ((l + ASCSZ + 3) & ~3) - ASCSZ;
		cvt_to_hex(l, ai->cpio_hdr.ah.e_namesz,
			sizeof(ai->cpio_hdr.ah.e_namesz));

		if (ai->a_hdr_type == cpio_hdr_type_asc)
		{
			cvt_to_hex(0, ai->cpio_hdr.ah.e_chksum,
				sizeof(ai->cpio_hdr.ah.e_chksum));
		}
		else
		{
			chksum = cpio_calc_chksum();
			cvt_to_hex(chksum, ai->cpio_hdr.ah.e_chksum,
				sizeof(ai->cpio_hdr.ah.e_chksum));
		}

		ai->cpio_hdr_len = ASCSZ + l;
		break;
	}
}

static int cpio_ask_for_volno (const char *n, int v)
{
	int c;

	if (ai->a_fd > 0)
		cpio_close(TRUE);
	strcpy(ai->a_name, n);
	ai->a_volno = v;
	if (ai->a_dev_type != A_DEV_FILE)
	{
		char buf[128];
		char dbuf[12];

		strcpy(buf, msgs(m_cpio_askvol));
		strcat(buf, itos(dbuf, ai->a_volno));
		strcat(buf, msgs(m_cpio_of));
		strcat(buf, ai->a_name);
		c = anykey_msg(buf);
		if (c < 0)
			return (-1);
	}
	ai->a_init = FALSE;
	c = cpio_open();
	ai->a_init = TRUE;
	if (c)
		return (-1);
	return (0);
}

static int cpio_skip_bytes (int n)
{
	long new_loc;
	int new_pos;
	long new_addr;
	int i;

	if (n < 0)
	{
		n = -n;
		if (ai->cpio_buf_pos >= n)
		{
			ai->cpio_buf_pos -= n;
			return (0);
		}
		new_loc = ai->cpio_buf_addr + ai->cpio_buf_pos - n;
	}
	else
	{
		if (ai->cpio_buf_pos + n < ai->cpio_bufsize)
		{
			ai->cpio_buf_pos += n;
			return (0);
		}
		new_loc = ai->cpio_buf_addr + ai->cpio_buf_pos + n;
	}

	new_pos = new_loc % ai->cpio_bufsize;
	new_addr = new_loc - new_pos;

	if (ai->cpio_volsize && new_addr >= ai->cpio_volsize)
	{
		i = new_addr / ai->cpio_volsize;
		if (cpio_ask_for_volno(ai->a_name, ai->a_volno+i))
			return (-1);
		new_addr -= i * ai->cpio_volsize;
		ai->cpio_buf_addr = 0;
	}

	if (ai->a_dev_type == A_DEV_NOSEEK)
	{
		while (ai->cpio_buf_addr < new_addr)
		{
			ai->cpio_buf_len = os_read(ai->a_fd, ai->a_buffer,
				ai->cpio_bufsize);
			if (ai->cpio_buf_len == -1)
			{
				errsys(ER_IOERR);
				return (-1);
			}
			ai->cpio_buf_addr += ai->cpio_bufsize;
		}
		ai->cpio_buf_pos = new_pos;
	}
	else
	{
		if (lseek(ai->a_fd, new_addr, SEEK_SET) == -1)
			return (-1);
		ai->cpio_buf_len = os_read(ai->a_fd, ai->a_buffer, ai->cpio_bufsize);
		if (ai->cpio_buf_len == -1)
			return (-1);
		ai->cpio_buf_addr = new_addr;
		ai->cpio_buf_pos = new_pos;
	}
	return (0);
}

int cpio_open (void)
{
	char *p;
	int c;
	int open_flags;

	ai->a_ioerr = FALSE;
	if (ai->a_init)
	{
		ai->cpio_bufsize = ai->a_blksize * ai->a_blkfactor;
		ai->cpio_volsize = ai->a_volsize * ai->a_blksize;
		switch (opt(cpio_hdr_fmt))
		{
		case cpio_hdr_bin:
			ai->a_hdr_type = cpio_hdr_type_bin;
			ai->cpio_hdr_is_bbs = FALSE;
			break;
		case cpio_hdr_bbs:
			ai->a_hdr_type = cpio_hdr_type_bin;
			ai->cpio_hdr_is_bbs = TRUE;
			break;
		case cpio_hdr_chr:
			ai->a_hdr_type = cpio_hdr_type_chr;
			break;
		case cpio_hdr_asc:
			ai->a_hdr_type = cpio_hdr_type_asc;
			break;
		case cpio_hdr_crc:
			ai->a_hdr_type = cpio_hdr_type_crc;
			break;
		}
		ai->cpio_buf_pos = -1;
		ai->cpio_buf_addr = -1;
	}

	open_flags = ai->a_open_flags;
	if (ai->a_dev_type != A_DEV_FILE)
		open_flags |= O_EXCL;
	if (ai->a_open_flags == O_WRONLY)
	{
		if (ai->a_dev_type == A_DEV_FILE)
			open_flags |= (O_CREAT | O_TRUNC);
	}
	ai->a_fd = os_open(ai->a_device, open_flags, 0666);
	if (ai->a_fd < 0)
	{
		/* try second open, first failure may be bogus */
		ai->a_fd = os_open(ai->a_device, open_flags, 0666);
		if (ai->a_fd < 0)
		{
			ai->a_fd = 0;
			c = errsys(ER_COTF);
			return (c);
		}
	}

	/*
	 *	If opened read/write (instead of read-only or write-only),
	 *	then position to end of file (at trailer cpio_hdr)
	 */

	if (ai->a_init && ai->a_open_flags == O_RDWR)
	{
		while (TRUE)
		{
			if (cpio_rd_hdr())
			{
				ai->a_ioerr = 1;
				cpio_close(TRUE);
				return (-1);
			}
			p = cpio_filename();
			if (strcmp(p, CPIO_TRAILER) == 0)
				break;
			if (cpio_skip_file())
			{
				ai->a_ioerr = 1;
				cpio_close(TRUE);
				return (-1);
			}
		}
		if (cpio_skip_bytes(-ai->cpio_hdr_len))
		{
			ai->a_ioerr = 1;
			cpio_close(TRUE);
			return (-1);
		}
		if (lseek(ai->a_fd, ai->cpio_buf_addr, SEEK_SET) == -1)
		{
			errsys(ER_IOERR);
			ai->a_ioerr = 1;
			cpio_close(TRUE);
			return (-1);
		}
	}
	else
	{
		os_set_sync_mode(ai->a_fd);
	}

	return (0);
}

/* ARGSUSED */
int cpio_close (int eject)
{
	if (!ai->a_ioerr && ai->a_open_flags != O_RDONLY)
	{
		/* set SYNC flag on write to insure blocks get written */

		/* zero fill last buffer & flush */

		if (ai->cpio_buf_pos)
		{
			memset(ai->a_buffer+ai->cpio_buf_pos, 0,
				ai->cpio_bufsize-ai->cpio_buf_pos);
			if (os_write(ai->a_fd, ai->a_buffer, ai->cpio_bufsize) == -1)
				errsys(ER_IOERR);
		}
	}

	if (eject)
	{
		if (ai->a_dev_type == A_DEV_SEEK)
			os_floppy_eject(ai->a_fd);
	}

	os_close(ai->a_fd);
	ai->a_fd = 0;

	return (0);
}

int cpio_read_rec (char *buf, int len)
{
	int l;
	int num_read = 0;

	/* prime buffer if no reads yet */

	if (ai->cpio_buf_addr == -1)
	{
		ai->cpio_buf_len = os_read(ai->a_fd, ai->a_buffer, ai->cpio_bufsize);
		if (ai->cpio_buf_len == -1)
		{
			errsys(ER_IOERR);
			return (-1);
		}
		if (ai->cpio_buf_len == 0)
			return (num_read);
		ai->cpio_buf_addr = 0;
		ai->cpio_buf_pos = 0;
	}

	/* fill from buffer & new buffers if necessary */

	while (len)
	{
		/* fill from current buffer */

		if (ai->cpio_buf_pos < ai->cpio_buf_len)
		{
			l = len;
			if (l > (ai->cpio_buf_len - ai->cpio_buf_pos))
				l = (ai->cpio_buf_len - ai->cpio_buf_pos);
			memcpy(buf, ai->a_buffer+ai->cpio_buf_pos, l);
			buf += l;
			len -= l;
			num_read += l;
			ai->cpio_buf_pos += l;
			if (!len)
				break;
		}

		/* get new buffer */

		ai->cpio_buf_addr += ai->cpio_buf_len;
		if (ai->cpio_volsize && ai->cpio_buf_addr >= ai->cpio_volsize)
		{
			/* ask for new volume */

			if (cpio_ask_for_volno(ai->a_name, ai->a_volno+1))
				return (-1);
			ai->cpio_buf_addr = 0;
		}
		ai->cpio_buf_len = os_read(ai->a_fd, ai->a_buffer, ai->cpio_bufsize);
		if (ai->cpio_buf_len == -1)
		{
			errsys(ER_IOERR);
			return (-1);
		}
		if (ai->cpio_buf_len == 0)
			return (num_read);
		ai->cpio_buf_pos = 0;
	}
	return (num_read);
}

int cpio_write_rec (const char *buf, int len)
{
	int l;

	/* initialize if first time */

	if (ai->cpio_buf_addr == -1)
	{
		ai->cpio_buf_pos = 0;
		ai->cpio_buf_addr = 0;
		ai->cpio_buf_len = ai->cpio_bufsize;
	}

	while (len)
	{
		/* check if past end-of-vol */

		if (ai->cpio_volsize && ai->cpio_buf_pos == 0 &&
			ai->cpio_buf_addr >= ai->cpio_volsize)
		{
			/* ask for new volume */

			if (cpio_ask_for_volno(ai->a_name, ai->a_volno+1))
				return (-1);
			ai->cpio_buf_addr = 0;
		}

		/* fill buffer */

		l = len;
		if (l > (ai->cpio_buf_len - ai->cpio_buf_pos))
			l = (ai->cpio_buf_len - ai->cpio_buf_pos);
		memcpy(ai->a_buffer+ai->cpio_buf_pos, buf, l);
		buf += l;
		len -= l;
		ai->cpio_buf_pos += l;

		/* check if full block */

		if (ai->cpio_buf_pos >= ai->cpio_bufsize)
		{
			l = os_write(ai->a_fd, ai->a_buffer, ai->cpio_bufsize);
			if (l == -1)
			{
				ai->a_ioerr = TRUE;
				errsys(ER_IOERR);
				return (-1);
			}
			ai->cpio_buf_addr += ai->cpio_bufsize;
			ai->cpio_buf_pos = 0;
		}
	}
	return (0);
}

int cpio_wr_hdr (FBLK *f, char *rel_name)
{
	long zero = 0;
	int l;
	int c;

	if (ai->cpio_buf_pos > 0)
	{
		if (ai->a_hdr_type == cpio_hdr_type_bin)
		{
			if (ai->cpio_buf_pos & 1)
				cpio_write_rec((char *)&zero, 1);
		}
		else if (ai->a_hdr_type == cpio_hdr_type_crc)
		{
			l = ai->cpio_buf_pos & 3;
			if (l)
				cpio_write_rec((char *)&zero, 4-l);
		}
	}

	c = cpio_make_hdr(f, rel_name);
	if (c)
		return (c);
	return (cpio_write_rec((char *)&ai->cpio_hdr, ai->cpio_hdr_len));
}

int cpio_wr_tlr (void)
{
	long zero = 0;
	int l;

	if (ai->cpio_buf_pos > 0)
	{
		if (ai->a_hdr_type == cpio_hdr_type_bin)
		{
			if (ai->cpio_buf_pos & 1)
				cpio_write_rec((char *)&zero, 1);
		}
		else if (ai->a_hdr_type == cpio_hdr_type_crc)
		{
			l = ai->cpio_buf_pos & 3;
			if (l)
				cpio_write_rec((char *)&zero, 4-l);
		}
	}

	cpio_make_tlr();
	return (cpio_write_rec((char *)&ai->cpio_hdr, ai->cpio_hdr_len));
}

int cpio_rd_hdr (void)
{
	int i;
	int l;

	i = cpio_read_rec((char *)&ai->cpio_hdr, 6);
	if (i != 6)
		return (-1);

	if (ai->cpio_hdr.bh.h_magic == CMN_BIN)
	{
		ai->a_hdr_type = cpio_hdr_type_bin;
		ai->cpio_hdr_is_bbs = FALSE;
	}
	else if (ai->cpio_hdr.bh.h_magic == CMN_BBS)
	{
		ai->a_hdr_type = cpio_hdr_type_bin;
		ai->cpio_hdr_is_bbs = TRUE;
	}
	else if (strncmp(ai->cpio_hdr.ch.c_magic, CMS_CHR,
		sizeof(ai->cpio_hdr.ch.c_magic)) == 0)
	{
		ai->a_hdr_type = cpio_hdr_type_chr;
	}
	else if (strncmp(ai->cpio_hdr.ah.e_magic, CMS_ASC,
		sizeof(ai->cpio_hdr.ah.e_magic)) == 0)
	{
		ai->a_hdr_type = cpio_hdr_type_asc;
	}
	else if (strncmp(ai->cpio_hdr.ah.e_magic, CMS_CRC,
		sizeof(ai->cpio_hdr.ah.e_magic)) == 0)
	{
		ai->a_hdr_type = cpio_hdr_type_crc;
	}
	else
	{
		errmsg(ER_NOTCPIO, "", ERR_ANY);
		return (-1);
	}

	switch (ai->a_hdr_type)
	{
	case cpio_hdr_type_bin:
		i = cpio_read_rec((char *)&ai->cpio_hdr+6, BINSZ-6);
		if (i != BINSZ-6)
			return (-1);
		if (ai->cpio_hdr_is_bbs)
		{
			X_SWAP2(&ai->cpio_hdr.bh.h_magic);
			X_SWAP2(&ai->cpio_hdr.bh.h_dev);
			X_SWAP2(&ai->cpio_hdr.bh.h_ino);
			X_SWAP2(&ai->cpio_hdr.bh.h_mode);
			X_SWAP2(&ai->cpio_hdr.bh.h_uid);
			X_SWAP2(&ai->cpio_hdr.bh.h_gid);
			X_SWAP2(&ai->cpio_hdr.bh.h_nlink);
			X_SWAP2(&ai->cpio_hdr.bh.h_rdev);
			X_SWAP2(&ai->cpio_hdr.bh.h_mtime[0]);
			X_SWAP2(&ai->cpio_hdr.bh.h_mtime[1]);
			X_SWAP2(&ai->cpio_hdr.bh.h_namesz);
			X_SWAP2(&ai->cpio_hdr.bh.h_filesz[0]);
			X_SWAP2(&ai->cpio_hdr.bh.h_filesz[1]);
		}

		if (we_are_little_endian())
		{
			short foo;

			foo = ai->cpio_hdr.bh.h_mtime[0];
			ai->cpio_hdr.bh.h_mtime[0] = ai->cpio_hdr.bh.h_mtime[1];
			ai->cpio_hdr.bh.h_mtime[1] = foo;

			foo = ai->cpio_hdr.bh.h_filesz[0];
			ai->cpio_hdr.bh.h_filesz[0] = ai->cpio_hdr.bh.h_filesz[1];
			ai->cpio_hdr.bh.h_filesz[1] = foo;
		}

		l = ai->cpio_hdr.bh.h_namesz;
		if (l & 1)
			l++;
		i = cpio_read_rec(ai->cpio_hdr.bh.h_name, l);
		if (i != l)
			return (-1);
		ai->cpio_hdr_len = BINSZ + l;
		break;

	case cpio_hdr_type_chr:
		i = cpio_read_rec((char *)&ai->cpio_hdr+6, CHRSZ-6);
		if (i != CHRSZ-6)
			return (-1);
		l = cvt_from_oct(ai->cpio_hdr.ch.c_namesz,
			sizeof(ai->cpio_hdr.ch.c_namesz));
		i = cpio_read_rec(ai->cpio_hdr.ch.c_name, l);
		if (i != l)
			return (-1);
		ai->cpio_hdr_len = CHRSZ + l;
		break;

	case cpio_hdr_type_asc:
	case cpio_hdr_type_crc:
		i = cpio_read_rec((char *)&ai->cpio_hdr+6, ASCSZ-6);
		if (i != ASCSZ-6)
			return (-1);
		l = cvt_from_hex(ai->cpio_hdr.ah.e_namesz,
			sizeof(ai->cpio_hdr.ah.e_namesz));
		if (ai->a_hdr_type == cpio_hdr_type_crc)
			l = ((l + ASCSZ + 3) & ~3) - ASCSZ;
		i = cpio_read_rec(ai->cpio_hdr.ah.e_name, l);
		if (i != l)
			return (-1);
		ai->cpio_hdr_len = ASCSZ + l;
		break;
	}
	return (0);
}

/* convert cpio header to phony stbuf */

void cpio_get_stbuf (struct stat *s)
{
	switch (ai->a_hdr_type)
	{
	case cpio_hdr_type_bin:
		s->st_dev   = ai->cpio_hdr.bh.h_dev;
		s->st_ino   = ai->cpio_hdr.bh.h_ino;
		s->st_mode  = ai->cpio_hdr.bh.h_mode;
		s->st_uid   = ai->cpio_hdr.bh.h_uid;
		s->st_gid   = ai->cpio_hdr.bh.h_gid;
		s->st_nlink = ai->cpio_hdr.bh.h_nlink;
		set_rdev(s, ai->cpio_hdr.bh.h_rdev);
		memcpy(&s->st_mtime, ai->cpio_hdr.bh.h_mtime, sizeof(s->st_mtime));
		s->st_ctime = s->st_mtime;
		s->st_atime = s->st_mtime;
		memcpy(&s->st_size, ai->cpio_hdr.bh.h_filesz, sizeof(s->st_size));
		break;

	case cpio_hdr_type_chr:
		s->st_dev   = (dev_t)cvt_from_oct(ai->cpio_hdr.ch.c_dev,
						sizeof(ai->cpio_hdr.ch.c_dev));
		s->st_ino   = (ino_t)cvt_from_oct(ai->cpio_hdr.ch.c_ino,
						sizeof(ai->cpio_hdr.ch.c_ino));
		s->st_mode  = (mode_t)cvt_from_oct(ai->cpio_hdr.ch.c_mode,
						sizeof(ai->cpio_hdr.ch.c_mode));
		s->st_uid   = (uid_t)cvt_from_oct(ai->cpio_hdr.ch.c_uid,
						sizeof(ai->cpio_hdr.ch.c_uid));
		s->st_gid   = (gid_t)cvt_from_oct(ai->cpio_hdr.ch.c_gid,
						sizeof(ai->cpio_hdr.ch.c_gid));
		s->st_nlink = (nlink_t)cvt_from_oct(ai->cpio_hdr.ch.c_nlink,
			sizeof(ai->cpio_hdr.ch.c_nlink));
		set_rdev(s, (int)cvt_from_oct(ai->cpio_hdr.ch.c_rdev,
			sizeof(ai->cpio_hdr.ch.c_rdev)));
		s->st_mtime = (time_t)cvt_from_oct(ai->cpio_hdr.ch.c_mtime,
			sizeof(ai->cpio_hdr.ch.c_mtime));
		s->st_ctime = s->st_mtime;
		s->st_atime = s->st_mtime;
		s->st_size  = (off_t)cvt_from_oct(ai->cpio_hdr.ch.c_filesz,
			sizeof(ai->cpio_hdr.ch.c_filesz));
		break;

	case cpio_hdr_type_asc:
	case cpio_hdr_type_crc:
		s->st_ino   = (ino_t)cvt_from_hex(ai->cpio_hdr.ah.e_ino,
						sizeof(ai->cpio_hdr.ah.e_ino));
		s->st_mode  = (mode_t)cvt_from_hex(ai->cpio_hdr.ah.e_mode,
						sizeof(ai->cpio_hdr.ah.e_mode));
		s->st_uid   = (uid_t)cvt_from_hex(ai->cpio_hdr.ah.e_uid,
						sizeof(ai->cpio_hdr.ah.e_uid));
		s->st_gid   = (gid_t)cvt_from_hex(ai->cpio_hdr.ah.e_gid,
						sizeof(ai->cpio_hdr.ah.e_gid));
		s->st_nlink = (nlink_t)cvt_from_hex(ai->cpio_hdr.ah.e_nlink,
						sizeof(ai->cpio_hdr.ah.e_nlink));
		s->st_mtime = (time_t)cvt_from_hex(ai->cpio_hdr.ah.e_mtime,
						sizeof(ai->cpio_hdr.ah.e_mtime));
		s->st_ctime = s->st_mtime;
		s->st_atime = s->st_mtime;
		s->st_size  = (off_t)cvt_from_hex(ai->cpio_hdr.ah.e_filesz,
						sizeof(ai->cpio_hdr.ah.e_filesz));
		s->st_dev   = (cvt_from_hex(ai->cpio_hdr.ah.e_maj,
						sizeof(ai->cpio_hdr.ah.e_maj)) << 8) |
					  (cvt_from_hex(ai->cpio_hdr.ah.e_min,
						sizeof(ai->cpio_hdr.ah.e_min))     );

		set_rdev(s, ((int)cvt_from_hex(ai->cpio_hdr.ah.e_rmaj,
						sizeof(ai->cpio_hdr.ah.e_rmaj)) << 8) |
					((int)cvt_from_hex(ai->cpio_hdr.ah.e_rmin,
						sizeof(ai->cpio_hdr.ah.e_rmin))     ));
		break;
	}
}

/* skip file in cpio file */

int cpio_skip_file (void)
{
	long size;

	switch (ai->a_hdr_type)
	{
	case cpio_hdr_type_bin:
		memcpy(&size, ai->cpio_hdr.bh.h_filesz,
			sizeof(ai->cpio_hdr.bh.h_filesz));
		if (size & 1)
			size++;
		break;

	case cpio_hdr_type_chr:
		size = cvt_from_oct(ai->cpio_hdr.ch.c_filesz,
			sizeof(ai->cpio_hdr.ch.c_filesz));
		break;

	case cpio_hdr_type_asc:
		size = cvt_from_hex(ai->cpio_hdr.ah.e_filesz,
			sizeof(ai->cpio_hdr.ah.e_filesz));
		break;

	case cpio_hdr_type_crc:
		size = cvt_from_hex(ai->cpio_hdr.ah.e_filesz,
			sizeof(ai->cpio_hdr.ah.e_filesz));
		if (size & 3)
			size = (size + 3) & ~3;
		break;
	}

	return (cpio_skip_bytes(size));
}

int cpio_position (const char *n, int v, off_t p)
{
	int k;
	int c;

	if (v != ai->a_volno || strcmp(n, ai->a_name))
	{
		if (cpio_ask_for_volno(n, v))
			return (-1);
	}

	if (ai->a_dev_type == A_DEV_NOSEEK)
	{
		/* tape or unknown device (unseekable) */

		if (p < (ai->cpio_buf_addr + ai->cpio_buf_pos))
		{
			cpio_close(FALSE);
			ai->a_init = FALSE;
			c = cpio_open();
			ai->a_init = TRUE;
			if (c)
				return (-1);
		}
	}
	k = p - (ai->cpio_buf_addr + ai->cpio_buf_pos);
	if (cpio_skip_bytes(k))
		return (-1);

	return (0);
}

char *cpio_filename (void)
{
	char *p;

	switch (ai->a_hdr_type)
	{
	case cpio_hdr_type_bin:
		p = ai->cpio_hdr.bh.h_name;
		break;

	case cpio_hdr_type_chr:
		p = ai->cpio_hdr.ch.c_name;
		break;

	case cpio_hdr_type_asc:
	case cpio_hdr_type_crc:
		p = ai->cpio_hdr.ah.e_name;
		break;

	default:
		p = 0;
		break;
	}
	return (p);
}
