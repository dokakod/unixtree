/*
 * gzip main program
 */

#include <stdio.h>
#include <stdlib.h>
#include "libgzip.h"

/*------------------------------------------------------------------------
 * need this if getopt() is not available
 */
#include "sysopt.h"

static char *progname;       /* program name */

static void usage (void)
{
	static const char *	help_msg[] =
	{
		"options:",
		"  -c    write on standard output & keep original files unchanged",
		"  -d    decompress",
		"file    files to (de)compress. If none or \"-\", use standard input.",
		0
	};
	const char **p;

	fprintf(stderr, "usage: %s [options] [file ...]\n", progname);

	for (p=help_msg; *p; p++)
		fprintf(stderr, "%s\n", *p++);
}

int main (int argc, char **argv)
{
	int optc;
	int exit_code;

	int bDecomp = 0;
	const char *oname = 0;
	char msgbuf[256];

	progname = argv[0];

	while ((optc = getopt(argc, argv, "cdh?")) != EOF)
	{
		switch (optc)
		{
		case 'c':
			oname = "-";
			break;

		case 'd':
			bDecomp = 1;
			break;

		case 'h':
		case '?':
			usage();
			return (0);

		default:
			fprintf(stderr, "%s: invalid option %c\n", progname, optopt);
			usage();
			return (1);
		}
	}

	/* And get to work */
	if (optind < argc)
	{
		for (; optind < argc; optind++)
		{
			exit_code = gzip(argv[optind], oname, bDecomp, msgbuf);
			if (exit_code != 0)
				break;
			remove(argv[optind]);
		}
	}
	else
	{
		/* Standard input */
		exit_code = gzip("-", oname, bDecomp, msgbuf);
	}

	if (exit_code != 0)
	{
		if (*msgbuf != 0)
			fprintf(stderr, "%s: %s\n", progname, msgbuf);
	}

	return exit_code;
}
