/*
 * fsmagic - magic based on filesystem info - directory, special files, etc.
 *
 * Copyright (c) Ian F. Darwin, 1987.
 * Written by Ian F. Darwin.
 *
 * This software is not subject to any license of the American Telephone
 * and Telegraph Company or of the Regents of the University of California.
 *
 * Permission is granted to anyone to use this software for any purpose on
 * any computer system, and to alter it and redistribute it freely, subject
 * to the following restrictions:
 *
 * 1. The author is not responsible for the consequences of use of this
 *    software, no matter how awful, even if they arise from flaws in it.
 *
 * 2. The origin of this software must not be misrepresented, either by
 *    explicit claim or by omission.  Since few users ever read sources,
 *    credits must appear in the documentation.
 *
 * 3. Altered versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.  Since few users
 *    ever read sources, credits must appear in the documentation.
 *
 * 4. This notice may not be removed or altered.
 */

#include "magcommon.h"

int
mag_fstat (struct stat *sb, char *result)
{
	if (sb->st_mode & S_ISUID) strcat(result, "setuid ");
	if (sb->st_mode & S_ISGID) strcat(result, "setgid ");
	if (sb->st_mode & S_ISVTX) strcat(result, "sticky ");

	switch (sb->st_mode & S_IFMT)
	{
	case S_IFDIR:
		strcat(result, "directory");
		return 1;

	case S_IFCHR:
		strcat(result, "character special");
		return 1;

	case S_IFBLK:
		strcat(result, "block special");
		return 1;

#ifdef	S_IFIFO
	case S_IFIFO:
		strcat(result, "fifo");
		return 1;
#endif

#ifdef	S_IFDOOR
	case S_IFDOOR:
		strcat(result, "door");
		return 1;
#endif

#ifdef	S_IFSOCK
	case S_IFSOCK:
		strcat(result, "socket");
		return 1;
#endif

	case S_IFREG:
		break;

	default:
		/* we will assume a regular file */
		break;
	}

	/*
	 * regular file, check next possibility
	 *
	 * If stat() tells us the file has zero length, report here that
	 * the file is empty, so we can skip all the work of opening and
	 * reading the file.
	 */
	if (sb->st_size == 0)
	{
		strcat(result, "empty");
		return 1;
	}

	return 0;
}
