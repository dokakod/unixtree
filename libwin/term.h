/*------------------------------------------------------------------------
 * header for low-level termcap/terminfo database access
 */
#ifndef TERM_TERMCAP_H
#define TERM_TERMCAP_H

/*------------------------------------------------------------------------
 * check if curses was included
 */
#ifndef A_NORMAL
#  include "curses.h"
#endif

/*------------------------------------------------------------------------
 * check if ecurses was included
 */
#ifndef ECURSES_VERSION
#  error "Wrong curses.h included."
#endif

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * TERMINAL struct
 */
#ifndef TERMINAL_T
#  define TERMINAL_T	TRUE;		/* indicate TERMINAL defined		*/
  typedef struct terminal TERMINAL;	/* opaque TERMINAL struct pointer	*/
#endif

/*------------------------------------------------------------------------
 * termcap/terminfo "variables"
 */
#define m_auto_left_margin			0
#define m_auto_right_margin			1
#define m_no_esc_ctlc				2
#define m_ceol_standout_glitch		3
#define m_eat_newline_glitch		4
#define m_erase_overstrike			5
#define m_generic_type				6
#define m_hard_copy					7
#define m_has_meta_key				8
#define m_has_status_line			9
#define m_insert_null_glitch		10
#define m_memory_above				11
#define m_memory_below				12
#define m_move_insert_mode			13
#define m_move_standout_mode		14
#define m_over_strike				15
#define m_status_line_esc_ok		16
#define m_dest_tabs_magic_smso		17
#define m_tilde_glitch				18
#define m_transparent_underline		19
#define m_xon_xoff					20
#define m_needs_xon_xoff			21
#define m_prtr_silent				22
#define m_hard_cursor				23
#define m_non_rev_rmcup				24
#define m_no_pad_char				25
#define m_non_dest_scroll_region	26
#define m_can_change				27
#define m_back_color_erase			28
#define m_hue_lightness_saturation	29
#define m_col_addr_glitch			30
#define m_cr_cancels_micro_mode		31
#define m_has_print_wheel			32
#define m_row_addr_glitch			33
#define m_semi_auto_right_margin	34
#define m_cpi_changes_res			35
#define m_lpi_changes_res			36

#define m_columns					0
#define m_init_tabs					1
#define m_lines						2
#define m_lines_of_memory			3
#define m_magic_cookie_glitch		4
#define m_padding_baud_rate			5
#define m_virtual_terminal			6
#define m_width_status_line			7
#define m_num_labels				8
#define m_label_height				9
#define m_label_width				10
#define m_max_attributes			11
#define m_maximum_windows			12
#define m_max_colors				13
#define m_max_pairs					14
#define m_no_color_video			15
#define m_buffer_capacity			16
#define m_dot_vert_spacing			17
#define m_dot_horz_spacing			18
#define m_max_micro_address			19
#define m_max_micro_jump			20
#define m_micro_char_size			21
#define m_micro_line_size			22
#define m_number_of_pins			23
#define m_output_res_char			24
#define m_output_res_line			25
#define m_output_res_horz_inch		26
#define m_output_res_vert_inch		27
#define m_print_rate				28
#define m_wide_char_size			29
#define m_buttons					30
#define m_bit_image_entwining		31
#define m_bit_image_type			32

#define m_back_tab					0
#define m_bell						1
#define m_carriage_return			2
#define m_change_scroll_region		3
#define m_clear_all_tabs			4
#define m_clear_screen				5
#define m_clr_eol					6
#define m_clr_eos					7
#define m_column_address			8
#define m_command_character			9
#define m_cursor_address			10
#define m_cursor_down				11
#define m_cursor_home				12
#define m_cursor_invisible			13
#define m_cursor_left				14
#define m_cursor_mem_address		15
#define m_cursor_normal				16
#define m_cursor_right				17
#define m_cursor_to_ll				18
#define m_cursor_up					19
#define m_cursor_visible			20
#define m_delete_character			21
#define m_delete_line				22
#define m_dis_status_line			23
#define m_down_half_line			24
#define m_enter_alt_charset_mode	25
#define m_enter_blink_mode			26
#define m_enter_bold_mode			27
#define m_enter_ca_mode				28
#define m_enter_delete_mode			29
#define m_enter_dim_mode			30
#define m_enter_insert_mode			31
#define m_enter_secure_mode			32
#define m_enter_protected_mode		33
#define m_enter_reverse_mode		34
#define m_enter_standout_mode		35
#define m_enter_underline_mode		36
#define m_erase_chars				37
#define m_exit_alt_charset_mode		38
#define m_exit_attribute_mode		39
#define m_exit_ca_mode				40
#define m_exit_delete_mode			41
#define m_exit_insert_mode			42
#define m_exit_standout_mode		43
#define m_exit_underline_mode		44
#define m_flash_screen				45
#define m_form_feed					46
#define m_from_status_line			47
#define m_init_1string				48
#define m_init_2string				49
#define m_init_3string				50
#define m_init_file					51
#define m_insert_character			52
#define m_insert_line				53
#define m_insert_padding			54
#define m_key_backspace				55
#define m_key_catab					56
#define m_key_clear					57
#define m_key_ctab					58
#define m_key_dc					59
#define m_key_dl					60
#define m_key_down					61
#define m_key_eic					62
#define m_key_eol					63
#define m_key_eos					64
#define m_key_f0					65
#define m_key_f1					66
#define m_key_f10					67
#define m_key_f2					68
#define m_key_f3					69
#define m_key_f4					70
#define m_key_f5					71
#define m_key_f6					72
#define m_key_f7					73
#define m_key_f8					74
#define m_key_f9					75
#define m_key_home					76
#define m_key_ic					77
#define m_key_il					78
#define m_key_left					79
#define m_key_ll					80
#define m_key_npage					81
#define m_key_ppage					82
#define m_key_right					83
#define m_key_sf					84
#define m_key_sr					85
#define m_key_stab					86
#define m_key_up					87
#define m_keypad_local				88
#define m_keypad_xmit				89
#define m_lab_f0					90
#define m_lab_f1					91
#define m_lab_f10					92
#define m_lab_f2					93
#define m_lab_f3					94
#define m_lab_f4					95
#define m_lab_f5					96
#define m_lab_f6					97
#define m_lab_f7					98
#define m_lab_f8					99
#define m_lab_f9					100
#define m_meta_off					101
#define m_meta_on					102
#define m_newline					103
#define m_pad_char					104
#define m_parm_dch					105
#define m_parm_delete_line			106
#define m_parm_down_cursor			107
#define m_parm_ich					108
#define m_parm_index				109
#define m_parm_insert_line			110
#define m_parm_left_cursor			111
#define m_parm_right_cursor			112
#define m_parm_rindex				113
#define m_parm_up_cursor			114
#define m_pkey_key					115
#define m_pkey_local				116
#define m_pkey_xmit					117
#define m_print_screen				118
#define m_prtr_off					119
#define m_prtr_on					120
#define m_repeat_char				121
#define m_reset_1string				122
#define m_reset_2string				123
#define m_reset_3string				124
#define m_reset_file				125
#define m_restore_cursor			126
#define m_row_address				127
#define m_save_cursor				128
#define m_scroll_forward			129
#define m_scroll_reverse			130
#define m_set_attributes			131
#define m_set_tab					132
#define m_set_window				133
#define m_tab						134
#define m_to_status_line			135
#define m_underline_char			136
#define m_up_half_line				137
#define m_init_prog					138
#define m_key_a1					139
#define m_key_a3					140
#define m_key_b2					141
#define m_key_c1					142
#define m_key_c3					143
#define m_prtr_non					144
#define m_char_padding				145
#define m_acs_chars					146
#define m_plab_norm					147
#define m_key_btab					148
#define m_enter_xon_mode			149
#define m_exit_xon_mode				150
#define m_enter_am_mode				151
#define m_exit_am_mode				152
#define m_xon_character				153
#define m_xoff_character			154
#define m_ena_acs					155
#define m_label_on					156
#define m_label_off					157
#define m_key_beg					158
#define m_key_cancel				159
#define m_key_close					160
#define m_key_command				161
#define m_key_copy					162
#define m_key_create				163
#define m_key_end					164
#define m_key_enter					165
#define m_key_exit					166
#define m_key_find					167
#define m_key_help					168
#define m_key_mark					169
#define m_key_message				170
#define m_key_move					171
#define m_key_next					172
#define m_key_open					173
#define m_key_options				174
#define m_key_previous				175
#define m_key_print					176
#define m_key_redo					177
#define m_key_reference				178
#define m_key_refresh				179
#define m_key_replace				180
#define m_key_restart				181
#define m_key_resume				182
#define m_key_save					183
#define m_key_suspend				184
#define m_key_undo					185
#define m_key_sbeg					186
#define m_key_scancel				187
#define m_key_scommand				188
#define m_key_scopy					189
#define m_key_screate				190
#define m_key_sdc					191
#define m_key_sdl					192
#define m_key_select				193
#define m_key_send					194
#define m_key_seol					195
#define m_key_sexit					196
#define m_key_sfind					197
#define m_key_shelp					198
#define m_key_shome					199
#define m_key_sic					200
#define m_key_sleft					201
#define m_key_smessage				202
#define m_key_smove					203
#define m_key_snext					204
#define m_key_soptions				205
#define m_key_sprevious				206
#define m_key_sprint				207
#define m_key_sredo					208
#define m_key_sreplace				209
#define m_key_sright				210
#define m_key_srsume				211
#define m_key_ssave					212
#define m_key_ssuspend				213
#define m_key_sundo					214
#define m_req_for_input				215
#define m_key_f11					216
#define m_key_f12					217
#define m_key_f13					218
#define m_key_f14					219
#define m_key_f15					220
#define m_key_f16					221
#define m_key_f17					222
#define m_key_f18					223
#define m_key_f19					224
#define m_key_f20					225
#define m_key_f21					226
#define m_key_f22					227
#define m_key_f23					228
#define m_key_f24					229
#define m_key_f25					230
#define m_key_f26					231
#define m_key_f27					232
#define m_key_f28					233
#define m_key_f29					234
#define m_key_f30					235
#define m_key_f31					236
#define m_key_f32					237
#define m_key_f33					238
#define m_key_f34					239
#define m_key_f35					240
#define m_key_f36					241
#define m_key_f37					242
#define m_key_f38					243
#define m_key_f39					244
#define m_key_f40					245
#define m_key_f41					246
#define m_key_f42					247
#define m_key_f43					248
#define m_key_f44					249
#define m_key_f45					250
#define m_key_f46					251
#define m_key_f47					252
#define m_key_f48					253
#define m_key_f49					254
#define m_key_f50					255
#define m_key_f51					256
#define m_key_f52					257
#define m_key_f53					258
#define m_key_f54					259
#define m_key_f55					260
#define m_key_f56					261
#define m_key_f57					262
#define m_key_f58					263
#define m_key_f59					264
#define m_key_f60					265
#define m_key_f61					266
#define m_key_f62					267
#define m_key_f63					268
#define m_clr_bol					269
#define m_clear_margins				270
#define m_set_left_margin			271
#define m_set_right_margin			272
#define m_label_format				273
#define m_set_clock					274
#define m_display_clock				275
#define m_remove_clock				276
#define m_create_window				277
#define m_goto_window				278
#define m_hangup					279
#define m_dial_phone				280
#define m_quick_dial				281
#define m_tone						282
#define m_pulse						283
#define m_flash_hook				284
#define m_fixed_pause				285
#define m_wait_tone					286
#define m_user0						287
#define m_user1						288
#define m_user2						289
#define m_user3						290
#define m_user4						291
#define m_user5						292
#define m_user6						293
#define m_user7						294
#define m_user8						295
#define m_user9						296
#define m_orig_pair					297
#define m_orig_colors				298
#define m_initialize_color			299
#define m_initialize_pair			300
#define m_set_color_pair			301
#define m_set_foreground			302
#define m_set_background			303
#define m_change_char_pitch			304
#define m_change_line_pitch			305
#define m_change_res_horz			306
#define m_change_res_vert			307
#define m_define_char				308
#define m_enter_doublewide_mode		309
#define m_enter_draft_quality		310
#define m_enter_italics_mode		311
#define m_enter_leftward_mode		312
#define m_enter_micro_mode			313
#define m_enter_near_letter_quality	314
#define m_enter_normal_quality		315
#define m_enter_shadow_mode			316
#define m_enter_subscript_mode		317
#define m_enter_superscript_mode	318
#define m_enter_upward_mode			319
#define m_exit_doublewide_mode		320
#define m_exit_italics_mode			321
#define m_exit_leftward_mode		322
#define m_exit_micro_mode			323
#define m_exit_shadow_mode			324
#define m_exit_subscript_mode		325
#define m_exit_superscript_mode		326
#define m_exit_upward_mode			327
#define m_micro_column_address		328
#define m_micro_down				329
#define m_micro_left				330
#define m_micro_right				331
#define m_micro_row_address			332
#define m_micro_up					333
#define m_order_of_pins				334
#define m_parm_down_micro			335
#define m_parm_left_micro			336
#define m_parm_right_micro			337
#define m_parm_up_micro				338
#define m_select_char_set			339
#define m_set_bottom_margin			340
#define m_set_bottom_margin_parm	341
#define m_set_left_margin_parm		342
#define m_set_right_margin_parm		343
#define m_set_top_margin			344
#define m_set_top_margin_parm		345
#define m_start_bit_image			346
#define m_start_char_set_def		347
#define m_stop_bit_image			348
#define m_stop_char_set_def			349
#define m_subscript_characters		350
#define m_superscript_characters	351
#define m_these_cause_cr			352
#define m_zero_motion				353
#define m_char_set_names			354
#define m_key_mouse					355
#define m_mouse_info				356
#define m_req_mouse_pos				357
#define m_get_mouse					358
#define m_set_a_foreground			359
#define m_set_a_background			360
#define m_pkey_plab					361
#define m_device_type				362
#define m_code_set_init				363
#define m_set0_des_seq				364
#define m_set1_des_seq				365
#define m_set2_des_seq				366
#define m_set3_des_seq				367
#define m_set_lr_margin				368
#define m_set_tb_margin				369
#define m_bit_image_repeat			370
#define m_bit_image_newline			371
#define m_bit_image_carriage_return	372
#define m_color_names				373
#define m_define_bit_image_region	374
#define m_end_bit_image_region		375
#define m_set_color_band			376
#define m_set_page_length			377
#define m_display_pc_char			378
#define m_enter_pc_charset_mode		379
#define m_exit_pc_charset_mode		380
#define m_enter_scancode_mode		381
#define m_exit_scancode_mode		382
#define m_pc_term_options			383
#define m_scancode_escape			384
#define m_alt_scancode_esc			385
#define m_enter_horizontal_hl_mode	386
#define m_enter_left_hl_mode		387
#define m_enter_low_hl_mode			388
#define m_enter_right_hl_mode		389
#define m_enter_top_hl_mode			390
#define m_enter_vertical_hl_mode	391
#define m_set_a_attributes			392
#define m_set_pglen_inch			393

#define auto_left_margin			tcgetflag(m_auto_left_margin)
#define auto_right_margin			tcgetflag(m_auto_right_margin)
#define no_esc_ctlc					tcgetflag(m_no_esc_ctlc)
#define ceol_standout_glitch		tcgetflag(m_ceol_standout_glitch)
#define eat_newline_glitch			tcgetflag(m_eat_newline_glitch)
#define erase_overstrike			tcgetflag(m_erase_overstrike)
#define generic_type				tcgetflag(m_generic_type)
#define hard_copy					tcgetflag(m_hard_copy)
#define has_meta_key				tcgetflag(m_has_meta_key)
#define has_status_line				tcgetflag(m_has_status_line)
#define insert_null_glitch			tcgetflag(m_insert_null_glitch)
#define memory_above				tcgetflag(m_memory_above)
#define memory_below				tcgetflag(m_memory_below)
#define move_insert_mode			tcgetflag(m_move_insert_mode)
#define move_standout_mode			tcgetflag(m_move_standout_mode)
#define over_strike					tcgetflag(m_over_strike)
#define status_line_esc_ok			tcgetflag(m_status_line_esc_ok)
#define dest_tabs_magic_smso		tcgetflag(m_dest_tabs_magic_smso)
#define tilde_glitch				tcgetflag(m_tilde_glitch)
#define transparent_underline		tcgetflag(m_transparent_underline)
#define xon_xoff					tcgetflag(m_xon_xoff)
#define needs_xon_xoff				tcgetflag(m_needs_xon_xoff)
#define prtr_silent					tcgetflag(m_prtr_silent)
#define hard_cursor					tcgetflag(m_hard_cursor)
#define non_rev_rmcup				tcgetflag(m_non_rev_rmcup)
#define no_pad_char					tcgetflag(m_no_pad_char)
#define non_dest_scroll_region		tcgetflag(m_non_dest_scroll_region)
#define can_change					tcgetflag(m_can_change)
#define back_color_erase			tcgetflag(m_back_color_erase)
#define hue_lightness_saturation	tcgetflag(m_hue_lightness_saturation)
#define col_addr_glitch				tcgetflag(m_col_addr_glitch)
#define cr_cancels_micro_mode		tcgetflag(m_cr_cancels_micro_mode)
#define has_print_wheel				tcgetflag(m_has_print_wheel)
#define row_addr_glitch				tcgetflag(m_row_addr_glitch)
#define semi_auto_right_margin		tcgetflag(m_semi_auto_right_margin)
#define cpi_changes_res				tcgetflag(m_cpi_changes_res)
#define lpi_changes_res				tcgetflag(m_lpi_changes_res)

#define columns						tcgetnum (m_columns)
#define init_tabs					tcgetnum (m_init_tabs)
#define lines						tcgetnum (m_lines)
#define lines_of_memory				tcgetnum (m_lines_of_memory)
#define magic_cookie_glitch			tcgetnum (m_magic_cookie_glitch)
#define padding_baud_rate			tcgetnum (m_padding_baud_rate)
#define virtual_terminal			tcgetnum (m_virtual_terminal)
#define width_status_line			tcgetnum (m_width_status_line)
#define num_labels					tcgetnum (m_num_labels)
#define label_height				tcgetnum (m_label_height)
#define label_width					tcgetnum (m_label_width)
#define max_attributes				tcgetnum (m_max_attributes)
#define maximum_windows				tcgetnum (m_maximum_windows)
#define max_colors					tcgetnum (m_max_colors)
#define max_pairs					tcgetnum (m_max_pairs)
#define no_color_video				tcgetnum (m_no_color_video)
#define buffer_capacity				tcgetnum (m_buffer_capacity)
#define dot_vert_spacing			tcgetnum (m_dot_vert_spacing)
#define dot_horz_spacing			tcgetnum (m_dot_horz_spacing)
#define max_micro_address			tcgetnum (m_max_micro_address)
#define max_micro_jump				tcgetnum (m_max_micro_jump)
#define micro_char_size				tcgetnum (m_micro_char_size)
#define micro_line_size				tcgetnum (m_micro_line_size)
#define number_of_pins				tcgetnum (m_number_of_pins)
#define output_res_char				tcgetnum (m_output_res_char)
#define output_res_line				tcgetnum (m_output_res_line)
#define output_res_horz_inch		tcgetnum (m_output_res_horz_inch)
#define output_res_vert_inch		tcgetnum (m_output_res_vert_inch)
#define print_rate					tcgetnum (m_print_rate)
#define wide_char_size				tcgetnum (m_wide_char_size)
#define buttons						tcgetnum (m_buttons)
#define bit_image_entwining			tcgetnum (m_bit_image_entwining)
#define bit_image_type				tcgetnum (m_bit_image_type)

#define back_tab					tcgetstr (m_back_tab)
#define bell						tcgetstr (m_bell)
#define carriage_return				tcgetstr (m_carriage_return)
#define change_scroll_region		tcgetstr (m_change_scroll_region)
#define clear_all_tabs				tcgetstr (m_clear_all_tabs)
#define clear_screen				tcgetstr (m_clear_screen)
#define clr_eol						tcgetstr (m_clr_eol)
#define clr_eos						tcgetstr (m_clr_eos)
#define column_address				tcgetstr (m_column_address)
#define command_character			tcgetstr (m_command_character)
#define cursor_address				tcgetstr (m_cursor_address)
#define cursor_down					tcgetstr (m_cursor_down)
#define cursor_home					tcgetstr (m_cursor_home)
#define cursor_invisible			tcgetstr (m_cursor_invisible)
#define cursor_left					tcgetstr (m_cursor_left)
#define cursor_mem_address			tcgetstr (m_cursor_mem_address)
#define cursor_normal				tcgetstr (m_cursor_normal)
#define cursor_right				tcgetstr (m_cursor_right)
#define cursor_to_ll				tcgetstr (m_cursor_to_ll)
#define cursor_up					tcgetstr (m_cursor_up)
#define cursor_visible				tcgetstr (m_cursor_visible)
#define delete_character			tcgetstr (m_delete_character)
#define delete_line					tcgetstr (m_delete_line)
#define dis_status_line				tcgetstr (m_dis_status_line)
#define down_half_line				tcgetstr (m_down_half_line)
#define enter_alt_charset_mode		tcgetstr (m_enter_alt_charset_mode)
#define enter_blink_mode			tcgetstr (m_enter_blink_mode)
#define enter_bold_mode				tcgetstr (m_enter_bold_mode)
#define enter_ca_mode				tcgetstr (m_enter_ca_mode)
#define enter_delete_mode			tcgetstr (m_enter_delete_mode)
#define enter_dim_mode				tcgetstr (m_enter_dim_mode)
#define enter_insert_mode			tcgetstr (m_enter_insert_mode)
#define enter_secure_mode			tcgetstr (m_enter_secure_mode)
#define enter_protected_mode		tcgetstr (m_enter_protected_mode)
#define enter_reverse_mode			tcgetstr (m_enter_reverse_mode)
#define enter_standout_mode			tcgetstr (m_enter_standout_mode)
#define enter_underline_mode		tcgetstr (m_enter_underline_mode)
#define erase_chars					tcgetstr (m_erase_chars)
#define exit_alt_charset_mode		tcgetstr (m_exit_alt_charset_mode)
#define exit_attribute_mode			tcgetstr (m_exit_attribute_mode)
#define exit_ca_mode				tcgetstr (m_exit_ca_mode)
#define exit_delete_mode			tcgetstr (m_exit_delete_mode)
#define exit_insert_mode			tcgetstr (m_exit_insert_mode)
#define exit_standout_mode			tcgetstr (m_exit_standout_mode)
#define exit_underline_mode			tcgetstr (m_exit_underline_mode)
#define flash_screen				tcgetstr (m_flash_screen)
#define form_feed					tcgetstr (m_form_feed)
#define from_status_line			tcgetstr (m_from_status_line)
#define init_1string				tcgetstr (m_init_1string)
#define init_2string				tcgetstr (m_init_2string)
#define init_3string				tcgetstr (m_init_3string)
#define init_file					tcgetstr (m_init_file)
#define insert_character			tcgetstr (m_insert_character)
#define insert_line					tcgetstr (m_insert_line)
#define insert_padding				tcgetstr (m_insert_padding)
#define key_backspace				tcgetstr (m_key_backspace)
#define key_catab					tcgetstr (m_key_catab)
#define key_clear					tcgetstr (m_key_clear)
#define key_ctab					tcgetstr (m_key_ctab)
#define key_dc						tcgetstr (m_key_dc)
#define key_dl						tcgetstr (m_key_dl)
#define key_down					tcgetstr (m_key_down)
#define key_eic						tcgetstr (m_key_eic)
#define key_eol						tcgetstr (m_key_eol)
#define key_eos						tcgetstr (m_key_eos)
#define key_f0						tcgetstr (m_key_f0)
#define key_f1						tcgetstr (m_key_f1)
#define key_f10						tcgetstr (m_key_f10)
#define key_f2						tcgetstr (m_key_f2)
#define key_f3						tcgetstr (m_key_f3)
#define key_f4						tcgetstr (m_key_f4)
#define key_f5						tcgetstr (m_key_f5)
#define key_f6						tcgetstr (m_key_f6)
#define key_f7						tcgetstr (m_key_f7)
#define key_f8						tcgetstr (m_key_f8)
#define key_f9						tcgetstr (m_key_f9)
#define key_home					tcgetstr (m_key_home)
#define key_ic						tcgetstr (m_key_ic)
#define key_il						tcgetstr (m_key_il)
#define key_left					tcgetstr (m_key_left)
#define key_ll						tcgetstr (m_key_ll)
#define key_npage					tcgetstr (m_key_npage)
#define key_ppage					tcgetstr (m_key_ppage)
#define key_right					tcgetstr (m_key_right)
#define key_sf						tcgetstr (m_key_sf)
#define key_sr						tcgetstr (m_key_sr)
#define key_stab					tcgetstr (m_key_stab)
#define key_up						tcgetstr (m_key_up)
#define keypad_local				tcgetstr (m_keypad_local)
#define keypad_xmit					tcgetstr (m_keypad_xmit)
#define lab_f0						tcgetstr (m_lab_f0)
#define lab_f1						tcgetstr (m_lab_f1)
#define lab_f10						tcgetstr (m_lab_f10)
#define lab_f2						tcgetstr (m_lab_f2)
#define lab_f3						tcgetstr (m_lab_f3)
#define lab_f4						tcgetstr (m_lab_f4)
#define lab_f5						tcgetstr (m_lab_f5)
#define lab_f6						tcgetstr (m_lab_f6)
#define lab_f7						tcgetstr (m_lab_f7)
#define lab_f8						tcgetstr (m_lab_f8)
#define lab_f9						tcgetstr (m_lab_f9)
#define meta_off					tcgetstr (m_meta_off)
#define meta_on						tcgetstr (m_meta_on)
#define newline						tcgetstr (m_newline)
#define pad_char					tcgetstr (m_pad_char)
#define parm_dch					tcgetstr (m_parm_dch)
#define parm_delete_line			tcgetstr (m_parm_delete_line)
#define parm_down_cursor			tcgetstr (m_parm_down_cursor)
#define parm_ich					tcgetstr (m_parm_ich)
#define parm_index					tcgetstr (m_parm_index)
#define parm_insert_line			tcgetstr (m_parm_insert_line)
#define parm_left_cursor			tcgetstr (m_parm_left_cursor)
#define parm_right_cursor			tcgetstr (m_parm_right_cursor)
#define parm_rindex					tcgetstr (m_parm_rindex)
#define parm_up_cursor				tcgetstr (m_parm_up_cursor)
#define pkey_key					tcgetstr (m_pkey_key)
#define pkey_local					tcgetstr (m_pkey_local)
#define pkey_xmit					tcgetstr (m_pkey_xmit)
#define print_screen				tcgetstr (m_print_screen)
#define prtr_off					tcgetstr (m_prtr_off)
#define prtr_on						tcgetstr (m_prtr_on)
#define repeat_char					tcgetstr (m_repeat_char)
#define reset_1string				tcgetstr (m_reset_1string)
#define reset_2string				tcgetstr (m_reset_2string)
#define reset_3string				tcgetstr (m_reset_3string)
#define reset_file					tcgetstr (m_reset_file)
#define restore_cursor				tcgetstr (m_restore_cursor)
#define row_address					tcgetstr (m_row_address)
#define save_cursor					tcgetstr (m_save_cursor)
#define scroll_forward				tcgetstr (m_scroll_forward)
#define scroll_reverse				tcgetstr (m_scroll_reverse)
#define set_attributes				tcgetstr (m_set_attributes)
#define set_tab						tcgetstr (m_set_tab)
#define set_window					tcgetstr (m_set_window)
#define tab							tcgetstr (m_tab)
#define to_status_line				tcgetstr (m_to_status_line)
#define underline_char				tcgetstr (m_underline_char)
#define up_half_line				tcgetstr (m_up_half_line)
#define init_prog					tcgetstr (m_init_prog)
#define key_a1						tcgetstr (m_key_a1)
#define key_a3						tcgetstr (m_key_a3)
#define key_b2						tcgetstr (m_key_b2)
#define key_c1						tcgetstr (m_key_c1)
#define key_c3						tcgetstr (m_key_c3)
#define prtr_non					tcgetstr (m_prtr_non)
#define char_padding				tcgetstr (m_char_padding)
#define acs_chars					tcgetstr (m_acs_chars)
#define plab_norm					tcgetstr (m_plab_norm)
#define key_btab					tcgetstr (m_key_btab)
#define enter_xon_mode				tcgetstr (m_enter_xon_mode)
#define exit_xon_mode				tcgetstr (m_exit_xon_mode)
#define enter_am_mode				tcgetstr (m_enter_am_mode)
#define exit_am_mode				tcgetstr (m_exit_am_mode)
#define xon_character				tcgetstr (m_xon_character)
#define xoff_character				tcgetstr (m_xoff_character)
#define ena_acs						tcgetstr (m_ena_acs)
#define label_on					tcgetstr (m_label_on)
#define label_off					tcgetstr (m_label_off)
#define key_beg						tcgetstr (m_key_beg)
#define key_cancel					tcgetstr (m_key_cancel)
#define key_close					tcgetstr (m_key_close)
#define key_command					tcgetstr (m_key_command)
#define key_copy					tcgetstr (m_key_copy)
#define key_create					tcgetstr (m_key_create)
#define key_end						tcgetstr (m_key_end)
#define key_enter					tcgetstr (m_key_enter)
#define key_exit					tcgetstr (m_key_exit)
#define key_find					tcgetstr (m_key_find)
#define key_help					tcgetstr (m_key_help)
#define key_mark					tcgetstr (m_key_mark)
#define key_message					tcgetstr (m_key_message)
#define key_move					tcgetstr (m_key_move)
#define key_next					tcgetstr (m_key_next)
#define key_open					tcgetstr (m_key_open)
#define key_options					tcgetstr (m_key_options)
#define key_previous				tcgetstr (m_key_previous)
#define key_print					tcgetstr (m_key_print)
#define key_redo					tcgetstr (m_key_redo)
#define key_reference				tcgetstr (m_key_reference)
#define key_refresh					tcgetstr (m_key_refresh)
#define key_replace					tcgetstr (m_key_replace)
#define key_restart					tcgetstr (m_key_restart)
#define key_resume					tcgetstr (m_key_resume)
#define key_save					tcgetstr (m_key_save)
#define key_suspend					tcgetstr (m_key_suspend)
#define key_undo					tcgetstr (m_key_undo)
#define key_sbeg					tcgetstr (m_key_sbeg)
#define key_scancel					tcgetstr (m_key_scancel)
#define key_scommand				tcgetstr (m_key_scommand)
#define key_scopy					tcgetstr (m_key_scopy)
#define key_screate					tcgetstr (m_key_screate)
#define key_sdc						tcgetstr (m_key_sdc)
#define key_sdl						tcgetstr (m_key_sdl)
#define key_select					tcgetstr (m_key_select)
#define key_send					tcgetstr (m_key_send)
#define key_seol					tcgetstr (m_key_seol)
#define key_sexit					tcgetstr (m_key_sexit)
#define key_sfind					tcgetstr (m_key_sfind)
#define key_shelp					tcgetstr (m_key_shelp)
#define key_shome					tcgetstr (m_key_shome)
#define key_sic						tcgetstr (m_key_sic)
#define key_sleft					tcgetstr (m_key_sleft)
#define key_smessage				tcgetstr (m_key_smessage)
#define key_smove					tcgetstr (m_key_smove)
#define key_snext					tcgetstr (m_key_snext)
#define key_soptions				tcgetstr (m_key_soptions)
#define key_sprevious				tcgetstr (m_key_sprevious)
#define key_sprint					tcgetstr (m_key_sprint)
#define key_sredo					tcgetstr (m_key_sredo)
#define key_sreplace				tcgetstr (m_key_sreplace)
#define key_sright					tcgetstr (m_key_sright)
#define key_srsume					tcgetstr (m_key_srsume)
#define key_ssave					tcgetstr (m_key_ssave)
#define key_ssuspend				tcgetstr (m_key_ssuspend)
#define key_sundo					tcgetstr (m_key_sundo)
#define req_for_input				tcgetstr (m_req_for_input)
#define key_f11						tcgetstr (m_key_f11)
#define key_f12						tcgetstr (m_key_f12)
#define key_f13						tcgetstr (m_key_f13)
#define key_f14						tcgetstr (m_key_f14)
#define key_f15						tcgetstr (m_key_f15)
#define key_f16						tcgetstr (m_key_f16)
#define key_f17						tcgetstr (m_key_f17)
#define key_f18						tcgetstr (m_key_f18)
#define key_f19						tcgetstr (m_key_f19)
#define key_f20						tcgetstr (m_key_f20)
#define key_f21						tcgetstr (m_key_f21)
#define key_f22						tcgetstr (m_key_f22)
#define key_f23						tcgetstr (m_key_f23)
#define key_f24						tcgetstr (m_key_f24)
#define key_f25						tcgetstr (m_key_f25)
#define key_f26						tcgetstr (m_key_f26)
#define key_f27						tcgetstr (m_key_f27)
#define key_f28						tcgetstr (m_key_f28)
#define key_f29						tcgetstr (m_key_f29)
#define key_f30						tcgetstr (m_key_f30)
#define key_f31						tcgetstr (m_key_f31)
#define key_f32						tcgetstr (m_key_f32)
#define key_f33						tcgetstr (m_key_f33)
#define key_f34						tcgetstr (m_key_f34)
#define key_f35						tcgetstr (m_key_f35)
#define key_f36						tcgetstr (m_key_f36)
#define key_f37						tcgetstr (m_key_f37)
#define key_f38						tcgetstr (m_key_f38)
#define key_f39						tcgetstr (m_key_f39)
#define key_f40						tcgetstr (m_key_f40)
#define key_f41						tcgetstr (m_key_f41)
#define key_f42						tcgetstr (m_key_f42)
#define key_f43						tcgetstr (m_key_f43)
#define key_f44						tcgetstr (m_key_f44)
#define key_f45						tcgetstr (m_key_f45)
#define key_f46						tcgetstr (m_key_f46)
#define key_f47						tcgetstr (m_key_f47)
#define key_f48						tcgetstr (m_key_f48)
#define key_f49						tcgetstr (m_key_f49)
#define key_f50						tcgetstr (m_key_f50)
#define key_f51						tcgetstr (m_key_f51)
#define key_f52						tcgetstr (m_key_f52)
#define key_f53						tcgetstr (m_key_f53)
#define key_f54						tcgetstr (m_key_f54)
#define key_f55						tcgetstr (m_key_f55)
#define key_f56						tcgetstr (m_key_f56)
#define key_f57						tcgetstr (m_key_f57)
#define key_f58						tcgetstr (m_key_f58)
#define key_f59						tcgetstr (m_key_f59)
#define key_f60						tcgetstr (m_key_f60)
#define key_f61						tcgetstr (m_key_f61)
#define key_f62						tcgetstr (m_key_f62)
#define key_f63						tcgetstr (m_key_f63)
#define clr_bol						tcgetstr (m_clr_bol)
#define clear_margins				tcgetstr (m_clear_margins)
#define set_left_margin				tcgetstr (m_set_left_margin)
#define set_right_margin			tcgetstr (m_set_right_margin)
#define label_format				tcgetstr (m_label_format)
#define set_clock					tcgetstr (m_set_clock)
#define display_clock				tcgetstr (m_display_clock)
#define remove_clock				tcgetstr (m_remove_clock)
#define create_window				tcgetstr (m_create_window)
#define goto_window					tcgetstr (m_goto_window)
#define hangup						tcgetstr (m_hangup)
#define dial_phone					tcgetstr (m_dial_phone)
#define quick_dial					tcgetstr (m_quick_dial)
#define tone						tcgetstr (m_tone)
#define pulse						tcgetstr (m_pulse)
#define flash_hook					tcgetstr (m_flash_hook)
#define fixed_pause					tcgetstr (m_fixed_pause)
#define wait_tone					tcgetstr (m_wait_tone)
#define user0						tcgetstr (m_user0)
#define user1						tcgetstr (m_user1)
#define user2						tcgetstr (m_user2)
#define user3						tcgetstr (m_user3)
#define user4						tcgetstr (m_user4)
#define user5						tcgetstr (m_user5)
#define user6						tcgetstr (m_user6)
#define user7						tcgetstr (m_user7)
#define user8						tcgetstr (m_user8)
#define user9						tcgetstr (m_user9)
#define orig_pair					tcgetstr (m_orig_pair)
#define orig_colors					tcgetstr (m_orig_colors)
#define initialize_color			tcgetstr (m_initialize_color)
#define initialize_pair				tcgetstr (m_initialize_pair)
#define set_color_pair				tcgetstr (m_set_color_pair)
#define set_foreground				tcgetstr (m_set_foreground)
#define set_background				tcgetstr (m_set_background)
#define change_char_pitch			tcgetstr (m_change_char_pitch)
#define change_line_pitch			tcgetstr (m_change_line_pitch)
#define change_res_horz				tcgetstr (m_change_res_horz)
#define change_res_vert				tcgetstr (m_change_res_vert)
#define define_char					tcgetstr (m_define_char)
#define enter_doublewide_mode		tcgetstr (m_enter_doublewide_mode)
#define enter_draft_quality			tcgetstr (m_enter_draft_quality)
#define enter_italics_mode			tcgetstr (m_enter_italics_mode)
#define enter_leftward_mode			tcgetstr (m_enter_leftward_mode)
#define enter_micro_mode			tcgetstr (m_enter_micro_mode)
#define enter_near_letter_quality	tcgetstr (m_enter_near_letter_quality)
#define enter_normal_quality		tcgetstr (m_enter_normal_quality)
#define enter_shadow_mode			tcgetstr (m_enter_shadow_mode)
#define enter_subscript_mode		tcgetstr (m_enter_subscript_mode)
#define enter_superscript_mode		tcgetstr (m_enter_superscript_mode)
#define enter_upward_mode			tcgetstr (m_enter_upward_mode)
#define exit_doublewide_mode		tcgetstr (m_exit_doublewide_mode)
#define exit_italics_mode			tcgetstr (m_exit_italics_mode)
#define exit_leftward_mode			tcgetstr (m_exit_leftward_mode)
#define exit_micro_mode				tcgetstr (m_exit_micro_mode)
#define exit_shadow_mode			tcgetstr (m_exit_shadow_mode)
#define exit_subscript_mode			tcgetstr (m_exit_subscript_mode)
#define exit_superscript_mode		tcgetstr (m_exit_superscript_mode)
#define exit_upward_mode			tcgetstr (m_exit_upward_mode)
#define micro_column_address		tcgetstr (m_micro_column_address)
#define micro_down					tcgetstr (m_micro_down)
#define micro_left					tcgetstr (m_micro_left)
#define micro_right					tcgetstr (m_micro_right)
#define micro_row_address			tcgetstr (m_micro_row_address)
#define micro_up					tcgetstr (m_micro_up)
#define order_of_pins				tcgetstr (m_order_of_pins)
#define parm_down_micro				tcgetstr (m_parm_down_micro)
#define parm_left_micro				tcgetstr (m_parm_left_micro)
#define parm_right_micro			tcgetstr (m_parm_right_micro)
#define parm_up_micro				tcgetstr (m_parm_up_micro)
#define select_char_set				tcgetstr (m_select_char_set)
#define set_bottom_margin			tcgetstr (m_set_bottom_margin)
#define set_bottom_margin_parm		tcgetstr (m_set_bottom_margin_parm)
#define set_left_margin_parm		tcgetstr (m_set_left_margin_parm)
#define set_right_margin_parm		tcgetstr (m_set_right_margin_parm)
#define set_top_margin				tcgetstr (m_set_top_margin)
#define set_top_margin_parm			tcgetstr (m_set_top_margin_parm)
#define start_bit_image				tcgetstr (m_start_bit_image)
#define start_char_set_def			tcgetstr (m_start_char_set_def)
#define stop_bit_image				tcgetstr (m_stop_bit_image)
#define stop_char_set_def			tcgetstr (m_stop_char_set_def)
#define subscript_characters		tcgetstr (m_subscript_characters)
#define superscript_characters		tcgetstr (m_superscript_characters)
#define these_cause_cr				tcgetstr (m_these_cause_cr)
#define zero_motion					tcgetstr (m_zero_motion)
#define char_set_names				tcgetstr (m_char_set_names)
#define key_mouse					tcgetstr (m_key_mouse)
#define mouse_info					tcgetstr (m_mouse_info)
#define req_mouse_pos				tcgetstr (m_req_mouse_pos)
#define get_mouse					tcgetstr (m_get_mouse)
#define set_a_foreground			tcgetstr (m_set_a_foreground)
#define set_a_background			tcgetstr (m_set_a_background)
#define pkey_plab					tcgetstr (m_pkey_plab)
#define device_type					tcgetstr (m_device_type)
#define code_set_init				tcgetstr (m_code_set_init)
#define set0_des_seq				tcgetstr (m_set0_des_seq)
#define set1_des_seq				tcgetstr (m_set1_des_seq)
#define set2_des_seq				tcgetstr (m_set2_des_seq)
#define set3_des_seq				tcgetstr (m_set3_des_seq)
#define set_lr_margin				tcgetstr (m_set_lr_margin)
#define set_tb_margin				tcgetstr (m_set_tb_margin)
#define bit_image_repeat			tcgetstr (m_bit_image_repeat)
#define bit_image_newline			tcgetstr (m_bit_image_newline)
#define bit_image_carriage_return	tcgetstr (m_bit_image_carriage_return)
#define color_names					tcgetstr (m_color_names)
#define define_bit_image_region		tcgetstr (m_define_bit_image_region)
#define end_bit_image_region		tcgetstr (m_end_bit_image_region)
#define set_color_band				tcgetstr (m_set_color_band)
#define set_page_length				tcgetstr (m_set_page_length)
#define display_pc_char				tcgetstr (m_display_pc_char)
#define enter_pc_charset_mode		tcgetstr (m_enter_pc_charset_mode)
#define exit_pc_charset_mode		tcgetstr (m_exit_pc_charset_mode)
#define enter_scancode_mode			tcgetstr (m_enter_scancode_mode)
#define exit_scancode_mode			tcgetstr (m_exit_scancode_mode)
#define pc_term_options				tcgetstr (m_pc_term_options)
#define scancode_escape				tcgetstr (m_scancode_escape)
#define alt_scancode_esc			tcgetstr (m_alt_scancode_esc)
#define enter_horizontal_hl_mode	tcgetstr (m_enter_horizontal_hl_mode)
#define enter_left_hl_mode			tcgetstr (m_enter_left_hl_mode)
#define enter_low_hl_mode			tcgetstr (m_enter_low_hl_mode)
#define enter_right_hl_mode			tcgetstr (m_enter_right_hl_mode)
#define enter_top_hl_mode			tcgetstr (m_enter_top_hl_mode)
#define enter_vertical_hl_mode		tcgetstr (m_enter_vertical_hl_mode)
#define set_a_attributes			tcgetstr (m_set_a_attributes)
#define set_pglen_inch				tcgetstr (m_set_pglen_inch)

/*------------------------------------------------------------------------
 * low-level setup routines
 */
extern int			setterm		(const char *term);
extern int			setupterm	(const char *term, int fd, int *errret);
extern int			restartterm	(const char *term, int fd, int *errret);

extern TERMINAL *	new_curterm (const char *term, int inp_fd, int out_fd,
									int *errret);
extern TERMINAL *	get_curterm	(void);
extern int			set_curterm	(TERMINAL *t);
extern int			del_curterm	(TERMINAL *t);

/*------------------------------------------------------------------------
 * attribute functions
 */
extern int			vidputs		(attr_t attrs, int (*prtn)(int c));
extern int			vidattr		(attr_t attrs);
extern int			putp		(const char *str);

/*------------------------------------------------------------------------
 * output a string (applying padding information)
 */
extern int			tputs		(const char *str, int cnt, int (*prtn)(int c));

/*------------------------------------------------------------------------
 * terminfo-specific functions
 */
extern int			tigetent	(char *bp, const char *term);

extern int			tigetflag	(const char *id);
extern int			tigetnum	(const char *id);
extern const char *	tigetstr	(const char *id);

extern char *		tparm		(const char *str, ...);

/*------------------------------------------------------------------------
 * termcap-specific functions
 */
extern int			tgetent		(char *bp, const char *name);

extern int			tgetflag	(const char *id);
extern int			tgetnum		(const char *id);
extern const char *	tgetstr		(const char *id, char **area);

extern char *		tgoto		(const char *str, int col, int row);

/*------------------------------------------------------------------------
 * database-independent get routines
 */
extern void			tcdumpent	(FILE *fp, const char *term);

extern int			tcgetflag	(int id);
extern int			tcgetnum	(int id);
extern const char *	tcgetstr	(int id);

extern char *		tcparm		(char *buf, const char *str, ...);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TERM_TERMCAP_H */
