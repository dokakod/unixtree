/*------------------------------------------------------------------------
 * "bar" file processing
 */
#include "libprd.h"

/*
 *	low level routines for reading/writing bar files
 *
 *	entry points:
 *
 *		bar_open()
 *		bar_close(e)
 *		bar_read_rec()
 *		bar_write_rec()
 *		bar_calc_chksum(h)
 *		bar_calc_vol_chksum(h)
 *		bar_wr_hdr(f, r)
 *		bar_wr_part_hdr(f, s, r)
 *		bar_wr_link_hdr(f, l, r)
 *		bar_wr_sym_hdr(f, l, r)
 *		bar_wr_tlr()
 *		bar_get_stbuf(s)
 *		bar_skip_file()
 *		bar_skip_rec(n)
 *		bar_is_valid_hdr()
 *		bar_position(n, v, p)
 *		bar_ask_for_volno(n, v)
 *		bar_filename()
 *		bar_linkname()
 */

static long bar_calc_cksum (union bblock *h)
{
	long chksum;
	int i;

	chksum = 256;
	for (i=0; i<48; i++)
		chksum += h->dummy[i];
	for (i=56; i<sizeof(union bblock); i++)
		chksum += h->dummy[i];
	return (chksum);
}

static long bar_calc_vol_cksum (union bblock *h)
{
	long chksum;
	int i;

	chksum = 256;
	for (i=0; i<48; i++)
		chksum += h->dummy[i];
	for (i=56; i<sizeof(union bblock); i++)
		chksum += h->dummy[i];
	return (chksum);
}

static int bar_is_valid_vol_hdr (void)
{
	union bblock *h;

	h = (union bblock *)ai->a_recp;
	if (bar_is_hdr(h))
		return (TRUE);
	errmsg(ER_NOTBAR, "", ERR_ANY);
	return (FALSE);
}

static int bar_make_hdr (FBLK *f, char *rel_name)
{
	char path[MAX_PATHLEN];
	union bblock *h;
	long chksum;
	char *n;
	long m;
	int c;
	long size;

	memset(ai->a_recp, 0, ai->a_blksize);
	h = (union bblock *)ai->a_recp;

	n = get_name(f, rel_name, path);
	if ((int)strlen(n) > BNAMLEN-1)
	{
		c = errmsg(ER_NAMTL, "", ERR_ANY);
		return (c);
	}
	strcpy(h->dbuf.start_of_name, n);
	size = 0;
	m = f->stbuf.st_mode;
	if (S_ISREG(f->stbuf.st_mode))
	{
		m &= 0777;
		size = f->stbuf.st_size;
	}
	cvt_to_oct(m, h->dbuf.mode, sizeof(h->dbuf.mode), 4);
	cvt_to_oct(f->stbuf.st_uid, h->dbuf.uid, sizeof(h->dbuf.uid), 1);
	cvt_to_oct(f->stbuf.st_gid, h->dbuf.gid, sizeof(h->dbuf.gid), 1);
	cvt_to_oct(size, h->dbuf.size, sizeof(h->dbuf.size), 2);
	cvt_to_oct(f->stbuf.st_mtime, h->dbuf.mtime, sizeof(h->dbuf.mtime), 2);
	cvt_to_oct(get_rdev(&f->stbuf), h->dbuf.rdev, sizeof(h->dbuf.rdev), 5);
	switch (f->stbuf.st_mode & S_IFMT)
	{
	case S_IFDIR:
		h->dbuf.linkflag = B_TYPE_DIR;
		fn_terminate_dirname(h->dbuf.start_of_name);
		break;

	case S_IFREG:
		h->dbuf.linkflag = B_TYPE_REG;
		break;

	case S_IFLNK:
		h->dbuf.linkflag = B_TYPE_SYM;
		break;

	default:
		h->dbuf.linkflag = B_TYPE_SPC;
		break;
	}
	chksum = bar_calc_cksum(h);
	cvt_to_oct(chksum, h->dbuf.chksum, sizeof(h->dbuf.chksum), 3);
	return (0);
}

static void bar_make_vol_hdr (long size)
{
	union bblock *h;
	long chksum;
	struct tm *t;

	h = (union bblock *)ai->a_recp;
	memset(h->dummy, 0, sizeof(h->dummy));

	cvt_to_dec(gbl(pgm_euid), h->dbuf.uid, sizeof(h->dbuf.uid));
	cvt_to_dec(gbl(pgm_egid), h->dbuf.gid, sizeof(h->dbuf.gid));
	cvt_to_oct(size, h->dbuf.size, sizeof(h->dbuf.size), 5);
	strcpy(h->dbuf.bar_magic, BAR_MAGIC);
	cvt_to_dec(ai->a_volno, h->dbuf.volume_num, sizeof(h->dbuf.volume_num));
	h->dbuf.compressed = (ai->a_compressed? '1' : '0');

	if (ai->a_volno == 1)
		ai->a_time = time((time_t *)NULL);
	t = localtime(&ai->a_time);
	t->tm_year %= 100;
	t->tm_mon  += 1;

	h->dbuf.date[0] = t->tm_year / 10 + '0';
	h->dbuf.date[1] = t->tm_year % 10 + '0';
	h->dbuf.date[2] = t->tm_mon  / 10 + '0';
	h->dbuf.date[3] = t->tm_mon  % 10 + '0';
	h->dbuf.date[4] = t->tm_mday / 10 + '0';
	h->dbuf.date[5] = t->tm_mday % 10 + '0';
	h->dbuf.date[6] = t->tm_hour / 10 + '0';
	h->dbuf.date[7] = t->tm_hour % 10 + '0';
	h->dbuf.date[8] = t->tm_min  / 10 + '0';
	h->dbuf.date[9] = t->tm_min  % 10 + '0';

	chksum = bar_calc_vol_cksum(h);
	cvt_to_oct(chksum, h->dbuf.chksum, sizeof(h->dbuf.chksum), 3);
}

static int bar_read_blk (void)
{
	int c;

	c = os_read(ai->a_fd, ai->a_buffer, ai->a_blkfactor*ai->a_blksize);
	if (c == -1)
	{
		errsys(ER_IOERR);
		return (-1);
	}
	ai->a_rec_count = 0;
	ai->a_recp = ai->a_buffer;
	return (c);
}

static int bar_write_blk (void)
{
	int c;

	c = os_write(ai->a_fd, ai->a_buffer, ai->a_rec_count*ai->a_blksize);
	ai->a_rec_count = 0;
	ai->a_recp = ai->a_buffer;
	if (c == -1)
	{
		c = errsys(ER_IOERR);
		return (c);
	}
	return (0);
}

static int bar_skip_rec (int n)
{
	int i;
	int c;

	if (n == 0)
		return (0);

	c = ai->a_blk_count + n;
	if (ai->a_volsize && c >= ai->a_volsize)
	{
		c -= ai->a_volsize;
		i = ai->a_volno + 1 + (c / (ai->a_volsize-1));
		n -= (ai->a_volsize - ai->a_blk_count);
		n -= (i-ai->a_volno-1) * (ai->a_volsize-1);
		if (bar_ask_for_volno(ai->a_name, i))
			return (-1);
	}

	if (ai->a_dev_type == A_DEV_NOSEEK)
	{
		/* tape device or unknown device */

		if (n < 0)
		{
			n = ai->a_blk_count + n;
			bar_close(FALSE);
			ai->a_init = FALSE;
			c = bar_open();
			ai->a_init = TRUE;
			if (c)
				return (-1);
		}

		for (i=0; i<n; i++)
			if (bar_read_rec() < 0)
				return (-1);
	}
	else
	{
		/* file or non-tape device (seekable) */

		if (n < 0)
		{
			n = ai->a_blk_count + n;
			ai->a_blk_count = 0;
			ai->a_rec_count = 0;
			ai->a_recp = ai->a_buffer;
			ai->a_recs_read = 0;
		}

		if (ai->a_blk_count && ai->a_rec_count + n < ai->a_blkfactor)
		{
			ai->a_rec_count += n;
			ai->a_blk_count += n;
			ai->a_recp += n * ai->a_blksize;
		}
		else
		{
			ai->a_blk_count += n;
			i = (ai->a_blk_count / ai->a_blkfactor) * ai->a_blkfactor;
			lseek(ai->a_fd, ((off_t)i) * ai->a_blksize, SEEK_SET);
			if (ai->a_blk_count > i)
			{
				c = bar_read_blk();
				if (c < 0)
					return (-1);
				ai->a_recs_read = c / ai->a_blksize;
				ai->a_rec_count = ai->a_blk_count - i - 1;
				ai->a_recp += ai->a_rec_count * ai->a_blksize;
			}
			else
			{
				ai->a_rec_count = ai->a_recs_read;
			}
		}
	}
	return (0);
}

int bar_open (void)
{
	long s;
	int c;
	int v;
	union bblock *h;
	int open_flags;

	ai->a_blk_count = 0;
	ai->a_rec_count = 0;
	ai->a_recs_read = 0;
	ai->a_recp = ai->a_buffer;

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
	 *	If opened read/write or read-only,
	 *	read in volume header.
	 */

	if (ai->a_open_flags == O_RDWR || ai->a_open_flags == O_RDONLY)
	{
		c = bar_read_rec();
		if (c < 0)
		{
			bar_close(TRUE);
			return (-1);
		}
		h = (union bblock *)ai->a_recp;

		/* check bar header */

		if (!bar_is_valid_vol_hdr())
		{
			bar_close(TRUE);
			return (-1);
		}

		v = cvt_from_dec(h->dbuf.volume_num, sizeof(h->dbuf.volume_num));
		if (v != ai->a_volno)
		{
			errmsg(ER_WVOL, "", ERR_ANY);
			bar_close(TRUE);
			return (-1);
		}
		ai->a_compressed = FALSE;
		if (h->dbuf.compressed && h->dbuf.compressed != '0')
			ai->a_compressed = TRUE;
	}

	/*
	 *	If opened read/write (instead of read-only or write-only),
	 *	then position to end of file (at zero header)
	 */

	if (ai->a_init && ai->a_open_flags == O_RDWR)
	{
		while (TRUE)
		{
			c = bar_read_rec();
			if (c < 0)
			{
				bar_close(TRUE);
				return (-1);
			}

			h = (union bblock *)ai->a_recp;
			if (!*h->dbuf.start_of_name)
				break;

			if (!bar_is_valid_hdr())
			{
				bar_close(TRUE);
				return (-1);
			}

			if (bar_skip_file())
			{
				bar_close(TRUE);
				return (-1);
			}
		}
		s = lseek(ai->a_fd, -ai->a_blksize*ai->a_recs_read, SEEK_CUR);
		if (s == -1)
		{
			errsys(ER_IOERR);
			bar_close(TRUE);
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
int bar_close (int eject)
{
	if (eject)
	{
		if (ai->a_dev_type == A_DEV_SEEK)
			os_floppy_eject(ai->a_fd);
	}

	os_close(ai->a_fd);
	ai->a_fd = 0;

	return (0);
}

int bar_read_rec (void)
{
	int c;

	ai->a_rec_count++;
	ai->a_recp += ai->a_blksize;
	if (ai->a_rec_count >= ai->a_recs_read)
	{
		if (ai->a_volsize && ai->a_blk_count >= ai->a_volsize)
		{
			c = bar_ask_for_volno(ai->a_name, ai->a_volno+1);
			if (c < 0)
				return (c);
			return (bar_read_rec());
		}
		else
		{
			c = bar_read_blk();
			if (c <= 0)
			{
				ai->a_recs_read = 0;
				return (c);
			}
			ai->a_recs_read = c/ai->a_blksize;
		}
	}
	ai->a_blk_count++;
	return (ai->a_recs_read);
}

int bar_write_rec (void)
{
	int c;

	c = 0;
	ai->a_blk_count++;
	ai->a_rec_count++;
	ai->a_recp += ai->a_blksize;
	if (ai->a_rec_count == ai->a_blkfactor)
		c = bar_write_blk();
	return (c);
}

int bar_wr_vol_hdr (long size)
{
	int c;

	bar_make_vol_hdr(size);
	c = bar_write_rec();
	return (c);
}

int bar_wr_hdr (FBLK *f, char *rel_name)
{
	int c;

	c = bar_make_hdr(f, rel_name);
	if (c)
		return (c);
	return (bar_write_rec());
}

int bar_wr_link_hdr (FBLK *f, FBLK *lf, char *rel_name)
{
	char path[MAX_PATHLEN];
	union bblock *h;
	long chksum;
	char *p;
	char *n;
	int c;

	if (bar_make_hdr(f, rel_name))
		return (-1);
	h = (union bblock *)ai->a_recp;
	h->dbuf.linkflag = B_TYPE_LNK;
	p = bar_linkname();
	n = get_name(lf, rel_name, path);
	if ((int)strlen(n) > BNAMLEN-1)
	{
		c = errmsg(ER_NAMTL, "", ERR_ANY);
		return (c);
	}
	strcpy(p, n);
	chksum = bar_calc_cksum(h);
	cvt_to_oct(chksum, h->dbuf.chksum, sizeof(h->dbuf.chksum), 3);
	return (bar_write_rec());
}

int bar_wr_sym_hdr (FBLK *f, char *l, char *rel_name)
{
	union bblock *h;
	long chksum;
	char *p;
	int c;

	bar_make_hdr(f, rel_name);
	h = (union bblock *)ai->a_recp;
	h->dbuf.linkflag = B_TYPE_SYM;
	p = bar_linkname();
	if ((int)strlen(l) > BNAMLEN-1)
	{
		c = errmsg(ER_NAMTL, "", ERR_ANY);
		return (c);
	}
	strcpy(p, l);
	chksum = bar_calc_cksum(h);
	cvt_to_oct(chksum, h->dbuf.chksum, sizeof(h->dbuf.chksum), 3);
	return (bar_write_rec());
}

int bar_wr_tlr (void)
{
	int c;

	/* set SYNC flag on write to insure blocks get written */

	memset(ai->a_recp, 0, ai->a_blksize);
	c = bar_write_rec();
	if (c == -1)
		return (-1);

	memset(ai->a_recp, 0, ai->a_blksize);
	c = bar_write_rec();
	if (c == -1)
		return (-1);

	while (ai->a_rec_count)
	{
		memset(ai->a_recp, 0, ai->a_blksize);
		c = bar_write_rec();
		if (c == -1)
			return (-1);
	}

	return (0);
}

/* convert bar header to phony stbuf */

void bar_get_stbuf (struct stat *s)
{
	union bblock *h;

	h = (union bblock *)ai->a_recp;
	s->st_dev = 0;
	s->st_ino = 0;
	s->st_mode = (mode_t)cvt_from_oct(h->dbuf.mode, sizeof(h->dbuf.mode));
	if (fn_is_path_a_dirname(h->dbuf.start_of_name) || S_ISDIR(s->st_mode))
	{
		s->st_mode |= S_IFDIR;
	}
	else
	{
		switch (h->dbuf.linkflag)
		{
		case B_TYPE_DIR:
			s->st_mode |= S_IFDIR;
			s->st_nlink = 1;
			break;

		case B_TYPE_REG:
			s->st_mode |= S_IFREG;
			s->st_nlink = 1;
			break;

		case B_TYPE_LNK:
			s->st_mode |= S_IFREG;
			s->st_nlink = 2;
			break;

		case B_TYPE_SYM:
			s->st_mode &= ~S_IFMT;
			s->st_mode |= S_IFLNK;
			s->st_nlink = 1;
			break;

		default:
			s->st_nlink = 1;
			break;
		}
	}
	s->st_uid   = (uid_t)cvt_from_oct(h->dbuf.uid, sizeof(h->dbuf.uid));
	s->st_gid   = (gid_t)cvt_from_oct(h->dbuf.gid, sizeof(h->dbuf.gid));
	s->st_size  = (off_t)cvt_from_oct(h->dbuf.size, sizeof(h->dbuf.size));
	s->st_mtime = (time_t)cvt_from_oct(h->dbuf.mtime, sizeof(h->dbuf.mtime));
	s->st_atime = s->st_mtime;
	s->st_ctime = s->st_mtime;
	set_rdev(s, (int)cvt_from_oct(h->dbuf.rdev, sizeof(h->dbuf.rdev)));
}

/* skip file in bar file */

int bar_skip_file (void)
{
	union bblock *h;
	int n;
	off_t size;

	h = (union bblock *)ai->a_recp;
	if (h->dbuf.linkflag == B_TYPE_REG)
	{
		size = cvt_from_oct(h->dbuf.size, sizeof(h->dbuf.size));
		n = (size + ai->a_blksize - 1) / ai->a_blksize;
		return (bar_skip_rec(n));
	}
	return (0);
}

/* check for valid bar header */

int bar_is_valid_hdr (void)
{
	union bblock *h;
	int chksum;
	long c;

	h = (union bblock *)ai->a_recp;
	chksum = cvt_from_oct(h->dbuf.chksum, 8);
	c = bar_calc_cksum(h);
	if (c != chksum)
	{
		errmsg(ER_ITC, "", ERR_ANY);
		return (FALSE);
	}
	return (TRUE);
}

int bar_is_hdr (union bblock *h)
{

	if (strcmp(h->dbuf.bar_magic, BAR_MAGIC))
		return (FALSE);

#if 0	/* bar vol hdr chksums are FM */
	{
		long chksum;
		long c;

		chksum = cvt_from_oct(h->dbuf.chksum, sizeof(h->dbuf.chksum));
		c = bar_calc_vol_cksum(h);
		if (c != chksum)
			return (FALSE);
	}
#endif

	return (TRUE);
}

int bar_position (const char *n, int v, off_t p)
{
	int k;
	int c;

	if (v != ai->a_volno || strcmp(n, ai->a_name))
	{
		if (bar_ask_for_volno(n, v))
			return (-1);
	}

	if (ai->a_dev_type == A_DEV_NOSEEK)
	{
		/* tape or unknown device (unseekable) */

		if (p < ai->a_blk_count)
		{
			bar_close(FALSE);
			ai->a_init = FALSE;
			c = bar_open();
			ai->a_init = TRUE;
			if (c)
				return (-1);
		}
	}
	k = p - ai->a_blk_count;
	if (bar_skip_rec(k))
		return (-1);

	/* read file header */

	if (bar_read_rec() < 0)
		return (-1);
	if (!bar_is_valid_hdr())
		return (-1);

	return (0);
}

int bar_ask_for_volno (const char *n, int v)
{
	int c;

	if (ai->a_fd > 0)
		bar_close(TRUE);
	ai->a_volno = v;
	strcpy(ai->a_name, n);
	if (ai->a_dev_type != A_DEV_FILE)
	{
		char buf[128];
		char dbuf[12];

		strcpy(buf, msgs(m_bar_askvol));
		strcat(buf, itos(dbuf, ai->a_volno));
		strcat(buf, msgs(m_bar_of));
		strcat(buf, ai->a_name);
		c = anykey_msg(buf);
		if (c < 0)
			return (-1);
	}
	ai->a_init = FALSE;
	c = bar_open();
	ai->a_init = TRUE;
	if (c)
		return (-1);
	return (0);
}

char *bar_filename (void)
{
	union bblock *h;

	h = (union bblock *)ai->a_recp;
	return (h->dbuf.start_of_name);
}

char *bar_linkname (void)
{
	union bblock *h;
	char *p;

	h = (union bblock *)ai->a_recp;
	p = h->dbuf.start_of_name;
	p += strlen(p)+1;
	return (p);
}
