/*------------------------------------------------------------------------
 * main diff engine
 */
#include "diffsys.h"

/* Compare two files (or dirs) with specified names NAME0 and NAME1.
   This is self-contained; it opens the files and closes them. */

DIFF_LINE *
diff_engine (const char *name0, const char *name1,
	DIFF_OPTS *opts, int *rc, char *msgbuf)
{
	DIFF_LINE *	dl;
	FILE_DATA inf[2];
	int i;

	*msgbuf = 0;

	memset (inf, 0, sizeof (inf));

	/* Mark any nonexistent file with 0 in the fp field.  */

	inf[0].fp = 0;
	inf[1].fp = 0;

	/* Now record the full name of each file  */

	inf[0].name = name0;
	inf[1].name = name1;

	/* Stat the files. */

	for (i = 0; i <= 1; i++)
	{
		int stat_result;

		for (;;)
		{
			stat_result = stat(inf[i].name, &inf[i].stat);
			if (stat_result < 0 && errno == EINTR)
				continue;
			break;
		}

		if (stat_result != 0)
		{
			sprintf(msgbuf, "Cannot stat file %s: %s",
				inf[i].name, strerror(errno));
			*rc = DIFF_ERROR;
			return (0);
		}
		else
		{
			if (! S_ISREG(inf[i].stat.st_mode))
			{
				sprintf(msgbuf, "File %s is not a regular file",
					inf[i].name);
				*rc = DIFF_ERROR;
				return (0);
			}
		}
	}

	if (same_file (&inf[0].stat, &inf[1].stat))
	{
		/* The two named files are actually the same physical file.
		 We know they are identical without actually reading them.  */

		*rc = DIFF_SAME_FILE;
		return (0);
	}

	/* Open the files and record their descriptors.  */

	inf[0].fp = fopen(inf[0].name, "rb");
	if (inf[0].fp == 0)
	{
		sprintf(msgbuf, "Cannot open file %s: %s",
			inf[0].name, strerror(errno));
		*rc = DIFF_ERROR;
		return (0);
	}

	inf[1].fp = fopen(inf[1].name, "rb");
	if (inf[1].fp == 0)
	{
		sprintf(msgbuf, "Cannot open file %s: %s",
			inf[1].name, strerror(errno));
		fclose(inf[0].fp);
		*rc = DIFF_ERROR;
		return (0);
	}

	/* Compare the files, if no error was found.  */

	dl = diff_2_files(inf, opts, rc, msgbuf);
	if (dl != 0)
	{
		DIFF_LINE *d;

		for (d=dl; d->prev; d=d->prev)
			;
		dl = d;
	}

	/* Close the file descriptors.  */

	fclose(inf[0].fp);
	fclose(inf[1].fp);

	return (dl);
}

/*------------------------------------------------------------------------
 * free up the lines
 */
DIFF_LINE *
diff_free_lines (DIFF_LINE *dl)
{
	DIFF_LINE *	dp;
	DIFF_LINE *	dn;

	for (dp = dl; dl; dl = dn)
	{
		dn = dl->next;
		FREE(dl);
	}

	return (0);
}
