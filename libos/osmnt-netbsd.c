/*------------------------------------------------------------------------
 *	Routines to read the mount table & get free-space for a file-system
 *
 *	platform: NetBSD
 */
#include "oscommon.h"

#include <sys/param.h>
#include <sys/ucred.h>
#include <sys/mount.h>

/*------------------------------------------------------------------------
 * mount-table info struct
 */
struct mt_info
{
	struct statfs *	fsptr;
	int				num;
	int				cnt;
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
	 * get mount info
	 */
	mip->fsptr = 0;
	mip->num   = getmntinfo(&mip->fsptr, MNT_WAIT);
	if (mip->num <= 0)
	{
		return (0);
		FREE(mip);
	}

	/*--------------------------------------------------------------------
	 * initialize mt_info struct
	 */
	mip->cnt   = 0;

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
	struct statfs *	fsptr;
	int				rc;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (mip == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * check if all entries processed
	 */
	if (mip->cnt >= mip->num)
		return (-1);

	/*--------------------------------------------------------------------
	 * get pointer to next entry
	 */
	fsptr = mip->fsptr + mip->cnt++;

	/*--------------------------------------------------------------------
	 * extract info
	 */
	strcpy(mptr->mt_dev, fsptr->f_mntfromname);
	strcpy(mptr->mt_dir, fsptr->f_mntonname);
	mptr->mt_flags = 0;

	if (fsptr->f_flags & MNT_RDONLY)
		mptr->mt_flags |= MT_READONLY;

	if (strcmp(fsptr->f_fstypename, MOUNT_NFS) == 0)
		mptr->mt_flags |= MT_REMOTE;

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
	rc = statfs(path, &fsbuf);
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
