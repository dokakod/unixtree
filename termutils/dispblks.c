/*------------------------------------------------------------------------
 * dispblks: test program to display block letters
 *
 * usage: dispblks [-t term] [-a] [-n]
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

	printf("  -a           display all alpha chars\n");
	printf("  -n           display all numbers\n");
	printf("  -c           display using char if alt-chars not available\n");
}

/*------------------------------------------------------------------------
 * do_blk_lets() - display all block letters
 */
static void do_blk_lets (int t)
{
	blk_str( 0, 0, "abcdefgh", t);
	blk_str( 6, 0, "ijklmnop", t);
	blk_str(12, 0, "qrstuvwx", t);
	blk_str(18, 0, "yz", t);

	move(24, 0);
	addstr("Press any key to continue ");

	refresh();
	getch();
}

/*------------------------------------------------------------------------
 * do_blk_nums() - display all block numbers
 */
static void do_blk_nums (int t)
{
	blk_str( 0, 0, "01234", t);
	blk_str( 6, 0, "56789", t);

	move(24, 0);
	addstr("Press any key to continue ");
	refresh();

	getch();
}

/*------------------------------------------------------------------------
 * do_blks() - read in strings & display them as block letters
 */
static void do_blks (int t)
{
	int max_chars = getmaxx(stdscr) / (BLK_WIDTH + 1);

	erase();

	while (TRUE)
	{
		char	str[80];
		int		c;
		int		i;

		move(1, 0);
		clrtoeol();
		addstr("string to show (CR to quit): ");
		refresh();

		i = 0;
		while (TRUE)
		{
			if (i >= max_chars)
				break;

			c = getch();
			if (c == KEY_RETURN)
				break;
			if (c < ' ' || c > 0xff)
				continue;
			addch(c);
			refresh();

			str[i++] = c;
		}
		str[i] = 0;

		if (i == 0)
			break;

		move(4, 0);
		clrtobot();
		blk_str(4, -1, str, t);
		refresh();
	}
}

/*------------------------------------------------------------------------
 * main program
 */
int main (int argc, char **argv)
{
	const char *	term_type	= 0;
	const char *	term_db		= 0;
	int				opt			= 0;
	int				t			= FALSE;
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
	while ((c = getopt(argc, argv, "anct:T:Z:")) != EOF)
	{
		switch (c)
		{
		case 'a':
		case 'n':
			opt = c;
			break;

		case 'c':
			t = TRUE;
			break;

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
	 * now do it
	 */
	switch (opt)
	{
	case 'a':	do_blk_lets(t);	break;
	case 'n':	do_blk_nums(t);	break;
	default:	do_blks(t);		break;
	}

	/*--------------------------------------------------------------------
	 * restore the screen
	 */
	erase();
	move(0, 0);
	refresh();

	/*--------------------------------------------------------------------
	 * close out
	 */
	endwin();
	end_curses();

	return (EXIT_SUCCESS);
}
