/*
 * file.h - definitions for file(1) program
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

#ifndef MAGCOMMON_H
#define MAGCOMMON_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "libsys.h"
#include "libmagic.h"

extern int			mag_fstat		(struct stat *sb,
										char *result);
extern int			mag_ascii		(unsigned char *buf, int nbytes,
										char *result);
extern int			mag_intl		(unsigned char *buf, int nbytes,
										char *result);
extern int			mag_tar			(unsigned char *buf, int nbytes,
										char *result);
extern int			mag_soft		(MAGIC *magics,
										unsigned char *buf, int nbytes,
										char *result);

extern void			mag_dump		(MAGIC *m);
extern unsigned int	mag_signex		(MAGIC *m, unsigned int);

#endif /* MAGCOMMON_H */
