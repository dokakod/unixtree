/*------------------------------------------------------------------------
 * dispclrs: test program to display colors
 *
 * usage: dispclrs [-t term]
 */
#include <stdlib.h>
#include <string.h>
#include "curses.h"
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
			const char *	name = get_term_db_name(i);
			const char *	desc = get_term_db_desc(i);

			if (name == 0)
				break;
			printf("                 %-12s %s\n", name, desc);
		}
		printf("               (default is \"%s\")\n", get_term_db());

		printf("  -ZS          debug screen to binary log\n");
		printf("  -Zs          debug screen to text   log\n");
	}
}

/*------------------------------------------------------------------------
 * disp_colors() - display all colors on the screen
 */
static void disp_colors (void)
{
	attr_t save_attr = attrget();
	int i;
	int j;

	for (i=0; i<16; i++)
	{
		for (j=0; j<16; j++)
		{
			char line[80];

			sprintf(line, "%02d/%02d", i, j);

			move(i, 5 * j);
			attrset(A_CLR(i, j));
			addstr(line);
		}
	}

	attrset(save_attr);
}

/*------------------------------------------------------------------------
 * main program
 */
int main (int argc, char **argv)
{
	const char *	term_type	= 0;
	const char *	term_db		= 0;
	int				debug		= 0;
	int				c;

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
			{
				const char *s;

				for (s=optarg; *s; s++)
				{
					switch (*s)
					{
					case 's':
						debug = 2;
						break;

					case 'S':
						debug = 1;
						break;
					}
				}
			}
			break;

		default:
			return (EXIT_FAILURE);
		}
	}

	/*--------------------------------------------------------------------
	 * initialize the screen
	 */
	if (term_db != 0)
	{
			if (set_term_db(term_db) != 0)
			{
				fprintf(stderr, "invalid database type %s\n", term_db);
				usage(argv[0], 0);
				return (EXIT_FAILURE);
			}
	}

	if (term_type != 0)
	{
			if (newterm(term_type, 0, 0) == 0)
			{
				fprintf(stderr, "cannot initialize term type %s\n", term_type);
				return (EXIT_FAILURE);
			}
	}

	if (initscr() == 0)
	{
		fprintf(stderr, "cannot initialize curses\n");
		return (EXIT_FAILURE);
	}

	if (debug)
	{
		set_debug_scr(0, TRUE, debug-1);
	}

	/*--------------------------------------------------------------------
	 * display all colors
	 */
	disp_colors();

	/*--------------------------------------------------------------------
	 * wait for key
	 */
	move(17, 0);
	addstr("press any key to quit ");
	refresh();
	getch();

	/*--------------------------------------------------------------------
	 * restore the screen
	 */
	move(0, 0);
	refresh();

	/*--------------------------------------------------------------------
	 * close out
	 */
	endwin();
	end_curses();

	return (EXIT_SUCCESS);
}
