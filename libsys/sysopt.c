/*------------------------------------------------------------------------
 *  Routines to return options from a command line.
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "sysopt.h"

/*------------------------------------------------------------------------
 * global sysopt data
 */
char *		sysopt_optarg		= 0;
int			sysopt_optind		= 0;
int			sysopt_optopt		= 0;
int			sysopt_optsw		= 0;
int			sysopt_opterr		= 1;

/*------------------------------------------------------------------------
 * internal sysopt data
 */
static char	sysopt_optmsg[128]	= "";
static int	sysopt_optpos		= 0;

/*------------------------------------------------------------------------
 * sysopt_reset() - reset argv pointer
 */
void sysopt_reset (void)
{
	sysopt_optind = 0;
	sysopt_optpos = 0;
}

/*------------------------------------------------------------------------
 * sysopt_getopt() - getopt() replacement
 */
int sysopt_getopt (int argc, char * const *argv, const char *optstr)
{
	int	rc;

	rc = sysopt_getopt_ext(argc, argv, optstr, "-");
	switch (rc)
	{
	case OPT_ERR_INV:
	case OPT_ERR_ARG:
		if (sysopt_opterr)
		{
			fprintf(stderr, "%s: %s\n", argv[0], sysopt_optmsg);
		}
		break;
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * sysopt_getopt_ext() - extended option processing
 */
int sysopt_getopt_ext (int argc, char * const *argv, const char *optstr,
	const char *switches)
{
	const char *op;

	/*--------------------------------------------------------------------
	 * initialize if first time called
	 */
	if (sysopt_optind == 0)
	{
		sysopt_optind = 1;
		sysopt_optpos = 0;
	}

	/*--------------------------------------------------------------------
	 * clear entries
	 */
	sysopt_optarg		= 0;
	sysopt_optmsg[0]	= 0;

	/*--------------------------------------------------------------------
	 * if past all arguments, done
	 */
	if (sysopt_optind >= argc)
		return (OPT_ERR_EOF);

	/*--------------------------------------------------------------------
	 * check if new argument
	 */
	if (sysopt_optpos == 0)
	{
		/*----------------------------------------------------------------
		 * check if option & longer than 1 char
		 */
		op = strchr(switches, argv[sysopt_optind][0]);
		if (op != 0 && argv[sysopt_optind][1] != 0)
		{
			sysopt_optpos = 1;
			sysopt_optsw = argv[sysopt_optind][0];
		}
		else
		{
			return (OPT_ERR_EOF);
		}
	}

	/*--------------------------------------------------------------------
	 * get next option & bump ptr
	 */
	sysopt_optopt = argv[sysopt_optind][sysopt_optpos++];

	/*--------------------------------------------------------------------
	 * check for double option chars
	 */
	if (sysopt_optopt == sysopt_optsw)
	{
		sysopt_optpos = 0;
		sysopt_optind++;
		return (OPT_ERR_EOF);
	}

	/*--------------------------------------------------------------------
	 * check for number
	 */
	if (isdigit(sysopt_optopt))
	{
		for (op=optstr; *op; op++)
		{
			if (*op == OPT_ARG_NUM_C)
			{
				sysopt_optopt = *op;
				sysopt_optarg = &argv[sysopt_optind][sysopt_optpos-1];
				sysopt_optpos = 0;
				sysopt_optind++;
				return (sysopt_optopt);
			}
		}
	}

	/*--------------------------------------------------------------------
	 * check if at end of string of options
	 */
	if (argv[sysopt_optind][sysopt_optpos] == 0)
	{
		sysopt_optpos = 0;
		sysopt_optind++;
	}

	/*--------------------------------------------------------------------
	 * search for next option
	 */
	for (op=optstr; *op; op++)
	{
		if (*op == OPT_ARG_XPTD_C)		/* skip over colons */
			continue;

		if (sysopt_optopt != *op)
			continue;

		/*----------------------------------------------------------------
		 * got a matching letter in string - check if arg expected
		 */
		if (*(op+1) != OPT_ARG_XPTD_C)
			return (sysopt_optopt);

		/*----------------------------------------------------------------
		 * process option argument
		 */
		if (sysopt_optind == argc)
		{
			/*------------------------------------------------------------
			 * at end-of-list
			 */
			sprintf(sysopt_optmsg, "option requires an argument -- %c",
				sysopt_optopt);
			return (OPT_ERR_ARG);
		}

		sysopt_optarg = &argv[sysopt_optind][sysopt_optpos];
		sysopt_optpos = 0;
		sysopt_optind++;
		return (sysopt_optopt);
	}

	/*--------------------------------------------------------------------
	 * if we get here, we found an invalid option
	 */
	sprintf(sysopt_optmsg, "invalid option -- %c", sysopt_optopt);
	return (OPT_ERR_INV);
}

/*------------------------------------------------------------------------
 * sysopt_optget_msg() - get error message from last call
 */
char * sysopt_optget_msg (void)
{
	return (sysopt_optmsg);
}
