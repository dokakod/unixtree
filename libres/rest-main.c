/*------------------------------------------------------------------------
 * program to compile a resource file
 */
#include "restool.h"

/*------------------------------------------------------------------------
 * usage msg
 */
static void usage (FILE *fp, const char *pgm_name)
{
	fprintf(fp, "usage: %s [options] name\n", pgm_name);
	fprintf(fp, "options:\n");
	fprintf(fp, "  -c           compile text files into \"<name>.res\"\n");
	fprintf(fp, "  -e           extract text files from \"<name>.res\"\n");
	fprintf(fp, "  -h           dump  headers from \"<name>.res\"\n");
	fprintf(fp, "  -p sect[:n]  print section from \"<name>.res\"\n");
	fprintf(fp, "  -d orig      diff \"<name>.res\" against \"<orig>.res\"\n");
}

/*------------------------------------------------------------------------
 * remove an extension from a pathname
 */
static void remove_ext (char *path)
{
	char *	p;

	/*--------------------------------------------------------------------
	 * bail if empty string
	 */
	if (*path == 0)
		return;

	/*--------------------------------------------------------------------
	 * look for final part of name (basename)
	 */
	while (TRUE)
	{
		p = strpbrk(path, "/\\");
		if (p == 0)
			break;
		path = p+1;
	}

	/*--------------------------------------------------------------------
	 * now look for an extension
	 */
	p = strrchr(path, '.');
	if (p != 0)
		*p = 0;
}

/*------------------------------------------------------------------------
 * main program
 */
int main (int argc, char **argv)
{
	char				msgbuf[256];
	char				name[256];
	char				orig[256];
	const char *		pgm_name;
	const char *		section;
	int					action;
	int					c;
	int					rc;

	/*--------------------------------------------------------------------
	 * process options
	 */
	pgm_name = argv[0];

	*orig	= 0;
	action	= 0;
	section	= 0;

	while ((c = getopt(argc, argv, "cehp:d:?")) != OPT_ERR_EOF)
	{
		switch (c)
		{
		case 'c':
		case 'e':
		case 'h':
			action = c;
			break;

		case 'p':
			action = c;
			section = optarg;
			break;

		case 'd':
			action = c;
			strcpy(orig, optarg);
			break;

		case '?':
			usage(stdout, pgm_name);
			return (0);

		default:
			usage(stderr, pgm_name);
			return (1);
		}
	}

	/*--------------------------------------------------------------------
	 * get language name
	 */
	if (optind >= argc)
	{
		fprintf(stderr, "No name specified\n");
		usage(stderr, pgm_name);
		return (1);
	}

	strcpy(name, argv[optind++]);

	/*--------------------------------------------------------------------
	 * remove extensions if present
	 */
	remove_ext(name);
	remove_ext(orig);

	/*--------------------------------------------------------------------
	 * check if action specified
	 */
	if (action == 0)
	{
		fprintf(stderr, "No action specified\n");
		usage(stderr, pgm_name);
		return (1);
	}

	/*--------------------------------------------------------------------
	 * now do it
	 */
	switch (action)
	{
	case 'c':	rc = res_compile (name, msgbuf);			break;
	case 'e':	rc = res_extract (name, msgbuf);			break;
	case 'h':	rc = res_headers (name, msgbuf);			break;
	case 'p':	rc = res_print   (name, section, msgbuf);	break;
	case 'd':	rc = res_diff    (name, orig, msgbuf);		break;
	}

	if (rc)
	{
		fprintf(stderr, "%s: %s\n", pgm_name, msgbuf);
	}

	return (rc == 0 ? 0 : 1);
}
