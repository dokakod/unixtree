/*
   Copyright (C) 1988, 1989, 1992, 1993 Free Software Foundation, Inc.

This file is part of GNU DIFF.

GNU DIFF is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU DIFF is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU DIFF; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* This statement applies to all files in this library */
/* They have been seriously hacked from the original GNU-DIFF version */

/*------------------------------------------------------------------------
 * GNU DIFF internal diff structs & definitions
 */
#ifndef DIFFSYS_H
#define DIFFSYS_H

/*------------------------------------------------------------------------
 * System dependent declarations.
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>

#include "libsys.h"
#include "libdiff.h"

/*------------------------------------------------------------------------
 * misc definitions
 */
#ifndef INT_MAX
#  define INT_MAX				2147483647
#endif

#ifndef STAT_BLOCKSIZE
#  define STAT_BLOCKSIZE(s)		(8 * 1024)
#endif

#ifdef min
#  undef min
#endif
#define min(a,b)				((a) <= (b) ? (a) : (b))

#ifdef max
#  undef max
#endif
#define max(a,b)				((a) >= (b) ? (a) : (b))

#ifndef same_file
#    define same_file(s,t)		( \
									((s)->st_ino != 0 && (t)->st_ino != 0) && \
									((s)->st_ino == (t)->st_ino) && \
									((s)->st_dev == (t)->st_dev) \
								)
#endif

/*------------------------------------------------------------------------
 * The result of comparison is an "edit script": a chain of `CHANGE'.
 * Each `CHANGE' represents one place where some lines are deleted
 * and some are inserted.
 *
 * LINE0 and LINE1 are the first affected lines in the two files (origin 0).
 * DELETED is the number of lines deleted here from file 0.
 * INSERTED is the number of lines inserted here in file 1.
 *
 * If DELETED is 0 then LINE0 is the number of the line before
 * which the insertion was done; vice versa for INSERTED and LINE1.
 */
typedef struct change CHANGE;
struct change
{
	CHANGE *	link;			/* Previous or next edit command	*/
	int			inserted;		/* # lines of file 1 changed here	*/
	int			deleted;		/* # lines of file 0 changed here	*/
	int			line0;			/* Line number of 1st deleted line	*/
	int			line1;			/* Line number of 1st inserted line	*/
	char		ignore;			/* Flag used in context.c			*/
};

/*------------------------------------------------------------------------
 * Data on one input file being compared.
 */
typedef struct file_data FILE_DATA;
struct file_data
{
	FILE *          fp;		/* File stream  */
	const char *	name;	/* File name  */
	struct stat     stat;	/* File status from fstat()  */

	/*----------------------------------------------------------------
	 * Buffer in which text of file is read.
	 */
	char *			buffer;

	/*----------------------------------------------------------------
	 * Allocated size of buffer.
	 */
	int				bufsize;

	/*----------------------------------------------------------------
	 * Number of valid characters now in the buffer.
	 */
	int				buffered_chars;

	/*----------------------------------------------------------------
	 * Array of pointers to lines in the file.
	 */
	const char **	linbuf;

	/*----------------------------------------------------------------
	 * linbuf_base <= buffered_lines <= valid_lines <= alloc_lines.
	 *
	 * linebuf[linbuf_base ... buffered_lines - 1] are possibly differing.
	 * linebuf[linbuf_base ... valid_lines    - 1] contain valid data.
	 * linebuf[linbuf_base ... alloc_lines    - 1] are allocated.
	 */
	int				linbuf_base;
	int				buffered_lines;
	int				valid_lines;
	int				alloc_lines;

	/*----------------------------------------------------------------
	 * Pointer to end of prefix of this file to ignore when hashing.
	 */
	const char *	prefix_end;

	/*----------------------------------------------------------------
	 * Count of lines in the prefix.
	 * There are this many lines in the file before linbuf[0].
	 */
	int				prefix_lines;

	/*----------------------------------------------------------------
	 * Pointer to start of suffix of this file to ignore when hashing.
	 */
	const char *	suffix_begin;

	/*----------------------------------------------------------------
	 * Vector, indexed by line number, containing an equivalence code for
	 * each line.  It is this vector that is actually compared with that
	 * of another file to generate differences.
	 */
	int *			equivs;

	/*----------------------------------------------------------------
	 * Vector, like the previous one except that
	 * the elements for discarded lines have been squeezed out.
	 */
	int *			undiscarded;

	/*----------------------------------------------------------------
	 * Vector mapping virtual line numbers (not counting discarded lines)
	 * to real ones (counting those lines).  Both are origin-0.
	 */
	int *			realindexes;

	/*----------------------------------------------------------------
	 * Total number of nondiscarded lines.
	 */
	int				nondiscarded_lines;

	/*----------------------------------------------------------------
	 * Vector, indexed by real origin-0 line number,
	 * containing 1 for a line that is an insertion or a deletion.
	 * The results of comparison are stored here.
	 */
	char *			changed_flag;

	/*----------------------------------------------------------------
	 * 1 if file ends in a line with no final newline.
	 */
	int				missing_newline;

	/*----------------------------------------------------------------
	 * 1 more than the maximum equivalence value used for this or its
	 * sibling file.
	 */
	int				equiv_max;
};

#define TRANSLATE_NUM(f,n)	((f)->prefix_lines + (n) + 1)

/*------------------------------------------------------------------------
 * functions
 */

/*------------------------------------------------------------------------
 * diffeng.c
 */
extern DIFF_LINE *	diff_engine		(const char *name0, const char *name1,
										DIFF_OPTS *opts,
										int *rc, char *msgbuf);

/*------------------------------------------------------------------------
 * diffanal.c
 */
extern DIFF_LINE *	diff_2_files	(FILE_DATA *filevec, DIFF_OPTS *opts,
										int *rc, char *msgbuf);

/*------------------------------------------------------------------------
 * diffio.c
 */
extern int		read_files			(FILE_DATA *filevec, int pretend_binary,
										DIFF_OPTS *opts, char *msgbuf);
extern int		sip					(FILE_DATA *current, int skip_test,
										char *msgbuf);
extern int		slurp				(FILE_DATA *current,
										char *msgbuf);

/*------------------------------------------------------------------------
 * diffside.c
 */
extern DIFF_LINE *	print_sdiff_script	(FILE_DATA *files,
										CHANGE *script, DIFF_OPTS *opts);

/*------------------------------------------------------------------------
 * diffutil.c
 */
extern int		line_cmp			(const char *s1, const char *s2,
										DIFF_OPTS *opts);
extern int		translate_line_number (const FILE_DATA *, int);
extern CHANGE *	find_change			(CHANGE *start);
extern CHANGE *	find_reverse_change	(CHANGE *start);
extern void		analyze_hunk		(FILE_DATA *files, CHANGE *hunk,
										int *first0, int *last0,
										int *first1, int *last1,
										int *deletes, int *inserts,
										DIFF_OPTS *opts);
extern int		buffer_lcm			(int a, int b);

extern void *	diff_malloc			(int n);
extern void *	diff_realloc		(void *p, int n);
extern void		diff_free			(void *p);

#endif /* DIFFSYS_H */
