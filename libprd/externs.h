/*------------------------------------------------------------------------
 * function definitions
 */
#ifndef EXTERNS_H
#define EXTERNS_H

/*------------------------------------------------------------------------
 * ablk.c
 */
extern ABLK *	ablk_make (void);
extern void		ablk_free (ABLK *a);

/*------------------------------------------------------------------------
 * about.c
 */
extern int		do_about (int key, void *data);

/*------------------------------------------------------------------------
 * altcmd.c
 */
extern void		do_file_disp (void);
extern void		set_file_disp (void);
extern void		do_dir_disp (void);
extern void		set_dir_disp (void);

/*------------------------------------------------------------------------
 * arcdcmds.c
 */
extern int		do_reg_dir_arc (int c);
extern int		do_tag_dir_arc (int c);
extern int		do_alt_dir_arc (int c);

/*------------------------------------------------------------------------
 * arcfcmds.c
 */
extern int		do_reg_file_arc (int c);
extern int		do_tag_file_arc (int c);
extern int		do_alt_file_arc (int c);

/*------------------------------------------------------------------------
 * archive.c
 */
extern void		log_archive (void);
extern void		disp_arch_dirs (TREE *dir);

/*------------------------------------------------------------------------
 * arcutils.c
 */
extern int		arch_open (void);
extern int		arch_close (void);
extern void		do_arch_open (void);
extern void		do_extract (void);
extern void		do_tag_extract (void);
extern void		arc_print (void);
extern void		arc_view (void);
extern int		get_arch_device (const char *cur_working_dir, int out);
extern int		get_arch_type (void);
extern TREE *	add_phony_dblk_to_node (const char *path, NBLK *n,
					int *newdir);
extern DBLK *	make_phony_dblk (const char *name);
extern FBLK *	make_phony_fblk (const char *name);
extern void		set_top_file (void);
extern int		set_arch_buf (void);
extern int		is_arch_path_rel (FBLK *f);
extern int		get_arch_name (void);
extern int		determine_arch_type (void);
extern void		arch_load_dev_tbl (void);
extern void		arch_free_dev_tbl (void);

/*------------------------------------------------------------------------
 * args.c
 */
extern int		check_opts (void);
extern int		check_args (char *node_to_log);

/*------------------------------------------------------------------------
 * attr.c
 */
extern void		do_file_attr (void);
extern void		do_tag_file_attr (void);
extern void		do_dir_attr (void);
extern int		attr_get_file (const char *item, struct stat *pstbuf);
extern int		get_perms (int tag);

/*------------------------------------------------------------------------
 * autoview.c
 */
extern void		autoview (void);
extern void		end_av (void);

/*------------------------------------------------------------------------
 * avail.c
 */
extern void		do_available (void);

/*------------------------------------------------------------------------
 * avcmds.c
 */
extern int		do_av_cmd (int c);
extern int		do_reg_av_cmd (int c);
extern int		do_tag_av_cmd (int c);
extern int		do_alt_av_cmd (int c);
extern int		do_reg_fv_cmd (int c);

/*------------------------------------------------------------------------
 * avcmds2.c
 */
extern void		open_av_file (FBLK *f);
extern void		close_av_file (void);
extern void		display_av (void);
extern void		disp_av_pct (int ref);
extern void		disp_av_mode (void);
extern int		get_disp_mode (void);

/*------------------------------------------------------------------------
 * backup.c
 */
extern void		do_backup (void);
extern void		do_tag_backup (void);

/*------------------------------------------------------------------------
 * bang.c
 */
extern void		bang (const char *string);
extern void		bangnc (const char *string);
extern void		bang_save (chtype *save);
extern void		bang_restore (chtype *save);
extern int		anykey (void);
extern int		wanykey (WINDOW *win);
extern int		anykey_msg (const char *mesg);
extern int		yesno (void);
extern int		wyesno (WINDOW *win);
extern int		yesno_msg (const char *mesg);
extern void		do_beep (void);
extern void		dest_msg (int code);
extern int		check_read_only (const char *cmd);
extern int		check_tag_count (void);
extern void		esc_msg (void);
extern void		ret_ok (void);
extern void		ret_msg (WINDOW *win);
extern void		left_arrow (WINDOW *win);
extern void		position_cursor (void);
extern void		fk_msg (WINDOW *win, int code, const char *mesg);
extern char *	fk_msg_str (int code, const char *mesg, char *buf);

/*------------------------------------------------------------------------
 * bar.c
 */
extern int		bar_open (void);
extern int		bar_close (int eject);
extern int		bar_read_rec (void);
extern int		bar_write_rec (void);
extern int		bar_wr_vol_hdr (long size);
extern int		bar_wr_hdr (FBLK *f, char *rel_name);
extern int		bar_wr_link_hdr (FBLK *f, FBLK *lf, char *rel_name);
extern int		bar_wr_sym_hdr (FBLK *f, char *l, char *rel_name);
extern int		bar_wr_tlr (void);
extern void		bar_get_stbuf (struct stat *s);
extern int		bar_skip_file (void);
extern int		bar_is_valid_hdr (void);
extern int		bar_is_hdr (union bblock *h);
extern int		bar_position (const char *n, int v, off_t p);
extern int		bar_ask_for_volno (const char *n, int v);
extern char *	bar_filename (void);
extern char *	bar_linkname (void);

/*------------------------------------------------------------------------
 * batch.c
 */
extern void		batch (void);
extern void		expand_cmd (const char *inp_str, char *out_str);

/*------------------------------------------------------------------------
 * border.c
 */
extern void		main_border (void);
extern void		split_main_border (void);
extern void		small_border (int on_off);
extern void		dest_border (void);
extern void		fv_border (void);
extern void		av_border (void);
extern void		split_av_border (void);
extern void		config_border (void);
extern void		diff_border (void);

/*------------------------------------------------------------------------
 * chkdir.c
 */
extern int		chk_dir_in_tree (const char *path);

/*------------------------------------------------------------------------
 * chkfile.c
 */
extern void		check_the_file (const char *pathname);
extern void		check_for_file (const char *file_name, const char *dir_name);
extern void		add_file_to_dir (const char *file_name, const char *dir_name,
					TREE *dir, int flag);

/*------------------------------------------------------------------------
 * cmncmds.c
 */
extern void		do_cmn_reg_dir_invert    (void);
extern void		do_cmn_reg_dir_tag       (void);
extern void		do_cmn_reg_dir_untag     (void);
extern void		do_cmn_reg_dir_global    (void);
extern void		do_cmn_reg_dir_showall   (void);

extern void		do_cmn_tag_dir_invert    (void);
extern void		do_cmn_tag_dir_tag       (void);
extern void		do_cmn_tag_dir_untag     (void);
extern void		do_cmn_tag_dir_global    (void);
extern void		do_cmn_tag_dir_showall   (void);

extern void		do_cmn_alt_dir_invert    (void);
extern void		do_cmn_alt_dir_tag       (void);
extern void		do_cmn_alt_dir_untag     (void);
extern void		do_cmn_alt_dir_sort      (void);

extern void		do_cmn_reg_file_invert   (void);
extern void		do_cmn_reg_file_tag      (void);
extern void		do_cmn_reg_file_untag    (void);

extern void		do_cmn_tag_file_invert   (void);
extern void		do_cmn_tag_file_tag      (void);
extern void		do_cmn_tag_file_untag    (void);

extern void		do_cmn_alt_file_invert   (void);
extern void		do_cmn_alt_file_tag      (void);
extern void		do_cmn_alt_file_untag    (void);
extern void		do_cmn_alt_file_copytags (void);
extern void		do_cmn_alt_file_sort     (void);

extern void		do_cmn_next_node         (void);
extern void		do_cmn_prev_node         (void);

/*------------------------------------------------------------------------
 * cmndcmds.c
 */
extern int		do_dir_cmd (int c);

/*------------------------------------------------------------------------
 * cmnfcmds.c
 */
extern int		do_file_cmd (int c);

/*------------------------------------------------------------------------
 * cmp.c
 */
extern void		do_cmp_dirs (void);
extern void		do_comp_files (void);

/*------------------------------------------------------------------------
 * comp.c
 */
extern void		toggle_compress_cur_file (void);
extern int		x_is_file_compressed (const char *filename);
extern void		x_make_compressed_name   (char *filename, int type);
extern void		x_make_uncompressed_name (char *filename, int type);
extern int		x_comp   (const char *old_name, const char *new_name,
					int type);
extern int		x_decomp (const char *old_name, const char *new_name,
					int type);

/*------------------------------------------------------------------------
 * compact.c
 */
extern void		compact (void);

/*------------------------------------------------------------------------
 * compare.c
 */
extern int		compare_files (const void *a, const void *b);
extern int		compare_dirs  (const void *a, const void *b);
extern int		cmp_archloc   (const void *a, const void *b);

/*------------------------------------------------------------------------
 * config.c
 */
extern void		config (void);
extern void		cfg_redraw (void);
extern int		cfg_lang (const void *valp);
extern int		cfg_screen (const void *valp);

/*------------------------------------------------------------------------
 * copy.c
 */
extern void		do_link_dir (void);
extern void		do_link_file (void);
extern void		do_copy (void);
extern int		chk_dir_and_make (const char *path);
extern void		disp_new_tree_if_needed (void);
extern void		do_tag_link (void);
extern void		do_tag_copy (void);
extern void		do_alt_copy (void);
extern int		filecopy (const char *old_name, const char *new_name,
					int mode);

/*------------------------------------------------------------------------
 * cpio.c
 */
extern int		cpio_open (void);
extern int		cpio_close (int eject);
extern int		cpio_read_rec (char *buf, int len);
extern int		cpio_write_rec (const char *buf, int len);
extern int		cpio_wr_hdr (FBLK *f, char *rel_name);
extern int		cpio_wr_tlr (void);
extern int		cpio_rd_hdr (void);
extern void		cpio_get_stbuf (struct stat *s);
extern int		cpio_skip_file (void);
extern int		cpio_position (const char *n, int v, off_t p);
extern char *	cpio_filename (void);

/*------------------------------------------------------------------------
 * credits.c
 */
extern char *	package_name (void);
extern char *	ver_line (char *line);
extern void		headline (void);
extern void		disp_credits (void);
extern void		disp_credits_below (void);
extern void		disp_run_stats (void);

/*------------------------------------------------------------------------
 * crnl.c
 */
extern void		dos_to_unix (void);
extern void		unix_to_dos (void);

/*------------------------------------------------------------------------
 * date.c
 */
extern char *	date_to_str (char *dl, time_t time);
extern char *	date_2_str (char *dl, time_t time);
extern char *	tm_to_str (char *dl,
					int yr, int mo, int dy, int hh, int mm, int ss);
extern int		cdate (char *string, int *year, int *month, int *day,
					int *hour, int *mn, int *sec);

/*------------------------------------------------------------------------
 * dblk.c
 */
extern DBLK *	dblk_make (void);
extern void		dblk_free (DBLK *d);
extern void		dblk_set_name (DBLK *d, const char *name);
extern void		dblk_change_name (DBLK *d, const char *name);

/*------------------------------------------------------------------------
 * default.c
 */
extern void		setup_defaults (const char *lang);
extern void		check_defaults (const char *initfile);
extern int		disp_defaults (int restore);

/*------------------------------------------------------------------------
 * delete.c
 */
extern int		do_delete (void);
extern void		do_del_files (void);
extern int		remove_file (FBLK *f, int for_real);

/*------------------------------------------------------------------------
 * dest.c
 */
extern int		do_dest_dir (char *path, int show_opt, TREE **dtptr);

/*------------------------------------------------------------------------
 * diff.c
 */
extern void		diff (void);

/*------------------------------------------------------------------------
 * diffcmds.c
 */
extern void		diff_display_init (void);
extern void		diff_view (const char *path1, const char *path2,
					DIFF_LINE *dl);
extern void		diff_view_setup (void);
extern int		do_diff_cmd (int c);

/*------------------------------------------------------------------------
 * diffprt.c
 */
extern void		diff_print (DIFF_LINE *lines);

/*------------------------------------------------------------------------
 * dircmd2.c
 */
extern int		do_reg_dir_cmd (int c);
extern int		do_tag_dir_cmd (int c);
extern int		do_alt_dir_cmd (int c);

/*------------------------------------------------------------------------
 * dirrtns.c
 */
extern void		do_make_dir (void);
extern void		set_top_dir (void);
extern TREE *	add_dir (const char *name, const char *fullpath, TREE *p);
extern void		fix_dir_list (NBLK *n);
extern void		add_dir_to_parent (TREE *t, TREE *p);
extern void		do_delete_dir (void);
extern void		del_dir_from_node (BLIST *b, NBLK *n);
extern void		do_rename_dir (void);
extern int		make_dir_path (const char *path);

/*------------------------------------------------------------------------
 * dispcmds.c
 */
extern void		disp_cmds (void);

/*------------------------------------------------------------------------
 * dispdir.c
 */
extern int		get_max_dir_level (WINDOW *win);
extern void		disp_dir_line (BLIST *nodeptr);
extern void		disp_dir_tree (void);
extern void		hilite_dir (int on_off);
extern void		hilite_the_dir (int on_off, BLIST *dptr, int line);
extern void		dir_scroll_bar_init (void);
extern void		dir_scroll_bar_remove (void);
extern void		dir_scroll_bar_off (void);
extern void		do_dir_scroll_bar (void);

/*------------------------------------------------------------------------
 * dispfile.c
 */
extern void		disp_file_line (FBLK *f);
extern void		disp_file_list (void);
extern void		hilite_file (int on_off);
extern char *	perm_str (int mode, char *buf);
extern void		check_small_window (void);
extern void		start_file_display (void);
extern void		file_scroll_bar_init (void);
extern void		file_scroll_bar_remove (void);
extern void		do_file_scroll_bar (void);

/*------------------------------------------------------------------------
 * edit.c
 */
extern void		do_hexedit (FBLK *f);
extern void		do_editor (FBLK *f);
extern void		do_viewer (FBLK *f);
extern void		view_file (const char *filename);
extern void		do_tag_viewer (void);

/*------------------------------------------------------------------------
 * errmsg.c
 */
extern int		errsys (int str1);
extern int		errmsg (int str1, const char *str2, int mode);
extern int		errmsgi (int str1, const char *str2, int mode);
extern int		err_message (const char *str1, const char *str2, int mode);

/*------------------------------------------------------------------------
 * extbar.c
 */
extern void		do_bar_extract (void);
extern void		do_tag_bar_extract (void);
extern int		bar_extract_file (const char *path);
extern int		bar_extract_temp (FBLK *f, const char *name);

/*------------------------------------------------------------------------
 * extcpio.c
 */
extern void		do_cpio_extract (void);
extern void		do_tag_cpio_extract (void);
extern int		cpio_extract_file (const char *path);
extern int		cpio_extract_temp (FBLK *f, const char *name);

/*------------------------------------------------------------------------
 * extract.c
 */
extern BLIST *	make_ext_list (void);
extern BLIST *	disp_ext_list (BLIST *taglist);
extern void		restore_saved_first_file (BLIST *save_first_file);
extern int		ext_ask (int rel_flag);
extern long		cvt_from_dec (const char *string, int len);
extern void		cvt_to_dec (long num, char *string, int len);
extern void		cvt_to_hex (long num, char *string, int len);
extern long		cvt_from_hex (const char *string, int len);
extern char *	get_name (FBLK *f, char *rel_name, char *path);
extern long		cvt_from_oct (const char *string, int len);
extern void		cvt_to_oct (long num, char *string, int len, int fmt);

/*------------------------------------------------------------------------
 * exttar.c
 */
extern void		do_tar_extract (void);
extern void		do_tag_tar_extract (void);
extern int		tar_extract_file (const char *path);
extern int		tar_extract_temp (FBLK *f, const char *name);

/*------------------------------------------------------------------------
 * extztar.c
 */
extern void		do_ztar_extract (void);
extern void		do_tag_ztar_extract (void);
extern int		ztar_extract_file (const char *path);
extern int		ztar_extract_temp (FBLK *f, const char *name);

/*------------------------------------------------------------------------
 * falist.c
 */
extern attr_t	get_file_attr (WINDOW *win, FBLK *f);
extern void		fa_list_load (int *errs);
extern void		fa_list_dump (void);
extern void		fa_list_free (void);

/*------------------------------------------------------------------------
 * fblk.c
 */
extern FBLK *	fblk_make (void);
extern void		fblk_free (FBLK *f);
extern void		fblk_set_name (FBLK *f, const char *name);
extern void		fblk_change_name (FBLK *f, const char *name);

/*------------------------------------------------------------------------
 * fileattr.c
 */
extern attr_t	fileattr_attr (BLIST *flist, FBLK *f);

/*------------------------------------------------------------------------
 * filecmd2.c
 */
extern int		do_reg_file_cmd (int c);
extern int		do_tag_file_cmd (int c);
extern int		do_alt_file_cmd (int c);

/*------------------------------------------------------------------------
 * filespec.c
 */
extern void		do_file_spec (void);
extern void		locate_cur_file (void);
extern void		rm_file_spec (void);
extern BLIST *	get_mlist (BLIST *flist, const char *pattern,
					int *count, int *bytes);
extern void		set_first_file (int n);

/*------------------------------------------------------------------------
 * ftpcopy.c
 */
extern void		ftp_do_copy (void);
extern void		ftp_do_tag_copy (void);
extern void		ftp_do_alt_copy (void);

/*------------------------------------------------------------------------
 * ftpdir.c
 */
extern void		ftp_do_make_dir (void);
extern void		ftp_do_delete_dir (void);

/*------------------------------------------------------------------------
 * ftpfile.c
 */
extern int		ftp_do_delete (void);
extern int		ftp_do_tag_delete (void);
extern int		ftp_do_rename (void);
extern int		ftp_do_tag_rename (void);

/*------------------------------------------------------------------------
 * ftpdcmds.c
 */
extern int		do_reg_dir_ftp (int c);
extern int		do_tag_dir_ftp (int c);
extern int		do_alt_dir_ftp (int c);

/*------------------------------------------------------------------------
 * ftpfcmds.c
 */
extern int		do_reg_file_ftp (int c);
extern int		do_tag_file_ftp (int c);
extern int		do_alt_file_ftp (int c);

/*------------------------------------------------------------------------
 * ftplog.c
 */
extern void		relog_ftp_dir (int recurse);

/*------------------------------------------------------------------------
 * ftpmisc.c
 */
extern void		ftp_do_print (void);
extern void		ftp_do_tag_print (void);
extern void		ftp_do_view (void);
extern int		ftp_do_file_attr (void);
extern void		ftp_do_tag_file_attr (void);
extern int		ftp_resolve_symlink (void);
extern int		ftp_get_syminfo (FTP_NODE *fn, const char *path, char *sympath,
					struct stat *stbuf);
extern TREE *	ftp_add_dir (const char *filename, struct stat *stbuf,
					TREE *p);

/*------------------------------------------------------------------------
 * ftpxnode.c
 */
extern int		close_ftp_node (NBLK *n);
extern int		delete_ftp_node (NBLK *n);
extern NBLK *	init_ftp_node (char *host, char *user, char *passwd,
					void (*logmsg)(const char *msg),
					int (*errmsg)(const char *msg));

/*------------------------------------------------------------------------
 * ftpxutil.c
 */
extern char *	ftp_host (NBLK *n);
extern char *	ftp_addr (NBLK *n);
extern int		ftp_get_type (NBLK *n);
extern void		ftp_setup_node (char *host, char *user, char *passwd,
					char *rdir);
extern int		ftp_stat (FTP_NODE *fn, const char *file_name,
					const char *dir_name, struct stat *stbuf);
extern DBLK *	make_phony_ftp_dblk (const char *name);
extern DBLK *	make_ftp_dblk (const char *name, struct stat *stbuf);
extern FBLK *	make_ftp_fblk (const char *name, struct stat *stbuf);
extern void		make_ftp_xstat (struct stat *stbuf, struct stat *xbuf);
extern char *	get_ftp_owner (NBLK *n, int uid);
extern char *	get_ftp_group (NBLK *n, int gid);

/*------------------------------------------------------------------------
 * futils.c
 */
extern int		can_we_read  (struct stat *stbuf);
extern int		can_we_write (struct stat *stbuf);
extern int		can_we_exec  (struct stat *stbuf);

/*------------------------------------------------------------------------
 * getft.c
 */
extern int		getft (NBLK *n, struct stat *pstbuf);
extern DBLK *	fs_make_dblk (const char *pathname, struct stat *pstbuf);
extern FBLK *	fs_make_fblk (const char *name, struct stat *pstbuf);
extern TREE *	process_dir (NBLK *n, TREE *curr_node, int flag);
extern void		make_xstat (struct stat *pstbuf, struct stat *xbuf);
extern int		is_it_a_dir (const char *directory, struct stat *pstbuf);
extern int		is_directory (const char *directory, struct stat *pstbuf);

/*------------------------------------------------------------------------
 * getnode.c
 */
extern void		do_load_node (void);
extern int		getnode (void);

/*------------------------------------------------------------------------
 * getperms.c
 */
extern int		get_perm_masks (int tag, int *pa_perms, int *po_perms);

/*------------------------------------------------------------------------
 * gblvars.c
 */
extern void		init_globals (int argc, char **argv);

/*------------------------------------------------------------------------
 * global.c
 */
extern void		do_global (void);

/*------------------------------------------------------------------------
 * goto.c
 */
extern void		goto_file (void);
extern void		goto_dir (void);

/*------------------------------------------------------------------------
 * graft.c
 */
extern void		graft (void);
extern int		check_sub_node (int graft_flag);
extern int		check_dir (DBLK *d);

/*------------------------------------------------------------------------
 * help.c
 */
extern int		help_open (void);
extern void		help_close (void);
extern void		help_topic (unsigned index);
extern void		help_redisplay (void);

/*------------------------------------------------------------------------
 * hexcmds.c
 */
extern int		do_hexedit_cmd (int c);
extern void		hexedit_redisplay (void);

/*------------------------------------------------------------------------
 * hexedit.c
 */
extern void		hexedit_run (const char *path);
extern void		hexedit_file_end (void);
extern void		hexedit_redisplay (void);
extern void		hexedit_setup_display (void);
extern void		hexedit_set_win (HXCB *h, WINDOW *win);
extern HXCB *	hexedit_init (WINDOW *win);
extern HXCB *	hexedit_end (HXCB *h);

/*------------------------------------------------------------------------
 * hide.c
 */
extern void		hide_sub_node (int onoff);
extern void		hide_entire_node (int onoff);
extern void		hide_dirs_in_node (NBLK *n, int s);
extern void		setup_dir_stats (void);

/*------------------------------------------------------------------------
 * info.c
 */
extern void		show_file_info (const char *name, off_t size, time_t mtime,
					int mode, int uid, int gid);
extern void		disp_cur_file_info (void);
extern void		disp_cur_dir_info (void);

/*------------------------------------------------------------------------
 * intmain.c
 */
extern int		int_main (int argc, char **argv);

/*------------------------------------------------------------------------
 * loadenv.c
 */
extern void		load_env (int argc, char **argv);

/*------------------------------------------------------------------------
 * loadres.c
 */
extern void		load_res (void);
extern void		free_res (void);
extern int		select_lang_by_name (const char *lang);
extern int		select_lang_by_num  (int num);
extern int		get_cur_lang (void);

/*------------------------------------------------------------------------
 * logbar.c
 */
extern int		log_bar (void);

/*------------------------------------------------------------------------
 * logcpio.c
 */
extern int		log_cpio (void);

/*------------------------------------------------------------------------
 * logtar.c
 */
extern int		log_tar (void);

/*------------------------------------------------------------------------
 * logztar.c
 */
extern int		log_ztar (void);

/*------------------------------------------------------------------------
 * match.c
 */
extern int		match (const char *name, const char *pattern);
extern int		match_names (const char *name, const char *pattern);

/*------------------------------------------------------------------------
 * mdl.c
 */
extern void		make_dir_list (NBLK *node_ptr);

/*------------------------------------------------------------------------
 * move.c
 */
extern void		do_move (void);
extern void		do_move_files (void);

/*------------------------------------------------------------------------
 * menu.c
 */
extern void		Menu_paint_menu (void);
extern void		menu_free_memory (void);
extern void		do_menu (void);

/*------------------------------------------------------------------------
 * menupick.c
 */
extern int		menupick (BLIST *menu_list, int (*action)(int n, int x),
					WINDOW *win, int ox);

/*------------------------------------------------------------------------
 * nblk.c
 */
extern NBLK *	nblk_make (void);
extern void		nblk_free (NBLK *n);

/*------------------------------------------------------------------------
 * newnode.c
 */
extern void		do_log (void);
extern void		do_release (void);
extern void		relog_dir (int recurse);
extern void		unlog_dir (void);
extern void		zap_dir (BLIST *b);
extern void		check_invalid_dir (void);
extern int		do_new_log (char *input_str);
extern TREE *	sort_dirs (TREE *t);

/*------------------------------------------------------------------------
 * nodutils.c
 */
extern void		init_cd (void);
extern int		init_node (const char *directory);
extern int		read_node (NBLK *node);
extern void		del_node (NBLK *n);
extern void		setup_node (void);
extern void		init_disp (void);
extern void		setup_file_display (int n);
extern void		setup_sa_display (void);

/*------------------------------------------------------------------------
 * open.c
 */
extern void		do_open (void);
extern void		do_open_file (const char *fullpath);

/*------------------------------------------------------------------------
 * pcmd.c
 */
extern void		process_cmd (int c);

/*------------------------------------------------------------------------
 * pcmdm.c
 */
extern int		check_mouse_in_win (WINDOW *win, int n);
extern int		check_mouse_pos (void);
extern int		read_vert_scrollbar (WINDOW *win, int n);
extern int		calculate_top_item (int cur, int num, int lines);

/*------------------------------------------------------------------------
 * pgm.c
 */
extern void		pgm (const char *directory);

/*------------------------------------------------------------------------
 * pgmhelp.c
 */
extern int		do_help (int key, void *data);

/*------------------------------------------------------------------------
 * pinput.c
 */
extern void		process_input (int c);
extern int		get_input_char (WINDOW *win, void (*redisplay)(void),
					int (*loop)(int count), void (*curpos)(void));

/*------------------------------------------------------------------------
 * print.c
 */
extern PRINTER *printer_open (const char *name, char *msgbuf);
extern void		print_close (int exit_flag);
extern void		print_scrn (void);
extern void		print_file (void);
extern void		print_file_with_hdr (const char *file, const char *path,
					const char *hdr);
extern int		print_init (void);
extern void		do_tag_print (void);
extern int		print_path (const char *path, const char *hdr);
extern void		do_dir_print (void);

/*------------------------------------------------------------------------
 * prompt.c
 */
extern void		disp_prompt_line (void);

/*------------------------------------------------------------------------
 * prune.c
 */
extern void		prune (void);
extern int		get_first_last (BLIST **first, BLIST **last, int inc_first);
extern int		clean_dir (TREE *t, NBLK *n, int del_flag);

/*------------------------------------------------------------------------
 * putnode.c
 */
extern void		do_save_node (void);
extern int		putnode (void);

/*------------------------------------------------------------------------
 * quit.c
 */
extern int		quit (void);
extern int		alt_quit (void);
extern void		done_processing (void);
extern void		all_done (void);
extern void		pgm_exit (int rc);

/*------------------------------------------------------------------------
 * rename.c
 */
extern int		do_rename (void);
extern void		make_file_name (const char *template, const char *origname,
					char *newname);
extern void		do_tag_rename (void);

/*------------------------------------------------------------------------
 * restore.c
 */
extern void		do_restore (void);

/*------------------------------------------------------------------------
 * search.c
 */
extern void		do_search (void);
extern REGEXP *	get_search_str (const char *prompt);

/*------------------------------------------------------------------------
 * setupdsp.c
 */
extern void		setup_display (void);

/*------------------------------------------------------------------------
 * showall.c
 */
extern void		do_showall (void);
extern BLIST *	get_files_to_level (NBLK *n);

/*------------------------------------------------------------------------
 * sort.c
 */
extern int		do_sort (void);
extern BLIST *	put_file_in_list (FBLK *f, BLIST *file_list);
extern BLIST *	merge_lists (BLIST *one, BLIST *two);

/*------------------------------------------------------------------------
 * split.c
 */
extern int		next_scr_no (int scr_no);
extern void		do_split (void);
extern void		clone_screen (int from, int to);
extern void		kill_screen (int num);
extern void		disp_splits (void);
extern void		disp_split_window (int i);
extern void		copy_dstat (DSTAT *d1, DSTAT *d2);
extern void		toggle_split (int n);
extern void		is_screen_kosher (int n);

/*------------------------------------------------------------------------
 * stats.c
 */
extern int		get_cur_dev_ent (void);
extern void		disp_path_line (void);
extern void		disp_file_box (void);
extern void		disp_drive_info (dev_t new_dev, int flag);
extern void		disp_node_stats (NBLK *root);
extern void		disp_dir_stats (void);
extern void		disp_cur_dir (void);
extern void		disp_cur_file (void);
extern void		disp_info_window (void);
extern void		info_win_off (void);

/*------------------------------------------------------------------------
 * sym.c
 */
extern int		resolve_symlink (void);
extern int		check_symlink (const char *path);
extern void		do_alt_link (void);
extern void		fblk_sym (FBLK *f);
extern int		make_sym_link (const char *old_path, const char *new_path,
					int abs);

/*------------------------------------------------------------------------
 * tagfile.c
 */
extern int		is_file_tagged (FBLK *f);
extern void		tag_file (FBLK *f);
extern void		untag_file (FBLK *f);
extern void		invert_file (FBLK *f);
extern void		tag_dir (BLIST *dir_ptr);
extern void		untag_dir (BLIST *dir_ptr);
extern void		invert_dir (BLIST *dir_ptr);
extern int		ask_alt_tag (int tag_mode, TAG_INFO *ti);
extern void		tag_sub_node (TAG_INFO *ti);
extern void		untag_sub_node (TAG_INFO *ti);
extern void		invert_sub_node (TAG_INFO *ti);
extern void		alt_invert_file (FBLK *f, TAG_INFO *ti);
extern void		alt_tag_file (FBLK *f, TAG_INFO *ti);
extern void		alt_untag_file (FBLK *f, TAG_INFO *ti);
extern void		alt_invert_dir (BLIST *dir_ptr, TAG_INFO *ti);
extern void		alt_tag_dir (BLIST *dir_ptr, TAG_INFO *ti);
extern void		alt_untag_dir (BLIST *dir_ptr, TAG_INFO *ti);

/*------------------------------------------------------------------------
 * tar.c
 */
extern int		tar_open (void);
extern int		tar_close (int eject);
extern int		tar_read_rec (void);
extern int		tar_write_rec (void);
extern int		tar_wr_hdr (FBLK *f, char *rel_name);
extern int		tar_wr_part_hdr (FBLK *f, char *rel_name, long s, int n,
					int t);
extern int		tar_wr_link_hdr (FBLK *f, FBLK *lf, char *rel_name);
extern int		tar_wr_sym_hdr (FBLK *f, char *l, char *rel_name);
extern void		tar_get_stbuf (struct stat *s);
extern int		tar_skip_file (void);
extern int		tar_is_hdr (union tblock *h);
extern int		tar_is_valid_hdr (void);
extern int		tar_position (const char *n, int v, off_t p);
extern int		tar_ask_for_volno (const char *n, int v);
extern char *	tar_filename (void);
extern char *	tar_linkname (void);
extern int		tar_num_exts (void);
extern int		tar_get_extno (void);
extern long		tar_get_file_ext_size (void);

/*------------------------------------------------------------------------
 * term.c
 */
extern int		win_cols (const void *valp);
extern int		win_rows (const void *valp);
extern int		win_opts (const void *valp);
extern int		win_arg (const char *argstr);
extern void		clone_term (void);
extern void		end_wins (void);
extern void		free_wins (void);
extern int		trm_use_mouse (const void *valp);
extern int		trm_alt_chars_ok (const void *valp);
extern int		trm_is_changed (int bf);
extern void		setup_window_attributes (void);
extern void		delete_windows (void);
extern void		resize_term (void);
extern int		init_wins (void);

/*------------------------------------------------------------------------
 * traverse.c
 */
extern void		traverse (int (*action)(void *data), void *data);

/*------------------------------------------------------------------------
 * treespec.c
 */
extern void		treespec (void);

/*------------------------------------------------------------------------
 * usage.c
 */
extern void		usage (int verbose, const char *pgm);

/*------------------------------------------------------------------------
 * usrgrp.c
 */
extern char *	get_owner (int uid, char *buf);
extern char *	get_group (int gid, char *buf);
extern void		check_owner (int uid);
extern void		check_group (int gid);
extern int		owner_len (void);
extern int		group_len (void);
extern BLIST *	owner_free (BLIST *owners);
extern BLIST *	group_free (BLIST *groups);

/*------------------------------------------------------------------------
 * util.c
 */
extern TREE *	pathname_to_dirtree (const char *pathname);
extern TREE *	path_to_dt (const char *pathname, int ntype);
extern TREE *	path_to_tree (const char *path, NBLK *n);
extern void		dptr_to_dirname (BLIST *dptr, char *line);
extern void		dirtree_to_dirname (TREE *t, char *line);
extern void		dblk_to_dirname (DBLK *d, char *line);
extern void		fblk_to_dirname (FBLK *f, char *line);
extern void		fblk_to_pathname (FBLK *f, char *path);
extern NBLK *	get_root_of_file (FBLK *fptr);
extern NBLK *	get_root_of_dir (TREE *t);
extern int		fblk_fs_type (FBLK *f);
extern int		dblk_fs_type (DBLK *d);
extern int		is_file_displayed (FBLK *f, int *frow, int *fcol);
extern void		redisplay_file (FBLK *f, int frow, int fcol);
extern void		update_dir (DBLK *d);
extern long		sum_files_in_list (BLIST *file_list, int check_tag);
extern int		check_for_dup_inode (BLIST *file_list, FBLK *file, int mode,
					int check_tag);
extern long		file_size_in_blks (FBLK *f, long size);
extern int		set_wide (const void *valp);
extern void		clock_rtn (WINDOW *win, void *data, int yr, int mo, int da,
					int hh, int mm, int ss, int wday, int yday);

/*------------------------------------------------------------------------
 * util2.c
 */
extern int		get_rdev (struct stat *s);
extern int		get_minor (struct stat *s);
extern int		get_major (struct stat *s);
extern int		set_rdev (struct stat *s, int rdev);
extern int		make_home_dir (void);

/*------------------------------------------------------------------------
 * view.c
 */
extern VFCB *	view_init (WINDOW *win);
extern void		set_view_win (VFCB *v, WINDOW *win);
extern VFCB *	view_end (VFCB *v);
extern void		view_open (VFCB *v, int rdwr);
extern void		view_close (VFCB *v);
extern void		get_buffer (VFCB *v);
extern void		down_line (VFCB *v);
extern void		up_line (VFCB *v);
extern void		bump_pos (VFCB *v);
extern void		view_next (VFCB *v);
extern void		view_prev (VFCB *v);
extern int		view_set_mode (const void *valp);

/*------------------------------------------------------------------------
 * viewcmd.c
 */
extern void		view_command (VFCB *v, int cmd);

/*------------------------------------------------------------------------
 * viewdisp.c
 */
extern void		v_display_no_refresh (VFCB *v);
extern void		v_display (VFCB *v);
extern void		view_scroll_bar_off (VFCB *v);

/*------------------------------------------------------------------------
 * what.c
 */
extern void		do_what (void);
extern void		what_free (void);

/*------------------------------------------------------------------------
 * xaddstr.c
 */
extern int		is_hilite_toggle (int c);
extern void		xaddch (WINDOW *window, int c);
extern void		xaddstr (WINDOW *window, const char *string);
extern void		xcaddstr (WINDOW *window, int code, const char *string);
extern void		xiaddstr (WINDOW *window, const char *string);
extern void		xciaddstr (WINDOW *window, int code, const char *string);
extern void		zaddstr (WINDOW *window, const char *string);
extern char *	zstring (const char *string, char *buf);
extern int		fill_fw_str (char *buf, const char *str, int len);
extern void		disp_fw_str (WINDOW *win, const char *str, int len);
extern void		disp_fw_str_m (WINDOW *win, const char *str, int len,
					int code, int pad);
extern int		display_len (const char *s);

/*------------------------------------------------------------------------
 * xform.c
 */
extern char *	xform  (char *buf, long num);
extern char *	xform2 (char *buf, int num, int len);
extern char *	xform3 (char *buf, int num);

/*------------------------------------------------------------------------
 * xgetch.c
 */
extern int		xchk_kbd (void);
extern int		xgetch (WINDOW *win);
extern int		rgetch (WINDOW *win);

/*------------------------------------------------------------------------
 * xgetstr.c
 */
extern int		xgetstr (WINDOW *window, char *string, int history,
					int str_len, int disp_len, int type);
extern int		xgetstr_cb (WINDOW *window, char *string, int history,
					int str_len, int disp_len, int type,
					void (*rtn)(void *data, int c), void *data);
extern int		xgetstr_tr (WINDOW *window, char *string, int history,
					int str_len, int disp_len, int type, TREE **t);

/*------------------------------------------------------------------------
 * xglist.c
 */
extern void		xg_ent_free (XG_ENTRY *xe);
extern void		xg_tbl_free (void);
extern void		xg_tbl_load (void);
extern void		xg_tbl_save (void);
extern XG_LIST *xg_tbl_find (int tbl_no);
extern void		xg_tbl_add  (XG_LIST *xg, const char *string, int keep);
extern void		xg_tbl_mark (XG_LIST *xg, XG_ENTRY *xe);

/*------------------------------------------------------------------------
 * xgmenu.c
 */
extern XG_ENTRY *	xg_menu (XG_LIST *xg, XG_INFO *xi,
					void (*action)(XG_ENTRY *xe, XG_INFO *xi));

/*------------------------------------------------------------------------
 * xsystem.c
 */
extern void		xsystem (const char *cmd, const char *dir, int wait_mode);
extern void		do_exec (void);
extern void		do_tag_exec (void);
extern void		exec_tag_args (const char *cmd);

#endif /* EXTERNS_H */
