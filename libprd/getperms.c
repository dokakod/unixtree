/*------------------------------------------------------------------------
 * routine to get a perms string
 */
#include "libprd.h"

static const int perm_list[] =
{
	S_ISUID, S_ISGID, S_ISVTX,
	S_IRUSR, S_IWUSR, S_IXUSR,
	S_IRGRP, S_IWGRP, S_IXGRP,
	S_IROTH, S_IWOTH, S_IXOTH
};
#define NPERMS	sizeof(perm_list) / sizeof(*perm_list)

static int perm_display (XG_INFO *xi, const char *str, int pos)
{
	int	i;
	int	rpos;

	/*--------------------------------------------------------------------
	 * validate string
	 */
	for (i=0; str[i]; i++)
	{
		switch (str[i])
		{
		case '0':
		case '1':
		case '-':
		case ' ':
			break;

		default:
			return (0);
		}
	}

	if (i != (NPERMS + 3))
		return (0);

	/*--------------------------------------------------------------------
	 * display the string
	 */
	wmove(xi->xg_win, xi->xg_y, xi->xg_x);
	waddstr(xi->xg_win, str);

	/*--------------------------------------------------------------------
	 * now position the cursor if requested
	 */
	if (pos >= 0 && pos < NPERMS)
	{
		rpos = pos + (pos / 3);
		wmove(xi->xg_win, xi->xg_y, xi->xg_x + rpos);
	}
	else
	{
		rpos = 0;
	}

	return (rpos);
}

static void menu_display (XG_ENTRY *xe, XG_INFO *xi)
{
	int rc;

	/*--------------------------------------------------------------------
	 * update keep/nokeep msg
	 */
	wmove(gbl(win_message), 0, xi->xg_kep_col);
	fk_msg(gbl(win_message), CMDS_XGETSTR_KEEP,
		xe->keep ? msgs(m_xgetstr_nokeep) : msgs(m_xgetstr_keep));
	wrefresh(gbl(win_message));

	/*--------------------------------------------------------------------
	 * display the string
	 */
	rc = perm_display(xi, xe->line, -1);
	if (rc >= 0)
		wrefresh(xi->xg_win);
}

static void perms_to_str (int tag, int a_perms, int o_perms, char *str)
{
	char *	s = str;
	int		i;

	for (i=0; i<NPERMS; i++)
	{
		if (i > 0 && (i % 3) == 0)
			*s++ = ' ';

		if (tag)
		{
			if (! (a_perms & perm_list[i]))
				*s++ = '-';
			else if (o_perms & perm_list[i])
				*s++ = '1';
			else
				*s++ = '0';
		}
		else
		{
			if (! (a_perms & perm_list[i]))
				*s++ = '0';
			else if (o_perms & perm_list[i])
				*s++ = '1';
			else
				*s++ = '-';
		}
	}
	*s = 0;
}

static void str_to_perms (int tag, int *a_perms, int *o_perms,
	const char *str)
{
	const char *	s = str;
	int				i;

	*a_perms = 0;
	*o_perms = 0;

	for (i=0; i<NPERMS; i++)
	{
		if (i > 0 && (i % 3) == 0)
			*s++;

		switch (*s)
		{
		case '0':
			if (tag)
			{
				*a_perms |=  perm_list[i];
				*o_perms &= ~perm_list[i];
			}
			else
			{
				*a_perms &= ~perm_list[i];
				*o_perms &= ~perm_list[i];
			}
			break;

		case '1':
			if (tag)
			{
				*a_perms |=  perm_list[i];
				*o_perms |=  perm_list[i];
			}
			else
			{
				*a_perms |=  perm_list[i];
				*o_perms |=  perm_list[i];
			}
			break;

		case '-':
			if (tag)
			{
				*a_perms &= ~perm_list[i];
				*o_perms &= ~perm_list[i];
			}
			else
			{
				*a_perms |=  perm_list[i];
				*o_perms &= ~perm_list[i];
			}
			break;
		}

		s++;
	}
}

static int perms_input (XG_INFO *xi, int tag, char *buf)
{
	XG_LIST *	history	= xg_tbl_find(tag ? XGL_SET_PERMS : XGL_GET_PERMS);
	XG_ENTRY *	xe		= 0;
	char		save_buf[128];
	int			pos		= 3;
	int			keep	= FALSE;
	int			c;

	/*--------------------------------------------------------------------
	 * save the original buffer
	 */
	strcpy(save_buf, buf);

	/*--------------------------------------------------------------------
	 * display keep msg
	 */
	wmove(gbl(win_message), 0, xi->xg_kep_col);
		fk_msg(gbl(win_message), CMDS_XGETSTR_KEEP,
		keep ? msgs(m_xgetstr_nokeep) : msgs(m_xgetstr_keep));
	wrefresh(gbl(win_message));

	/*--------------------------------------------------------------------
	 * process input
	 */
	while (TRUE)
	{
		int rpos;

		rpos = perm_display(xi, buf, pos);
		wrefresh(xi->xg_win);

		c = xgetch(xi->xg_win);

		if (c == KEY_ESCAPE)
		{
			return (-1);
		}

		if (c == KEY_RETURN)
		{
			break;
		}

		if (c == cmds(CMDS_ATTR_COPYFILE))
		{
			struct stat stbuf;

			if (attr_get_file(msgs(m_attr_per), &stbuf))
				return (-1);

			if (tag)
			{
				perms_to_str(tag, stbuf.st_mode, stbuf.st_mode, buf);
			}
			else
			{
				perms_to_str(tag, stbuf.st_mode, S_PERMS, buf);
			}
			xe = 0;
			break;
		}

		if (c == cmds(CMDS_XGETSTR_KEEP))
		{
			keep = ! keep;
			wmove(gbl(win_message), 0, xi->xg_kep_col);
				fk_msg(gbl(win_message), CMDS_XGETSTR_KEEP,
				keep ? msgs(m_xgetstr_nokeep) : msgs(m_xgetstr_keep));
			wrefresh(gbl(win_message));
			continue;
		}

		if (c == KEY_HOME)
		{
			pos = 0;
			continue;
		}

		if (c == KEY_END)
		{
			pos = NPERMS - 1;
			continue;
		}

		if (c == KEY_BS || c == KEY_LEFT)
		{
			if (pos > 0)
				pos--;
			continue;
		}

		if (c == ' ' || c == KEY_RIGHT)
		{
			if (pos < (NPERMS - 1))
				pos++;
			continue;
		}

		if (c == KEY_DOWN)
		{
			strcpy(buf, save_buf);
			pos = 3;
			continue;
		}

		if (c == KEY_UP)
		{
			xe = xg_menu(history, xi, menu_display);
			if (xe != 0)
			{
				strcpy(buf, xe->line);
				pos = 3;
			}
		}

		if (c == '0' || c == '1' || c == '-')
		{
			buf[rpos] = c;
			if (pos < (NPERMS-1))
				pos++;
			xe = 0;
			continue;
		}
	}

	/*--------------------------------------------------------------------
	 * save history if new entry
	 */
	if (xe == 0)
	{
		xg_tbl_add(history, buf, keep);
	}
	else
	{
		xg_tbl_mark(history, xe);
	}

	return (0);
}

int get_perm_masks (int tag, int *pa_perms, int *po_perms)
{
	char		buf[128];
	XG_INFO		xi_s;
	XG_INFO *	xi = &xi_s;
	int			rc;

	/*--------------------------------------------------------------------
	 * display window stuff
	 */
	{
		int x;

		if (tag)
			bang(msgs(m_attr_onoff_tag));
		else
			bang(msgs(m_attr_onoff));
		dest_msg(CMDS_ATTR_COPYFILE);
		esc_msg();
		wrefresh(gbl(win_message));

		werase(gbl(win_commands));
		wmove(gbl(win_commands), 1, 0);
		waddstr(gbl(win_commands), msgs(m_attr_entper));
		x = getcurx(gbl(win_commands));
		wmove(gbl(win_commands), 0, x);
		waddstr(gbl(win_commands), msgs(m_attr_rwx3));
		wmove(gbl(win_commands), 1, x+1);
		wrefresh(gbl(win_commands));
	}

	/*--------------------------------------------------------------------
	 * initialize XG_INFO struct
	 */
	xi->xg_win		= gbl(win_commands);
	xi->xg_y		= getcury(xi->xg_win);
	xi->xg_x		= getcurx(xi->xg_win);

	xi->xg_slen		= NPERMS + 3;
	xi->xg_dlen		= NPERMS + 3;

	xi->xg_spos		= 0;
	xi->xg_dpos		= 0;

	xi->xg_count	= NPERMS + 3;
	xi->xg_offset	= 0;

	/*--------------------------------------------------------------------
	 * get columns for msg keywords
	 */
	xi->xg_esc_col = getmaxx(gbl(win_message)) - 1 -
		display_len(msgs(m_bang_esc));

	xi->xg_ins_col = xi->xg_esc_col - 1 -
		display_len(msgs(m_xgetstr_insert));

	fk_msg_str(CMDS_XGETSTR_KEEP, msgs(m_xgetstr_keep), buf);
	xi->xg_kep_col = xi->xg_ins_col - 1 - display_len(buf);

	/*--------------------------------------------------------------------
	 * convert perms to a string
	 */
	perms_to_str(tag, *pa_perms, *po_perms, buf);

	/*--------------------------------------------------------------------
	 * now do the input
	 */
	rc = perms_input(xi, tag, buf);
	if (rc)
		return (-1);

	/*--------------------------------------------------------------------
	 * convert the final string back
	 */
	str_to_perms(tag, pa_perms, po_perms, buf);

	return (0);
}
