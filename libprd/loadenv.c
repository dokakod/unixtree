/*------------------------------------------------------------------------
 * load program environment
 */
#include "libprd.h"

/*------------------------------------------------------------------------
 * load environment
 *
 * env vars we look for:
 *
 *	$<PGM>INIT			name of rc file to load
 *	$<PGM>HOME			home directory
 *	$<PGM>PATH			path to use
 *	$<PGM>TERM			TERM to use instead of $TERM
 *	$<PGM>EDITOR		editor to use
 *	$<PGM>HEXEDITOR		hex editor to use
 *	$<PGM>VIEWER		viewer to use
 *	$<PGM>PRINTER		printer to use
 *	$<PGM>NODE			default node to log
 *	$<PGM>LANG			language to use
 *	$<PGM>MAGIC			magic path
 */
void load_env (int argc, char **argv)
{
	char			prefix[12];
	char			varname[24];
	const char *	our_path;
	const char *	p;
	const char *	lang;
	time_t			t;

	/*--------------------------------------------------------------------
	 * initialize all globals
	 */
	init_globals(argc, argv);

	/*--------------------------------------------------------------------
	 * get program name (without extension)
	 */
	strcpy(gbl(pgm_name), fn_basename(argv[0]));
	fn_rem_ext(gbl(pgm_name));

	/*--------------------------------------------------------------------
	 * process system variables
	 */
	gbl(pgm_euid) = os_get_euid();			/* effective user  id */
	gbl(pgm_egid) = os_get_egid();			/* effective group id */

	fn_getcwd(gbl(pgm_cwd));				/* current working directory */

	our_path = getenv("PATH");				/* path */
	if (our_path == 0)
		our_path = fn_cwdname();

	gbl(pgm_tmp) = getenv("TMP");			/* tmp file directory */
	if (gbl(pgm_tmp) == 0)
		gbl(pgm_tmp) = fn_tmpname();

	t = time(0);							/* get gmt & dst */
	gbl(pgm_dst) = os_get_dst(t);
	gbl(pgm_gmt) = os_get_gmt();

	/*--------------------------------------------------------------------
	 * get directory program is in (as real, abs path)
	 */
	{
		char fullpath[MAX_PATHLEN];

		p = os_get_path(argv[0], our_path, fullpath);
		if (p != 0)
		{
			char real_path[MAX_PATHLEN];
			struct stat stbuf;
			int rc;

			/*------------------------------------------------------------
			 * check if this path is a symlink
			 */
			rc = os_lstat(p, &stbuf);
			if (rc == 0)
			{
				if (S_ISLNK(stbuf.st_mode))
				{
					rc = os_readlink(p, real_path, sizeof(real_path));
					if (rc > 0)
					{
						real_path[rc] = 0;
						if (! fn_is_path_absolute(real_path))
						{
							char temp_path[MAX_PATHLEN];

							fn_dirname(p, temp_path);
							fn_append_dirname_to_dir(temp_path, real_path);
							fn_cleanup_path(temp_path);
							strcpy(real_path, temp_path);
						}
						p = real_path;
					}
				}
			}

			fn_dirname(p, gbl(pgm_dir));
		}
		else
		{
			strcpy(gbl(pgm_dir), fn_cwdname());
		}

		if (strcmp(gbl(pgm_dir), fn_cwdname()) == 0)
			strcpy(gbl(pgm_dir), gbl(pgm_cwd));
	}

	/*--------------------------------------------------------------------
	 * get env-var prefix
	 */
	strcpy(prefix, pgmi(m_pgm_varprfx));

	/*--------------------------------------------------------------------
	 * load resource file
	 */
	load_res();

	/*--------------------------------------------------------------------
	 * $<PGM>LANG	language to use
	 */
	strcpy(varname, prefix);
	strcat(varname, "LANG");
	lang = getenv(varname);

	/*--------------------------------------------------------------------
	 * $<PGM>TERM	term to use ($TERM is not found)
	 */
	strcpy(varname, prefix);
	strcat(varname, "TERM");
	gbl(pgm_term) = getenv(varname);

	if (gbl(pgm_term) == 0)
		gbl(pgm_term) = getenv("TERM");

	/*--------------------------------------------------------------------
	 * get pgm_home (~/.<pgm_name>)
	 */
	strcpy(varname, prefix);
	strcat(varname, "HOME");
	p = getenv(varname);
	if (p != 0 && *p != 0)
	{
		/*----------------------------------------------------------------
		 * $<PGM>HOME found
		 */
		strcpy(gbl(pgm_home), p);
	}
	else
	{
		/*----------------------------------------------------------------
		 * $<PGM>HOME not found: default is "~/.<pgm>"
		 */
		char filename[MAX_FILELEN];

		strcpy(filename, ".");
		strcat(filename, pgmi(m_pgm_program));

		strcpy(gbl(pgm_home), fn_home());
		fn_append_filename_to_dir(gbl(pgm_home), filename);
	}
	fn_resolve_pathname(gbl(pgm_home));

	/*--------------------------------------------------------------------
	 * $<PGM>PATH	path to use
	 */
	strcpy(varname, prefix);
	strcat(varname, "PATH");
	p = getenv(varname);
	if (p != 0 && *p != 0)
	{
		/*----------------------------------------------------------------
		 * $<PGM>PATH found
		 */
		strcpy(gbl(pgm_path), p);
	}
	else
	{
		/*----------------------------------------------------------------
		 * $<PGM>PATH not found: default is ".:<pgm-home>:<pgm-dir>"
		 */
		strcpy(gbl(pgm_path), fn_cwdname());
		strcat(gbl(pgm_path), fn_path_separator());
		strcat(gbl(pgm_path), gbl(pgm_home));
		strcat(gbl(pgm_path), fn_path_separator());
		strcat(gbl(pgm_path), gbl(pgm_dir));
	}

	/*--------------------------------------------------------------------
	 * setup default magic path
	 */
	{
		char mh_path[MAX_PATHLEN];
		char ms_path[MAX_PATHLEN];

		strcpy(mh_path, gbl(pgm_home));
		fn_append_filename_to_dir(mh_path, "magic");

		strcpy(ms_path, gbl(pgm_dir));
		fn_append_filename_to_dir(ms_path, "magic");

		strcpy(gbl(pgm_magic), mh_path);
		strcat(gbl(pgm_magic), fn_path_separator());
		strcat(gbl(pgm_magic), ms_path);
		strcat(gbl(pgm_magic), fn_path_separator());
		strcat(gbl(pgm_magic), "/etc/magic");
	}

	/*--------------------------------------------------------------------
	 * setup default values for all options
	 */
	setup_defaults(lang);

	gbl(pgm_dflts_setup) = TRUE;

	xvt_data_set_appname(gbl(xvt_data), package_name());

	/*--------------------------------------------------------------------
	 * process any configuration files
	 */
	{
		/*----------------------------------------------------------------
		 * $<PGM>INIT		init file to use
		 */
		strcpy(varname, prefix);
		strcat(varname, "INIT");
		p = getenv(varname);

		/*----------------------------------------------------------------
		 * load init file(s)
		 */
		check_defaults(p);
	}

	/*--------------------------------------------------------------------
	 * process any environment variables
	 */
	{
		/*----------------------------------------------------------------
		 * $<PGM>EDITOR		editor to use
		 */
		strcpy(varname, prefix);
		strcat(varname, "EDITOR");
		p = getenv(varname);
		if (p != 0 && *p != 0)
			strcpy(opt(editor), p);

		/*----------------------------------------------------------------
		 * $<PGM>HEXEDITOR	hex-editor to use
		 */
		strcpy(varname, prefix);
		strcat(varname, "HEXEDITOR");
		p = getenv(varname);
		if (p != 0 && *p != 0)
			strcpy(opt(hex_editor), p);

		/*----------------------------------------------------------------
		 * $<PGM>VIEWER		viewer to use
		 */
		strcpy(varname, prefix);
		strcat(varname, "VIEWER");
		p = getenv(varname);
		if (p != 0 && *p != 0)
			strcpy(opt(viewer), p);

		/*----------------------------------------------------------------
		 * $<PGM>DIFF		diff-rtn to use
		 */
		strcpy(varname, prefix);
		strcat(varname, "DIFF");
		p = getenv(varname);
		if (p != 0 && *p != 0)
			strcpy(opt(diff_rtn), p);

		/*----------------------------------------------------------------
		 * $<PGM>PRINTER	print-spooler to use
		 */
		strcpy(varname, prefix);
		strcat(varname, "PRINTER");
		p = getenv(varname);
		if (p != 0 && *p != 0)
			strcpy(opt(printer), p);

		/*----------------------------------------------------------------
		 * $<PGM>NODE		node to log
		 */
		strcpy(varname, prefix);
		strcat(varname, "NODE");
		p = getenv(varname);
		if (p != 0 && *p != 0)
			strcpy(opt(default_node), p);

		/*----------------------------------------------------------------
		 * $<PGM>MAGIC		magic path
		 */
		strcpy(varname, prefix);
		strcat(varname, "MAGIC");
		p = getenv(varname);
		if (p != 0 && *p != 0)
			strcpy(gbl(pgm_magic), p);
	}

	/*--------------------------------------------------------------------
	 * load entry histories
	 */
	xg_tbl_load();
}
