/* Support routines for GNU DIFF.
   Copyright (C) 1988, 1989, 1992, 1993, 1994 Free Software Foundation, Inc.

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


#include "diffsys.h"

static DIFF_LINE * print_1sdiff_line (DIFF_LINE *, LINE_CLASS, const char **,
	int, const char **, int);
static DIFF_LINE * print_sdiff_common_lines (FILE_DATA *, DIFF_LINE *, int,
	int, int *, int *);
static DIFF_LINE * print_sdiff_hunk (FILE_DATA *, DIFF_LINE *, CHANGE *,
	DIFF_OPTS *, int *, int *);

/* Divide SCRIPT into pieces by calling HUNKFUN and
   print each piece with PRINTFUN.
   Both functions take one arg, an edit script.

   HUNKFUN is called with the tail of the script
   and returns the last link that belongs together with the start
   of the tail.

   PRINTFUN takes a subscript which belongs together (with a null
   link at the end) and prints it.  */

static DIFF_LINE *
print_script (FILE_DATA *files, DIFF_LINE *dl, CHANGE *script, DIFF_OPTS *opts,
	CHANGE * (*hunkfun)(CHANGE *),
	DIFF_LINE * (*printfun)
		(FILE_DATA *, DIFF_LINE *, CHANGE *, DIFF_OPTS *, int *, int *),
	int *pnext0, int *pnext1)
{
	CHANGE *next = script;

	while (next)
	{
		CHANGE *that, *end;

		/* Find a set of changes that belong together.  */
		that = next;
		end = (*hunkfun) (next);

		/* Disconnect them from the rest of the changes,
			 making them a hunk, and remember the rest for next iteration.  */
		next = end->link;
		end->link = 0;

		/* Print this hunk.  */
		dl = (*printfun) (files, dl, that, opts, pnext0, pnext1);

		/* Reconnect the script so it will all be freed properly.  */
		end->link = next;
	}

	return (dl);
}

/*
 * Print 1 line of output file.
 * 0 parameters are taken to indicate white space text.
 * Blank lines that can easily be caught are reduced to a single newline.
 */

static DIFF_LINE *
print_1sdiff_line (DIFF_LINE *dl, LINE_CLASS type, const char **left,
	int num_lf, const char **right, int num_rt)
{
	DIFF_LINE	line;
	DIFF_LINE *	d;
	char *		dc;

	/*--------------------------------------------------------------------
	 * fill in line header
	 */
	line.prev		= 0;
	line.next		= 0;
	line.type		= type;
	line.num_lf		= num_lf;
	line.num_rt		= num_rt;
	line.len_lf		= left  ? line.len_lf = left [1] - left [0] : 0;
	line.len_rt		= right ? line.len_rt = right[1] - right[0] : 0;
	line.size		= sizeof(line) + 2 + line.len_lf + line.len_rt;

	/*--------------------------------------------------------------------
	 * now allocate a copy
	 */
	d = (DIFF_LINE *)MALLOC(line.size);
	if (d == 0)
		return (dl);

	dc = (char *)d;

	/*--------------------------------------------------------------------
	 * now fill it in
	 */
	memcpy(dc, &line, sizeof(*d));
	dc += sizeof(*d);

	if (left)
	{
		memcpy(dc, left [0], line.len_lf);
		dc += line.len_lf;
	}
	*dc++ = 0;

	if (right)
	{
		memcpy(dc, right[0], line.len_rt);
		dc += line.len_rt;
	}
	*dc++ = 0;

	/*--------------------------------------------------------------------
	 * now hook it in to the list
	 *
	 * Note: for efficiency, we build the list backward & we will
	 * reverse it at the end (actually just return the tail).
	 */
	if (dl != 0)
		dl->next = d;
	d->prev = dl;

	return (d);
}

/* Print lines common to both files in side-by-side format.  */
static DIFF_LINE *
print_sdiff_common_lines (FILE_DATA *files, DIFF_LINE *dl,
	int limit0, int limit1, int *pnext0, int *pnext1)
{
	int i0 = *pnext0, i1 = *pnext1;
	int ln, rn;

	if (i0 != limit0 || i1 != limit1)
	{
		while (i0 != limit0 && i1 != limit1)
		{
			ln = TRANSLATE_NUM(&files[0], i0);
			rn = TRANSLATE_NUM(&files[1], i1);

			dl = print_1sdiff_line(dl, COMMON, &files[0].linbuf[i0++], ln,
				&files[1].linbuf[i1++], rn);
		}

		while (i1 != limit1)
		{
			rn = TRANSLATE_NUM(&files[1], i1);
			dl = print_1sdiff_line(dl, RIGHT, 0, 0, &files[1].linbuf[i1++], rn);
		}

		while (i0 != limit0)
		{
			ln = TRANSLATE_NUM(&files[0], i0);
			dl = print_1sdiff_line(dl, LEFT, &files[0].linbuf[i0++], ln, 0, 0);
		}
	}

	*pnext0 = limit0;
	*pnext1 = limit1;

	return (dl);
}

/* Print a hunk of an sdiff diff.
   This is a contiguous portion of a complete edit script,
   describing changes in consecutive lines.  */

static DIFF_LINE *
print_sdiff_hunk (FILE_DATA *files, DIFF_LINE *dl, CHANGE *hunk,
	DIFF_OPTS *opts, int *pnext0, int *pnext1)
{
	int first0, last0, first1, last1, deletes, inserts;
	int i, j;
	int ln, rn;

	/* Determine range of line numbers involved in each file.  */
	analyze_hunk(files, hunk, &first0, &last0, &first1, &last1,
		&deletes, &inserts, opts);
	if (!deletes && !inserts)
		return (dl);

	/* Print out lines up to this change.  */
	dl = print_sdiff_common_lines(files, dl, first0, first1, pnext0, pnext1);

	/* Print ``xxx  |  xxx '' lines */
	if (inserts && deletes)
	{
		for (i = first0, j = first1;  i <= last0 && j <= last1; ++i, ++j)
		{
			ln = TRANSLATE_NUM(&files[0], i);
			rn = TRANSLATE_NUM(&files[1], j);
			dl = print_1sdiff_line(dl, CHANGED, &files[0].linbuf[i], ln,
				&files[1].linbuf[j], rn);
		}

		deletes = i <= last0;
		inserts = j <= last1;
		*pnext0 = first0 = i;
		*pnext1 = first1 = j;
	}

	/* Print ``     >  xxx '' lines */
	if (inserts)
	{
		for (j = first1; j <= last1; ++j)
		{
			rn = TRANSLATE_NUM(&files[1], j);
			dl = print_1sdiff_line(dl, RIGHT, 0, 0, &files[1].linbuf[j], rn);
		}

		*pnext1 = j;
	}

	/* Print ``xxx  <     '' lines */
	if (deletes)
	{
		for (i = first0; i <= last0; ++i)
		{
			ln = TRANSLATE_NUM(&files[0], i);
			dl = print_1sdiff_line(dl, LEFT, &files[0].linbuf[i], ln, 0, 0);
		}

		*pnext0 = i;
	}

	return (dl);
}

/* Print the edit-script SCRIPT as a sdiff style output.  */

DIFF_LINE *
print_sdiff_script (FILE_DATA *files, CHANGE *script, DIFF_OPTS *opts)
{
	DIFF_LINE *	dl = 0;
	int next0;
	int next1;

	next0 = next1 = - files[0].prefix_lines;

	dl = print_script(files, dl, script, opts, find_change, print_sdiff_hunk,
		&next0, &next1);

	dl = print_sdiff_common_lines(files, dl,
		files[0].valid_lines, files[1].valid_lines, &next0, &next1);

	return (dl);
}
