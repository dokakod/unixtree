/*------------------------------------------------------------------------
 * internal main
 */
#include "libprd.h"

int int_main (int argc, char **argv)
{
	char	node_to_log[MAX_PATHLEN];
	int		rc;

	/*--------------------------------------------------------------------
	 * load environment
	 */
	load_env(argc, argv);

	/*--------------------------------------------------------------------
	 * process command line options
	 */
	rc = check_opts();
	if (rc)
	{
		return (rc > 0 ? 0 : 1);
	}

	/*--------------------------------------------------------------------
	 * process command line arguments
	 */
	rc = check_args(node_to_log);
	if (rc)
	{
		return (1);
	}

	/*--------------------------------------------------------------------
	 * initialize terminal to curses environment
	 */
	rc = init_wins();
	if (rc)
	{
		return (1);
	}

	/*--------------------------------------------------------------------
	 * finally - call the real pgm
	 */
	pgm(node_to_log);

	/*--------------------------------------------------------------------
	 * we never actually get here
	 */
	return (0);
}
