/*------------------------------------------------------------------------
 * "main" program for xvt terminal-emulator
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libxvt.h"

/*------------------------------------------------------------------------
 * This is needed if getopt() is not available and cmdline options are used.
 */
#if USE_CMDLINE_OPTS
#  include "sysopt.h"
#endif

/*------------------------------------------------------------------------
 * usage display
 */
static void xvt_main_usage (const char *pgm, int level)
{
	if (level == 0)
	{
		fprintf(stderr, "Type \"%s -help\" for help.\n", pgm);
	}
	else
	{
		const char *	all		= (level > 1 ? "all"      : "");
		const char *	verbose	= (level > 1 ? "verbose " : "");
		const char *	shell	= getenv("SHELL");

		if (shell == 0 || *shell == 0)
			shell = XVT_SHELL;

#if USE_CMDLINE_OPTS
		printf("usage: %s [options] [toolkit options] [cmd ...]\n", pgm);
		printf("options:\n");
		/* DISPLAY OPTIONS HERE */
#else
		printf("usage: %s [toolkit options] [cmd ...]\n", pgm);
#endif

		printf("\n");
		xvt_tk_help(0, stdout,
			level == 1 ? XVT_TK_HELP_SHORT : XVT_TK_HELP_LONG,
			XVT_TK_HELP_TASK);

		printf("\n");
		printf("If no cmd is specified, \"%s\" will be used.\n",
			shell);

		printf("\n");
		printf("Type \"%s -help%s toolkit\"    "
			"for a %slist of all toolkit options.\n",	pgm, all, verbose);
		printf("Type \"%s -help%s pointers\"   "
			"for a %slist of pointer types.\n",			pgm, all, verbose);
		printf("Type \"%s -help%s colors\"     "
			"for a %slist of supported color names.\n",	pgm, all, verbose);
		printf("Type \"%s -help%s resources\"  "
			"for a %slist of resource locations.\n",	pgm, all, verbose);

		if (level == 1)
		{
		printf("\n");
		printf("Type \"%s -helpall [topic]\" "
			"for a more verbose list.\n",				pgm);
		}
	}
}

/*------------------------------------------------------------------------
 * check for special args which don't fit getopt() standard
 *
 *		[-]-?	[-]-help		[ toolkit | pointers | colors | resources ]
 *		[-]-??	[-]-helpall		[ toolkit | pointers | colors | resources ]
 *				[-]-toolkit
 *				[-]-pointers
 *				[-]-colors
 *				[-]-resources
 * 				[-]-version
 *
 * returns:
 *		ARGS_FAILURE	exit with failure
 *		ARGS_SUCCESS	exit with success
 *		ARGS_CONTINUE	continue processing
 */
#define ARGS_FAILURE	-1
#define ARGS_SUCCESS	 1
#define ARGS_CONTINUE	 0

static int xvt_main_args (int argc, char **argv, const char *pgm)
{
	int rc;

	/*--------------------------------------------------------------------
	 * process "[-]-help [option]"
	 */
	if (strcmp(argv[1], "-?")  == 0 || strcmp(argv[1], "-help")  == 0 ||
	    strcmp(argv[1], "--?") == 0 || strcmp(argv[1], "--help") == 0)
	{
		rc = ARGS_SUCCESS;

		if (argc > 2)
		{
			if (strcmp(argv[2], "toolkit") == 0)
			{
				xvt_tk_help(0, stdout, XVT_TK_HELP_LONG, XVT_TK_HELP_TASK);
			}
			else
			if (strcmp(argv[2], "pointers") == 0)
			{
				xvt_dump_pointers(stdout, FALSE);
			}
			else
			if (strcmp(argv[2], "colors") == 0)
			{
				xvt_dump_colors(stdout, FALSE);
			}
			else
			if (strcmp(argv[2], "resources") == 0)
			{
				xvt_dump_resources(stdout, FALSE, 0);
			}
			else
			{
				fprintf(stderr, "%s: unknown help option \"%s\"\n",
					pgm, argv[2]);
				xvt_main_usage(pgm, 0);
				rc = ARGS_FAILURE;
			}
		}
		else
		{
			xvt_main_usage(pgm, 1);
		}

		return (rc);
	}

	/*--------------------------------------------------------------------
	 * process "[-]-helpall [option]"
	 */
	if (strcmp(argv[1], "-??")  == 0 || strcmp(argv[1], "-helpall")  == 0 ||
	    strcmp(argv[1], "--??") == 0 || strcmp(argv[1], "--helpall") == 0)
	{
		rc = ARGS_SUCCESS;

		if (argc > 2)
		{
			if (strcmp(argv[2], "toolkit") == 0)
			{
				xvt_tk_help(0, stdout, XVT_TK_HELP_DETAIL, XVT_TK_HELP_TASK);
			}
			else
			if (strcmp(argv[2], "pointers") == 0)
			{
				xvt_dump_pointers(stdout, TRUE);
			}
			else
			if (strcmp(argv[2], "colors") == 0)
			{
				xvt_dump_colors(stdout, TRUE);
			}
			else
			if (strcmp(argv[2], "resources") == 0)
			{
				xvt_dump_resources(stdout, TRUE, 0);
			}
			else
			{
				fprintf(stderr, "%s: unknown help option \"%s\"\n",
					pgm, argv[2]);
				xvt_main_usage(pgm, 0);
				rc = ARGS_FAILURE;
			}
		}
		else
		{
			xvt_main_usage(pgm, 2);
		}

		return (rc);
	}

	/*--------------------------------------------------------------------
	 * process "[-]-toolkit"
	 */
	if (strcmp(argv[1], "-toolkit")  == 0 ||
	    strcmp(argv[1], "--toolkit") == 0)
	{
		xvt_tk_help(0, stdout, XVT_TK_HELP_DETAIL, XVT_TK_HELP_TASK);
		return (ARGS_SUCCESS);
	}

	/*--------------------------------------------------------------------
	 * process "[-]-pointers"
	 */
	if (strcmp(argv[1], "-pointers")  == 0 ||
	    strcmp(argv[1], "--pointers") == 0)
	{
		xvt_dump_pointers(stdout, TRUE);
		return (ARGS_SUCCESS);
	}

	/*--------------------------------------------------------------------
	 * process "[-]-colors"
	 */
	if (strcmp(argv[1], "-colors")  == 0 ||
	    strcmp(argv[1], "--colors") == 0)
	{
		xvt_dump_colors(stdout, TRUE);
		return (ARGS_SUCCESS);
	}

	/*--------------------------------------------------------------------
	 * process "[-]-resources"
	 */
	if (strcmp(argv[1], "-resources")  == 0 ||
	    strcmp(argv[1], "--resources") == 0)
	{
		xvt_dump_resources(stdout, TRUE, 0);
		return (ARGS_SUCCESS);
	}

	/*--------------------------------------------------------------------
	 * process "[-]-version"
	 */
	if (strcmp(argv[1], "-version")  == 0 ||
	    strcmp(argv[1], "--version") == 0)
	{
		printf("%s: version %s\n", pgm, XVT_VERSION);
		return (ARGS_SUCCESS);
	}

	/*--------------------------------------------------------------------
	 * no special option - just continue on
	 */
	return (ARGS_CONTINUE);
}

/*------------------------------------------------------------------------
 * "main" program
 */
int xvt_main (int argc, char **argv)
{
	XVT_DATA *		xd;
	const char *	pgm;
	int				rc;

	/*--------------------------------------------------------------------
	 * Get program name (minus any directory part) for user messages.
	 */
	{
		const char *p = strrchr(argv[0], '/');
		pgm = (p == 0 ? argv[0] : p+1);
	}

	/*--------------------------------------------------------------------
	 * Check for -help & friends (which don't fit getopt() standard).
	 */
	if (argc > 1)
	{
		rc = xvt_main_args(argc, argv, pgm);
		switch (rc)
		{
		case ARGS_FAILURE:	return (XVT_ERR_EMULATOR);
		case ARGS_SUCCESS:	return (EXIT_SUCCESS);
		case ARGS_CONTINUE:	break;
		}
	}

	/*--------------------------------------------------------------------
	 * Setup xvt-data struct.
	 */
	xd = xvt_data_alloc();
	if (xd == 0)
	{
			fprintf(stderr, "%s: No memory\n", pgm);
			return (XVT_ERR_EMULATOR);
	}

	/*--------------------------------------------------------------------
	 * Extract toolkit options (which also don't fit getopt() standard).
	 */
	rc = xvt_tk_args(xd, argc, argv);

	/*--------------------------------------------------------------------
	 * Now process any resource files.
	 */
	if (rc == XVT_OK)
		rc = xvt_tk_resource(xd);

	/*--------------------------------------------------------------------
	 * Check if any errors so far.
	 */
	if (rc != XVT_OK)
	{
		fprintf(stderr, "%s: %s\n", pgm, xvt_data_get_error_msg(xd));
		xd = xvt_data_free(xd);
		return (XVT_ERR_EMULATOR);
	}

	/*--------------------------------------------------------------------
	 * Reset argc & argv to new vector with toolkit options removed.
	 */
	argc = xvt_data_get_argc(xd);
	argv = xvt_data_get_argv(xd);

	/*--------------------------------------------------------------------
	 * Process any cmd-line options.
	 *
	 * Note that we have no cmd-line options, but a user copying this
	 * code for his own use may.
	 *
	 * Also note that any cmd-line options processed here belong to this
	 * program, not to the command to be executed.  The options for
	 * the command to be executed will appear after the command name on
	 * the cmd-line.
	 */
#if USE_CMDLINE_OPTS
	{
		const char *	opts	= "?";		/* ADD OPTIONS HERE */
		int				c;

		while ((c = getopt(argc, argv, opts)) != EOF)
		{
			switch (c)
			{
			case EOF:		/* avoid compiler warnings */
				break;

			/* PROCESS OPTIONS HERE */

			default:
				xvt_main_usage(pgm, 0);
				xd = xvt_data_free(xd);
				return (XVT_ERR_EMULATOR);
			}
		}
	}

	argv += optind;
#else
	argv++;
#endif

	/*--------------------------------------------------------------------
	 * Finally, run the cmd.
	 */
	rc = xvt_task_run(xd, argv);
	if (rc != XVT_OK)
	{
		/*----------------------------------------------------------------
		 * Execution failed: display why & set return code.
		 */
		fprintf(stderr, "%s: %s\n", pgm, xvt_data_get_error_msg(xd));
		rc = XVT_ERR_EMULATOR;
	}
	else
	{
		/*----------------------------------------------------------------
		 * Execution succeeded: get sub-task's exit code.
		 *
		 * Note this doesn't mean the sub-task succeeded, it just
		 * means that we successfully were able to execute it.
		 */
		rc = xvt_data_get_exit_code(xd);
	}

	/*--------------------------------------------------------------------
	 * close out
	 */
	xd = xvt_data_free(xd);

	return (rc);
}
