/*------------------------------------------------------------------------
 * Routines for processing config menu.
 */
#include "libprd.h"

/*------------------------------------------------------------------------
 * item display indents
 */
#define INDENT_HDG		2
#define INDENT_ITEM		6
#define INDENT_VAL		52

/*------------------------------------------------------------------------
 * cfg_get_input() - get input option
 */
static int cfg_get_input (CONFIG_INFO *config)
{
	int c;

	while (TRUE)
	{
		if (trm_is_changed(TRUE))
		{
			resize_term();
			wclear(stdscr);
			wrefresh(stdscr);
			cfg_redraw();
		}
		else if (config->screen_changed)
		{
			int old_scrn = config->setting_color;

			switch (opt(screen_type))
			{
			case screen_auto:	config->setting_color = has_colors();	break;
			case screen_color:	config->setting_color = TRUE;			break;
			case screen_mono:	config->setting_color = FALSE;			break;
			}

			if (old_scrn != config->setting_color)
			{
				setup_window_attributes();
				wclear(stdscr);
				wrefresh(stdscr);
				cfg_redraw();
			}

			config->screen_changed = FALSE;
		}
		else if (config->acs_changed)
		{
			cfg_redraw();
			config->acs_changed = FALSE;
		}

		wmove(gbl(win_config), 1, 1);
		wrefresh(gbl(win_config));
		setdelay(stdscr, pgm_const(kbd_wait_interval));
		c = xgetch(gbl(win_config));
		setdelay(stdscr, -1);

		if (c == cmds(CMDS_REFRESH_SCREEN))
		{
			wrefresh(curscr);
			c = 0;
		}
		else if (c == cmds(CMDS_SHELL_ESCAPE))
		{
			xsystem("", "", FALSE);
			c = 0;
		}

		if (c)
			break;
	}
	return (c);
}

/*------------------------------------------------------------------------
 * cfg_prompt_clear() - clear out the prompt line
 */
static void cfg_prompt_clear (void)
{
	int i;

	wmove(gbl(win_config), getmaxy(gbl(win_config))-2, 2);
	for (i=2; i<getmaxx(gbl(win_config))-2; i++)
		waddch(gbl(win_config), ' ');
	wmove(gbl(win_config), getmaxy(gbl(win_config))-2, 2);
}

/*------------------------------------------------------------------------
 * cfg_prompt() - display the appropriate prompt
 */
static void cfg_prompt (const CFG_MENU *line)
{
	const DEFS *ds;

	cfg_prompt_clear();

	switch (line->type)
	{
	case CFG_HDG:
		break;

	case CFG_ITEM:
		ds = dflt_find_entry(dflt_tbl, line->varptr);
		if (ds != 0)
		{
			waddstr(gbl(win_config), dflt(ds->p_str));
		}
		break;

	case CFG_BTN:
		switch (line->selection)
		{
		case CMDS_CONFIG_NEXT:
				waddstr(gbl(win_config), msgs(m_config_next_p));
				break;
		case CMDS_CONFIG_PREV:
				waddstr(gbl(win_config), msgs(m_config_prev_p));
				break;
		case CMDS_CONFIG_MAIN:
				waddstr(gbl(win_config), msgs(m_config_main_p));
				break;
		}
		break;
	}

	wrefresh(gbl(win_config));
}

/*------------------------------------------------------------------------
 * cfg_get_fn() - get filename for config file
 */
static int cfg_get_fn (CONFIG_INFO *config)
{
	int c;
	char input_str[MAX_PATHLEN];
	char filename[MAX_FILELEN];

	if (*config->cfg_fn == 0)
	{
		strcpy(filename, pgmi(m_pgm_program));
		fn_set_ext(filename, pgm_const(cfg_ext));

		if (config->display_changes_made)
		{
			if (gbl(pgm_term) != 0 && *gbl(pgm_term) != 0)
			{
				strcat(filename, ".");
				strcat(filename, gbl(pgm_term));
			}
		}

		strcpy(config->cfg_fn, gbl(pgm_home));
		fn_append_filename_to_dir(config->cfg_fn, filename);
	}

	bang(msgs(m_config_fnb));
	cfg_prompt_clear();
	xaddstr(gbl(win_config), msgs(m_config_fnp));
	wrefresh(gbl(win_config));

	strcpy(input_str, config->cfg_fn);
	c = xgetstr(gbl(win_config), input_str, XGL_CFG_FILE, MAX_PATHLEN, -2,
		XG_FILEPATH);
	if (c <= 0)
		return (-1);
	strcpy(config->cfg_fn, input_str);

	return (0);
}

/*------------------------------------------------------------------------
 * cfg_get_num() - get a numeric entry
 */
static int cfg_get_num (int n)
{
	char input_str[128];
	int i;

	sprintf(input_str, "%d", n);
	bang(msgs(m_config_numb));
	cfg_prompt_clear();
	xaddstr(gbl(win_config), msgs(m_config_nump));
	wrefresh(gbl(win_config));
	i = xgetstr(gbl(win_config), input_str, XGL_CFG_NUM, 16, -2, XG_NUMBER);
	if (i > 0)
	{
		i = atoi(input_str);
		return (i);
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * cfg_get_char() - get a char entry
 */
static int cfg_get_char (char *str)
{
	int i;

	bang(msgs(m_config_chrb));
	cfg_prompt_clear();
	xaddstr(gbl(win_config), msgs(m_config_chrp));
	wrefresh(gbl(win_config));
	i = xgetstr(gbl(win_config), str, XGL_CFG_CHR, 2, -2, XG_STRING);
	if (i >= 0)
	{
		return (0);
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * cfg_get_str() - get a string entry
 */
static int cfg_get_str (char *str)
{
	int i;

	bang(msgs(m_config_strb));
	cfg_prompt_clear();
	xaddstr(gbl(win_config), msgs(m_config_strp));
	wrefresh(gbl(win_config));
	i = xgetstr(gbl(win_config), str, XGL_CFG_STR, 128, -2, XG_STRING);
	if (i >= 0)
	{
		return (0);
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * cfg_display_line() - display a menu line
 */
static void cfg_display_line (CONFIG_INFO *config, const CFG_MENU *line,
	int hilite)
{
	int				i;
	int				l;
	attr_t			old_st;
	char *			m;
	const DEFS *	ds;
	char			buff[128];

	old_st = wstandget(gbl(win_config));

	if (hilite)
	{
		wstandset(gbl(win_config), gbl(scr_reg_cursor_attr));
		wstandout(gbl(win_config));
	}
	else
	{
		wstandset(gbl(win_config), gbl(scr_reg_menu_attr));
	}

	switch (line->type)
	{
	case CFG_MAIN:
		/* add space before & after for hiliting */
		m = msgs((int)line->varptr);
		wmove(gbl(win_config), line->line+3, config->main_indent-1);
		waddch(gbl(win_config), ' ');
		if (! hilite)
			wstandout(gbl(win_config));
		waddch(gbl(win_config), *m);
		if (! hilite)
			wstandend(gbl(win_config));
		waddstr(gbl(win_config), m+1);
		for (i=display_len(m); i<=config->main_width; i++)
			waddch(gbl(win_config), ' ');
		break;

	case CFG_HDG:
		/* no need for hilite stuff since we never choose this */
		wmove(gbl(win_config), line->line+3, INDENT_HDG);
		waddstr(gbl(win_config), dflt((int)line->varptr));
		break;

	case CFG_ITEM:
		/* space before and after to end of line */
		ds = dflt_find_entry(dflt_tbl, line->varptr);
		if (ds == 0)
			break;

		/* only display a list entry if more than 1 entry in list */

		if (ds->type == DEF_LIST)
		{
			BLIST *list = DFLT_LPTR(dflt_tbl, ds);

			if (bcount(list) <= 1)
				break;
		}

		wmove(gbl(win_config), line->line+3, INDENT_ITEM-1);
		/* display asterisk if different from default */
		if (! dflt_is_opt_default(dflt_tbl, ds))
			waddch(gbl(win_config), '*');
		else
			waddch(gbl(win_config), ' ');
		if (! hilite)
			wstandout(gbl(win_config));
		waddch(gbl(win_config), line->selection);
		if (! hilite)
			wstandend(gbl(win_config));
		waddch(gbl(win_config), ' ');
		m = dflt(ds->m_str);
		l = display_len(m) + INDENT_ITEM;
		waddstr(gbl(win_config), m);
		for (l++; l<INDENT_VAL; l++)
			waddch(gbl(win_config), ' ');
		dflt_format(dflt_tbl, ds, FALSE, buff);
		if (*buff == 0)
			strcpy(buff, "\"\"");

		/*----------------------------------------------------------------
		 * If item is color or mono attribute & display
		 * corresponds (ie color on color & mono on mono)
		 * then display in correct attribute.
		 * Otherwise use standout to end of line.
		 */
		if ((ds->type == DEF_COLOR || ds->type == DEF_MONO) &&
			config->setting_color == gbl(scr_is_color))
		{
			OPT_ATTR *	oa;
			attr_t		save;

			oa = (OPT_ATTR *)DFLT_OPTS(dflt_tbl, ds);
			save = wattrget(gbl(win_config));

			if (hilite)
				wstandend(gbl(win_config));
			wattrset(gbl(win_config), oa->opts[OPT_OPT]);
			waddstr(gbl(win_config), buff);
			wattrset(gbl(win_config), save);
			for (l=getmaxx(gbl(win_config))-getcurx(gbl(win_config))-1; l; l--)
				waddch(gbl(win_config), ' ');
			if (hilite)
				wstandout(gbl(win_config));
		}
		else
		{
			waddstr(gbl(win_config), buff);
			for (l=getmaxx(gbl(win_config))-getcurx(gbl(win_config))-1; l; l--)
				waddch(gbl(win_config), ' ');
		}
		break;

	case CFG_BTN:
		/* space before & after */
		wmove(gbl(win_config), getmaxy(gbl(win_config))-4, line->line-1);
		waddch(gbl(win_config), ' ');
		m = msgs((int)line->varptr);
		if (hilite)
			xciaddstr(gbl(win_config), line->selection, m);
		else
			xcaddstr (gbl(win_config), line->selection, m);
		waddch(gbl(win_config), ' ');
		break;
	}

	wstandend(gbl(win_config));
	wstandset(gbl(win_config), old_st);
}

/*------------------------------------------------------------------------
 * cfg_display_menu() - display a menu
 */
static void cfg_display_menu (CONFIG_INFO *config, const CFG_MENU *menu)
{
	int i;

	for (i=0; menu[i].type; i++)
	{
		cfg_display_line(config, menu + i, OFF);
	}
	wrefresh(gbl(win_config));
}

/*------------------------------------------------------------------------
 * cfg_bang_display() - display a message line for display entries
 */
static void cfg_bang_display (CONFIG_INFO *config)
{
	char *m;

	m = msgs(m_config_esc2);
	wmove(gbl(win_message), 0, getmaxx(gbl(win_message))-2-display_len(m));
	xcaddstr(gbl(win_message), CMDS_ESCAPE, m);
	wmove(gbl(win_message), 0, 0);
	if (config->setting_color)
	{
		fk_msg(gbl(win_message), CMDS_CONFIG_VAL_UP, msgs(m_config_clrfu));
		waddstr(gbl(win_message), "  ");
		fk_msg(gbl(win_message), CMDS_CONFIG_VAL_DN, msgs(m_config_clrfd));
		waddstr(gbl(win_message), "  ");
		fk_msg(gbl(win_message), CMDS_CONFIG_BG_UP, msgs(m_config_clrbu));
		waddstr(gbl(win_message), "  ");
		fk_msg(gbl(win_message), CMDS_CONFIG_BG_DN, msgs(m_config_clrbd));
	}
	else
	{
		fk_msg(gbl(win_message), CMDS_CONFIG_VAL_UP, msgs(m_config_valu));
		waddstr(gbl(win_message), "  ");
		fk_msg(gbl(win_message), CMDS_CONFIG_VAL_DN, msgs(m_config_vald));
	}

	/*--------------------------------------------------------------------
	 * display RESTORE msg if changed
	 */
	if (config->item_changed)
	{
		waddstr(gbl(win_message), "  ");
		fk_msg(gbl(win_message), CMDS_CONFIG_RESTORE, msgs(m_config_f2));
	}
	else
	{
		wmove(gbl(win_message), 0, getcurx(gbl(win_message))+3+
				strlen(keyname(cmds(CMDS_CONFIG_RESTORE)))+
				display_len(msgs(m_config_f2)));
	}

	/*--------------------------------------------------------------------
	 * display DEFAULT msg if not default
	 */
	if (config->menu[config->page_ent].type == CFG_ITEM)
	{
		const DEFS *ds;

		ds = dflt_find_entry(dflt_tbl, config->menu[config->page_ent].varptr);
		if (ds != 0 && ! dflt_is_opt_default(dflt_tbl, ds))
		{
			waddstr(gbl(win_message), "   ");
			fk_msg(gbl(win_message), CMDS_CONFIG_DEFAULT, msgs(m_config_f3));
		}
	}
}

/*------------------------------------------------------------------------
 * cfg_bang_settings() - display a message line for a settings entry
 */
static void cfg_bang_settings (CONFIG_INFO *config)
{
	char *m;

	m = msgs(m_config_esc2);
	wmove(gbl(win_message), 0, getmaxx(gbl(win_message))-2-display_len(m));
	xcaddstr(gbl(win_message), CMDS_ESCAPE, m);
	wmove(gbl(win_message), 0, 0);
	fk_msg(gbl(win_message), CMDS_CONFIG_VAL_UP, msgs(m_config_valu));
	waddstr(gbl(win_message), "  ");
	fk_msg(gbl(win_message), CMDS_CONFIG_VAL_DN, msgs(m_config_vald));
	waddstr(gbl(win_message), "  ");
	xcaddstr(gbl(win_message), CMDS_RETURN, msgs(m_config_ret2));

	/*--------------------------------------------------------------------
	 * display RESTORE msg if changed
	 */
	if (config->item_changed)
	{
		waddstr(gbl(win_message), "   ");
		fk_msg(gbl(win_message), CMDS_CONFIG_RESTORE, msgs(m_config_f2));
	}
	else
	{
		wmove(gbl(win_message), 0, getcurx(gbl(win_message))+3+
			strlen(keyname(cmds(CMDS_CONFIG_RESTORE)))+
			display_len(msgs(m_config_f2)));
	}

	/*--------------------------------------------------------------------
	 * display DEFAULT msg if not default
	 */
	if (config->menu[config->page_ent].type == CFG_ITEM)
	{
		const DEFS *ds;

		ds = dflt_find_entry(dflt_tbl, config->menu[config->page_ent].varptr);
		if (ds != 0 && ! dflt_is_opt_default(dflt_tbl, ds))
		{
			waddstr(gbl(win_message), "   ");
			fk_msg(gbl(win_message), CMDS_CONFIG_DEFAULT, msgs(m_config_f3));
		}
	}
}

/*------------------------------------------------------------------------
 * cfg_bang_main() - display a message line for a main entry
 */
static void cfg_bang_main (CONFIG_INFO *config)
{
	char *m;

	m = msgs(m_config_esc1);
	wmove(gbl(win_message), 0, getmaxx(gbl(win_message))-2-display_len(m));
	xcaddstr(gbl(win_message), CMDS_ESCAPE, m);
	wmove(gbl(win_message), 0, 0);
	xaddstr(gbl(win_message), msgs(m_config_sel1));
	waddstr(gbl(win_message), "   ");
	xcaddstr(gbl(win_message), CMDS_RETURN, msgs(m_config_ret1));
}

/*------------------------------------------------------------------------
 * cfg_bang() - display a message line appropriate to the entry type
 */
static void cfg_bang (CONFIG_INFO *config)
{
	wattrset (gbl(win_message), gbl(scr_reg_lolite_attr));
	wstandset(gbl(win_message), gbl(scr_reg_menu_attr));

	leaveok(gbl(win_message), TRUE);
	werase(gbl(win_message));

	switch (config->menu_type)
	{
	case CFG_TYPE_MAIN:		cfg_bang_main(config);		break;
	case CFG_TYPE_SETTINGS:	cfg_bang_settings(config);	break;
	case CFG_TYPE_DISPLAY:	cfg_bang_display(config);	break;
	}

	wrefresh(gbl(win_message));
	leaveok(gbl(win_message), FALSE);
}

/*------------------------------------------------------------------------
 * cfg_nav() - navigate around a menu
 */
static int cfg_nav (CONFIG_INFO *config, const CFG_MENU *menu, int *cur_ent,
	int c)
{
	int i;
	int m;
	int y;
	int new_ent;

	c = toupper(c);
	new_ent = *cur_ent;

	if (c == KEY_MOUSE)
	{
		m = mouse_get_event(gbl(win_config));
		c = -1;
		if (m == MOUSE_LBD)
		{
			y = mouse_y(gbl(win_config)) - getbegy(gbl(win_config)) - 3;
			for (i=0; menu[i].type; i++)
			{
				if (menu[i].selection && menu[i].line == y)
					break;
			}
			if (menu[i].type)
			{
				new_ent = i;
				if (*cur_ent != new_ent)
				{
					cfg_display_line(config, menu + *cur_ent, OFF);
					cfg_display_line(config, menu +  new_ent, ON);
					cfg_prompt(&menu[new_ent]);
				}
				c = KEY_RETURN;
			}
		}
	}

	else if (c == KEY_UP || c == KEY_LEFT)
	{
		/* go up one to next item or button */
		for (new_ent--; new_ent>=0; new_ent--)
			if (menu[new_ent].type != CFG_HDG)
				break;
		if (new_ent < 0)
		{
			for (new_ent=0; menu[new_ent].type; new_ent++)
				;
			for (new_ent--; menu[new_ent].type == CFG_HDG; new_ent--)
				;
		}
		c = 0;
	}

	else if (c == KEY_DOWN || c == KEY_RIGHT || c == ' ')
	{
		/* go down one to next item or button */
		for (new_ent++; menu[new_ent].type; new_ent++)
			if (menu[new_ent].type != CFG_HDG)
				break;
		if (!menu[new_ent].type)
			for (new_ent=0; menu[new_ent].type == CFG_HDG; new_ent++)
				;
		c = 0;
	}

	else if (c == KEY_HOME)
	{
		/* go to first item on page */
		for (new_ent=0; ; new_ent++)
			if (menu[new_ent].type == CFG_ITEM ||
				menu[new_ent].type == CFG_MAIN)
				break;
		c = 0;
	}

	else if (c == KEY_END)
	{
		/* go to last item on page */
		for (new_ent=0; menu[new_ent].type; new_ent++)
			;
		for (new_ent--; ; new_ent--)
		{
			if (menu[new_ent].type == CFG_ITEM ||
				menu[new_ent].type == CFG_MAIN)
				break;
		}
		c = 0;
	}

	else if (c == KEY_TAB)
	{
		if (menu[new_ent].type == CFG_ITEM)
		{
			/* go to first item in next group (after heading) */
			for (new_ent++; menu[new_ent].type; new_ent++)
				if (menu[new_ent].type == CFG_HDG)
					break;

			if (!menu[new_ent].type)
				for (new_ent=0; menu[new_ent].type; new_ent++)
					if (menu[new_ent].type == CFG_HDG)
						break;
			new_ent++;
			c = 0;
		}
		else if (menu[new_ent].type == CFG_BTN)
		{
			/* go to next button */
			for (new_ent++; menu[new_ent].type; new_ent++)
				if (menu[new_ent].type == CFG_BTN)
					break;

			if (!menu[new_ent].type)
				for (new_ent=0; menu[new_ent].type; new_ent++)
					if (menu[new_ent].type == CFG_BTN)
						break;
			c = 0;
		}
	}

	else if (c == KEY_SHIFT_TAB)
	{
		if (menu[new_ent].type == CFG_ITEM)
		{
			/* go to first item in previous group (after heading) */
			if (menu[new_ent-1].type == CFG_ITEM)
			{
				for (new_ent--; menu[new_ent].type!=CFG_HDG; new_ent--)
					;
			}
			else
			{
				new_ent--;
				if (new_ent == 0)
					for (new_ent=0; menu[new_ent].type; new_ent++)
						;
				for (new_ent--; menu[new_ent].type!=CFG_HDG; new_ent--)
					;
			}
			new_ent++;
			c = 0;
		}

		else if (menu[new_ent].type == CFG_BTN)
		{
			/* go to prev button */
			for (new_ent--; new_ent>=0; new_ent--)
				if (menu[new_ent].type == CFG_BTN)
					break;
			if (new_ent < 0)
			{
				for (new_ent=0; menu[new_ent].type; new_ent++)
					;
				for (new_ent--; ; new_ent--)
					if (menu[new_ent].type == CFG_BTN)
						break;
			}
			c = 0;
		}
	}

	else
	{
		if      (TO_LOWER(c) == cmds(CMDS_CONFIG_PREV) || c == KEY_PGUP)
		{
			c = CMDS_CONFIG_PREV;
		}
		else if (TO_LOWER(c) == cmds(CMDS_CONFIG_NEXT) || c == KEY_PGDN)
		{
			c = CMDS_CONFIG_NEXT;
		}
		else if (TO_LOWER(c) == cmds(CMDS_CONFIG_MAIN) || c == KEY_BS)
		{
			c = CMDS_CONFIG_MAIN;
		}
		else if (TO_LOWER(c) == cmds(CMDS_COMMON_QUIT) &&
					config->menu_type != CFG_TYPE_MAIN)
		{
			c = CMDS_CONFIG_MAIN;
		}

		for (i=0; menu[i].type; i++)
		{
			if ((menu[i].type == CFG_MAIN && c == *msgs((int)menu[i].varptr))||
				(menu[i].type == CFG_ITEM && c == menu[i].selection) ||
				(menu[i].type == CFG_BTN  && c == menu[i].selection))
			{
				new_ent = i;
				if (new_ent != *cur_ent)
				{
					cfg_display_line(config, menu + *cur_ent, OFF);
					cfg_display_line(config, menu +  new_ent, OFF);
					cfg_prompt(&menu[new_ent]);
					cfg_bang(config);
				}
				c = KEY_RETURN;
				break;
			}
		}
	}

	if (c == 0)
	{
		if (*cur_ent != new_ent)
		{
			cfg_display_line(config, menu + *cur_ent, OFF);
			cfg_display_line(config, menu +  new_ent, ON);
			cfg_prompt(menu + new_ent);
		}
	}

	*cur_ent = new_ent;
	return (c);
}

/*------------------------------------------------------------------------
 * cfg_change_ent() - change an entry
 */
static int cfg_change_ent (CONFIG_INFO *config, const CFG_MENU *line, int c)
{
	OPT_BOOL *		ob;
	OPT_INT	 *		oi;
	OPT_ATTR *		oa;
	OPT_STR	 *		os;
	int				i;
	int				f, b;
	attr_t			a;
	int				rc = 0;
	const DEFS *	ds;
	const DEFC *	dc;
	BLIST *			bl;
	BLIST *			bp;
	char			input_str[128];

	/*--------------------------------------------------------------------
	 * find option entry
	 */
	ds = dflt_find_entry(dflt_tbl, line->varptr);
	if (ds == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * change it
	 */
	switch (ds->type)
	{
	case DEF_BOOL:
		ob = (OPT_BOOL *)DFLT_OPTS(dflt_tbl, ds);
		if (c == KEY_RETURN ||
			c == cmds(CMDS_CONFIG_VAL_UP) ||
			c == cmds(CMDS_CONFIG_VAL_DN))
		{
			i = ! ob->opts[OPT_OPT];
			dflt_set_opt(dflt_tbl, ds, &i);
			config->setting_changes_made = TRUE;
		}
		break;

	case DEF_NUM:
		oi = (OPT_INT *)DFLT_OPTS(dflt_tbl, ds);
		if (c == cmds(CMDS_CONFIG_VAL_DN))
		{
			i = oi->opts[OPT_OPT];
			if (i)
			{
				i--;
				dflt_set_opt(dflt_tbl, ds, &i);
				config->setting_changes_made = TRUE;
			}
		}
		else if (c == cmds(CMDS_CONFIG_VAL_UP))
		{
			i = oi->opts[OPT_OPT];
			if ((int *)ds->limit && *(int *)ds->limit)
			{
				if (i < *(int *)ds->limit)
				{
					i++;
					dflt_set_opt(dflt_tbl, ds, &i);
					config->setting_changes_made = TRUE;
				}
			}
			else
			{
				i++;
				dflt_set_opt(dflt_tbl, ds, &i);
				config->setting_changes_made = TRUE;
			}
		}
		else if (c == KEY_RETURN)
		{
			i = cfg_get_num(oi->opts[OPT_OPT]);
			if (i == -1)
			{
				rc = -1;
				break;
			}
			if ((int *)ds->limit && *(int *)ds->limit)
			{
				if (i > *(int *)ds->limit)
				{
					i = -1;
					errmsg(ER_IV, "", ERR_ANY);
				}
			}
			if (i == -1)
				break;
			dflt_set_opt(dflt_tbl, ds, &i);
			config->setting_changes_made = TRUE;
		}
		break;

	case DEF_CHAR:
		oi = (OPT_INT *)DFLT_OPTS(dflt_tbl, ds);
		if (c == KEY_RETURN)
		{
			input_str[0] = oi->opts[OPT_OPT];
			input_str[1] = 0;
			i = cfg_get_char(input_str);
			if (i == -1)
			{
				rc = -1;
				break;
			}
			i = *input_str;
			dflt_set_opt(dflt_tbl, ds, &i);
			config->setting_changes_made = TRUE;
		}
		break;

	case DEF_ENUM:
		oi = (OPT_INT *)DFLT_OPTS(dflt_tbl, ds);
		if (c == KEY_RETURN || c == cmds(CMDS_CONFIG_VAL_DN))
		{
			dc = (const DEFC *)ds->limit;
			i = oi->opts[OPT_OPT];
			if (dc[++i].f_str == 0)
				i = 0;
			dflt_set_opt(dflt_tbl, ds, &i);
			config->setting_changes_made = TRUE;
		}
		else if (c == cmds(CMDS_CONFIG_VAL_UP))
		{
			dc = (const DEFC *)ds->limit;
			i = oi->opts[OPT_OPT];
			if (--i < 0)
				for (i=0; dc[i+1].f_str; i++)
					;
			dflt_set_opt(dflt_tbl, ds, &i);
			config->setting_changes_made = TRUE;
		}
		break;

	case DEF_LIST:
		oi = (OPT_INT *)DFLT_OPTS(dflt_tbl, ds);
		if (c == KEY_RETURN || c == cmds(CMDS_CONFIG_VAL_DN))
		{
			bl = DFLT_LPTR(dflt_tbl, ds);
			i = oi->opts[OPT_OPT];
			bp = bnth(bl, ++i);
			if (bp == 0)
				i = 0;
			dflt_set_opt(dflt_tbl, ds, &i);
			config->setting_changes_made = TRUE;
		}
		else if (c == cmds(CMDS_CONFIG_VAL_UP))
		{
			bl = *(BLIST **)ds->limit;
			i = oi->opts[OPT_OPT];
			if (--i < 0)
				i = bcount(bl) - 1;
			dflt_set_opt(dflt_tbl, ds, &i);
			config->setting_changes_made = TRUE;
		}
		break;

	case DEF_COLOR:
		oa = (OPT_ATTR *)DFLT_OPTS(dflt_tbl, ds);
		if (c == KEY_RETURN)
			break;
		a = oa->opts[OPT_OPT];
		f = A_FG_CLRNUM(a);
		b = A_BG_CLRNUM(a);

#define NEXT_COLOR(c)	( ((c) == NUM_COLORS - 1) ? 0              : (c) + 1 )
#define PREV_COLOR(c)	( ((c) == 0             ) ? NUM_COLORS - 1 : (c) - 1 )

		/*----------------------------------------------------------------
		 * Note that we skip over any fg color which matches the bg color
		 * & vice-versa.
		 */
		if (c == cmds(CMDS_CONFIG_VAL_DN))
		{
			f = PREV_COLOR(f);
			if (f == b)
				f = PREV_COLOR(f);
		}
		else if (c == cmds(CMDS_CONFIG_VAL_UP))
		{
			f = NEXT_COLOR(f);
			if (f == b)
				f = NEXT_COLOR(f);
		}
		else if (c == cmds(CMDS_CONFIG_BG_DN))
		{
			b = PREV_COLOR(b);
			if (b == f)
				b = PREV_COLOR(b);
		}
		else if (c == cmds(CMDS_CONFIG_BG_UP))
		{
			b = NEXT_COLOR(b);
			if (b == f)
				b = NEXT_COLOR(b);
		}

		a = A_CLR(f, b);
		dflt_set_opt(dflt_tbl, ds, &a);
		config->display_changes_made = TRUE;
		break;

	case DEF_MONO:
		oa = (OPT_ATTR *)DFLT_OPTS(dflt_tbl, ds);
		if (c == KEY_RETURN)
			break;
		a = oa->opts[OPT_OPT];
		for (i=0; i<NUM_MONO_ATTRS; i++)
		{
			if (a == get_attr_value_by_num(i))
				break;
		}

		if (i < NUM_MONO_ATTRS)
		{
			if (c == cmds(CMDS_CONFIG_VAL_DN))
			{
				if (--i < 0)
					i = NUM_MONO_ATTRS - 1;
			}
			else if (c == cmds(CMDS_CONFIG_VAL_UP))
			{
				if (++i == NUM_MONO_ATTRS)
					i = 0;
			}

			a = get_attr_value_by_num(i);
			dflt_set_opt(dflt_tbl, ds, &a);
			config->display_changes_made = TRUE;
		}
		break;

	case DEF_STR:
		os = (OPT_STR *)DFLT_OPTS(dflt_tbl, ds);
		if (c == KEY_RETURN)
		{
			strcpy(input_str, os->opts[OPT_OPT]);
			i = cfg_get_str(input_str);
			if (i == -1)
			{
				rc = -1;
				break;
			}
			dflt_set_opt(dflt_tbl, ds, input_str);
			config->setting_changes_made = TRUE;
		}
		break;
	}

	/*--------------------------------------------------------------------
	 * redisplay entry
	 */
	cfg_display_line(config, line, ON);
	cfg_prompt(line);
	wrefresh(gbl(win_config));
	cfg_bang(config);

	return (rc);
}

/*------------------------------------------------------------------------
 * cfg_process_menu() - process a menu
 */
static int cfg_process_menu (CONFIG_INFO *config, const CFG_MENU *menu,
	int *cur_ent)
{
	int c;

	while (TRUE)
	{
		c = cfg_get_input(config);
		c = cfg_nav(config, menu, cur_ent, c);
		if (c > 0)
			break;

		if (c == 0)
		{
			config->item_changed = FALSE;
			cfg_bang(config);
		}
	}

	return (c);
}

/*------------------------------------------------------------------------
 * cfg_display_main_menu() - display the main menu
 */
static void cfg_display_main_menu (CONFIG_INFO *config)
{
	char *m;

	werase(gbl(win_config));
	config_border();
	wmove(gbl(win_config), 1, 2);
	waddstr(gbl(win_config), package_name());
	waddch(gbl(win_config), ' ');
	waddstr(gbl(win_config), msgs(m_config_title));
	m = msgs(m_config_main);
	wmove(gbl(win_config), 1, getmaxx(gbl(win_config))-2-display_len(m));
	xiaddstr(gbl(win_config), m);
	wrefresh(gbl(win_config));

	cfg_display_menu(config, cfg_data->main_menu);
}

/*------------------------------------------------------------------------
 * cfg_display_page() - display page info
 */
static void cfg_display_page (int n)
{
	char *m;

	werase(gbl(win_config));
	config_border();
	wmove(gbl(win_config), 1, 2);
	waddstr(gbl(win_config), package_name());
	waddch(gbl(win_config), ' ');
	waddstr(gbl(win_config), msgs(m_config_title));
	m = msgs(m_config_page);
	wmove(gbl(win_config), 1, getmaxx(gbl(win_config))-4-display_len(m));
	xaddstr(gbl(win_config), m);
	waddch(gbl(win_config), ' ');
	waddch(gbl(win_config), '1'+n);
	wrefresh(gbl(win_config));
}

/*------------------------------------------------------------------------
 * cfg_do_menu() - process input for a menu
 */
static void cfg_do_menu (CONFIG_INFO *config, const CFG_MENU **menu_list)
{
	int				c;
	int				old_ent;
	int				old_page;
	char			buff[MAX_PATHLEN];
	int				loop;
	const DEFS *	ds;

	old_page = -1;
	old_ent  = -1;
	config->page_no = 0;
	while (TRUE)
	{
		if (config->page_no != old_page)
		{
			config->menu = menu_list[config->page_no];
			for (config->page_ent=0;
				config->menu[config->page_ent].type == CFG_HDG;
				config->page_ent++)
				;
			cfg_display_page(config->page_no);
			cfg_display_menu(config, config->menu);
			cfg_display_line(config, config->menu + config->page_ent, ON);
			wrefresh(gbl(win_config));
			cfg_prompt(config->menu + config->page_ent);
			config->item_changed = FALSE;
			old_ent = -1;
			old_page = config->page_no;
		}

		for (loop=TRUE; loop; )
		{
			cfg_bang(config);
			c = cfg_process_menu(config, config->menu, &config->page_ent);
			if (config->page_ent != old_ent)
			{
				old_ent = config->page_ent;
				ds = dflt_find_entry(dflt_tbl,
					config->menu[config->page_ent].varptr);
				if (ds != 0)
					dflt_format(dflt_tbl, ds, FALSE, buff);
				else
					*buff = 0;
				config->item_changed = FALSE;
			}
			if (c == KEY_RETURN ||
				c == cmds(CMDS_CONFIG_VAL_UP) ||
				c == cmds(CMDS_CONFIG_VAL_DN) ||
				c == cmds(CMDS_CONFIG_BG_UP) ||
				c == cmds(CMDS_CONFIG_BG_DN))
			{
				if (config->menu[config->page_ent].type == CFG_ITEM)
				{
					if (cfg_change_ent(config,
						config->menu + config->page_ent, c) == 0)
					{
						config->item_changed = TRUE;
					}
				}
				else if (config->menu[config->page_ent].type == CFG_BTN)
				{
					c = config->menu[config->page_ent].selection;
					switch (c)
					{
					case CMDS_CONFIG_NEXT:
						config->page_no++;
						if (menu_list[config->page_no] == 0)
							config->page_no = 0;
						loop = FALSE;
						break;

					case CMDS_CONFIG_PREV:
						config->page_no--;
						if (config->page_no < 0)
						{
							for (config->page_no=0;
								menu_list[config->page_no+1];
								config->page_no++)
								;
						}
						loop = FALSE;
						break;

					case CMDS_CONFIG_MAIN:
						c = KEY_ESCAPE;
						loop = FALSE;
						break;
					}
				}
			}
			else if (c == cmds(CMDS_CONFIG_RESTORE))
			{
				if (config->menu[config->page_ent].type == CFG_ITEM)
				{
					if (config->item_changed)
					{
						config->item_changed = FALSE;
						ds = dflt_find_entry(dflt_tbl,
							config->menu[config->page_ent].varptr);
						if (ds != 0)
							dflt_process_entry(dflt_tbl, ds, buff);
						cfg_display_line(config,
							config->menu + config->page_ent, ON);
						wrefresh(gbl(win_config));
					}
				}
			}
			else if (c == cmds(CMDS_CONFIG_DEFAULT))
			{
				if (config->menu[config->page_ent].type == CFG_ITEM)
				{
					ds = dflt_find_entry(dflt_tbl,
						config->menu[config->page_ent].varptr);
					if (ds != 0 && ! dflt_is_opt_default(dflt_tbl, ds))
					{
						dflt_restore_opt_entry(dflt_tbl, ds);
						config->item_changed = TRUE;
						cfg_display_line(config,
							config->menu + config->page_ent, ON);
						wrefresh(gbl(win_config));
					}
				}
			}

			if (c == KEY_ESCAPE)
				break;
		}

		if (c == KEY_ESCAPE)
			break;
	}
}

/*------------------------------------------------------------------------
 * cfg_read_file() - read a config file
 */
static void cfg_read_file (CONFIG_INFO *config)
{
	char pathname[MAX_PATHLEN];

	if (cfg_get_fn(config) == 0)
	{
		strcpy(pathname, config->cfg_fn);
		fn_resolve_pathname(pathname);
		if (dflt_read(dflt_tbl, pathname, gbl(pgm_path)) == 0)
		{
			errsys(ER_COF);
		}
		else
		{
			config->setting_changes_made = FALSE;
			config->display_changes_made = FALSE;
		}
	}
}

/*------------------------------------------------------------------------
 * cfg_write_file() - write a config file
 */
static int cfg_write_file (CONFIG_INFO *config)
{
	char pathname[MAX_PATHLEN];
	int c;
	int rc;
	struct stat stbuf;

	rc = cfg_get_fn(config);
	if (rc == 0)
	{
		char cfg_dir[MAX_PATHLEN];

		strcpy(pathname, config->cfg_fn);
		fn_resolve_pathname(pathname);
		if (os_stat(pathname, &stbuf) == 0)
		{
			c = errmsg(ER_FER, "", ERR_YESNO);
			if (c)
				return (c);
		}

		fn_dirname(pathname, cfg_dir);
		if (os_dir_make(cfg_dir) < 0)
		{
			c = errsys(ER_COOF);
			return (c);
		}

		if (dflt_write(dflt_tbl, pathname, cmdopt(save_all_defaults)))
		{
			c = errsys(ER_COOF);
			return (c);
		}
		else
		{
			config->setting_changes_made = FALSE;
			config->display_changes_made = FALSE;
			check_the_file(pathname);
		}
	}
	return (rc);
}

/*------------------------------------------------------------------------
 * cfg_main_menu() - main config routine
 */
static void cfg_main_menu (CONFIG_INFO *config)
{
	int c;
	int i;

	gbl(scr_in_config) = TRUE;

	/*--------------------------------------------------------------------
	 * initialize config struct
	 */
	config->setting_changes_made	= FALSE;
	config->display_changes_made	= FALSE;
	config->item_changed			= FALSE;
	config->screen_changed			= FALSE;

	config->menu_type				= CFG_TYPE_MAIN;
	config->main_ent				= 0;
	config->page_ent				= 0;
	config->page_no					= 0;

	switch (opt(screen_type))
	{
	case screen_auto:	config->setting_color = has_colors();	break;
	case screen_color:	config->setting_color = TRUE;			break;
	case screen_mono:	config->setting_color = FALSE;			break;
	}

	/*--------------------------------------------------------------------
	 * get max width for all main entries
	 * & center them based on longest.
	 */
	config->main_width = 0;
	for (i=0; cfg_data->main_menu[i].type; i++)
	{
		if (cfg_data->main_menu[i].type == CFG_MAIN)
		{
			int l = display_len(msgs((int)cfg_data->main_menu[i].varptr));

			if (l > config->main_width)
				config->main_width = l;
		}
	}

	config->main_indent =
		(getmaxx(gbl(win_config))-(config->main_width+1)) / 2;

	/*--------------------------------------------------------------------
	 * do initial display
	 */
	cfg_display_main_menu(config);
	cfg_display_line(config, cfg_data->main_menu + config->main_ent, ON);

	/*--------------------------------------------------------------------
	 * now process all input
	 */
	while (TRUE)
	{
		cfg_prompt(&cfg_data->main_menu[config->main_ent]);
		cfg_bang(config);
		wrefresh(gbl(win_config));

		c = cfg_process_menu(config, cfg_data->main_menu, &config->main_ent);

		if (c == KEY_RETURN || c == KEY_ESCAPE)
		{
			if (c == KEY_RETURN)
				c = cfg_data->main_menu[config->main_ent].selection;

			switch (c)
			{
			case '1':
				config->menu_type = CFG_TYPE_SETTINGS;
				cfg_do_menu(config, cfg_data->settings);
				config->menu_type = CFG_TYPE_MAIN;
				cfg_display_main_menu(config);
				cfg_display_line(config,
					cfg_data->main_menu + config->main_ent, ON);
				cfg_bang(config);
				break;

			case '2':
				config->menu_type = CFG_TYPE_DISPLAY;
				cfg_do_menu(config, config->setting_color ?
					cfg_data->colors : cfg_data->monos);
				config->menu_type = CFG_TYPE_MAIN;
				setup_window_attributes();
				cfg_redraw();
				break;

			case '3':
				cfg_read_file(config);
				break;

			case '4':
				cfg_write_file(config);
				break;

			case '5':
				dflt_restore_opts(dflt_tbl);
				config->setting_changes_made = FALSE;
				config->display_changes_made = FALSE;
				setup_window_attributes();
				cfg_redraw();
				break;

			case '6':
				dflt_init_opts(dflt_tbl);
				config->setting_changes_made = TRUE;
				config->display_changes_made = TRUE;
				setup_window_attributes();
				cfg_redraw();
				break;

			case '7':
				if (cfg_write_file(config) == -1)
					break;
				/*FALLTHROUGH*/

			case '8':
				if (opt(prompt_for_quit) &&
					(config->setting_changes_made ||
					 config->display_changes_made))
				{
					c = errmsg(ER_CMQA, "", ERR_YESNO);
					if (c == 0)
						c = KEY_ESCAPE;
				}
				else
				{
					c = KEY_ESCAPE;
				}
				break;

			case KEY_ESCAPE:
				dflt_restore_opts(dflt_tbl);
				setup_window_attributes();
				config->setting_changes_made = FALSE;
				config->display_changes_made = FALSE;
				break;
			}
		}

		if (c == KEY_ESCAPE)
			break;
	}

	gbl(scr_in_config) = FALSE;
}

/*------------------------------------------------------------------------
 * config() - config processing entry point
 */
void config (void)
{
	CONFIG_INFO *	config	= &gbl(config_info);

	/*--------------------------------------------------------------------
	 * turn off the clock
	 */
	win_clock_set(FALSE);

	/*--------------------------------------------------------------------
	 * save all entries in defaults table
	 */
	dflt_save_opts(dflt_tbl);

	/*--------------------------------------------------------------------
	 * do config processing
	 */
	cfg_main_menu(config);

	/*--------------------------------------------------------------------
	 * set display back to where we were
	 */
	setup_display();
}

/*------------------------------------------------------------------------
 * cfg_redraw() - redisplay the config menu
 */
void cfg_redraw (void)
{
	CONFIG_INFO *	config = &gbl(config_info);

	switch (config->menu_type)
	{
	case CFG_TYPE_MAIN:
		/*----------------------------------------------------------------
		 * main menu
		 */
		config->menu = cfg_data->main_menu;
		cfg_display_main_menu(config);
		cfg_display_line(config, config->menu + config->main_ent, ON);
		break;

	case CFG_TYPE_DISPLAY:
		/*----------------------------------------------------------------
		 * screen display menu
		 */
		if (config->setting_color)
			config->menu = cfg_data->colors[config->page_no];
		else
			config->menu = cfg_data->monos[config->page_no];
		cfg_display_page(config->page_no);
		cfg_display_menu(config, config->menu);
		cfg_display_line(config, config->menu + config->page_ent, ON);
		break;

	case CFG_TYPE_SETTINGS:
		/*----------------------------------------------------------------
		 * settings menu
		 */
		config->menu = cfg_data->settings[config->page_no];
		cfg_display_page(config->page_no);
		cfg_display_menu(config, config->menu);
		cfg_display_line(config, config->menu + config->page_ent, ON);
		break;
	}

	wrefresh(gbl(win_config));
	cfg_prompt(config->menu + config->page_ent);
	cfg_bang(config);
}

/*------------------------------------------------------------------------
 * cfg_lang() - called when the language is changed
 */
int cfg_lang (const void *valp)
{
	int old_lang	= get_cur_lang();
	int num			= *(const int *)valp;
	int rc;

	/*--------------------------------------------------------------------
	 * If defaults are not setup, then "num" is a bogus value,
	 * and it must be set to the current language used.
	 */
	if (! gbl(pgm_dflts_setup))
		num = old_lang;

	/*--------------------------------------------------------------------
	 * check if new language same as old
	 */
	if (num == get_cur_lang())
		return (num);

	/*--------------------------------------------------------------------
	 * select new language
	 */
	rc = select_lang_by_num(num);
	if (rc)
		return (-1);

	/*--------------------------------------------------------------------
	 * set all defaults from new resource file
	 */
	dflt_init_defs(dflt_tbl, TRUE);

	/*--------------------------------------------------------------------
	 * if in config, redraw screen
	 */
	if (gbl(scr_in_config))
		cfg_redraw();

	return (num);
}

/*------------------------------------------------------------------------
 * cfg_screen() - called when screen type changes
 */
int cfg_screen (const void *valp)
{
	if (gbl(scr_in_config))
	{
		CONFIG_INFO *	config = &gbl(config_info);

		if (valp == 0)
			config->acs_changed = TRUE;
		else
			config->screen_changed = TRUE;
	}

	return (0);
}
