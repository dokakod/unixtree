/*------------------------------------------------------------------------
 * This file contains all non-const variables in this library.
 *
 * They are all in one struct which is allocated on the heap by load_env().
 */
#ifndef GBLVARS_H
#define GBLVARS_H

/*------------------------------------------------------------------------
 * global variables struct
 */
struct gblvars
{
	/*--------------------------------------------------------------------
	 * command line (after toolkit options are removed)
	 */
	int			pgm_argc;					/* new argc					*/
	char **		pgm_argv;					/* new argv list			*/

	/*--------------------------------------------------------------------
	 * environment variables (set by load_env(), stay constant)
	 */
	char		pgm_name  [MAX_FILELEN];	/* program name				*/
	char		pgm_cwd   [MAX_PATHLEN];	/* current working dir		*/
	char		pgm_dir   [MAX_PATHLEN];	/* directory program is in	*/
	char		pgm_home  [MAX_PATHLEN];	/* program home dir			*/

	char		pgm_path  [MAX_PATHLEN];	/* program path				*/
	char		pgm_magic [MAX_PATHLEN];	/* magic path				*/

	char		pgm_label [MAX_PATHLEN];	/* window label				*/

	const char *pgm_version;			/* version built with			*/
	const char *pgm_release;			/* release built with			*/
	const char *pgm_build;				/* build   built with			*/

	const char *pgm_tmp;				/* $TMP							*/
	const char *pgm_term;				/* $TERM						*/

	int			pgm_euid;				/* our effective uid			*/
	int			pgm_egid;				/* our effective gid			*/
	int			pgm_dst;				/* our current DST adjustment	*/
	int			pgm_gmt;				/* our current GMT adjustment	*/

	int			pgm_dflts_setup;		/* TRUE is defaults setup		*/

	/*--------------------------------------------------------------------
	 * global info lists
	 */
	BLIST *		owner_list;				/* (USR_GRP    *) owner names	*/
	BLIST *		group_list;				/* (USR_GRP    *) group names	*/
	BLIST *		nodes_list;				/* (NBLK       *) logged nodes	*/
	BLIST *		mount_list;				/* (MOUNT_INFO *) mount list	*/

	/*--------------------------------------------------------------------
	 * program state variables
	 */
	int			scr_cur_cmd;			/* current command				*/
	int			scr_cur_sub_cmd;		/* current sub-command			*/
	int			scr_temp_sub_mode;		/* TRUE = sub-mode is temp		*/

	int			scr_is_split;			/* TRUE = screen is split		*/
	int			scr_av_focus;			/* TRUE = in display win		*/
	int			scr_info_displayed;		/* TRUE = info displayed		*/

	int			scr_in_autoview;		/* TRUE = in autoview			*/
	int			scr_in_fullview;		/* TRUE = in fullview			*/
	int			scr_in_hexedit;			/* TRUE = in hexedit			*/
	int			scr_in_config;			/* TRUE = in config window		*/
	int			scr_in_menu;			/* TRUE = in menu window		*/
	int			scr_in_diffview;		/* TRUE = in diff window		*/
	int			scr_in_help;			/* TRUE = in help				*/
	int			scr_in_about_box;		/* TRUE = in about box			*/

	int			scr_kbd_shift;			/* TRUE = shift key is down		*/
	int			scr_kbd_alt;			/* TRUE = alt   key is down		*/

	/*--------------------------------------------------------------------
	 * global screen variables
	 */
	int			scr_size_changed;		/* T = screen size changed		*/
	int			scr_inp_fd;				/* input  fd					*/
	int			scr_out_fd;				/* output fd					*/

	int			scr_is_color;			/* TRUE if color				*/
	int			scr_in_window;			/* TRUE if in window			*/

	/*--------------------------------------------------------------------
	 * global display variables
	 */
	int			scr_num_split_wins;		/* max number of screens		*/
	int			scr_cur_count;			/* current num of screens		*/
	int			scr_cur_no;				/* active screen number			*/

	dev_t		scr_cur_dev;			/* current device ID			*/
	long		scr_cur_free;			/* free space on curr dev		*/

	DSTAT *		scr_cur;				/* ptr to cur scr_stat entry	*/
	DSTAT		scr_stat[NUM_SPLIT_WINS];	/* screen stats array		*/

	/*--------------------------------------------------------------------
	 * screen attributes
	 *
	 * Each attribute is set from the corresponding mono or color entry
	 * in setup_window_attributes().
	 */
	attr_t		scr_reg_hilite_attr;
	attr_t		scr_reg_lolite_attr;
	attr_t		scr_reg_cursor_attr;
	attr_t		scr_reg_menu_attr;

	attr_t		scr_sa_hilite_attr;
	attr_t		scr_sa_lolite_attr;
	attr_t		scr_sa_cursor_attr;
	attr_t		scr_sa_menu_attr;

	attr_t		scr_av_hilite_attr;
	attr_t		scr_av_lolite_attr;
	attr_t		scr_av_cursor_attr;
	attr_t		scr_av_menu_attr;

	attr_t		scr_diff_same_attr;
	attr_t		scr_diff_diff_attr;
	attr_t		scr_diff_lfrt_attr;
	attr_t		scr_diff_numb_attr;

	attr_t		scr_help_box_hilite_attr;
	attr_t		scr_help_box_lolite_attr;
	attr_t		scr_help_box_bold_attr;
	attr_t		scr_help_box_hlink_attr;
	attr_t		scr_help_box_glink_attr;
	attr_t		scr_gloss_box_attr;

	attr_t		scr_menu_hilite_attr;
	attr_t		scr_menu_lolite_attr;
	attr_t		scr_menu_cursor_attr;
	attr_t		scr_menu_border_attr;

	attr_t		scr_border_attr;
	attr_t		scr_clock_attr;
	attr_t		scr_error_attr;
	attr_t		scr_mouse_attr;
	attr_t		scr_mouse_cmd_attr;
	attr_t		scr_escape_attr;
	attr_t		scr_exit_attr;

	/*--------------------------------------------------------------------
	 * windows
	 *
	 * These are all created in create_windows() &
	 * deleted in delete_windows().
	 */
	WINDOW *	win_dir_reg;
	WINDOW *	win_dir_split[NUM_SPLIT_WINS];
	WINDOW *	win_dir_wide;

	WINDOW *	win_dest_dir_reg;
	WINDOW *	win_dest_dir_split[NUM_SPLIT_WINS];
	WINDOW *	win_dest_dir_wide;

	WINDOW *	win_large_file_reg;
	WINDOW *	win_large_file_split[NUM_SPLIT_WINS];
	WINDOW *	win_large_file_wide;

	WINDOW *	win_small_file_reg;
	WINDOW *	win_small_file_split[NUM_SPLIT_WINS];
	WINDOW *	win_small_file_wide;

	WINDOW *	win_path_line_reg;
	WINDOW *	win_path_line_split[NUM_SPLIT_WINS];

	WINDOW *	win_av_path_line_reg;
	WINDOW *	win_av_path_line_split[NUM_SPLIT_WINS];

	WINDOW *	win_av_mode_reg;
	WINDOW *	win_av_mode_split[NUM_SPLIT_WINS];

	WINDOW *	win_av_file_reg;
	WINDOW *	win_av_file_split[NUM_SPLIT_WINS];

	WINDOW *	win_av_view_reg;
	WINDOW *	win_av_view_split[NUM_SPLIT_WINS];

	WINDOW *	win_av_dest_dir_reg;
	WINDOW *	win_av_dest_dir_split[NUM_SPLIT_WINS];

	WINDOW *	win_full_view;

	WINDOW *	win_file_box;
	WINDOW *	win_drive_box;
	WINDOW *	win_stats;
	WINDOW *	win_current;
	WINDOW *	win_commands;
	WINDOW *	win_message;
	WINDOW *	win_clkdisp;
	WINDOW *	win_border;
	WINDOW *	win_error;
	WINDOW *	win_config;

	WINDOW *	win_menu_reg;
	WINDOW *	win_menu_sub;

	WINDOW *	win_diff_lf;
	WINDOW *	win_diff_rt;
	WINDOW *	win_diff_tp;

	/*--------------------------------------------------------------------
	 * view file control blocks for autoview windows
	 */
	VFCB *		vfcb_av;
	VFCB *		vfcb_av_split[NUM_SPLIT_WINS];

	/*--------------------------------------------------------------------
	 * view file control blocks for fullview windows
	 */
	VFCB *		vfcb_fv;

	/*--------------------------------------------------------------------
	 * hexedit control block
	 */
	HXCB *		hxcb;

	/*--------------------------------------------------------------------
	 * resource files
	 */
	const RES_FILE *cur_res;			/* pointer to current res file	*/

	BLIST *		res_list;				/* list of resource info ents	*/

	/*--------------------------------------------------------------------
	 * compiled reg exp to search for & the string which created it
	 */
	REGEXP *	srch_re;				/* current RE to search for		*/
	char		srch_str[MAX_PATHLEN];	/* string defining the RE		*/

	/*--------------------------------------------------------------------
	 * list of tagged files to view
	 */
	BLIST *		view_list;				/* used by tag-view cmd			*/

	/*--------------------------------------------------------------------
	 * archive info
	 */
	ARCH_INFO	arch_info;				/* archive info					*/

	BLIST *		arch_dev_list;			/* archive device list			*/

	/*--------------------------------------------------------------------
	 * The following are only used in one routine, but are considered
	 * global because they contain non-transient data.
	 */
	BLIST *		fa_list;				/* color   list	(falist.c)		*/
	MAGIC *		magic_list;				/* magic   list	(what.c)		*/
	XG_LIST *	xgl_list;				/* history list	(xglist.c)		*/

	CONFIG_INFO	config_info;			/* config  info	(config.c)		*/
	DIFF_INFO	diff_info;				/* diff    info	(diffcmds.c)	*/
	HELP_INFO	help_info;				/* help    info	(help.c)		*/
	MENU_INFO	menu_info;				/* menu    info	(menu.c)		*/
	PRINT_INFO	print_info;				/* print   info	(print.c)		*/

	/*--------------------------------------------------------------------
	 * cmd-line options
	 */
	CMDOPTS		cmdopts;				/* cmd-line options				*/

	/*--------------------------------------------------------------------
	 * config options
	 */
	OPTIONS		options;				/* config options				*/

	/*--------------------------------------------------------------------
	 * xvt options
	 */
	XVT_DATA *	xvt_data;				/* xvt options					*/
	int			xvt_avail;				/* TRUE if xvt available		*/
};
typedef struct gblvars GBLVARS;

/*------------------------------------------------------------------------
 * global data pointer
 */
extern GBLVARS *	gblvars_ptr;

/*------------------------------------------------------------------------
 * map variable names into gblvars struct
 */
#define gbl(n)						(gblvars_ptr->n)

#define ai							(&gbl(arch_info))

#define cmdopt(n)					(gbl(cmdopts).n)
#define opt(n)						(gbl(options).n.opts[OPT_OPT])

#endif /* GBLVARS_H */
