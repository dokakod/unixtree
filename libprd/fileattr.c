/*------------------------------------------------------------------------
 * process fileattr files
 */
#include "libprd.h"

/*------------------------------------------------------------------------
 * check a file block against a list
 */
static int fileattr_match (FILEATTR *fa, FBLK *f)
{
	char	dir_name[MAX_PATHLEN];
	int		match		= FALSE;
	int		secs_in_day	= (24 * 60 * 60);
	int		num;
	int		val;

	switch (fa->type)
	{
	case FA_ATTR_ATIME:
		num = time(0) - fa->data.nums.data * secs_in_day;
		val = f->stbuf.st_atime;

	comp_num:
		switch (fa->data.nums.comp)
		{
		case FA_COMP_GE:	match = (val >= num);	break;
		case FA_COMP_GT:	match = (val >  num);	break;
		case FA_COMP_LE:	match = (val <= num);	break;
		case FA_COMP_LT:	match = (val <  num);	break;
		case FA_COMP_EQ:	match = (val == num);	break;
		case FA_COMP_NE:	match = (val != num);	break;
		}
		break;

	case FA_ATTR_CTIME:
		num = time(0) - fa->data.nums.data * secs_in_day;
		val = f->stbuf.st_ctime;
		goto comp_num;

	case FA_ATTR_MTIME:
		num = time(0) - fa->data.nums.data * secs_in_day;
		val = f->stbuf.st_mtime;
		goto comp_num;

	case FA_ATTR_INODE:
		num = fa->data.nums.data;
		val = f->stbuf.st_ino;
		goto comp_num;

	case FA_ATTR_LINKS:
		num = fa->data.nums.data;
		val = f->stbuf.st_nlink;
		goto comp_num;

	case FA_ATTR_SIZE:
		num = fa->data.nums.data;
		val = f->stbuf.st_size;
		goto comp_num;

	case FA_ATTR_OWNER:
		num = fa->data.nums.data;
		val = f->stbuf.st_uid;
		goto comp_num;

	case FA_ATTR_GROUP:
		num = fa->data.nums.data;
		val = f->stbuf.st_gid;
		goto comp_num;

	case FA_ATTR_PERMS:
		val = f->stbuf.st_mode & fa->data.perms.and_mask;
		num = fa->data.perms.or_mask;
		match = (num == val);
		break;

	case FA_ATTR_TYPE:
		switch (fa->data.types.type)
		{
		case FA_TYPE_REG:
		case FA_TYPE_CHR:
		case FA_TYPE_BLK:
		case FA_TYPE_LINK:
		case FA_TYPE_FIFO:
		case FA_TYPE_PIPE:
		case FA_TYPE_SOCK:
		case FA_TYPE_DOOR:
			match = (fa->data.types.mode == (f->stbuf.st_mode & S_IFMT));
			break;

		case FA_TYPE_UNK:
			match = (fa->data.types.mode == (f->stbuf.st_mode & S_IFMT) &&
					 f->sym_mode == 0);
			break;
		}
		if (fa->data.types.allb)
			match = ! match;
		break;

	case FA_ATTR_NAME:
		match = match_names(FULLNAME(f), fa->data.names.spec);
		if (fa->data.names.allb)
			match = ! match;
		break;

	case FA_ATTR_DIR:
		dblk_to_dirname(f->dir, dir_name);
		match = fn_is_dir_in_dirname(fa->data.names.spec, dir_name);
		if (fa->data.names.allb)
			match = ! match;
		break;
	}

	return (match);
}

attr_t
fileattr_attr (BLIST *flist, FBLK *f)
{
	BLIST *	l;
	int		skip = FALSE;

	for (l=flist; l; l=bnext(l))
	{
		FILEATTR *	fa = (FILEATTR *)bid(l);
		int			match;

		if (skip)
		{
			if (! fa->more)
				skip = FALSE;
			continue;
		}

		match = fileattr_match(fa, f);
		if (match)
		{
			if (! fa->more)
				return (fa->attr);
		}
		else
		{
			if (fa->more)
				skip = TRUE;
		}
	}

	return (A_UNSET);
}
