/*------------------------------------------------------------------------
 * process the "extract" cmd
 */
#include "libprd.h"

BLIST *make_ext_list (void)
{
	BLIST *b;
	BLIST *x;
	BLIST *taglist;
	FBLK *f;

	/* create list of tagged files */

	taglist = 0;
	for (b=gbl(scr_cur)->first_file; b; b=bnext(b))
	{
		f = (FBLK *)bid(b);
		if (is_file_tagged(f))
		{
			x = BNEW(f);
			if (x)
			{
				taglist = bappend(taglist, x);
			}
			else
			{
				errmsg(ER_IM, "", ERR_ANY);
				BSCRAP(taglist, FALSE);
				return (0);
			}
		}
	}

	/* sort list into archive file order */

	taglist = bsort(taglist, cmp_archloc);

	return (taglist);
}

BLIST * disp_ext_list (BLIST *taglist)
{
	BLIST *save_first_file   = gbl(scr_cur)->first_file;

	gbl(scr_cur)->first_file    = taglist;
	gbl(scr_cur)->top_file      = gbl(scr_cur)->first_file;
	gbl(scr_cur)->file_cursor   = gbl(scr_cur)->first_file;
	gbl(scr_cur)->cur_file      = (FBLK *)bid(gbl(scr_cur)->first_file);
	gbl(scr_cur)->cur_file_no   = 0;
	gbl(scr_cur)->cur_file_line = 0;
	gbl(scr_cur)->cur_file_col  = 0;

	disp_file_list();
	hilite_file(ON);

	return (save_first_file);
}

void restore_saved_first_file (BLIST *save_first_file)
{
	gbl(scr_cur)->first_file    = save_first_file;
	gbl(scr_cur)->top_file      = gbl(scr_cur)->first_file;
	gbl(scr_cur)->file_cursor   = gbl(scr_cur)->first_file;
	if (gbl(scr_cur)->first_file)
		gbl(scr_cur)->cur_file  = (FBLK *)bid(gbl(scr_cur)->first_file);
	gbl(scr_cur)->cur_file_no   = 0;
	gbl(scr_cur)->cur_file_line = 0;
	gbl(scr_cur)->cur_file_col  = 0;

	disp_file_list();
	hilite_file(ON);
}

int ext_ask (int rel_flag)
{
	int c;
	int i;
	char input_str[MAX_PATHLEN];
	NBLK *n;
	ABLK *a;

	n = gbl(scr_cur)->cur_root;
	a = (ABLK *)n->node_sub_blk;
	waddstr(gbl(win_commands), msgs(m_exttar_from));
	waddstr(gbl(win_commands), a->arch_devname);
	if (rel_flag)
	{
		*input_str = 0;
		while (!*input_str)
		{
			bang(msgs(m_exttar_entrel));
			waddstr(gbl(win_message), "  ");
			fk_msg(gbl(win_message), CMDS_DEST_DIR, msgs(m_exttar_f2pt));
			wrefresh(gbl(win_message));
			wmove(gbl(win_commands), 1, 0);
			xaddstr(gbl(win_commands), msgs(m_exttar_entnod));
			wrefresh(gbl(win_commands));
			if (gbl(scr_is_split))
			{
				i = next_scr_no(gbl(scr_cur_no));
				if ((gbl(scr_stat)[i].cur_root)->node_type == N_FS)
					strcpy(input_str, gbl(scr_stat)[i].path_name);
			}
			c = xgetstr(gbl(win_commands), input_str, XGL_EXT_REL_PATH,
				MAX_PATHLEN, 0, XG_PATHNAME);
			if (c <= 0)
				return (-1);
			fn_resolve_pathname(input_str);
			if (!fn_is_path_absolute(input_str))
			{
				errmsg(ER_NMBA, "", ERR_ANY);
				*input_str = 0;
			}
			else
			{
				strcpy(ai->a_ext_rel_path, input_str);
			}
		}
	}
	else
	{
		wrefresh(gbl(win_commands));
		bang("");
	}

	c = yesno_msg(msgs(m_exttar_extful));
	if (c < 0)
		return (c);
	ai->a_name_only = (c != 0);

	c = yesno_msg(msgs(m_exttar_repexi));
	if (c < 0)
		return (c);
	ai->a_do_auto = (c == 0);

	return (0);
}

long cvt_from_dec (const char *string, int len)
{
	long l;
	int i;
	int c;

	l = 0;
	for (i=0; i<len; i++)
	{
		c = string[i];
		if (c == 0)
			break;
		if (c > ' ')
			l = l*10 + c-'0';
	}
	return (l);
}

void cvt_to_dec (long num, char *string, int len)
{
	int i;
	int d;
	char buf[20];

	memset(string, 0, len);
	for (i=0; i<len; i++)
	{
		d = num % 10;
		buf[i] = d+'0';
		num = num / 10;
		if (num == 0)
			break;
	}
	for (; i>=0; i--)
		*string++ = buf[i];
}

void cvt_to_hex (long num, char *string, int len)
{
	int i;
	int c;

	for (i=0; i<len; i++)
		string[i] = '0';

	while (num)
	{
		c = num & 0x0f;
		if (c < 10)
			string[--i] = c + '0';
		else
			string[--i] = (c-10) + 'a';
		num >>= 4;
	}
}

long cvt_from_hex (const char *string, int len)
{
	long num;
	int i;
	int c;

	num = 0;
	for (i=0; i<len; i++)
	{
		num <<= 4;
		c = tolower(string[i]);
		if (isdigit(c))
			num += (c - '0');
		else
			num += (c - 'a' + 10);
	}
	return (num);
}

char *get_name (FBLK *f, char *rel_name, char *path)
{
	char *p;

	fblk_to_pathname(f, path);
	if (!*rel_name)
		p = path;
	else
	{
		if (strncmp(path, rel_name, strlen(rel_name)) == 0)
			p = path+strlen(rel_name);
		else
			p = path;
	}
	return (p);
}

long cvt_from_oct (const char *string, int len)
{
	long l;
	int i;
	int c;

	l = 0;
	for (i=0; i<len; i++)
	{
		c = string[i];
		if (c == 0)
			break;
		if (c > ' ')
			l = l*8 + c-'0';
	}
	return (l);
}

/*
 *	convert number to octal
 *
 *	fmt values:
 *		1	oooo<sp>0
 *		2	oooo<sp>
 *		3	oooo0<sp>
 *		4	oooo0
 *		5	oooo0 (left adjusted)
 *		6	ooooo
 */

void cvt_to_oct (long num, char *string, int len, int fmt)
{
	int i;
	int d;
	long n;

	if (fmt == 1)
	{
		string[--len] = 0;
		string[--len] = ' ';
		fmt = 0;
	}
	else if (fmt == 2)
	{
		string[--len] = ' ';
		fmt = 0;
	}
	else if (fmt == 3)
	{
		string[--len] = ' ';
		string[--len] = 0;
		fmt = 0;
	}
	else if (fmt == 4)
	{
		string[--len] = 0;
		fmt = 0;
	}

	if (fmt == 0)
	{
		for (i=0; i<len; i++)
			string[i] = ' ';
		if (!num)
		{
			string[len-1] = '0';
			return;
		}
		i = len-1;
		while (num)
		{
			d = num%8;
			string[i--] = d+'0';
			num = num/8;
		}
	}
	else if (fmt == 6)
	{
		for (i=0; i<len; i++)
			string[i] = '0';
		while (num)
		{
			d = num%8;
			string[--i] = d+'0';
			num = num/8;
		}
	}
	else /* if (fmt == 5) */
	{
		for (i=0; i<len; i++)
			string[i] = 0;
		if (num == 0)
		{
			string[0] = '0';
			return;
		}
		i = -1;
		for (n=num; n; n>>=3)
			i++;
		while (num)
		{
			d = num%8;
			string[i--] = d+'0';
			num = num/8;
		}
	}
}
