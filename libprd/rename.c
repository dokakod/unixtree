/*------------------------------------------------------------------------
 * process the "rename" cmd
 */
#include "libprd.h"

struct rename_info
{
	char pattern[MAX_PATHLEN];
};
typedef struct rename_info RENAME_INFO;

static int rename_cur_file (void *data)
{
	RENAME_INFO *	ri = (RENAME_INFO *)data;
	char new_name[MAX_PATHLEN];
	char old_name[MAX_PATHLEN];
	char buffer[MAX_PATHLEN];
	int c;

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_rename_ren2));
	zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_file));
	waddstr(gbl(win_commands), msgs(m_rename_as1));
	zaddstr(gbl(win_commands), buffer);
	wrefresh(gbl(win_commands));
	make_file_name(ri->pattern, FULLNAME(gbl(scr_cur)->cur_file), buffer);
	fblk_to_dirname((gbl(scr_cur)->cur_file), old_name);
	strcpy(new_name, old_name);
	fn_append_filename_to_dir(old_name, FULLNAME(gbl(scr_cur)->cur_file));
	fn_append_filename_to_dir(new_name, buffer);
	if (os_file_rename(old_name, new_name))
	{
		c = errsys(ER_CRF);
		return (c);
	}
	fblk_change_name(gbl(scr_cur)->cur_file, buffer);
	hilite_file(ON);
	disp_cur_file();
	update_dir((gbl(scr_cur)->cur_file)->dir);
	return (1);				/* for traverse() */
}

int do_rename (void)
{
	RENAME_INFO		rename_info;
	RENAME_INFO *	ri = &rename_info;
	int i;
	char *p, *q;

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	p = msgs(m_rename_ren1);
	q = msgs(m_rename_as2);
	xaddstr(gbl(win_commands), p);
	zaddstr(gbl(win_commands), FULLNAME(gbl(scr_cur)->cur_file));
	bang(msgs(m_rename_efsfn));
	wmove(gbl(win_commands), 1, display_len(p)-display_len(q));
	waddstr(gbl(win_commands), q);
	wrefresh(gbl(win_commands));
	strcpy(ri->pattern, FULLNAME(gbl(scr_cur)->cur_file));
	i = xgetstr(gbl(win_commands), ri->pattern, XGL_PATTERN, MAX_FILELEN, 0,
		XG_FILENAME);
	if (i <= 0)
	{
		disp_cmds();
		return (0);
	}

	rename_cur_file(ri);
	disp_cmds();
	return (0);
}

void make_file_name (const char *tplate, const char *origname, char *newname)
{
    char *result;

    if (!tplate)
        tplate = fn_all();
    if (!origname)
        origname = "";
    result = newname;

	while (*tplate)
	{
		if (*tplate == '?')
		{
			*result++ = *origname++;
			tplate++;
		}
		else if (*tplate != '*')
		{
			*result++ = *tplate++;
			origname++;
		}
		else
		{
			tplate++;
			while (*origname)
			{
				if (*origname == *tplate)
					break;
				*result++ = *origname++;
			}
		}
	}
    *result = 0;
}

void do_tag_rename (void)
{
	RENAME_INFO		rename_info;
	RENAME_INFO *	ri = &rename_info;
	int i;

	if (check_tag_count())
		return;

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	xaddstr(gbl(win_commands), msgs(m_rename_ratf));
	bang(msgs(m_rename_efsfn));
	wmove(gbl(win_commands), 1, 0);
	waddstr(gbl(win_commands), msgs(m_rename_as2));
	wrefresh(gbl(win_commands));
	*ri->pattern = 0;
	i = xgetstr(gbl(win_commands), ri->pattern, XGL_PATTERN, MAX_FILELEN, 0,
		XG_FILENAME);
	if (i <= 0)
		return;

	traverse(rename_cur_file, ri);
}
