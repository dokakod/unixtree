/*------------------------------------------------------------------------
 * config tables
 */
#include "libprd.h"

/*------------------------------------------------------------------------
 * shorthand macros
 */
#define D(n)	(n)				/* dflt(n) message num  entry	*/
#define M(n)	(n)				/* msgs(n) message num  entry	*/
#define O(n)	opts_off(n)		/* option table offset  entry	*/

/*------------------------------------------------------------------------
 * settings menus
 */
static const CFG_MENU cfg_settings_1[] =
{
	{ CFG_HDG,	(0),				D(m_default_hdg_disp),		 0 },
	{ CFG_ITEM,	'1',				O(screen_type),				 1 },
	{ CFG_ITEM,	'2',				O(screen_blank_interval),	 2 },
	{ CFG_ITEM,	'3',				O(use_alt_char_sets),		 3 },

	{ CFG_HDG,	(0),				D(m_default_hdg_win),		 6 },
	{ CFG_ITEM,	'4',				O(run_in_window),			 7 },
	{ CFG_ITEM,	'5',				O(use_mouse),				 8 },
	{ CFG_ITEM,	'6',				O(columns),					 9 },
	{ CFG_ITEM,	'7',				O(rows),					10 },
	{ CFG_ITEM,	'8',				O(norm_font),				11 },
	{ CFG_ITEM,	'9',				O(bold_font),				12 },
	{ CFG_ITEM,	'A',				O(toolkit_opts),			13 },

	{ CFG_BTN,	CMDS_CONFIG_PREV,	M(m_config_prev),			 2 },
	{ CFG_BTN,	CMDS_CONFIG_NEXT,	M(m_config_next),			22 },
	{ CFG_BTN,	CMDS_CONFIG_MAIN,	M(m_config_main),			44 },

	{ 0, 0, 0, 0 }
};

static const CFG_MENU cfg_settings_2[] =
{
	{ CFG_HDG,	(0),				D(m_default_hdg_dir),		 0 },
	{ CFG_ITEM,	'1',				O(dir_fmt),					 1 },
	{ CFG_ITEM,	'2',				O(dir_scroll),				 2 },

	{ CFG_HDG,	(0),				D(m_default_hdg_file),		 4 },
	{ CFG_ITEM,	'3',				O(file_fmt),				 5 },
	{ CFG_ITEM,	'4',				O(sort_type),				 6 },
	{ CFG_ITEM,	'5',				O(sort_order),				 7 },
	{ CFG_ITEM,	'6',				O(sort_by_path),			 8 },
	{ CFG_ITEM,	'7',				O(do_small_window),			 9 },
	{ CFG_ITEM,	'8',				O(autolog),					10 },
	{ CFG_ITEM,	'9',				O(do_file_colors),			11 },

	{ CFG_HDG,	(0),				D(m_default_hdg_comp),		13 },
	{ CFG_ITEM,	'A',				O(comp_type),				14 },

	{ CFG_BTN,	CMDS_CONFIG_PREV,	M(m_config_prev),			 2 },
	{ CFG_BTN,	CMDS_CONFIG_NEXT,	M(m_config_next),			22 },
	{ CFG_BTN,	CMDS_CONFIG_MAIN,	M(m_config_main),			44 },

	{ 0, 0, 0, 0 }
};

static const CFG_MENU cfg_settings_3[] =
{
	{ CFG_HDG,	(0),				D(m_default_hdg_log),		 0 },
	{ CFG_ITEM,	'1',				O(logging_method),			 1 },
	{ CFG_ITEM,	'2',				O(default_node),			 2 },
	{ CFG_ITEM,	'3',				O(home_to_node),			 3 },
	{ CFG_ITEM,	'4',				O(log_network_files),		 4 },
	{ CFG_ITEM,	'5',				O(scroll_during_logging),	 5 },
	{ CFG_ITEM,	'6',				O(cont_at_mount_points),	 6 },
	{ CFG_ITEM,	'7',				O(resolve_sym_links),		 7 },
	{ CFG_ITEM,	'8',				O(arch_log_interval),		 8 },
	{ CFG_ITEM,	'9',				O(ftp_logging),				 9 },
	{ CFG_ITEM,	'A',				O(ftp_debug),				10 },
	{ CFG_ITEM,	'B',				O(ftp_timeout),				11 },

	{ CFG_HDG,	(0),				D(m_default_hdg_sec),		13 },
	{ CFG_ITEM,	'C',				O(read_only),				14 },

	{ CFG_BTN,	CMDS_CONFIG_PREV,	M(m_config_prev),			 2 },
	{ CFG_BTN,	CMDS_CONFIG_NEXT,	M(m_config_next),			22 },
	{ CFG_BTN,	CMDS_CONFIG_MAIN,	M(m_config_main),			44 },

	{ 0, 0, 0, 0 }
};

static const CFG_MENU cfg_settings_4[] =
{
	{ CFG_HDG,	(0),				D(m_default_hdg_msc1),		 0 },
	{ CFG_ITEM,	'1',				O(beep_on_error),			 1 },
	{ CFG_ITEM,	'2',				O(keep_date_in_copy),		 2 },
	{ CFG_ITEM,	'3',				O(display_clock),			 3 },
	{ CFG_ITEM,	'4',				O(keep_file_spec),			 4 },
	{ CFG_ITEM,	'5',				O(prompt_for_quit),			 5 },
	{ CFG_ITEM,	'6',				O(prompt_for_edit),			 6 },
	{ CFG_ITEM,	'7',				O(prompt_for_delete),		 7 },
	{ CFG_ITEM,	'8',				O(display_headline),		 8 },
	{ CFG_ITEM,	'9',				O(wide_screen),				 9 },
	{ CFG_ITEM,	'A',				O(show_tag_blocks),			10 },
	{ CFG_ITEM, 'B',				O(enter_app_menu),			11 },
	{ CFG_ITEM,	'C',				O(shift_for_tag),			12 },
	{ CFG_ITEM,	'D',				O(show_appmenu_lines),		13 },

	{ CFG_BTN,	CMDS_CONFIG_PREV,	M(m_config_prev),			 2 },
	{ CFG_BTN,	CMDS_CONFIG_NEXT,	M(m_config_next),			22 },
	{ CFG_BTN,	CMDS_CONFIG_MAIN,	M(m_config_main),			44 },

	{ 0, 0, 0, 0 }
};

static const CFG_MENU cfg_settings_5[] =
{
	{ CFG_HDG,	(0),				D(m_default_hdg_msc2),		 0 },
	{ CFG_ITEM,	'1',				O(date_type),				 1 },
	{ CFG_ITEM,	'2',				O(editor),					 2 },
	{ CFG_ITEM,	'3',				O(viewer),					 3 },
	{ CFG_ITEM,	'4',				O(hex_editor),				 4 },
	{ CFG_ITEM,	'5',				O(max_saved_entries),		 5 },
	{ CFG_ITEM,	'6',				O(insert_mode),				 6 },
	{ CFG_ITEM,	'7',				O(ignore_filename_case),	 7 },
	{ CFG_ITEM,	'8',				O(ignore_search_case),		 8 },
	{ CFG_ITEM,	'9',				O(show_mnt_dev),			 9 },

	{ CFG_HDG,	(0),				D(m_default_hdg_arch),		11 },
	{ CFG_ITEM,	'A',				O(cpio_hdr_fmt),			12 },
	{ CFG_ITEM,	'B',				O(tar_hdr_fmt),				13 },
	{ CFG_ITEM,	'C',				O(arch_ignore_bad_types),	14 },

	{ CFG_BTN,	CMDS_CONFIG_PREV,	M(m_config_prev),			 2 },
	{ CFG_BTN,	CMDS_CONFIG_NEXT,	M(m_config_next),			22 },
	{ CFG_BTN,	CMDS_CONFIG_MAIN,	M(m_config_main),			44 },

	{ 0, 0, 0, 0 }
};

static const CFG_MENU cfg_settings_6[] =
{
	{ CFG_HDG,	(0),				D(m_default_hdg_view),		 0 },
	{ CFG_ITEM,	'1',				O(view_fmt),				 1 },
	{ CFG_ITEM,	'2',				O(tab_width),				 2 },
	{ CFG_ITEM,	'3',				O(display_mask),			 3 },
	{ CFG_ITEM,	'4',				O(exp_comp_files),			 4 },

	{ CFG_HDG,	(0),				D(m_default_hdg_intl),		 6 },
	{ CFG_ITEM,	'5',				O(time_fmt),				 7 },
	{ CFG_ITEM,	'6',				O(date_fmt),				 8 },
	{ CFG_ITEM,	'7',				O(time_sep_char),			 9 },
	{ CFG_ITEM,	'8',				O(date_sep_char),			10 },
	{ CFG_ITEM,	'9',				O(num_sep_char),			11 },
	{ CFG_ITEM,	'A',				O(language),				12 },

	{ CFG_BTN,	CMDS_CONFIG_PREV,	M(m_config_prev),			 2 },
	{ CFG_BTN,	CMDS_CONFIG_NEXT,	M(m_config_next),			22 },
	{ CFG_BTN,	CMDS_CONFIG_MAIN,	M(m_config_main),			44 },

	{ 0, 0, 0, 0 }
};

static const CFG_MENU cfg_settings_7[] =
{
	{ CFG_HDG,	(0),				D(m_default_hdg_diff),		 0 },
	{ CFG_ITEM,	'1',				O(diff_rtn),				 1 },
	{ CFG_ITEM,	'2',				O(diff_ignore_case),		 2 },
	{ CFG_ITEM,	'3',				O(diff_ignore_ws),			 3 },
	{ CFG_ITEM,	'4',				O(diff_show_numbers),		 4 },
	{ CFG_ITEM,	'5',				O(diff_show_diff),			 5 },
	{ CFG_ITEM,	'6',				O(diff_page_width),			 6 },
	{ CFG_ITEM,	'7',				O(diff_pattern),			 7 },

	{ CFG_BTN,	CMDS_CONFIG_PREV,	M(m_config_prev),			 2 },
	{ CFG_BTN,	CMDS_CONFIG_NEXT,	M(m_config_next),			22 },
	{ CFG_BTN,	CMDS_CONFIG_MAIN,	M(m_config_main),			44 },

	{ 0, 0, 0, 0 }
};

static const CFG_MENU cfg_settings_8[] =
{
	{ CFG_HDG,	(0),				D(m_default_hdg_prt), 		 0 },
	{ CFG_ITEM,	'1',				O(printer),					 1 },
	{ CFG_ITEM,	'2',				O(print_filename),			 2 },
	{ CFG_ITEM,	'3',				O(page_length),				 3 },
	{ CFG_ITEM,	'4',				O(page_width),				 4 },
	{ CFG_ITEM,	'5',				O(page_margin),				 5 },
	{ CFG_ITEM,	'6',				O(close_printer_at_exit),	 6 },

	{ CFG_BTN,	CMDS_CONFIG_PREV,	M(m_config_prev),			 2 },
	{ CFG_BTN,	CMDS_CONFIG_NEXT,	M(m_config_next),			22 },
	{ CFG_BTN,	CMDS_CONFIG_MAIN,	M(m_config_main),			44 },

	{ 0, 0, 0, 0 }
};

/*------------------------------------------------------------------------
 * color attribute menus
 */
static const CFG_MENU cfg_color_1[] =
{
	{ CFG_HDG,	(0),				D(m_default_hdg_c1),		 0 },

	{ CFG_ITEM,	'1',				O(reg_lolite_color),		 2 },
	{ CFG_ITEM,	'2',				O(reg_hilite_color),		 3 },
	{ CFG_ITEM,	'3',				O(reg_cursor_color),		 4 },
	{ CFG_ITEM,	'4',				O(reg_menu_color),			 5 },

	{ CFG_HDG,	(0),				D(m_default_hdg_c2),		 7 },

	{ CFG_ITEM,	'5',				O(sa_lolite_color),			 9 },
	{ CFG_ITEM,	'6',				O(sa_hilite_color),			10 },
	{ CFG_ITEM,	'7',				O(sa_cursor_color),			11 },
	{ CFG_ITEM,	'8',				O(sa_menu_color),			12 },

	{ CFG_BTN,	CMDS_CONFIG_PREV,	M(m_config_prev),			 2 },
	{ CFG_BTN,	CMDS_CONFIG_NEXT,	M(m_config_next),			22 },
	{ CFG_BTN,	CMDS_CONFIG_MAIN,	M(m_config_main),			44 },

	{ 0, 0, 0, 0 }
};

static const CFG_MENU cfg_color_2[] =
{
	{ CFG_HDG,	(0),				D(m_default_hdg_c3),		 0 },

	{ CFG_ITEM,	'1',				O(av_lolite_color),			 2 },
	{ CFG_ITEM,	'2',				O(av_hilite_color),			 3 },
	{ CFG_ITEM,	'3',				O(av_cursor_color),			 4 },
	{ CFG_ITEM,	'4',				O(av_menu_color),			 5 },

	{ CFG_HDG,	(0),				D(m_default_hdg_c4),		 7 },

	{ CFG_ITEM,	'5',				O(diff_same_color),			 9 },
	{ CFG_ITEM,	'6',				O(diff_diff_color),			10 },
	{ CFG_ITEM,	'7',				O(diff_lfrt_color),			11 },
	{ CFG_ITEM,	'8',				O(diff_numb_color),			12 },

	{ CFG_BTN,	CMDS_CONFIG_PREV,	M(m_config_prev),			 2 },
	{ CFG_BTN,	CMDS_CONFIG_NEXT,	M(m_config_next),			22 },
	{ CFG_BTN,	CMDS_CONFIG_MAIN,	M(m_config_main),			44 },

	{ 0, 0, 0, 0 }
};

static const CFG_MENU cfg_color_3[] =
{
	{ CFG_HDG,	(0),				D(m_default_hdg_c5),		 0 },

	{ CFG_ITEM,	'1',				O(help_box_lolite_color),	 2 },
	{ CFG_ITEM,	'2',				O(help_box_hilite_color),	 3 },
	{ CFG_ITEM,	'3',				O(help_box_bold_color),		 4 },
	{ CFG_ITEM,	'4',				O(help_box_hlink_color),	 5 },
	{ CFG_ITEM,	'5',				O(gloss_box_color),			 6 },
	{ CFG_ITEM,	'6',				O(help_box_glink_color),	 7 },

	{ CFG_HDG,	(0),				D(m_default_hdg_c6),		 9 },

	{ CFG_ITEM,	'7',				O(menu_lolite_color),		11 },
	{ CFG_ITEM,	'8',				O(menu_hilite_color),		12 },
	{ CFG_ITEM,	'9',				O(menu_cursor_color),		13 },
	{ CFG_ITEM,	'A',				O(menu_border_color),		14 },

	{ CFG_BTN,	CMDS_CONFIG_PREV,	M(m_config_prev),			 2 },
	{ CFG_BTN,	CMDS_CONFIG_NEXT,	M(m_config_next),			22 },
	{ CFG_BTN,	CMDS_CONFIG_MAIN,	M(m_config_main),			44 },

	{ 0, 0, 0, 0 }
};

static const CFG_MENU cfg_color_4[] =
{
	{ CFG_HDG,	(0),				D(m_default_hdg_c7),		 0 },

	{ CFG_ITEM,	'1',				O(border_color),			 2 },
	{ CFG_ITEM,	'2',				O(clock_color),				 3 },
	{ CFG_ITEM,	'3',				O(error_color),				 4 },
	{ CFG_ITEM,	'4',				O(mouse_color),				 5 },
	{ CFG_ITEM,	'5',				O(mouse_cmd_color),			 6 },
	{ CFG_ITEM,	'6',				O(escape_color),			 7 },
	{ CFG_ITEM,	'7',				O(exit_color),				 8 },

	{ CFG_BTN,	CMDS_CONFIG_PREV,	M(m_config_prev),			 2 },
	{ CFG_BTN,	CMDS_CONFIG_NEXT,	M(m_config_next),			22 },
	{ CFG_BTN,	CMDS_CONFIG_MAIN,	M(m_config_main),			44 },

	{ 0, 0, 0, 0 }
};

/*------------------------------------------------------------------------
 * mono attribute menus
 */
static const CFG_MENU cfg_mono_1[] =
{
	{ CFG_HDG,	(0),				D(m_default_hdg_m1),		 0 },

	{ CFG_ITEM,	'1',				O(reg_lolite_mono),			 2 },
	{ CFG_ITEM,	'2',				O(reg_hilite_mono),			 3 },
	{ CFG_ITEM,	'3',				O(reg_cursor_mono),			 4 },
	{ CFG_ITEM,	'4',				O(reg_menu_mono),			 5 },

	{ CFG_HDG,	(0),				D(m_default_hdg_m2),		 7 },

	{ CFG_ITEM,	'5',				O(sa_lolite_mono),			 9 },
	{ CFG_ITEM,	'6',				O(sa_hilite_mono),			10 },
	{ CFG_ITEM,	'7',				O(sa_cursor_mono),			11 },
	{ CFG_ITEM,	'8',				O(sa_menu_mono),			12 },

	{ CFG_BTN,	CMDS_CONFIG_PREV,	M(m_config_prev),			 2 },
	{ CFG_BTN,	CMDS_CONFIG_NEXT,	M(m_config_next),			22 },
	{ CFG_BTN,	CMDS_CONFIG_MAIN,	M(m_config_main),			44 },

	{ 0, 0, 0, 0 }
};

static const CFG_MENU cfg_mono_2[] =
{
	{ CFG_HDG,	(0),				D(m_default_hdg_m3),		 0 },

	{ CFG_ITEM,	'1',				O(av_lolite_mono),			 2 },
	{ CFG_ITEM,	'2',				O(av_hilite_mono),			 3 },
	{ CFG_ITEM,	'3',				O(av_cursor_mono),			 4 },
	{ CFG_ITEM,	'4',				O(av_menu_mono),			 5 },

	{ CFG_HDG,	(0),				D(m_default_hdg_m4),		 7 },

	{ CFG_ITEM,	'5',				O(diff_same_mono),			 9 },
	{ CFG_ITEM,	'6',				O(diff_diff_mono),			10 },
	{ CFG_ITEM,	'7',				O(diff_lfrt_mono),			11 },
	{ CFG_ITEM,	'8',				O(diff_numb_mono),			12 },

	{ CFG_BTN,	CMDS_CONFIG_PREV,	M(m_config_prev),			 2 },
	{ CFG_BTN,	CMDS_CONFIG_NEXT,	M(m_config_next),			22 },
	{ CFG_BTN,	CMDS_CONFIG_MAIN,	M(m_config_main),			44 },

	{ 0, 0, 0, 0 }
};

static const CFG_MENU cfg_mono_3[] =
{
	{ CFG_HDG,	(0),				D(m_default_hdg_m5),		 0 },

	{ CFG_ITEM,	'1',				O(help_box_lolite_mono),	 2 },
	{ CFG_ITEM,	'2',				O(help_box_hilite_mono),	 3 },
	{ CFG_ITEM,	'3',				O(help_box_bold_mono),		 4 },
	{ CFG_ITEM,	'4',				O(help_box_hlink_mono),		 5 },
	{ CFG_ITEM,	'5',				O(gloss_box_mono),			 6 },
	{ CFG_ITEM,	'6',				O(help_box_glink_mono),		 7 },

	{ CFG_HDG,	(0),				D(m_default_hdg_m6),		 9 },

	{ CFG_ITEM,	'7',				O(menu_lolite_mono),		11 },
	{ CFG_ITEM,	'8',				O(menu_hilite_mono),		12 },
	{ CFG_ITEM,	'9',				O(menu_cursor_mono),		13 },
	{ CFG_ITEM,	'A',				O(menu_border_mono),		14 },

	{ CFG_BTN,	CMDS_CONFIG_PREV,	M(m_config_prev),			 2 },
	{ CFG_BTN,	CMDS_CONFIG_NEXT,	M(m_config_next),			22 },
	{ CFG_BTN,	CMDS_CONFIG_MAIN,	M(m_config_main),			44 },

	{ 0, 0, 0, 0 }
};

static const CFG_MENU cfg_mono_4[] =
{
	{ CFG_HDG,	(0),				D(m_default_hdg_m7),		 0 },

	{ CFG_ITEM,	'1',				O(border_mono),				 2 },
	{ CFG_ITEM,	'2',				O(clock_mono),				 3 },
	{ CFG_ITEM,	'3',				O(error_mono),				 4 },
	{ CFG_ITEM,	'4',				O(mouse_mono),				 5 },
	{ CFG_ITEM,	'5',				O(mouse_cmd_mono),			 6 },
	{ CFG_ITEM,	'6',				O(escape_mono),				 7 },
	{ CFG_ITEM,	'7',				O(exit_mono),				 8 },

	{ CFG_BTN,	CMDS_CONFIG_PREV,	M(m_config_prev),			 2 },
	{ CFG_BTN,	CMDS_CONFIG_NEXT,	M(m_config_next),			22 },
	{ CFG_BTN,	CMDS_CONFIG_MAIN,	M(m_config_main),			44 },

	{ 0, 0, 0, 0 }
};

/*------------------------------------------------------------------------
 * main menu
 */
static const CFG_MENU cfg_main_menu[] =
{
	{ CFG_MAIN,	'1',				M(m_config_mm1),			 2 },
	{ CFG_MAIN,	'2',				M(m_config_mm2),			 3 },

	{ CFG_MAIN,	'3',				M(m_config_mm3),			 5 },
	{ CFG_MAIN,	'4',				M(m_config_mm4),			 6 },

	{ CFG_MAIN,	'5',				M(m_config_mm5),			 8 },
	{ CFG_MAIN,	'6',				M(m_config_mm6),			 9 },

	{ CFG_MAIN,	'7',				M(m_config_mm7),			11 },
	{ CFG_MAIN,	'8',				M(m_config_mm8),			12 },

	{ 0, 0, 0, 0 }
};

/*------------------------------------------------------------------------
 * list of settings menus
 */
static const CFG_MENU *cfg_settings[] =
{
	cfg_settings_1,
	cfg_settings_2,
	cfg_settings_3,
	cfg_settings_4,
	cfg_settings_5,
	cfg_settings_6,
	cfg_settings_7,
	cfg_settings_8,
	0
};

/*------------------------------------------------------------------------
 * list of color attr menus
 */
static const CFG_MENU *cfg_colors[] =
{
	cfg_color_1,
	cfg_color_2,
	cfg_color_3,
	cfg_color_4,
	0
};

/*------------------------------------------------------------------------
 * list of mono attr menus
 */
static const CFG_MENU *cfg_monos[] =
{
	cfg_mono_1,
	cfg_mono_2,
	cfg_mono_3,
	cfg_mono_4,
	0
};

/*------------------------------------------------------------------------
 * config data table
 */
static const CFG_DATA cfg_data_tbl =
{
	cfg_main_menu,		/* pointer to main       menu		*/
	cfg_settings,		/* pointer to settings   menu list	*/
	cfg_colors,			/* pointer to color attr menu list	*/
	cfg_monos			/* pointer to monor attr menu list	*/
};

/*------------------------------------------------------------------------
 * pointer to config data table
 */
const CFG_DATA *	cfg_data = &cfg_data_tbl;
