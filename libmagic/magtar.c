/*
 * is_tar() -- figure out whether file is a tar archive.
 *
 * Stolen (by the author!) from the public domain tar program:
 * Public Domain version written 26 Aug 1985 John Gilmore (ihnp4!hoptoad!gnu).
 *
 * @(#)list.c 1.18 9/23/86 Public Domain - gnu
 *
 * Comments changed and some code/comments reformatted
 * for file command by Ian Darwin.
 */

#include "magcommon.h"

/*
 * tar header
 */
union record
{
	char		charptr[256];
	struct header
	{
		char	name[100];
		char	mode[8];
		char	uid[8];
		char	gid[8];
		char	size[12];
		char	mtime[12];
		char	chksum[8];
		char	linkflag;
		char	linkname[100];
		char	magic[8];
		char	uname[32];
		char	gname[32];
		char	devmajor[8];
		char	devminor[8];
	} header;
};

/* The magic field is filled with this if uname and gname are valid. */
#define	TMAGIC		"ustar"

#define	isodigit(c)	( ((c) >= '0') && ((c) <= '7') )

/*
 * Quick and dirty octal conversion.
 *
 * Result is -1 if the field is invalid (all blank, or nonoctal).
 */
static int
from_oct(int digs, char *where)
{
	int	value;

	while (isspace((unsigned char)*where))
	{
		/* Skip spaces */
		where++;
		if (--digs <= 0)
			return -1;		/* All blank field */
	}

	value = 0;
	while (digs > 0 && isodigit(*where))
	{
		/* Scan til nonoctal */
		value = (value << 3) | (*where++ - '0');
		--digs;
	}

	if (digs > 0 && *where && !isspace((unsigned char)*where))
		return -1;			/* Ended on non-space/nul */

	return value;
}

/*
 * Return
 *	0 if the checksum is bad (i.e., probably not a tar archive),
 *	1 if tar file
 */
int
mag_tar (unsigned char *buf, int nbytes, char *result)
{
	union record *header = (union record *)buf;
	int	i;
	int	sum, recsum;
	char	*p;

	if (nbytes < sizeof(union record))
		return 0;

	recsum = from_oct(8,  header->header.chksum);

	sum = 0;
	p = header->charptr;
	for (i = sizeof(union record); --i >= 0;)
	{
		/*
		 * We can't use unsigned char here because of old compilers,
		 * e.g. V7.
		 */
		sum += 0xFF & *p++;
	}

	/* Adjust checksum to count the "chksum" field as blanks. */
	for (i = sizeof(header->header.chksum); --i >= 0;)
		sum -= 0xFF & header->header.chksum[i];
	sum += ' '* sizeof header->header.chksum;

	if (sum != recsum)
		return 0;	/* Not a tar archive */

	if (strcmp(header->header.magic, TMAGIC) == 0)
		strcat(result, "POSIX tar archive");
	else
		strcat(result, "tar archive");

	return 1;
}
