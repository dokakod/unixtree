/*------------------------------------------------------------------------
 * tag/untag routines
 */
#include "libprd.h"

#define SECS_PER_DAY		(24 * 60 * 60)

int is_file_tagged (FBLK *f)
{
	return (f->tagged & gbl(scr_cur)->tag_mask);
}

void tag_file (FBLK *f)
{
	NBLK *n;
	long blks;

	if (!is_file_tagged(f))
	{
		f->tagged |= gbl(scr_cur)->tag_mask;
		gbl(scr_cur)->dir_tagged_count++;
		blks = file_size_in_blks(f, f->stbuf.st_size);
		if (!check_for_dup_inode(gbl(scr_cur)->base_file, f, 0, 1))
		{
			gbl(scr_cur)->dir_tagged_bytes += f->stbuf.st_size;
			gbl(scr_cur)->dir_tagged_blocks += blks;
		}
		n = get_root_of_file(f);
		n->node_tagged_count++;
		if (!check_for_dup_inode(n->showall_flist, f, 0, 1))
		{
			n->node_tagged_bytes += f->stbuf.st_size;
			n->node_tagged_blocks += blks;
		}
	}
}

void untag_file (FBLK *f)
{
	NBLK *n;
	long blks;

	if (is_file_tagged(f))
	{
		f->tagged &= ~gbl(scr_cur)->tag_mask;
		gbl(scr_cur)->dir_tagged_count--;
		blks = file_size_in_blks(f, f->stbuf.st_size);
		if (!check_for_dup_inode(gbl(scr_cur)->base_file, f, 0, 1))
		{
			gbl(scr_cur)->dir_tagged_bytes -= f->stbuf.st_size;
			gbl(scr_cur)->dir_tagged_blocks -= blks;
		}
		n = get_root_of_file(f);
		n->node_tagged_count--;
		if (!check_for_dup_inode(n->showall_flist, f, 0, 1))
		{
			n->node_tagged_bytes -= f->stbuf.st_size;
			n->node_tagged_blocks -= blks;
		}
	}
}

void invert_file (FBLK *f)
{
	if (is_file_tagged(f))
		untag_file(f);
	else
		tag_file(f);
}

void tag_dir (BLIST *dir_ptr)
{
	BLIST *b;
	TREE *t;
	DBLK *d;

	t = (TREE *)bid(dir_ptr);
	d = (DBLK *)tid(t);
	for (b=d->mlist[gbl(scr_cur_no)]; b; b=bnext(b))
		tag_file((FBLK *)bid(b));
}

void untag_dir (BLIST *dir_ptr)
{
	BLIST *b;
	TREE *t;
	DBLK *d;

	t = (TREE *)bid(dir_ptr);
	d = (DBLK *)tid(t);
	for (b=d->mlist[gbl(scr_cur_no)]; b; b=bnext(b))
		untag_file((FBLK *)bid(b));
}

void invert_dir (BLIST *dir_ptr)
{
	BLIST *b;
	TREE *t;
	DBLK *d;

	t = (TREE *)bid(dir_ptr);
	d = (DBLK *)tid(t);
	for (b=d->mlist[gbl(scr_cur_no)]; b; b=bnext(b))
		invert_file((FBLK *)bid(b));
}

int ask_alt_tag (int tag_mode, TAG_INFO *ti)
{
	char input_str[MAX_PATHLEN];
	char buffer[MAX_PATHLEN];
	char dest_dir[MAX_PATHLEN];
	struct stat stbuf;
	TREE *t;
	BLIST *l;
	NBLK *n;
	USR_GRP *u;
	int mo, dy, yr;
	int mm, hh, ss;
	int c;
	int i;
	int need_order = FALSE;
	int ll;

	tag_mode = tolower(tag_mode);
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	if (tag_mode == cmds(CMDS_COMMON_INVERT))
			xaddstr(gbl(win_commands), msgs(m_tagfile_inv2));
	else if (tag_mode == cmds(CMDS_COMMON_TAG))
			xaddstr(gbl(win_commands), msgs(m_tagfile_tag2));
	else if (tag_mode == cmds(CMDS_COMMON_UNTAG))
			xaddstr(gbl(win_commands), msgs(m_tagfile_unt2));
	xaddstr(gbl(win_commands), msgs(m_tagfile_amfb));
	ll = getcurx(gbl(win_commands));
	xcaddstr(gbl(win_commands), CMDS_TAG_DATE, msgs(m_tagfile_ctypd));
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_TAG_PERMS, msgs(m_tagfile_ctypp));
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_TAG_SIZE, msgs(m_tagfile_ctyps));
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_TAG_OWNER, msgs(m_tagfile_ctypo));
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_TAG_GROUP, msgs(m_tagfile_ctypg));
	wmove(gbl(win_commands), 1, ll);
	xcaddstr(gbl(win_commands), CMDS_TAG_INODE, msgs(m_tagfile_ctypi));
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_TAG_LINKS, msgs(m_tagfile_ctypl));
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_TAG_NODE, msgs(m_tagfile_ctypn));
	waddstr(gbl(win_commands), "  ");
	xcaddstr(gbl(win_commands), CMDS_TAG_TYPE, msgs(m_tagfile_ctypt));
	wrefresh(gbl(win_commands));
	werase(gbl(win_message));
	esc_msg();
	bangnc(msgs(m_tagfile_enter));
	if (tag_mode == cmds(CMDS_COMMON_INVERT))
		waddstr(gbl(win_message), msgs(m_tagfile_inv1));
	else if (tag_mode == cmds(CMDS_COMMON_TAG))
		waddstr(gbl(win_message), msgs(m_tagfile_tag1));
	else if (tag_mode == cmds(CMDS_COMMON_UNTAG))
		waddstr(gbl(win_message), msgs(m_tagfile_unt1));
	waddstr(gbl(win_message), msgs(m_tagfile_crit));
	wrefresh(gbl(win_message));
	while (TRUE)
	{
		c = xgetch(gbl(win_message));
		c = TO_LOWER(c);
		if (c == KEY_RETURN ||
			c == KEY_ESCAPE ||
		    c == cmds(CMDS_TAG_INODE) ||
			c == cmds(CMDS_TAG_LINKS) ||
			c == cmds(CMDS_TAG_SIZE)  ||
			c == cmds(CMDS_TAG_NODE)  ||
			c == cmds(CMDS_TAG_PERMS) ||
			c == cmds(CMDS_TAG_DATE)  ||
			c == cmds(CMDS_TAG_OWNER) ||
			c == cmds(CMDS_TAG_GROUP) ||
			c == cmds(CMDS_TAG_TYPE))
			break;
	}
	bang("");
	if (c == KEY_ESCAPE || c == KEY_RETURN)
		return (0);
	gbl(scr_cur_sub_cmd) = c;
	if (c == cmds(CMDS_TAG_DATE))
	{
		gbl(scr_cur_sub_cmd) = CMDS_TAG_DATE;
		ti->tag_choice = cmds(CMDS_TAG_DATE);
		dest_msg(CMDS_TAG_COPYFILE);
		werase(gbl(win_commands));
		wmove(gbl(win_commands), 0, 0);
		xaddstr(gbl(win_commands), msgs(m_tagfile_entdat));
		wrefresh(gbl(win_commands));
		*input_str = 0;
		c = xgetstr(gbl(win_commands), input_str, XGL_DATE, 32, 0,
			XG_STRING_AB);
		if (c < 0)
			return (0);
		need_order = FALSE;
		if (c == cmds(CMDS_TAG_COPYFILE))
		{
			werase(gbl(win_commands));
			wmove(gbl(win_commands), 1, 0);
			xaddstr(gbl(win_commands), msgs(m_tagfile_copdat));
			wrefresh(gbl(win_commands));
			*input_str = 0;
			c = xgetstr(gbl(win_commands), input_str, XGL_FILE_TO_COPY,
				MAX_PATHLEN, 0, XG_FILEPATH);
			if (c <= 0)
				return (0);

			fn_get_abs_path(gbl(scr_cur)->path_name, input_str, buffer);
			if (os_lstat(buffer, &stbuf))
			{
				errsys(ER_CSN);
				return (0);
			}
			ti->tag_date = stbuf.st_mtime;
		}
		else
		{
			if (c == 0)
			{
				waddstr(gbl(win_commands), msgs(m_tagfile_today));
				wrefresh(gbl(win_commands));
			}
			c = cdate(input_str, &yr, &mo, &dy, &hh, &mm, &ss);
			if (c)
			{
				errmsg(ER_IDS, "", ERR_ANY);
				return (0);
			}
			ti->tag_date = datecvt(yr, mo, dy, 0, 0, 0);
		}
		need_order = TRUE;
	}
	else if (c == cmds(CMDS_TAG_INODE))
	{
		gbl(scr_cur_sub_cmd) = CMDS_TAG_INODE;
		ti->tag_choice = cmds(CMDS_TAG_INODE);
		dest_msg(CMDS_TAG_COPYFILE);
		werase(gbl(win_commands));
		wmove(gbl(win_commands), 0, 0);
		xaddstr(gbl(win_commands), msgs(m_tagfile_entino));
		wrefresh(gbl(win_commands));
		*input_str = 0;
		c = xgetstr(gbl(win_commands), input_str, XGL_INODE, 12, 0, XG_NUMBER);
		if (c < 0)
			return (0);
		if (c == cmds(CMDS_TAG_COPYFILE))
		{
			werase(gbl(win_commands));
			wmove(gbl(win_commands), 1, 0);
			xaddstr(gbl(win_commands), msgs(m_tagfile_copino));
			wrefresh(gbl(win_commands));
			*input_str = 0;
			c = xgetstr(gbl(win_commands), input_str, XGL_FILE_TO_COPY,
				MAX_PATHLEN, 0, XG_FILEPATH);
			if (c <= 0)
				return (0);
			fn_get_abs_path(gbl(scr_cur)->path_name, input_str, buffer);
			if (os_lstat(buffer, &stbuf))
			{
				errsys(ER_CSN);
				return (0);
			}
			ti->tag_inode = stbuf.st_ino;
		}
		else
		{
			ti->tag_inode = atoi(input_str);
		}
		need_order = TRUE;
	}
	else if (c == cmds(CMDS_TAG_LINKS))
	{
		gbl(scr_cur_sub_cmd) = CMDS_TAG_LINKS;
		ti->tag_choice = cmds(CMDS_TAG_LINKS);
		dest_msg(CMDS_TAG_COPYFILE);
		werase(gbl(win_commands));
		wmove(gbl(win_commands), 0, 0);
		xaddstr(gbl(win_commands), msgs(m_tagfile_entlin));
		wrefresh(gbl(win_commands));
		*input_str = 0;
		c = xgetstr(gbl(win_commands), input_str, XGL_NLINK, 12, 0, XG_NUMBER);
		if (c < 0)
			return (0);
		if (c == cmds(CMDS_TAG_COPYFILE))
		{
			werase(gbl(win_commands));
			wmove(gbl(win_commands), 1, 0);
			xaddstr(gbl(win_commands), msgs(m_tagfile_coplin));
			wrefresh(gbl(win_commands));
			*input_str = 0;
			c = xgetstr(gbl(win_commands), input_str, XGL_FILE_TO_COPY,
				MAX_PATHLEN, 0, XG_FILEPATH);
			if (c <= 0)
				return (0);
			fn_get_abs_path(gbl(scr_cur)->path_name, input_str, buffer);
			if (os_lstat(buffer, &stbuf))
			{
				errsys(ER_CSN);
				return (0);
			}
			ti->tag_link_count = stbuf.st_nlink;
		}
		else
		{
			ti->tag_link_count = atoi(input_str);
		}
		need_order = TRUE;
	}
	else if (c == cmds(CMDS_TAG_SIZE))
	{
		gbl(scr_cur_sub_cmd) = CMDS_TAG_SIZE;
		ti->tag_choice = cmds(CMDS_TAG_SIZE);
		dest_msg(CMDS_TAG_COPYFILE);
		werase(gbl(win_commands));
		wmove(gbl(win_commands), 0, 0);
		xaddstr(gbl(win_commands), msgs(m_tagfile_entsiz));
		wrefresh(gbl(win_commands));
		*input_str = 0;
		c = xgetstr(gbl(win_commands), input_str, XGL_SIZE, 12, 0, XG_NUMBER);
		if (c < 0)
			return (0);
		if (c == cmds(CMDS_TAG_COPYFILE))
		{
			werase(gbl(win_commands));
			wmove(gbl(win_commands), 1, 0);
			xaddstr(gbl(win_commands), msgs(m_tagfile_copsiz));
			wrefresh(gbl(win_commands));
			*input_str = 0;
			c = xgetstr(gbl(win_commands), input_str, XGL_FILE_TO_COPY,
				MAX_PATHLEN, 0, XG_FILEPATH);
			if (c <= 0)
				return (0);
			fn_get_abs_path(gbl(scr_cur)->path_name, input_str, buffer);
			if (os_lstat(buffer, &stbuf))
			{
				errsys(ER_CSN);
				return (0);
			}
			ti->tag_size = stbuf.st_size;
		}
		else
		{
			if (c == 0)
			{
				waddch(gbl(win_commands), '0');
				wrefresh(gbl(win_commands));
			}
			ti->tag_size = atoi(input_str);
		}
		need_order = TRUE;
	}
	else if (c == cmds(CMDS_TAG_PERMS))
	{
		gbl(scr_cur_sub_cmd) = CMDS_TAG_PERMS;
		ti->tag_choice = cmds(CMDS_TAG_PERMS);
		ti->tag_perm_mask = 0;
		ti->tag_perm_tag  = 0;
		if (get_perm_masks(TRUE, &ti->tag_perm_mask, &ti->tag_perm_tag))
			return (0);
		if (ti->tag_perm_mask == 0)
			return (0);
		bang("");
		need_order = FALSE;
	}
	else if (c == cmds(CMDS_TAG_OWNER))
	{
		gbl(scr_cur_sub_cmd) = CMDS_TAG_OWNER;
		ti->tag_choice = cmds(CMDS_TAG_OWNER);
		dest_msg(CMDS_TAG_COPYFILE);
		werase(gbl(win_commands));
		wmove(gbl(win_commands), 0, 0);
		xaddstr(gbl(win_commands), msgs(m_tagfile_entown));
		wrefresh(gbl(win_commands));
		*input_str = 0;
		c = xgetstr(gbl(win_commands), input_str, XGL_OWNER, UG_LEN, 0,
			XG_STRING_AB);
		if (c < 0)
			return (0);
		if (c == cmds(CMDS_TAG_COPYFILE))
		{
			werase(gbl(win_commands));
			wmove(gbl(win_commands), 1, 0);
			xaddstr(gbl(win_commands), msgs(m_tagfile_copown));
			wrefresh(gbl(win_commands));
			*input_str = 0;
			c = xgetstr(gbl(win_commands), input_str, XGL_FILE_TO_COPY,
				MAX_PATHLEN, 0, XG_FILEPATH);
			if (c <= 0)
				return (0);
			fn_get_abs_path(gbl(scr_cur)->path_name, input_str, buffer);
			if (os_lstat(buffer, &stbuf))
			{
				errsys(ER_CSN);
				return (0);
			}
			ti->tag_uid = stbuf.st_uid;
		}
		else
		{
			if (isdigit(*input_str))
			{
				ti->tag_uid = atoi(input_str);
			}
			else
			{
				for (l=gbl(owner_list); l; l=bnext(l))
				{
					u = (USR_GRP *)bid(l);
					if (strncmp(u->ug_name, input_str, strlen(input_str)) == 0)
					{
						ti->tag_uid = u->ug_id;
						break;
					}
				}
				if (!l)
				{
					errmsg(ER_ION, "", ERR_ANY);
					return (0);
				}
			}
		}
		need_order = FALSE;
	}
	else if (c == cmds(CMDS_TAG_GROUP))
	{
		gbl(scr_cur_sub_cmd) = CMDS_TAG_GROUP;
		ti->tag_choice = cmds(CMDS_TAG_GROUP);
		dest_msg(CMDS_TAG_COPYFILE);
		werase(gbl(win_commands));
		wmove(gbl(win_commands), 0, 0);
		xaddstr(gbl(win_commands), msgs(m_tagfile_entgrp));
		wrefresh(gbl(win_commands));
		*input_str = 0;
		c = xgetstr(gbl(win_commands), input_str, XGL_GROUP, UG_LEN, 0,
			XG_STRING_AB);
		if (c < 0)
			return (0);
		if (c == cmds(CMDS_TAG_COPYFILE))
		{
			werase(gbl(win_commands));
			wmove(gbl(win_commands), 1, 0);
			xaddstr(gbl(win_commands), msgs(m_tagfile_copgrp));
			wrefresh(gbl(win_commands));
			*input_str = 0;
			c = xgetstr(gbl(win_commands), input_str, XGL_FILE_TO_COPY,
				MAX_PATHLEN, 0, XG_FILEPATH);
			if (c <= 0)
				return (0);
			fn_get_abs_path(gbl(scr_cur)->path_name, input_str, buffer);
			if (os_lstat(buffer, &stbuf))
			{
				errsys(ER_CSN);
				return (0);
			}
			ti->tag_gid = stbuf.st_gid;
		}
		else
		{
			if (isdigit(*input_str))
			{
				ti->tag_gid = atoi(input_str);
			}
			else
			{
				for (l=gbl(group_list); l; l=bnext(l))
				{
					u = (USR_GRP *)bid(l);
					if (strncmp(u->ug_name, input_str, strlen(input_str)) == 0)
					{
						ti->tag_gid = u->ug_id;
						break;
					}
				}
				if (!l)
				{
					errmsg(ER_IGN, "", ERR_ANY);
					return (0);
				}
			}
		}
		need_order = FALSE;
	}
	else if (c == cmds(CMDS_TAG_TYPE))
	{
		gbl(scr_cur_sub_cmd) = CMDS_TAG_TYPE;
		ti->tag_choice = cmds(CMDS_TAG_TYPE);
		ti->tag_brk = FALSE;
		dest_msg(CMDS_TAG_COPYFILE);
		esc_msg();
		werase(gbl(win_commands));

		wmove(gbl(win_commands), 0, 0);
		xaddstr(gbl(win_commands), msgs(m_tagfile_enttype));

		xcaddstr(gbl(win_commands), CMDS_TAG_TYPE_REG,
			msgs(m_tagfile_enttypr));
		waddstr(gbl(win_commands), "  ");
		xcaddstr(gbl(win_commands), CMDS_TAG_TYPE_BLK,
			msgs(m_tagfile_enttypb));
		waddstr(gbl(win_commands), "  ");
		xcaddstr(gbl(win_commands), CMDS_TAG_TYPE_CHR,
			msgs(m_tagfile_enttypc));
		waddstr(gbl(win_commands), "  ");
		xcaddstr(gbl(win_commands), CMDS_TAG_TYPE_FIFO,
			msgs(m_tagfile_enttypf));
		waddstr(gbl(win_commands), "  ");
		xcaddstr(gbl(win_commands), CMDS_TAG_TYPE_DOOR,
			msgs(m_tagfile_enttypd));

		wmove(gbl(win_commands), 1, display_len(msgs(m_tagfile_enttype)));
		xcaddstr(gbl(win_commands), CMDS_TAG_TYPE_NAME,
			msgs(m_tagfile_enttypn));
		waddstr(gbl(win_commands), "  ");
		xcaddstr(gbl(win_commands), CMDS_TAG_TYPE_LINK,
			msgs(m_tagfile_enttypl));
		waddstr(gbl(win_commands), "  ");
		xcaddstr(gbl(win_commands), CMDS_TAG_TYPE_SOCK,
			msgs(m_tagfile_enttyps));
		waddstr(gbl(win_commands), "  ");
		xcaddstr(gbl(win_commands), CMDS_TAG_TYPE_UNKLINK,
			msgs(m_tagfile_enttypu));
		wrefresh(gbl(win_commands));
		bangnc(msgs(m_tagfile_chotag));
		while (TRUE)
		{
			c = xgetch(gbl(win_message));
			c = TO_LOWER(c);
			if (c == KEY_ESCAPE ||
				c == cmds(CMDS_TAG_TYPE_REG)  ||
				c == cmds(CMDS_TAG_TYPE_BLK)  ||
				c == cmds(CMDS_TAG_TYPE_CHR)  ||
				c == cmds(CMDS_TAG_TYPE_DOOR) ||
				c == cmds(CMDS_TAG_TYPE_FIFO) ||
				c == cmds(CMDS_TAG_TYPE_NAME) ||
				c == cmds(CMDS_TAG_TYPE_LINK) ||
				c == cmds(CMDS_TAG_TYPE_SOCK) ||
				c == cmds(CMDS_TAG_TYPE_UNKLINK) ||
				c == cmds(CMDS_TAG_COPYFILE))
				break;
		}
		bang("");
		if (c == KEY_ESCAPE)
			return (0);
		if (c == cmds(CMDS_TAG_COPYFILE))
		{
			werase(gbl(win_commands));
			wmove(gbl(win_commands), 1, 0);
			xaddstr(gbl(win_commands), msgs(m_tagfile_coptyp));
			wrefresh(gbl(win_commands));
			*input_str = 0;
			c = xgetstr(gbl(win_commands), input_str, XGL_FILE_TO_COPY,
				MAX_PATHLEN, 0, XG_FILEPATH);
			if (c <= 0)
				return (0);
			fn_get_abs_path(gbl(scr_cur)->path_name, input_str, buffer);
			if (os_lstat(buffer, &stbuf))
			{
				errsys(ER_CSN);
				return (0);
			}
			ti->tag_type = stbuf.st_mode & S_IFMT;
		}
		else
		{
			if (c == cmds(CMDS_TAG_TYPE_REG))
				ti->tag_type = S_IFREG;
			else if (c == cmds(CMDS_TAG_TYPE_BLK))
				ti->tag_type = S_IFBLK;
			else if (c == cmds(CMDS_TAG_TYPE_CHR))
				ti->tag_type = S_IFCHR;
			else if (c == cmds(CMDS_TAG_TYPE_DOOR))
				ti->tag_type = S_IFDOOR;
			else if (c == cmds(CMDS_TAG_TYPE_FIFO))
				ti->tag_type = S_IFIFO;
			else if (c == cmds(CMDS_TAG_TYPE_NAME))
				ti->tag_type = S_IFNAM;
			else if (c == cmds(CMDS_TAG_TYPE_LINK))
				ti->tag_type = S_IFLNK;
			else if (c == cmds(CMDS_TAG_TYPE_SOCK))
				ti->tag_type = S_IFSOCK;
			else if (c == cmds(CMDS_TAG_TYPE_UNKLINK))
				ti->tag_type = S_IFLNK, ti->tag_brk = TRUE;
			else
				return (0);
		}
		need_order = FALSE;
	}
	else if (c == cmds(CMDS_TAG_NODE))
	{
		gbl(scr_cur_sub_cmd) = CMDS_TAG_NODE;
		ti->tag_choice = cmds(CMDS_TAG_NODE);
		wmove(gbl(win_message), 0, 40);
		fk_msg(gbl(win_message), CMDS_DEST_DIR, msgs(m_tagfile_f2));
		wrefresh(gbl(win_message));
		bangnc("");
		werase(gbl(win_commands));
		wmove(gbl(win_commands), 0, 0);
		if (tag_mode == cmds(CMDS_COMMON_INVERT))
			xaddstr(gbl(win_commands), msgs(m_tagfile_inv2));
		else if (tag_mode == cmds(CMDS_COMMON_TAG))
			xaddstr(gbl(win_commands), msgs(m_tagfile_tag2));
		else if (tag_mode == cmds(CMDS_COMMON_UNTAG))
			xaddstr(gbl(win_commands), msgs(m_tagfile_unt2));
		xaddstr(gbl(win_commands), msgs(m_tagfile_amfisn));
		wmove(gbl(win_commands), 1, 0);
		xaddstr(gbl(win_commands), msgs(m_tagfile_entnod));
		wrefresh(gbl(win_commands));
		*input_str = 0;
		i = xgetstr_tr(gbl(win_commands), input_str, XGL_NODE, MAX_PATHLEN, 0,
			XG_PATHNAME, &t);
		if (i <= 0)
			return (0);

		fn_resolve_pathname(input_str);
		fn_get_abs_path(gbl(scr_cur)->path_name, input_str, dest_dir);
#if 0
		if (is_directory(dest_dir, 0))
			return (0);
#endif
		if (!t)
		{
			t = pathname_to_dirtree(dest_dir);
			if (!t)
			{
				errmsg(ER_DNL, "", ERR_ANY);
				return (0);
			}
		}
		n = get_root_of_dir(t);
		ti->tag_node = bfind(n->dir_list, t);
		need_order = FALSE;
	}

	if (need_order)
	{
		esc_msg();
		wrefresh(gbl(win_message));
		wmove(gbl(win_commands), 1, 0);
		xaddstr(gbl(win_commands), msgs(m_tagfile_entord));
		wrefresh(gbl(win_commands));
		while (TRUE)
		{
			c = xgetch(gbl(win_commands));
			if (c == KEY_ESCAPE || c == KEY_RETURN)
				return (0);
			if (c == cmds(CMDS_LESS_THAN) ||
				c == cmds(CMDS_GREATER_THAN) ||
				c == cmds(CMDS_EQUAL))
				break;
		}
		waddch(gbl(win_commands), c);
		wrefresh(gbl(win_commands));
		ti->tag_order = c;
	}

	return (1);
}

static int check_tag (FBLK *f, TAG_INFO *ti)
{
	if (ti->tag_choice == cmds(CMDS_TAG_PERMS))
	{
		return ((f->stbuf.st_mode & ti->tag_perm_mask) == ti->tag_perm_tag);
	}
	else if (ti->tag_choice == cmds(CMDS_TAG_INODE))
	{
		if (ti->tag_order == cmds(CMDS_LESS_THAN))
			return (f->stbuf.st_ino <  ti->tag_inode);
		if (ti->tag_order == cmds(CMDS_GREATER_THAN))
			return (f->stbuf.st_ino >  ti->tag_inode);
		if (ti->tag_order == cmds(CMDS_EQUAL))
			return (f->stbuf.st_ino == ti->tag_inode);
	}
	else if (ti->tag_choice == cmds(CMDS_TAG_LINKS))
	{
		if (ti->tag_order == cmds(CMDS_LESS_THAN))
			return (f->stbuf.st_nlink <  ti->tag_link_count);
		if (ti->tag_order == cmds(CMDS_GREATER_THAN))
			return (f->stbuf.st_nlink >  ti->tag_link_count);
		if (ti->tag_order == cmds(CMDS_EQUAL))
			return (f->stbuf.st_nlink == ti->tag_link_count);
	}
	else if (ti->tag_choice == cmds(CMDS_TAG_DATE))
	{
		if (ti->tag_order == cmds(CMDS_LESS_THAN))
			return (f->stbuf.st_mtime <  ti->tag_date);
		if (ti->tag_order == cmds(CMDS_GREATER_THAN))
			return (f->stbuf.st_mtime >  ti->tag_date+SECS_PER_DAY-1);
		if (ti->tag_order == cmds(CMDS_EQUAL))
			return (f->stbuf.st_mtime >= ti->tag_date &&
					f->stbuf.st_mtime <= ti->tag_date+SECS_PER_DAY);
	}
	else if (ti->tag_choice == cmds(CMDS_TAG_SIZE))
	{
		if (ti->tag_order == cmds(CMDS_LESS_THAN))
			return (f->stbuf.st_size  <  ti->tag_size);
		if (ti->tag_order == cmds(CMDS_GREATER_THAN))
			return (f->stbuf.st_size  >  ti->tag_size);
		if (ti->tag_order == cmds(CMDS_EQUAL))
			return (f->stbuf.st_size  == ti->tag_size);
	}
	else if (ti->tag_choice == cmds(CMDS_TAG_OWNER))
	{
		return (f->stbuf.st_uid == ti->tag_uid);
	}
	else if (ti->tag_choice == cmds(CMDS_TAG_GROUP))
	{
		return (f->stbuf.st_gid == ti->tag_gid);
	}
	else if (ti->tag_choice == cmds(CMDS_TAG_TYPE))
	{
		return ((f->stbuf.st_mode & S_IFMT) == ti->tag_type &&
				(ti->tag_brk ? (f->sym_mode == 0) : TRUE) );
	}
	return (0);
}

void tag_sub_node (TAG_INFO *ti)
{
	BLIST *b;
	int lev;

	tag_dir(ti->tag_node);
	lev = tdepth((TREE *)bid(ti->tag_node));
	for (b=bnext(ti->tag_node); b; b=bnext(b))
	{
		if (tdepth((TREE *)bid(b)) <= lev)
			break;
		tag_dir(b);
	}
}

void untag_sub_node (TAG_INFO *ti)
{
	BLIST *b;
	int lev;

	untag_dir(ti->tag_node);
	lev = tdepth((TREE *)bid(ti->tag_node));
	for (b=bnext(ti->tag_node); b; b=bnext(b))
	{
		if (tdepth((TREE *)bid(b)) <= lev)
			break;
		untag_dir(b);
	}
}

void invert_sub_node (TAG_INFO *ti)
{
	BLIST *b;
	int lev;

	invert_dir(ti->tag_node);
	lev = tdepth((TREE *)bid(ti->tag_node));
	for (b=bnext(ti->tag_node); b; b=bnext(b))
	{
		if (tdepth((TREE *)bid(b)) <= lev)
			break;
		invert_dir(b);
	}
}

void alt_invert_file (FBLK *f, TAG_INFO *ti)
{
	if (check_tag(f, ti))
		invert_file(f);
}

void alt_tag_file (FBLK *f, TAG_INFO *ti)
{
	if (check_tag(f, ti))
		tag_file(f);
}

void alt_untag_file (FBLK *f, TAG_INFO *ti)
{
	if (check_tag(f, ti))
		untag_file(f);
}

void alt_invert_dir (BLIST *dir_ptr, TAG_INFO *ti)
{
	BLIST *b;
	TREE *t;
	DBLK *d;

	t = (TREE *)bid(dir_ptr);
	d = (DBLK *)tid(t);
	for (b=d->mlist[gbl(scr_cur_no)]; b; b=bnext(b))
		alt_invert_file((FBLK *)bid(b), ti);
}

void alt_tag_dir (BLIST *dir_ptr, TAG_INFO *ti)
{
	BLIST *b;
	TREE *t;
	DBLK *d;

	t = (TREE *)bid(dir_ptr);
	d = (DBLK *)tid(t);
	for (b=d->mlist[gbl(scr_cur_no)]; b; b=bnext(b))
		alt_tag_file((FBLK *)bid(b), ti);
}

void alt_untag_dir (BLIST *dir_ptr, TAG_INFO *ti)
{
	BLIST *b;
	TREE *t;
	DBLK *d;

	t = (TREE *)bid(dir_ptr);
	d = (DBLK *)tid(t);
	for (b=d->mlist[gbl(scr_cur_no)]; b; b=bnext(b))
		alt_untag_file((FBLK *)bid(b), ti);
}
