/*------------------------------------------------------------------------
 * defaults table
 */
#include "libprd.h"

/*------------------------------------------------------------------------
 * max lengths for strings
 */
static const int pathlen	= MAX_PATHLEN;				/* max pathname len	*/
static const int filelen	= MAX_FILELEN;				/* max filename len	*/

/*------------------------------------------------------------------------
 * list of "yes" words
 */
static const DEFC yes_list[] =
{
	{ m_default_f_on,		m_default_m_on   },			/* "on"				*/
	{ m_default_f_yes,		m_default_m_yes  },			/* "yes"			*/
	{ m_default_f_true,		m_default_m_true },			/* "true"			*/
	{ 0, 0 }
};

/*------------------------------------------------------------------------
 * list of "no" words
 */
static const DEFC nos_list[] =
{
	{ m_default_f_off,		m_default_m_off   },		/* "off"			*/
	{ m_default_f_no,		m_default_m_no    },		/* "no"				*/
	{ m_default_f_false,	m_default_m_false },		/* "false"			*/
	{ 0, 0 }
};

/*------------------------------------------------------------------------
 * autolog option (autolog)
 */
static const DEFC autolog_list[] =
{
	{ m_default_auto_f_d,	m_default_auto_m_d	},		/* auto_disabled	*/
	{ m_default_auto_f_c,	m_default_auto_m_c	},		/* auto_check		*/
	{ m_default_auto_f_l,	m_default_auto_m_l	},		/* auto_log			*/
	{ 0, 0 }
};

/*------------------------------------------------------------------------
 * sort order (sort_order)
 */
static const DEFC sort_order_list[] =
{
	{ m_default_f_asc,		m_default_m_asc		},		/* sort_asc			*/
	{ m_default_f_desc,		m_default_m_desc	},		/* sort_dsc			*/
	{ 0, 0 }
};

/*------------------------------------------------------------------------
 * sort type (sort_type)
 */
static const DEFC sort_type_list[] =
{
	{ m_default_f_name,		m_default_m_name	},		/* sort_name		*/
	{ m_default_f_ext,		m_default_m_ext		},		/* sort_ext			*/
	{ m_default_f_date,		m_default_m_date	},		/* sort_date		*/
	{ m_default_f_inode,	m_default_m_inode	},		/* sort_inode		*/
	{ m_default_f_links,	m_default_m_links	},		/* sort_links		*/
	{ m_default_f_size,		m_default_m_size	},		/* sort_size		*/
	{ m_default_f_uns,		m_default_m_uns		},		/* sort_unsorted	*/
	{ 0, 0 }
};

/*------------------------------------------------------------------------
 * screen type (screen_type)
 */
static const DEFC screen_type_list[] =
{
	{ m_default_f_auto,		m_default_m_auto	},		/* screen_auto		*/
	{ m_default_f_color,	m_default_m_color	},		/* screen_color		*/
	{ m_default_f_mono,		m_default_m_mono	},		/* screen_mono		*/
	{ 0, 0 }
};

/*------------------------------------------------------------------------
 * file display format (file_fmt)
 */
static const DEFC file_disp_list[] =
{
	{ m_default_f_fname1,	m_default_m_fname1	},		/* fmt_fname1		*/
	{ m_default_f_fname3,	m_default_m_fname3	},		/* fmt_fname3		*/
	{ m_default_f_fsize,	m_default_m_fsize	},		/* fmt_fsize		*/
	{ m_default_f_fdate,	m_default_m_fdate	},		/* fmt_fdate		*/
	{ m_default_f_fperms,	m_default_m_fperms	},		/* fmt_fperms		*/
	{ 0, 0 }
};

/*------------------------------------------------------------------------
 * directory display format (dir_fmt)
 */
static const DEFC dir_disp_list[] =
{
	{ m_default_f_dname,	m_default_m_dname	},		/* fmt_dname		*/
	{ m_default_f_dsize,	m_default_m_dsize	},		/* fmt_dsize		*/
	{ m_default_f_ddate,	m_default_m_ddate	},		/* fmt_ddate		*/
	{ m_default_f_dperms,	m_default_m_dperms	},		/* fmt_dperms		*/
	{ 0, 0 }
};

/*------------------------------------------------------------------------
 * logging method (logging_method)
 */
static const DEFC logging_method_list[] =
{
	{ m_default_f_logm,		m_default_m_logm	},		/* log_manual		*/
	{ m_default_f_loga,		m_default_m_loga	},		/* log_auto			*/
	{ 0, 0 }
};

/*------------------------------------------------------------------------
 * time display format (time_fmt)
 */
static const DEFC time_fmt_list[] =
{
	{ m_default_f_time12,	m_default_m_time12	},		/* time_12			*/
	{ m_default_f_time24,	m_default_m_time24	},		/* time_24			*/
	{ 0, 0 }
};

/*------------------------------------------------------------------------
 * date display format (date_fmt)
 */
static const DEFC date_fmt_list[] =	/* list for date_fmt */
{
	{ m_default_f_datemdy,	m_default_m_datemdy	},		/* date_mdy			*/
	{ m_default_f_datedmy,	m_default_m_datedmy	},		/* date_dmy			*/
	{ m_default_f_dateymd,	m_default_m_dateymd	},		/* date_ymd			*/
	{ 0, 0 }
};

/*------------------------------------------------------------------------
 * compression type (comp_type)
 */
static const DEFC comp_type_list[] =
{
	{ m_default_f_complzw,	m_default_m_complzw		},	/* comp_lzw			*/
	{ m_default_f_compgzip,	m_default_m_compgzip	},	/* comp_gzip		*/
	{ m_default_f_comppack,	m_default_m_comppack	},	/* comp_pack		*/
	{ 0, 0 }
};

/*------------------------------------------------------------------------
 * cpio header format (cpio_hdr_fmt)
 */
static const DEFC cpio_hdr_list[] =
{
	{ m_default_f_cpiochr,	m_default_m_cpiochr	},		/* cpio_hdr_chr		*/
	{ m_default_f_cpiobin,	m_default_m_cpiobin	},		/* cpio_hdr_bin		*/
	{ m_default_f_cpiobbs,	m_default_m_cpiobbs	},		/* cpio_hdr_bbs		*/
	{ m_default_f_cpioasc,	m_default_m_cpioasc	},		/* cpio_hdr_asc		*/
	{ m_default_f_cpiocrc,	m_default_m_cpiocrc	},		/* cpio_hdr_crc		*/
	{ 0, 0 }
};

/*------------------------------------------------------------------------
 * tar header format (tar_hdr_fmt)
 */
static const DEFC tar_hdr_list[] =
{
	{ m_default_f_tarold,	m_default_m_tarold	},		/* tar_hdr_old		*/
	{ m_default_f_tarus,	m_default_m_tarus	},		/* tar_hdr_ustar	*/
	{ 0, 0 }
};

/*------------------------------------------------------------------------
 * viewer format (view_fmt)
 */
static const DEFC view_fmt_list[] =
{
	{ m_default_f_vwrauto,	m_default_m_vwrauto	},		/* view_fmt_auto	*/
	{ m_default_f_vwrasc,	m_default_m_vwrasc	},		/* view_fmt_ascii	*/
	{ m_default_f_vwrwrap,	m_default_m_vwrwrap	},		/* view_fmt_wrap	*/
	{ m_default_f_vwrhex,	m_default_m_vwrhex	},		/* view_fmt_hex		*/
	{ m_default_f_vwrdump,	m_default_m_vwrdump	},		/* view_fmt_dump	*/
	{ 0, 0 }
};

/*------------------------------------------------------------------------
 * date display type (date_type)
 */
static const DEFC date_type_list[] =
{
	{ m_default_date_f_m,	m_default_date_m_m	},		/* date_modified	*/
	{ m_default_date_f_r,	m_default_date_m_r	},		/* date_accessed	*/
	{ m_default_date_f_c,	m_default_date_m_c	},		/* date_created		*/
	{ 0, 0 }
};

/*------------------------------------------------------------------------
 * color fg names
 */
static const DEFC fg_list[] =
{
	{ m_default_fg_f_00,	m_default_fg_m_00	},
	{ m_default_fg_f_01,	m_default_fg_m_01	},
	{ m_default_fg_f_02,	m_default_fg_m_02	},
	{ m_default_fg_f_03,	m_default_fg_m_03	},
	{ m_default_fg_f_04,	m_default_fg_m_04	},
	{ m_default_fg_f_05,	m_default_fg_m_05	},
	{ m_default_fg_f_06,	m_default_fg_m_06	},
	{ m_default_fg_f_07,	m_default_fg_m_07	},
	{ m_default_fg_f_08,	m_default_fg_m_08	},
	{ m_default_fg_f_09,	m_default_fg_m_09	},
	{ m_default_fg_f_10,	m_default_fg_m_10	},
	{ m_default_fg_f_11,	m_default_fg_m_11	},
	{ m_default_fg_f_12,	m_default_fg_m_12	},
	{ m_default_fg_f_13,	m_default_fg_m_13	},
	{ m_default_fg_f_14,	m_default_fg_m_14	},
	{ m_default_fg_f_15,	m_default_fg_m_15	},
	{ 0, 0 }
};

/*------------------------------------------------------------------------
 * color bg names
 */
static const DEFC bg_list[] =
{
	{ m_default_bg_f_00,	m_default_bg_m_00	},
	{ m_default_bg_f_01,	m_default_bg_m_01	},
	{ m_default_bg_f_02,	m_default_bg_m_02	},
	{ m_default_bg_f_03,	m_default_bg_m_03	},
	{ m_default_bg_f_04,	m_default_bg_m_04	},
	{ m_default_bg_f_05,	m_default_bg_m_05	},
	{ m_default_bg_f_06,	m_default_bg_m_06	},
	{ m_default_bg_f_07,	m_default_bg_m_07	},
	{ m_default_bg_f_08,	m_default_bg_m_08	},
	{ m_default_bg_f_09,	m_default_bg_m_09	},
	{ m_default_bg_f_10,	m_default_bg_m_10	},
	{ m_default_bg_f_11,	m_default_bg_m_11	},
	{ m_default_bg_f_12,	m_default_bg_m_12	},
	{ m_default_bg_f_13,	m_default_bg_m_13	},
	{ m_default_bg_f_14,	m_default_bg_m_14	},
	{ m_default_bg_f_15,	m_default_bg_m_15	},
	{ 0, 0 }
};

/*------------------------------------------------------------------------
 * attr names
 */
static const DEFC attr_list[] =
{
	{ m_default_attr_f_0,	m_default_attr_m_0	},
	{ m_default_attr_f_1,	m_default_attr_m_1	},
	{ m_default_attr_f_2,	m_default_attr_m_2	},
	{ m_default_attr_f_3,	m_default_attr_m_3	},
	{ m_default_attr_f_4,	m_default_attr_m_4	},
	{ m_default_attr_f_5,	m_default_attr_m_5	},
	{ m_default_attr_f_6,	m_default_attr_m_6	},
	{ m_default_attr_f_7,	m_default_attr_m_7	},
	{ 0, 0 }
};

/*------------------------------------------------------------------------
 * list of defaults
 */
static const DEFS pgm_defs[] =
{
	/*--------------------------------------------------------------------
	 * Display
	 */
	{ DEF_TITLE, m_default_hdg_disp, 0, 0, 0, 0, 0, 0 },

	{
		DEF_ENUM,
		m_default_f_sm, m_default_m_sm, m_default_p_sm, m_default_v_sm,
		screen_type_list,
		opts_off(screen_type),
		cfg_screen
	},

	{
		DEF_NUM,
		m_default_f_sbi, m_default_m_sbi, m_default_p_sbi, m_default_v_sbi,
		0,
		opts_off(screen_blank_interval),
		0
	},

	{
		DEF_BOOL,
		m_default_f_uac, m_default_m_uac, m_default_p_uac, m_default_v_uac,
		0,
		opts_off(use_alt_char_sets),
		trm_alt_chars_ok
	},

	/*--------------------------------------------------------------------
	 * Window
	 */
	{ DEF_TITLE, m_default_hdg_win, 0, 0, 0, 0, 0, 0 },

	{
		DEF_BOOL,
		m_default_f_win, m_default_m_win, m_default_p_win, m_default_v_win,
		0,
		opts_off(run_in_window),
		0
	},

	{
		DEF_BOOL,
		m_default_f_ptr, m_default_m_ptr, m_default_p_ptr, m_default_v_ptr,
		0,
		opts_off(use_mouse),
		trm_use_mouse
	},

	{
		DEF_NUM,
		m_default_f_col, m_default_m_col, m_default_p_col, m_default_v_col,
		0,
		opts_off(columns),
		win_cols
	},

	{
		DEF_NUM,
		m_default_f_row, m_default_m_row, m_default_p_row, m_default_v_row,
		0,
		opts_off(rows),
		win_rows
	},

	{
		DEF_STR,
		m_default_f_rgf, m_default_m_rgf, m_default_p_rgf, m_default_v_rgf,
		0,
		opts_off(norm_font),
		0
	},

	{
		DEF_STR,
		m_default_f_bdf, m_default_m_bdf, m_default_p_bdf, m_default_v_bdf,
		0,
		opts_off(bold_font),
		0
	},

	{
		DEF_STR,
		m_default_f_xvt, m_default_m_xvt, m_default_p_xvt, m_default_v_xvt,
		0,
		opts_off(toolkit_opts),
		win_opts
	},

	/*--------------------------------------------------------------------
	 * Directory window
	 */
	{ DEF_TITLE, m_default_hdg_dir, 0, 0, 0, 0, 0, 0 },

	{
		DEF_ENUM,
		m_default_f_df, m_default_m_df, m_default_p_df, m_default_v_df,
		dir_disp_list,
		opts_off(dir_fmt),
		0
	},

	{
		DEF_BOOL,
		m_default_f_ds, m_default_m_ds, m_default_p_ds, m_default_v_ds,
		0,
		opts_off(dir_scroll),
		0
	},

	/*--------------------------------------------------------------------
	 * File window
	 */
	{ DEF_TITLE, m_default_hdg_file, 0, 0, 0, 0, 0, 0 },

	{
		DEF_ENUM,
		m_default_f_ff, m_default_m_ff, m_default_p_ff, m_default_v_ff,
		file_disp_list,
		opts_off(file_fmt),
		0
	},

	{
		DEF_ENUM,
		m_default_f_st, m_default_m_st, m_default_p_st, m_default_v_st,
		sort_type_list,
		opts_off(sort_type),
		0
	},

	{
		DEF_ENUM,
		m_default_f_so, m_default_m_so, m_default_p_so, m_default_v_so,
		sort_order_list,
		opts_off(sort_order),
		0
	},

	{
		DEF_BOOL,
		m_default_f_sbp, m_default_m_sbp, m_default_p_sbp, m_default_v_sbp,
		0,
		opts_off(sort_by_path),
		0
	},

	{
		DEF_BOOL,
		m_default_f_esw, m_default_m_esw, m_default_p_esw, m_default_v_esw,
		0,
		opts_off(do_small_window),
		0
	},

	{
		DEF_ENUM,
		m_default_f_alo, m_default_m_alo, m_default_p_alo, m_default_v_alo,
		autolog_list,
		opts_off(autolog),
		0
	},

	{
		DEF_BOOL,
		m_default_f_dfc, m_default_m_dfc, m_default_p_dfc, m_default_v_dfc,
		0,
		opts_off(do_file_colors),
		0
	},

	/*--------------------------------------------------------------------
	 * Compression
	 */
	{ DEF_TITLE, m_default_hdg_comp, 0, 0, 0, 0, 0, 0 },

	{
		DEF_ENUM,
		m_default_f_cpt, m_default_m_cpt, m_default_p_cpt, m_default_v_cpt,
		comp_type_list,
		opts_off(comp_type),
		0
	},

	/*--------------------------------------------------------------------
	 * Node logging
	 */
	{ DEF_TITLE, m_default_hdg_log, 0, 0, 0, 0, 0, 0 },

	{
		DEF_ENUM,
		m_default_f_lm, m_default_m_lm, m_default_p_lm, m_default_v_lm,
		logging_method_list,
		opts_off(logging_method),
		0
	},

	{
		DEF_STR,
		m_default_f_dn, m_default_m_dn, m_default_p_dn, m_default_v_dn,
		&pathlen,
		opts_off(default_node),
		0
	},

	{
		DEF_BOOL,
		m_default_f_htn, m_default_m_htn, m_default_p_htn, m_default_v_htn,
		0,
		opts_off(home_to_node),
		0
	},

	{
		DEF_BOOL,
		m_default_f_lfo, m_default_m_lfo, m_default_p_lfo, m_default_v_lfo,
		0,
		opts_off(log_network_files),
		0
	},

	{
		DEF_BOOL,
		m_default_f_sdl, m_default_m_sdl, m_default_p_sdl, m_default_v_sdl,
		0,
		opts_off(scroll_during_logging),
		0
	},

	{
		DEF_BOOL,
		m_default_f_cmp, m_default_m_cmp, m_default_p_cmp, m_default_v_cmp,
		0,
		opts_off(cont_at_mount_points),
		0
	},

	{
		DEF_BOOL,
		m_default_f_rsl, m_default_m_rsl, m_default_p_rsl, m_default_v_rsl,
		0,
		opts_off(resolve_sym_links),
		0
	},

	{
		DEF_NUM,
		m_default_f_ali, m_default_m_ali, m_default_p_ali, m_default_v_ali,
		0,
		opts_off(arch_log_interval),
		0
	},

	{
		DEF_BOOL,
		m_default_f_flg, m_default_m_flg, m_default_p_flg, m_default_v_flg,
		0,
		opts_off(ftp_logging),
		0
	},

	{
		DEF_BOOL,
		m_default_f_ftd, m_default_m_ftd, m_default_p_ftd, m_default_v_ftd,
		0,
		opts_off(ftp_debug),
		0
	},

	{
		DEF_NUM,
		m_default_f_fti, m_default_m_fti, m_default_p_fti, m_default_v_fti,
		0,
		opts_off(ftp_timeout),
		0
	},

	/*--------------------------------------------------------------------
	 * Security
	 */
	{ DEF_TITLE, m_default_hdg_sec, 0, 0, 0, 0, 0, 0 },

	{
		DEF_BOOL,
		m_default_f_ro, m_default_m_ro, m_default_p_ro, m_default_v_ro,
		0,
		opts_off(read_only),
		0
	},

	/*--------------------------------------------------------------------
	 * Miscellaneous 1
	 */
	{ DEF_TITLE, m_default_hdg_msc1, 0, 0, 0, 0, 0, 0 },

	{
		DEF_BOOL,
		m_default_f_boe, m_default_m_boe, m_default_p_boe, m_default_v_boe,
		0,
		opts_off(beep_on_error),
		0
	},

	{
		DEF_BOOL,
		m_default_f_kd, m_default_m_kd, m_default_p_kd, m_default_v_kd,
		0,
		opts_off(keep_date_in_copy),
		0
	},

	{
		DEF_BOOL,
		m_default_f_dc, m_default_m_dc, m_default_p_dc, m_default_v_dc,
		0,
		opts_off(display_clock),
		0
	},

	{
		DEF_BOOL,
		m_default_f_kfs, m_default_m_kfs, m_default_p_kfs, m_default_v_kfs,
		0,
		opts_off(keep_file_spec),
		0
	},

	{
		DEF_BOOL,
		m_default_f_pfq, m_default_m_pfq, m_default_p_pfq, m_default_v_pfq,
		0,
		opts_off(prompt_for_quit),
		0
	},

	{
		DEF_BOOL,
		m_default_f_bep, m_default_m_bep, m_default_p_bep, m_default_v_bep,
		0,
		opts_off(prompt_for_edit),
		0
	},

	{
		DEF_BOOL,
		m_default_f_pro, m_default_m_pro, m_default_p_pro, m_default_v_pro,
		0,
		opts_off(prompt_for_delete),
		0
	},

	{
		DEF_BOOL,
		m_default_f_hl, m_default_m_hl, m_default_p_hl, m_default_v_hl,
		0,
		opts_off(display_headline),
		0
	},

	{
		DEF_BOOL,
		m_default_f_ws, m_default_m_ws, m_default_p_ws, m_default_v_ws,
		0,
		opts_off(wide_screen),
		set_wide
	},

	{
		DEF_BOOL,
		m_default_f_stb, m_default_m_stb, m_default_p_stb, m_default_v_stb,
		0,
		opts_off(show_tag_blocks),
		0
	},

	{
		DEF_BOOL,
		m_default_f_eam, m_default_m_eam, m_default_p_eam, m_default_v_eam,
		0,
		opts_off(enter_app_menu),
		0
	},

	{
		DEF_BOOL,
		m_default_f_sft, m_default_m_sft, m_default_p_sft, m_default_v_sft,
		0,
		opts_off(shift_for_tag),
		0
	},

	{
		DEF_BOOL,
		m_default_f_sal, m_default_m_sal, m_default_p_sal, m_default_v_sal,
		0,
		opts_off(show_appmenu_lines),
		0
	},

	/*--------------------------------------------------------------------
	 * Miscellaneous 2
	 */
	{ DEF_TITLE, m_default_hdg_msc2, 0, 0, 0, 0, 0, 0 },

	{
		DEF_ENUM,
		m_default_f_dtp, m_default_m_dtp, m_default_p_dtp, m_default_v_dtp,
		date_type_list,
		opts_off(date_type),
		0
	},

	{
		DEF_STR,
#if V_WINDOWS
		m_default_f_edi, m_default_m_edi, m_default_p_edi, m_default_v_edi_w,
#else
		m_default_f_edi, m_default_m_edi, m_default_p_edi, m_default_v_edi_u,
#endif
		&pathlen,
		opts_off(editor),
		0
	},

	{
		DEF_STR,
		m_default_f_vwr, m_default_m_vwr, m_default_p_vwr, m_default_v_vwr,
		&pathlen,
		opts_off(viewer),
		0
	},

	{
		DEF_STR,
		m_default_f_hex, m_default_m_hex, m_default_p_hex, m_default_v_hex,
		&pathlen,
		opts_off(hex_editor),
		0
	},

	{
		DEF_NUM,
		m_default_f_mse, m_default_m_mse, m_default_p_mse, m_default_v_mse,
		0,
		opts_off(max_saved_entries),
		0
	},

	{
		DEF_BOOL,
		m_default_f_ins, m_default_m_ins, m_default_p_ins, m_default_v_ins,
		0,
		opts_off(insert_mode),
		0
	},

	{
		DEF_BOOL,
		m_default_f_ign, m_default_m_ign, m_default_p_ign, m_default_v_ign,
		0,
		opts_off(ignore_filename_case),
		0
	},

	{
		DEF_BOOL,
		m_default_f_isn, m_default_m_isn, m_default_p_isn, m_default_v_isn,
		0,
		opts_off(ignore_search_case),
		0
	},

	{
		DEF_BOOL,
		m_default_f_smd, m_default_m_smd, m_default_p_smd, m_default_v_smd,
		0,
		opts_off(show_mnt_dev),
		0
	},

	/*--------------------------------------------------------------------
	 * Archive
	 */
	{ DEF_TITLE, m_default_hdg_arch, 0, 0, 0, 0, 0, 0 },

	{
		DEF_ENUM,
		m_default_f_ch, m_default_m_ch, m_default_p_ch, m_default_v_ch,
		cpio_hdr_list,
		opts_off(cpio_hdr_fmt),
		0
	},

	{
		DEF_ENUM,
		m_default_f_th, m_default_m_th, m_default_p_th, m_default_v_th,
		tar_hdr_list,
		opts_off(tar_hdr_fmt),
		0
	},

	{
		DEF_BOOL,
		m_default_f_ibt, m_default_m_ibt, m_default_p_ibt, m_default_v_ibt,
		0,
		opts_off(arch_ignore_bad_types),
		0
	},

	/*--------------------------------------------------------------------
	 * Viewer
	 */
	{ DEF_TITLE, m_default_hdg_view, 0, 0, 0, 0, 0, 0 },

	{
		DEF_ENUM,
		m_default_f_vwm, m_default_m_vwm, m_default_p_vwm, m_default_v_vwm,
		view_fmt_list,
		opts_off(view_fmt),
		view_set_mode
	},

	{
		DEF_NUM,
		m_default_f_tw, m_default_m_tw, m_default_p_tw, m_default_v_tw,
		0,
		opts_off(tab_width),
		0
	},

	{
		DEF_BOOL,
		m_default_f_msk, m_default_m_msk, m_default_p_msk, m_default_v_msk,
		0,
		opts_off(display_mask),
		0
	},

	{
		DEF_BOOL,
		m_default_f_ecf, m_default_m_ecf, m_default_p_ecf, m_default_v_ecf,
		0,
		opts_off(exp_comp_files),
		0
	},

	/*--------------------------------------------------------------------
	 * International
	 */
	{ DEF_TITLE, m_default_hdg_intl, 0, 0, 0, 0, 0, 0 },

	{
		DEF_ENUM,
		m_default_f_tmf, m_default_m_tmf, m_default_p_tmf, m_default_v_tmf,
		time_fmt_list,
		opts_off(time_fmt),
		0
	},

	{
		DEF_ENUM,
		m_default_f_dtf, m_default_m_dtf, m_default_p_dtf, m_default_v_dtf,
		date_fmt_list,
		opts_off(date_fmt),
		0
	},

	{
		DEF_CHAR,
		m_default_f_tsc, m_default_m_tsc, m_default_p_tsc, m_default_v_tsc,
		0,
		opts_off(time_sep_char),
		0
	},

	{
		DEF_CHAR,
		m_default_f_dtc, m_default_m_dtc, m_default_p_dtc, m_default_v_dtc,
		0,
		opts_off(date_sep_char),
		0
	},

	{
		DEF_CHAR,
		m_default_f_nsc, m_default_m_nsc, m_default_p_nsc, m_default_v_nsc,
		0,
		opts_off(num_sep_char),
		0
	},

	{
		DEF_LIST,
		m_default_f_lang, m_default_m_lang, m_default_p_lang, m_default_v_lang,
		(void *)X_OFFSET_OF(GBLVARS,res_list),
		opts_off(language),
		cfg_lang
	},

	/*--------------------------------------------------------------------
	 * File compare
	 */
	{ DEF_TITLE, m_default_hdg_diff, 0, 0, 0, 0, 0, 0 },

	{
		DEF_STR,
		m_default_f_dfr, m_default_m_dfr, m_default_p_dfr, m_default_v_dfr,
		&pathlen,
		opts_off(diff_rtn),
		0
	},

	{
		DEF_BOOL,
		m_default_f_dgn, m_default_m_dgn, m_default_p_dgn, m_default_v_dgn,
		0,
		opts_off(diff_ignore_case),
		0
	},

	{
		DEF_BOOL,
		m_default_f_dws, m_default_m_dws, m_default_p_dws, m_default_v_dws,
		0,
		opts_off(diff_ignore_ws),
		0
	},

	{
		DEF_BOOL,
		m_default_f_dsn, m_default_m_dsn, m_default_p_dsn, m_default_v_dsn,
		0,
		opts_off(diff_show_numbers),
		0
	},

	{
		DEF_BOOL,
		m_default_f_dod, m_default_m_dod, m_default_p_dod, m_default_v_dod,
		0,
		opts_off(diff_show_diff),
		0
	},

	{
		DEF_NUM,
		m_default_f_dpw, m_default_m_dpw, m_default_p_dpw, m_default_v_dpw,
		0,
		opts_off(diff_page_width),
		0
	},

	{
		DEF_STR,
#if V_WINDOWS
		m_default_f_dpt, m_default_m_dpt, m_default_p_dpt, m_default_v_dpt_w,
#else
		m_default_f_dpt, m_default_m_dpt, m_default_p_dpt, m_default_v_dpt_u,
#endif
		&filelen,
		opts_off(diff_pattern),
		0
	},

	/*--------------------------------------------------------------------
	 * Printer
	 */
	{ DEF_TITLE, m_default_hdg_prt, 0, 0, 0, 0, 0, 0 },

	{
		DEF_STR,
		m_default_f_pri, m_default_m_pri, m_default_p_pri, m_default_v_pri,
		&pathlen,
		opts_off(printer),
		0
	},

	{
		DEF_STR,
		m_default_f_prf, m_default_m_prf, m_default_p_prf, m_default_v_prf,
		&pathlen,
		opts_off(print_filename),
		0
	},

	{
		DEF_NUM,
		m_default_f_ps, m_default_m_ps, m_default_p_ps, m_default_v_ps,
		0,
		opts_off(page_length),
		0
	},

	{
		DEF_NUM,
		m_default_f_pl, m_default_m_pl, m_default_p_pl, m_default_v_pl,
		0,
		opts_off(page_width),
		0
	},

	{
		DEF_NUM,
		m_default_f_pm, m_default_m_pm, m_default_p_pm, m_default_v_pm,
		0,
		opts_off(page_margin),
		0
	},

	{
		DEF_BOOL,
		m_default_f_cpe, m_default_m_cpe, m_default_p_cpe, m_default_v_cpe,
		0,
		opts_off(close_printer_at_exit),
		0
	},

	/*--------------------------------------------------------------------
	 * Regular window colors
	 */
	{ DEF_TITLE, m_default_hdg_c1, 0, 0, 0, 0, 0, 0 },

	{
		DEF_COLOR,
		m_default_f_rlc, m_default_m_rlc, m_default_p_rlc, m_default_v_rlc,
		0,
		opts_off(reg_lolite_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_rhc, m_default_m_rhc, m_default_p_rhc, m_default_v_rhc,
		0,
		opts_off(reg_hilite_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_rcc, m_default_m_rcc, m_default_p_rcc, m_default_v_rcc,
		0,
		opts_off(reg_cursor_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_rmc, m_default_m_rmc, m_default_p_rmc, m_default_v_rmc,
		0,
		opts_off(reg_menu_color),
		0
	},

	/*--------------------------------------------------------------------
	 * Showall window colors
	 */
	{ DEF_TITLE, m_default_hdg_c2, 0, 0, 0, 0, 0, 0 },

	{
		DEF_COLOR,
		m_default_f_slc, m_default_m_slc, m_default_p_slc, m_default_v_slc,
		0,
		opts_off(sa_lolite_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_shc, m_default_m_shc, m_default_p_shc, m_default_v_shc,
		0,
		opts_off(sa_hilite_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_scc, m_default_m_scc, m_default_p_scc, m_default_v_scc,
		0,
		opts_off(sa_cursor_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_smc, m_default_m_smc, m_default_p_smc, m_default_v_smc,
		0,
		opts_off(sa_menu_color),
		0
	},

	/*--------------------------------------------------------------------
	 * Autoview window colors
	 */
	{ DEF_TITLE, m_default_hdg_c3, 0, 0, 0, 0, 0, 0 },

	{
		DEF_COLOR,
		m_default_f_alc, m_default_m_alc, m_default_p_alc, m_default_v_alc,
		0,
		opts_off(av_lolite_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_ahc, m_default_m_ahc, m_default_p_ahc, m_default_v_ahc,
		0,
		opts_off(av_hilite_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_acc, m_default_m_acc, m_default_p_acc, m_default_v_acc,
		0,
		opts_off(av_cursor_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_amc, m_default_m_amc, m_default_p_amc, m_default_v_amc,
		0,
		opts_off(av_menu_color),
		0
	},

	/*--------------------------------------------------------------------
	 * Diff colors
	 */
	{ DEF_TITLE, m_default_hdg_c4, 0, 0, 0, 0, 0, 0 },

	{
		DEF_COLOR,
		m_default_f_dsc, m_default_m_dsc, m_default_p_dsc, m_default_v_dsc,
		0,
		opts_off(diff_same_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_ddc, m_default_m_ddc, m_default_p_ddc, m_default_v_ddc,
		0,
		opts_off(diff_diff_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_dlc, m_default_m_dlc, m_default_p_dlc, m_default_v_dlc,
		0,
		opts_off(diff_lfrt_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_dnc, m_default_m_dnc, m_default_p_dnc, m_default_v_dnc,
		0,
		opts_off(diff_numb_color),
		0
	},

	/*--------------------------------------------------------------------
	 * Help system colors
	 */
	{ DEF_TITLE, m_default_hdg_c5, 0, 0, 0, 0, 0, 0 },

	{
		DEF_COLOR,
		m_default_f_hblc, m_default_m_hblc, m_default_p_hblc, m_default_v_hblc,
		0,
		opts_off(help_box_lolite_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_hbhc, m_default_m_hbhc, m_default_p_hbhc, m_default_v_hbhc,
		0,
		opts_off(help_box_hilite_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_hbbc, m_default_m_hbbc, m_default_p_hbbc, m_default_v_hbbc,
		0,
		opts_off(help_box_bold_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_hhlc, m_default_m_hhlc, m_default_p_hhlc, m_default_v_hhlc,
		0,
		opts_off(help_box_hlink_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_gbc, m_default_m_gbc, m_default_p_gbc, m_default_v_gbc,
		0,
		opts_off(gloss_box_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_hglc, m_default_m_hglc, m_default_p_hglc, m_default_v_hglc,
		0,
		opts_off(help_box_glink_color),
		0
	},

	/*--------------------------------------------------------------------
	 * Menu colors
	 */
	{ DEF_TITLE, m_default_hdg_c6, 0, 0, 0, 0, 0, 0 },

	{
		DEF_COLOR,
		m_default_f_mlc, m_default_m_mlc, m_default_p_mlc, m_default_v_mlc,
		0,
		opts_off(menu_lolite_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_mhc, m_default_m_mhc, m_default_p_mhc, m_default_v_mhc,
		0,
		opts_off(menu_hilite_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_mcc, m_default_m_mcc, m_default_p_mcc, m_default_v_mcc,
		0,
		opts_off(menu_cursor_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_mbc, m_default_m_mbc, m_default_p_mbc, m_default_v_mbc,
		0,
		opts_off(menu_border_color),
		0
	},

	/*--------------------------------------------------------------------
	 * Miscellaneous colors
	 */
	{ DEF_TITLE, m_default_hdg_c7, 0, 0, 0, 0, 0, 0 },

	{
		DEF_COLOR,
		m_default_f_bc, m_default_m_bc, m_default_p_bc, m_default_v_bc,
		0,
		opts_off(border_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_cc, m_default_m_cc, m_default_p_cc, m_default_v_cc,
		0,
		opts_off(clock_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_ec, m_default_m_ec, m_default_p_ec, m_default_v_ec,
		0,
		opts_off(error_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_msc, m_default_m_msc, m_default_p_msc, m_default_v_msc,
		0,
		opts_off(mouse_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_mmc, m_default_m_mmc, m_default_p_mmc, m_default_v_mmc,
		0,
		opts_off(mouse_cmd_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_escc, m_default_m_escc, m_default_p_escc, m_default_v_escc,
		0,
		opts_off(escape_color),
		0
	},

	{
		DEF_COLOR,
		m_default_f_xc, m_default_m_xc, m_default_p_xc, m_default_v_xc,
		0,
		opts_off(exit_color),
		0
	},

	/*--------------------------------------------------------------------
	 * Regular window attributes
	 */
	{ DEF_TITLE, m_default_hdg_m1, 0, 0, 0, 0, 0, 0 },

	{
		DEF_MONO,
		m_default_f_rlm, m_default_m_rlm, m_default_p_rlm, m_default_v_rlm,
		0,
		opts_off(reg_lolite_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_rhm, m_default_m_rhm, m_default_p_rhm, m_default_v_rhm,
		0,
		opts_off(reg_hilite_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_rcm, m_default_m_rcm, m_default_p_rcm, m_default_v_rcm,
		0,
		opts_off(reg_cursor_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_rmm, m_default_m_rmm, m_default_p_rmm, m_default_v_rmm,
		0,
		opts_off(reg_menu_mono),
		0
	},

	/*--------------------------------------------------------------------
	 * Showall window attributes
	 */
	{ DEF_TITLE, m_default_hdg_m2, 0, 0, 0, 0, 0, 0 },

	{
		DEF_MONO,
		m_default_f_slm, m_default_m_slm, m_default_p_slm, m_default_v_slm,
		0,
		opts_off(sa_lolite_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_shm, m_default_m_shm, m_default_p_shm, m_default_v_shm,
		0,
		opts_off(sa_hilite_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_scm, m_default_m_scm, m_default_p_scm, m_default_v_scm,
		0,
		opts_off(sa_cursor_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_smm, m_default_m_smm, m_default_p_smm, m_default_v_smm,
		0,
		opts_off(sa_menu_mono),
		0
	},

	/*--------------------------------------------------------------------
	 * Autoview window attributes
	 */
	{ DEF_TITLE, m_default_hdg_m3, 0, 0, 0, 0, 0, 0 },

	{
		DEF_MONO,
		m_default_f_alm, m_default_m_alm, m_default_p_alm, m_default_v_alm,
		0,
		opts_off(av_lolite_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_ahm, m_default_m_ahm, m_default_p_ahm, m_default_v_ahm,
		0,
		opts_off(av_hilite_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_acm, m_default_m_acm, m_default_p_acm, m_default_v_acm,
		0,
		opts_off(av_cursor_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_amm, m_default_m_amm, m_default_p_amm, m_default_v_amm,
		0,
		opts_off(av_menu_mono),
		0
	},

	/*--------------------------------------------------------------------
	 * Diff attributes
	 */
	{ DEF_TITLE, m_default_hdg_m4, 0, 0, 0, 0, 0, 0 },

	{
		DEF_MONO,
		m_default_f_dsm, m_default_m_dsm, m_default_p_dsm, m_default_v_dsm,
		0,
		opts_off(diff_same_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_ddm, m_default_m_ddm, m_default_p_ddm, m_default_v_ddm,
		0,
		opts_off(diff_diff_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_dlm, m_default_m_dlm, m_default_p_dlm, m_default_v_dlm,
		0,
		opts_off(diff_lfrt_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_dnm, m_default_m_dnm, m_default_p_dnm, m_default_v_dnm,
		0,
		opts_off(diff_numb_mono),
		0
	},

	/*--------------------------------------------------------------------
	 * Help system attributes
	 */
	{ DEF_TITLE, m_default_hdg_m5, 0, 0, 0, 0, 0, 0 },

	{
		DEF_MONO,
		m_default_f_hblm, m_default_m_hblm, m_default_p_hblm, m_default_v_hblm,
		0,
		opts_off(help_box_lolite_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_hbhm, m_default_m_hbhm, m_default_p_hbhm, m_default_v_hbhm,
		0,
		opts_off(help_box_hilite_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_hbbm, m_default_m_hbbm, m_default_p_hbbm, m_default_v_hbbm,
		0,
		opts_off(help_box_bold_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_hhlm, m_default_m_hhlm, m_default_p_hhlm, m_default_v_hhlm,
		0,
		opts_off(help_box_hlink_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_gbm, m_default_m_gbm, m_default_p_gbm, m_default_v_gbm,
		0,
		opts_off(gloss_box_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_hglm, m_default_m_hglm, m_default_p_hglm, m_default_v_hglm,
		0,
		opts_off(help_box_glink_mono),
		0
	},

	/*--------------------------------------------------------------------
	 * Menu attributes
	 */
	{ DEF_TITLE, m_default_hdg_m6, 0, 0, 0, 0, 0, 0 },

	{
		DEF_MONO,
		m_default_f_mlm, m_default_m_mlm, m_default_p_mlm, m_default_v_mlm,
		0,
		opts_off(menu_lolite_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_mhm, m_default_m_mhm, m_default_p_mhm, m_default_v_mhm,
		0,
		opts_off(menu_hilite_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_mcm, m_default_m_mcm, m_default_p_mcm, m_default_v_mcm,
		0,
		opts_off(menu_cursor_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_mbm, m_default_m_mbm, m_default_p_mbm, m_default_v_mbm,
		0,
		opts_off(menu_border_mono),
		0
	},

	/*--------------------------------------------------------------------
	 * Miscellaneous attributes
	 */
	{ DEF_TITLE, m_default_hdg_m7, 0, 0, 0, 0, 0, 0 },

	{
		DEF_MONO,
		m_default_f_bm, m_default_m_bm, m_default_p_bm, m_default_v_bm,
		0,
		opts_off(border_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_cm, m_default_m_cm, m_default_p_cm, m_default_v_cm,
		0,
		opts_off(clock_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_em, m_default_m_em, m_default_p_em, m_default_v_em,
		0,
		opts_off(error_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_msm, m_default_m_msm, m_default_p_msm, m_default_v_msm,
		0,
		opts_off(mouse_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_mmm, m_default_m_mmm, m_default_p_mmm, m_default_v_mmm,
		0,
		opts_off(mouse_cmd_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_escm, m_default_m_escm, m_default_p_escm, m_default_v_escm,
		0,
		opts_off(escape_mono),
		0
	},

	{
		DEF_MONO,
		m_default_f_xm, m_default_m_xm, m_default_p_xm, m_default_v_xm,
		0,
		opts_off(exit_mono),
		0
	},

	/*--------------------------------------------------------------------
	 * end-of-list
	 */
	{ 0, 0, 0, 0, 0, 0, 0, 0 }
};

/*------------------------------------------------------------------------
 * routine to return a message string
 */
static const char * dflt_msg_rtn (int n)
{
	return dflt(n);
}

/*------------------------------------------------------------------------
 * defaults table
 */
static const DEFT dflt_table =
{
	dflt_msg_rtn,						/* msg routine */
	yes_list,							/* "yes" list */
	nos_list,							/* "nos" list */
	fg_list,							/* fg names */
	bg_list,							/* bg names */
	attr_list,							/* attr names */
	pgm_defs,							/* pointer to defaults table */
	(void **)&gblvars_ptr,				/* pointer to data struct */
	X_OFFSET_OF(GBLVARS, options)		/* offset in struct to options tbl */
};

/*------------------------------------------------------------------------
 * defaults table pointer
 */
const DEFT * dflt_tbl = &dflt_table;
