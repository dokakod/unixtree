/*------------------------------------------------------------------------
 * test program for magic
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libmagic.h"

/*------------------------------------------------------------------------
 * need this if getopt() is not available
 */
#include "sysopt.h"

/*------------------------------------------------------------------------
 * default magic file list
 */
#define MAGIC_PATH		"/etc/magic"

/*------------------------------------------------------------------------
 * usage
 */
static void usage (const char *pgm)
{
	printf("usage: %s [options] file ...\n", pgm);
	printf("options:\n");
	printf("  -c          check magic files for errors\n");
	printf("  -m mfiles   specify list of magic files\n");
	printf("              (default is $MAGIC then %s)\n", MAGIC_PATH);
	printf("  -f ffiles   specify list of files to examine\n");
}

/*------------------------------------------------------------------------
 * check_file - check one file
 */
static void check_file (MAGIC *magics, const char *filename)
{
	char	result[256];

	mag_check(magics, filename, result);
	printf("%s: %s\n", filename, result);
}

/*------------------------------------------------------------------------
 * main - parse arguments and handle options
 */
int
main(int argc, char **argv)
{
	int				check	= 0;
	const char *	ffiles	= 0;
	const char *	magic_path;
	const char *	pgm;
	MAGIC *			magics;
	int				c;

	/*--------------------------------------------------------------------
	 * get program name
	 */
	pgm = argv[0];

	/*--------------------------------------------------------------------
	 * get default magic path
	 */
	magic_path = getenv("MAGIC");
	if (magic_path == 0)
		magic_path = MAGIC_PATH;

	/*--------------------------------------------------------------------
	 * parse all options
	 */
	while ((c = getopt(argc, argv, "cm:f:?")) != EOF)
	{
		switch (c)
		{
		case 'c':
			++check;
			break;

		case 'm':
			magic_path = optarg;
			break;

		case 'f':
			ffiles = optarg;
			break;

		case '?':
		default:
			usage(pgm);
			return (1);
		}
	}

	/*--------------------------------------------------------------------
	 * now load the magic files & bail if that's all asked for
	 */
	magics = mag_parse(magic_path, check);
	if (check)
		return (0);

	/*--------------------------------------------------------------------
	 * if list of files given, process it
	 */
	if (ffiles != 0 && *ffiles != 0)
	{
		FILE *	fp = fopen(ffiles, "r");

		if (fp != 0)
		{
			for (;;)
			{
				char line[BUFSIZ];

				if (fgets(line, sizeof(line), fp) == 0)
					break;

				line[strlen(line)-1] = 0;
				check_file(magics, line);
			}
			fclose(fp);
		}
	}

	/*--------------------------------------------------------------------
	 * process all files on cmd line
	 */
	for (; optind < argc; optind++)
	{
		check_file(magics, argv[optind]);
	}

	/*--------------------------------------------------------------------
	 * free back magic list
	 */
	mag_free(magics);

	return (0);
}
