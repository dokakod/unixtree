/*------------------------------------------------------------------------
 *	Routines to read the mount table & get free-space for a file-system
 *
 *	platform: generic SVR4
 */
#include "oscommon.h"

#include <sys/mnttab.h>
#include <sys/statvfs.h>

#ifndef MOUNTED
#define MOUNTED		"/etc/mnttab"
#endif

#define REMOTE_SYS		0x8000		/* remote file flag in st_dev */

/*------------------------------------------------------------------------
 * mount-table info struct
 */
struct mt_info
{
	FILE *	filep;
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
	mip->filep = fopen(MOUNTED, "r");
	if (mip->filep == 0)
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
		fclose(mip->filep);

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
	struct mnttab	mnt;
	struct stat		stbuf;
	int				rc;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (mip == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * get next entry
	 */
	rc = getmntent(mip->filep, &mnt);
	if (rc)
		return (-1);

	/*--------------------------------------------------------------------
	 * extract info
	 */
	strcpy(mptr->mt_dev, mnt.mnt_special);
	strcpy(mptr->mt_dir, mnt.mnt_mountp);
	mptr->mt_flags = 0;

	/*--------------------------------------------------------------------
	 * stat the mount point to get dev info
	 */
	rc = os_stat(mptr->mt_dir, &stbuf);
	if (rc)
		return (-1);

	mptr->st_dev = stbuf.st_dev;

	if (stbuf.st_dev & REMOTE_SYS)
		mptr->mt_flags |= MT_REMOTE;

	return (0);
}

/*------------------------------------------------------------------------
 * os_mnt_space() - get free space for a file-system a path is in
 */
long os_mnt_space (const char *path)
{
	struct statvfs	fsbuf;
	long			free_amount;
	int				rc;

	/*--------------------------------------------------------------------
	 * get fs status for path
	 */
	rc = statvfs(path, &fsbuf);
	if (rc)
		return (-1);

	/*--------------------------------------------------------------------
	 * calculate free space in 1K blocks
	 */
	if (fsbuf.f_frsize == 512)
		free_amount = fsbuf.f_bavail / 2;
	else
		free_amount = fsbuf.f_bavail * (fsbuf.f_frsize / 1024);

	if (free_amount < 0)
		free_amount = 0;

	return (free_amount);
}
