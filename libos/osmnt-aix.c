/*------------------------------------------------------------------------
 *	Routines to read the mount table & get free-space for a file-system
 *
 *	platform: IBM AIX
 */
#include "oscommon.h"

#include <sys/statfs.h>
#include <sys/mntctl.h>
#include <sys/vmount.h>

/*------------------------------------------------------------------------
 * mount-table info struct
 */
struct mt_info
{
	struct vmount *	buffer;
	struct vmount *	bufptr;
	int				num_mnts;
	int				num_read;
};
typedef struct mt_info MT_INFO;

/*------------------------------------------------------------------------
 * os_mnt_open() - open the mount table
 */
void * os_mnt_open (void)
{
	MT_INFO *	mip;
	int			bufsize;
	int			rc;

	/*--------------------------------------------------------------------
	 * allocate mt_info struct
	 */
	mip = (MT_INFO *)MALLOC(sizeof(*mip));
	if (mip == 0)
	{
		return (0);
	}

	/*--------------------------------------------------------------------
	 * allocate vmount buffer
	 */
	bufsize = 1024;
	mip->buffer = (struct vmount *)MALLOC(bufsize);
	if (mip->buffer == 0)
	{
		FREE(mip);
		return (0);
	}

	rc = mntctl(MCTL_QUERY, bufsize, mip->buffer);
	if (rc == -1)
	{
		FREE(mip);
		return (0);
	}

	if (rc == 0)
	{
		bufsize = *(int *)mip->buffer;
		FREE(mip->buffer);

		mip->buffer = (struct vmount *)MALLOC(bufsize);
		if (mip->buffer == 0)
		{
			FREE(mip);
			return (0);
		}
	}

	/*--------------------------------------------------------------------
	 * get mount info
	 */
	rc = mntctl(MCTL_QUERY, bufsize, mip->buffer);
	if (rc == -1)
	{
		FREE(mip->buffer);
		FREE(mip);
		return (0);
	}

	/*--------------------------------------------------------------------
	 * initialize mt_info struct
	 */
	mip->num_mnts = rc;
	mip->bufptr   = mip->buffer;
	mip->num_read = 0;

	return (mip);
}

/*------------------------------------------------------------------------
 * os_mnt_close() - close the mount table
 */
void os_mnt_close (void *md)
{
	if (md != 0)
	{
		MT_INFO * mip = (MT_INFO *)md;

		/*----------------------------------------------------------------
		 * free mount info
		 */
		FREE(mip->buffer);

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
	int				rc;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (mip == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * get next entry if any left
	 */
	if (mip->num_read++ < mip->num_mnts)
	{
		struct vmount *	bufptr	= mip->bufptr;
		char *			d		= vmt2dataptr(bufptr, VMT_OBJECT);
		char *			f		= vmt2dataptr(bufptr, VMT_STUB);
		char *			n		= vmt2dataptr(bufptr, VMT_HOST);
		long			len		= bufptr->vmt_length;

		/*----------------------------------------------------------------
		 * get pointer to next vmount struct in buffer
		 */
		mip->bufptr = (struct vmount *)((char *)(mip->bufptr) + len);

		/*----------------------------------------------------------------
		 * extract info from it
		 */
		mptr->mt_flags = 0;
		if (bufptr->vmt_flags & MNT_REMOTE)
		{
			strcpy(mptr->mt_dev, n);
			strcat(mptr->mt_dev, ":");
			strcat(mptr->mt_dev, d);
			mptr->mt_flags |= MT_REMOTE;
		}
		else
		{
			strcpy(mptr->mt_dev, d);
		}

		if (bufptr->vmt_flags & MNT_READONLY)
			mptr->mt_flags |= MT_READONLY;

		strcpy(mptr->mt_dir, f);

		/*----------------------------------------------------------------
		 * stat the mount pount to get dev-info
		 */
		rc = os_stat(mptr->mt_dir, &stbuf);
		if (rc)
			return (-1);

		mptr->st_dev = stbuf.st_dev;

		return (0);
	}

	return (-1);
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
	rc = statfs((char *)path, &fsbuf);	/* BOGUS cast ! */
	if (rc)
		return (-1);

	/*--------------------------------------------------------------------
	 * calculate free space in 1K blocks
	 */
	free_amount = fsbuf.f_bfree * (fsbuf.f_bsize / 1024);
	if (free_amount < 0)
		free_amount = 0;

	return (free_amount);
}
