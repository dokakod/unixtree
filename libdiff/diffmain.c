/*------------------------------------------------------------------------
 * a test "diff" program
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libdiff.h"

/*------------------------------------------------------------------------
 * need this if getopt() is not available
 */
#include "sysopt.h"

static void usage (FILE *fp, const char *pgm)
{
	fprintf(fp, "usage: %s [options] file1 file2\n", pgm);
	fprintf(fp, "options:\n");
	fprintf(fp, "  -i       ignore case\n");
	fprintf(fp, "  -b       ignore blank lines\n");
	fprintf(fp, "  -w       ignore white-space in lines\n");
	fprintf(fp, "  -s       display side-by-side report\n");
	fprintf(fp, "  -c       show only changed lines in side-by-side display\n");
	fprintf(fp, "  -n       show line-numbers in side-by-side display\n");
	fprintf(fp, "  -t len   specify tab length (default = 8)\n");
	fprintf(fp, "  -p len   specify page width (default = 80)\n");
}

static void print_line (void *data, const char *line)
{
	fputs(line, (FILE *)data);
}

int main (int argc, char **argv)
{
	DIFF_LINE *		dl;
	DIFF_OPTS		opts;
	const char *	pgm;
	const char *	file1;
	const char *	file2;
	char			msgbuf[128];
	int				ignore_case	= 0;
	int				ignore_bl	= 0;
	int				ignore_ws	= 0;
	int				show_sbs	= 0;
	int				show_nums	= 0;
	int				show_change	= 0;
	int				tab_width	= 8;
	int				page_width	= 80;
	int				c;
	int				rc;

	/*--------------------------------------------------------------------
	 * get program name
	 */
	pgm = argv[0];

	/*--------------------------------------------------------------------
	 * parse all options
	 */
	while ((c = getopt(argc, argv, "ibwsnct:w:?")) != EOF)
	{
		switch (c)
		{
		case 'i':	ignore_case	= 1;			break;
		case 'b':	ignore_bl	= 1;			break;
		case 'w':	ignore_ws	= 1;			break;
		case 's':	show_sbs	= 1;			break;
		case 'n':	show_nums	= 1;			break;
		case 'c':	show_change	= 1;			break;
		case 't':	tab_width	= atoi(optarg);	break;
		case 'p':	page_width	= atoi(optarg);	break;

		case '?':
			usage(stdout, pgm);
			return (0);

		default:
			usage(stderr, pgm);
			return (1);
		}
	}

	/*--------------------------------------------------------------------
	 * must be 2 files here
	 */
	if ((argc - optind) != 2)
	{
		fprintf(stderr, "%s: need 2 files\n", pgm);
		return (1);
	}

	file1 = argv[optind++];
	file2 = argv[optind++];

	/*--------------------------------------------------------------------
	 * do the diff
	 */
	memset(&opts, 0, sizeof(opts));
	opts.ignore_blank_lines_flag	= ignore_bl;
	opts.ignore_all_space_flag		= ignore_ws;
	opts.ignore_case_flag			= ignore_case;

	dl = diff_engine(file1, file2, &opts, &rc, msgbuf);

	/*--------------------------------------------------------------------
	 * check diff result
	 */
	switch (rc)
	{
	case DIFF_SAME_FILE:	printf("files same\n");				return (0);
	case DIFF_TXT_SAME:		printf("text files identical\n");	return (0);
	case DIFF_BIN_SAME:		printf("binary files identical\n");	return (0);
	case DIFF_BIN_DIFF:		printf("binary files differ\n");	return (1);

	case DIFF_TXT_DIFF:
		break;

	default:
		fprintf(stderr, "%s: %s\n", pgm, msgbuf);
		return (1);
	}

	/*--------------------------------------------------------------------
	 * we have two different text files. print it out
	 */
	if (show_sbs)
	{
		diff_print_side(print_line, stdout, dl,
			show_change, page_width, show_nums, tab_width);
	}
	else
	{
		diff_print_diff(print_line, stdout, dl);
	}

	diff_free_lines(dl);

	return (1);
}
