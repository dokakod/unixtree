/*
 * file - find type of a file or files - main program.
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

static void
tryit (MAGIC *magics, unsigned char *buf, int nb, char *result)
{
	/* try tests in /etc/magic (or surrogate magic file) */
	if (mag_soft(magics, buf, nb, result))
		return;

	/* try known keywords, check whether it is ASCII */
	if (mag_ascii(buf, nb, result))
		return;

	/* see if it's international language text */
	if (mag_intl(buf, nb, result))
		return;

	/* abandon hope, all ye who remain here */
	strcat(result, "data");
}

int
mag_check (MAGIC *magics, const char *filename, char *result)
{
	FILE *			fp;
	unsigned char	buf[HOWMANY+1];
	int				nbytes;
	struct stat		stbuf;
	int				rc;

	*result = 0;

	/*
	 * first try judging the file based on its filesystem status
	 */
	while (1)
	{
		rc = stat(filename, &stbuf);
		if (rc < 0)
		{
			if (errno == EINTR)
				continue;

			sprintf(result, "cannot stat: %s", strerror(errno));
			return (-1);
		}
		break;
	}

	if (mag_fstat(&stbuf, result) != 0)
	{
		return (0);
	}

	fp = fopen(filename, "rb");
	if (fp == 0)
	{
		sprintf(result, "cannot open: %s", strerror(errno));
		return (-1);
	}

	/*
	 * try looking at the first HOWMANY bytes
	 */
	nbytes = fread(buf, 1, HOWMANY, fp);
	fclose(fp);

	if (nbytes < 0)
	{
		sprintf(result, "cannot read: %s", strerror(errno));
		return (-1);
	}

	if (nbytes == 0)
	{
		sprintf(result, "empty");
	}
	else
	{
		buf[nbytes] = 0;	/* null-terminate it */
		tryit(magics, buf, nbytes, result);
	}

	return (0);
}
