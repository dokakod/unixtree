# ------------------------------------------------------------------------
# script to create/delete man entries for all ecurses functions
#
# This will enable a user to do a "man <func-name>" for any
# ecurses function. Your MANPATH should contain the parent of
# this directory in the list.
# ------------------------------------------------------------------------

# ------------------------------------------------------------------------
# usage
#
usage()
{
	echo "usage: $PGM [options] [func]"
	echo "options:"
	echo "  -a     add    all entries"
	echo "  -d     delete all entries"
	echo "  -l     list   all entries"
	echo "  -v     verbose"
	echo "func     show section for <func>"
}

# ------------------------------------------------------------------------
# output list of "section/function" lines
#
funclist ()
{
	cat <<-EOF
		ecurs_acs		blk_chr
		ecurs_acs		blk_ltr
		ecurs_acs		blk_str
		ecurs_acs		box_chr
		ecurs_acs		wblk_chr
		ecurs_acs		wblk_ltr
		ecurs_acs		wblk_str
		ecurs_acs		wbox_chr
		ecurs_addch		addch
		ecurs_addch		echochar
		ecurs_addch		mvaddch
		ecurs_addch		mvwaddch
		ecurs_addch		waddch
		ecurs_addch		wechochar
		ecurs_addchstr		addchnstr
		ecurs_addchstr		addchstr
		ecurs_addchstr		mvaddchnstr
		ecurs_addchstr		mvaddchstr
		ecurs_addchstr		mvwaddchnstr
		ecurs_addchstr		mvwaddchstr
		ecurs_addchstr		waddchnstr
		ecurs_addchstr		waddchstr
		ecurs_addstr		addnstr
		ecurs_addstr		addstr
		ecurs_addstr		addtstr
		ecurs_addstr		center
		ecurs_addstr		mvaddnstr
		ecurs_addstr		mvaddstr
		ecurs_addstr		mvaddtstr
		ecurs_addstr		mvwaddnstr
		ecurs_addstr		mvwaddstr
		ecurs_addstr		mvwaddtstr
		ecurs_addstr		waddnstr
		ecurs_addstr		waddstr
		ecurs_addstr		waddtstr
		ecurs_addstr		wcenter
		ecurs_alarm		win_alarm_active
		ecurs_alarm		win_alarm_check
		ecurs_alarm		win_alarm_clr
		ecurs_alarm		win_alarm_off
		ecurs_alarm		win_alarm_on
		ecurs_alarm		win_alarm_set
		ecurs_alarm		win_alarm_start
		ecurs_alarm		win_alarm_stop
		ecurs_alarm		win_alarm_trip
		ecurs_attr		attrget
		ecurs_attr		attroff
		ecurs_attr		attron
		ecurs_attr		attrset
		ecurs_attr		getattrs
		ecurs_attr		getbkgd
		ecurs_attr		getcode
		ecurs_attr		getstmode
		ecurs_attr		setbkgd
		ecurs_attr		setcode
		ecurs_attr		setcolor
		ecurs_attr		setstmode
		ecurs_attr		standend
		ecurs_attr		standget
		ecurs_attr		standout
		ecurs_attr		standset
		ecurs_attr		wattrget
		ecurs_attr		wattroff
		ecurs_attr		wattron
		ecurs_attr		wattrset
		ecurs_attr		wstandend
		ecurs_attr		wstandget
		ecurs_attr		wstandout
		ecurs_attr		wstandset
		ecurs_attrvals		get_attr_name_by_code
		ecurs_attrvals		get_attr_name_by_num
		ecurs_attrvals		get_attr_num_by_code
		ecurs_attrvals		get_attr_num_by_name
		ecurs_attrvals		get_attr_value_by_num
		ecurs_attrvals		get_bg_name_by_code
		ecurs_attrvals		get_bg_name_by_num
		ecurs_attrvals		get_bg_num_by_code
		ecurs_attrvals		get_bg_num_by_name
		ecurs_attrvals		get_bg_value_by_num
		ecurs_attrvals		get_char_name
		ecurs_attrvals		get_color_name_by_code
		ecurs_attrvals		get_default_attrs
		ecurs_attrvals		get_fg_name_by_code
		ecurs_attrvals		get_fg_name_by_num
		ecurs_attrvals		get_fg_num_by_code
		ecurs_attrvals		get_fg_num_by_name
		ecurs_attrvals		get_fg_value_by_num
		ecurs_attrvals		get_key_name
		ecurs_attrvals		get_key_value
		ecurs_beep		beep
		ecurs_beep		flash
		ecurs_bkgd		bkgd
		ecurs_bkgd		bkgdset
		ecurs_bkgd		wbkgd
		ecurs_bkgd		wbkgdset
		ecurs_border		border
		ecurs_border		box
		ecurs_border		box_around
		ecurs_border		hline
		ecurs_border		mvhline
		ecurs_border		mvvline
		ecurs_border		mvwhline
		ecurs_border		mvwvline
		ecurs_border		smart_box_chr
		ecurs_border		smart_wbox
		ecurs_border		vline
		ecurs_border		wborder
		ecurs_border		wbox
		ecurs_border		whline
		ecurs_border		wvline
		ecurs_caddch		caddch
		ecurs_caddch		mvcaddch
		ecurs_caddch		mvwcaddch
		ecurs_caddch		wcaddch
		ecurs_caddstr		caddnstr
		ecurs_caddstr		caddstr
		ecurs_caddstr		caddtstr
		ecurs_caddstr		ccenter
		ecurs_caddstr		mvcaddnstr
		ecurs_caddstr		mvcaddstr
		ecurs_caddstr		mvcaddtstr
		ecurs_caddstr		mvwcaddnstr
		ecurs_caddstr		mvwcaddstr
		ecurs_caddstr		mvwcaddtstr
		ecurs_caddstr		wcaddnstr
		ecurs_caddstr		wcaddstr
		ecurs_caddstr		wcaddtstr
		ecurs_caddstr		wccenter
		ecurs_clear		clear
		ecurs_clear		clrtobot
		ecurs_clear		clrtoeol
		ecurs_clear		erase
		ecurs_clear		wclear
		ecurs_clear		wclrtobot
		ecurs_clear		wclrtoeol
		ecurs_clear		werase
		ecurs_clock		win_clock
		ecurs_clock		win_clock_active
		ecurs_clock		win_clock_check
		ecurs_clock		win_clock_set
		ecurs_color		COLOR_PAIR
		ecurs_color		PAIR_NUMBER
		ecurs_color		can_change_color
		ecurs_color		color_content
		ecurs_color		has_colors
		ecurs_color		init_all_pairs
		ecurs_color		init_color
		ecurs_color		init_pair
		ecurs_color		pair_content
		ecurs_color		start_color
		ecurs_delch		delch
		ecurs_delch		mvdelch
		ecurs_delch		mvwdelch
		ecurs_delch		wdelch
		ecurs_deleteln		deleteln
		ecurs_deleteln		insdelln
		ecurs_deleteln		insertln
		ecurs_deleteln		wdeleteln
		ecurs_deleteln		winsdelln
		ecurs_deleteln		winsertln
		ecurs_exec		exec_argv
		ecurs_exec		exec_cmd
		ecurs_getch		chkkbd
		ecurs_getch		getch
		ecurs_getch		mgetch
		ecurs_getch		mvgetch
		ecurs_getch		mvwgetch
		ecurs_getch		ungetch
		ecurs_getch		wchkkbd
		ecurs_getch		wgetch
		ecurs_getch		wmgetch
		ecurs_getch		wungetch
		ecurs_getstr		getnstr
		ecurs_getstr		getstr
		ecurs_getstr		mvgetstr
		ecurs_getstr		mvwgetstr
		ecurs_getstr		wgetnstr
		ecurs_getstr		wgetstr
		ecurs_getyx		getbegx
		ecurs_getyx		getbegy
		ecurs_getyx		getcurx
		ecurs_getyx		getcury
		ecurs_getyx		getmaxx
		ecurs_getyx		getmaxy
		ecurs_getyx		getparx
		ecurs_getyx		getpary
		ecurs_getyx		setbegx
		ecurs_getyx		setbegy
		ecurs_getyx		setbegyx
		ecurs_getyx		setcurx
		ecurs_getyx		setcury
		ecurs_getyx		setcuryx
		ecurs_getyx		setmaxx
		ecurs_getyx		setmaxy
		ecurs_getyx		setmaxyx
		ecurs_getyx		setparx
		ecurs_getyx		setpary
		ecurs_getyx		setparyx
		ecurs_getyx		setyx
		ecurs_inch		chat
		ecurs_inch		inch
		ecurs_inch		mvinch
		ecurs_inch		mvwinch
		ecurs_inch		wchat
		ecurs_inch		winch
		ecurs_inchstr		inchnstr
		ecurs_inchstr		inchstr
		ecurs_inchstr		mvinchnstr
		ecurs_inchstr		mvinchstr
		ecurs_inchstr		mvwinchnstr
		ecurs_inchstr		mvwinchstr
		ecurs_inchstr		winchnstr
		ecurs_inchstr		winchstr
		ecurs_initscr		delscreen
		ecurs_initscr		end_curses
		ecurs_initscr		endscr
		ecurs_initscr		endscreen
		ecurs_initscr		endwin
		ecurs_initscr		get_curscr
		ecurs_initscr		get_screen
		ecurs_initscr		get_stdscr
		ecurs_initscr		get_term
		ecurs_initscr		get_term_db
		ecurs_initscr		get_term_db_desc
		ecurs_initscr		get_term_db_name
		ecurs_initscr		get_trmscr
		ecurs_initscr		get_usr_data
		ecurs_initscr		initscr
		ecurs_initscr		isendwin
		ecurs_initscr		newscreen
		ecurs_initscr		newscreen_defs
		ecurs_initscr		newterm
		ecurs_initscr		set_term
		ecurs_initscr		set_term_db
		ecurs_initscr		set_term_path
		ecurs_initscr		set_usr_data
		ecurs_inopts		addkey
		ecurs_inopts		cbreak
		ecurs_inopts		echo
		ecurs_inopts		getdelay
		ecurs_inopts		getkeypad
		ecurs_inopts		getnotimeout
		ecurs_inopts		halfdelay
		ecurs_inopts		intrflush
		ecurs_inopts		keypad
		ecurs_inopts		meta
		ecurs_inopts		metaoff
		ecurs_inopts		metaon
		ecurs_inopts		nlinp
		ecurs_inopts		nocbreak
		ecurs_inopts		nodelay
		ecurs_inopts		noecho
		ecurs_inopts		nonlinp
		ecurs_inopts		noqiflush
		ecurs_inopts		noraw
		ecurs_inopts		notimeout
		ecurs_inopts		qiflush
		ecurs_inopts		raw
		ecurs_inopts		setdelay
		ecurs_inopts		timeout
		ecurs_inopts		typeahead
		ecurs_inopts		wtimeout
		ecurs_insch		insch
		ecurs_insch		mvinsch
		ecurs_insch		mvwinsch
		ecurs_insch		winsch
		ecurs_insstr		insnstr
		ecurs_insstr		insstr
		ecurs_insstr		mvinsnstr
		ecurs_insstr		mvinsstr
		ecurs_insstr		mvwinsnstr
		ecurs_insstr		mvwinsstr
		ecurs_insstr		winsnstr
		ecurs_insstr		winsstr
		ecurs_instr		innstr
		ecurs_instr		instr
		ecurs_instr		mvinnstr
		ecurs_instr		mvinstr
		ecurs_instr		mvwinnstr
		ecurs_instr		mvwinstr
		ecurs_instr		winnstr
		ecurs_instr		winstr
		ecurs_intio		get_2byte
		ecurs_intio		get_4byte
		ecurs_intio		get_4time
		ecurs_intio		put_2byte
		ecurs_intio		put_4byte
		ecurs_intio		put_4time
		ecurs_kernel		curs_set
		ecurs_kernel		def_prog_mode
		ecurs_kernel		def_shell_mode
		ecurs_kernel		get_syx
		ecurs_kernel		napms
		ecurs_kernel		reset_prog_mode
		ecurs_kernel		reset_shell_mode
		ecurs_kernel		resetty
		ecurs_kernel		ripoffline
		ecurs_kernel		ripoffmline
		ecurs_kernel		savetty
		ecurs_kernel		set_syx
		ecurs_misc		blank_screen
		ecurs_misc		resize_screen
		ecurs_misc		set_async_key
		ecurs_misc		set_debug_kbd
		ecurs_misc		set_debug_scr
		ecurs_misc		set_event_rtn
		ecurs_misc		set_icon
		ecurs_misc		set_input_rtn
		ecurs_misc		set_output_rtn
		ecurs_misc		set_title
		ecurs_misc		win_snap
		ecurs_mouse		mouse_c
		ecurs_mouse		mouse_get_display
		ecurs_mouse		mouse_get_event
		ecurs_mouse		mouse_hide
		ecurs_mouse		mouse_init
		ecurs_mouse		mouse_is_in_win
		ecurs_mouse		mouse_setup
		ecurs_mouse		mouse_show
		ecurs_mouse		mouse_x
		ecurs_mouse		mouse_y
		ecurs_move		move
		ecurs_move		mvcur
		ecurs_move		wmove
		ecurs_outch		mvouch
		ecurs_outch		mvwouch
		ecurs_outch		ouch
		ecurs_outch		outch
		ecurs_outch		wouch
		ecurs_outch		woutch
		ecurs_outopts		clearok
		ecurs_outopts		get_valid_acs
		ecurs_outopts		getclear
		ecurs_outopts		getidc
		ecurs_outopts		getidl
		ecurs_outopts		getimmed
		ecurs_outopts		getleave
		ecurs_outopts		getscroll
		ecurs_outopts		idcok
		ecurs_outopts		idlok
		ecurs_outopts		immedok
		ecurs_outopts		leaveok
		ecurs_outopts		nl
		ecurs_outopts		nlout
		ecurs_outopts		nonl
		ecurs_outopts		nonlout
		ecurs_outopts		scrollok
		ecurs_outopts		set_valid_acs
		ecurs_outopts		setscrreg
		ecurs_outopts		wsetscrreg
		ecurs_overlay		copywin
		ecurs_overlay		overlap
		ecurs_overlay		overlay
		ecurs_overlay		overwrite
		ecurs_pad		newpad
		ecurs_pad		pad_new
		ecurs_pad		pechochar
		ecurs_pad		pnoutrefresh
		ecurs_pad		prefresh
		ecurs_pad		subpad
		ecurs_printer		print_win
		ecurs_printer		prt_close
		ecurs_printer		prt_open
		ecurs_printer		prt_output_fmt
		ecurs_printer		prt_output_str
		ecurs_printer		prt_output_var
		ecurs_printw		mvprintw
		ecurs_printw		mvwprintw
		ecurs_printw		printw
		ecurs_printw		vwprintw
		ecurs_printw		wprintw
		ecurs_refresh		doupdate
		ecurs_refresh		doupdateln
		ecurs_refresh		noutrefresh
		ecurs_refresh		noutrefreshln
		ecurs_refresh		redrawwin
		ecurs_refresh		refresh
		ecurs_refresh		refreshln
		ecurs_refresh		wnoutrefresh
		ecurs_refresh		wnoutrefreshln
		ecurs_refresh		wredrawln
		ecurs_refresh		wrefresh
		ecurs_refresh		wrefreshln
		ecurs_scanw		mvscanw
		ecurs_scanw		mvwscanw
		ecurs_scanw		scanw
		ecurs_scanw		vwscanw
		ecurs_scanw		wscanw
		ecurs_scrdump		scr_dump
		ecurs_scrdump		scr_init
		ecurs_scrdump		scr_restore
		ecurs_scrdump		scr_set
		ecurs_scroll		scrl
		ecurs_scroll		scroll
		ecurs_scroll		wscrl
		ecurs_scrollbar		scrollbar
		ecurs_signal		get_signal_data
		ecurs_signal		get_signal_rtn
		ecurs_signal		set_signal_rtn
		ecurs_slk		slk_attrbkgd
		ecurs_slk		slk_attroff
		ecurs_slk		slk_attron
		ecurs_slk		slk_attrset
		ecurs_slk		slk_clear
		ecurs_slk		slk_init
		ecurs_slk		slk_label
		ecurs_slk		slk_noutrefresh
		ecurs_slk		slk_refresh
		ecurs_slk		slk_restore
		ecurs_slk		slk_set
		ecurs_slk		slk_set_code
		ecurs_slk		slk_touch
		ecurs_termattrs		baudrate
		ecurs_termattrs		erasechar
		ecurs_termattrs		get_curr_attr
		ecurs_termattrs		get_curr_col
		ecurs_termattrs		get_curr_row
		ecurs_termattrs		get_max_cols
		ecurs_termattrs		get_max_rows
		ecurs_termattrs		get_orig_attr
		ecurs_termattrs		get_tab_size
		ecurs_termattrs		get_ttyname
		ecurs_termattrs		get_window_env
		ecurs_termattrs		has_ic
		ecurs_termattrs		has_il
		ecurs_termattrs		intrchar
		ecurs_termattrs		killchar
		ecurs_termattrs		longname
		ecurs_termattrs		quitchar
		ecurs_termattrs		termattrs
		ecurs_termattrs		termname
		ecurs_termattrs		termtype
		ecurs_touch		is_linetouched
		ecurs_touch		is_wintouched
		ecurs_touch		touchline
		ecurs_touch		touchwin
		ecurs_touch		untouchwin
		ecurs_touch		wlntouch
		ecurs_touch		wtouchln
		ecurs_util		delay_output
		ecurs_util		filter
		ecurs_util		flushinp
		ecurs_util		getwin
		ecurs_util		keyname
		ecurs_util		putwin
		ecurs_util		unctrl
		ecurs_util		use_env
		ecurs_window		delwin
		ecurs_window		derwin
		ecurs_window		dupwin
		ecurs_window		getsync
		ecurs_window		mvderwin
		ecurs_window		mvwin
		ecurs_window		newwin
		ecurs_window		subwin
		ecurs_window		syncok
		ecurs_window		wcursyncup
		ecurs_window		wsyncdown
		ecurs_window		wsyncup
	EOF
}

# ------------------------------------------------------------------------
# create all function entries
#
add_entries ()
{
	if [ -d "c:/" ]
	then
		LN="cp"		# Windows
	else
		LN="ln -s"	# Unix
	fi

	funclist |
	while read line
	do
		set $line
		sect=$1
		func=$2

		if [ ! -f $func ]
		then
			[ "$VERBOSE" = "yes" ] && echo "Adding $func ($sect)"
			$LN $sect $func
		fi
	done
}

# ------------------------------------------------------------------------
# delete all function entries
#
del_entries ()
{
	funclist |
	while read line
	do
		set $line
		sect=$1
		func=$2

		[ "$VERBOSE" = "yes" ] && echo "Deleting $func ($sect)"
		rm -f $func
	done
}

# ------------------------------------------------------------------------
# list all function entries
#
lst_entries ()
{
	if [ "$VERBOSE" = "yes" ]
	then
		echo	"Section     		Function"
		echo	"------------		----------------------"
	fi

	funclist
}

# ------------------------------------------------------------------------
# show function section
#
show_func()
{
	func="$1"

	sect=`funclist | grep "	$func$" | cut -f1`

	if [ "$sect" = "" ]
	then
		echo "$PGM: invalid function \"$func\"" >&2
		return 1
	fi

	echo "$sect"
}

# ========================================================================
# main
#
PGM=`basename $0`
CMD=
VERBOSE=no

# ------------------------------------------------------------------------
# parse options
#
ARGS=`getopt "adlvh:?" $*`
[ $? -ne 0 ] && { usage >&2; exit 1; }

set -- $ARGS

while [ "$1" != "" ]
do
	case "$1" in

	-a)	CMD=add;	shift;;
	-d)	CMD=del;	shift;;
	-l)	CMD=lst;	shift;;

	-v)	VERBOSE=yes
		shift
		;;

	'-?' | -help | --help)
		usage
		exit
		;;

	--)
		shift
		break
		;;

	-*)
		echo "$PGM: invalid option $1" >&2
		usage >&2
		exit 1
		;;

	*)
		break
		;;
	esac
done

FUNC="$1"

# ------------------------------------------------------------------------
# now do it
#
if [ "$FUNC" = "" ]
then
	case "$CMD" in

	add)	add_entries	;;
	del)	del_entries	;;
	lst)	lst_entries	;;

	esac
else
	show_func "$FUNC"
fi
