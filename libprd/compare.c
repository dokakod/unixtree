/*------------------------------------------------------------------------
 * process the "compare file/dir" cmds
 */
#include "libprd.h"

static int compare_names (const char *a, const char *b)
{
	if (opt(ignore_filename_case))
		return strccmp(a, b);
	else
		return strcmp(a, b);
}

static int cmp_date (const void *a, const void *b)
{
	int rc = 0;
	FBLK *fa = *(FBLK **)a;
	FBLK *fb = *(FBLK **)b;
	char patha[MAX_PATHLEN];
	char pathb[MAX_PATHLEN];

	if (opt(sort_by_path))
	{
		if (fa->dir != fb->dir)
		{
			dblk_to_dirname(fa->dir, patha);
			dblk_to_dirname(fb->dir, pathb);
			rc = compare_names(patha, pathb);
		}
	}

	if (rc == 0)
	{
		switch (opt(date_type))
		{
		case date_modified:
			rc = fa->stbuf.st_mtime - fb->stbuf.st_mtime;
			break;
		case date_accessed:
			rc = fa->stbuf.st_atime - fb->stbuf.st_atime;
			break;
		case date_created:
			rc = fa->stbuf.st_ctime - fb->stbuf.st_ctime;
			break;
		}
		/* add secondary sort on name if match */
		if (rc == 0)
			rc = compare_names(FULLNAME(fa), FULLNAME(fb));
	}

	if (opt(sort_order) == sort_desc)
		rc = -rc;
	return (rc);
}

static int cmp_ext (const void *a, const void *b)
{
	int rc = 0;
	FBLK *fa = *(FBLK **)a;
	FBLK *fb = *(FBLK **)b;
	char patha[MAX_PATHLEN];
	char pathb[MAX_PATHLEN];
	const char *pa;
	const char *pb;

	if (opt(sort_by_path))
	{
		if (fa->dir != fb->dir)
		{
			dblk_to_dirname(fa->dir, patha);
			dblk_to_dirname(fb->dir, pathb);
			rc = compare_names(patha, pathb);
		}
	}

	if (rc == 0)
	{
		pa = strrchr(FULLNAME(fa), '.');
		if (!pa)
			pa = "";
		pb = strrchr(FULLNAME(fb), '.');
		if (!pb)
			pb = "";
		rc = compare_names(pa, pb);
		/* add secondary sort on name if match */
		if (rc == 0)
			rc = compare_names(FULLNAME(fa), FULLNAME(fb));
	}

	if (opt(sort_order) == sort_desc)
		rc = -rc;
	return (rc);
}

static int cmp_name (const void *a, const void *b)
{
	int rc = 0;
	FBLK *fa = *(FBLK **)a;
	FBLK *fb = *(FBLK **)b;
	char patha[MAX_PATHLEN];
	char pathb[MAX_PATHLEN];

	if (opt(sort_by_path))
	{
		if (fa->dir != fb->dir)
		{
			dblk_to_dirname(fa->dir, patha);
			dblk_to_dirname(fb->dir, pathb);
			rc = compare_names(patha, pathb);
		}
	}

	if (rc == 0)
	{
		rc = compare_names(FULLNAME(fa), FULLNAME(fb));
	}

	if (opt(sort_order) == sort_desc)
		rc = -rc;
	return (rc);
}

static int cmp_inode (const void *a, const void *b)
{
	int rc = 0;
	FBLK *fa = *(FBLK **)a;
	FBLK *fb = *(FBLK **)b;
	char patha[MAX_PATHLEN];
	char pathb[MAX_PATHLEN];

	if (opt(sort_by_path))
	{
		if (fa->dir != fb->dir)
		{
			dblk_to_dirname(fa->dir, patha);
			dblk_to_dirname(fb->dir, pathb);
			rc = compare_names(patha, pathb);
		}
	}

	if (rc == 0)
	{
		rc = fa->stbuf.st_ino - fb->stbuf.st_ino;
		/* add secondary sort on name if match */
		if (rc == 0)
			rc = compare_names(FULLNAME(fa), FULLNAME(fb));
	}

	if (opt(sort_order) == sort_desc)
		rc = -rc;
	return (rc);
}

static int cmp_links (const void *a, const void *b)
{
	int rc = 0;
	FBLK *fa = *(FBLK **)a;
	FBLK *fb = *(FBLK **)b;
	char patha[MAX_PATHLEN];
	char pathb[MAX_PATHLEN];

	if (opt(sort_by_path))
	{
		if (fa->dir != fb->dir)
		{
			dblk_to_dirname(fa->dir, patha);
			dblk_to_dirname(fb->dir, pathb);
			rc = compare_names(patha, pathb);
		}
	}

	if (rc == 0)
	{
		rc = fa->stbuf.st_nlink - fb->stbuf.st_nlink;
		/* add secondary sort on name if match */
		if (rc == 0)
			rc = compare_names(FULLNAME(fa), FULLNAME(fb));
	}

	if (opt(sort_order) == sort_desc)
		rc = -rc;
	return (rc);
}

static int cmp_size (const void *a, const void *b)
{
	int rc = 0;
	FBLK *fa = *(FBLK **)a;
	FBLK *fb = *(FBLK **)b;
	char patha[MAX_PATHLEN];
	char pathb[MAX_PATHLEN];

	if (opt(sort_by_path))
	{
		if (fa->dir != fb->dir)
		{
			dblk_to_dirname(fa->dir, patha);
			dblk_to_dirname(fb->dir, pathb);
			rc = compare_names(patha, pathb);
		}
	}

	if (rc == 0)
	{
		rc = fa->stbuf.st_size - fb->stbuf.st_size;
		/* add secondary sort on name if match */
		if (rc == 0)
			rc = compare_names(FULLNAME(fa), FULLNAME(fb));
	}

	if (opt(sort_order) == sort_desc)
		rc = -rc;
	return (rc);
}

/* ARGSUSED */
static int cmp_unsorted (const void *a, const void *b)
{
	return (0);
}

static int (*cmp_rtn[])(const void *p1, const void *p2) =
{
	cmp_name,
	cmp_ext,
	cmp_date,
	cmp_inode,
	cmp_links,
	cmp_size,
	cmp_unsorted
};

int compare_files (const void *a, const void *b)
{
	return (cmp_rtn[opt(sort_type)])(a, b);
}

int compare_dirs (const void *a, const void *b)
{
	DBLK *da = (DBLK *)(tid((TREE *)a));
	DBLK *db = (DBLK *)(tid((TREE *)b));

	return ( compare_names(FULLNAME(da), FULLNAME(db)) );
}

int cmp_archloc (const void *a, const void *b)
{
	int rc = 0;
	FBLK *fa = *(FBLK **)a;
	FBLK *fb = *(FBLK **)b;
	NBLK *na;
	NBLK *nb;
	ABLK *aa;
	ABLK *ab;

	if (gbl(scr_cur)->command_mode == m_global ||
	    gbl(scr_cur)->command_mode == m_tag_global)
	{
		na = get_root_of_file(fa);
		nb = get_root_of_file(fb);
		aa = (ABLK *)na->node_sub_blk;
		ab = (ABLK *)nb->node_sub_blk;
		rc = compare_names(aa->arch_name, ab->arch_name);
		if (rc)
			return (rc);
	}

	rc = fa->archive_vol - fb->archive_vol;
	if (rc == 0)
		rc = fa->archive_loc - fb->archive_loc;

	return (rc);
}
