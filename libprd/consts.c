/*------------------------------------------------------------------------
 * this file contains all const variables for this library
 */
#include "libprd.h"

/*------------------------------------------------------------------------
 * program consts
 */
const PGM_CONSTS	pgm_consts =
{
	/*--------------------------------------------------------------------
	 * support filename extensions
	 */
	"hlp",		/* hlp_ext					*/
	"mnu",		/* mnu_ext					*/
	"res",		/* res_ext					*/
	"dev",		/* dev_ext					*/
	"ckp",		/* ckp_ext					*/
	"hst",		/* hst_ext					*/
	"rc",		/* cfg_ext					*/
	"fc",		/* clr_ext					*/

	/*--------------------------------------------------------------------
	 * other misc variables
	 */
	5,			/* time_interval			*/
	1000,		/* kbd_wait_interval		*/
	B_DVDH,		/* border_type				*/
	3,			/* default_dir_line			*/

	/*--------------------------------------------------------------------
	 * misc display chars
	 */
	'+',			/* tag_disp_char		*/
	'>',			/* too_long_char		*/
	'~',			/* unprintable_char		*/
	'<',			/* selected_char		*/

	/*--------------------------------------------------------------------
	 * dir flag display chars
	 */
	'.',			/* hidden_dir_char		*/
	'+',			/* unlogged_dir_char	*/
	'-',			/* changed_dir_char		*/

	/*--------------------------------------------------------------------
	 * file type display chars
	 */
	' ',			/* reg_display_char		*/
	' ',			/* blk_display_char		*/
	' ',			/* chr_display_char		*/
	' ',			/* ffo_display_char		*/
	' ',			/* nam_display_char		*/
	' ',			/* sck_display_char		*/
	'@',			/* sym_display_char		*/
	'?',			/* syd_display_char		*/
	'D'				/* dor_display_char		*/
};
