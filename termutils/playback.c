/*------------------------------------------------------------------------
 * playback: Playback a screen log file
 *
 * usage: playback [-t term] [-x] [-f file]
 *
 * playback will play back a screen log file one
 * character (escape sequences count as one char)
 * at a time for each keypress.  If the keypress is
 * a number, then that number of characters are played.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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

	printf("  -x           log file is text\n");
	printf("  -f file      log file to playback\n");
	printf("               (default is \"%s\" if text,\n", TCAP_SCRN_LOG_TXT);
	printf("               and \"%s\" if not text)\n",     TCAP_SCRN_LOG_BIN);
}

/*------------------------------------------------------------------------
 * do_char() - output 1 char or 1 esc sequence
 */
static int do_char (TERMINAL *sp, FILE *fp, int text)
{
	int c;

	/*--------------------------------------------------------------------
	 * check if doing a text or binary file
	 */
	if (! text)
	{
		/*----------------------------------------------------------------
		 * binary file
		 */

		/*----------------------------------------------------------------
		 * read in next character
		 */
		c = getc(fp);
		if (c == EOF)
			return (EOF);

		/*----------------------------------------------------------------
		 * output it to the screen
		 */
		tcap_outc(sp, c);

		/*----------------------------------------------------------------
		 * if escape char, process rest of escape sequence
		 *
		 * We assume here that an escape sequence ends when we
		 * get a character that is not a '[', ';', or a digit.
		 */
		if (c == 0x1b)
		{
			while (TRUE)
			{
				c = getc(fp);
				if (c == EOF)
					return (EOF);
				tcap_outc(sp, c);
				if (! (c == '[' || c == ';' || isdigit(c)))
					break;
			}
		}
	}
	else
	{
		/*----------------------------------------------------------------
		 * text file
		 */
		static int	prev_c	= '\n';

		/*----------------------------------------------------------------
		 * read in next char
		 */
		c = getc(fp);
		if (c == EOF)
			return (EOF);

		/*----------------------------------------------------------------
		 * If char is a '<' and the prev char was a new-line,
		 * then we have an escape sequence to be translated.
		 */
		if (c == '<' && prev_c == '\n')
		{
			char		str[128];
			char		buf[128];
			SCRN_CMD	cmd;
			int			p1;
			int			p2;
			int			rc;
			char *		s;

			/*------------------------------------------------------------
			 * read in escape sequence name
			 */
			s = str;
			while (TRUE)
			{
				c = getc(fp);
				if (c == EOF)
					break;
				if (c == '\n')
					break;

				if (c != '>')
					*s++ = c;
			}
			*s = 0;

			/*------------------------------------------------------------
			 * get actual escape sequence to be output
			 */
			rc = tcap_get_cmdstr_by_str(sp, str, &cmd, buf, &p1, &p2);

			/*------------------------------------------------------------
			 * now output it if no error
			 */
			if (rc == 0)
			{
				if (*buf != 0)
				{
					tcap_outflush(sp);
					tcap_outcmd(sp, cmd, buf);
				}
			}
			else if (rc > 0)
			{
				tcap_outflush(sp);
				tcap_outparm(sp, cmd, p1, p2);
			}
		}
		else
		{
			/*------------------------------------------------------------
			 * just a regular char - output it if not a new-line
			 */
			if (c != '\n')
				tcap_outc(sp, c);
			prev_c = c;
		}
	}

	/*--------------------------------------------------------------------
	 * flush output buffer so user can see what happened
	 */
	tcap_outflush(sp);

	return (0);
}

/*------------------------------------------------------------------------
 * main program
 */
int main (int argc, char **argv)
{
	TERMINAL *		sp;
	char *			file		= 0;
	FILE *			fp;
	const char *	term_type	= 0;
	const char *	term_db		= 0;
	int				c;
	int				count;
	int				rc			= 0;
	int				text		= FALSE;

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
	while ((c = getopt(argc, argv, "f:t:T:xZ:")) != EOF)
	{
		switch (c)
		{
		case 'f':
			file = optarg;
			break;

		case 'x':
			text = TRUE;
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
	 * determine filename if not specified
	 */
	if (file == 0)
	{
		if (text)
			file = TCAP_SCRN_LOG_TXT;
		else
			file = TCAP_SCRN_LOG_BIN;
	}

	/*--------------------------------------------------------------------
	 * open playback file
	 */
	fp = fopen(file, "rb");
	if (fp == 0)
	{
		fprintf(stderr, "Cannot open %s\n", file);
		return (EXIT_FAILURE);
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
		return(1);

	tcap_cinit(sp);

	/*--------------------------------------------------------------------
	 * start with a clear screen
	 */
	tcap_clear_screen(sp);
	tcap_outflush(sp);

	/*--------------------------------------------------------------------
	 * now process the file
	 */
	count = 0;
	while (TRUE)
	{
		/*----------------------------------------------------------------
		 * read in user input
		 */
		c = tcap_kbd_get_key(sp, -1, TRUE, TRUE, TRUE);
		if (c == -1)
			break;

		if (c == 'q' || c == 'Q')
			break;

		/*----------------------------------------------------------------
		 * convert to # chars to be processed
		 */
		if (c <= 0xff && isdigit(c))
			count = c - '0';
		else
			count = 1;

		/*----------------------------------------------------------------
		 * now do that many of chars
		 */
		while (count--)
		{
			rc = do_char(sp, fp, text);
			if (rc)
				break;
		}

		/*----------------------------------------------------------------
		 * break if EOF
		 */
		if (rc)
			break;
	}

	/*--------------------------------------------------------------------
	 * restore the screen
	 */
	tcap_cexit(sp, TRUE);
	tcap_delterminal(sp);

	/*--------------------------------------------------------------------
	 * close the playback file
	 */
	fclose(fp);

	return (EXIT_SUCCESS);
}
