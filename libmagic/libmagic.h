/*------------------------------------------------------------------------
 * header for libmagic
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
 *
 * This source has been seriously hacked.
 */

#ifndef LIBMAGIC_H
#define LIBMAGIC_H

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * MAGIC struct
 */
#define HOWMANY		16384		/* how much of the file to look at */

#define MAXDESC		50			/* max len of text description */
#define MAXstring	32			/* max len of "string" types */
#define MAXOFFS		100			/* max number of offsets */

#define INDIR		1			/* if '>(...)' appears,  */
#define	UNSIGNED	2			/* comparison is unsigned */
#define ADD			4			/* if '>&' appears,  */
#define ALSO		8			/* if '&' appears */

#define BYTE	1
#define STRING	2
#define SHORT	3
#define BESHORT	4
#define LESHORT	5
#define LONG	6
#define BELONG	7
#define LELONG	8

union valuetype
{
	unsigned char	b;
	unsigned short	h;
	unsigned int	l;
	char			s[MAXstring];
	unsigned char	hs[2];		/* 2 bytes of a fixed-endian "short" */
	unsigned char	hl[4];		/* 4 bytes of a fixed-endian "long" */
};
typedef union valuetype VALUETYPE;

typedef struct magic MAGIC;
struct magic
{
	MAGIC *			next;			/* pointer to next entry */

	int				flag;
	int				cont_level;		/* level of ">" */
	struct
	{
		unsigned char	type;		/* byte short long */
		int				offset;		/* offset from indirection */
	} in;
	int				offset;			/* offset to magic number */
	unsigned char	reln;			/* relation (0=eq, '>'=gt, etc) */
	unsigned char	type;			/* int, short, long or string. */
	int				vallen;			/* length of string value, if any */
	VALUETYPE		value;
	unsigned int	mask;			/* mask before comparison with value */
	int				nospflag;		/* supress space character */
	char			desc[MAXDESC];	/* description */
};

/*------------------------------------------------------------------------
 * functions
 */
extern MAGIC *		mag_parse		(const char *mag_path, int check);
extern void			mag_free		(MAGIC *magics);

extern int			mag_check		(MAGIC *magics, const char *filename,
										char *result);


#ifdef __cplusplus
}
#endif

#endif /* LIBMAGIC_H */
