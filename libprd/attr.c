/*------------------------------------------------------------------------
 * attribute routines
 */
#include "libprd.h"

struct attr_info
{
	int		and_mask;
	int		or_mask;
	time_t	attr_date;
	int		new_uid;
	int		new_gid;
	int		attr_choice;
	BLIST *	links;
};
typedef struct attr_info ATTR_INFO;

static int set_file_attr (void *data)
{
	ATTR_INFO *	ti	= (ATTR_INFO *)data;
	BLIST *		l;
	FBLK *		f;
	FBLK *		fc = gbl(scr_cur)->cur_file;
	NBLK *		n;
	int			i;
	int			c;
	char		filename[MAX_PATHLEN];
	int			new_perm;
	int			frow;
	int			fcol;

	/* check if link to file already processed */

	if (gbl(scr_cur)->cmd_sub_mode == m_tag && fc->stbuf.st_nlink>1)
	{
		for (l=ti->links; l; l=bnext(l))
		{
			f = (FBLK *)bid(l);
			if (f->stbuf.st_ino == fc->stbuf.st_ino &&
			    f->stbuf.st_dev == fc->stbuf.st_dev)
				break;
		}
		if (l)						/* if so, return as if done */
			return (1);
		l = BNEW(fc);				/* if not - add to done list */
		if (l)
		{
			ti->links = bappend(ti->links, l);
		}
	}
	n = get_root_of_file(fc);
	strcpy(filename, gbl(scr_cur)->path_name);
	fn_append_filename_to_dir(filename, FULLNAME(fc));

	if (ti->attr_choice == cmds(CMDS_ATTR_PERMS))
	{
		new_perm = fc->stbuf.st_mode & S_PERMS;
		new_perm &= ti->and_mask;
		new_perm |= ti->or_mask;
		i = os_chmod(filename, new_perm);
		if (i)
		{
			c = errsys(ER_CCP);
			return (c);
		}
		os_stat(filename, &fc->stbuf);
		if (fc->stbuf.st_nlink > 1)
		{
			for (l=n->link_list; l; l=bnext(l))
			{
				f = (FBLK *)bid(l);
				if (f != fc &&
				    f->stbuf.st_ino == fc->stbuf.st_ino &&
				    f->stbuf.st_dev == fc->stbuf.st_dev)
				{
					f->stbuf.st_mode &= ~S_PERMS;
					f->stbuf.st_mode |= new_perm;
					if (is_file_displayed(f, &frow, &fcol))
						redisplay_file(f, frow, fcol);
				}
			}
		}
	}
	else if (ti->attr_choice == cmds(CMDS_ATTR_OWNER))
	{
		i = os_chown(filename, ti->new_uid, fc->stbuf.st_gid);
		if (i)
		{
			c = errsys(ER_CCO);
			return (c);
		}
		os_stat(filename, &fc->stbuf);
		if (fc->stbuf.st_nlink > 1)
		{
			for (l=n->link_list; l; l=bnext(l))
			{
				f = (FBLK *)bid(l);
				if (f != fc &&
				    f->stbuf.st_ino == fc->stbuf.st_ino &&
				    f->stbuf.st_dev == fc->stbuf.st_dev)
				{
					f->stbuf.st_uid = ti->new_uid;
					if (is_file_displayed(f, &frow, &fcol))
						redisplay_file(f, frow, fcol);
				}
			}
		}
	}
	else if (ti->attr_choice == cmds(CMDS_ATTR_GROUP))
	{
		i = os_chown(filename, fc->stbuf.st_uid, ti->new_gid);
		if (i)
		{
			c = errsys(ER_CCG);
			return (c);
		}
		os_stat(filename, &fc->stbuf);
		if (fc->stbuf.st_nlink > 1)
		{
			for (l=n->link_list; l; l=bnext(l))
			{
				f = (FBLK *)bid(l);
				if (f != fc &&
				    f->stbuf.st_ino == fc->stbuf.st_ino &&
				    f->stbuf.st_dev == fc->stbuf.st_dev)
				{
					f->stbuf.st_gid = ti->new_gid;
					if (is_file_displayed(f, &frow, &fcol))
						redisplay_file(f, frow, fcol);
				}
			}
		}
	}
	else if (ti->attr_choice == cmds(CMDS_ATTR_DATE))
	{
		i = os_set_file_time(filename, ti->attr_date, ti->attr_date);
		if (i)
		{
			c = errsys(ER_CCD);
			return (c);
		}
		if (fc->stbuf.st_nlink > 1)
		{
			for (l=n->link_list; l; l=bnext(l))
			{
				f = (FBLK *)bid(l);
				if (f != fc &&
				    f->stbuf.st_ino == fc->stbuf.st_ino &&
				    f->stbuf.st_dev == fc->stbuf.st_dev)
				{
					f->stbuf.st_mtime = ti->attr_date;
					if (is_file_displayed(f, &frow, &fcol))
						redisplay_file(f, frow, fcol);
				}
			}
		}
		fc->stbuf.st_mtime = ti->attr_date;
	}
	hilite_file(ON);
	return (1);					/* for traverse() */
}

int attr_get_file (const char *item, struct stat *pstbuf)
{
	char input_str[MAX_PATHLEN];
	char buffer[MAX_PATHLEN];
	int i;

	bang("");
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 1, 0);
	xaddstr(gbl(win_commands), msgs(m_attr_copy));
	xaddstr(gbl(win_commands), item);
	xaddstr(gbl(win_commands), msgs(m_attr_frofil));
	wrefresh(gbl(win_commands));
	*input_str = 0;
	i = xgetstr(gbl(win_commands), input_str, XGL_FILE_TO_COPY, MAX_PATHLEN, 0,
		XG_FILEPATH);
	if (i <= 0)
		return (-1);
	fn_get_abs_path(gbl(scr_cur)->path_name, input_str, buffer);
	if (os_stat(buffer, pstbuf))
	{
		errsys(ER_CSN);
		return (-1);
	}
	return (0);
}

static int set_dir_attr (ATTR_INFO *ti)
{
	int i;
	int c;
	int new_perm;
	DBLK *dc = gbl(scr_cur)->cur_dir;

	if (ti->attr_choice == cmds(CMDS_ATTR_PERMS))
	{
		new_perm  = dc->stbuf.st_mode & S_PERMS;
		new_perm &= ti->and_mask;
		new_perm |= ti->or_mask;
		i = os_chmod(gbl(scr_cur)->path_name, new_perm);
		if (i)
		{
			c = errsys(ER_CCP);
			return (c);
		}
		os_stat(gbl(scr_cur)->path_name, &dc->stbuf);
	}
	else if (ti->attr_choice == cmds(CMDS_ATTR_OWNER))
	{
		i = os_chown(gbl(scr_cur)->path_name, ti->new_uid, dc->stbuf.st_gid);
		if (i)
		{
			c = errsys(ER_CCO);
			return (c);
		}
		os_stat(gbl(scr_cur)->path_name, &dc->stbuf);
	}
	else if (ti->attr_choice == cmds(CMDS_ATTR_GROUP))
	{
		i = os_chown(gbl(scr_cur)->path_name, dc->stbuf.st_uid, ti->new_gid);
		if (i)
		{
			c = errsys(ER_CCG);
			return (c);
		}
		os_stat(gbl(scr_cur)->path_name, &dc->stbuf);
	}
	else if (ti->attr_choice == cmds(CMDS_ATTR_DATE))
	{
		i = os_set_file_time(gbl(scr_cur)->path_name,
			ti->attr_date, ti->attr_date);
		if (i)
		{
			c = errsys(ER_CCD);
			return (c);
		}
		dc->stbuf.st_mtime = ti->attr_date;
	}
	if (gbl(scr_cur)->dir_fmt != fmt_dname)
		hilite_dir(ON);
	return (0);
}

static int ask_for_attr (int type, ATTR_INFO *ti)
{
	char input_str[MAX_PATHLEN];
	BLIST *l;
	USR_GRP *u;
	DBLK *d;
	FBLK *f;
	char *s;
	int mo, dy, yr;
	int hh, mm, ss;
	int c;
	int i;
	int p;
	struct stat stbuf;

	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	s =  msgs(m_attr_att);
	xaddstr(gbl(win_commands), s);
	if (type == 1)
	{
		p = (gbl(scr_cur)->cur_file)->stbuf.st_mode & S_PERMS;
		ti->and_mask  = p;
		ti->or_mask   = p;
		xaddstr(gbl(win_commands), msgs(m_attr_fortgd));
	}
	else if (type == 0)
	{
		p = (gbl(scr_cur)->cur_file)->stbuf.st_mode & S_PERMS;
		ti->and_mask  = p;
		ti->or_mask   = p;
		xaddstr(gbl(win_commands), msgs(m_attr_forfil));
		f = gbl(scr_cur)->cur_file;
		show_file_info(FULLNAME(f),
			(off_t)-1, f->stbuf.st_mtime, f->stbuf.st_mode,
			f->stbuf.st_uid, f->stbuf.st_gid);
	}
	else
	{
		p = (gbl(scr_cur)->cur_dir)->stbuf.st_mode & S_PERMS;
		ti->and_mask  = p;
		ti->or_mask   = p;
		xaddstr(gbl(win_commands), msgs(m_attr_fordir));
		d = gbl(scr_cur)->cur_dir;
		show_file_info(FULLNAME(d),
			(off_t)-1, d->stbuf.st_mtime, d->stbuf.st_mode,
			d->stbuf.st_uid, d->stbuf.st_gid);
	}

	wmove(gbl(win_commands), 1, display_len(s));
	xaddstr(gbl(win_commands), msgs(m_attr_cha));
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_ATTR_DATE, msgs(m_attr_chad));
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_ATTR_PERMS, msgs(m_attr_chap));
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_ATTR_OWNER, msgs(m_attr_chao));
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_ATTR_GROUP, msgs(m_attr_chag));
	wrefresh(gbl(win_commands));
	bang(msgs(m_attr_entatt));		/* leave cursor here */
	while (TRUE)
	{
		c = xgetch(gbl(win_message));
		c = TO_LOWER(c);
		if (c == KEY_ESCAPE ||
			c == KEY_RETURN ||
			c == cmds(CMDS_ATTR_DATE) ||
		    c == cmds(CMDS_ATTR_PERMS) ||
		    c == cmds(CMDS_ATTR_OWNER) ||
			c == cmds(CMDS_ATTR_GROUP))
			break;
	}
	bang("");
	if (c == KEY_ESCAPE || c == KEY_RETURN)
		return (-1);

	if (c == cmds(CMDS_ATTR_DATE))
	{
		gbl(scr_cur_sub_cmd) = CMDS_ATTR_DATE;
		ti->attr_choice = cmds(CMDS_ATTR_DATE);
		dest_msg(CMDS_ATTR_COPYFILE);
		wmove(gbl(win_commands), 1, 0);
		wclrtoeol(gbl(win_commands));
		xaddstr(gbl(win_commands), msgs(m_attr_entdat));
		wrefresh(gbl(win_commands));

		*input_str = 0;
		i = xgetstr(gbl(win_commands), input_str, XGL_DATE, 36, 0,
			XG_STRING_AB);
		if (i < 0)
			return (-1);
		if (i == cmds(CMDS_ATTR_COPYFILE))
		{
			if (attr_get_file(msgs(m_attr_dat), &stbuf))
				return (-1);
			ti->attr_date = stbuf.st_mtime;
			return (0);
		}

		c = cdate(input_str, &yr, &mo, &dy, &hh, &mm, &ss);
		if (c)
		{
			errmsg(ER_IDS, "", ERR_ANY);
			return (-1);
		}
		ti->attr_date = datecvt(yr, mo, dy, hh, mm, ss);
		return (0);
	}
	else if (c == cmds(CMDS_ATTR_OWNER))
	{
		gbl(scr_cur_sub_cmd) = CMDS_ATTR_OWNER;
		ti->attr_choice = cmds(CMDS_ATTR_OWNER);
		dest_msg(CMDS_ATTR_COPYFILE);
		wmove(gbl(win_commands), 1, 0);
		wclrtoeol(gbl(win_commands));
		xaddstr(gbl(win_commands), msgs(m_attr_entown));
		wrefresh(gbl(win_commands));

		*input_str = 0;
		i = xgetstr(gbl(win_commands), input_str, XGL_OWNER, UG_LEN, 0,
			XG_STRING_AB);
		if (i < 0)
			return (-1);
		if (i == cmds(CMDS_ATTR_COPYFILE))
		{
			if (attr_get_file(msgs(m_attr_own), &stbuf))
				return (-1);
			ti->new_uid = stbuf.st_uid;
			check_owner(ti->new_uid);
			return (0);
		}

		if (isdigit(*input_str))
		{
			ti->new_uid = atoi(input_str);
			check_owner(ti->new_uid);
			return (0);
		}
		else
		{
			for (l=gbl(owner_list); l; l=bnext(l))
			{
				u = (USR_GRP *)bid(l);
				if (strncmp(u->ug_name, input_str, strlen(input_str)) == 0)
				{
					ti->new_uid = u->ug_id;
					return (0);
				}
			}
			ti->new_uid = os_get_usr_id_from_name(input_str);
			if (ti->new_uid != -1)
			{
				check_owner(ti->new_uid);
				return (0);
			}
			else
			{
				errmsg(ER_ION, "", ERR_ANY);
				return (-1);
			}
		}
	}
	else if (c == cmds(CMDS_ATTR_GROUP))
	{
		gbl(scr_cur_sub_cmd) = CMDS_ATTR_GROUP;
		ti->attr_choice = cmds(CMDS_ATTR_GROUP);
		dest_msg(CMDS_ATTR_COPYFILE);
		wmove(gbl(win_commands), 1, 0);
		wclrtoeol(gbl(win_commands));
		xaddstr(gbl(win_commands), msgs(m_attr_entgrp));
		wrefresh(gbl(win_commands));
		*input_str = 0;
		i = xgetstr(gbl(win_commands), input_str, XGL_GROUP, UG_LEN, 0,
			XG_STRING_AB);
		if (i < 0)
			return (-1);
		if (i == cmds(CMDS_ATTR_COPYFILE))
		{
			if (attr_get_file(msgs(m_attr_grp), &stbuf))
				return (-1);
			ti->new_gid = stbuf.st_gid;
			check_group(ti->new_gid);
			return (0);
		}
		if (isdigit(*input_str))
		{
			ti->new_gid = atoi(input_str);
			check_group(ti->new_gid);
			return (0);
		}
		else
		{
			for (l=gbl(group_list); l; l=bnext(l))
			{
				u = (USR_GRP *)bid(l);
				if (strncmp(u->ug_name, input_str, strlen(input_str)) == 0)
				{
					ti->new_gid = u->ug_id;
					return (0);
				}
			}
			ti->new_gid = os_get_grp_id_from_name(input_str);
			if (ti->new_gid != -1)
			{
				check_group(ti->new_gid);
				return (0);
			}
			else
			{
				errmsg(ER_IGN, "", ERR_ANY);
				return (-1);
			}
		}
	}
	else /* if (c == cmds(CMDS_ATTR_PERMS)) */
	{
		gbl(scr_cur_sub_cmd) = CMDS_ATTR_PERMS;
		ti->attr_choice = cmds(CMDS_ATTR_PERMS);
		i = get_perm_masks(FALSE, &ti->and_mask, &ti->or_mask);
		return (i);
	}
	/*NOTREACHED*/
}

void do_file_attr (void)
{
	ATTR_INFO	ai_info;
	ATTR_INFO *	ti = &ai_info;
	int i;

	i = ask_for_attr(0, ti);
	if (i < 0)
		return;

	set_file_attr(ti);
}

void do_tag_file_attr (void)
{
	ATTR_INFO	ai_info;
	ATTR_INFO *	ti = &ai_info;
	int i;

	if (check_tag_count())
		return;

	i = ask_for_attr(1, ti);
	if (i < 0)
		return;

	ti->links = 0;
	traverse(set_file_attr, ti);
	BSCRAP(ti->links, FALSE);
}

void do_dir_attr (void)
{
	ATTR_INFO	ai_info;
	ATTR_INFO *	ti = &ai_info;
	int i;

	i = ask_for_attr(2, ti);
	if (i < 0)
		return;

	set_dir_attr(ti);
}
