/* Analyze file differences for GNU DIFF.
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

/* The basic algorithm is described in:
   "An O(ND) Difference Algorithm and its Variations", Eugene Myers,
   Algorithmica Vol. 1 No. 2, 1986, pp. 251-266;
   see especially section 4.2, which describes the variation used below.
   Unless the --minimal option is specified, this code uses the TOO_EXPENSIVE
   heuristic, by Paul Eggert, to limit the cost to O(N**1.5 log N)
   at the price of producing suboptimal output for large inputs with
   many differences.

   The basic algorithm was independently discovered as described in:
   "Algorithms for Approximate String Matching", E. Ukkonen,
   Information and Control Vol. 64, 1985, pp. 100-118.  */

#include "diffsys.h"

typedef struct anal_info
{
	int *xvec, *yvec;	/* Vectors being compared. */

	int *fdiag;			/* Vector, indexed by diagonal, containing
				  			 1 + the X coordinate of the point furthest
				  			 along the given diagonal in the forward
				  			 search of the edit matrix. */

	int *bdiag;			/* Vector, indexed by diagonal, containing
				  			 the X coordinate of the point furthest
				  			 along the given diagonal in the backward
				  			 search of the edit matrix. */

	int too_expensive;	/* Edit scripts longer than this are too
				  			 expensive to compute.  */
} ANAL_INFO;

#define SNAKE_LIMIT 20	/* Snakes bigger than this are considered `big'.  */

struct partition
{
	int xmid, ymid;	/* Midpoints of this partition.  */
	int lo_minimal;	/* Nonzero if low half will be analyzed minimally.  */
	int hi_minimal;	/* Likewise for high half.  */
};

static int diag (int, int, int, int, int, struct partition *, DIFF_OPTS *,
	ANAL_INFO *);
static CHANGE *add_change (int, int, int, int, CHANGE *);
static CHANGE *build_script (const FILE_DATA[]);
static void compareseq (FILE_DATA *, int, int, int, int, int, DIFF_OPTS *,
	ANAL_INFO *);
static int discard_confusing_lines (FILE_DATA *, DIFF_OPTS *, char *);
static void shift_boundaries (FILE_DATA *);

/* Find the midpoint of the shortest edit script for a specified
   portion of the two files.

   Scan from the beginnings of the files, and simultaneously from the ends,
   doing a breadth-first search through the space of edit-sequence.
   When the two searches meet, we have found the midpoint of the shortest
   edit sequence.

   If MINIMAL is nonzero, find the minimal edit script regardless
   of expense.  Otherwise, if the search is too expensive, use
   heuristics to stop the search and report a suboptimal answer.

   Set PART->(XMID,YMID) to the midpoint (XMID,YMID).  The diagonal number
   XMID - YMID equals the number of inserted lines minus the number
   of deleted lines (counting only lines before the midpoint).
   Return the approximate edit cost; this is the total number of
   lines inserted or deleted (counting only lines before the midpoint),
   unless a heuristic is used to terminate the search prematurely.

   Set PART->LEFT_MINIMAL to nonzero iff the minimal edit script for the
   left half of the partition is known; similarly for PART->RIGHT_MINIMAL.

   This function assumes that the first lines of the specified portions
   of the two files do not match, and likewise that the last lines do not
   match.  The caller must trim matching lines from the beginning and end
   of the portions it is going to specify.

   If we return the "wrong" partitions,
   the worst this can do is cause suboptimal diff output.
   It cannot cause incorrect diff output.  */

static int
diag (int xoff, int xlim, int yoff, int ylim, int minimal,
	struct partition *part, DIFF_OPTS *opts, ANAL_INFO *a)
{
	int *const fd = a->fdiag;	/* Give the compiler a chance. */
	int *const bd = a->bdiag;	/* Additional help for the compiler. */
	int const *xv = a->xvec;	/* Still more help for the compiler. */
	int const *yv = a->yvec;	/* And more and more . . . */
	int const dmin = xoff - ylim;	/* Minimum valid diagonal. */
	int const dmax = xlim - yoff;	/* Maximum valid diagonal. */
	int const fmid = xoff - yoff;	/* Center diagonal of top-down search. */
	int const bmid = xlim - ylim;	/* Center diagonal of bottom-up search. */
	int fmin = fmid, fmax = fmid;	/* Limits of top-down search. */
	int bmin = bmid, bmax = bmid;	/* Limits of bottom-up search. */
	int c;			/* Cost. */
	int odd = (fmid - bmid) & 1;	/* True if southeast corner is on an odd
					   diagonal with respect to the northwest. */

	fd[fmid] = xoff;
	bd[bmid] = xlim;

	for (c = 1;; ++c)
	{
		int d;			/* Active diagonal. */
		int big_snake = 0;

		/* Extend the top-down search by an edit step in each diagonal. */
		fmin > dmin ? fd[--fmin - 1] = -1 : ++fmin;
		fmax < dmax ? fd[++fmax + 1] = -1 : --fmax;
		for (d = fmax; d >= fmin; d -= 2)
		{
			int x, y, oldx, tlo = fd[d - 1], thi = fd[d + 1];

			if (tlo >= thi)
				x = tlo + 1;
				else
				x = thi;
			oldx = x;
			y = x - d;
			while (x < xlim && y < ylim && xv[x] == yv[y])
				++x, ++y;
			if (x - oldx > SNAKE_LIMIT)
				big_snake = 1;
			fd[d] = x;
			if (odd && bmin <= d && d <= bmax && bd[d] <= x)
			{
				part->xmid = x;
				part->ymid = y;
				part->lo_minimal = part->hi_minimal = 1;
				return 2 * c - 1;
			}
		}

		/* Similarly extend the bottom-up search.  */
		bmin > dmin ? bd[--bmin - 1] = INT_MAX : ++bmin;
		bmax < dmax ? bd[++bmax + 1] = INT_MAX : --bmax;
		for (d = bmax; d >= bmin; d -= 2)
		{
			int x, y, oldx, tlo = bd[d - 1], thi = bd[d + 1];

			if (tlo < thi)
				x = tlo;
				else
				x = thi - 1;
			oldx = x;
			y = x - d;
			while (x > xoff && y > yoff && xv[x - 1] == yv[y - 1])
				--x, --y;
			if (oldx - x > SNAKE_LIMIT)
				big_snake = 1;
			bd[d] = x;
			if (!odd && fmin <= d && d <= fmax && x <= fd[d])
			{
				part->xmid = x;
				part->ymid = y;
				part->lo_minimal = part->hi_minimal = 1;
				return 2 * c;
			}
		}

		if (minimal)
			continue;

		/* Heuristic: check occasionally for a diagonal that has made
		 lots of progress compared with the edit distance.
		 If we have any such, find the one that has made the most
		 progress and return it as if it had succeeded.

		 With this heuristic, for files with a constant small density
		 of changes, the algorithm is linear in the file size.  */

		if (c > 200 && big_snake && opts->heuristic)
		{
			int best;

			best = 0;
			for (d = fmax; d >= fmin; d -= 2)
			{
				int dd = d - fmid;
				int x = fd[d];
				int y = x - d;
				int v = (x - xoff) * 2 - dd;
				if (v > 12 * (c + (dd < 0 ? -dd : dd)))
				{
					if (v > best
					    && xoff + SNAKE_LIMIT <= x && x < xlim
					    && yoff + SNAKE_LIMIT <= y && y < ylim)
					{
						/* We have a good enough best diagonal;
						 now insist that it end with a significant snake.  */
						int k;

						for (k = 1; xv[x - k] == yv[y - k]; k++)
							if (k == SNAKE_LIMIT)
							{
								best = v;
								part->xmid = x;
								part->ymid = y;
								break;
							}
					}
				}
			}
			if (best > 0)
			{
				part->lo_minimal = 1;
				part->hi_minimal = 0;
				return 2 * c - 1;
			}

			best = 0;
			for (d = bmax; d >= bmin; d -= 2)
			{
				int dd = d - bmid;
				int x = bd[d];
				int y = x - d;
				int v = (xlim - x) * 2 + dd;
				if (v > 12 * (c + (dd < 0 ? -dd : dd)))
				{
					if (v > best
					    && xoff < x && x <= xlim - SNAKE_LIMIT
					    && yoff < y && y <= ylim - SNAKE_LIMIT)
					{
						/* We have a good enough best diagonal;
						 now insist that it end with a significant snake.  */
						int k;

						for (k = 0; xv[x + k] == yv[y + k]; k++)
							if (k == SNAKE_LIMIT - 1)
							{
								best = v;
								part->xmid = x;
								part->ymid = y;
								break;
							}
					}
				}
			}
			if (best > 0)
			{
				part->lo_minimal = 0;
				part->hi_minimal = 1;
				return 2 * c - 1;
			}
		}

		/* Heuristic: if we've gone well beyond the call of duty,
		 give up and report halfway between our best results so far.  */
		if (c >= a->too_expensive)
		{
			int fxybest, fxbest;
			int bxybest, bxbest;

			fxbest = bxbest = 0;  /* Pacify `gcc -Wall'.  */

			/* Find forward diagonal that maximizes X + Y.  */
			fxybest = -1;
			for (d = fmax; d >= fmin; d -= 2)
			{
				int x = min (fd[d], xlim);
				int y = x - d;
				if (ylim < y)
					x = ylim + d, y = ylim;
				if (fxybest < x + y)
				{
					fxybest = x + y;
					fxbest = x;
				}
			}

			/* Find backward diagonal that minimizes X + Y.  */
			bxybest = INT_MAX;
			for (d = bmax; d >= bmin; d -= 2)
			{
				int x = max (xoff, bd[d]);
				int y = x - d;
				if (y < yoff)
					x = yoff + d, y = yoff;
				if (x + y < bxybest)
				{
					bxybest = x + y;
					bxbest = x;
				}
			}

			/* Use the better of the two diagonals.  */
			if ((xlim + ylim) - bxybest < fxybest - (xoff + yoff))
			{
				part->xmid = fxbest;
				part->ymid = fxybest - fxbest;
				part->lo_minimal = 1;
				part->hi_minimal = 0;
			}
			else
			{
				part->xmid = bxbest;
				part->ymid = bxybest - bxbest;
				part->lo_minimal = 0;
				part->hi_minimal = 1;
			}
			return 2 * c - 1;
		}
	}
}

/* Compare in detail contiguous subsequences of the two files
   which are known, as a whole, to match each other.

   The results are recorded in the vectors files[N].changed_flag, by
   storing a 1 in the element for each line that is an insertion or deletion.

   The subsequence of file 0 is [XOFF, XLIM) and likewise for file 1.

   Note that XLIM, YLIM are exclusive bounds.
   All line numbers are origin-0 and discarded lines are not counted.

   If MINIMAL is nonzero, find a minimal difference no matter how
   expensive it is.  */

static void
compareseq (FILE_DATA *files, int xoff, int xlim, int yoff, int ylim,
	int minimal, DIFF_OPTS *opts, ANAL_INFO *a)
{
	const int * xv = a->xvec; /* Help the compiler.  */
	const int * yv = a->yvec;

	/* Slide down the bottom initial diagonal. */
	while (xoff < xlim && yoff < ylim && xv[xoff] == yv[yoff])
		++xoff, ++yoff;
	/* Slide up the top initial diagonal. */
	while (xlim > xoff && ylim > yoff && xv[xlim - 1] == yv[ylim - 1])
		--xlim, --ylim;

	/* Handle simple cases. */
	if (xoff == xlim)
		while (yoff < ylim)
			files[1].changed_flag[files[1].realindexes[yoff++]] = 1;
	else if (yoff == ylim)
		while (xoff < xlim)
			files[0].changed_flag[files[0].realindexes[xoff++]] = 1;
			else
	{
		int c;
		struct partition part;

		/* Find a point of correspondence in the middle of the files.  */

		c = diag(xoff, xlim, yoff, ylim, minimal, &part, opts, a);

		if (c == 1)
		{
#if 0
			/* This should be impossible, because it implies that
				     one of the two subsequences is empty,
				     and that case was handled above without calling `diag'.
				     Let's verify that this is true.  */
			abort ();
#else
			/* The two subsequences differ by a single insert or delete;
				     record it and we are done.  */
			if (part.xmid - part.ymid < xoff - yoff)
				files[1].changed_flag[files[1].realindexes[part.ymid - 1]] = 1;
				else
				files[0].changed_flag[files[0].realindexes[part.xmid]] = 1;
#endif
		}
		else
		{
			/* Use the partitions to split this problem into subproblems.  */
			compareseq(files, xoff, part.xmid, yoff, part.ymid,
				part.lo_minimal, opts, a);
			compareseq(files, part.xmid, xlim, part.ymid, ylim,
				part.hi_minimal, opts, a);
		}
	}
}

/* Discard lines from one file that have no matches in the other file.

   A line which is discarded will not be considered by the actual
   comparison algorithm; it will be as if that line were not in the file.
   The file's `realindexes' table maps virtual line numbers
   (which don't count the discarded lines) into real line numbers;
   this is how the actual comparison algorithm produces results
   that are comprehensible when the discarded lines are counted.

   When we discard a line, we also mark it as a deletion or insertion
   so that it will be printed in the output.  */

static int
discard_confusing_lines (FILE_DATA *filevec, DIFF_OPTS *opts, char *msgbuf)
{
	int f, i;
	char *discarded[2];
	int *equiv_count[2];
	int *p;

	/* Allocate our results.  */
	p = (int *) diff_malloc
		((filevec[0].buffered_lines + filevec[1].buffered_lines)
	    * (2 * sizeof (int)));
	if (p == 0)
	{
		sprintf(msgbuf, "Cannot allocate lines");
		return (-1);
	}

	for (f = 0; f < 2; f++)
	{
		filevec[f].undiscarded = p;
		p += filevec[f].buffered_lines;
		filevec[f].realindexes = p;
		p += filevec[f].buffered_lines;
	}

	/* Set up equiv_count[F][I] as the number of lines in file F
     that fall in equivalence class I.  */

	p = (int *) diff_malloc (filevec[0].equiv_max * (2 * sizeof (int)));
	if (p == 0)
	{
		sprintf(msgbuf, "Cannot allocate equivs");
		return (-1);
	}

	equiv_count[0] = p;
	equiv_count[1] = p + filevec[0].equiv_max;
	memset (p, 0, filevec[0].equiv_max * (2 * sizeof (int)));

	for (i = 0; i < filevec[0].buffered_lines; ++i)
		++equiv_count[0][filevec[0].equivs[i]];
	for (i = 0; i < filevec[1].buffered_lines; ++i)
		++equiv_count[1][filevec[1].equivs[i]];

	/* Set up tables of which lines are going to be discarded.  */

	discarded[0] = (char *)diff_malloc (sizeof (char)
	    * (filevec[0].buffered_lines
	    + filevec[1].buffered_lines));
	if (discarded[0] == 0)
	{
		diff_free((void *)equiv_count[0]);

		sprintf(msgbuf, "Cannot allocate equivs");
		return (-1);
	}

	discarded[1] = discarded[0] + filevec[0].buffered_lines;
	memset (discarded[0], 0, sizeof (char) * (filevec[0].buffered_lines
	    + filevec[1].buffered_lines));

	/* Mark to be discarded each line that matches no line of the other file.
     If a line matches many lines, mark it as provisionally discardable.  */

	for (f = 0; f < 2; f++)
	{
		int end = filevec[f].buffered_lines;
		char *discards = discarded[f];
		int *counts = equiv_count[1 - f];
		int *equivs = filevec[f].equivs;
		int many = 5;
		int tem = end / 64;

		/* Multiply MANY by approximate square root of number of lines.
		 That is the threshold for provisionally discardable lines.  */
		while ((tem = tem >> 2) > 0)
			many *= 2;

		for (i = 0; i < end; i++)
		{
			int nmatch;
			if (equivs[i] == 0)
				continue;
			nmatch = counts[equivs[i]];
			if (nmatch == 0)
				discards[i] = 1;
			else if (nmatch > many)
				discards[i] = 2;
		}
	}

	/* Don't really discard the provisional lines except when they occur
     in a run of discardables, with nonprovisionals at the beginning
     and end.  */

	for (f = 0; f < 2; f++)
	{
		int end = filevec[f].buffered_lines;
		char *discards = discarded[f];

		for (i = 0; i < end; i++)
		{
			/* Cancel provisional discards not in middle of run of discards. */
			if (discards[i] == 2)
				discards[i] = 0;
			else if (discards[i] != 0)
			{
				/* We have found a nonprovisional discard.  */
				int j;
				int length;
				int provisional = 0;

				/* Find end of this run of discardable lines.
				 Count how many are provisionally discardable.  */
				for (j = i; j < end; j++)
				{
					if (discards[j] == 0)
						break;
					if (discards[j] == 2)
						++provisional;
				}

				/* Cancel provisional discards at end, and shrink the run.  */
				while (j > i && discards[j - 1] == 2)
					discards[--j] = 0, --provisional;

				/* Now we have the length of a run of discardable lines
				 whose first and last are not provisional.  */
				length = j - i;

				/* If 1/4 of the lines in the run are provisional,
				 cancel discarding of all provisional lines in the run.  */
				if (provisional * 4 > length)
				{
					while (j > i)
						if (discards[--j] == 2)
							discards[j] = 0;
				}
				else
				{
					int consec;
					int minimum = 1;
					int tem = length / 4;

					/* MINIMUM is approximate square root of LENGTH/4.
				     A subrun of two or more provisionals can stand
				     when LENGTH is at least 16.
				     A subrun of 4 or more can stand when LENGTH >= 64.  */
					while ((tem = tem >> 2) > 0)
						minimum *= 2;
					minimum++;

					/* Cancel any subrun of MINIMUM or more provisionals
							     within the larger run.  */
					for (j = 0, consec = 0; j < length; j++)
						if (discards[i + j] != 2)
							consec = 0;
						else if (minimum == ++consec)
							/* Back up to start of subrun, to cancel it all. */
							j -= consec;
						else if (minimum < consec)
							discards[i + j] = 0;

					/* Scan from beginning of run
				     until we find 3 or more nonprovisionals in a row
				     or until the first nonprovisional at least 8 lines in.
				     Until that point, cancel any provisionals.  */
					for (j = 0, consec = 0; j < length; j++)
					{
						if (j >= 8 && discards[i + j] == 1)
							break;
						if (discards[i + j] == 2)
							consec = 0, discards[i + j] = 0;
						else if (discards[i + j] == 0)
							consec = 0;
							else
							consec++;
						if (consec == 3)
							break;
					}

					/* I advances to the last line of the run.  */
					i += length - 1;

					/* Same thing, from end.  */
					for (j = 0, consec = 0; j < length; j++)
					{
						if (j >= 8 && discards[i - j] == 1)
							break;
						if (discards[i - j] == 2)
							consec = 0, discards[i - j] = 0;
						else if (discards[i - j] == 0)
							consec = 0;
							else
							consec++;
						if (consec == 3)
							break;
					}
				}
			}
		}
	}

	/* Actually discard the lines. */
	for (f = 0; f < 2; f++)
	{
		char *discards = discarded[f];
		int end = filevec[f].buffered_lines;
		int j = 0;
		for (i = 0; i < end; ++i)
			if (opts->no_discards || discards[i] == 0)
			{
				filevec[f].undiscarded[j] = filevec[f].equivs[i];
				filevec[f].realindexes[j++] = i;
			}
		else
				filevec[f].changed_flag[i] = 1;
		filevec[f].nondiscarded_lines = j;
	}

	diff_free((void *)discarded[0]);
	diff_free((void *)equiv_count[0]);

	return (0);
}

/* Adjust inserts/deletes of identical lines to join changes
   as much as possible.

   We do something when a run of changed lines include a
   line at one end and have an excluded, identical line at the other.
   We are free to choose which identical line is included.
   `compareseq' usually chooses the one at the beginning,
   but usually it is cleaner to consider the following identical line
   to be the "change".  */

static void
shift_boundaries (FILE_DATA *filevec)
{
	int f;

	for (f = 0; f < 2; f++)
	{
		char *changed = filevec[f].changed_flag;
		const char *other_changed = filevec[1-f].changed_flag;
		int const *equivs = filevec[f].equivs;
		int i = 0;
		int j = 0;
		int i_end = filevec[f].buffered_lines;

		while (1)
		{
			int runlength, start, corresponding;

			/* Scan forwards to find beginning of another run of changes.
		     Also keep track of the corresponding point in the other file.  */

			while (i < i_end && changed[i] == 0)
			{
				while (other_changed[j++])
					continue;
				i++;
			}

			if (i == i_end)
				break;

			start = i;

			/* Find the end of this run of changes.  */

			while (changed[++i])
				continue;
			while (other_changed[j])
				j++;

			do
			{
				/* Record the length of this run of changes, so that
				 we can later determine whether the run has grown.  */
				runlength = i - start;

				/* Move the changed region back, so long as the
				 previous unchanged line matches the last changed one.
				 This merges with previous changed regions.  */

				while (start && equivs[start - 1] == equivs[i - 1])
				{
					changed[--start] = 1;
					changed[--i] = 0;
					while (changed[start - 1])
						start--;
					while (other_changed[--j])
						continue;
				}

				/* Set CORRESPONDING to the end of the changed run, at the last
				 point where it corresponds to a changed run in the other file.
				 CORRESPONDING == I_END means no such point has been found.  */
				corresponding = other_changed[j - 1] ? i : i_end;

				/* Move the changed region forward, so long as the
				 first changed line matches the following unchanged one.
				 This merges with following changed regions.
				 Do this second, so that if there are no merges,
				 the changed region is moved forward as far as possible.  */

				while (i != i_end && equivs[start] == equivs[i])
				{
					changed[start++] = 0;
					changed[i++] = 1;
					while (changed[i])
						i++;
					while (other_changed[++j])
						corresponding = i;
				}
			}	  while (runlength != i - start);

			/* If possible, move the fully-merged run of changes
		     back to a corresponding run in the other file.  */

			while (corresponding < i)
			{
				changed[--start] = 1;
				changed[--i] = 0;
				while (other_changed[--j])
					continue;
			}
		}
	}
}

/* Cons an additional entry onto the front of an edit script OLD.
   LINE0 and LINE1 are the first affected lines in the two files (origin 0).
   DELETED is the number of lines deleted here from file 0.
   INSERTED is the number of lines inserted here in file 1.

   If DELETED is 0 then LINE0 is the number of the line before
   which the insertion was done; vice versa for INSERTED and LINE1.  */

static CHANGE *
add_change (int line0, int line1, int deleted, int inserted, CHANGE *old)
{
	CHANGE *neu = (CHANGE *) diff_malloc (sizeof (CHANGE));

	if (neu == 0)
		return (old);

	neu->line0 = line0;
	neu->line1 = line1;
	neu->inserted = inserted;
	neu->deleted = deleted;
	neu->link = old;
	return neu;
}

/* Scan the tables of which lines are inserted and deleted,
   producing an edit script in forward order.  */

static CHANGE *
build_script (const FILE_DATA *filevec)
{
	CHANGE *script = 0;
	char *changed0 = filevec[0].changed_flag;
	char *changed1 = filevec[1].changed_flag;
	int i0 = filevec[0].buffered_lines, i1 = filevec[1].buffered_lines;

	/* Note that changedN[-1] does exist, and contains 0.  */

	while (i0 >= 0 || i1 >= 0)
	{
		if (changed0[i0 - 1] || changed1[i1 - 1])
		{
			int line0 = i0, line1 = i1;

			/* Find # lines changed here in each file.  */
			while (changed0[i0 - 1]) --i0;
			while (changed1[i1 - 1]) --i1;

			/* Record this change.  */
			script = add_change (i0, i1, line0 - i0, line1 - i1, script);
		}

		/* We have reached lines in the two files that match each other.  */
		i0--, i1--;
	}

	return script;
}

/* Report the differences of two files. */
DIFF_LINE *
diff_2_files (FILE_DATA *filevec, DIFF_OPTS *opts,
	int *prc, char *msgbuf)
{
	DIFF_LINE *	dl = 0;
	ANAL_INFO ai;
	ANAL_INFO *a = &ai;
	int diags;
	int i;
	CHANGE *e, *p;
	CHANGE *script;
	int changes;
	int rc;

	/* If we have detected that either file is binary,
     compare the two files as binary.  This can happen
     only when the first chunk is read.
     Also, --brief without any --ignore-* options means
     we can speed things up by treating the files as binary.  */

	rc = read_files (filevec, 0, opts, msgbuf);
	if (rc < 0)
	{
		*prc = DIFF_ERROR;
		return (0);
	}
	else if (rc > 0)
	{
		/* binary file */

		/* Files with different lengths must be different.  */
		if (filevec[0].stat.st_size != filevec[1].stat.st_size)
		{
			changes = DIFF_BIN_DIFF;
		}
		else
		/* Scan both files, a buffer at a time, looking for a difference.  */
		{
			/* Allocate same-sized buffers for both files.  */
			int buffer_size = buffer_lcm (STAT_BLOCKSIZE (filevec[0].stat),
			    STAT_BLOCKSIZE (filevec[1].stat));

			for (i = 0; i < 2; i++)
			{
				filevec[i].buffer = (char *)diff_malloc (buffer_size);
				if (filevec[i].buffer == 0)
				{
					sprintf(msgbuf, "Cannot alloc buffer");
					*prc = DIFF_ERROR;
					return (0);
				}
			}

			for (;;  filevec[0].buffered_chars = filevec[1].buffered_chars = 0)
			{
				/* Read a buffer's worth from both files.  */
				for (i = 0; i < 2; i++)
				{
					if (filevec[i].fp != 0)
					{
						while (filevec[i].buffered_chars != buffer_size)
						{
							int r = fread (
							    filevec[i].buffer
							    + filevec[i].buffered_chars, 1,
							    buffer_size - filevec[i].buffered_chars,
								filevec[i].fp);
							if (r == 0)
								break;
							if (r < 0)
							{
								sprintf(msgbuf, "IO error reading %s: %s",
									filevec[i].name, strerror(errno));
								*prc = DIFF_ERROR;
								return (0);
							}

							filevec[i].buffered_chars += r;
						}
					}
				}

				/* If the buffers differ, the files differ.  */
				if (filevec[0].buffered_chars != filevec[1].buffered_chars
				    || (filevec[0].buffered_chars != 0
				    && memcmp (filevec[0].buffer,
				    filevec[1].buffer,
				    filevec[0].buffered_chars) != 0))
				{
					changes = DIFF_BIN_DIFF;
					break;
				}

				/* If we reach end of file, the files are the same.  */
				if (filevec[0].buffered_chars != buffer_size)
				{
					changes = DIFF_BIN_SAME;
					break;
				}
			}
		}
	}
	else
	{
		/* Allocate vectors for the results of comparison:
		 a flag for each line of each file, saying whether that line
		 is an insertion or deletion.
		 Allocate an extra element, always zero, at each end of each vector. */

		int s = filevec[0].buffered_lines + filevec[1].buffered_lines + 4;
		filevec[0].changed_flag = (char *)diff_malloc (s);
		if (filevec[0].changed_flag == 0)
		{
			sprintf(msgbuf, "Cannot allocate changed_flag");
			*prc = DIFF_ERROR;
			return (0);
		}

		memset (filevec[0].changed_flag, 0, s);
		filevec[0].changed_flag++;
		filevec[1].changed_flag = filevec[0].changed_flag
		    + filevec[0].buffered_lines + 2;

		/* Some lines are obviously insertions or deletions
		 because they don't match anything.  Detect them now, and
		 avoid even thinking about them in the main comparison algorithm.  */

		if (discard_confusing_lines (filevec, opts, msgbuf) < 0)
		{
			*prc = DIFF_ERROR;
			return (0);
		}

		/* Now do the main comparison algorithm, considering just the
		 undiscarded lines.  */

		a->xvec = filevec[0].undiscarded;
		a->yvec = filevec[1].undiscarded;
		diags = filevec[0].nondiscarded_lines +
			filevec[1].nondiscarded_lines + 3;
		a->fdiag = (int *) diff_malloc (diags * (2 * sizeof (int)));
		if (a->fdiag == 0)
		{
			sprintf(msgbuf, "Cannot allocate fdiag");
			*prc = DIFF_ERROR;
			return (0);
		}

		a->bdiag = a->fdiag + diags;
		a->fdiag += filevec[1].nondiscarded_lines + 1;
		a->bdiag += filevec[1].nondiscarded_lines + 1;

		/* Set TOO_EXPENSIVE to be approximate square root of input size,
			 bounded below by 256.  */
		a->too_expensive = 1;
		for (i = filevec[0].nondiscarded_lines + filevec[1].nondiscarded_lines;
		    i != 0; i >>= 2)
			a->too_expensive <<= 1;
		a->too_expensive = max (256, a->too_expensive);

		compareseq (filevec, 0, filevec[0].nondiscarded_lines,
		    0, filevec[1].nondiscarded_lines, opts->no_discards, opts, a);

		diff_free((void *)(a->fdiag - (filevec[1].nondiscarded_lines + 1)));

		/* Modify the results slightly to make them prettier
			 in cases where that can validly be done.  */

		shift_boundaries (filevec);

		/* Get the results of comparison in the form of a chain
			 of `CHANGE's -- an edit script.  */

		script = build_script (filevec);

		/* Set CHANGES if we had any diffs.
		 If some changes are ignored, we must scan the script to decide.  */
		if (opts->ignore_blank_lines_flag)
		{
			CHANGE *next = script;
			changes = 0;

			while (next && changes == 0)
			{
				CHANGE *that, *end;
				int first0, last0, first1, last1, deletes, inserts;

				/* Find a set of changes that belong together.  */
				that = next;
				end = find_change (next);

				/* Disconnect them from the rest of the changes, making them
						 a hunk, and remember the rest for next iteration.  */
				next = end->link;
				end->link = 0;

				/* Determine whether this hunk is really a difference.  */
				analyze_hunk (filevec, that, &first0, &last0, &first1, &last1,
				    &deletes, &inserts, opts);

				/* Reconnect the script so it will all be freed properly.  */
				end->link = next;

				if (deletes || inserts)
					changes = DIFF_TXT_DIFF;
			}
		}
		else
		{
			changes = (script != 0 ? DIFF_TXT_DIFF : DIFF_TXT_SAME);
		}

		if (changes == DIFF_TXT_DIFF)
		{
			dl = print_sdiff_script(filevec, script, opts);
		}

		/* now free everything */

		diff_free((void *)filevec[0].undiscarded);
		diff_free((void *)(filevec[0].changed_flag - 1));

		for (i = 1; i >= 0; --i)
			diff_free((void *)filevec[i].equivs);

		for (i = 0; i < 2; ++i)
			diff_free((void *)(filevec[i].linbuf + filevec[i].linbuf_base));

		for (e = script; e; e = p)
		{
			p = e->link;
			diff_free((void *)e);
		}
	}

	if (filevec[0].buffer != filevec[1].buffer)
		diff_free((void *)filevec[0].buffer);
	diff_free((void *)filevec[1].buffer);

	*prc = changes;
	return (dl);
}
