/*------------------------------------------------------------------------
 * dispsnap: display a snap dump file on the screen
 *
 * usage: dispsnap [-f file] [-t term]
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

	printf("  -f file      snapshot file to display\n");
	printf("               (default is \"%s\")\n", WIN_SNAP_FILE);
}

/*------------------------------------------------------------------------
 * dispsnap() - display a snap file
 */
static void dispsnap (const char *file)
{
	FILE *		fp;
	WINDOW *	win;
	int			c;

	/*--------------------------------------------------------------------
	 * open the snap-file
	 */
	fp = fopen(file, "rb");
	if (fp == 0)
		return;

	/*--------------------------------------------------------------------
	 * process all snapshots in the file
	 */
	while (TRUE)
	{
		/*----------------------------------------------------------------
		 * load snapshot into window
		 */
		win = getwin(fp);
		if (win == 0)
			break;

		/*----------------------------------------------------------------
		 * display the window
		 */
		touchwin(win);
		leaveok(win, FALSE);
		wmove(win, getcury(win), getcurx(win));
		wrefresh(win);

		/*----------------------------------------------------------------
		 * wait for a keypress
		 */
		c = wgetch(win);

		/*----------------------------------------------------------------
		 * delete the window
		 */
		delwin(win);

		/*----------------------------------------------------------------
		 * break if ESC was entered, otherwise loop to next snapshot
		 */
		if (c == KEY_ESCAPE)
			break;
	}

	/*--------------------------------------------------------------------
	 * close the file
	 */
	fclose(fp);
}

/*------------------------------------------------------------------------
 * main program
 */
int main (int argc, char **argv)
{
	const char *	term_type	= 0;
	const char *	term_db		= 0;
	const char *	file		= WIN_SNAP_FILE;
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
	while ((c = getopt(argc, argv, "t:T:f:Z:")) != EOF)
	{
		switch (c)
		{
		case 'f':
			file = optarg;
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
	 * display the snap file
	 */
	dispsnap(file);

	/*--------------------------------------------------------------------
	 * restore the screen
	 */
	clear();
	refresh();

	/*--------------------------------------------------------------------
	 * close out
	 */
	endwin();
	end_curses();

	return (EXIT_SUCCESS);
}
