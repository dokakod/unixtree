/*------------------------------------------------------------------------
 * process the "edit" & "view" cmds
 */
#include "libprd.h"

static void do_hexeditor (const char *name)
{
	struct stat stbuf;

	if (os_stat(name, &stbuf) == 0)
	{
		if (! S_ISLNK(stbuf.st_mode))
		{
			if (! S_ISREG(stbuf.st_mode))
			{
				errmsg(ER_CESF, "", ERR_ANY);
				return;
			}
		}

		if (! can_we_read(&stbuf))
		{
			errmsg(ER_NPTRF, "", ERR_ANY);
			return;
		}

		if (! can_we_write(&stbuf))
		{
			errmsg(ER_NPTWF, "", ERR_ANY);
			return;
		}
	}
	else
	{
		errmsg(ER_CSN, "", ERR_ANY);
		return;
	}

	if (*opt(hex_editor) == 0)
	{
		hexedit_run(name);
		disp_cmds();
	}
	else
	{
		char cmd[BUFSIZ];

		/* edit the file */

		strcpy(cmd, opt(hex_editor));
		strcat(cmd, " \"");
		strcat(cmd, name);
		strcat(cmd, "\"");

		xsystem(cmd, "", FALSE);

		if (gbl(scr_in_autoview) || gbl(scr_in_fullview))
			open_av_file(gbl(scr_cur)->cur_file);
		setup_display();
	}
}

void do_hexedit (FBLK *f)
{
	char edit_buffer[MAX_PATHLEN];
	fblk_to_pathname(f, edit_buffer);

	do_hexeditor(edit_buffer);
}

static void do_edit (const char *name)
{
	char edit_buffer[MAX_PATHLEN];
	char input_str[MAX_PATHLEN];
	char cmd[BUFSIZ];
	struct stat stbuf;
	int c;
	char *p;

	if (!name || !*name)
	{
		xsystem(opt(editor), "", FALSE);
		return;
	}

	if (opt(prompt_for_edit))
	{
		bang(msgs(m_edit_entfil));
		werase(gbl(win_commands));
		wmove(gbl(win_commands), 0, 0);
		xaddstr(gbl(win_commands), msgs(m_edit_edit));
		wrefresh(gbl(win_commands));
		strcpy(input_str, name);
		c = xgetstr(gbl(win_commands), input_str, XGL_EDIT_FILE, MAX_PATHLEN,
			0, XG_FILEPATH);
		if (c <= 0)
		{
			disp_cmds();
			return;
		}
	}
	else
	{
		strcpy(input_str, name);
	}
	fn_get_abs_path(gbl(scr_cur)->path_name, input_str, edit_buffer);
	if (os_stat(edit_buffer, &stbuf) == 0)
	{
		if (! S_ISLNK(stbuf.st_mode))
		{
			if (! S_ISREG(stbuf.st_mode))
			{
				errmsg(ER_CESF, "", ERR_ANY);
				disp_cmds();
				return;
			}
		}
		if (!can_we_read(&stbuf))
		{
			errmsg(ER_NPTRF, "", ERR_ANY);
			disp_cmds();
			return;
		}
		if (opt(prompt_for_edit) && !can_we_write(&stbuf))
		{
			c = errmsg(ER_NPTWF, "", ERR_YESNO);
			if (c)
			{
				disp_cmds();
				return;
			}
		}
	}

	/* edit the file */

	strcpy(cmd, opt(editor));
	strcat(cmd, " \"");
	strcat(cmd, edit_buffer);
	strcat(cmd, "\"");
	xsystem(cmd, "", FALSE);

	check_the_file(edit_buffer);
	p = fn_basename(edit_buffer);

	/* check any .bak file */

	if ((int)strlen(p) > MAX_FILELEN-5)
		p[MAX_FILELEN-5] = 0;
	strcat(p, ".bak");
	check_the_file(edit_buffer);
}

void do_editor (FBLK *f)
{
	const char *name;

	if (f)
		name = FULLNAME(f);
	else
		name = "";

	do_edit(name);
}

void do_viewer (FBLK *f)
{
	char edit_buffer[MAX_PATHLEN];
	struct stat stbuf;
	int i;

	strcpy(edit_buffer, gbl(scr_cur)->path_name);
	fn_append_filename_to_dir(edit_buffer, FULLNAME(f));

	(gbl(scr_cur)->vfcb)->v_mode &= ~V_SYM;
	if (S_ISLNK(f->stbuf.st_mode))
	{
		(gbl(scr_cur)->vfcb)->v_mode |= V_SYM;
		i = os_lstat(edit_buffer, &stbuf);
	}
	else
	{
		i = os_stat(edit_buffer, &stbuf);
	}

	if (i == 0)
	{
		if (! S_ISLNK(stbuf.st_mode))
		{
			if (! S_ISREG(stbuf.st_mode))
			{
				errmsg(ER_CVSF, "", ERR_ANY);
				disp_cmds();
				return;
			}
		}
		if (!can_we_read(&stbuf))
		{
			errmsg(ER_NPTRF, "", ERR_ANY);
			disp_cmds();
			return;
		}
	}
	else
	{
		errmsg(ER_FDNE, "", ERR_ANY);
		disp_cmds();
		return;
	}

	/* view the file */

	view_file(edit_buffer);
}

static void setup_view (const char *filename)
{
	VFCB *v;

	v = gbl(scr_cur)->vfcb;
	gbl(scr_in_fullview) = TRUE;
	gbl(scr_cur)->vfcb = gbl(vfcb_fv);
	(gbl(scr_cur)->vfcb)->v_mode &= ~V_SYM;
	if (v->v_mode & V_SYM)
		(gbl(scr_cur)->vfcb)->v_mode |= V_SYM;
	strcpy((gbl(scr_cur)->vfcb)->v_pathname, filename);
	view_command(gbl(scr_cur)->vfcb, V_CMD_OPEN_RO);
	if (gbl(scr_in_autoview))
	{
		(gbl(scr_cur)->vfcb)->v_top_pos = v->v_top_pos;
		(gbl(scr_cur)->vfcb)->v_mode =
			(v->v_mode & ~V_VSCROLLBAR) | V_HSCROLLBAR;
	}
	gbl(scr_cur)->cur_path_line   = gbl(win_av_path_line_reg);
	gbl(scr_cur)->cur_av_mode_win = gbl(win_av_mode_reg);
	setup_display();
}

void view_file (const char *filename)
{
	char cmd[BUFSIZ];

	if (*opt(viewer) != 0)
	{
		/* use specified view program */

		strcpy(cmd, opt(viewer));
		strcat(cmd, " ");
		strcat(cmd, "\"");
		strcat(cmd, filename);
		strcat(cmd, "\"");

		xsystem(cmd, "", TRUE);
	}
	else
	{
		/* use internal view program */

		setup_view(filename);
	}
}

void do_tag_viewer (void)
{
	char edit_buffer[MAX_PATHLEN];
	FBLK *f;

	if (check_tag_count())
		return;

	if (*opt(viewer) != 0)
	{
		exec_tag_args(opt(viewer));
	}
	else
	{
		/* in avcmds.c, view_list is bumped if next cmd received */

		f = 0;
		for (gbl(view_list)=gbl(scr_cur)->first_file; gbl(view_list);
			gbl(view_list)=bnext(gbl(view_list)))
		{
			f = (FBLK *)bid(gbl(view_list));
			if (is_file_tagged(f))
				break;
		}
		fblk_to_pathname(f, edit_buffer);
		(gbl(scr_cur)->vfcb)->v_mode &= ~V_SYM;
		if (S_ISLNK(f->stbuf.st_mode))
			(gbl(scr_cur)->vfcb)->v_mode |= V_SYM;
		setup_view(edit_buffer);
	}
}
