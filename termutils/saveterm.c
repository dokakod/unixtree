/*------------------------------------------------------------------------
 * saveterm: Program to save a trm file
 *
 * usage: saveterm [-t term] [-c] [-v] [-f file]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
		printf("  -ZM          debug malloc chain at exit\n");
	}

	printf("  -c           save compiled term file\n");
	printf("  -v           verbose mode\n");
	printf("  -f file      name of file to save\n");
	printf("               (default is \"<term>.trm\" if not compiled,\n");
	printf("               and \"<term>.trc\" if compiled)\n");
}

/*------------------------------------------------------------------------
 * main program
 */
int main (int argc, char **argv)
{
	TERMINAL *		sp;
	const char *	term_type	= 0;
	const char *	term_db		= 0;
	const char *	file		= 0;
	char			path[256];
	int				rc;
	int				c;
	int				compile		= FALSE;
	int				verbose		= FALSE;

	/*--------------------------------------------------------------------
	 * default database type is "all"
	 */
	tcap_set_term_db("all");

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
	while ((c = getopt(argc, argv, "cvt:f:T:Z:")) != EOF)
	{
		switch (c)
		{
		case 'c':
			compile = TRUE;
			break;

		case 'v':
			verbose = TRUE;
			break;

		case 't':
			term_type = optarg;
			break;

		case 'T':
			term_db = optarg;
			break;

		case 'f':
			file = optarg;
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
	 * get term type if not specified
	 */
	if (term_type == 0 || *term_type == 0)
	{
		term_type = getenv("TERM");
	}

	if (term_type == 0 || *term_type == 0)
	{
		fprintf(stderr, "%s: no terminal type specified\n", argv[0]);
		return (EXIT_FAILURE);
	}

	/*--------------------------------------------------------------------
	 * create default filename if one not specified
	 */
	if (file == 0)
	{
		strcpy(path, term_type);

		if (compile)
			strcat(path, ".trc");
		else
			strcat(path, ".trm");

		file = path;
	}

	/*--------------------------------------------------------------------
	 * load screen struct
	 */
	sp = tcap_newterm(term_type, 0, 0, 0);
	if (sp == 0)
		return (EXIT_FAILURE);

	/*--------------------------------------------------------------------
	 * now save it
	 */
	if (compile)
		rc = tcap_put_trc(sp, file);
	else
		rc = tcap_put_trm(sp, FALSE, file);

	/*--------------------------------------------------------------------
	 * delete the screen struct
	 */
	tcap_delterminal(sp);

	/*--------------------------------------------------------------------
	 * check if any errors
	 */
	if (rc)
	{
		fprintf(stderr, "%s: cannot save file %s\n", argv[0], path);
	}
	else
	{
		if (verbose)
			fprintf(stderr, "%s written\n", path);
	}

	return (rc == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
