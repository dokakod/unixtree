/*------------------------------------------------------------------------
 * file compare functions
 */
#include "oscommon.h"

/*------------------------------------------------------------------------
 * compare 2 files
 *
 * returns:	TRUE if same, FALSE if different
 */
int os_file_compare (const char *path1, const char *path2)
{
	struct stat	stbuf1;
	struct stat	stbuf2;
	FILE *		fp1;
	FILE *		fp2;
	int			rc;

	/*--------------------------------------------------------------------
	 * if paths are the same, the files are the same
	 */
	if (strcmp(path1, path2) == 0)
	{
		return (TRUE);
	}

	/*--------------------------------------------------------------------
	 * get stat structs - if either doesn't exist, they are different
	 */
	rc = os_stat(path1, &stbuf1);
	if (rc)
	{
		return (FALSE);
	}

	rc = os_stat(path2, &stbuf2);
	if (rc)
	{
		return (FALSE);
	}

	/*--------------------------------------------------------------------
	 * if links to each other, they are the same
	 */
	if (stbuf1.st_ino == stbuf2.st_ino &&
	    stbuf1.st_dev == stbuf2.st_dev)
	{
		return (TRUE);
	}

	/*--------------------------------------------------------------------
	 * if sizes are different, they are too
	 */
	if (stbuf1.st_size != stbuf2.st_size)
	{
		return (FALSE);
	}

	/*--------------------------------------------------------------------
	 * I guess we gotta read the file now.
	 */
	fp1 = fopen(path1, "rb");
	if (fp1 == 0)
	{
		return (FALSE);
	}

	fp2 = fopen(path2, "rb");
	if (fp2 == 0)
	{
		fclose(fp1);
		return (FALSE);
	}

	rc = TRUE;
	while (TRUE)
	{
		int	c1;
		int	c2;

		c1 = getc(fp1);
		c2 = getc(fp2);

		if (c1 != c2)
		{
			rc = FALSE;
			break;
		}

		if (c1 == EOF)
			break;
	}

	fclose(fp1);
	fclose(fp2);

	return (rc);
}
