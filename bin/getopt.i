/*------------------------------------------------------------------------
 * getopt stuff
 */
#ifndef GETOPT_I
#define GETOPT_I

#if _WIN32 || MSC_VER
/*------------------------------------------------------------------------
 * getopt replacement (for Windoze)
 */
int		optind	= 1;		/* index into parent argv vector	*/
int		optopt	= 0;		/* character checked for validity	*/
int		opterr	= 1;		/* set to 0 for no error msgs		*/
char *	optarg;				/* argument associated with option	*/

#define BADCH	'?'

int getopt (int argc, char * const *argv, const char *opts)
{
	static char	*	place = "";		/* option letter processing */
	char *			oli;			/* option letter list index */

	/*--------------------------------------------------------------------
	 * update scanning pointer
	 */
	if (! *place)
	{
		/*----------------------------------------------------------------
		 * check for end of arg list or arg with no leading "-"
		 */
		if (optind >= argc ||
		    *(place = argv[optind]) != '-' ||
		   *++place == 0)
		{
			return (EOF);
		}

		/*----------------------------------------------------------------
		 * check for "--"
		 */
		if (*place == '-')
		{
			++optind;
			return (EOF);
		}
	}

	/*--------------------------------------------------------------------
	 * got option letter - is it OK?
	 */
	optopt = *place++;
	if (optopt == ':' || (oli = strchr(opts, optopt)) == 0)
	{
		if (*place == 0)
			++optind;
		if (opterr)
		{
			fprintf(stderr, "%s: illegal option -- %c\n", argv[0], optopt);
		}
		return (BADCH);
	}

	/*--------------------------------------------------------------------
	 * check if argument needed
	 */
	if (*++oli != ':')
	{
		optarg = 0;
		if (*place == 0)
			optind++;
	}
	else
	{
		if (*place != 0)
		{
			/*------------------------------------------------------------
			 * no white space
			 */
			optarg = place;
		}
		else if (argc <= ++optind)
		{
			/*------------------------------------------------------------
			 * no arg
			 */
			place = "";
			if (opterr)
			{
				fprintf(stderr, "%s: option requires an argument -- %c\n",
					argv[0], optopt);
			}
			return (BADCH);
		}
	 	else
		{
			/*------------------------------------------------------------
			 * white space
			 */
			optarg = argv[optind];
		}

		place = "";
		optind++;
	}

	/*--------------------------------------------------------------------
	 * dump back option letter
	 */
	return (optopt);
}

#else /* windows */

/*------------------------------------------------------------------------
 * some stdlibs (linux) don't have these entries ...
 */
extern int		optind;
extern int		optopt;
extern int		opterr;
extern char *	optarg;

#endif

#endif /* GETOPT_I */
