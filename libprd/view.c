/*------------------------------------------------------------------------
 * View routines
 */
#include "libprd.h"

VFCB *view_init (WINDOW *win)
{
	VFCB *v;
	int i;
	unsigned char *p;

	v = (VFCB *)MALLOC(sizeof(VFCB));
	if (!v)
		return (0);

	/* these are constant */

	set_view_win(v, win);
	v->v_blksize     = V_BLKSIZE;
	v->v_tab_width   = opt(tab_width);

	/* these are variable */

	*v->v_pathname = 0;
	*v->v_dispname = 0;
	*v->v_linkname = 0;
	v->v_fd        = 0;
	switch (opt(view_fmt))
	{
	case view_fmt_auto:
		v->v_mode = V_AUTO;
		break;
	case view_fmt_ascii:
		v->v_mode = V_ASCII;
		break;
	case view_fmt_wrap:
		v->v_mode = V_WRAP;
		break;
	case view_fmt_hex:
		v->v_mode = V_HEX;
		break;
	case view_fmt_dump:
		v->v_mode = V_DUMP;
		break;
	}
	v->v_filesize  = 0;
	v->v_top_pos   = 0;
	v->v_bot_pos   = 0;
	v->v_cur_pos   = 0;
	v->v_srch_pos  = -1;
	v->v_re        = 0;
	v->v_cur_ptr   = 0;
	v->v_fnd_ptr   = 0;
	v->v_end_ptr   = 0;

	/* allocate the buffer pool */

#if 0
	v->v_num_buffers   = ((getmaxy(win) + V_BLKSIZE/V_MAX_LINEWIDTH - 1) /
						    (V_BLKSIZE/V_MAX_LINEWIDTH)) * 2;
#else
	v->v_num_buffers   = 10;
#endif
	v->v_buffers       = (BUFFER *)MALLOC(v->v_num_buffers * sizeof(BUFFER));
	p = (unsigned char *)MALLOC(v->v_num_buffers * v->v_blksize);
	for (i=0; i<v->v_num_buffers; i++)
	{
		v->v_buffers[i].b_mode = 0;
		v->v_buffers[i].b_addr = -1;
		v->v_buffers[i].b_data = p;
		p += v->v_blksize;
	}
	v->v_empty_no      = 0;

	return (v);
}

void set_view_win (VFCB *v, WINDOW *win)
{
	v->v_win         = win;
	v->v_ascii_width = getmaxx(win);
	v->v_dump_width  = (getmaxx(win) - 8) & ~3;
	v->v_hex_width   = (((getmaxx(win) - 9)*4)/17) & ~3;
}

VFCB *view_end (VFCB *v)
{
	if (v)
	{
		if (v->v_fd)
			view_command(v, V_CMD_CLOSE);
		/* note - we don't free v->v_re */
		FREE(v->v_buffers[0].b_data);
		FREE(v->v_buffers);
		FREE(v);
	}
	return (0);
}

static void view_chk_compressed (VFCB *v)
{
	char temp_name[MAX_PATHLEN];
	int type;

	v->v_mode &= ~V_EXPANDED;
	if (! opt(exp_comp_files))
		return;

	type = x_is_file_compressed(v->v_pathname);
	if (type != -1)
	{
		os_make_temp_name(temp_name, gbl(pgm_tmp), NULL);

		x_decomp(v->v_pathname, temp_name, type);

		if (*v->v_dispname == 0)
			strcpy(v->v_dispname, v->v_pathname);

		strcpy(v->v_pathname, temp_name);
		v->v_mode |= V_EXPANDED;
	}
}

static void select_mode (VFCB *v)
{
	BUFFER *b;
	int i;

	if (opt(display_mask))
		v->v_mode |=  V_MASKED;
	else
		v->v_mode &= ~V_MASKED;

	if (v->v_fd == -1)
	{
		v->v_mode &= ~V_MODE;
		v->v_mode |=  V_ASCII;
		return;
	}

	if ((v->v_mode & V_RDWR))
	{
		v->v_mode &= ~V_MODE;
		v->v_mode |=  V_HEX;
		return;
	}

	if ((v->v_mode & V_AUTO) && v->v_filesize)
	{
		/*----------------------------------------------------------------
		 * check the current buffer for NUL bytes or control
		 * chars that are not tab/cr/nl.
		 */
		v->v_mode &= ~V_MODE;
		v->v_mode |=  V_ASCII;

		b = v->v_curbuf;
		for (i=0; i<b->b_len; i++)
		{
			int c = b->b_data[i];

			if (c == 0 || (iscntrl(c) && ! isspace(c)))
			{
				v->v_mode &= ~V_MODE;
				v->v_mode |=  V_HEX;
				break;
			}
		}
	}
}

static void buf_read (VFCB *v, long address, int j)
{
	BUFFER *b = v->v_buffers + j;
	int i;

	if (b->b_addr == address)
		return;

	b->b_addr = -1;

	if (v->v_fd == -1)
	{
		i = strlen(v->v_linkname);
		if (i > 0)
		{
			strncpy((char *)b->b_data, v->v_linkname, i);
		}
	}
	else
	{
		if (lseek(v->v_fd, address, SEEK_SET) == -1)
			return;

		i = os_read(v->v_fd, (char *)b->b_data, v->v_blksize);
	}

	if (i > 0)
	{
		b->b_addr = address;
		b->b_mode = 0;
		b->b_len  = i;
	}
}

static void buf_flush (VFCB *v, BUFFER *b)
{
	if ((v->v_mode & V_RDWR) && (b->b_mode & B_DIRTY))
	{
		if (lseek(v->v_fd, b->b_addr, SEEK_SET) >= 0)
			os_write(v->v_fd, (char *)b->b_data, b->b_len);

		b->b_mode &= ~B_DIRTY;
	}
}

void view_open (VFCB *v, int rdwr)
{
	int mode = rdwr ? O_RDWR : O_RDONLY;
	struct stat stbuf;

	v->v_fd = 0;
	view_scroll_bar_off(v);

	if (os_lstat(v->v_pathname, &stbuf))
		return;

	if (S_ISLNK(stbuf.st_mode))
	{
		int i;

		i = os_readlink(v->v_pathname, v->v_linkname,
			sizeof(v->v_linkname));
		if (i > 0)
		{
			v->v_linkname[i] = 0;
		}
		else
		{
			v->v_linkname[0] = 0;
		}
	}
	else
	{
		if (! S_ISREG(stbuf.st_mode))
			return;

		v->v_linkname[0] = 0;
	}

	view_chk_compressed(v);

	if (v->v_mode & V_SYM)
	{
		if (os_lstat(v->v_pathname, &stbuf))
			return;
	}
	else
	{
#if V_WINDOWS
		if (*v->v_linkname != 0)
		{
			if (os_stat(v->v_linkname, &stbuf))
				return;
		}
		else
#endif

		if (os_stat(v->v_pathname, &stbuf))
			return;
	}

	if (S_ISLNK(stbuf.st_mode))
	{
		v->v_fd       = -1;
		v->v_filesize = strlen(v->v_linkname);
		v->v_top_pos  = 0;
		v->v_bot_pos  = 0;
		v->v_cur_pos  = 0;
		v->v_srch_pos = -1;

		get_buffer(v);
		select_mode(v);
	}
	else
	{
		v->v_filesize = stbuf.st_size;
	}

#if V_WINDOWS
	if (*v->v_linkname)
		v->v_fd = os_open(v->v_linkname, mode, 0666);
	else
#endif
		v->v_fd = os_open(v->v_pathname, mode, 0666);

	if (v->v_fd > 0)
	{
		v->v_top_pos  = 0;
		v->v_bot_pos  = 0;
		v->v_cur_pos  = 0;
		v->v_srch_pos = -1;
		v->v_fnd_ptr  = 0;
		v->v_end_ptr  = 0;

		if (rdwr)
			v->v_mode |= V_RDWR;

		get_buffer(v);
		select_mode(v);
	}
}

void view_close (VFCB *v)
{
	int i;

	if (v->v_fd > 0)
		os_close(v->v_fd);

	if (v->v_mode & V_EXPANDED)
	{
		os_file_delete(v->v_pathname);
	}

	for (i=0; i<v->v_num_buffers; i++)
	{
		BUFFER *b = v->v_buffers + i;

		if (b->b_mode & B_DIRTY)
			buf_flush(v, b);

		b->b_mode = 0;
		b->b_addr = -1;
	}

	*v->v_linkname	= 0;
	*v->v_dispname	= 0;
	v->v_fd			= 0;
	v->v_filesize	= 0;
	v->v_top_pos	= 0;
	v->v_bot_pos	= 0;
	v->v_curbuf		= 0;
	v->v_cur_ptr	= 0;
	v->v_fnd_ptr	= 0;
	v->v_end_ptr	= 0;
	v->v_empty_no	= 0;
}

/*
 *	get_buffer - get a buffer corresponding to the cur_pos
 *	location and set curbuf & cur_ptr.
 *
 *	Note: The buffer returned will always be between 0 & n-1
 *	buffer no. and the next buffer will always be the next
 *	buffer (for searches).
 */

void get_buffer (VFCB *v)
{
	BUFFER *b = 0;
	int i, j;
	long address;

	v->v_cur_ptr = 0;
	v->v_curbuf  = 0;

	/* if past end of file, just return */

	if (v->v_cur_pos >= v->v_filesize)
		return;

	/*
	 *	look for a buffer with that address in it
	 *	and simultaneously look for an empty buffer
	 */

	address = v->v_cur_pos & (~(V_BLKSIZE-1));
	j = -1;
	for (i=0; i<v->v_num_buffers-1; i++)
	{
		b = v->v_buffers + i;

		if (address == b->b_addr)
		{
			v->v_curbuf = b;
			v->v_cur_ptr = b->b_data + (v->v_cur_pos - address);
			if (b->b_addr + v->v_blksize < v->v_filesize)
				buf_read(v, address+v->v_blksize, i+1);
			return;
		}

		if (j == -1 && b->b_addr == -1)
			j = i;
	}

	/*
	 *	If we get here, no buffer was found with the requested address.
	 *	Check if an empty buffer was found
	 */

	if (j == -1)
	{
		/*	No empty buffer was found, so choose one */

		j = v->v_empty_no++;
		if (v->v_empty_no >= v->v_num_buffers-1)
			v->v_empty_no = 0;
	}

	/* flush buffer if dirty */

	b = v->v_buffers + j;
	if (b->b_mode & B_DIRTY)
		buf_flush(v, b);

	buf_read(v, address, j);
	v->v_cur_ptr = b->b_data + (v->v_cur_pos - address);
	v->v_curbuf = b;
	if (b->b_addr + v->v_blksize < v->v_filesize)
		buf_read(v, address+v->v_blksize, j+1);
}

void down_line (VFCB *v)
{
	if (v->v_cur_ptr)
	{
		int i;
		int c;

		for (i=0; i<V_MAX_LINEWIDTH && v->v_cur_ptr; i++)
		{
			c = *(v->v_cur_ptr);
			bump_pos(v);
			if (c == '\n')
				break;
		}
	}
}

void bump_pos (VFCB *v)
{
	v->v_cur_pos++;
	v->v_cur_ptr++;

	if (v->v_cur_pos >= (v->v_curbuf->b_addr + v->v_curbuf->b_len))
		get_buffer(v);
}

static void dec_pos (VFCB *v)
{
	if (v->v_cur_pos)
	{
		v->v_cur_pos--;
		v->v_cur_ptr--;
		if (v->v_cur_pos < v->v_curbuf->b_addr)
			get_buffer(v);
	}
}

void up_line (VFCB *v)
{
	if (v->v_cur_pos)
	{
		int i;
		int c;

		dec_pos(v);
		for (i=0; i<V_MAX_LINEWIDTH; i++)
		{
			if (!v->v_cur_pos)
				return;
			dec_pos(v);
			c = *(v->v_cur_ptr);
			if (c == '\n')
				break;
		}
		bump_pos(v);
	}
}

static void down_line_view (VFCB *v)
{
	if (v->v_cur_ptr)
	{
		int i;
		int c;

		for (i=0; i<V_MAX_LINEWIDTH && v->v_cur_ptr; i++)
		{
			c = *(v->v_cur_ptr);
			bump_pos(v);
			if (!c || c == '\n')
				break;
		}
	}
}

static void up_line_view (VFCB *v)
{
	if (v->v_cur_pos)
	{
		int i;
		int c;

		dec_pos(v);
		for (i=0; i<V_MAX_LINEWIDTH; i++)
		{
			if (!v->v_cur_pos)
				return;
			dec_pos(v);
			c = *(v->v_cur_ptr);
			if (!c || c == '\n')
				break;
		}
		bump_pos(v);
	}
}

void view_next (VFCB *v)
{
	long save_top;
	long save_cur;
	unsigned char *save_ptr;
	const char *p;
	const char *e;

	if (v->v_re == 0)
		return;

	save_top = v->v_top_pos;
	save_cur = v->v_cur_pos;
	save_ptr = v->v_cur_ptr;

	v->v_cur_pos = v->v_top_pos;
	get_buffer(v);

	p = 0;
	e = 0;
	while (TRUE)
	{
		down_line_view(v);
		if (v->v_cur_ptr == 0)
		{
			do_beep();
			break;
		}

		p = ut_regexec(v->v_re, (char *)v->v_cur_ptr, &e);
		if (p)
			break;
	}

	if (p)
	{
		v->v_top_pos = v->v_cur_pos;
		if (e)
		{
			v->v_fnd_ptr = p;
			v->v_end_ptr = e;
		}
	}
	else
	{
		v->v_top_pos = save_top;
		v->v_cur_pos = save_cur;
		v->v_cur_ptr = save_ptr;
		get_buffer(v);
	}

	v_display(v);
}

void view_prev (VFCB *v)
{
	long save_top;
	long save_cur;
	unsigned char *save_ptr;
	const char *p;
	const char *e;

	if (v->v_re == 0)
		return;

	save_top = v->v_top_pos;
	save_cur = v->v_cur_pos;
	save_ptr = v->v_cur_ptr;

	v->v_cur_pos = v->v_top_pos;
	get_buffer(v);

	p = 0;
	e = 0;
	while (TRUE)
	{
		if (v->v_cur_pos == 0)
		{
			do_beep();
			return;
		}

		up_line_view(v);

		if (v->v_cur_ptr == 0)
		{
			do_beep();
			break;
		}

		p = ut_regexec(v->v_re, (char *)v->v_cur_ptr, &e);
		if (p)
			break;
	}

	if (p)
	{
		v->v_top_pos = v->v_cur_pos;
		if (e)
		{
			v->v_fnd_ptr = p;
			v->v_end_ptr = e;
		}
	}
	else
	{
		v->v_top_pos = save_top;
		v->v_cur_pos = save_cur;
		v->v_cur_ptr = save_ptr;
		get_buffer(v);
	}

	v_display(v);
}

static void view_set_mode_in_vfcb (VFCB *v, int mode)
{
	v->v_mode &= ~(V_AUTO | V_MODE);
	switch (mode)
	{
	case view_fmt_auto:
		v->v_mode |= V_AUTO;
		break;
	case view_fmt_ascii:
		v->v_mode |= V_ASCII;
		break;
	case view_fmt_wrap:
		v->v_mode |= V_WRAP;
		break;
	case view_fmt_hex:
		v->v_mode |= V_HEX;
		break;
	case view_fmt_dump:
		v->v_mode |= V_DUMP;
		break;
	}
}

int view_set_mode (const void *valp)
{
	int mode = *(const int *)valp;

	if (stdscr != 0)
	{
		int i;

		view_set_mode_in_vfcb(gbl(vfcb_av), mode);
		for (i=0; i<gbl(scr_cur_count); i++)
			view_set_mode_in_vfcb(gbl(vfcb_av_split)[i], mode);
		view_set_mode_in_vfcb(gbl(vfcb_fv), mode);
	}

	return (0);
}
