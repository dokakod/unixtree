/*------------------------------------------------------------------------
 * dispkeys: display name of each key depressed
 *
 * usage: dispkeys [-t term] [-d] [-m]
 *
 * If -d is specified, then esc-sequences will not be evaluated.
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

	printf("  -d           display key strokes\n");
	printf("  -m           ignore mouse input\n");
}

/*------------------------------------------------------------------------
 * async key routines
 */
static int async_rtn (int key, void *data)
{
	char buf[64];

	printf("{%s} ", tcap_get_key_name(key, buf));
	fflush(stdout);

	return (1);
}

/*------------------------------------------------------------------------
 * main program
 */
int main (int argc, char **argv)
{
	TERMINAL *		sp;
	int				c;
	const char *	term_type	= 0;
	const char *	term_db		= 0;
	int				debug		= FALSE;
	int				mouse		= TRUE;

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
	while ((c = getopt(argc, argv, "dmt:T:Z:")) != EOF)
	{
		switch (c)
		{
		case 'd':
			debug = TRUE;
			break;

		case 'm':
			mouse = FALSE;
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
	 * initialize the screen
	 */
	sp = tcap_newterm(term_type, 0, 0, 0);
	if (sp == 0)
		return (EXIT_FAILURE);

	tcap_cinit(sp);

	/*--------------------------------------------------------------------
	 * set NL to CR/NL on output
	 */
	tcap_set_out_nl(sp, TRUE);

	/*--------------------------------------------------------------------
	 * initialize the mouse if requested
	 */
	if (mouse)
		tcap_mouse_init(sp);

	/*--------------------------------------------------------------------
	 * setup async keys
	 */
	tcap_kbd_add_async(sp, KEY_SHIFT_PRESS,   async_rtn, 0);
	tcap_kbd_add_async(sp, KEY_SHIFT_RELEASE, async_rtn, 0);

	tcap_kbd_add_async(sp, KEY_CTRL_PRESS,    async_rtn, 0);
	tcap_kbd_add_async(sp, KEY_CTRL_RELEASE,  async_rtn, 0);

	tcap_kbd_add_async(sp, KEY_ALT_PRESS,     async_rtn, 0);
	tcap_kbd_add_async(sp, KEY_ALT_RELEASE,   async_rtn, 0);

	/*--------------------------------------------------------------------
	 * process key presses
	 */
	while (TRUE)
	{
		char buf[128];

		printf("key: ");
		fflush(stdout);
		c = tcap_kbd_get_key(sp, -1, !debug, TRUE, TRUE);

		if (c == KEY_MOUSE)
		{
			c = tcap_mouse_get_event(sp);
			printf("%s at (%d,%d)\r\n", tcap_get_key_name(c, buf),
				tcap_mouse_get_x(sp), tcap_mouse_get_y(sp));
		}
		else
		{
			printf("%s\r\n", tcap_get_key_name(c, buf));
		}

		if (c == CTL_D)
		{
			break;
		}
	}

	/*--------------------------------------------------------------------
	 * restore the screen
	 */
	tcap_cexit(sp, TRUE);
	tcap_delterminal(sp);

	return (EXIT_SUCCESS);
}
