/*------------------------------------------------------------------------
 * ditto: duplicate output on multiple terminals
 */
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "curses.h"
#include "libxvt.h"
#include "sysopt.h"
#include "sysmem.h"
#include "systhread.h"

/*------------------------------------------------------------------------
 * data struct for each terminal
 */
typedef struct
{
	int				input;			/* input  fd				*/
	int				output;			/* output fd				*/
	int				control;		/* TRUE if controlling scr	*/
	XVT_DATA *		xd;				/* XVT_DATA for this screen	*/
	SCREEN *		screen;			/* SCREEN pointer			*/
	WINDOW *		win;			/* stdscr for this screen	*/
	const char *	term;			/* term type of screen		*/
} DITTO;

/*------------------------------------------------------------------------
 * cmdline data
 */
static int			use_threads	= FALSE;
static XVT_DATA *	xd;
static char			term_buf[128];

DITTO *				data		= 0;
int					data_cnt	= 0;
int					num_data	= 0;
int					Continue	= TRUE;

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

	printf("usage: %s [X toolkit options] [options] terminal ...\n", pgm);
	printf("options:\n");
	printf("  -t term      term type to use (default is $TERM)\n");
	printf("  -s           don't use threads\n");

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
	}

	printf("terminal       device name or \"x\"\n");
}

/*------------------------------------------------------------------------
 * open a tty
 */
static int open_tty (const char *path)
{
	char		pathname[256];
	FILE *		fp;
	struct stat	sb;

	if (*path == 0)
	{
		fprintf(stderr, "empty tty name\n");
		return (-1);
	}

	if (*path != '/')
	{
		strcpy(pathname, "/dev/");
		strcat(pathname, path);
		path = pathname;
	}

	if (stat(path, &sb) < 0)
	{
		fprintf(stderr, "%s does not exist\n", path);
		return (-1);
	}

	if ((sb.st_mode & S_IFMT) != S_IFCHR)
	{
		fprintf(stderr, "%s is not a tty\n", path);
		return (-1);
	}

	fp = fopen(path, "a+");
	if (fp == 0)
	{
		fprintf(stderr, "Cannot open %s: ", path);
		perror("");
		return (-1);
	}

	return fileno(fp);
}

/*------------------------------------------------------------------------
 * screen I/O routine
 */
static int ditto_io (DITTO *d)
{
	int ch;
	int j;

	/*--------------------------------------------------------------------
	 * get char from stdin
	 */
	ch = wgetch(d->win);
	if (ch == ERR)
		return (0);
	if (ch == CTL_D)
	{
		delscreen(d->screen);
		d->screen = 0;

		if (d->xd != 0)
		{
			xvt_here_close(d->xd);
			xvt_data_free(d->xd);
			d->xd = 0;
		}
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * output char to all screens
	 */
	for (j = 0; j < num_data; j++)
	{
		d = data + j;

		if (d->screen != 0)
			waddch(d->win, ch);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * thread routine
 */
THREAD_RTN_DECL thread_rtn (void *tdata)
{
	DITTO *	d = (DITTO *)tdata;

	while (Continue)
	{
		if (ditto_io(d))
		{
			if (d->control)
				Continue = FALSE;
			break;
		}
	}

	data_cnt--;

	return THREAD_RTN_VAL;
}

/*------------------------------------------------------------------------
 * actual ditto routine
 */
int ditto (int num, char **argv)
{
	DITTO *			d;
	int				j;
	int				rc;

	data_cnt = ++num;
	num_data = data_cnt;
	data = (DITTO *)MALLOC(data_cnt * sizeof(DITTO));
	if (data == 0)
	{
		fprintf(stderr, "no memory\n");
		return (-1);
	}

	memset(data, 0, data_cnt * sizeof(DITTO));

	/*--------------------------------------------------------------------
	 * open all terminals
	 */
	data[0].input	= fileno(stdin);
	data[0].output	= fileno(stdout);
	data[0].control	= TRUE;
	data[0].term	= term_buf;

	for (j = 1; j < num; j++)
	{
		d = data + j;

		if (*argv[j-1] == 'x' || *argv[j-1] == 'X')
		{
			d->xd = xvt_data_copy(xd);
			rc = xvt_here_run(d->xd, FALSE);
			if (rc)
			{
				fprintf(stderr, "%s\n", xvt_data_get_error_msg(d->xd));
				return (-1);
			}

			d->input	= xvt_data_get_fd_inp(d->xd);
			d->output	= xvt_data_get_fd_out(d->xd);
			d->control	= FALSE;
			d->term		= "xterm";
		}
		else
		{
			rc = open_tty(argv[j-1]);
			if (rc < 0)
			{
				return (-1);
			}

			d->input	= rc;
			d->output	= rc;
			d->term		= term_buf;
		}
	}

	/*--------------------------------------------------------------------
	 * If we got this far, we have open connection(s) to the terminal(s).
	 * Set up the screens.
	 */
	for (j = 0; j < num; j++)
	{
		SCREEN *	s;
		WINDOW *	w;

		/*----------------------------------------------------------------
		 * create the screen
		 */
		d = data + j;

		s = newscreen(d->term, d->output, d->input);
		if (s == 0)
		{
			fprintf(stderr, "Cannot setup screen %d\n", j);
			return (-1);
		}

		/*----------------------------------------------------------------
		 * add event routine if needed
		 */
		if (d->xd != 0)
		{
			scrn_set_event_rtn(s,
				xvt_data_get_event_rtn(d->xd),
				xvt_data_get_event_data(d->xd));
		}

		/*----------------------------------------------------------------
		 * set screen state
		 */
		scrn_cbreak(s);
		scrn_noecho(s);
		scrn_nl(s);

		w = scrn_stdscr(s);
		scrollok(w, TRUE);
		immedok(w, TRUE);
		setdelay(w, 100);

		/*----------------------------------------------------------------
		 * store this screen & stdscr
		 */
		d->screen	= s;
		d->win		= w;

		/*----------------------------------------------------------------
		 * start thread if needed
		 */
		if (use_threads)
			systhread_begin(thread_rtn, d);
	}

	/*--------------------------------------------------------------------
	 * set screen[0] (the real terminal) as the current screen,
	 * so interrupts get handled correctly.
	 */
	set_term(data[0].screen);

	/*--------------------------------------------------------------------
	 * Loop, reading characters from stdin and writing to all the screens.
	 */
	if (use_threads)
	{
		while (data_cnt)
		{
			napms(100);
		}
	}
	else
	{
		while (TRUE)
		{
			if (ditto_io(data))
				break;
		}
	}

	/*--------------------------------------------------------------------
	 * Cleanup and exit
	 */
	for (j = num-1; j >= 0; j--)
	{
		d = data + j;

		if (d->screen != 0)
			delscreen(d->screen);

		if (d->xd != 0)
		{
			xvt_here_close(d->xd);
			xvt_data_free(d->xd);
		}
	}

	FREE(data);

	return (0);
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
	 * initialize xvt struct
	 */
	xd = xvt_data_alloc();
	xvt_data_set_win_label(xd, argv[0]);

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
	xvt_tk_args(xd, argc, argv);
	argc = xvt_data_get_argc(xd);
	argv = xvt_data_get_argv(xd);

	use_threads = systhread_avail();

	while ((c = getopt(argc, argv, "st:T:Z:")) != EOF)
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

		case 's':
			use_threads = FALSE;
			break;

		default:
			usage(argv[0], 0);
			return (EXIT_FAILURE);
		}
	}

	/*--------------------------------------------------------------------
	 * cache original term value
	 */
	if (term_type == 0)
		term_type = getenv("TERM");
	strcpy(term_buf, term_type);

	/*--------------------------------------------------------------------
	 * set term database type if specified
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

	/*--------------------------------------------------------------------
	 * check if any terminals specified
	 */
	if ((argc - optind) <= 0)
	{
		fprintf(stderr, "no terminals specified\n");
		usage(argv[0], 0);
		return (EXIT_FAILURE);
	}

	/*--------------------------------------------------------------------
	 * now do it
	 */
	rc = ditto(argc - optind, argv + optind);

	/*--------------------------------------------------------------------
	 * free main XVT struct
	 */
	xvt_data_free(xd);

	return (rc == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
