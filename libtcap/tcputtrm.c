/*------------------------------------------------------------------------
 * save term file
 */
#include "tcapcommon.h"

#define NUM_LEFT	20		/* # chars on left of equal sign */

typedef void OUT_RTN (void *data, const char *line);

/*------------------------------------------------------------------------
 * tcap_put_left() - internal routine to output a string with a fixed width
 */
static int tcap_put_left (char *buf, const char *str)
{
	int i;

	for (i=0; str[i]; i++)
		buf[i] = str[i];

	for (; i<NUM_LEFT; i++)
		buf[i] = ' ';

	buf[i++] = '=';
	buf[i++] = ' ';
	buf[i  ] = 0;

	return (i);
}

/*------------------------------------------------------------------------
 * tcap_print_char() - internal routine to print a char
 */
static int tcap_print_char (char *buf, int c)
{
	int i = 0;

	if (c < ' ')
	{
		switch (c)
		{
		case '\b':
			buf[i++] = '\\';
			buf[i++] = 'b';
			break;

		case '\t':
			buf[i++] = '\\';
			buf[i++] = 't';
			break;

		case '\n':
			buf[i++] = '\\';
			buf[i++] = 'n';
			break;

		case '\f':
			buf[i++] = '\\';
			buf[i++] = 'f';
			break;

		case '\r':
			buf[i++] = '\\';
			buf[i++] = 'r';
			break;

		case 0x1b:
			buf[i++] = '\\';
			buf[i++] = 'e';
			break;

		default:
			buf[i++] = '^';
			buf[i++] = c + '@';
			break;
		}
	}
	else if (c == '\\')
	{
		buf[i++] = '\\';
		buf[i++] = '\\';
	}
	else if (c == 0x7f)
	{
		buf[i++] = '^';
		buf[i++] = '?';
	}
	else if (c < 0x80)
	{
		buf[i++] = c;
	}
	else
	{
		buf[i++] = '\\';
		buf[i++] = 'x';
		buf[i++] = "0123456789abcdef"[c / 16];
		buf[i++] = "0123456789abcdef"[c % 16];
	}

	buf[i] = 0;

	return (i);
}

/*------------------------------------------------------------------------
 * tcap_print_qst() - internal routine to print a quoted string
 */
static int tcap_print_qst (char *buf, const char *str)
{
	int i = 0;

	buf[i++] = '"';
	for (; *str; str++)
		i += tcap_print_char(buf+i, *str);
	buf[i++] = '"';
	buf[i  ] = 0;

	return (i);
}

/*------------------------------------------------------------------------
 * tcap_print_tbl() - internal routine to print a char set
 */
static void tcap_print_tbl (OUT_RTN *rtn, void *data,
	const unsigned char *tbl_ptr, const unsigned char *font_ptr, int type)
{
	char			buf[128];
	const char *	font_name;
	int b;
	int i;
	int j;
	int f;
	int do_it;
	int did_print = FALSE;

	for (i=0; i<256; i++)
	{
		do_it = FALSE;
		f = 0;
		if (font_ptr)
			f = font_ptr[i];
		j = tbl_ptr[i];

		switch (type)
		{
		case FONT_TBL_INP:
			if (i != j)
				do_it = TRUE;
			font_name = "input";
			break;

		case FONT_TBL_REG:
			if (f || i != j)
				do_it = TRUE;
			font_name = "reg";
			break;

		case FONT_TBL_ALT:
			if ((i != j) ||
				(i >= 0x00 && i <= 0x1f && f != 2) ||
				(i >= 0x20 && i <= 0x7f && f != 0) ||
				(i >= 0x80 && i <= 0xff && f != 1) )
			{
				do_it = TRUE;
			}
			font_name = "alt";
			break;
		}

		if (do_it)
		{
			const XLATE_LIST *xp;

			if (! did_print)
			{
				(*rtn)(data, "");
				sprintf(buf, "# %s character set data", font_name);
				(*rtn)(data, buf);
				(*rtn)(data, "");
			}

			for (xp=tcap_xlate_list; xp->tbl_name; xp++)
			{
				if (xp->font_type == type && xp->font_no == f)
					break;
			}

			b  = tcap_put_left(buf, xp->tbl_name);
			buf[b++] = '"';
			b += tcap_print_char(buf+b, i);
			b += tcap_print_char(buf+b, j);
			buf[b++] = '"';
			buf[b  ] = 0;
			(*rtn)(data, buf);

			did_print = TRUE;
		}
	}
}

/*------------------------------------------------------------------------
 * tcap_put_trm_rtn() - output a term file to a CB routine
 */
int tcap_put_trm_rtn (const TERMINAL *tp, int all,
	void (*rtn)(void *data, const char *line), void *data)
{
	const TCAP_DATA *	td;
	const TC_DATA *		tc;
	const KFUNC *		k;
	const char *		ts;
	const char *		tv;
	char				buf[1024];
	int					i;
	int					b;
	int					hdr_printed;
	int					changed;
	const char *		title;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (tp == 0 || rtn == 0)
		return (-1);

	td = tp->tcap;

	tv = (const char *)td;
	ts = (const char *)&tcap_def_tcap_data;

	/*--------------------------------------------------------------------
	 * write title info if supplied
	 */
	title = td->tty_type;
	if (title && *title)
	{
		sprintf(buf, "# term info for \"%s\"", title);
		(*rtn)(data, buf);
		(*rtn)(data, "");
	}

	/*--------------------------------------------------------------------
	 * write database type if present & if it is needed
	 */
	if (td->db_type >= 0)
	{
		if (is_cmd_pres(td->parms.cm)  ||
			is_cmd_pres(td->parms.csr) ||
			is_cmd_pres(td->parms.csm) ||
			is_cmd_pres(td->parms.fgr) ||
			is_cmd_pres(td->parms.fgm) ||
			is_cmd_pres(td->parms.bgr) ||
			is_cmd_pres(td->parms.bgm) )
		{
			b  = tcap_put_left(buf, tcap_keyword_dbtype);
			b += sprintf(buf+b, "%s", tcap_db_val_to_name(td->db_type));
			(*rtn)(data, buf);
			(*rtn)(data, "");
		}
	}

	/*--------------------------------------------------------------------
	 * write termcap-data entries.
	 */
	(*rtn)(data, "# screen data");
	(*rtn)(data, "");

	for (tc=tcap_termcap_data; tc->trmc_label; tc++)
	{
		int				v;
		const char **	cp;
		const int *		ip;
		const int *		bp;
		int				print = TRUE;

		v  = tc->trmc_varptr;

		switch (tc->trmc_flags & TC_TYPE)
		{
		case TC_STR:
			cp = (const char **)(tv + v);
			if ( is_cmd_pres(*cp) )
			{
				b  = tcap_put_left(buf, tc->trmc_label);
				b += tcap_print_qst(buf+b, *cp);
				(*rtn)(data, buf);
			}
			break;

		case TC_NUM:
			ip = (const int *)(tv + v);
			if (! all)
			{
				const int *	ix = (const int *)(ts + v);

				if (*ip == *ix)
					print = FALSE;
			}

			if (print)
			{
			b  = tcap_put_left(buf, tc->trmc_label);
				b += sprintf(buf+b, "%d", *ip);
				(*rtn)(data, buf);
			}
			break;

		case TC_BLN:
			bp = (const int *)(tv + v);
			if (! all)
			{
				const int *	bx = (const int *)(ts + v);

				if (*bp == *bx)
					print = FALSE;
			}

			if (print)
			{
				b  = tcap_put_left(buf, tc->trmc_label);
				b += sprintf(buf+b, "%s", *bp ? "on" : "off");
				(*rtn)(data, buf);
			}
			break;
		}
	}

	/*--------------------------------------------------------------------
	 * color data
	 */
	{
		const char *	cd;
		int				print = TRUE;

		for (i=0; i<td->ints.max_fg; i++)
		{
			cd = td->strs.clr_fg[i];
			if (is_cmd_pres(cd) )
			{
				if (print)
				{
					(*rtn)(data, "");
					(*rtn)(data, "# color data");
					(*rtn)(data, "");
					print = FALSE;
				}

				b  = tcap_put_left(buf, tcap_get_fg_name_by_num(i));
				b += tcap_print_qst(buf+b, td->strs.clr_fg[i]);
				(*rtn)(data, buf);
			}
		}

		if (print)
			(*rtn)(data, "");

		for (i=0; i<td->ints.max_bg; i++)
		{
			cd = td->strs.clr_bg[i];
			if (is_cmd_pres(cd) )
			{
				if (print)
				{
					(*rtn)(data, "");
					(*rtn)(data, "# color data");
					(*rtn)(data, "");
					print = FALSE;
				}

				b  = tcap_put_left(buf, tcap_get_bg_name_by_num(i));
				b += tcap_print_qst(buf+b, td->strs.clr_bg[i]);
				(*rtn)(data, buf);
			}
		}
	}

	/*--------------------------------------------------------------------
	 * fg color map data
	 */
	{
		changed = FALSE;

		for (i=0; i<NUM_COLORS; i++)
		{
			int m = td->ints.color_fg_map[i];

			if (m != tcap_def_tcap_data.ints.color_fg_map[i])
			{
				changed = TRUE;
				break;
			}
		}

		if (changed)
		{
			(*rtn)(data, "");
			(*rtn)(data, "# color fg map");
			(*rtn)(data, "");

			for (i=0; i<NUM_COLORS; i++)
			{
				int m = td->ints.color_fg_map[i];

				b  = tcap_put_left(buf, tcap_keyword_map_fg);
				b += sprintf(buf+b, "%s", tcap_get_fg_name_by_num(i));
				buf[b++] = ':';
				if (m < NUM_COLORS)
					b += sprintf(buf+b, "%s", tcap_get_fg_name_by_num(m));
				else
					b += sprintf(buf+b, "%d", m);
				(*rtn)(data, buf);
			}
		}
	}

	/*--------------------------------------------------------------------
	 * bg color map data
	 */
	{
		changed = FALSE;

		for (i=0; i<NUM_COLORS; i++)
		{
			int m = td->ints.color_bg_map[i];

			if (m != tcap_def_tcap_data.ints.color_bg_map[i])
			{
				changed = TRUE;
				break;
			}
		}

		if (changed)
		{
			(*rtn)(data, "");
			(*rtn)(data, "# color bg map");
			(*rtn)(data, "");

			for (i=0; i<NUM_COLORS; i++)
			{
				int m = td->ints.color_bg_map[i];

				b  = tcap_put_left(buf, tcap_keyword_map_bg);
				b += sprintf(buf+b, "%s", tcap_get_fg_name_by_num(i));
				buf[b++] = ':';
				if (m < NUM_COLORS)
					b += sprintf(buf+b, "%s", tcap_get_fg_name_by_num(m));
				else
					b += sprintf(buf+b, "%d", m);
				(*rtn)(data, buf);
			}
		}
	}

	/*--------------------------------------------------------------------
	 * character sets
	 */
	tcap_print_tbl(rtn, data, td->inp_tbl, 0,                FONT_TBL_INP);
	tcap_print_tbl(rtn, data, td->reg_tbl, td->reg_font_tbl, FONT_TBL_REG);
	tcap_print_tbl(rtn, data, td->alt_tbl, td->alt_font_tbl, FONT_TBL_ALT);

	/*--------------------------------------------------------------------
	 * keys
	 */
	hdr_printed = FALSE;
	for (k=td->kfuncs; k; k=k->kf_next)
	{
		char kn[64];

		if (! hdr_printed)
		{
			hdr_printed = TRUE;
			(*rtn)(data, "");
			(*rtn)(data, "# key definitions");
			(*rtn)(data, "");
		}

		tcap_get_key_name(k->kf_code, kn);
		b  = tcap_put_left(buf, kn);
		b += tcap_print_qst(buf+b, k->kf_str);
		(*rtn)(data, buf);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_put_trm() - output a term file to a file
 */
static void tcap_put_trm_fp (void *data, const char *line)
{
	FILE *fp = (FILE *)data;

	fputs(line, fp);
	fputc('\n', fp);
}

int tcap_put_trm (const TERMINAL *tp, int all, const char *path)
{
	FILE *	fp;
	int		opened;
	int		rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (tp == 0 || path == 0 || *path == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * open the file
	 */
	if (strcmp(path, "-") == 0)
	{
		fp = stdout;
		opened = FALSE;
	}
	else
	{
		fp = fopen(path, "w");
		if (fp == 0)
			return (-1);
		opened = TRUE;
	}

	/*--------------------------------------------------------------------
	 * output the term info
	 */
	rc = tcap_put_trm_rtn(tp, all, tcap_put_trm_fp, fp);

	/*--------------------------------------------------------------------
	 * close the file
	 */
	if (opened)
		fclose(fp);

	return (rc);
}
