/*------------------------------------------------------------------------
 * process the "info" cmd
 */
#include "libprd.h"

void show_file_info (const char *name, off_t size, time_t mtime,
	int mode, int uid, int gid)
{
	char ubuf[16];
	char gbuf[16];
	char pbuf[16];
	char dbuf[16];
	char dl[24];
	char *user;
	char *group;
	int i;
	int l;

	user  = get_owner(uid, ubuf);
	group = get_group(gid, gbuf);

	if (size < 0)
		l = 46;
	else
		l = 58;
	i = getmaxx(gbl(win_commands)) - getcurx(gbl(win_commands)) - l;
	disp_fw_str(gbl(win_commands), name, i);
	if (size >= 0)
	{
		waddch(gbl(win_commands), ' ');					/*  1 byte  */
		waddstr(gbl(win_commands), xform(dbuf, size));	/* 11 bytes */
	}
	waddch(gbl(win_commands), ' ');						/*  1 byte  */
	waddstr(gbl(win_commands), date_to_str(dl, mtime));	/* 17 bytes */
	waddch(gbl(win_commands), ' ');						/*  1 byte  */
	waddstr(gbl(win_commands), perm_str(mode, pbuf));	/*  9 bytes */
	waddch(gbl(win_commands), ' ');						/*  1 byte  */
	waddstr(gbl(win_commands), user);					/*  8 bytes */
	waddch(gbl(win_commands), ' ');						/*  1 byte  */
	waddstr(gbl(win_commands), group);					/*  8 bytes */
	wrefresh(gbl(win_commands));						/* 58 bytes total */
}

static void show_file_info_2 (const char *name, off_t size, time_t mtime,
	int mode, int uid, int gid, char *sym_name)
{
	char ubuf[16];
	char gbuf[16];
	char pbuf[16];
	char dbuf[16];
	char dl[24];
	char *user;
	char *group;

	user  = get_owner(uid, ubuf);
	group = get_group(gid, gbuf);

	werase(gbl(win_commands));

	wmove(gbl(win_commands), 0, 0);
	waddstr(gbl(win_commands), name);
	if (sym_name != 0)
	{
		waddstr(gbl(win_commands), " -> ");
		waddstr(gbl(win_commands), sym_name);
	}

	wmove(gbl(win_commands), 1, 0);
	waddstr(gbl(win_commands), xform(dbuf, size));
	waddch(gbl(win_commands), ' ');
	waddstr(gbl(win_commands), date_to_str(dl, mtime));
	waddch(gbl(win_commands), ' ');
	waddstr(gbl(win_commands), perm_str(mode, pbuf));
	waddch(gbl(win_commands), ' ');
	waddstr(gbl(win_commands), user);
	waddch(gbl(win_commands), ' ');
	waddstr(gbl(win_commands), group);

	wrefresh(gbl(win_commands));
}

void disp_cur_file_info (void)
{
	FBLK *f = gbl(scr_cur)->cur_file;

	show_file_info_2(FULLNAME(f),
		f->stbuf.st_size,
		f->stbuf.st_mtime,
		f->stbuf.st_mode,
		f->stbuf.st_uid,
		f->stbuf.st_gid,
		f->sym_name);

	anykey_msg("");
	disp_cmds();
}

void disp_cur_dir_info (void)
{
	FBLK *f = gbl(scr_cur)->cur_file;

	show_file_info_2(FULLNAME(f),
		f->stbuf.st_size,
		f->stbuf.st_mtime,
		f->stbuf.st_mode,
		f->stbuf.st_uid,
		f->stbuf.st_gid,
		NULL);

	anykey_msg("");
	disp_cmds();
}
