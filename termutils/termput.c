/*------------------------------------------------------------------------
 * termput: display a terminal entry value
 *
 * usage: termput [-t term] name
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

	printf("usage: %s [options] cap-name [parm ...]\n", pgm);
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

	printf("cap-name       name of capability to display\n");
	printf("parm           parameters to use in evaluating capability\n");
}

/*------------------------------------------------------------------------
 * main program
 */
int main (int argc, char **argv)
{
	TERMINAL *		sp;
	SCRN_CMD		cmd;
	const char *	term_type	= 0;
	const char *	term_db		= 0;
	const char *	name;
	char			cmd_buf[64];
	int				p1			= 0;
	int				p2			= 0;
	int				got_parms	= FALSE;
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
	 * check for capability name
	 */
	if (optind >= argc)
	{
		fprintf(stderr, "%s: no name specified\n", argv[0]);
		usage(argv[0], 0);
		return (EXIT_FAILURE);
	}

	name = argv[optind++];

	/*--------------------------------------------------------------------
	 * check for parameters
	 */
	if (optind < argc)
	{
		got_parms = TRUE;

		p1 = atoi(argv[optind++]);

		if (optind < argc)
			p2 = atoi(argv[optind++]);
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
	 * load screen struct
	 */
	sp = tcap_newterm(term_type, 0, 0, 0);
	if (sp == 0)
		return (EXIT_FAILURE);

	/*--------------------------------------------------------------------
	 * extract capability string
	 */
	rc = tcap_get_cmdstr_by_name(sp, name, &cmd, cmd_buf);
	if (rc)
	{
		fprintf(stderr, "%s: invalid name %s\n", argv[0], name);
	}

	/*--------------------------------------------------------------------
	 * check if we should evaluate parameters
	 */
	if (rc == 0 && got_parms)
	{
		tcap_eval_parm(sp, cmd_buf, cmd, p1, p2);
	}

	/*--------------------------------------------------------------------
	 * delete the screen struct
	 */
	tcap_delterminal(sp);

	/*--------------------------------------------------------------------
	 * output the string if successful
	 */
	if (rc == 0)
	{
		printf("%s", cmd_buf);
		fflush(stdout);
	}

	return (rc == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
