/*------------------------------------------------------------------------
 *	This program will check if a specified file is "newer" than
 *	another file.
 *
 *	usage: chknewer file1 file2
 *
 *	exits:	0 file1 is newer or file2 does not exist
 *			1 file2 exists and is newer
 *			2 file1 does not exist
 *
 * This program is used by the "deliver" script to determine whether
 * a file should be copied.
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define EXIT_NO_FILE2		0
#define EXIT_FILE1_NEWER	0
#define EXIT_FILE2_NEWER	1
#define EXIT_NO_FILE1		2

int main (int argc, char **argv)
{
	const char *	pgm		= argv[0];
	const char *	file1;
	const char *	file2;
	struct stat		stbuf1;
	struct stat		stbuf2;

	/*-----------------------------------------------------------
	 * check for help
	 */
	if (argc > 1)
	{
		if (strcmp(argv[1], "-?")     == 0 ||
	    	strcmp(argv[1], "-help")  == 0 ||
	    	strcmp(argv[1], "--help") == 0)
		{
			printf("usage: %s file1 file2\n", pgm);
			return (0);
		}
	}

	/*--------------------------------------------------------------------
	 * check if we have 2 args
	 */
	if (argc != 3)
	{
		fprintf(stderr, "%s: invalid number of args\n", pgm);
		fprintf(stderr, "usage: %s file1 file2\n", pgm);
		return (1);
	}

	file1 = argv[1];
	file2 = argv[2];

	/*--------------------------------------------------------------------
	 * first file should exist
	 */
	if (stat(file1, &stbuf1) != 0)
	{
		fprintf(stderr, "%s: file %s does not exist\n",
			pgm, file1);
		return (EXIT_NO_FILE1);
	}

	/*--------------------------------------------------------------------
	 * check if second file exists
	 */
	if (stat(file2, &stbuf2) != 0)
	{
		return (EXIT_NO_FILE2);
	}

	/*--------------------------------------------------------------------
	 * compare file fimes
	 */
	if (stbuf1.st_mtime > stbuf2.st_mtime)
	{
		return (EXIT_FILE1_NEWER);
	}

	return (EXIT_FILE2_NEWER);
}
