/*------------------------------------------------------------------------
 * Here are all non-const variables for this library
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * default term db type
 */
#if V_WINDOWS
#  define TCAP_DB_DEFAULT	TCAP_DB_WINDOWS
#else
#  define TCAP_DB_DEFAULT	TCAP_DB_AUTO
#endif

/*------------------------------------------------------------------------
 * This struct contains all non-const data
 */
TCAP_VARS	tcap_vars =
{
	/*--------------------------------------------------------------------
	 * default term db type
	 */
	TCAP_DB_DEFAULT,		/* db_type		*/

	/*--------------------------------------------------------------------
	 * should environment variables be queried
	 */
	TRUE,					/* use_env_vars	*/

	/*--------------------------------------------------------------------
	 * default path to look for term-files
	 */
	0,						/* term_path	*/

	/*--------------------------------------------------------------------
	 * pointer to current terminal struct
	 */
	0						/* cur_terminal	*/
};
