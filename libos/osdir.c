/*------------------------------------------------------------------------
 *	generic directory reading routines
 */
#include "oscommon.h"

#if V_UNIX
#  include <dirent.h>
	typedef struct dirent DIRECT;
#endif

/*------------------------------------------------------------------------
 * real struct
 */
typedef struct utdir_int
{
#if V_UNIX
	DIR	*			dirfile;
#else
	HANDLE			find_handle;
	WIN32_FIND_DATA	wdata;
	int				wdata_valid;
#endif
} UTDIR_INT;

/*------------------------------------------------------------------------
 * test for . & ..
 */
#define	isdot(fn)	(strcmp(fn, ".") == 0 || strcmp(fn, "..") == 0)

/*------------------------------------------------------------------------
 * os_dir_open()
 *
 * open a directory for reading
 *
 * returns: a UTDIR pointer or NULL
 */
UTDIR *
os_dir_open (const char *path)
{
	UTDIR_INT *	ut;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (path == 0 || *path == 0)
		return (0);

	/*--------------------------------------------------------------------
	 * allocate struct
	 */
	ut = (UTDIR_INT *)MALLOC(sizeof(*ut));
	if (ut == 0)
		return (0);

#if V_UNIX
	/*--------------------------------------------------------------------
	 * UNIX
	 */
	ut->dirfile = opendir(path);
	if (ut->dirfile == 0)
	{
		FREE(ut);
		return (0);
	}
#else
	/*--------------------------------------------------------------------
	 * NT / WIN32
	 *
	 * Construct the glob pattern required to search for all the files
	 * in the mentioned directory and create the
	 */
	{
		char	nt_path[MAX_PATHLEN];
		char *	p = nt_path;

		/*----------------------------------------------------------------
		 * copy over the user-provided pathname and make sure it
		 * properly ends with  \*  This means we have to
		 * take special care to look for any trailing slash or
		 * backslash provided by the user and drop it, then add
		 * in the proper wildcard.
		 */
		while ( *p = *path++ )		/* copy name literally */
			p++;

		while ( p > nt_path  &&  (p[-1]=='/' ||  p[-1]=='\\') )
		{
			p--;
		}

		*p++ = '\\';	/* add     \*     at the end  */
		*p++ = '*';
		*p   = '\0';

		/*----------------------------------------------------------------
		 * Now try to start this process by finding the first proper
		 * file of the type requested. If we get either of the
		 * failure indicators (NULL or INVALID_HANDLE_VALUE) we just
		 * bail on the whole operation and return failure.
		 */
		ut->find_handle = FindFirstFile(nt_path, &ut->wdata);

		if (ut->find_handle == INVALID_HANDLE_VALUE)
		{
			FREE(ut);
			return (0);
		}

		/*----------------------------------------------------------------
		 * This "open" procedure actually read in the first entry.
		 * So we just note that we have valid data & let the first
		 * "read" find it.
		 */
		ut->wdata_valid = TRUE;
	}
#endif

	return (UTDIR *)ut;
}

/*------------------------------------------------------------------------
 * os_dir_close()
 *
 * close a directory & free the associated struct
 */
int
os_dir_close (UTDIR *utdir)
{
	UTDIR_INT *	ut;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (utdir == 0)
		return (-1);

	ut = (UTDIR_INT *)utdir;

#if V_UNIX
	/*--------------------------------------------------------------------
	 * UNIX
	 */
	if (ut->dirfile != 0)
	{
		closedir(ut->dirfile);
	}
#else
	/*--------------------------------------------------------------------
	 * NT / WIN32
	 *
	 * This "find_handle" must be closed, but since it's not a real
	 * live kernel object handle, we've seen that horrible failure
	 * occurs if garbage data is passed to FindClose().  We have seen
	 * the main thread of a daemon *block* in this case.
	 */
	if (ut->find_handle != INVALID_HANDLE_VALUE)
	{
		FindClose(ut->find_handle);
	}
#endif

	FREE(ut);

	return (0);
}

/*------------------------------------------------------------------------
 * os_dir_read()
 *
 * read in the next directory entry
 *
 * returns: 0 if another found, -1 if EOF or error
 *
 * Note: . & .. are skipped if present
 */
int
os_dir_read(UTDIR *utdir, char *filename)
{
	UTDIR_INT *	ut;

	/*--------------------------------------------------------------------
	 * check for valid pointer
	 */
	if (utdir == 0)
		return (-1);

	ut = (UTDIR_INT *)utdir;

#if V_UNIX
	/*--------------------------------------------------------------------
	 * UNIX
	 */
	{
		DIRECT *dp;
		char *name;

		/*----------------------------------------------------------------
		 * bail if invalid pointer
		 */
		if (ut->dirfile == 0)
			return (-1);

		/*----------------------------------------------------------------
		 * read until we get a valid entry
		 */
		while (TRUE)
		{
			/*------------------------------------------------------------
			 * deal with interrupted system calls
			 */
			errno = 0;
			dp = readdir(ut->dirfile);
			if (dp == 0)
			{
				if (errno == EINTR)
					continue;
				return (-1);
			}

			name = dp->d_name;

			/*------------------------------------------------------------
			 * skip . & ..
			 */
			if ( isdot(name) )
				continue;

			break;
		}
		strcpy(filename, name);

		return 0;
	}
#else
	/*--------------------------------------------------------------------
	 * NT
	 *
	 * If we drop out of this loop because of failure, it means we have
	 * almost certainly reached the end of the directory so we just
	 * go ahead and close this handle right now.
	 */

	if ( ut->find_handle == INVALID_HANDLE_VALUE )
		return -1;

	while ( TRUE )
	{
		char	*p;

		/*----------------------------------------------------------------
		 * if there is not a "cached" entry, then try to read in another
		 */
		if (! ut->wdata_valid)
		{
			if (! FindNextFile(ut->find_handle, &ut->wdata) )
				break;
		}
		ut->wdata_valid = FALSE;

		p = ut->wdata.cFileName;

		/*----------------------------------------------------------------
		 * skip over . & ..
		 */
		if (isdot(p) )
			continue;

		/*----------------------------------------------------------------
		 * copy filename
		 */
		strcpy(filename, p);

		return 0;
	}

	FindClose(ut->find_handle);
	ut->find_handle = INVALID_HANDLE_VALUE;

	return -1;
#endif
}
