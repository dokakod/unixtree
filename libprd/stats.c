/*------------------------------------------------------------------------
 * display stats windows
 */
#include "libprd.h"

int get_cur_dev_ent (void)
{
	int num;
	int n;

	/* get number of entries in mount list */

	num = ml_get_num_mounts(gbl(mount_list));

	/* find mount entry for currently displayed drive */

	for (n=0; n<num; n++)
	{
		MOUNT_INFO *m;

		m = ml_get_mount_ent_by_num(gbl(mount_list), n);
		if (m->st_dev == gbl(scr_cur_dev))
			return (n);
	}

	return (-1);
}

static void disp_info_file_spec (WINDOW *win_info)
{
	wmove(win_info, 0, 1);
	waddstr(win_info, msgs(m_stats_fil1));
	wstandout(win_info);
	waddstr(win_info, gbl(scr_cur)->file_spec);
	wstandend(win_info);
}

static void disp_info_disk_box (WINDOW *win_info)
{
	MOUNT_INFO *v;
	char dbuf[12];

	wmove(win_info, 2, 1);
	waddstr(win_info, msgs(m_stats_disk));
	wstandout(win_info);
	v = ml_get_mount_ent_by_dev(gbl(mount_list), gbl(scr_cur_dev));
	if (v)
	{
		if (opt(show_mnt_dev))
			waddstr(win_info, v->mt_dev);
		else
			waddstr(win_info, v->mt_dir);
	}
	else
	{
		waddch(win_info, TO_HEX((gbl(scr_cur_dev) & 0xf000) >> 12));
		waddch(win_info, TO_HEX((gbl(scr_cur_dev) & 0x0f00) >>  8));
		waddch(win_info, TO_HEX((gbl(scr_cur_dev) & 0x00f0) >>  4));
		waddch(win_info, TO_HEX((gbl(scr_cur_dev) & 0x000f)      ));
	}
	wstandend(win_info);

	wmove(win_info, 3, 1);
	waddstr(win_info, msgs(m_stats_fre2));
	wstandout(win_info);
	if (gbl(scr_cur_free) == -1)
		waddstr(win_info, "        ???");
	else
		waddstr(win_info, xform(dbuf, gbl(scr_cur_free)));
	waddch(win_info, 'K');
	wstandend(win_info);
}

static void disp_info_node_stats (WINDOW *win_info)
{
	NBLK *info_root = get_root_of_file(gbl(scr_cur)->cur_file);
	char dbuf[12];

	wmove(win_info, 5, 1);
	wstandout(win_info);
	waddstr(win_info, msgs(m_stats_nodst));
	wstandend(win_info);

	wmove(win_info, 6, 1);
	waddstr(win_info, msgs(m_stats_tot));

	wmove(win_info, 7, 1);
	waddstr(win_info, msgs(m_stats_fil3));
	wstandout(win_info);
	waddstr(win_info, xform(dbuf, info_root->node_total_count));
	wstandend(win_info);

	wmove(win_info, 8, 1);
	waddstr(win_info, msgs(m_stats_byt));
	wstandout(win_info);
	waddstr(win_info, xform(dbuf, info_root->node_total_bytes));
	wstandend(win_info);

	wmove(win_info, 9, 1);
	waddstr(win_info, msgs(m_stats_mat));

	wmove(win_info, 10, 1);
	waddstr(win_info, msgs(m_stats_fil3));
	wstandout(win_info);
	waddstr(win_info, xform(dbuf, info_root->node_match_count));
	wstandend(win_info);

	wmove(win_info, 11, 1);
	waddstr(win_info, msgs(m_stats_byt));
	wstandout(win_info);
	waddstr(win_info, xform(dbuf, info_root->node_match_bytes));
	wstandend(win_info);

	wmove(win_info, 12, 1);
	waddstr(win_info, msgs(m_stats_tag));

	wmove(win_info, 13, 1);
	waddstr(win_info, msgs(m_stats_fil3));
	wstandout(win_info);
	waddstr(win_info, xform(dbuf, info_root->node_tagged_count));
	wstandend(win_info);

	wmove(win_info, 14, 1);
	if (opt(show_tag_blocks))
	{
		waddstr(win_info, msgs(m_stats_blk));
		wstandout(win_info);
		waddstr(win_info, xform(dbuf, info_root->node_tagged_blocks));
		wstandend(win_info);
	}
	else
	{
		waddstr(win_info, msgs(m_stats_byt));
		wstandout(win_info);
		waddstr(win_info, xform(dbuf, info_root->node_tagged_bytes));
		wstandend(win_info);
	}
}

static void disp_info_dir_stats (WINDOW *win_info)
{
	char dbuf[12];

	wmove(win_info, 5, (getmaxx(win_info) / 2) + 1);
	wstandout(win_info);
	switch (gbl(scr_cur)->command_mode)
	{
	case m_dir:
	case m_file:
		waddstr(win_info, msgs(m_stats_dirst));
		break;

	case m_showall:
	case m_level_showall:
		waddstr(win_info, msgs(m_stats_shost));
		break;

	case m_global:
		waddstr(win_info, msgs(m_stats_glbst));
		break;

	case m_tag_showall:
		waddstr(win_info, msgs(m_stats_shost));
		waddch(win_info, pgm_const(tag_disp_char));
		break;

	case m_tag_global:
		waddstr(win_info, msgs(m_stats_glbst));
		waddch(win_info, pgm_const(tag_disp_char));
		break;
	}
	wstandend(win_info);

	wmove(win_info, 6, (getmaxx(win_info) / 2) + 1);
	waddstr(win_info, msgs(m_stats_tot));

	wmove(win_info, 7, (getmaxx(win_info) / 2) + 1);
	waddstr(win_info, msgs(m_stats_fil3));
	wstandout(win_info);
	waddstr(win_info, xform(dbuf, gbl(scr_cur)->dir_total_count));
	wstandend(win_info);

	wmove(win_info, 8, (getmaxx(win_info) / 2) + 1);
	waddstr(win_info, msgs(m_stats_byt));
	wstandout(win_info);
	waddstr(win_info, xform(dbuf, gbl(scr_cur)->dir_total_bytes));
	wstandend(win_info);

	wmove(win_info, 9, (getmaxx(win_info) / 2) + 1);
	waddstr(win_info, msgs(m_stats_mat));

	wmove(win_info, 10, (getmaxx(win_info) / 2) + 1);
	waddstr(win_info, msgs(m_stats_fil3));
	wstandout(win_info);
	waddstr(win_info, xform(dbuf, gbl(scr_cur)->dir_match_count));
	wstandend(win_info);

	wmove(win_info, 11, (getmaxx(win_info) / 2) + 1);
	waddstr(win_info, msgs(m_stats_byt));
	wstandout(win_info);
	waddstr(win_info, xform(dbuf, gbl(scr_cur)->dir_match_bytes));
	wstandend(win_info);

	wmove(win_info, 12, (getmaxx(win_info) / 2) + 1);
	waddstr(win_info, msgs(m_stats_tag));

	wmove(win_info, 13, (getmaxx(win_info) / 2) + 1);
	waddstr(win_info, msgs(m_stats_fil3));
	wstandout(win_info);
	waddstr(win_info, xform(dbuf, gbl(scr_cur)->dir_tagged_count));
	wstandend(win_info);

	wmove(win_info, 14, (getmaxx(win_info) / 2) + 1);
	if (opt(show_tag_blocks))
	{
		waddstr(win_info, msgs(m_stats_blk));
		wstandout(win_info);
		waddstr(win_info, xform(dbuf, gbl(scr_cur)->dir_tagged_blocks));
		wstandend(win_info);
	}
	else
	{
		waddstr(win_info, msgs(m_stats_byt));
		wstandout(win_info);
		waddstr(win_info, xform(dbuf, gbl(scr_cur)->dir_tagged_bytes));
		wstandend(win_info);
	}
}

static void disp_info_dir_win (WINDOW *win_info)
{
	DBLK *d;
	char dbuf[12];
	char *p;

	if (gbl(scr_cur)->command_mode == m_dir)
	{
		d = gbl(scr_cur)->cur_dir;
	}
	else
	{
		d = gbl(scr_cur)->cur_file->dir;
	}

	if (getmaxy(win_info) > 18)
	{
		wmove(win_info, 16, 1);
		wstandout(win_info);
		disp_fw_str(win_info, FULLNAME(d), 17);
		wstandend(win_info);

		wmove(win_info, 17, 1);
		waddstr(win_info, msgs(m_stats_fil3));
		wstandout(win_info);
		waddstr(win_info, xform(dbuf, bcount((gbl(scr_cur)->cur_dir)->flist)));
		wstandend(win_info);

		wmove(win_info, 18, 1);
		waddstr(win_info, msgs(m_stats_byt));
		wstandout(win_info);
		waddstr(win_info, xform(dbuf, (gbl(scr_cur)->cur_dir)->dir_size));
		wstandend(win_info);
	}
	else
	{
		wmove(win_info, 16, 1);
		wstandout(win_info);
		p = xform3(dbuf, bcount((gbl(scr_cur)->cur_dir)->flist));
		disp_fw_str(win_info, FULLNAME(d), 17 - 1 - strlen(p));
		waddch(win_info, ' ');
		wstandout(win_info);
		waddstr(win_info, p);
		wstandend(win_info);
		wstandend(win_info);

		wmove(win_info, 17, 1);
		waddstr(win_info, msgs(m_stats_byt));
		wstandout(win_info);
		waddstr(win_info, xform(dbuf, (gbl(scr_cur)->cur_dir)->dir_size));
		wstandend(win_info);
	}
}

static void disp_info_file_win (WINDOW *win_info)
{
	char dbuf[12];

	if (getmaxy(win_info) > 18)
	{
		if (gbl(scr_cur)->command_mode != m_dir)
		{
			wmove(win_info, 16, (getmaxx(win_info) / 2) + 1);
			wstandout(win_info);
			disp_fw_str(win_info, FULLNAME(gbl(scr_cur)->cur_file), 17);
			wstandend(win_info);

			wmove(win_info, 18, (getmaxx(win_info) / 2) + 1);
			waddstr(win_info, msgs(m_stats_byt));
			wstandout(win_info);
			xform3(dbuf, (gbl(scr_cur)->cur_file)->stbuf.st_size);
			waddstr(win_info, dbuf);
			wstandend(win_info);
		}
		else
		{
			wmove(win_info, 16, (getmaxx(win_info) / 2) + 1);
			wclrtoeol(win_info);
			wmove(win_info, 18, (getmaxx(win_info) / 2) + 1);
			wclrtoeol(win_info);
		}
	}
	else
	{
		if (gbl(scr_cur)->command_mode != m_dir)
		{
			wmove(win_info, 16, (getmaxx(win_info) / 2) + 1);
			wstandout(win_info);
			disp_fw_str(win_info, FULLNAME(gbl(scr_cur)->cur_file), 17);
			wstandend(win_info);

			wmove(win_info, 17, (getmaxx(win_info) / 2) + 1);
			waddstr(win_info, msgs(m_stats_byt));
			wstandout(win_info);
			waddstr(win_info,
				xform(dbuf, (gbl(scr_cur)->cur_file)->stbuf.st_size));
			wstandend(win_info);
		}
		else
		{
			wmove(win_info, 16, (getmaxx(win_info) / 2) + 1);
			wclrtoeol(win_info);
			wmove(win_info, 17, (getmaxx(win_info) / 2) + 1);
			wclrtoeol(win_info);
		}
	}
}

static int disp_ntype (void)
{
	NBLK *n;
	ABLK *a;

	n = gbl(scr_cur)->cur_root;
	switch (n->node_type)
	{
	case N_FS:
		return (0);

	case N_ARCH:
		a = (ABLK *)n->node_sub_blk;
		waddstr(gbl(scr_cur)->cur_path_line, "<");
		waddstr(gbl(scr_cur)->cur_path_line, a->arch_typename);
		waddch(gbl(scr_cur)->cur_path_line, ':');
		waddstr(gbl(scr_cur)->cur_path_line, a->arch_name);
		waddstr(gbl(scr_cur)->cur_path_line, "> ");
		return (strlen(a->arch_typename)+4+strlen(a->arch_name));

	case N_FTP:
		waddstr(gbl(scr_cur)->cur_path_line, "<");
		waddstr(gbl(scr_cur)->cur_path_line, msgs(m_stats_ftp));
		waddstr(gbl(scr_cur)->cur_path_line, "> ");
		return (strlen(msgs(m_stats_ftp)) + 3);
	}

	return (0);
}

void disp_path_line (void)
{
	char fixed[MAX_PATHLEN];
	char path_name[MAX_PATHLEN];
	int l;
	char *p;
	int node_type = FN_SYSTEM;
	NBLK *n = gbl(scr_cur)->cur_root;

	/* set attributes of window */

	if (gbl(scr_in_fullview) || gbl(scr_in_hexedit))
	{
		wattrset (gbl(scr_cur)->cur_path_line, gbl(scr_av_lolite_attr));
		wstandset(gbl(scr_cur)->cur_path_line, gbl(scr_av_hilite_attr));
	}
	else if (gbl(scr_cur)->command_mode == m_dir ||
			 gbl(scr_cur)->command_mode == m_file)
	{
		wattrset (gbl(scr_cur)->cur_path_line, gbl(scr_reg_lolite_attr));
		wstandset(gbl(scr_cur)->cur_path_line, gbl(scr_reg_hilite_attr));
	}
	else
	{
		wattrset (gbl(scr_cur)->cur_path_line, gbl(scr_sa_lolite_attr));
		wstandset(gbl(scr_cur)->cur_path_line, gbl(scr_sa_hilite_attr));
	}

	werase(gbl(scr_cur)->cur_path_line);
	wmove(gbl(scr_cur)->cur_path_line, 0, 2);

	if (gbl(scr_in_hexedit))
	{
		xaddstr(gbl(scr_cur)->cur_path_line, msgs(m_stats_fil1));
		l = getmaxx(gbl(scr_cur)->cur_path_line) - 3;
		p = gbl(hxcb)->x_dispname;
	}
	else if (gbl(scr_in_fullview))
	{
		xaddstr(gbl(scr_cur)->cur_path_line, msgs(m_stats_fil1));
		l = disp_ntype();
		l += getmaxx(gbl(scr_cur)->cur_path_line) -
			(strlen(msgs(m_stats_fil1))+2);
		if (*(gbl(scr_cur)->vfcb)->v_dispname)
			p = (gbl(scr_cur)->vfcb)->v_dispname;
		else
			p = (gbl(scr_cur)->vfcb)->v_pathname;
	}
	else if (gbl(scr_is_split) && !gbl(scr_in_autoview))
	{
		l = disp_ntype();
		l += getmaxx(gbl(scr_cur)->cur_path_line) - 3;
		p = gbl(scr_cur)->path_name;
	}
	else
	{
		xaddstr(gbl(scr_cur)->cur_path_line, msgs(m_stats_path));
		l = disp_ntype();
		l += getmaxx(gbl(scr_cur)->cur_path_line) -
			(strlen(msgs(m_stats_path))+2);
		p = gbl(scr_cur)->path_name;
	}

	if (n->node_type == N_FTP)
	{
		node_type = ftp_get_type(n);
		p = fn_convert_dirname(p, FN_SYSTEM, node_type, path_name);
	}

	wstandout(gbl(scr_cur)->cur_path_line);
	zaddstr(gbl(scr_cur)->cur_path_line,
		x_fn_disp_path(node_type, p, l, fixed));
	wstandend(gbl(scr_cur)->cur_path_line);
	wrefresh(gbl(scr_cur)->cur_path_line);
}

void disp_file_box (void)
{
	int i, j;

	if (gbl(scr_in_autoview))
		return;

	if (gbl(scr_is_split) || opt(wide_screen))
	{
		wmove(gbl(win_border), 1, getbegx(gbl(scr_cur)->cur_file_win)+1);
		i = getmaxx(gbl(scr_cur)->cur_file_win)-5;
		waddch(gbl(win_border), '<');
		switch (gbl(scr_cur)->command_mode)
		{
		case m_dir:
			wattrset(gbl(win_border), gbl(scr_reg_lolite_attr));
			if (gbl(scr_cur)->invert_file_spec)
				wstandset(gbl(win_border), gbl(scr_reg_cursor_attr));
			else
				wstandset(gbl(win_border), gbl(scr_reg_hilite_attr));
			waddstr(gbl(win_border), msgs(m_stats_nod));
			i  -= 6;
			break;

		case m_file:
			wattrset(gbl(win_border), gbl(scr_reg_lolite_attr));
			if (gbl(scr_cur)->invert_file_spec)
				wstandset(gbl(win_border), gbl(scr_reg_cursor_attr));
			else
				wstandset(gbl(win_border), gbl(scr_reg_hilite_attr));
			waddstr(gbl(win_border), msgs(m_stats_fil2));
			i -= 6;
			break;

		case m_showall:
		case m_level_showall:
			wattrset(gbl(win_border), gbl(scr_sa_lolite_attr));
			if (gbl(scr_cur)->invert_file_spec)
				wstandset(gbl(win_border), gbl(scr_sa_cursor_attr));
			else
				wstandset(gbl(win_border), gbl(scr_sa_hilite_attr));
			waddstr(gbl(win_border), msgs(m_stats_sho));
			i -= 9;
			break;

		case m_tag_showall:
			wattrset(gbl(win_border), gbl(scr_sa_lolite_attr));
			if (gbl(scr_cur)->invert_file_spec)
				wstandset(gbl(win_border), gbl(scr_sa_cursor_attr));
			else
				wstandset(gbl(win_border), gbl(scr_sa_hilite_attr));
			waddch(gbl(win_border), pgm_const(tag_disp_char));
			waddstr(gbl(win_border), msgs(m_stats_sho));
			i -= 10;
			break;

		case m_global:
			wattrset(gbl(win_border), gbl(scr_sa_lolite_attr));
			if (gbl(scr_cur)->invert_file_spec)
				wstandset(gbl(win_border), gbl(scr_sa_cursor_attr));
			else
				wstandset(gbl(win_border), gbl(scr_sa_hilite_attr));
			waddstr(gbl(win_border), msgs(m_stats_glb));
			i -= 8;
			break;

		case m_tag_global:
			wattrset(gbl(win_border), gbl(scr_sa_lolite_attr));
			if (gbl(scr_cur)->invert_file_spec)
				wstandset(gbl(win_border), gbl(scr_sa_cursor_attr));
			else
				wstandset(gbl(win_border), gbl(scr_sa_hilite_attr));
			waddch(gbl(win_border), pgm_const(tag_disp_char));
			waddstr(gbl(win_border), msgs(m_stats_glb));
			i -= 9;
			break;
		}
		wstandout(gbl(win_border));
		for (j=0; j<i; j++)
		{
			if (!gbl(scr_cur)->file_spec[j])
				break;
			waddch(gbl(win_border), gbl(scr_cur)->file_spec[j]);
		}
		if (gbl(scr_cur)->file_spec[j])
		{
			j++;
			waddch(gbl(win_border), pgm_const(too_long_char));
		}
		wstandend(gbl(win_border));
		wattrset (gbl(win_border), gbl(scr_border_attr));
		wstandset(gbl(win_border), 0);
		waddch(gbl(win_border), '>');
		for (; j<i; j++)
			wbox_chr(gbl(win_border), B_HO, B_DVDH);
		wrefresh(gbl(win_border));

		if (gbl(scr_is_split) && gbl(scr_info_displayed))
			disp_info_window();
	}
	else
	{
		if (gbl(scr_cur)->command_mode == m_dir ||
			gbl(scr_cur)->command_mode == m_file)
		{
			wattrset(gbl(win_file_box), gbl(scr_reg_lolite_attr));
			if (gbl(scr_cur)->invert_file_spec)
				wstandset(gbl(win_file_box), gbl(scr_reg_cursor_attr));
			else
				wstandset(gbl(win_file_box), gbl(scr_reg_hilite_attr));
		}
		else
		{
			wattrset(gbl(win_file_box), gbl(scr_sa_lolite_attr));
			if (gbl(scr_cur)->invert_file_spec)
				wstandset(gbl(win_file_box), gbl(scr_sa_cursor_attr));
			else
				wstandset(gbl(win_file_box), gbl(scr_sa_hilite_attr));
		}

		werase(gbl(win_file_box));
		wmove(gbl(win_file_box), 0, 0);
		wclrtoeol(gbl(win_file_box));
		waddstr(gbl(win_file_box), msgs(m_stats_fil1));
		wstandout(gbl(win_file_box));
		waddstr(gbl(win_file_box), gbl(scr_cur)->file_spec);
		if ((int)strlen(gbl(scr_cur)->file_spec) >
			getmaxx(gbl(win_file_box))-6)
		{
			wmove(gbl(win_file_box), 0, getmaxx(gbl(win_file_box))-1);
			waddch(gbl(win_file_box), pgm_const(too_long_char));
		}
		wstandend(gbl(win_file_box));
		wrefresh(gbl(win_file_box));
	}
}

/* flag: -1 print if n == o, 0 print if n!=o, 1 print o=n */
void disp_drive_info (dev_t new_dev, int flag)
{
	MOUNT_INFO *v;
	int do_it = FALSE;
	NBLK *n;
	ABLK *a;
	char dbuf[12];

	if (gbl(scr_cur)->command_mode == m_dir ||
		gbl(scr_cur)->command_mode == m_file)
	{
		wattrset (gbl(win_drive_box), gbl(scr_reg_lolite_attr));
		wstandset(gbl(win_drive_box), gbl(scr_reg_hilite_attr));
	}
	else
	{
		wattrset (gbl(win_drive_box), gbl(scr_sa_lolite_attr));
		wstandset(gbl(win_drive_box), gbl(scr_sa_hilite_attr));
	}

	n = gbl(scr_cur)->cur_root;
	switch (n->node_type)
	{
	case N_ARCH:
		if (! opt(wide_screen) && !gbl(scr_is_split))
		{
			werase (gbl(win_drive_box));

			a = (ABLK *)n->node_sub_blk;

			wmove(gbl(win_drive_box), 0, 0);
			xaddstr(gbl(win_drive_box), msgs(m_stats_fil1));
			wstandout(gbl(win_drive_box));
			disp_fw_str(gbl(win_drive_box), fn_basename(a->arch_devname),
				getmaxx(gbl(win_drive_box))-display_len(msgs(m_stats_fil1)));
			wstandend(gbl(win_drive_box));

			wmove(gbl(win_drive_box), 1, 2);
			xaddstr(gbl(win_drive_box), msgs(m_stats_blk));
			wstandout(gbl(win_drive_box));
			waddstr(gbl(win_drive_box), xform(dbuf, a->arch_size));
			wstandend(gbl(win_drive_box));

			wrefresh(gbl(win_drive_box));
		}
		return;

	case N_FTP:
		if (! opt(wide_screen) && !gbl(scr_is_split))
		{
			werase(gbl(win_drive_box));

			wmove(gbl(win_drive_box), 0, 0);
			xaddstr(gbl(win_drive_box), msgs(m_stats_host));
			wstandout(gbl(win_drive_box));
			waddstr(gbl(win_drive_box), ftp_host(n));
			wstandend(gbl(win_drive_box));

			wmove(gbl(win_drive_box), 1, 0);
			xaddstr(gbl(win_drive_box), msgs(m_stats_addr));
			wstandout(gbl(win_drive_box));
			waddstr(gbl(win_drive_box), ftp_addr(n));
			wstandend(gbl(win_drive_box));

			wrefresh(gbl(win_drive_box));
		}
		return;
	}

	if (! new_dev)
	{
		do_it = TRUE;
	}
	else
	{
		switch (flag)
		{
		case -1:
				if (new_dev == gbl(scr_cur_dev))
					do_it = TRUE;
				break;
		case  0:
				if (new_dev != gbl(scr_cur_dev))
				{
					do_it = TRUE;
					gbl(scr_cur_dev) = new_dev;
				}
				break;
		case  1:
				do_it = TRUE;
				gbl(scr_cur_dev) = new_dev;
				break;
		}
	}

	if (do_it)
	{
		gbl(scr_cur_free) = ml_get_free_by_dev(gbl(mount_list),
			gbl(scr_cur_dev));

		if (gbl(scr_in_autoview) || opt(wide_screen))
			return;

		if (gbl(scr_is_split))
		{
			if (gbl(scr_info_displayed))
				disp_info_window();
			return;
		}

		werase (gbl(win_drive_box));

		wmove(gbl(win_drive_box), 0, 0);
		waddstr(gbl(win_drive_box), msgs(m_stats_disk));
		wstandout(gbl(win_drive_box));

		if (*n->node_name)
		{
				disp_fw_str(gbl(win_drive_box), n->node_name,
					getmaxx(gbl(win_drive_box)) -
						display_len(msgs(m_stats_disk)));
		}
		else
		{
			v = ml_get_mount_ent_by_dev(gbl(mount_list), gbl(scr_cur_dev));
			if (v)
			{
				if (opt(show_mnt_dev))
				{
					disp_fw_str(gbl(win_drive_box), v->mt_dev,
						getmaxx(gbl(win_drive_box)) -
							display_len(msgs(m_stats_disk)));
				}
				else
				{
					disp_fw_str(gbl(win_drive_box), v->mt_dir,
						getmaxx(gbl(win_drive_box)) -
							display_len(msgs(m_stats_disk)));
				}
			}
			else
			{
				waddch(gbl(win_drive_box),
					TO_HEX((gbl(scr_cur_dev) & 0xf000) >> 12));
				waddch(gbl(win_drive_box),
					TO_HEX((gbl(scr_cur_dev) & 0x0f00) >>  8));
				waddch(gbl(win_drive_box),
					TO_HEX((gbl(scr_cur_dev) & 0x00f0) >>  4));
				waddch(gbl(win_drive_box),
					TO_HEX((gbl(scr_cur_dev) & 0x000f)      ));
			}
		}
		wstandend(gbl(win_drive_box));

		wmove(gbl(win_drive_box), 1, 1);
		waddstr(gbl(win_drive_box), msgs(m_stats_fre1));
		wstandout(gbl(win_drive_box));
		if (gbl(scr_cur_free) == -1)
		{
			waddstr(gbl(win_drive_box), "        ???");
		}
		else
		{
			waddstr(gbl(win_drive_box), xform(dbuf, gbl(scr_cur_free)));
			waddch(gbl(win_drive_box), 'K');
		}
		wstandend(gbl(win_drive_box));

		wrefresh(gbl(win_drive_box));
	}
}

void disp_node_stats (NBLK *root)
{
	char dbuf[12];

	if (gbl(scr_in_autoview) || opt(wide_screen))
		return;

	if (gbl(scr_is_split))
	{
		disp_file_box();
		if (gbl(scr_info_displayed))
			disp_info_window();
		return;
	}

	wattrset (gbl(win_stats), gbl(scr_reg_lolite_attr));
	wstandset(gbl(win_stats), gbl(scr_reg_hilite_attr));
	werase (gbl(win_stats));

	wmove(gbl(win_stats), 0, 0);
	wstandout(gbl(win_stats));
	xaddstr(gbl(win_stats), msgs(m_stats_nodst));
	wstandend(gbl(win_stats));

	wmove(gbl(win_stats), 1, 1);
	waddstr(gbl(win_stats), msgs(m_stats_tot));

	wmove(gbl(win_stats), 2, 2);
	waddstr(gbl(win_stats), msgs(m_stats_fil3));
	wstandout(gbl(win_stats));
	waddstr(gbl(win_stats), xform(dbuf, root->node_total_count));
	wstandend(gbl(win_stats));

	wmove(gbl(win_stats), 3, 2);
	waddstr(gbl(win_stats), msgs(m_stats_byt));
	wstandout(gbl(win_stats));
	waddstr(gbl(win_stats), xform(dbuf, root->node_total_bytes));
	wstandend(gbl(win_stats));

	wmove(gbl(win_stats), 4, 1);
	waddstr(gbl(win_stats), msgs(m_stats_mat));

	wmove(gbl(win_stats), 5, 2);
	waddstr(gbl(win_stats), msgs(m_stats_fil3));
	wstandout(gbl(win_stats));
	waddstr(gbl(win_stats), xform(dbuf, root->node_match_count));
	wstandend(gbl(win_stats));

	wmove(gbl(win_stats), 6, 2);
	waddstr(gbl(win_stats), msgs(m_stats_byt));
	wstandout(gbl(win_stats));
	waddstr(gbl(win_stats), xform(dbuf, root->node_match_bytes));
	wstandend(gbl(win_stats));

	wmove(gbl(win_stats), 7, 1);
	waddstr(gbl(win_stats), msgs(m_stats_tag));

	wmove(gbl(win_stats), 8, 2);
	waddstr(gbl(win_stats), msgs(m_stats_fil3));
	wstandout(gbl(win_stats));
	waddstr(gbl(win_stats), xform(dbuf, root->node_tagged_count));
	wstandend(gbl(win_stats));

	wmove(gbl(win_stats), 9, 2);
	if (opt(show_tag_blocks))
	{
		waddstr(gbl(win_stats), msgs(m_stats_blk));
		wstandout(gbl(win_stats));
		waddstr(gbl(win_stats), xform(dbuf, root->node_tagged_blocks));
		wstandend(gbl(win_stats));
	}
	else
	{
		waddstr(gbl(win_stats), msgs(m_stats_byt));
		wstandout(gbl(win_stats));
		waddstr(gbl(win_stats), xform(dbuf, root->node_tagged_bytes));
		wstandend(gbl(win_stats));
	}
	wrefresh(gbl(win_stats));
}

void disp_dir_stats (void)
{
	char dbuf[12];

	if (gbl(scr_in_autoview) || opt(wide_screen))
		return;

	if (gbl(scr_is_split))
	{
		disp_file_box();
		if (gbl(scr_info_displayed))
			disp_info_window();
		return;
	}

	if (gbl(scr_cur)->command_mode == m_file)
	{
		wattrset (gbl(win_stats), gbl(scr_reg_lolite_attr));
		wstandset(gbl(win_stats), gbl(scr_reg_hilite_attr));
	}
	else
	{
		wattrset (gbl(win_stats), gbl(scr_sa_lolite_attr));
		wstandset(gbl(win_stats), gbl(scr_sa_hilite_attr));
	}
	werase (gbl(win_stats));
	wmove(gbl(win_stats), 0, 0);
	wclrtoeol(gbl(win_stats));
	switch (gbl(scr_cur)->command_mode)
	{
	case m_file:
		wstandout(gbl(win_stats));
		xaddstr(gbl(win_stats), msgs(m_stats_dirst));
		wstandend(gbl(win_stats));
		break;

	case m_showall:
	case m_level_showall:
		wstandout(gbl(win_stats));
		xaddstr(gbl(win_stats), msgs(m_stats_shost));
		wstandend(gbl(win_stats));
		break;

	case m_global:
		wstandout(gbl(win_stats));
		xaddstr(gbl(win_stats), msgs(m_stats_glbst));
		wstandend(gbl(win_stats));
		break;

	case m_tag_showall:
		wstandout(gbl(win_stats));
		xaddstr(gbl(win_stats), msgs(m_stats_shost));
		waddch(gbl(win_stats), pgm_const(tag_disp_char));
		wstandend(gbl(win_stats));
		break;

	case m_tag_global:
		wstandout(gbl(win_stats));
		xaddstr(gbl(win_stats), msgs(m_stats_glbst));
		waddch(gbl(win_stats), pgm_const(tag_disp_char));
		wstandend(gbl(win_stats));
		break;
	}

	wmove(gbl(win_stats), 1, 1);
	waddstr(gbl(win_stats), msgs(m_stats_tot));

	wmove(gbl(win_stats), 2, 2);
	waddstr(gbl(win_stats), msgs(m_stats_fil3));
	wstandout(gbl(win_stats));
	waddstr(gbl(win_stats), xform(dbuf, gbl(scr_cur)->dir_total_count));
	wstandend(gbl(win_stats));

	wmove(gbl(win_stats), 3, 2);
	waddstr(gbl(win_stats), msgs(m_stats_byt));
	wstandout(gbl(win_stats));
	waddstr(gbl(win_stats), xform(dbuf, gbl(scr_cur)->dir_total_bytes));
	wstandend(gbl(win_stats));

	wmove(gbl(win_stats), 4, 1);
	waddstr(gbl(win_stats), msgs(m_stats_mat));

	wmove(gbl(win_stats), 5, 2);
	waddstr(gbl(win_stats), msgs(m_stats_fil3));
	wstandout(gbl(win_stats));
	waddstr(gbl(win_stats), xform(dbuf, gbl(scr_cur)->dir_match_count));
	wstandend(gbl(win_stats));

	wmove(gbl(win_stats), 6, 2);
	waddstr(gbl(win_stats), msgs(m_stats_byt));
	wstandout(gbl(win_stats));
	waddstr(gbl(win_stats), xform(dbuf, gbl(scr_cur)->dir_match_bytes));
	wstandend(gbl(win_stats));

	wmove(gbl(win_stats), 7, 1);
	waddstr(gbl(win_stats), msgs(m_stats_tag));

	wmove(gbl(win_stats), 8, 2);
	waddstr(gbl(win_stats), msgs(m_stats_fil3));
	wstandout(gbl(win_stats));
	waddstr(gbl(win_stats), xform(dbuf, gbl(scr_cur)->dir_tagged_count));
	wstandend(gbl(win_stats));

	wmove(gbl(win_stats), 9, 2);
	if (opt(show_tag_blocks))
	{
		waddstr(gbl(win_stats), msgs(m_stats_blk));
		wstandout(gbl(win_stats));
		waddstr(gbl(win_stats), xform(dbuf, gbl(scr_cur)->dir_tagged_blocks));
		wstandend(gbl(win_stats));
	}
	else
	{
		waddstr(gbl(win_stats), msgs(m_stats_byt));
		wstandout(gbl(win_stats));
		waddstr(gbl(win_stats), xform(dbuf, gbl(scr_cur)->dir_tagged_bytes));
		wstandend(gbl(win_stats));
	}
	wrefresh(gbl(win_stats));
}

void disp_cur_dir (void)
{
	char dbuf[12];
	char *p;

	if (opt(wide_screen))
		return;

	if (gbl(scr_is_split))
	{
		if (gbl(scr_info_displayed))
			disp_info_window();
		return;
	}

	wattrset (gbl(win_current), gbl(scr_reg_lolite_attr));
	wstandset(gbl(win_current), gbl(scr_reg_hilite_attr));
	werase (gbl(win_current));
	wmove(gbl(win_current), 0, 1);
	wclrtoeol(gbl(win_current));
	if (getmaxy(gbl(win_current)) > 2)
	{
		wstandout(gbl(win_current));
		disp_fw_str(gbl(win_current), FULLNAME(gbl(scr_cur)->cur_dir), 18);
		wstandend(gbl(win_current));

		wmove(gbl(win_current), 1, 2);
		waddstr(gbl(win_current), msgs(m_stats_fil3));
		wstandout(gbl(win_current));
		waddstr(gbl(win_current),
			xform(dbuf, bcount((gbl(scr_cur)->cur_dir)->flist)));
		wstandend(gbl(win_current));

		wmove(gbl(win_current), 2, 2);
		waddstr(gbl(win_current), msgs(m_stats_byt));
		wstandout(gbl(win_current));
		waddstr(gbl(win_current),
			xform(dbuf, (gbl(scr_cur)->cur_dir)->dir_size));
		wstandend(gbl(win_current));
	}
	else
	{
		p = xform3(dbuf, bcount((gbl(scr_cur)->cur_dir)->flist));
		disp_fw_str(gbl(win_current), FULLNAME(gbl(scr_cur)->cur_dir),
			17 - strlen(p));
		waddch(gbl(win_current), ' ');
		wstandout(gbl(win_current));
		waddstr(gbl(win_current), p);
		wstandend(gbl(win_current));

		wmove(gbl(win_current), 1, 2);
		waddstr(gbl(win_current), msgs(m_stats_byt));
		wstandout(gbl(win_current));
		waddstr(gbl(win_current),
			xform(dbuf, (gbl(scr_cur)->cur_dir)->dir_size));
		wstandend(gbl(win_current));
	}
	wrefresh(gbl(win_current));
}

void disp_cur_file (void)
{
	char dbuf[12];

	if (gbl(scr_in_autoview) || opt(wide_screen))
		return;

	if (gbl(scr_is_split))
	{
		if (gbl(scr_info_displayed))
			disp_info_window();
		return;
	}

	if (gbl(scr_cur)->command_mode == m_file)
	{
		wattrset (gbl(win_current), gbl(scr_reg_lolite_attr));
		wstandset(gbl(win_current), gbl(scr_reg_hilite_attr));
	}
	else
	{
		wattrset (gbl(win_current), gbl(scr_sa_lolite_attr));
		wstandset(gbl(win_current), gbl(scr_sa_hilite_attr));
	}

	werase (gbl(win_current));

	wmove(gbl(win_current), 0, 1);
	wclrtoeol(gbl(win_current));
	wstandout(gbl(win_current));
	disp_fw_str(gbl(win_current), FULLNAME(gbl(scr_cur)->cur_file), 18);
	wstandend(gbl(win_current));

	wmove(gbl(win_current), 1, 2);
	waddstr(gbl(win_current), msgs(m_stats_byt));
	wstandout(gbl(win_current));
	waddstr(gbl(win_current),
		xform(dbuf, (gbl(scr_cur)->cur_file)->stbuf.st_size));
	wstandend(gbl(win_current));
	wrefresh(gbl(win_current));
}

void disp_info_window (void)
{
	WINDOW *win_info;
	DSTAT *save_cd;
	int save_scr_cur_no;
	int i;
	int save_attrs;
	int save_standout;

	/* if not split screen - done */

	if (!gbl(scr_is_split))
		return;

	if (gbl(scr_cur)->command_mode == m_dir)
		setup_dir_stats();

	save_cd = gbl(scr_cur);
	save_scr_cur_no = gbl(scr_cur_no);
	gbl(scr_cur_no) = next_scr_no(gbl(scr_cur_no));
	gbl(scr_cur) = &gbl(scr_stat)[gbl(scr_cur_no)];

	wmove(gbl(win_border), 1, getbegx(gbl(scr_cur)->cur_file_win));
	for (i=0; i<getmaxx(gbl(scr_cur)->cur_file_win); i++)
		wbox_chr(gbl(win_border), B_HO, B_DVDH);
	wrefresh(gbl(win_border));
	werase(gbl(scr_cur)->cur_path_line);
	wrefresh(gbl(scr_cur)->cur_path_line);
	file_scroll_bar_remove();
	if (gbl(scr_cur)->command_mode == m_dir || gbl(scr_cur)->in_small_window)
	{
		dir_scroll_bar_remove();
		small_border(OFF);
	}
	win_info = gbl(scr_cur)->large_file_win;
	gbl(scr_cur) = save_cd;
	gbl(scr_cur_no) = save_scr_cur_no;

	save_attrs = wattrget(win_info);
	save_standout = wstandget(win_info);
	if (gbl(scr_cur)->command_mode == m_dir ||
		gbl(scr_cur)->command_mode == m_file)
	{
		wattrset (win_info, gbl(scr_reg_lolite_attr));
		wstandset(win_info, gbl(scr_reg_hilite_attr));
	}
	else
	{
		wattrset (win_info, gbl(scr_sa_lolite_attr));
		wstandset(win_info, gbl(scr_sa_hilite_attr));
	}
	werase(win_info);

	disp_info_file_spec(win_info);
	disp_info_disk_box(win_info);
	disp_info_node_stats(win_info);
	disp_info_dir_stats(win_info);
	disp_info_dir_win(win_info);
	disp_info_file_win(win_info);

	wrefresh(win_info);
	wattrset (win_info, save_attrs);
	wstandset(win_info, save_standout);
	gbl(scr_info_displayed) = TRUE;
}

void info_win_off (void)
{
	if (gbl(scr_info_displayed))
	{
		int i;

		gbl(scr_info_displayed) = FALSE;
		i = next_scr_no(gbl(scr_cur_no));
		disp_split_window(i);
	}
}
