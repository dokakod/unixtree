/*------------------------------------------------------------------------
 * dumpterm: dump the contents of the term database
 *
 * usage: dumpterm [-T type] [-t term]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libtcap.h"
#include "sysopt.h"

/*------------------------------------------------------------------------
 * usage() - output usage message
 */
static void usage (const char *pgm, int level)
{
	if (level == 0)
	{
		fprintf(stderr, "Type \"%s -?\" for help.\n", pgm);
		return;
	}

	printf("usage: %s [options]\n", pgm);
	printf("options:\n");
	printf("  -t term      term type to use (default is $TERM)\n");

	if (level > 1)
	{
		int i;

		printf("  -T type      database type\n");
		printf("               Valid values are:\n");
		for (i=0; ; i++)
		{
			const char *	name = tcap_get_term_db_name(i);
			const char *	desc = tcap_get_term_db_desc(i);

			if (name == 0)
				break;
			printf("                 %-12s %s\n", name, desc);
		}
		printf("               (default is \"%s\")\n", tcap_get_term_db());
	}
}

/*------------------------------------------------------------------------
 * main program
 */
int main (int argc, char **argv)
{
	const char *	term_type	= 0;
	const char *	term_db		= 0;
	int				c;
	int				rc;

	/*--------------------------------------------------------------------
	 * process "-help" & friends which don't fit the getopt standard
	 */
	if (argc > 1)
	{
		if (strcmp(argv[1], "-?")     == 0 ||
			strcmp(argv[1], "-help")  == 0 ||
			strcmp(argv[1], "--help") == 0)
		{
			usage(argv[0], 1);
			return (EXIT_SUCCESS);
		}

		if (strcmp(argv[1], "-??")       == 0 ||
			strcmp(argv[1], "-helpall")  == 0 ||
			strcmp(argv[1], "--helpall") == 0)
		{
			usage(argv[0], 2);
			return (EXIT_SUCCESS);
		}
	}

	/*--------------------------------------------------------------------
	 * process cmd-line options
	 */
	while ((c = getopt(argc, argv, "t:T:Z:")) != EOF)
	{
		switch (c)
		{
		case 't':
			term_type = optarg;
			break;

		case 'T':
			term_db = optarg;
			break;

		case 'Z':
			break;

		default:
			usage(argv[0], 0);
			return (EXIT_FAILURE);
		}
	}

	/*--------------------------------------------------------------------
	 * set term database type if specified
	 */
	if (term_db != 0 && *term_db != 0)
	{
		if (tcap_set_term_db(term_db) != 0)
		{
			fprintf(stderr, "invalid database type %s\n", term_db);
			usage(argv[0], 0);
			return (EXIT_FAILURE);
		}
	}

	/*--------------------------------------------------------------------
	 * dump the database
	 */
	if (term_type == 0 || *term_type == 0)
		term_type = getenv("TERM");

	rc = tcap_dump_term(term_type, stdout);

	return (rc == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
