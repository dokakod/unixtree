/*------------------------------------------------------------------------
 *	Routines to read the mount table & get free-space for a file-system
 *
 *	platform: generic SVR3
 */
#include "oscommon.h"

#include <sys/statfs.h>
#include <mnttab.h>

#ifndef MOUNTED
#define MOUNTED		"/etc/mnttab"
#endif

/*------------------------------------------------------------------------
 * mount-table info struct
 */
struct mt_info
{
	int	mount;
};
typedef struct mt_info MT_INFO;

/*------------------------------------------------------------------------
 * os_mnt_open() - open the mount table
 */
void * os_mnt_open (void)
{
	MT_INFO *	mip;

	/*--------------------------------------------------------------------
	 * allocate mt_info struct
	 */
	mip = (MT_INFO *)MALLOC(sizeof(*mip));
	if (mip == 0)
	{
		return (0);
	}

	/*--------------------------------------------------------------------
	 * open mount table
	 */
	mip->mount = open(MOUNTED, O_RDONLY);
	if (mip->mount == -1)
	{
		FREE(mip);
		return (0);
	}

	return (mip);
}

/*------------------------------------------------------------------------
 * os_mnt_close() - close the mount table
 */
void os_mnt_close (void *md)
{
	if (md != 0)
	{
		MT_INFO *mip = (MT_INFO *)md;

		/*----------------------------------------------------------------
		 * close mount table
		 */
		close(mip->mount);

		/*----------------------------------------------------------------
		 * free mt_info struct
		 */
		FREE(mip);
	}
}

/*------------------------------------------------------------------------
 * os_mnt_read() - read the next entry from the mount table
 */
int os_mnt_read (void *md, MOUNT_INFO *mptr)
{
	MT_INFO *		mip = (MT_INFO *)md;
	struct stat		stbuf;
	struct mnttab	table;
	int				mnt_size = sizeof(table);
	int				rc;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (mip == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * get next real entry
	 */
	while (TRUE)
	{
		rc = read(mip->mount, &table, mnt_size);
		if (rc <= 0)
			return (-1);

		if (*(table.mt_dev) != 0)			/* skip null entry */
			break;
	}

	/*--------------------------------------------------------------------
	 * extract info
	 */
	strcpy(mptr->mt_dev, table.mt_dev);
	strcpy(mptr->mt_dir, table.mt_filsys);
	mptr->mt_flags = 0;

	if (table.mt_ro_flg)
		mptr->mt_flags |= MT_READONLY;

	/*--------------------------------------------------------------------
	 * stat the mount point to get dev info
	 */
	rc = os_stat(mptr->mt_dir, &stbuf);
	if (rc)
		return (-1);

	mptr->st_dev = stbuf.st_dev;

	return (0);
}

/*------------------------------------------------------------------------
 * os_mnt_space() - get free space for a file-system a path is in
 */
long os_mnt_space (const char *path)
{
	struct statfs	fsbuf;
	long			free_amount;
	int				rc;

	/*--------------------------------------------------------------------
	 * get fs status for path
	 */
	rc = statfs(path, &fsbuf, sizeof(fsbuf), 0);
	if (rc)
		return (-1);

	/*--------------------------------------------------------------------
	 * calculate free space in 1K blocks
	 */
	free_amount = fsbuf.f_bfree / 2;
	if (free_amount < 0)
		free_amount = 0;

	return (free_amount);
}
