/*------------------------------------------------------------------------
 *
 *	Description:
 *
 *	This program reads an ASCII file and generates a C source
 *	file in the form of a text list.  The intent of this is to
 *	embed a long list of text in a C program without having to
 *	use an auxilliary data file.
 *
 *	The output is a single C table declared as:
 *
 *		static const char *table_name[] =
 *		{
 *			"line 1",
 *			"line 2",
 *			"line 3",
 *			0
 *		};
 *
 *	The /static/ part is enabled by the -s command line option,
 *	and the generated table name is specified with the -t argument.
 *
 *	Since the input source file may have comments that should not
 *	be included in the output file, there is an option to strip comments.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "sysopt.h"

/*------------------------------------------------------------------------
 * handy defines
 */
#ifndef TRUE
#define TRUE			1
#endif

#ifndef FALSE
#define FALSE			0
#endif

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS	0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE	1
#endif

/*------------------------------------------------------------------------
 * cmd-line option list
 */
#define OPTS			"t:o:i:d:x:n:efcslwp"

/*------------------------------------------------------------------------
 * cmdline variables
 */
static const char *	pgm					= 0;
static const char *	Table_name			= "text_list";
static const char *	comment_chars		= 0;
static const char *	notes				= 0;
static int			show_from			= FALSE;
static int			should_be_static	= FALSE;
static int			strip_leading		= FALSE;
static int			delete_ws			= FALSE;
static int			empty_ok			= FALSE;
static int			make_ptr			= FALSE;
static int			compress_ws			= FALSE;
static int			expand_tabs			= 0;

static int			filecnt				= 0;
static int			files_this_tbl		= 0;

/*------------------------------------------------------------------------
 * usage()
 *
 * display a usage message
 */
static void
usage (int flag)
{
	if (flag)
	{
		printf("usage: %s [options] [filename ...]\n", pgm);
		printf("options:\n");
		printf("  -t tbl-name   specify name of table\n");
		printf("  -o out-file   specify output file\n");
		printf("  -i inp-file   specify input  file\n");
		printf("  -d char       delete comments beginning with any \"char\"\n");
		printf("  -x n          expand tabs to \"n\" spaces\n");
		printf("  -n notes      include \"notes\" in a comment\n");
		printf("  -s            table should be \"static\"\n");
		printf("  -l            strip leading white-space\n");
		printf("  -f            show in comment where tbl came from\n");
		printf("  -w            delete white-space not in quotes\n");
		printf("  -c            compress white-space to single space\n");
		printf("  -e            generate empty list if file does not exist\n");
		printf("  -p            make tbl-name a pointer\n");
	}
	else
	{
		fprintf(stderr, "Type \"%s -help\" for help\n", pgm);
	}
}

/*------------------------------------------------------------------------
 * strip()
 *
 * Strip trailing white space from a string & optionally
 * strip leading white-space.
 */
static char *
strip (char *str, int do_leading)
{
	char *	lastnsp;
	char *	s;

	if (do_leading)
	{
		for (; *str; str++)
		{
			if (! isspace(*str))
				break;
		}
	}

	lastnsp = str;
	for (s = str; *s; s++)
	{
		if (! isspace(*s))
			lastnsp = s + 1;
	}
	*lastnsp = 0;

	return (str);
}

/*------------------------------------------------------------------------
 * output_line()
 *
 *	Given a string, write it in C string format.  The string is
 *	wrapped in double quotes, and things requiring quotation are
 *	properly escaped.
 *
 * This routine also deals with the deleting of white-space.
 */
static void
output_line (char *buf, const char *str)
{
	char *	b			= buf;
	int		in_quotes	= FALSE;
	int		prev_ws		= FALSE;

	*b++ = '"';

	for (; *str; str++)
	{
		switch (*str)
		{
		case '\r':			/* carriage return	*/
		case '\n':			/* newline			*/
			/* always deleted */
			break;

		case '\f':			/* formfeed		*/
			if (in_quotes)
			{
				*b++ = '\\';
				*b++ = 'f';
			}
			else if (compress_ws && prev_ws)
			{
			}
			else if (! delete_ws)
			{
				if (compress_ws)
				{
					*b++ = ' ';
				}
				else
				{
					*b++ = '\\';
					*b++ = 'f';
				}
				prev_ws = TRUE;
			}
			break;

		case '\t':			/* tab			*/
			if (in_quotes)
			{
				*b++ = '\\';
				*b++ = 't';
			}
			else if (compress_ws && prev_ws)
			{
			}
			else if ((expand_tabs == 0 && ! delete_ws))
			{
				if (compress_ws)
				{
					*b++ = ' ';
				}
				else
				{
					*b++ = '\\';
					*b++ = 't';
				}
				prev_ws = TRUE;
			}
			else if (! delete_ws)
			{
				*b++ = ' ';

				while (((b - buf - 1) % expand_tabs) != 0 && ! compress_ws)
					*b++ = ' ';
				prev_ws = TRUE;
			}
			break;

		case ' ':			/* space			*/
			if (in_quotes)
			{
				*b++ = ' ';
			}
			else if (compress_ws && prev_ws)
			{
			}
			else if (! delete_ws)
			{
				*b++ = ' ';
				prev_ws = TRUE;
			}
			break;

		case '"':			/* double quote		*/
			in_quotes = ! in_quotes;
			/*FALLTHROUGH*/

		case '\\':			/* backslash		*/
			*b++ = '\\';
			*b++ = *str;
			prev_ws = FALSE;
			break;

		default:
			*b++ = *str;
			prev_ws = FALSE;
			break;
		}
	}

	*b++ = '"';
	*b++ = ',';
	*b   = 0;
}

/*------------------------------------------------------------------------
 * fix_line()
 *
 *	This is our function for line processing.
 *	It will optionally delete comments & lines which are empty.
 */
static char *
fix_line (char *str)
{
	if (comment_chars != 0)
	{
		char *s;

		s = strpbrk(str, comment_chars);
		if (s != 0)
		{
			*s = 0;
			strip(str, FALSE);
		}

		if (*str == 0)
			str = 0;
	}

	return (str);
}

/*------------------------------------------------------------------------
 * textify_lines()
 *
 *	do the actual work
 */
static int
textify_lines (FILE *fp_out, FILE *fp_inp)
{
	int linecnt = 0;

	/*----------------------------------------------------------------
	 * fetch lines from the input place and write them if needed.
	 */
	while (TRUE)
	{
		char	inpbuf[256];
		char	outbuf[256];
		char *	linep;

		if (fgets(inpbuf, sizeof(inpbuf), fp_inp) == 0)
			break;

		linep = strip(inpbuf, strip_leading);

		linep = fix_line(linep);
		if (linep != 0)
		{
			output_line(outbuf, linep);
			fprintf(fp_out, "\t%s\n", outbuf);
			linecnt++;
		}
	}

	return (linecnt);
}

/*------------------------------------------------------------------------
 * textify()
 *
 * textify one file
 */
static int
textify (FILE *fp_out, const char *filename)
{
	FILE *			fp_inp;
	const char *	from_name;
	const char *	tbl_name;
	char			tbl_name_buf[128];
	int				linecnt = 0;

	/*--------------------------------------------------------------------
	 * open input file if needed
	 */
	if (*filename == '-')
	{
		from_name = "<stdin>";
		fp_inp = stdin;
	}
	else
	{
		from_name = filename;
		fp_inp = fopen(filename, "r");
		if (fp_inp == 0)
		{
			if (! empty_ok)
			{
				fprintf(stderr, "%s: cannot open input file %s\n",
					pgm, filename);
				return (-1);
			}

			show_from = FALSE;
		}
	}

	/*----------------------------------------------------------------
	 * file header
	 */
	if (filecnt == 0)
	{
		fprintf(fp_out,
			"/* ALL EDITS WILL BE LOST - This is a generated file. */\n");
	}
	fprintf(fp_out, "\n");

	/*----------------------------------------------------------------
	 * generate comment if requested
	 */
	if (show_from || notes != 0)
	{
		fprintf(fp_out,
"/*------------------------------------------------------------------------\n");

		if (notes != 0)
		{
			const char *	n;
			int				start = TRUE;

			for (n=notes; *n; n++)
			{
				if (start)
				{
					fputs(" * ", fp_out);
					start = FALSE;
				}

				if (*n == '\n')
					start = TRUE;

				fputc(*n, fp_out);
			}
			fputc('\n', fp_out);
		}

		if (notes != 0 && show_from)
			fprintf(fp_out, " *\n");
		if (show_from)
			fprintf(fp_out, " * Table source: %s\n", from_name);

		fprintf(fp_out, " */\n");
	}

	/*----------------------------------------------------------------
	 * Generate the table preamble.  It contains the table name and
	 * optional /static/ and /const/ storage class or type qualifiers.
	 */
	tbl_name = Table_name;
	if (files_this_tbl > 0)
	{
		sprintf(tbl_name_buf, "%s_%d", Table_name, files_this_tbl);
		tbl_name = tbl_name_buf;
	}
	files_this_tbl++;

	if (make_ptr)
	{
		if (fp_inp != 0)
		{
			fprintf(fp_out, "static const char *%s_tbl[] =\n", tbl_name);
			fprintf(fp_out, "{\n");
		}
	}
	else
	{
		fprintf(fp_out, "%sconst char *%s[] =\n",
	    	should_be_static ? "static " : "",
	    	tbl_name);
		fprintf(fp_out, "{\n");
	}

	/*--------------------------------------------------------------------
	 * now do it
	 */
	if (fp_inp != 0)
		linecnt = textify_lines(fp_out, fp_inp);

	/*--------------------------------------------------------------------
	 * generate final lines
	 */
	if (fp_inp != 0 || ! make_ptr)
	{
		if (linecnt > 0)
			fprintf(fp_out, "\n");

		fprintf(fp_out, "\t0\n");
		fprintf(fp_out, "};\n");
	}

	if (make_ptr)
	{
		if (fp_inp != 0)
		{
			fprintf(fp_out, "%sconst char **%s = %s_tbl;\n",
	    		should_be_static ? "static " : "",
	    		tbl_name,
				tbl_name);
		}
		else
		{
			fprintf(fp_out, "%sconst char **%s = 0;\n",
	    		should_be_static ? "static " : "",
				tbl_name);
		}
	}

	/*--------------------------------------------------------------------
	 * close input file if needed
	 */
	if (*filename != '-' && fp_inp != 0)
		fclose(fp_inp);

	return (0);
}

/*------------------------------------------------------------------------
 * main program
 */
int
main (int argc, char **argv)
{
	const char *	out_file	= "-";
	FILE *			fp_out		= stdout;
	int				rc			= 0;
	int				c;

	pgm = argv[0];

	/*--------------------------------------------------------------------
	 * special case for -help (which does not fit getopt standard)
	 */
	if (argc > 1)
	{
		if (strcmp(argv[1], "-?")     == 0 ||
		    strcmp(argv[1], "-help")  == 0 ||
		    strcmp(argv[1], "--help") == 0)
		{
			usage(1);
			return (EXIT_SUCCESS);
		}

		if (strcmp(argv[1], "-??")       == 0 ||
		    strcmp(argv[1], "-helpall")  == 0 ||
		    strcmp(argv[1], "--helpall") == 0)
		{
			usage(2);
			return (EXIT_SUCCESS);
		}
	}

	/*--------------------------------------------------------------------
	 * parse options
	 */
	while ((c = getopt(argc, argv, OPTS)) != EOF)
	{
		switch (c)
		{
		case 'l':		/* strip leading w-s		*/
			strip_leading = TRUE;
			break;

		case 's':		/* should be static			*/
			should_be_static = TRUE;
			break;

		case 'f':		/* show where tbl from		*/
			show_from = TRUE;
			break;

		case 'w':		/* delete white-space		*/
			delete_ws = TRUE;
			break;

		case 'c':		/* compress w-s to space	*/
			compress_ws = TRUE;
			break;

		case 'e':		/* empty list if no file	*/
			empty_ok = TRUE;
			break;

		case 'p':		/* make tbl name a pointer	*/
			make_ptr = TRUE;
			break;

		case 'n':		/* notes					*/
			notes = optarg;
			if (*notes == 0)
				notes = 0;
			break;

		case 'd':		/* delete comments			*/
			comment_chars = optarg;
			if (*comment_chars == 0)
				comment_chars = 0;
			break;

		case 't':		/* table name				*/
			Table_name = optarg;
			files_this_tbl = 0;
			break;

		case 'x':		/* expand tabs				*/
			expand_tabs = atoi(optarg);
			break;

		case 'o':		/* output file				*/
			if (*out_file != '-')
			{
				fclose(fp_out);
				fp_out = stdout;
			}
			out_file = optarg;
			if (*out_file != '-')
			{
				fp_out = fopen(out_file, "w");
				if (fp_out == 0)
				{
					fprintf(stderr, "%s: cannot open output file %s\n",
						pgm, out_file);
					return (EXIT_FAILURE);
				}
			}
			filecnt = 0;
			break;

		case 'i':		/* input file			*/
			rc = textify(fp_out, optarg);
			if (rc)
				goto done;
			filecnt++;
			break;

		default:
			usage(0);
			return (EXIT_FAILURE);
		}
	}

	/*--------------------------------------------------------------------
	 * now do any filenames on cmd line
	 */
	for (; optind < argc; optind++)
	{
		rc = textify(fp_out, argv[optind]);
		if (rc)
			break;
		filecnt++;
	}

	/*--------------------------------------------------------------------
	 * if no files have been processed yet, just do stdin
	 */
	if (rc == 0 && filecnt == 0)
		rc = textify(fp_out, "-");

	/*--------------------------------------------------------------------
	 * close output file if needed
	 */
done:
	if (*out_file != '-')
		fclose(fp_out);

	return (rc == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
