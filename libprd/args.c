/*------------------------------------------------------------------------
 * process cmd-line options & args
 */
#include "libprd.h"

#define OPTIONS															\
				/* normal (user) options							*/	\
																		\
				"a"						/* Enter app menu			*/	\
				"c"						/* use alt-char sets		*/	\
				"k"						/* resolve symlinks			*/	\
				"m"						/* continue at mount point	*/	\
				"n"						/* log NFS nodes			*/	\
				"r"						/* read-only mode			*/	\
				"V"						/* display version			*/	\
				"v"						/* display version			*/	\
				"w"						/* run in a window			*/	\
				"x"						/* use checkpoint file		*/	\
																		\
				"e"	OPT_ARG_XPTD_S		/* exclude directory		*/	\
				"l"	OPT_ARG_XPTD_S		/* logging method			*/	\
				"t"	OPT_ARG_XPTD_S		/* term type				*/	\
																		\
				/* debugging (helpall) options						*/	\
																		\
				"D"						/* debug mode				*/	\
				"K"						/* show key-strokes			*/	\
				"M"						/* read mount table			*/	\
																		\
				"F"	OPT_ARG_XPTD_S		/* checkpoint file			*/	\
				"H"	OPT_ARG_XPTD_S		/* headline delay			*/	\
				"I"	OPT_ARG_XPTD_S		/* input  filename			*/	\
				"O"	OPT_ARG_XPTD_S		/* output filename			*/	\
				"S"	OPT_ARG_XPTD_S		/* screen type				*/	\
																		\
				"T"	OPT_ARG_XPTD_S		/* terminal DB type			*/	\
				"X"	OPT_ARG_XPTD_S		/* specify xvt option		*/	\
				"Z"	OPT_ARG_XPTD_S		/* special debug options	*/	\
				""

/*------------------------------------------------------------------------
 * process argv options
 *
 * returns:
 *		< 0		exit with error
 *		  0		OK
 *		> 0		exit with no error
 */
int check_opts (void)
{
	char	msgbuf[128];
	char *	p;
	char **	argv	= gbl(pgm_argv);
	int		argc	= gbl(pgm_argc);
	int		c;
	int		o;
	int		rc;

	/*--------------------------------------------------------------------
	 * special case for help displays
	 */
	if (argc > 1)
	{
		if (strcmp(argv[1], "-?")     == 0 ||
		    strcmp(argv[1], "-help")  == 0 ||
		    strcmp(argv[1], "--help") == 0)
		{
			if (argc > 2)
			{
				if (strcmp(argv[2], "toolkit") == 0)
				{
					xvt_tk_help(gbl(xvt_data), stdout, XVT_TK_HELP_LONG,
						XVT_TK_HELP_HERE);
					return (1);
				}
			}

			usage(1, gbl(pgm_name));
			return (1);
		}

		if (strcmp(argv[1], "-??")       == 0 ||
		    strcmp(argv[1], "-helpall")  == 0 ||
		    strcmp(argv[1], "--helpall") == 0)
		{
			if (argc > 2)
			{
				if (strcmp(argv[2], "toolkit") == 0)
				{
					xvt_tk_help(gbl(xvt_data), stdout, XVT_TK_HELP_DETAIL,
						XVT_TK_HELP_HERE);
					return (1);
				}
			}

			usage(2, gbl(pgm_name));
			return (1);
		}

		if (strcmp(argv[1], "-version")  == 0 ||
		    strcmp(argv[1], "--version") == 0)
		{
			printf("%s %s\n", package_name(), ver_line(msgbuf));
			return (1);
		}
	}

	/*--------------------------------------------------------------------
	 * collect all options
	 */
	while (TRUE)
	{
		c = getopt_ext(argc, argv, OPTIONS, "-+");
		if (c == OPT_ERR_EOF)
			break;

		o = (optsw == '-' ? OFF : ON);

		switch (c)
		{
		/*----------------------------------------------------------------
		 * normal options
		 */
		case 'v':
		case 'V':
			/*------------------------------------------------------------
			 * display version & exit
			 */
			printf("%s %s\n", package_name(), ver_line(msgbuf));
			return (1);

		case 'a':
			/*------------------------------------------------------------
			 * enter application menu
			 */
			opt(enter_app_menu)			= o;
			break;

		case 'c':
			/*------------------------------------------------------------
			 * use alt-char-sets
			 */
			opt(use_alt_char_sets)		= o;
			break;

		case 'k':
			/*------------------------------------------------------------
			 * automatically resolve symlinks
			 */
			opt(resolve_sym_links)		= o;
			break;

		case 'm':
			/*------------------------------------------------------------
			 * continue at mount points
			 */
			opt(cont_at_mount_points)	= o;
			break;

		case 'n':
			/*------------------------------------------------------------
			 * log network file-systems
			 */
			opt(log_network_files)		= o;
			break;

		case 'r':
			/*------------------------------------------------------------
			 * read-only
			 */
			opt(read_only)				= o;
			break;

		case 'w':
			/*------------------------------------------------------------
			 * run in a window
			 */
			opt(run_in_window)			= o;
			break;

		case 'x':
			/*------------------------------------------------------------
			 * use checkpoint file
			 */
			cmdopt(use_ckp_file)		= o;
			break;

		case 'e':
			/*------------------------------------------------------------
			 * specify directory to exclude from auto-logging
			 */
			cmdopt(exclude_list)		= bappend(cmdopt(exclude_list),
											BNEW(optarg));
			break;

		case 'l':
			/*------------------------------------------------------------
			 * specify logging method
			 */
			switch (*optarg)
			{
			case 'a':
				opt(logging_method)		= log_auto;
				break;

			case 'm':
				opt(logging_method)		= log_manual;
				break;

			default:
				sprintf(msgbuf, msgs(m_args_invlog), optarg);
				fprintf(stderr, "%s: %s\n", gbl(pgm_name), msgbuf);
				return (-1);
			}
			break;

		case 't':
			/*------------------------------------------------------------
			 * specify term type
			 */
			gbl(pgm_term)				= optarg;
			break;

		/*----------------------------------------------------------------
		 * debug (helpall) options
		 */
		case 'D':
			/*------------------------------------------------------------
			 * debug hack - disable clock display
			 */
			opt(display_clock)			= o;
			break;

		case 'K':
			/*------------------------------------------------------------
			 * display key-strokes at bottom of screen
			 */
			cmdopt(debug_kbd)			= o;
			break;

		case 'M':
			/*------------------------------------------------------------
			 * read mount table
			 */
			cmdopt(skip_mnt_table)		= ! o;
			break;

		case 'F':
			/*------------------------------------------------------------
			 * specify checkpoint file
			 */
			cmdopt(ckp_path)			= optarg;
			break;

		case 'H':
			/*------------------------------------------------------------
			 * specify headline delay
			 */
			cmdopt(headline_delay)		= atoi(optarg);
			break;

		case 'I':
			/*------------------------------------------------------------
			 * specify input path
			 */
			cmdopt(trm_inp)				= optarg;
			break;

		case 'O':
			/*------------------------------------------------------------
			 * specify output path
			 */
			cmdopt(trm_out)				= optarg;
			break;

		case 'S':
			/*------------------------------------------------------------
			 * specify screen mode
			 */
			switch (*optarg)
			{
			case 'a':
				opt(screen_type)		= screen_auto;
				break;

			case 'c':
				opt(screen_type)		= screen_color;
				break;

			case 'm':
				opt(screen_type)		= screen_mono;
				break;

			default:
				sprintf(msgbuf, msgs(m_args_invscrn), optarg);
				fprintf(stderr, "%s: %s\n", gbl(pgm_name), msgbuf);
				return (-1);
			}
			break;

		/*----------------------------------------------------------------
		 * other options
		 */
		case 'T':
			/*------------------------------------------------------------
			 * specify terminal database type
			 */
			rc = set_term_db(optarg);
			if (rc)
			{
				sprintf(msgbuf, msgs(m_main_invdb), optarg);
				fprintf(stderr, "%s: %s\n", gbl(pgm_name), msgbuf);
				return (-1);
			}
			break;

		case 'X':
			/*------------------------------------------------------------
			 * xvt arg
			 */
			rc = win_arg(optarg);
			if (rc)
			{
				switch (rc)
				{
				case XVT_ERR_INVOPT:
					sprintf(msgbuf, msgs(m_args_badxvtopt), optarg);
					break;

				case XVT_ERR_INVARG:
					sprintf(msgbuf, msgs(m_args_badxvtarg), optarg+1,
						*optarg);
					break;
				}
				fprintf(stderr, "%s: %s\n", gbl(pgm_name), msgbuf);
				return (-1);
			}
			break;

		case 'Z':
			/*------------------------------------------------------------
			 * specify debug modes
			 */
			for (p=optarg; *p; p++)
			{
				switch (*p)
				{
				case 'C':
					/*----------------------------------------------------
					 * print current config options & exit
					 */
					rc = disp_defaults(FALSE);
					return (rc == 0 ? 1 : -1);

				case 'D':
					/*----------------------------------------------------
					 * print default config options & exit
					 */
					rc = disp_defaults(TRUE);
					return (rc == 0 ? 1 : -1);

				case 'F':
					/*----------------------------------------------------
					 * debug file-color file & exit
					 */
					cmdopt(debug_fcfile)		= TRUE;
					break;

				case 'M':
					/*----------------------------------------------------
					 * debug memory
					 */
					cmdopt(debug_malloc)++;
					break;

				case 'k':
					/*----------------------------------------------------
					 * log key-strokes to text file
					 */
					cmdopt(debug_keys)			= TRUE;
					cmdopt(debug_keys_text)		= TRUE;
					break;

				case 'K':
					/*----------------------------------------------------
					 * log key-strokes to binary file
					 */
					cmdopt(debug_keys)			= TRUE;
					cmdopt(debug_keys_text)		= FALSE;
					break;

				case 's':
					/*----------------------------------------------------
					 * log screen output to text file
					 */
					cmdopt(debug_scrn)			= TRUE;
					cmdopt(debug_scrn_text)		= TRUE;
					break;

				case 'S':
					/*----------------------------------------------------
					 * log screen output to binary file
					 */
					cmdopt(debug_scrn)			= TRUE;
					cmdopt(debug_scrn_text)		= FALSE;
					break;

				case 'V':
					/*----------------------------------------------------
					 * display version info & builder info
					 */
					printf("%s %s\n", package_name(), ver_line(msgbuf));
					printf("%s %s\n", msgs(m_credits_bldr_nam),
						pgmi(m_pgm_bldr_nam));
					printf("%s %s\n", msgs(m_credits_bldr_ema),
						pgmi(m_pgm_bldr_ema));
					printf("%s %s\n", msgs(m_credits_bldr_env),
						pgmi(m_pgm_bldr_env));
					return (1);
				}
			}
			break;

		/*----------------------------------------------------------------
		 * invalid options
		 */
		case OPT_ERR_ARG:
			/*------------------------------------------------------------
			 * ERROR: option requires an argument
			 */
			sprintf(msgbuf, msgs(m_main_ora), optopt);
			fprintf(stderr, "%s: %s\n", gbl(pgm_name), msgbuf);
			return (-1);

		case OPT_ERR_INV:
		default:
			/*------------------------------------------------------------
			 * ERROR: invalid option
			 */
			sprintf(msgbuf, msgs(m_main_illopt), optopt);
			fprintf(stderr, "%s: %s\n", gbl(pgm_name), msgbuf);
			return (-1);
		}
	}

	/*--------------------------------------------------------------------
	 * check if max of one arg left
	 */
	if ((argc - optind) > 1)
	{
		fprintf(stderr, "%s: %s\n", gbl(pgm_name), msgs(m_main_tma));
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * load file color entries
	 */
	rc = 0;
	fa_list_load(cmdopt(debug_fcfile) ? &rc : 0);
	if (rc)
		return (-1);

	if (cmdopt(debug_fcfile))
	{
		fa_list_dump();
		fa_list_free();
		return (1);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * check argv for a valid node to log
 */
int check_args (char *node_to_log)
{
	char **	argv	= gbl(pgm_argv);
	int		argc	= gbl(pgm_argc);
	char *	p;
	int		i;

	/*--------------------------------------------------------------------
	 * check if node specified on cmd line
	 */
	if (optind != argc)
	{
		/*----------------------------------------------------------------
		 * yes, use it & turn off checkpoint file processing
		 */
		strcpy(node_to_log, argv[optind]);
		cmdopt(use_ckp_file) = FALSE;
	}
	else
	{
		/*----------------------------------------------------------------
		 * no, use default node
		 */
		strcpy(node_to_log, opt(default_node));
	}

	/*--------------------------------------------------------------------
	 * resolve node-to-log
	 */
	fn_resolve_pathname(node_to_log);

	/*--------------------------------------------------------------------
	 * check if node-to-log exists & is a directory
	 */
	p = node_to_log;
	i = is_it_a_dir(p, 0);
	if (i)
	{
		char msgbuf[128];

		if (i > 0)
			sprintf(msgbuf, msgs(m_main_ind), p);
		else
			sprintf(msgbuf, msgs(m_main_dne), p);

		fprintf(stderr, "%s: %s\n", gbl(pgm_name), msgbuf);

		return (-1);
	}

	return (0);
}
