/*------------------------------------------------------------------------
 * process the "available" cmd
 */
#include "libprd.h"

void do_available (void)
{
	MOUNT_INFO *m;
	const char *p;
	int c;
	long space_avail;
	int num;
	int n;
	char dbuf[12];

	/* update mount table if changed */
	if (! cmdopt(skip_mnt_table))
		gbl(mount_list) = ml_get_mounts(gbl(mount_list));

	/* update drive info box */

	disp_drive_info(0, 1);

	/* get number of mounts */

	num = ml_get_num_mounts(gbl(mount_list));
	if (num == 0)
	{
		anykey_msg(msgs(m_avail_nomnts));
		return;
	}

	/* find mount entry for currently displayed drive */

	n = get_cur_dev_ent();

	while (TRUE)
	{
		m = ml_get_mount_ent_by_num(gbl(mount_list), n);

		werase(gbl(win_commands));
		wmove(gbl(win_commands), 0, 0);
		waddstr(gbl(win_commands), msgs(m_avail_dev));
		waddstr(gbl(win_commands), m->mt_dev);
		waddstr(gbl(win_commands), msgs(m_avail_mou));
		waddstr(gbl(win_commands), m->mt_dir);
		waddstr(gbl(win_commands), " ");
		space_avail = ml_get_free_by_dev(gbl(mount_list), m->st_dev);
		if (space_avail == -1)
		{
			p = "???";
		}
		else
		{
			p = xform(dbuf, space_avail);
			for (; *p == ' '; p++)
				;
		}
		waddstr(gbl(win_commands), p);
		waddstr(gbl(win_commands), msgs(m_avail_bytfre));
		wrefresh(gbl(win_commands));

		werase(gbl(win_message));
		esc_msg();
		bangnc(msgs(m_avail_anykey));
		while (TRUE)
		{
			c = xgetch(gbl(win_message));
			if (c != KEY_MOUSE)
				break;
		}
		bang("");

		if (c == KEY_ESCAPE || c == KEY_BS)
			break;

		if (num <= 1)		/* if only one, get out */
			break;

		if (c == KEY_UP || c == KEY_LEFT)
		{
			n--;
			if (n < 0)
				n = num - 1;
		}
		else
		{
			n++;
			if (n >= num)
				n = 0;
		}
	}
	disp_cmds();
}
