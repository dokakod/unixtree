/* 
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

/* Compare two lines (typically one from each input file)
   according to the command line options.
   For efficiency, this is invoked only when the lines do not match exactly
   but an option like -i might cause us to ignore the difference.
   Return nonzero if the lines differ.  */

int
line_cmp (const char *s1, const char *s2, DIFF_OPTS *opts)
{
	unsigned const char *t1 = (unsigned const char *) s1;
	unsigned const char *t2 = (unsigned const char *) s2;

	while (1)
	{
		unsigned char c1 = *t1++;
		unsigned char c2 = *t2++;

		/* Test for exact char equality first, since it's a common case.  */
		if (c1 != c2)
		{
			/* Ignore horizontal white space if -b or -w is specified.  */

			if (opts->ignore_all_space_flag)
			{
				/* For -w, just skip past any white space.  */
				while (isspace (c1) && c1 != '\n') c1 = *t1++;
				while (isspace (c2) && c2 != '\n') c2 = *t2++;
			}
			else if (opts->ignore_space_change_flag)
			{
				/* For -b, advance past any sequence of white space in line 1
						 and consider it just one Space, or nothing at all
						 if it is at the end of the line.  */
				if (isspace (c1))
				{
					while (c1 != '\n')
					{
						c1 = *t1++;
						if (! isspace (c1))
						{
							--t1;
							c1 = ' ';
							break;
						}
					}
				}

				/* Likewise for line 2.  */
				if (isspace (c2))
				{
					while (c2 != '\n')
					{
						c2 = *t2++;
						if (! isspace (c2))
						{
							--t2;
							c2 = ' ';
							break;
						}
					}
				}

				if (c1 != c2)
				{
					/* If we went too far when doing the simple test
					     for equality, go back to the first non-white-space
					     character in both sides and try again.  */
					if (c2 == ' ' && c1 != '\n'
					    && (unsigned const char *) s1 + 1 < t1
					    && isspace(t1[-2]))
					{
						--t1;
						continue;
					}
					if (c1 == ' ' && c2 != '\n'
					    && (unsigned const char *) s2 + 1 < t2
					    && isspace(t2[-2]))
					{
						--t2;
						continue;
					}
				}
			}

			/* Lowercase all letters if -i is specified.  */

			if (opts->ignore_case_flag)
			{
				if (isupper (c1))
					c1 = tolower (c1);
				if (isupper (c2))
					c2 = tolower (c2);
			}

			if (c1 != c2)
				break;
		}
		if (c1 == '\n')
			return 0;
	}

	return (1);
}

/* Find the consecutive changes at the start of the script START.
   Return the last link before the first gap.  */

CHANGE *
find_change (CHANGE *start)
{
	return start;
}

CHANGE *
find_reverse_change (CHANGE *start)
{
	return start;
}

/* Look at a hunk of edit script and report the range of lines in each file
   that it applies to.  HUNK is the start of the hunk, which is a chain
   of `CHANGE'.  The first and last line numbers of file 0 are stored in
   *FIRST0 and *LAST0, and likewise for file 1 in *FIRST1 and *LAST1.
   Note that these are internal line numbers that count from 0.

   If no lines from file 0 are deleted, then FIRST0 is LAST0+1.

   Also set *DELETES nonzero if any lines of file 0 are deleted
   and set *INSERTS nonzero if any lines of file 1 are inserted.
   If only ignorable lines are inserted or deleted, both are
   set to 0.  */

void
analyze_hunk (FILE_DATA *files, CHANGE *hunk,
	int *first0, int *last0, int *first1, int *last1,
	int *deletes, int *inserts, DIFF_OPTS *opts)
{
	int l0, l1, show_from, show_to;
	int ibl = opts->ignore_blank_lines_flag;
	int trivial = ibl;
	int i;
	CHANGE *next;

	show_from = show_to = 0;

	*first0 = hunk->line0;
	*first1 = hunk->line1;

	next = hunk;
	do
	{
		l0 = next->line0 + next->deleted - 1;
		l1 = next->line1 + next->inserted - 1;
		show_from += next->deleted;
		show_to += next->inserted;

		for (i = next->line0; i <= l0 && trivial; i++)
		{
			if (! ibl || files[0].linbuf[i][0] != '\n')
			{
	      		trivial = 0;
			}
		}

		for (i = next->line1; i <= l1 && trivial; i++)
		{
			if (! ibl || files[1].linbuf[i][0] != '\n')
			{
				trivial = 0;
			}
		}
	}  while ((next = next->link) != 0);

	*last0 = l0;
	*last1 = l1;

	/* If all inserted or deleted lines are ignorable,
	     tell the caller to ignore this hunk.  */

	if (trivial)
		show_from = show_to = 0;

	*deletes = show_from;
	*inserts = show_to;
}

/* Least common multiple of two buffer sizes A and B.  */

int
buffer_lcm (int a, int b)
{
	int m, n, r;

	/* Yield reasonable values if buffer sizes are zero.  */
	if (a == 0)
		return b ? b : 8 * 1024;

	if (b == 0)
		return a;

	/* n = gcd (a, b) */
	for (m = a, n = b;  (r = m % n) != 0;  m = n, n = r)
		continue;

	return a/n * b;
}

/*------------------------------------------------------------------------
 * malloc/realloc/free interface
 */
void *
diff_malloc (int n)
{
	void *p;

	if (n == 0)
		n = 1;

	p = MALLOC(n);

	return p;
}

void
diff_free (void *p)
{
	if (p != 0)
		FREE(p);
}

void *
diff_realloc (void *p, int n)
{
	if (p == 0)
		return diff_malloc(n);

	if (n == 0)
		n = 1;

	p = REALLOC(p, n);

	return p;
}
