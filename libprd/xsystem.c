/*------------------------------------------------------------------------
 * execute sub-tasks
 */
#include "libprd.h"

static int our_system (const char *dir, const char *cmd)
{
	int rc;

	rc = exec_cmd(dir, cmd, TRUE, NULL);

	ungetch(KEY_SHIFT_RELEASE);
	ungetch(KEY_ALT_RELEASE);

	return (rc);
}

static int our_exec (const char *dir, char **argv)
{
	int rc;

	rc = exec_argv(dir, argv, TRUE, NULL);

	ungetch(KEY_SHIFT_RELEASE);
	ungetch(KEY_ALT_RELEASE);

	return (rc);
}

static void xexec (char **argv)
{
	int save_attr;

	win_clock_set(FALSE);
	save_attr = wattrget(stdscr);
	wattrset(stdscr, gbl(scr_escape_attr));
	wclear(stdscr);
	wmove(stdscr, 0, 0);
	wrefresh(stdscr);
	wattrset(stdscr, save_attr);

	our_exec(gbl(scr_cur)->path_name, argv);

	anykey_msg("");
	wclear(stdscr);
	wrefresh(stdscr);
	setup_display();
	disp_cmds();
}

void xsystem (const char *cmd, const char *dir, int wait_mode)
{
	int save_attr;

	if (dir == 0 || *dir == 0)
		dir = gbl(scr_cur)->path_name;

	win_clock_set(FALSE);
	save_attr = wattrget(stdscr);
	wattrset(stdscr, gbl(scr_escape_attr));
	wclear(stdscr);
	wmove(stdscr, 0, 0);
	wrefresh(stdscr);
	wattrset(stdscr, save_attr);

	our_system(dir, cmd);

	if (wait_mode)
	{
		wmove(gbl(win_message), 0, 0);
		werase(gbl(win_message));
		anykey();
	}

	wclear(stdscr);
	wrefresh(stdscr);

	setup_display();
	if (! gbl(scr_in_config) && ! gbl(scr_in_menu))
		disp_cmds();
}

void do_exec (void)
{
	char cmd[MAX_FILELEN+MAX_PATHLEN+1];
	char input_str[MAX_PATHLEN];
	int i;
	char *p;

	bang(msgs(m_xsystem_entcmd));
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	p = msgs(m_xsystem_xcmd);
	xaddstr(gbl(win_commands), p);
	wrefresh(gbl(win_commands));
	if (can_we_exec(&(gbl(scr_cur)->cur_file->stbuf)))
		strcpy(input_str, FULLNAME(gbl(scr_cur)->cur_file));
	else
		*input_str = 0;
	i = xgetstr(gbl(win_commands), input_str, XGL_EXEC, MAX_PATHLEN, 0,
		XG_STRING);
	if (i < 0)
	{
		disp_cmds();
		return;
	}
	if (i == 0)
	{
		xsystem("", "", FALSE);
		return;
	}
	expand_cmd(input_str, cmd);
	xsystem(cmd, "", TRUE);
}

void do_tag_exec (void)
{
	char input_str[MAX_PATHLEN];
	int c;

	if (check_tag_count())
		return;
	bang(msgs(m_xsystem_entctx));
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_xsystem_xcwatf));
	wmove(gbl(win_commands), 1, 0);
	waddstr(gbl(win_commands), msgs(m_xsystem_cmd));
	wrefresh(gbl(win_commands));
	*input_str = 0;
	c = xgetstr(gbl(win_commands), input_str, XGL_CMD, MAX_PATHLEN, 0,
		XG_STRING);
	if (c <= 0)
		return;
	exec_tag_args(input_str);
}

void exec_tag_args (const char *cmd)
{
	char path[MAX_PATHLEN];
	BLIST *b;
	FBLK *f;
	char **argv;
	const char *s;
	int c;
	int i;
	int l;
	int j;

	c = 0;				/* count number of args in cmd */
	for (s=cmd; *s; s++)
	{
		if (isspace(*s))
			continue;
		c++;
		/*	note - if we count a few too many, no big thing */
		for (s++; *s; s++)
			if (isspace(*s))
				break;
	}
	c += gbl(scr_cur)->dir_tagged_count+1;
	argv = (char **) MALLOC((unsigned)c*sizeof(char *));
	if (!argv)
	{
		errmsg(ER_IM, "", ERR_ANY);
		disp_cmds();
		return;
	}
	c = 0;
	for (s=cmd; *s; s++)
	{
		if (isspace(*s))
			continue;
		if (*s == '\'' || *s == '\"')
		{
			j = 1;
			for (l=1; *(s+l); l++)
				if (*(s+l) == *s)
				{
					s++;
					l--;
					break;
				}
		}
		else
		{
			j = 0;
			for (l=1; *(s+l); l++)
				if (isspace(*(s+l)))
					break;
		}
		argv[c] = (char *) MALLOC((unsigned)l+1);
		for (i=0; i<l; i++)
			argv[c][i] = *(s+i);
		argv[c][i] = 0;
		s += l+j-1;
		c++;
	}
	for (b=gbl(scr_cur)->first_file; b; b=bnext(b))
	{
		f = (FBLK *)bid(b);
		if (is_file_tagged(f))
		{
			if (gbl(scr_cur)->command_mode == m_file)
				strcpy(path, FULLNAME(f));
			else
				fblk_to_pathname(f, path);
			argv[c] = (char *)MALLOC((unsigned)strlen(path)+1);
			for (i=0; path[i]; i++)
				argv[c][i] = path[i];
			argv[c][i] = 0;
			c++;
		}
	}
	argv[c] = 0;
	xexec(argv);
	for (i=0; argv[i]; i++)
		FREE(argv[i]);
	FREE((void *)argv);
}
