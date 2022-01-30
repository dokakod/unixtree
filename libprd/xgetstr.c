/*------------------------------------------------------------------------
 *	general get input string routine
 *
 *	usage: c = xgetstr (window, string, default, str_len, disp_len, type)
 *	where:
 *		window = window to display in
 *		string = char string to read into
 *		default = default string (used by up-arrow)
 *		str_len = max chars in string & default
 *		disp_len = max chars to display in window
 *		           0 means to end of window
 *		           neg means stop at end-of-window - disp_len
 *		type is as follows:
 *			XG_STRING		char string
 *			XG_FILENAME		filename pattern
 *			XG_FILESPEC		filespec pattern
 *			XG_PATHNAME		pathname
 *			XG_FILEPATH		pathname (no dest dir)
 *			XG_NUMBER		number (dest-dir key aborts)
 *			XG_STRING_AB	char string (dest-dir key aborts)
 *			XG_FILE_AB		pathname (dest-dir key aborts)
 *			XG_PASSWD		password (echo *)
 *			XG_STRING_DS	string (dest key ok)
 *
 *	returns number of bytes read or -1 for esc.
 */
#include "libprd.h"

static void xg_menu_disp (XG_ENTRY *xe, XG_INFO *xi)
{
	int		i;

	/*--------------------------------------------------------------------
	 * update keep/nokeep msg
	 */
	if (xi->xg_win != gbl(win_message))
	{
		wmove(gbl(win_message), 0, xi->xg_kep_col);
		fk_msg(gbl(win_message), CMDS_XGETSTR_KEEP,
			xe->keep ? msgs(m_xgetstr_nokeep) : msgs(m_xgetstr_keep));
		wrefresh(gbl(win_message));
	}

	/*--------------------------------------------------------------------
	 * display string & blank out remainder of display buffer
	 */
	wmove(xi->xg_win, xi->xg_y, xi->xg_x);
	for (i=0; xe->line[i]; i++)
	{
		if (i >= xi->xg_dlen)
			break;

		waddch(xi->xg_win, xe->line[i]);
	}
	for (; i<xi->xg_dlen; i++)
		waddch(xi->xg_win, ' ');

	wrefresh(xi->xg_win);
}

static int xgetstr_all (WINDOW *window, char *string, int history,
	int str_len, int disp_len, int type,
	void (*rtn)(void *data, int c), void *data, TREE **t)
{
	XG_LIST *	old_list = xg_tbl_find(history);
	TREE *		xget_tree;
	char		dest[MAX_PATHLEN];
	char		save_filename[MAX_FILELEN];
	XG_INFO		xg_info;
	XG_INFO *	xi			= &xg_info;
	XG_ENTRY *	xe_save		= 0;
	int			c;
	int			j;
	int			insert		= opt(insert_mode);
	int			keep		= FALSE;
	int			use_menu	= FALSE;
	int			recalc_loc	= TRUE;
	int			redisp_str	= TRUE;
	int			redisp_msg	= TRUE;

	/*--------------------------------------------------------------------
	 * get columns for msg keywords
	 */
	xi->xg_esc_col = getmaxx(gbl(win_message)) - 1 -
		display_len(msgs(m_bang_esc));

	xi->xg_ins_col = xi->xg_esc_col - 1 - display_len(msgs(m_xgetstr_insert));

	fk_msg_str(CMDS_XGETSTR_KEEP, msgs(m_xgetstr_keep), save_filename);
	xi->xg_kep_col = xi->xg_ins_col - 1 - display_len(save_filename);

	/*--------------------------------------------------------------------
	 * if filename type, save filename for DEFAULT key &
	 * clear the string if requested
	 */
	*save_filename = 0;
	if (type == XG_FILENAME || type == XG_FILENAME_NB)
	{
		strcpy(save_filename, string);

		if (type == XG_FILENAME)
			*string = 0;
	}

	/*--------------------------------------------------------------------
	 * clear tree pointer (for DEST_DIR functions)
	 */
	xget_tree = 0;

	/*--------------------------------------------------------------------
	 * setup xi struct
	 */
	xi->xg_win    = window;
	xi->xg_y      = getcury(window);
	xi->xg_x      = getcurx(window);
	xi->xg_dlen   = (disp_len <= 0) ?
		getmaxx(window) - getcurx(window) + disp_len : disp_len;
	xi->xg_slen   = str_len - 1;
	xi->xg_spos   = 0;
	xi->xg_dpos   = 0;
	xi->xg_count  = 0;
	xi->xg_offset = 0;

	/*--------------------------------------------------------------------
	 * now collect input until a CR is entered
	 */
	while (TRUE)
	{
		/*----------------------------------------------------------------
		 * redisplay msgs if requested
		 */
		if (redisp_msg)
		{
			if (xi->xg_win != gbl(win_message))
			{
				if (old_list != 0)
				{
					wmove(gbl(win_message), 0, xi->xg_kep_col);
					fk_msg(gbl(win_message), CMDS_XGETSTR_KEEP,
						keep ? msgs(m_xgetstr_nokeep) : msgs(m_xgetstr_keep));
				}

				wmove(gbl(win_message), 0, xi->xg_ins_col);
				xcaddstr(gbl(win_message), CMDS_INSERT,
					insert ? msgs(m_xgetstr_insert) : msgs(m_xgetstr_overlay));

				wmove(gbl(win_message), 0, xi->xg_esc_col);
				xcaddstr(gbl(win_message), CMDS_ESCAPE, msgs(m_bang_esc));
				wrefresh(gbl(win_message));

				if (! redisp_str)
				{
					wmove(xi->xg_win, xi->xg_y, xi->xg_x + xi->xg_dpos);
					wrefresh(xi->xg_win);
				}
			}

			redisp_msg = FALSE;
		}

		/*----------------------------------------------------------------
		 * redisplay string if requested
		 */
		if (redisp_str)
		{
			xi->xg_count  = strlen(string);

			if (recalc_loc)
			{
				xi->xg_offset = (xi->xg_count >= xi->xg_dlen - 1) ?
					xi->xg_count - xi->xg_dlen + 1 : 0;
			}

			wmove(xi->xg_win, xi->xg_y, xi->xg_x);
			for (j=0; string[j + xi->xg_offset]; j++)
			{
				if (j >= xi->xg_dlen)
					break;

				c = (type == XG_PASSWD ? '*' : string[j + xi->xg_offset]);
				waddch(xi->xg_win, c);
			}

			if (recalc_loc)
			{
				xi->xg_spos = j + xi->xg_offset;
				xi->xg_dpos = j;
			}

			for (; j<xi->xg_dlen; j++)
			{
				waddch(xi->xg_win, ' ');
			}

			if (xi->xg_spos >= xi->xg_slen)
			{
				xi->xg_spos--;
				xi->xg_dpos--;
			}

			wmove(xi->xg_win, xi->xg_y, xi->xg_x + xi->xg_dpos);
			wrefresh(xi->xg_win);

			redisp_str = FALSE;
			recalc_loc = FALSE;
		}

		/*----------------------------------------------------------------
		 * get next key
		 */
		c = xgetch(xi->xg_win);

		/*----------------------------------------------------------------
		 * bail if ESCAPE
		 */
		if (c == KEY_ESCAPE)
		{
			xi->xg_count = -1;
			break;
		}

		/*----------------------------------------------------------------
		 * done if RETURN
		 */
		if (c == KEY_RETURN)
		{
			break;
		}

		/*----------------------------------------------------------------
		 * ignore mouse keys
		 */
		if (c == KEY_MOUSE)
			continue;

		/*----------------------------------------------------------------
		 * check if DEST_DIR key
		 */
		if (c == cmds(CMDS_DEST_DIR))
		{
			switch (type)
			{
			case XG_NUMBER:
			case XG_STRING_AB:
			case XG_FILE_AB:
				return (cmds(CMDS_DEST_DIR));
			}
		}

		/*----------------------------------------------------------------
		 * check if DEFAULT key
		 */
		if (c == cmds(CMDS_XGETSTR_DEFAULT))
		{
			if (type == XG_FILENAME || type == XG_FILENAME_NB)
			{
				strcpy(string, save_filename);
				redisp_str = TRUE;
				recalc_loc = TRUE;
			}

			continue;
		}

		/*----------------------------------------------------------------
		 * check if KEEP key
		 */
		if (c == cmds(CMDS_XGETSTR_KEEP))
		{
			keep = ! keep;
			redisp_msg = TRUE;
			continue;
		}

		/*----------------------------------------------------------------
		 * INSERT key - toggle insert mode
		 */
		if (c == KEY_INSERT)
		{
			insert = 1 - insert;
			redisp_msg = TRUE;
			continue;
		}

		/*----------------------------------------------------------------
		 * check if PASTE key
		 */
		if (c == cmds(CMDS_XGETSTR_PASTE))
		{
			if (old_list != 0)
			{

				if (old_list->num > 0)
				{
					XG_ENTRY *xe = (XG_ENTRY *)bid(blast(old_list->lines));

					strcpy(string, xe->line);
					redisp_str = TRUE;
					recalc_loc = TRUE;

					xe_save = xe;
					use_menu = TRUE;
				}
			}

			continue;
		}

		/*----------------------------------------------------------------
		 * check if DEST_DIR key
		 */
		if ((c == cmds(CMDS_DEST_DIR)) &&
			(type == XG_PATHNAME || type == XG_STRING_DS))
		{
			char save_dest[MAX_PATHLEN];

			strcpy(save_dest, string);

			wmove(xi->xg_win, xi->xg_y, xi->xg_x);
			wclrtoeol(xi->xg_win);
			c = do_dest_dir(dest, TRUE, &xget_tree);
			if (c < 0)
			{
				strcpy(string, save_dest);
				redisp_str = TRUE;
				recalc_loc = TRUE;
				use_menu = FALSE;
				continue;
			}
			else if (c == 0)
			{
				strcpy(string, dest);
				use_menu = FALSE;
				redisp_str = TRUE;
				recalc_loc = TRUE;
				continue;
			}
			else /* c > 0 */
			{
				strcpy(string, dest);
				xi->xg_count = strlen(dest);
				use_menu = FALSE;
				if (t != 0)
					*t = xget_tree;
				break;
			}
		}
		xget_tree = 0;

		/*----------------------------------------------------------------
		 * BS key - delete char to left of cursor
		 */
		if (c == KEY_BS)
		{
			if (xi->xg_count == 0 || xi->xg_spos == 0)
				continue;

			xi->xg_count--;
			xi->xg_spos--;
			memmove(string + xi->xg_spos, string + xi->xg_spos + 1, xi->xg_count - xi->xg_spos + 1);

			if (xi->xg_dpos == 0)
			{
				xi->xg_offset--;
			}
			else
			{
				xi->xg_dpos--;
			}

			redisp_str = TRUE;
			use_menu = FALSE;
			continue;
		}

		/*----------------------------------------------------------------
		 * DELETE key - delete char at cursor
		 */
		if (c == KEY_DELETE)
		{
			if (xi->xg_count == 0 || xi->xg_spos == xi->xg_count)
				continue;

			xi->xg_count--;
			memmove(string + xi->xg_spos, string + xi->xg_spos + 1, xi->xg_count - xi->xg_spos + 1);
			if (xi->xg_count == 0)
			{
				xi->xg_spos = 0;
				xi->xg_dpos = 0;
			}
			else
			{
				if (xi->xg_spos > xi->xg_count)
				{
					xi->xg_spos--;
					xi->xg_dpos--;
				}
			}

			redisp_str = TRUE;
			use_menu = FALSE;
			continue;
		}

		/*----------------------------------------------------------------
		 * LEFT key - move cursor one place left
		 */
		if (c == KEY_LEFT)
		{
			if (xi->xg_spos == 0)
				continue;

			xi->xg_spos--;
			if (xi->xg_dpos == 0)
			{
				xi->xg_offset--;
			}
			else
			{
				xi->xg_dpos--;
			}

			redisp_str = TRUE;
			continue;
		}

		/*----------------------------------------------------------------
		 * RIGHT key - move cursor one place right
		 */
		if (c == KEY_RIGHT)
		{
			if (xi->xg_spos >= xi->xg_count)
				continue;

			xi->xg_spos++;
			if (xi->xg_dpos >= xi->xg_dlen - 1)
			{
				xi->xg_offset++;
			}
			else
			{
				xi->xg_dpos++;
			}

			redisp_str = TRUE;
			continue;
		}

		/*----------------------------------------------------------------
		 * DOWN key - clear the string
		 */
		if (c == KEY_DOWN)
		{
			*string = 0;
			redisp_str = TRUE;
			recalc_loc = TRUE;
			use_menu = FALSE;
			continue;
		}

		/*----------------------------------------------------------------
		 * UP key - display menu
		 */
		if (c == KEY_UP)
		{
			if (old_list != 0)
			{
				XG_ENTRY *xe;
				char save_string[MAX_PATHLEN];

				strcpy(save_string, string);
				xe = xg_menu(old_list, xi, xg_menu_disp);
				if (xe != 0)
				{
					strcpy(string, xe->line);
					use_menu = TRUE;
					xe_save = xe;
				}
				else
				{
					strcpy(string, save_string);
					use_menu = FALSE;
				}

				redisp_msg = TRUE;
				redisp_str = TRUE;
				recalc_loc = TRUE;
			}
			continue;
		}

		/*----------------------------------------------------------------
		 * HOME key - go to start of string
		 */
		if (c == KEY_HOME)
		{
			xi->xg_spos   = 0;
			xi->xg_dpos   = 0;
			xi->xg_offset = 0;

			redisp_str = TRUE;
			continue;
		}

		/*----------------------------------------------------------------
		 * END key - go to end of string
		 */
		if (c == KEY_END)
		{
			if (xi->xg_count == 0)
				continue;

			if (xi->xg_spos >= xi->xg_count)
				continue;

			xi->xg_spos = xi->xg_count;
			if ((xi->xg_spos - xi->xg_offset) < xi->xg_dlen - 1)
			{
				xi->xg_dpos = xi->xg_spos - xi->xg_offset;
			}
			else
			{
				xi->xg_offset = xi->xg_count - xi->xg_dlen + 1;
				xi->xg_dpos   = xi->xg_dlen - 1;
			}

			redisp_str = TRUE;
			continue;
		}

		/*----------------------------------------------------------------
		 * CONTROL key - throw it away
		 */
		if (IS_ACHAR(c) && iscntrl(c))
			continue;

		/*----------------------------------------------------------------
		 * NON-ASCII key - throw it away
		 */
		if (! IS_ACHAR(c))
		{
			/*------------------------------------------------------------
			 * if call-back routine specified, call it
			 */
			if (rtn != 0)
				(*rtn)(data, c);

			wmove(xi->xg_win, xi->xg_y, xi->xg_x + xi->xg_dpos);
			wrefresh(xi->xg_win);

			continue;
		}

		/*----------------------------------------------------------------
		 * got what should be a good char - check if bogus value
		 */
		switch (type)
		{
		case XG_FILENAME:
		case XG_FILENAME_NB:
			if (fn_bogus_filename(c))
			{
				do_beep();
				continue;
			}
			break;

		case XG_FILESPEC:
			if (fn_bogus_filespec(c))
			{
				do_beep();
				continue;
			}
			break;

		case XG_PATHNAME:
		case XG_FILEPATH:
		case XG_FILE_AB:
			if (fn_bogus_pathname(c))
			{
				do_beep();
				continue;
			}
			break;

		case XG_NUMBER:
			if (! isdigit(c))
			{
				do_beep();
				continue;
			}
			break;
		}

		/*----------------------------------------------------------------
		 * good character - add to string if room
		 */
		if (insert)
		{
			if (xi->xg_count == xi->xg_slen)
			{
				do_beep();
				continue;
			}

			for (j=xi->xg_count+1; j>=xi->xg_spos; j--)
				string[j+1] = string[j];
			string[xi->xg_spos++] = c;
			xi->xg_count++;
		}
		else
		{
			if (xi->xg_spos == xi->xg_count - 1 &&
			    xi->xg_spos == xi->xg_slen)
			{
				do_beep();
				continue;
			}

			string[xi->xg_spos++] = c;
			if (xi->xg_spos >= xi->xg_count)
				string[++xi->xg_count] = 0;
		}

		if (xi->xg_dpos < xi->xg_dlen - 1)
		{
		xi->xg_dpos++;
		}
		else
		{
			xi->xg_offset++;
		}

		redisp_str	= TRUE;
		use_menu	= FALSE;
	}

	/*--------------------------------------------------------------------
	 * if we got a string, either add to save list or
	 * mark which entry was used
	 */
	if (xi->xg_count > 0)
	{
		if (old_list != 0)
		{
			if (use_menu)
			{
				xg_tbl_mark(old_list, xe_save);
			}
			else
			{
				xg_tbl_add(old_list, string, keep);
			}
		}
	}

	/*--------------------------------------------------------------------
	 * clear msg line if not inputting there
	 */
	if (xi->xg_win != gbl(win_message))
		bang("");

	return (xi->xg_count);
}

int xgetstr (WINDOW *window, char *string, int history, int str_len,
	int disp_len, int type)
{
	return xgetstr_all(window, string, history, str_len, disp_len, type, 0, 0,
		0);
}

int xgetstr_cb (WINDOW *window, char *string, int history, int str_len,
	int disp_len, int type, void (*rtn)(void *data, int c), void *data)
{
	return xgetstr_all(window, string, history, str_len, disp_len, type, rtn,
		data, 0);
}

int xgetstr_tr (WINDOW *window, char *string, int history, int str_len,
	int disp_len, int type, TREE **t)
{
	return xgetstr_all(window, string, history, str_len, disp_len, type, 0,
		0, t);
}
