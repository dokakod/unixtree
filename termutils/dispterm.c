/*------------------------------------------------------------------------
 * dispterm: display all terminal info
 *
 * usage: dispterm [-t term]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libtcap.h"
#include "sysopt.h"

struct trm_data
{
	TERMINAL *	sp;
	int			line_cnt;
	int			max_lines;
	int			interactive;
};
typedef struct trm_data TRM_DATA;

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
	printf("  -i           interactive mode\n");
	printf("  -v           verbose errors\n");
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
 * press_return() - ask for action in interactive mode
 */
static void press_return (TRM_DATA *td)
{
	int c;

	/*--------------------------------------------------------------------
	 * output "--more--" in inverse
	 */
	tcap_outattr(td->sp, A_REVERSE, ' ');
	tcap_outs(td->sp, "--more--");
	tcap_outattr(td->sp, A_NORMAL, ' ');
	tcap_outflush(td->sp);

	/*--------------------------------------------------------------------
	 * wait for a key
	 */
	c = tcap_kbd_get_key(td->sp, -1, TRUE, TRUE, TRUE);

	/*--------------------------------------------------------------------
	 * clear out line
	 */
	tcap_outs(td->sp, "\r        \r");
	tcap_outflush(td->sp);

	/*--------------------------------------------------------------------
	 * process key response
	 */
	switch (c)
	{
	case 'q':
	case 'Q':
		/*----------------------------------------------------------------
		 * quit
		 */
		tcap_cexit(td->sp, TRUE);
		tcap_delterminal(td->sp);
		exit(0);
		/*NOTREACHED*/
		break;

	case ' ':
		/*----------------------------------------------------------------
		 * space -> do another page full
		 */
		td->line_cnt = (td->max_lines - 1);
		break;

	default:
		/*----------------------------------------------------------------
		 * cr -> do another line
		 */
		td->line_cnt = 1;
		break;
	}
}

/*------------------------------------------------------------------------
 * print_line() - display a line and check if screen is full
 */
static void print_line (void *data, const char *line)
{
	TRM_DATA *td	= (TRM_DATA *)data;

	/*--------------------------------------------------------------------
	 * if interactive, use term I/O & check line position
	 */
	if (td->interactive)
	{
		tcap_outs(td->sp, line);
		tcap_outs(td->sp, "\r\n");
		tcap_outflush(td->sp);

		if (--td->line_cnt == 0)
		{
			press_return(td);
		}
	}
	else
	{
		fputs(line, stdout);
		fputs("\n", stdout);
	}
}

/*------------------------------------------------------------------------
 * main program
 */
int main (int argc, char **argv)
{
	TRM_DATA		tds;
	TRM_DATA *		td			= &tds;
	const char *	term_type	= 0;
	const char *	term_db		= 0;
	int				interactive	= isatty(fileno(stdout));
	int				verbose		= FALSE;
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
	while ((c = getopt(argc, argv, "ivt:T:Z:")) != EOF)
	{
		switch (c)
		{
		case 'i':
			interactive = ! interactive;
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

		case 'Z':
			break;

		default:
			usage(argv[0], 0);
			return (EXIT_FAILURE);
		}
	}

	memset(td, 0, sizeof(*td));

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
	 * load screen info
	 */
	td->sp = tcap_newterm(term_type, 0, 0, 0);
	if (td->sp == 0)
	{
		return (EXIT_FAILURE);
	}

	/*--------------------------------------------------------------------
	 * get whether we are interactive
	 */
	td->interactive	= interactive;
	td->max_lines	= tcap_get_max_rows(td->sp);
	td->line_cnt	= (td->max_lines - 1);

	/*--------------------------------------------------------------------
	 * if interactive, set term environment
	 */
	if (td->interactive)
	{
		tcap_cinit(td->sp);
		tcap_set_out_nl(td->sp, TRUE);
	}

	/*--------------------------------------------------------------------
	 * dump the screen info
	 */
	rc = tcap_put_trm_rtn(td->sp, TRUE, print_line, td);

	/*--------------------------------------------------------------------
	 * if interactive, close out term environment
	 */
	if (td->interactive)
	{
		tcap_cexit(td->sp, TRUE);
	}

	/*--------------------------------------------------------------------
	 * delete screen
	 */
	tcap_delterminal(td->sp);

	return (rc == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
