/*------------------------------------------------------------------------
 * This file contains all non-const variables in this library.
 *
 * They are all in one struct which is allocated on the heap by load_env().
 */
#include "libprd.h"

/*------------------------------------------------------------------------
 * global variables struct pointer
 */
GBLVARS *	gblvars_ptr;

/*------------------------------------------------------------------------
 * initialize all global vars
 */
void init_globals (int argc, char **argv)
{
	/*--------------------------------------------------------------------
	 * allocate global-vars struct
	 */
	gblvars_ptr = (GBLVARS *)MALLOC(sizeof(*gblvars_ptr));
	if (gblvars_ptr == 0)
	{
		fprintf(stderr, "No memory\n");
		pgm_exit(1);
	}

	memset(gblvars_ptr, 0, sizeof(*gblvars_ptr));

	/*--------------------------------------------------------------------
	 * initialize resource info
	 */
	gbl(cur_res) = &res_file;

	/*--------------------------------------------------------------------
	 * get version info built with
	 */
	gbl(pgm_version)	= pgmi(m_pgm_version);
	gbl(pgm_release)	= pgmi(m_pgm_release);
	gbl(pgm_build)		= pgmi(m_pgm_build);

	/*--------------------------------------------------------------------
	 * allocate XVT_DATA struct
	 */
	gbl(xvt_data)  = xvt_data_alloc();
	gbl(xvt_avail) = xvt_available();

	/*--------------------------------------------------------------------
	 * process cmd line
	 */
	xvt_tk_args(gbl(xvt_data), argc, argv);

	gbl(pgm_argc) = xvt_data_get_argc(gbl(xvt_data));
	gbl(pgm_argv) = xvt_data_get_argv(gbl(xvt_data));
}
