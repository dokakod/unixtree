/*------------------------------------------------------------------------
 *	Routines to read the mount table & get free-space for a file-system
 *
 *	platform: Windows
 */
#include "oscommon.h"

/*------------------------------------------------------------------------
 * mount-table info struct
 */
struct mt_info
{
	int		drive;
	DWORD	drives;
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
	 * indicate no pop-up windows for errors
	 */
	SetErrorMode(SEM_FAILCRITICALERRORS);

	/*--------------------------------------------------------------------
	 * get drive info
	 */
	mip->drives	= GetLogicalDrives();
	mip->drive	= 0;

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
	MT_INFO *	mip = (MT_INFO *)md;
	char		path[MAX_PATHLEN];
	struct stat	stbuf;
	int			rc;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (mip == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * get next drive which has info for it
	 */
	for (; mip->drive < 26; mip->drive++)
	{
		if ( (mip->drives & (1 << mip->drive)) != 0)
			break;
	}

	if (mip->drive == 26)
		return (-1);

	/*--------------------------------------------------------------------
	 * get info for this drive
	 */
	sprintf(path, "%c:\\", 'a' + mip->drive++);
	rc = os_stat(path, &stbuf);
	if (rc == 0)
	{
		strcpy(mptr->mt_dev, path);
		strcpy(mptr->mt_dir, path);
		mptr->mt_flags = MT_NOCASE;
		mptr->st_dev   = stbuf.st_dev;

		return (0);
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * os_mnt_space() - get free space for a file-system a path is in
 */
long os_mnt_space (const char *path)
{
	DWORD	lSectorsPerCluster;
	DWORD	lBytesPerSector;
	DWORD	lNumberOfFreeClusters;
	DWORD	lTotalNumberOfClusters;
	BOOL	rc;

	long	cluster_size;
	long	blks_free;

	/*--------------------------------------------------------------------
	 * get fs status for path
	 */
	rc = GetDiskFreeSpace(path,
		&lSectorsPerCluster,
		&lBytesPerSector,
		&lNumberOfFreeClusters,
		&lTotalNumberOfClusters);
	if (! rc)
		return (-1);

	/*--------------------------------------------------------------------
	 * calculate free space in 1K blocks
	 */
	cluster_size = (lSectorsPerCluster * lBytesPerSector);
	blks_free    = lNumberOfFreeClusters * (cluster_size / 1024);

	return (blks_free);
}
