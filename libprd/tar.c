/*------------------------------------------------------------------------
 * "tar" file routines
 */
#include "libprd.h"

/*
 *	low level routines for reading/writing tar files
 *
 *	entry points:
 *
 *		tar_open()
 *		tar_close(e)
 *		tar_read_rec()
 *		tar_write_rec()
 *		tar_wr_hdr(f, r)
 *		tar_wr_part_hdr(f, s, r)
 *		tar_wr_link_hdr(f, l, r)
 *		tar_wr_sym_hdr(f, l, r)
 *		tar_wr_tlr()
 *		tar_get_stbuf(s)
 *		tar_skip_file()
 *		tar_skip_rec(n)
 *		tar_is_valid_hdr()
 *		tar_position(n, v, p)
 *		tar_ask_for_volno(n, v)
 *		tar_filename()
 *		tar_linkname()
 *		tar_num_exts()
 *		tar_get_extno()
 *		tar_get_file_ext_size()
 */

static long tar_calc_cksum (union tblock *h)
{
	long chksum;
	int i;

	chksum = 256;
	for (i=0; i<148; i++)
		chksum += h->dummy[i];
	for (i=156; i<sizeof(union tblock); i++)
		chksum += h->dummy[i];
	return (chksum);
}

static int tar_make_hdr (FBLK *f, char *rel_name)
{
	char path[MAX_PATHLEN];
	union tblock *h;
	long chksum;
	char *n;
	int c;
	char n_buf[16];

	memset(ai->a_recp, 0, ai->a_blksize);
	h = (union tblock *)ai->a_recp;

	n = get_name(f, rel_name, path);
	if ((int)strlen(n) > TNAMLEN-1)
	{
		c = errmsg(ER_NAMTL, "", ERR_ANY);
		return (c);
	}
	strcpy(h->obuf.name, n);
	cvt_to_oct(f->stbuf.st_mode&0777, h->obuf.mode,  sizeof(h->obuf.mode), 1);
	cvt_to_oct(f->stbuf.st_uid,       h->obuf.uid,   sizeof(h->obuf.uid), 1);
	cvt_to_oct(f->stbuf.st_gid,       h->obuf.gid,   sizeof(h->obuf.gid), 1);
	cvt_to_oct(f->stbuf.st_size,      h->obuf.size,  sizeof(h->obuf.size), 2);
	cvt_to_oct(f->stbuf.st_mtime,     h->obuf.mtime, sizeof(h->obuf.mtime), 2);
	switch (ai->a_hdr_type)
	{
	case tar_hdr_old:
		break;

	case tar_hdr_ustar:
		h->nbuf.linkflag = T_TYPE_REG;
		strcpy(h->nbuf.magic, TAR_MAGIC);
		strcpy(h->nbuf.version, "00");
		strcpy(h->nbuf.uname, get_owner(f->stbuf.st_uid, n_buf));
		strcpy(h->nbuf.gname, get_group(f->stbuf.st_gid, n_buf));
		break;
	}
	chksum = tar_calc_cksum(h);
	cvt_to_oct(chksum, h->obuf.chksum, sizeof(h->obuf.chksum), 3);
	return (0);
}

static int tar_read_blk (void)
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

static int tar_write_blk (void)
{
	int c;

	c = os_write(ai->a_fd, ai->a_buffer, ai->a_rec_count*ai->a_blksize);
	ai->a_rec_count = 0;
	ai->a_recp = ai->a_buffer;
	if (c == -1)
	{
		ai->a_ioerr = TRUE;
		c = errsys(ER_IOERR);
		return (c);
	}
	return (0);
}

static int tar_wr_tlr (void)
{
	int c;

	/* set SYNC flag on write to insure blocks get written */

	memset(ai->a_recp, 0, ai->a_blksize);
	c = tar_write_rec();
	if (c == -1)
		return (-1);

	memset(ai->a_recp, 0, ai->a_blksize);
	c = tar_write_rec();
	if (c == -1)
		return (-1);

	while (ai->a_rec_count)
	{
		memset(ai->a_recp, 0, ai->a_blksize);
		c = tar_write_rec();
		if (c == -1)
			return (-1);
	}

	return (0);
}

static int tar_skip_rec (int n)
{
	int i;
	int c;

	if (n == 0)
		return (0);

	if (ai->a_dev_type == A_DEV_NOSEEK)
	{
		/* tape device or unknown device */

		if (n < 0)
		{
			n = ai->a_blk_count + n;
			tar_close(FALSE);
			ai->a_init = FALSE;
			c = tar_open();
			ai->a_init = TRUE;
			if (c)
				return (-1);
		}

		for (i=0; i<n; i++)
			if (tar_read_rec() < 0)
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
				c = tar_read_blk();
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

int tar_open (void)
{
	long s;
	int c;
	union tblock *h;
	int open_flags;

	ai->a_hdr_type = opt(tar_hdr_fmt);
	ai->a_ioerr = FALSE;
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
	 *	If opened read/write (instead of read-only or write-only),
	 *	then position to end of file (at zero header)
	 */

	if (ai->a_init && ai->a_open_flags == O_RDWR)
	{
		while (TRUE)
		{
			tar_read_rec();
			h = (union tblock *)ai->a_recp;
			if (!*h->obuf.name)
				break;
			if (!tar_is_valid_hdr())
			{
				ai->a_ioerr = 1;
				tar_close(TRUE);
				return (-1);
			}
			if (tar_skip_file())
			{
				ai->a_ioerr = 1;
				tar_close(TRUE);
				return (-1);
			}
		}
		s = lseek(ai->a_fd, -ai->a_blksize*ai->a_recs_read, SEEK_CUR);
		if (s == -1)
		{
			c = errsys(ER_IOERR);
			ai->a_ioerr = 1;
			tar_close(TRUE);
			return (c);
		}
	}
	else
	{
		ai->a_recp = ai->a_buffer;
		ai->a_blk_count = 0;
		ai->a_rec_count = 0;
		ai->a_recs_read = 0;

		os_set_sync_mode(ai->a_fd);
	}
	return (0);
}

/* ARGSUSED */
int tar_close (int eject)
{
	if (!ai->a_ioerr && ai->a_open_flags != O_RDONLY)
	{
		tar_wr_tlr();
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

int tar_read_rec (void)
{
	int c;

	ai->a_rec_count++;
	ai->a_recp += ai->a_blksize;
	if (ai->a_rec_count >= ai->a_recs_read)
	{
		c = tar_read_blk();
		if (c <= 0)
		{
			ai->a_recs_read = 0;
			return (c);
		}
		ai->a_recs_read = c/ai->a_blksize;
	}
	ai->a_blk_count++;
	return (ai->a_recs_read);
}

int tar_write_rec (void)
{
	int c;

	c = 0;
	ai->a_blk_count++;
	ai->a_rec_count++;
	ai->a_recp += ai->a_blksize;
	if (ai->a_rec_count == ai->a_blkfactor)
		c = tar_write_blk();
	return (c);
}

int tar_wr_hdr (FBLK *f, char *rel_name)
{
	int c;

	c = tar_make_hdr(f, rel_name);
	if (c)
		return (c);
	return (tar_write_rec());
}

int tar_wr_part_hdr (FBLK *f, char *rel_name, long s, int n, int t)
{
	long chksum;
	union tblock *h;

	if (tar_make_hdr(f, rel_name))
		return (-1);
	h = (union tblock *)ai->a_recp;
	switch (ai->a_hdr_type)
	{
	case tar_hdr_old:
		cvt_to_oct(s, h->obuf.size, sizeof(h->obuf.size), 2);
		cvt_to_oct(n, h->obuf.extno, sizeof(h->obuf.extno), 5);
		cvt_to_oct(t, h->obuf.extotal, sizeof(h->obuf.extotal), 5);
		cvt_to_oct(f->stbuf.st_size, h->obuf.efsize,
			sizeof(h->obuf.efsize), 2);
		break;

	case tar_hdr_ustar:
		cvt_to_oct(s, h->obuf.size, sizeof(h->obuf.size), 2);
		*h->nbuf.extno = n;
		*h->nbuf.extotal = t;
		cvt_to_oct(f->stbuf.st_size,  h->nbuf.totsize,
			sizeof(h->nbuf.totsize), 2);
		break;
	}
	chksum = tar_calc_cksum(h);
	cvt_to_oct(chksum, h->obuf.chksum, sizeof(h->obuf.chksum), 3);
	return (tar_write_rec());
}

int tar_wr_link_hdr (FBLK *f, FBLK *lf, char *rel_name)
{
	char path[MAX_PATHLEN];
	union tblock *h;
	long chksum;
	char *n;
	int c;

	if (tar_make_hdr(f, rel_name))
		return (-1);
	h = (union tblock *)ai->a_recp;
	h->obuf.linkflag = T_TYPE_LNK;
	n = get_name(lf, rel_name, path);
	if ((int)strlen(n) > TNAMLEN-1)
	{
		c = errmsg(ER_NAMTL, "", ERR_ANY);
		return (c);
	}
	strcpy(h->obuf.linkname, n);
	chksum = tar_calc_cksum(h);
	cvt_to_oct(chksum, h->obuf.chksum, sizeof(h->obuf.chksum), 3);
	return (tar_write_rec());
}

int tar_wr_sym_hdr (FBLK *f, char *l, char *rel_name)
{
	union tblock *h;
	long chksum;
	int c;

	if (tar_make_hdr(f, rel_name))
		return (-1);
	h = (union tblock *)ai->a_recp;
	h->obuf.linkflag = T_TYPE_SYM;
	if ((int)strlen(l) > TNAMLEN-1)
	{
		c = errmsg(ER_NAMTL, "", ERR_ANY);
		return (c);
	}
	strcpy(h->obuf.linkname, l);
	chksum = tar_calc_cksum(h);
	cvt_to_oct(chksum, h->obuf.chksum, sizeof(h->obuf.chksum), 3);
	return (tar_write_rec());
}

/* convert tar header to phony stbuf */

void tar_get_stbuf (struct stat *s)
{
	union tblock *h;

	h = (union tblock *)ai->a_recp;
	s->st_dev = 0;
	s->st_ino = 0;
	s->st_mode = (mode_t)cvt_from_oct(h->obuf.mode, sizeof(h->obuf.mode));
	switch (ai->a_hdr_type)
	{
	case tar_hdr_old:
		if (fn_is_path_a_dirname(h->obuf.name))
			s->st_mode |= S_IFDIR;
		else
			s->st_mode |= S_IFREG;
		switch (h->obuf.linkflag)
		{
		case T_TYPE_LNK:
			s->st_nlink = 2;
			break;
		case T_TYPE_SYM:
			s->st_nlink = 1;
			s->st_mode &= ~S_IFMT;
			s->st_mode |= S_IFLNK;
			break;
		default:
			s->st_nlink = 1;
			break;
		}
		break;

	case tar_hdr_ustar:
		switch (h->nbuf.linkflag)
		{
		case T_TYPE_ARG:
		case T_TYPE_REG:
			s->st_mode |= S_IFREG;
			s->st_nlink = 1;
			break;

		case T_TYPE_LNK:
			s->st_mode |= S_IFREG;
			s->st_nlink = 2;
			break;

		case T_TYPE_SYM:
			s->st_mode |= S_IFLNK;
			s->st_nlink = 1;
			break;

		case T_TYPE_CHR:
			s->st_mode |= S_IFCHR;
			s->st_nlink = 1;
			break;

		case T_TYPE_BLK:
			s->st_mode |= S_IFBLK;
			s->st_nlink = 1;
			break;

		case T_TYPE_DIR:
			s->st_mode |= S_IFDIR;
			s->st_nlink = 1;
			break;

		case T_TYPE_FFO:
			s->st_mode |= S_IFIFO;
			s->st_nlink = 1;
			break;
		}
		break;
	}
	s->st_uid = (uid_t)cvt_from_oct(h->obuf.uid, sizeof(h->obuf.uid));
	s->st_gid = (gid_t)cvt_from_oct(h->obuf.gid, sizeof(h->obuf.gid));
	set_rdev(s, 0);
	s->st_mtime = (time_t)cvt_from_oct(h->obuf.mtime, sizeof(h->obuf.mtime));
	s->st_atime = s->st_mtime;
	s->st_ctime = s->st_mtime;
	switch (ai->a_hdr_type)
	{
	case tar_hdr_old:
		if (*h->obuf.extno)
		{
			s->st_size = cvt_from_oct(h->obuf.efsize,
				sizeof(h->obuf.efsize));
		}
		else
		{
			s->st_size = cvt_from_oct(h->obuf.size,
				sizeof(h->obuf.size));
		}
		break;

	case tar_hdr_ustar:
		if (*h->nbuf.extno)
		{
			s->st_size = cvt_from_oct(h->nbuf.totsize,
				sizeof(h->nbuf.totsize));
		}
		else
		{
			s->st_size = cvt_from_oct(h->nbuf.size,
				sizeof(h->nbuf.size));
		}
		break;
	}
}

/* skip file in tar file */

int tar_skip_file (void)
{
	union tblock *h;
	int n;
	off_t size = 0;
	int vol_cnt;
	int v;

	h = (union tblock *)ai->a_recp;
	if (h->obuf.linkflag == T_TYPE_ARG || h->obuf.linkflag == T_TYPE_REG)
	{
		switch (ai->a_hdr_type)
		{
		case tar_hdr_old:
			if (*h->obuf.extno)
			{
				vol_cnt = cvt_from_oct(h->obuf.extotal,
					sizeof(h->obuf.extotal));
				ai->a_volno += vol_cnt-1;
				while (TRUE)
				{
					if (tar_ask_for_volno(ai->a_name, ai->a_volno))
						return (-1);
					if (tar_read_rec() < 0)
						return (-1);
					h = (union tblock *)ai->a_recp;
					v = cvt_from_oct(h->obuf.extno, sizeof(h->obuf.extno));
					if (v == vol_cnt)
						break;
					if (errmsg(ER_WVOL, "", ERR_ANY) < 0)
						return (-1);
				}
			}
			size = cvt_from_oct(h->obuf.size, sizeof(h->obuf.size));
			break;

		case tar_hdr_ustar:
			if (*h->nbuf.extno)
			{
				vol_cnt = *h->nbuf.extotal;
				ai->a_volno += vol_cnt-1;
				while (TRUE)
				{
					if (tar_ask_for_volno(ai->a_name, ai->a_volno))
						return (-1);
					if (tar_read_rec() < 0)
						return (-1);
					h = (union tblock *)ai->a_recp;
					v = *h->nbuf.extno;
					if (v == vol_cnt)
						break;
					if (errmsg(ER_WVOL, "", ERR_ANY) < 0)
						return (-1);
				}
			}
			size = cvt_from_oct(h->nbuf.size, sizeof(h->nbuf.size));
			break;
		}
		n = (size + ai->a_blksize - 1) / ai->a_blksize;
		return (tar_skip_rec(n));
	}
	return (0);
}

/* check for valid tar header */

int tar_is_hdr (union tblock *h)
{
	long chksum;
	long c;

	chksum = cvt_from_oct(h->obuf.chksum, sizeof(h->obuf.chksum));
	c = tar_calc_cksum(h);
	if (c == chksum)
	{
		if (strncmp(TAR_MAGIC, h->nbuf.magic, strlen(TAR_MAGIC)) == 0)
			ai->a_hdr_type = tar_hdr_ustar;
		else
			ai->a_hdr_type = tar_hdr_old;
		return (TRUE);
	}
	return (FALSE);
}

int tar_is_valid_hdr (void)
{
	union tblock *h;

	h = (union tblock *)ai->a_recp;
	if (tar_is_hdr(h))
		return (TRUE);
	errmsg(ER_ITC, "", ERR_ANY);
	return (FALSE);
}

int tar_position (const char *n, int v, off_t p)
{
	int k;
	int c;

	if (v != ai->a_volno || strcmp(n, ai->a_name))
	{
		if (tar_ask_for_volno(n, v))
			return (-1);
	}

	if (ai->a_dev_type == A_DEV_NOSEEK)
	{
		/* tape or unknown device (unseekable) */

		if (p < ai->a_blk_count)
		{
			tar_close(FALSE);
			ai->a_init = FALSE;
			c = tar_open();
			ai->a_init = TRUE;
			if (c)
				return (-1);
		}
	}
	k = p - ai->a_blk_count;
	if (tar_skip_rec(k))
		return (-1);

	/* read file header */

	if (tar_read_rec() < 0)
		return (-1);
	if (!tar_is_valid_hdr())
		return (-1);

	return (0);
}

int tar_ask_for_volno (const char *n, int v)
{
	int c;

	if (ai->a_fd > 0)
		tar_close(TRUE);
	strcpy(ai->a_name, n);
	ai->a_volno = v;
	if (ai->a_dev_type != A_DEV_FILE)
	{
		char buf[128];
		char dbuf[12];

		strcpy(buf, msgs(m_tar_askvol));
		strcat(buf, itos(dbuf, ai->a_volno));
		strcat(buf, msgs(m_tar_of));
		strcat(buf, ai->a_name);
		c = anykey_msg(buf);
		if (c < 0)
			return (-1);
	}
	ai->a_init = FALSE;
	c = tar_open();
	ai->a_init = TRUE;
	if (c)
		return (-1);
	return (0);
}

char *tar_filename (void)
{
	union tblock *h;

	h = (union tblock *)ai->a_recp;
	return (h->obuf.name);
}

char *tar_linkname (void)
{
	union tblock *h;

	h = (union tblock *)ai->a_recp;
	return (h->obuf.linkname);
}

int tar_num_exts (void)
{
	union tblock *h;
	int v;

	h = (union tblock *)ai->a_recp;
	v = 1;
	switch (ai->a_hdr_type)
	{
	case tar_hdr_old:
		if (*h->obuf.extno)
			v = cvt_from_oct(h->obuf.extotal, sizeof(h->obuf.extotal));
		break;

	case tar_hdr_ustar:
		if (*h->nbuf.extno)
			v = *h->nbuf.extotal;
		break;
	}
	return (v);
}

int tar_get_extno (void)
{
	union tblock *h;
	int v;

	h = (union tblock *)ai->a_recp;
	v = 1;
	switch (ai->a_hdr_type)
	{
	case tar_hdr_old:
		if (*h->obuf.extno)
			v = cvt_from_oct(h->obuf.extno, sizeof(h->obuf.extno));
		break;

	case tar_hdr_ustar:
		if (*h->nbuf.extno)
			v = *h->nbuf.extno;
		break;
	}
	return (v);
}

long tar_get_file_ext_size (void)
{
	union tblock *h;
	long s;

	h = (union tblock *)ai->a_recp;
	s = cvt_from_oct(h->obuf.size, sizeof(h->obuf.size));
	return (s);
}
