/*------------------------------------------------------------------------
 * Routines to process the mount table
 *
 * ml_get_mounts (ml)					read the mount table
 * ml_free_mounts (ml)					clear the mount table
 * ml_get_num_mounts (ml)				get number of entries in table
 *
 * ml_get_mount_ent_by_num (ml, n)		get nth mount entry
 * ml_get_mount_ent_by_dev (ml, dev)	get mount entry for a device
 * ml_get_free_by_dev (ml, dev)			get free space on a device
 *
 * ml_is_remote_filesys (ml, dev)		check if device is remotely mounted
 * ml_is_readonly_filesys (ml, dev)		check if device is mounted read only
 * ml_is_nocase_filesys (ml, dev)		check if case-insensitive file system
 */
#include "utcommon.h"

/*------------------------------------------------------------------------
 * alloc routine
 */
static MOUNT_INFO * os_mnt_alloc (MOUNT_INFO *m)
{
	MOUNT_INFO *mp;

	mp = (MOUNT_INFO *)MALLOC(sizeof(*m));
	if (mp != 0)
		memcpy(mp, m, sizeof(*m));

	return (mp);
}

/*------------------------------------------------------------------------
 * mntlist routines
 */
BLIST * ml_get_mounts (BLIST *ml)
{
	BLIST *			b;
	MOUNT_INFO		m;
	MOUNT_INFO *	mp;
	void *			md;
	int				rc;

	if (ml != 0)
		ml = ml_free_mounts(ml);

	md = os_mnt_open();
	if (md == 0)
		return (ml);

	while (TRUE)
	{
		rc = os_mnt_read(md, &m);
		if (rc)
			break;

		mp = os_mnt_alloc(&m);
		if (mp == 0)
			break;

		b = BNEW(mp);
		if (b == 0)
		{
			FREE(mp);
			break;
		}

		ml = bappend(ml, b);
	}

	os_mnt_close(md);

	return (ml);
}

BLIST * ml_free_mounts (BLIST *ml)
{
	return BSCRAP(ml, TRUE);
}

int ml_get_num_mounts (BLIST *ml)
{
	return (bcount(ml));
}

MOUNT_INFO *ml_get_mount_ent_by_num (BLIST *ml, int n)
{
	MOUNT_INFO *	m = 0;
	BLIST *			b;

	b = bnth(ml, n);
	if (b)
		m = (MOUNT_INFO *)bid(b);

	return (m);
}

MOUNT_INFO *ml_get_mount_ent_by_dev (BLIST *ml, int dev)
{
	MOUNT_INFO *	m;
	BLIST *			b;

	for (b=ml; b; b=bnext(b))
	{
		m = (MOUNT_INFO *)bid(b);
		if ((int)m->st_dev == dev)
		{
			return (m);
		}
	}
	return (0);
}

long ml_get_free_by_dev (BLIST *ml, int dev)
{
	MOUNT_INFO *m;

	m = ml_get_mount_ent_by_dev(ml, dev);
	if (m)
	{
		return (os_mnt_space(m->mt_dir));
	}
	return (-1);
}

int ml_is_remote_filesys (BLIST *ml, int dev)
{
	MOUNT_INFO *m;

	m = ml_get_mount_ent_by_dev(ml, dev);
	if (m)
	{
		if (m->mt_flags & MT_REMOTE)
			return (TRUE);
	}
	return (FALSE);
}

int ml_is_readonly_filesys (BLIST *ml, int dev)
{
	MOUNT_INFO *m;

	m = ml_get_mount_ent_by_dev(ml, dev);
	if (m)
	{
		if (m->mt_flags & MT_READONLY)
			return (TRUE);
	}
	return (FALSE);
}

int ml_is_nocase_filesys (BLIST *ml, int dev)
{
	MOUNT_INFO *m;

	m = ml_get_mount_ent_by_dev(ml, dev);
	if (m)
	{
		if (m->mt_flags & MT_NOCASE)
			return (TRUE);
	}
	return (FALSE);
}
