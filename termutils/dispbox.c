/*------------------------------------------------------------------------
 * dispbox: a test program to draw boxes
 *
 * usage: dispbox [-t term]
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
 * do_boxes() - display all boxes
 */
static void do_boxes (void)
{
	/*--------------------------------------------------------------------
	 * top rows
	 */
	move(0, 0);

	box_chr(B_TL, B_ASCII);
	box_chr(B_HO, B_ASCII);
	box_chr(B_TC, B_ASCII);
	box_chr(B_HO, B_ASCII);
	box_chr(B_TR, B_ASCII);

	addstr("   ");

	box_chr(B_TL, B_SVSH);
	box_chr(B_HO, B_SVSH);
	box_chr(B_TC, B_SVSH);
	box_chr(B_HO, B_SVSH);
	box_chr(B_TR, B_SVSH);

	addstr("   ");

	box_chr(B_TL, B_SVDH);
	box_chr(B_HO, B_SVDH);
	box_chr(B_TC, B_SVDH);
	box_chr(B_HO, B_SVDH);
	box_chr(B_TR, B_SVDH);

	addstr("   ");

	box_chr(B_TL, B_DVSH);
	box_chr(B_HO, B_DVSH);
	box_chr(B_TC, B_DVSH);
	box_chr(B_HO, B_DVSH);
	box_chr(B_TR, B_DVSH);

	addstr("   ");

	box_chr(B_TL, B_DVDH);
	box_chr(B_HO, B_DVDH);
	box_chr(B_TC, B_DVDH);
	box_chr(B_HO, B_DVDH);
	box_chr(B_TR, B_DVDH);

	/*--------------------------------------------------------------------
	 * sides
	 */
	move(1, 0);

	box_chr(B_VE, B_ASCII);
	addch(' ');
	box_chr(B_VE, B_ASCII);
	addch(' ');
	box_chr(B_VE, B_ASCII);

	addstr("   ");

	box_chr(B_VE, B_SVSH);
	addch(' ');
	box_chr(B_VE, B_SVSH);
	addch(' ');
	box_chr(B_VE, B_SVSH);

	addstr("   ");

	box_chr(B_VE, B_SVDH);
	addch(' ');
	box_chr(B_VE, B_SVDH);
	addch(' ');
	box_chr(B_VE, B_SVDH);

	addstr("   ");

	box_chr(B_VE, B_DVSH);
	addch(' ');
	box_chr(B_VE, B_DVSH);
	addch(' ');
	box_chr(B_VE, B_DVSH);

	addstr("   ");

	box_chr(B_VE, B_DVDH);
	addch(' ');
	box_chr(B_VE, B_DVDH);
	addch(' ');
	box_chr(B_VE, B_DVDH);

	/*--------------------------------------------------------------------
	 * middle rows
	 */
	move(2, 0);

	box_chr(B_ML, B_ASCII);
	box_chr(B_HO, B_ASCII);
	box_chr(B_MC, B_ASCII);
	box_chr(B_HO, B_ASCII);
	box_chr(B_MR, B_ASCII);

	addstr("   ");

	box_chr(B_ML, B_SVSH);
	box_chr(B_HO, B_SVSH);
	box_chr(B_MC, B_SVSH);
	box_chr(B_HO, B_SVSH);
	box_chr(B_MR, B_SVSH);

	addstr("   ");

	box_chr(B_ML, B_SVDH);
	box_chr(B_HO, B_SVDH);
	box_chr(B_MC, B_SVDH);
	box_chr(B_HO, B_SVDH);
	box_chr(B_MR, B_SVDH);

	addstr("   ");

	box_chr(B_ML, B_DVSH);
	box_chr(B_HO, B_DVSH);
	box_chr(B_MC, B_DVSH);
	box_chr(B_HO, B_DVSH);
	box_chr(B_MR, B_DVSH);

	addstr("   ");

	box_chr(B_ML, B_DVDH);
	box_chr(B_HO, B_DVDH);
	box_chr(B_MC, B_DVDH);
	box_chr(B_HO, B_DVDH);
	box_chr(B_MR, B_DVDH);

	/*--------------------------------------------------------------------
	 * sides
	 */
	move(3, 0);

	box_chr(B_VE, B_ASCII);
	addch(' ');
	box_chr(B_VE, B_ASCII);
	addch(' ');
	box_chr(B_VE, B_ASCII);

	addstr("   ");

	box_chr(B_VE, B_SVSH);
	addch(' ');
	box_chr(B_VE, B_SVSH);
	addch(' ');
	box_chr(B_VE, B_SVSH);

	addstr("   ");

	box_chr(B_VE, B_SVDH);
	addch(' ');
	box_chr(B_VE, B_SVDH);
	addch(' ');
	box_chr(B_VE, B_SVDH);

	addstr("   ");

	box_chr(B_VE, B_DVSH);
	addch(' ');
	box_chr(B_VE, B_DVSH);
	addch(' ');
	box_chr(B_VE, B_DVSH);

	addstr("   ");

	box_chr(B_VE, B_DVDH);
	addch(' ');
	box_chr(B_VE, B_DVDH);
	addch(' ');
	box_chr(B_VE, B_DVDH);

	/*--------------------------------------------------------------------
	 * bottom rows
	 */
	move(4, 0);

	box_chr(B_BL, B_ASCII);
	box_chr(B_HO, B_ASCII);
	box_chr(B_BC, B_ASCII);
	box_chr(B_HO, B_ASCII);
	box_chr(B_BR, B_ASCII);

	addstr("   ");

	box_chr(B_BL, B_SVSH);
	box_chr(B_HO, B_SVSH);
	box_chr(B_BC, B_SVSH);
	box_chr(B_HO, B_SVSH);
	box_chr(B_BR, B_SVSH);

	addstr("   ");

	box_chr(B_BL, B_SVDH);
	box_chr(B_HO, B_SVDH);
	box_chr(B_BC, B_SVDH);
	box_chr(B_HO, B_SVDH);
	box_chr(B_BR, B_SVDH);

	addstr("   ");

	box_chr(B_BL, B_DVSH);
	box_chr(B_HO, B_DVSH);
	box_chr(B_BC, B_DVSH);
	box_chr(B_HO, B_DVSH);
	box_chr(B_BR, B_DVSH);

	addstr("   ");

	box_chr(B_BL, B_DVDH);
	box_chr(B_HO, B_DVDH);
	box_chr(B_BC, B_DVDH);
	box_chr(B_HO, B_DVDH);
	box_chr(B_BR, B_DVDH);

	/*--------------------------------------------------------------------
	 * names
	 */
	move(6, 0);

	addstr("ascii   svsh    svdh    dvsh    dvdh");
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
			usage(argv[0], 0);
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
	 * draw the boxes
	 */
	do_boxes();

	/*--------------------------------------------------------------------
	 * wait for key
	 */
	move(8, 0);
	addstr("press any key to quit ");
	refresh();
	getch();
	move(10, 0);
	refresh();

	/*--------------------------------------------------------------------
	 * close out
	 */
	endwin();
	end_curses();

	return (EXIT_SUCCESS);
}
