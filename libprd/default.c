/*------------------------------------------------------------------------
 * process defaults file
 */
#include "libprd.h"

void setup_defaults (const char *lang)
{
	int old_lang = get_cur_lang();
	int cur_lang;
	int	in_window;

	/*--------------------------------------------------------------------
	 * initialize all default values from current resource file
	 */
	dflt_init_defs(dflt_tbl, FALSE);

	/*--------------------------------------------------------------------
	 * adjust some defaults depending on our environment
	 */
	{
		const DEFS *	ds;
		struct stat		stbuf;

		/*----------------------------------------------------------------
		 * setup default language if specified
		 */
		if (lang != 0 && *lang != 0)
		{
			if (select_lang_by_name(lang) == 0)
			{
				/*--------------------------------------------------------
				 * if language changed, init defs from that resource
				 */
				cur_lang = get_cur_lang();

				if (cur_lang != old_lang)
					dflt_init_defs(dflt_tbl, FALSE);
			}
		}

		/*----------------------------------------------------------------
		 * If in a window (i.e. not on the "console"), change default
		 * exit & escape colors from lt-white/black to black/lt-white.
		 */
		in_window = get_window_env();
		if (in_window)
		{
			attr_t	bw	= A_CLR(COLOR_BLACK, COLOR_LTWHITE);

			ds = dflt_find_entry(dflt_tbl, opts_off(escape_color));
			dflt_set_def(dflt_tbl, ds, &bw, FALSE);

			ds = dflt_find_entry(dflt_tbl, opts_off(exit_color));
			dflt_set_def(dflt_tbl, ds, &bw, FALSE);
		}

		/*----------------------------------------------------------------
		 * If in linux, do the following:
		 *
		 *	1.	Make default compression "gzip" rather than "lzw".
		 *
		 * We determine that we are in linux by looking for the
		 * file "/etc/lilo.conf".
		 */
		if (os_stat("/etc/lilo.conf", &stbuf) == 0)
		{
			int	comp_type = comp_gzip;

			/*------------------------------------------------------------
			 * set default comp-type to "gzip"
			 */
			ds = dflt_find_entry(dflt_tbl, opts_off(comp_type));
			dflt_set_def(dflt_tbl, ds, &comp_type, FALSE);
		}
	}

	/*--------------------------------------------------------------------
	 * setup all opt values
	 */
	dflt_init_opts(dflt_tbl);
}

void check_defaults (const char *initfile)
{
	char	filename[MAX_PATHLEN];

	/*--------------------------------------------------------------------
	 * load generic defaults file
	 */
	if (initfile != 0 && *initfile == 0)
		initfile = 0;

	if (initfile == 0)
	{
		strcpy(filename, pgmi(m_pgm_program));
		fn_set_ext(filename, pgm_const(cfg_ext));
	}
	else
	{
		strcpy(filename, initfile);
	}

	dflt_read(dflt_tbl, filename, gbl(pgm_path));

	/*--------------------------------------------------------------------
	 * load term-specific defaults file if it exists
	 */
	if (gbl(pgm_term) != 0 && *gbl(pgm_term) != 0 && initfile == 0)
	{
		strcat(filename, ".");
		strcat(filename, gbl(pgm_term));

		dflt_read(dflt_tbl, filename, gbl(pgm_path));
	}
}

int disp_defaults (int restore)
{
	int rc;

	if (restore)
		dflt_init_opts(dflt_tbl);

	rc = dflt_write(dflt_tbl, "-", TRUE);

	return (rc);
}
