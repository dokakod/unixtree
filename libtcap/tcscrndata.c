/*------------------------------------------------------------------------
 * screen data
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * default screen struct
 */
static const SCRN_DATA tcap_def_screen_data =
{
	FALSE,					/* mode				*/

	FALSE,					/* num_lock			*/
	FALSE,					/* caps_lock		*/
	FALSE,					/* scroll_lock		*/

	FALSE,					/* alt_chars_on		*/
	0,						/* curr_reg_font	*/
	0,						/* curr_alt_font	*/

	VISIBILITY_ON,			/* visibility		*/

	A_UNSET,				/* curr_attr		*/
	A_UNSET,				/* orig_attr		*/

	FALSE,					/* color_flag		*/
	FALSE,					/* disp_in_window	*/

	-1,						/* curr_col			*/
	-1,						/* curr_row			*/

	500,					/* prefix interval	*/
	0,						/* alarm_time_left	*/

	FALSE,					/* mouse_restore	*/

	"",						/* debug_keys_path	*/
	0,						/* debug_keys_fp	*/
	FALSE,					/* debug_keys_text	*/

	"",						/* debug_scrn_path	*/
	0,						/* debug_scrn_fp	*/
	FALSE,					/* debug_scrn_text	*/
	FALSE,					/* debug_scrn_mode	*/

	0,						/* out_buf_count	*/
	sizeof(tcap_def_screen_data.out_buf)
							/* out_buf_size		*/
};

/*------------------------------------------------------------------------
 * tcap_init_screen_data() - initialize a SCRN_DATA struct
 */
SCRN_DATA * tcap_init_screen_data (void)
{
	SCRN_DATA *s;

	s = (SCRN_DATA *)MALLOC(sizeof(*s));
	if (s != 0)
	{
		memcpy(s, &tcap_def_screen_data, sizeof(*s));
	}

	return (s);
}

/*------------------------------------------------------------------------
 * tcap_free_screen_data() - free a SCRN_DATA struct
 */
void tcap_free_screen_data (SCRN_DATA *s)
{
	if (s != 0)
	{
		tcap_async_free(s);
		FREE(s);
	}
}
