/*------------------------------------------------------------------------
 * This file contains all options
 */
#ifndef OPTIONS_H
#define OPTIONS_H

/*------------------------------------------------------------------------
 * cmdopts struct
 *
 * These options can only be set on the cmd line.
 *
 * Note: there are other cmd-line options which affect config options.
 */
struct cmdopts
{
	int		use_ckp_file;		/* +/-x		T  = use existing ckp file		*/
	int		debug_kbd;			/* +/-K		T  = show keystrokes			*/
	int		skip_mnt_table;		/* +/-M		T  = skip mount tbl read		*/
	int		debug_keys;			/* -ZK		T  = save keystrokes in file	*/
	int		debug_keys_text;	/* -Zk		T  = save keystrokes as text	*/
	int		debug_scrn;			/* -ZS		T  = save output in file		*/
	int		debug_scrn_text;	/* -Zs		T  = save output as text		*/
	int		debug_fcfile;		/* -ZF		T  = debug file-color file		*/
	int		save_all_defaults;	/* -ZC		T  = save all defaults			*/

	int		debug_malloc;		/* -ZM		>0 = print mem usage at end		*/
								/* -ZMM		>1 = print chain if present		*/

	int		headline_delay;		/* -H secs	>0 = headline delay in secs		*/
								/* -H -1	<0 = wait for keypress			*/

	char *	trm_inp;			/* -I path	path for input					*/
	char *	trm_out;			/* -O path	path for output					*/
	char *	ckp_path;			/* -F path	path for checkpoint-file		*/

	BLIST *	exclude_list;		/* -e dir	list of dirs to exclude			*/
};
typedef struct cmdopts CMDOPTS;

/*------------------------------------------------------------------------
 * options struct
 *
 * Each option has three entries:
 *	0	OPT_OPT	current option value			set by dflt_init_opts()
 *	1	OPT_DEF	default value					set by dflt_init_defs()
 *	2	OPT_SAV	save slot for option values		set by dflt_save_opts()
 * See libutils/utdefaults.h for details.
 *
 * The options are grouped as they appear in the config menu.
 */
struct options
{
	/*--------------------------------------------------------------------
	 * Display
	 */
	OPT_INT		screen_type;
	OPT_INT		screen_blank_interval;
	OPT_BOOL	use_alt_char_sets;

	/*--------------------------------------------------------------------
	 * Window
	 */
	OPT_BOOL	run_in_window;
	OPT_BOOL	use_mouse;
	OPT_INT		columns;
	OPT_INT		rows;
	OPT_STR		norm_font;
	OPT_STR		bold_font;
	OPT_STR		toolkit_opts;

	/*--------------------------------------------------------------------
	 * Printer
	 */
	OPT_STR		printer;
	OPT_STR		print_filename;
	OPT_INT		page_length;
	OPT_INT		page_width;
	OPT_INT		page_margin;
	OPT_BOOL	close_printer_at_exit;

	/*--------------------------------------------------------------------
	 * Directory window
	 */
	OPT_INT		dir_fmt;
	OPT_BOOL	dir_scroll;

	/*--------------------------------------------------------------------
	 * File window
	 */
	OPT_INT		file_fmt;
	OPT_INT		sort_type;
	OPT_INT		sort_order;
	OPT_BOOL	sort_by_path;
	OPT_BOOL	do_small_window;
	OPT_INT		autolog;
	OPT_BOOL	do_file_colors;

	/*--------------------------------------------------------------------
	 * Compression
	 */
	OPT_INT		comp_type;

	/*--------------------------------------------------------------------
	 * Node logging
	 */
	OPT_INT		logging_method;
	OPT_STR		default_node;
	OPT_BOOL	home_to_node;
	OPT_BOOL	log_network_files;
	OPT_BOOL	scroll_during_logging;
	OPT_BOOL	cont_at_mount_points;
	OPT_BOOL	resolve_sym_links;
	OPT_INT		arch_log_interval;
	OPT_BOOL	ftp_logging;
	OPT_BOOL	ftp_debug;
	OPT_INT		ftp_timeout;

	/*--------------------------------------------------------------------
	 * Security
	 */
	OPT_BOOL	read_only;

	/*--------------------------------------------------------------------
	 * Miscellaneous 1
	 */
	OPT_BOOL	beep_on_error;
	OPT_BOOL	keep_date_in_copy;
	OPT_BOOL	display_clock;
	OPT_BOOL	keep_file_spec;
	OPT_BOOL	prompt_for_quit;
	OPT_BOOL	prompt_for_edit;
	OPT_BOOL	prompt_for_delete;
	OPT_BOOL	display_headline;
	OPT_BOOL	wide_screen;
	OPT_BOOL	show_tag_blocks;
	OPT_BOOL	enter_app_menu;
	OPT_BOOL	shift_for_tag;
	OPT_BOOL	show_appmenu_lines;

	/*--------------------------------------------------------------------
	 * Miscellaneous 2
	 */
	OPT_INT		date_type;
	OPT_STR		editor;
	OPT_STR		viewer;
	OPT_STR		hex_editor;
	OPT_INT		max_saved_entries;
	OPT_BOOL	insert_mode;
	OPT_BOOL	ignore_filename_case;
	OPT_BOOL	ignore_search_case;
	OPT_BOOL	show_mnt_dev;

	/*--------------------------------------------------------------------
	 * Archive
	 */
	OPT_INT		cpio_hdr_fmt;
	OPT_INT		tar_hdr_fmt;
	OPT_BOOL	arch_ignore_bad_types;

	/*--------------------------------------------------------------------
	 * Viewer
	 */
	OPT_INT		view_fmt;
	OPT_INT		tab_width;
	OPT_BOOL	display_mask;
	OPT_BOOL	exp_comp_files;

	/*--------------------------------------------------------------------
	 * International
	 */
	OPT_INT		time_fmt;
	OPT_INT		date_fmt;
	OPT_INT		time_sep_char;
	OPT_INT		date_sep_char;
	OPT_INT		num_sep_char;
	OPT_INT		language;

	/*--------------------------------------------------------------------
	 * File compare
	 */
	OPT_STR		diff_rtn;
	OPT_BOOL	diff_ignore_case;
	OPT_BOOL	diff_ignore_ws;
	OPT_BOOL	diff_show_numbers;
	OPT_BOOL	diff_show_diff;
	OPT_INT		diff_page_width;
	OPT_STR		diff_pattern;

	/*--------------------------------------------------------------------
	 * Regular window colors
	 */
	OPT_ATTR	reg_lolite_color;
	OPT_ATTR	reg_hilite_color;
	OPT_ATTR	reg_cursor_color;
	OPT_ATTR	reg_menu_color;

	/*--------------------------------------------------------------------
	 * Showall window colors
	 */
	OPT_ATTR	sa_lolite_color;
	OPT_ATTR	sa_hilite_color;
	OPT_ATTR	sa_cursor_color;
	OPT_ATTR	sa_menu_color;

	/*--------------------------------------------------------------------
	 * Autoview window colors
	 */
	OPT_ATTR	av_lolite_color;
	OPT_ATTR	av_hilite_color;
	OPT_ATTR	av_cursor_color;
	OPT_ATTR	av_menu_color;

	/*--------------------------------------------------------------------
	 * Diff colors
	 */
	OPT_ATTR	diff_same_color;
	OPT_ATTR	diff_diff_color;
	OPT_ATTR	diff_lfrt_color;
	OPT_ATTR	diff_numb_color;

	/*--------------------------------------------------------------------
	 * Help system window colors
	 */
	OPT_ATTR	help_box_lolite_color;
	OPT_ATTR	help_box_hilite_color;
	OPT_ATTR	help_box_bold_color;
	OPT_ATTR	help_box_hlink_color;
	OPT_ATTR	help_box_glink_color;
	OPT_ATTR	gloss_box_color;

	/*--------------------------------------------------------------------
	 * Menu colors
	 */
	OPT_ATTR	menu_lolite_color;
	OPT_ATTR	menu_hilite_color;
	OPT_ATTR	menu_cursor_color;
	OPT_ATTR	menu_border_color;

	/*--------------------------------------------------------------------
	 * Miscellaneous colors
	 */
	OPT_ATTR	border_color;
	OPT_ATTR	clock_color;
	OPT_ATTR	error_color;
	OPT_ATTR	mouse_color;
	OPT_ATTR	mouse_cmd_color;
	OPT_ATTR	escape_color;
	OPT_ATTR	exit_color;

	/*--------------------------------------------------------------------
	 * Regular window attributes
	 */
	OPT_ATTR	reg_lolite_mono;
	OPT_ATTR	reg_hilite_mono;
	OPT_ATTR	sa_cursor_mono;
	OPT_ATTR	reg_menu_mono;

	/*--------------------------------------------------------------------
	 * Showall window attributes
	 */
	OPT_ATTR	sa_lolite_mono;
	OPT_ATTR	sa_hilite_mono;
	OPT_ATTR	reg_cursor_mono;
	OPT_ATTR	sa_menu_mono;

	/*--------------------------------------------------------------------
	 * Autoview window attributes
	 */
	OPT_ATTR	av_lolite_mono;
	OPT_ATTR	av_hilite_mono;
	OPT_ATTR	av_cursor_mono;
	OPT_ATTR	av_menu_mono;

	/*--------------------------------------------------------------------
	 * Diff attributes
	 */
	OPT_ATTR	diff_same_mono;
	OPT_ATTR	diff_diff_mono;
	OPT_ATTR	diff_lfrt_mono;
	OPT_ATTR	diff_numb_mono;

	/*--------------------------------------------------------------------
	 * Help system window attributes
	 */
	OPT_ATTR	help_box_lolite_mono;
	OPT_ATTR	help_box_hilite_mono;
	OPT_ATTR	help_box_bold_mono;
	OPT_ATTR	help_box_hlink_mono;
	OPT_ATTR	help_box_glink_mono;
	OPT_ATTR	gloss_box_mono;

	/*--------------------------------------------------------------------
	 * Menu attributes
	 */
	OPT_ATTR	menu_lolite_mono;
	OPT_ATTR	menu_hilite_mono;
	OPT_ATTR	menu_cursor_mono;
	OPT_ATTR	menu_border_mono;

	/*--------------------------------------------------------------------
	 * Miscellaneous attributes
	 */
	OPT_ATTR	border_mono;
	OPT_ATTR	clock_mono;
	OPT_ATTR	error_mono;
	OPT_ATTR	mouse_mono;
	OPT_ATTR	mouse_cmd_mono;
	OPT_ATTR	escape_mono;
	OPT_ATTR	exit_mono;
};
typedef struct options OPTIONS;

#define opts_off(n)		X_OFFSET_OF(OPTIONS,n)

#endif /* OPTIONS_H */
