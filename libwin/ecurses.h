/*------------------------------------------------------------------------
 *
 * Extended curses (ecurses) library header.
 *
 * This library represents an extension to the "standard" curses library,
 * and is designed to be source-compatible with the standard library.
 * That is, any program written using standard curses should be able to
 * compile and link with this library and behave exactly the same.
 *
 * All functions here which are also in standard curses are the same
 * in calling syntax and behavior, but there are many extensions which
 * are not in standard curses.  Also, the same "defines" are present here,
 * but their values may differ from standard curses.
 *
 * Note that this results in this library being *source* compatible with
 * standard curses, but not *binary* compatible, i.e., you cannot link it
 * with a program which was compiled using the "standard" curses headers.
 *
 *------------------------------------------------------------------------
 *
 * Some major differences between this implementation of curses and
 * "standard" curses packages are:
 *
 * 1.	This library has *no* dependencies on any other library, such as
 *		libcurses, libncurses, libtermcap, etc. (except for libc).  Thus,
 *		it will work on any O/S, regardless of any installed "packages",
 *		and behave consistently everywhere.
 *
 * 2.	Unlike "standard" curses, which is O/S vendor supplied and can
 *		differ in implementation and capabilities, this library provides
 *		a consistent interface which is O/S independent.
 *
 * 3.	Unlike "standard" curses, which uses many global variables, this
 *		library is designed to be thread-safe and provides extended calls
 *		for most functions for use in a multi-threaded environment.
 *
 * 4.	This library will transparently use either the termcap or terminfo
 *		databases, depending on what it finds.  It also supports the use
 *		of "termfiles", in which a user can override any database entries
 *		or specify other, extended, capabilities not included in either
 *		termcap or terminfo.
 *
 * 5.	This implementation assumes a "PC-style" keyboard, which is now
 *		(practically) universal on all machines.  Thus, defines are provided
 *		only for key-names which are present on a PC-style keyboard.
 *		Support is provided for control, shift, and alt key modifiers,
 *		and for up to 16 function keys (in each mode).
 *
 * 6.	Color support is standardized for 16 colors (the 8 basic colors
 *		and 8 "bright" colors) for both the foreground and background
 *		colors.
 *
 * 7.	Wide (16-bit) characters are not supported.
 *
 * 8.	Mouse support is provided, but the interface is different from
 *		other implementations.  This *should* not be a problem, since most
 *		all curses implementations which support the mouse do it their
 *		own way, so there is no "standard" (that I could find, anyway).
 *		Functions are provided to enable the library to transparently
 *		track & handle the mouse with no calling-program intervention.
 *
 * 9.	Extensive debugging capabilities are provided, including the
 *		ability to take "snapshots" of the screen, and to playback
 *		log files for offline analysis.
 *
 * 10.	This header assumes the use of an ANSI C compiler.
 *
 *------------------------------------------------------------------------
 */
#ifndef CURSES_H
#define CURSES_H

/*------------------------------------------------------------------------
 * system headers used
 */
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*@ +++API+++ */
/*------------------------------------------------------------------------
 * curses sub-headers
 */
#include "termkeys.h"		/* defines KEY_*  stuff	*/
#include "termattrs.h"		/* defines attr_t stuff	*/
#include "wchtype.h"		/* defines chtype stuff	*/
#include "wbox.h"			/* defines ACS    stuff	*/
/*@ ---API--- */

/*------------------------------------------------------------------------
 * This is the "official" define for testing for the presence of the
 * ecurses library.
 */
#define ECURSES_VERSION			"1.0"

#define ECURSES_VERSION_MAJOR	1
#define ECURSES_VERSION_MINOR	0

/*------------------------------------------------------------------------
 * Miscellaneous defines for compatability with ncurses
 */
#ifndef NCURSES_API
#  define NCURSES_API			/* nada */
#  define NCURSES_IMPEXP		/* nada */
#  define NCURSES_CONST			const
#  define NCURSES_EXPORT(x)		x
#  define NCURSES_EXPORT_VAR(x)	x
#endif

/*------------------------------------------------------------------------
 * Miscellaneous defines mandated by "curses"
 */

/*------------------------------------------------------------------------
 * The "bool" type
 *
 *	Although the curses "standard" mandates the definition of a "bool" type,
 * "bool" is a built-in type in C++ (per ANSI standard) and defining
 * (or using) this type *may* cause problems in a C++ environment.
 *
 *	Therefore, we only define "bool" if we are NOT in C++, and we don't
 *	use this type ourselves, but only make it available to the user.
 *
 *	For this reason also, we use a "define" here, rather than a "typedef".
 *	If your code uses the "bool" type and you have previously declared
 *	it via a typedef, then define BOOL_T to inhibit our define of it.
 *
 *	Note that this header defines the "bool" type as an int, rather than
 * a "char" which some implementations use.  This follows the C++ usage.
 */
#if ! defined(bool) && ! defined(__cplusplus) && ! defined(BOOL_T)
#  define bool		int
#endif

/*------------------------------------------------------------------------
 * Definitions of TRUE & FALSE
 *
 *	This library does not depend on TRUE being the value 1.  It may
 *	be any positive non-zero value.  But FALSE *must* be the value 0.
 *
 *	If these names were previously defined, they better be be set to
 *	the values positive non-zero & zero.
 *
 *	Note, however, that if TRUE is not (1), then a statement such as
 *	( ((x=2) > 1) == TRUE ) will fail, since the compiler will assume
 *	that ( ((x=2) > 1) == (1) ).
 */
#if ! defined(TRUE)
#  define TRUE		(1)
#endif

#if ! defined(FALSE)
#  define FALSE		(0)
#endif

/*------------------------------------------------------------------------
 * Definitions of OK & ERR
 *
 *	Routines which return an int MAY return values other than OK if
 *	they succeed, but they will ALWAYS return ERR if they fail.
 *
 *	Therefore, if you are checking the return codes from these routines,
 *	don't assume success only if OK is returned, but rather assume failure
 *	if ERR is returned.
 *
 *	In other words, don't do the following:
 *
 *		if ( <int_rtn>(...) != OK )		// WRONG way
 *		{ ... }
 *
 *	but do the following:
 *
 *		if ( <int_rtn>(...) == ERR )	// RIGHT way
 *		{ ... }
 *
 *	Routines which return a pointer will always return (0) upon failure:
 *
 *		if ( <ptr_rtn>(...) == 0 )
 *		{ ... }
 *
 *	Note that if OK and ERR were previously defined, they *better* have
 *	the values (0) and (-1) or all hell will break loose.
 */
#if ! defined(OK)
#  define OK		(0)
#endif

#if ! defined(ERR)
#  define ERR		(-1)
#endif

/*------------------------------------------------------------------------
 * SCREEN struct
 *
 * This is an opaque struct which contains all the low-level data needed
 * for a given screen (and terminal type).
 */
#ifndef SCREEN_T
#  define SCREEN_T		TRUE		/* indicate SCREEN defined		*/
typedef struct screen	SCREEN;		/* opaque SCREEN struct pointer	*/
#endif

/*------------------------------------------------------------------------
 * WINDOW typedef
 *
 * We provide this as a forward reference so we can use it in
 * the following typedefs.
 */
#ifndef WINDOW_T
#  define WINDOW_T		TRUE		/* indicate WINDOW defined		*/
typedef	struct _win_st	WINDOW;
#endif

/*------------------------------------------------------------------------
 * clock routine definitions
 *
 * This routine is a call-back routine that is called asyncronously with
 * the local time as described below.  Note that a "tm" struct is not used,
 * since the values for the year and month are different.
 *
 * This routine does not need to erase or refresh the window (that will
 * done by the calling routine).
 *
 * This routine is specified in the call to win_clock().
 */
#ifndef WIN_CLOCK_T
typedef void	WIN_CLOCK_RTN	(WINDOW *	win,	/* window			*/
									void *	data,	/* user data		*/
									int		yr,		/* year:      yyyy	*/
									int		mo,		/* month:     1-12	*/
									int		da,		/* day:       1-31	*/
									int		hh,		/* hour:      0-23	*/
									int		mm,		/* minute:    0-59	*/
									int		ss,		/* second:    0-59	*/
									int		wday,	/* day of wk: 0-6	*/
									int		yday);	/* day of yr: 0-366	*/

#define WIN_CLOCK_T		TRUE		/* WIN_CLOCK_RTN defined			*/
#endif

/*------------------------------------------------------------------------
 * async key definitions
 *
 * This defines a call-back routine that is called whenever the specified
 * key is input.
 *
 * It is expected to return the following:
 *
 *	   0	consume this key stroke (don't pass it up the line).
 *	!= 0	key value to be passed up to caller of the input routine.
 *
 * This routine is specified in the call to win_async_key().
 */
typedef int		WIN_ASYNC_KEY_RTN	(int key, void *data);

/*------------------------------------------------------------------------
 * ripoffline definitions
 *
 * The WIN_RIPOFF_RTN is specified in the call to ripoffline(), which the
 * user can call before the call to initscr() to "reserve" lines on the
 * screen.
 *
 * This routine is called by initscr() after all "standard" windows
 * are created.  It may NOT call wrefresh() or doupdate(), but it MAY
 * call wnoutrefresh() on the window passed to it.
 *
 * The "standard" curses package assumes that "win" is a one-line window,
 * and so the routine is called with only the number of columns.
 * However, in "ecurses", the user may call ripoffmline(), which will
 * reserve a multiple-line window.
 *
 * Therefore, we recommend that the user ignore the "cols" argument,
 * and use getmaxy(win) & getmaxx(win) instead to get the number of
 * lines & columns in the window.
 *
 * The return value from this routine is ignored.  It is an int for
 * compatability reasons.
 */
typedef int		WIN_RIPOFF_RTN		(WINDOW *win, int cols);

#define MAX_RIPOFFS		5			/* max number of ripoffs allowed */

/*------------------------------------------------------------------------
 * WINDOW struct
 *
 * Note that this struct should NEVER be instantiated (via "WINDOW foo"),
 * but should only be created on the heap via newwin(), subwin(), etc.
 *
 * A call to initscr(), newterm(), or newscreen() creates three
 * "standard" windows:
 *
 *		stdscr	A default screen available for user display.  This is the
 *				"default" window to output to.
 *
 *				This window is the size of the terminal screen (minus any
 *				lines "reserved" by the ripoffline() routine).
 *
 *		curscr	An internal screen which all wrefresh() calls use
 *				to create an image of what "should" be displayed on
 *				the terminal screen.
 *
 *				This window is always the size of the terminal screen.
 *
 *		trmscr	An internal screen which all doupdate() calls use
 *				to create an image of what is actually currently displayed
 *				on the terminal screen.
 *
 *				This window is always the size of the terminal screen.
 *
 * The WINDOW struct *should* be considered opaque, with any access to
 * data in the window done via the various accessor macros (or functions)
 * defined below.
 */
struct _win_st
{
	/*--------------------------------------------------------------------
	 * window geometry
	 */
	int				_maxy,  _maxx;	/* max coordinates				*/
	int				_begy,  _begx;	/* offset from (0,0)			*/
	int				_cury,  _curx;	/* current coordinates			*/
	int				_pary,  _parx;	/* offset from parent			*/
	int				_tmarg, _bmarg;	/* top/bot scrolling margin		*/

	/*--------------------------------------------------------------------
	 * pad display info (set by pnoutrefresh(), used by pechochar())
	 */
	int				_ppy, _ppx;		/* last pad display position	*/
	int				_psy, _psx;		/* last pad display offset beg	*/
	int				_ply, _plx;		/* last pad display offset end	*/

	/*--------------------------------------------------------------------
	 * window flags
	 */
	unsigned int	_flags;			/* window flags					*/

	int				_clear;			/* clearok()   flag				*/
	int				_leaveit;		/* leaveok()   flag				*/
	int				_scroll;		/* scrollok()  flag				*/
	int				_immed;			/* immedok()   flag				*/
	int				_sync;			/* syncok()    flag				*/
	int				_use_keypad;	/* keypad()    flag				*/
	int				_use_idc;		/* idcok()     flag				*/
	int				_use_idl;		/* idlok()     flag				*/
	int				_notimeout;		/* notimeout() flag				*/
	int				_stmode;		/* TRUE if in standout mode		*/

	/*--------------------------------------------------------------------
	 * window attribute data
	 */
	int				_code;			/* current char code			*/
	attr_t			_attrs;			/* normal   attributes			*/
	attr_t			_attro;			/* standout attributes			*/
	chtype			_bkgd;			/* bg char, normally blank		*/

	/*--------------------------------------------------------------------
	 * keyboard input data
	 */
	int				_delay;			/* delay period for wgetch()	*/
									/* < 0  infinite delay			*/
									/* = 0  no delay				*/
									/* > 0  delay time in msecs		*/

	/*--------------------------------------------------------------------
	 * window line data
	 */
	chtype **		_y;				/* ptr to array of lines		*/
	int *			_firstch;		/* ptr to array of indexes		*/
	int *			_lastch;		/* ptr to array of indexes		*/

	/*--------------------------------------------------------------------
	 * sub-window data
	 */
	WINDOW *		_parent;		/* ptr to parent if sub_window	*/
	int				_ndescs;		/* number of sub-wins if parent	*/

	/*--------------------------------------------------------------------
	 * screen data
	 */
	SCREEN *		_screen;		/* ptr to screen				*/
};

/*------------------------------------------------------------------------
 * bits in the _flags parameter of WINDOW
 */
#define _ISPAD		0x0001			/* window is a pad				*/
#define	_ENDLINE	0x0002			/* window goes to right edge	*/
#define _FULLINE	0x0004			/* window is full width			*/
#define	_FULLWIN	0x0008			/* window fills the screen		*/

/*------------------------------------------------------------------------
 * _firstch & _lastch arrays
 *
 * These arrays contain the index of the first & last character in
 * a line which has been changed since the last refresh of that line.
 *
 * The value _NOCHANGE indicates that this line has not been changed.
 */
#define _NOCHANGE	-1				/* line has not changed			*/

/*------------------------------------------------------------------------
 * global "variables"
 *
 * Note these "variables" are all actually macros (unlike "standard" curses).
 * Therefore, they should never be used as an lvalue but only as an rvalue
 * (i.e. the user should only *get* these values, never *set* them).
 *
 * Note also that LINES & COLS will contain the size of the terminal screen,
 * and will be the size of curscr & trmscr, but will NOT necessarily be
 * the size of stdscr (use getmaxx(stdscr) & getmaxy(stdscr) for those
 * values).
 */
#define LINES	get_max_rows()		/* number of lines on screen	*/
#define COLS	get_max_cols()		/* number of rows  on screen	*/
#define TABSIZE	get_tab_size()		/* width  of tabs  on screen	*/

#define stdscr	get_stdscr()		/* standard screen pointer		*/
#define curscr	get_curscr()		/* current  screen pointer		*/
#define trmscr	get_trmscr()		/* terminal screen pointer		*/

/*------------------------------------------------------------------------
 * screen database types (used in term_db routines below)
 */
#define SCREEN_DB_N_AUTO		0	/* auto-detect database type	*/
#define SCREEN_DB_N_TERMFILES	1	/* use termfiles only			*/
#define SCREEN_DB_N_TERMINFO	2	/* (Unix) use terminfo database	*/
#define SCREEN_DB_N_TERMCAP		3	/* (Unix) use termcap  database	*/
#define SCREEN_DB_N_WINDOWS		4	/* (Windows) use std console	*/

#define SCREEN_DB_S_AUTO		"auto"			/* Unix default		*/
#define SCREEN_DB_S_TERMFILES	"termfiles"
#define SCREEN_DB_S_TERMINFO	"terminfo"
#define SCREEN_DB_S_TERMCAP		"termcap"
#define SCREEN_DB_S_WINDOWS		"windows"		/* Windows default	*/

/*------------------------------------------------------------------------
 * initialization functions (ecurs_initscr)
 */
extern WINDOW *	initscr			(void);
extern int		endscr			(int exiting);
extern int		isendwin		(void);
extern void		end_curses		(void);

extern SCREEN *	set_term		(SCREEN *s);
extern SCREEN *	get_term		(void);
extern SCREEN *	newterm			(const char *term,
									FILE *out_fp, FILE *inp_fp);
extern SCREEN *	newscreen		(const char *term,
									int   out_fd, int   inp_fd);
extern SCREEN *	newscreen_defs	(const char *term, const char **defs,
									int   out_fd, int   inp_fd);
extern void		delscreen		(SCREEN *s);
extern int		endscreen		(SCREEN *s, int exiting);
extern int		scrn_isendwin	(const SCREEN *s);

extern SCREEN *	get_screen		(WINDOW *win);

extern void *	set_usr_data	(SCREEN *s, void *usrdata, int on_heap);
extern void *	get_usr_data	(SCREEN *s);

extern void		set_term_path	(const char *path);
extern int		set_term_db		(const char *db_type);

extern const char *	get_term_db			(void);
extern const char *	get_term_db_name	(int type);
extern const char *	get_term_db_desc	(int type);

#if NOMACROS
extern int		endwin			(void);
#else
#  define		endwin()		endscr(FALSE)
#endif

/* the following use the current screen */

extern WINDOW *	get_stdscr		(void);
extern WINDOW *	get_curscr		(void);
extern WINDOW *	get_trmscr		(void);

/* the following use the specified screen */

extern WINDOW *	scrn_stdscr		(SCREEN *s);
extern WINDOW *	scrn_curscr		(SCREEN *s);
extern WINDOW *	scrn_trmscr		(SCREEN *s);

/*------------------------------------------------------------------------
 * window functions (ecurs_window)
 */
extern WINDOW *	scrn_newwin		(SCREEN *s, int rows, int cols, int y, int x);
extern WINDOW *	dupwin			(WINDOW *win);
extern WINDOW *	derwin			(WINDOW *win, int rows, int cols,
									int y, int x);
extern WINDOW *	subwin			(WINDOW *win, int rows, int cols,
									int y, int x);
extern WINDOW *	newwin			(             int rows, int cols,
									int y, int x);
extern int		mvderwin		(WINDOW *win, int par_y, int par_x);
extern int		delwin			(WINDOW *win);
extern int		mvwin			(WINDOW *win, int y, int x);
extern void		wsyncup			(WINDOW *win);
extern void		wcursyncup		(WINDOW *win);
extern void		wsyncdown		(WINDOW *win);

#if NOMACROS
extern int		syncok			(WINDOW *w, int bf);
extern int		getsync			(WINDOW *w);
#else
#  define		syncok(w,bf)	(((w)->_sync = (bf) ? TRUE : FALSE), OK)
#  define		getsync(w)		((w)->_sync)
#endif

/*------------------------------------------------------------------------
 * curses kernel routines (ecurs_kernel)
 */
extern int		ripoffline		(int line, WIN_RIPOFF_RTN *init);
extern int		ripoffmline		(int line, WIN_RIPOFF_RTN *init);
extern int		napms			(int ms);

/* the following use the current screen */

extern int		curs_set				(int visibility);
extern int		get_syx					(int *y, int *x);
extern int		set_syx					(int  y, int  x);

#define			getsyx(y,x)				get_syx(&(y), &(x))
#define			setsyx(y,x)				set_syx( (y),  (x))

extern int		def_shell_mode			(void);
extern int		reset_shell_mode		(void);

extern int		def_prog_mode			(void);
extern int		reset_prog_mode			(void);

extern int		savetty					(void);
extern int		resetty					(void);

/* the following use the specified screen */

extern int		scrn_curs_set			(SCREEN *s, int visibility);
extern int		scrn_get_syx			(const SCREEN *s, int *y, int *x);
extern int		scrn_set_syx			(SCREEN *s, int  y, int  x);

extern int		scrn_def_shell_mode		(SCREEN *s);
extern int		scrn_reset_shell_mode	(SCREEN *s);

extern int		scrn_def_prog_mode		(SCREEN *s);
extern int		scrn_reset_prog_mode	(SCREEN *s);

extern int		scrn_savetty			(SCREEN *s);
extern int		scrn_resetty			(SCREEN *s);

/*------------------------------------------------------------------------
 * window background functions (ecurs_bkgd)
 */
extern void		wbkgdset		(WINDOW *win, chtype ch);
extern int		wbkgd			(WINDOW *win, chtype ch);

#if NOMACROS
extern int		bkgd			(chtype ch);
extern void		bkgdset			(chtype ch);
#else
#  define		bkgd(ch)		wbkgd(stdscr,ch)
#  define		bkgdset(ch)		wbkgdset(stdscr,ch)
#endif

/*------------------------------------------------------------------------
 * cursor move functions (ecurs_move)
 */
extern int		wmove			(WINDOW *win, int y, int x);

#if NOMACROS
extern int		move			(int y, int x);
#else
#  define		move(y,x)		wmove(stdscr,y,x)
#endif

/* the following use the current screen */

extern int		mvcur			(int old_y, int old_x, int new_y, int new_x);

/* the following use the specified screen */

extern int		scrn_mvcur		(SCREEN *s,
								 int old_y, int old_x, int new_y, int new_x);

/*------------------------------------------------------------------------
 * window clear functions (ecurs_clear)
 */
extern int		wclrtobot		(WINDOW *win);
extern int		wclrtoeol		(WINDOW *win);

#if NOMACROS
extern int		werase			(WINDOW *w);
extern int		wclear			(WINDOW *w);

extern int		clear			(void);
extern int		clrtobot		(void);
extern int		clrtoeol		(void);
extern int		erase			(void);
#else
#  define		werase(w)		(wmove(w,0,0), wclrtobot(w))
#  define		wclear(w)		(clearok(w,TRUE), werase(w))

#  define		clear()			wclear(stdscr)
#  define		clrtobot()		wclrtobot(stdscr)
#  define		clrtoeol()		wclrtoeol(stdscr)
#  define		erase()			werase(stdscr)
#endif

/*------------------------------------------------------------------------
 * alternate-char functions (ecurs_acs)
 */
extern int		wbox_chr		(WINDOW *win, int c, int type);
extern int		wblk_chr		(WINDOW *win, int b, int c);
extern int		wblk_str		(WINDOW *win, int y, int x, const char *str,
									int type);
extern int		wblk_ltr		(WINDOW *win, int y, int x, int c, int type);

#if NOMACROS
extern int		box_chr			(int c, int type);
extern int		blk_chr			(int b, int c);
extern int		blk_str			(int y, int x, const char *str, int type);
extern int		blk_ltr			(int y, int x, int c, int type);
#else
#  define		box_chr(c,t)		wbox_chr(stdscr,c,t)
#  define		blk_chr(b,c)		wblk_chr(stdscr,b,c)
#  define		blk_str(y,x,s,t)	wblk_str(stdscr,y,x,s,t)
#  define		blk_ltr(y,x,c,t)	wblk_ltr(stdscr,y,x,c,t)
#endif

/*------------------------------------------------------------------------
 * box/line functions (ecurs_border)
 */
extern int		wborder			(WINDOW *win, chtype ls, chtype rs,
									chtype ts, chtype bs,
									chtype tl, chtype tr,
									chtype bl, chtype br);
extern int		wbox			(WINDOW *win, int type);
extern int		box_around		(WINDOW *win, WINDOW *brdr_win, int type);
extern int		smart_box_chr	(WINDOW *win, int y, int x, int c, int type);
extern int		smart_wbox		(WINDOW *win, int y_start, int x_start,
									int rows, int cols, int type);
extern int		whline			(WINDOW *win, chtype c, int n);
extern int		wvline			(WINDOW *win, chtype c, int n);

#if NOMACROS
extern int		box				(WINDOW *w, chtype v, chtype h);

extern int		border			(chtype ls, chtype rs,
									chtype ts, chtype bs,
									chtype tl, chtype tr,
									chtype bl, chtype br);

extern int		hline			(chtype c, int n);
extern int		vline			(chtype c, int n);

extern int		mvwhline		(WINDOW *w, int y, int x, chtype c, int n);
extern int		mvhline			(           int y, int x, chtype c, int n);

extern int		mvwvline		(WINDOW *w, int y, int x, chtype c, int n);
extern int		mvvline			(           int y, int x, chtype c, int n);
#else
#  define		box(w,v,h)		wborder(w,v,v,h,h,0,0,0,0)

#  define		border(ls,rs,ts,bs,tl,tr,bl,br) \
								wborder(stdscr,ls,rs,ts,bs,tl,tr,bl,br)
#  define		hline(c,n)		whline(stdscr,c,n)
#  define		vline(c,n)		wvline(stdscr,c,n)

#  define		mvwhline(w,y,x,c,n)		(wmove(w,y,x) == ERR ? ERR : \
											whline(w,c,n))
#  define		mvhline(y,x,c,n)		mvwhline(stdscr,y,x,c,n)

#  define		mvwvline(w,y,x,c,n)		(wmove(w,y,x) == ERR ? ERR : \
											wvline(w,c,n))
#  define		mvvline(y,x,c,n)		mvwvline(stdscr,y,x,c,n)
#endif

/*------------------------------------------------------------------------
 * output a char and attributes (ecurs_addch)
 */
extern int		waddch			(WINDOW *win, chtype ch);
extern int		wechochar		(WINDOW *win, chtype ch);

#if NOMACROS
extern int		addch			(chtype ch);
extern int		echochar		(chtype ch);

extern int		mvwaddch		(WINDOW *w, int y, int x, chtype ch);
extern int		mvaddch			(           int y, int x, chtype ch);
#else
#  define		addch(ch)		waddch(stdscr,ch)
#  define		echochar(ch)	wechochar(stdscr,ch)

#  define		mvwaddch(w,y,x,ch)		(wmove(w,y,x) == ERR ? ERR : \
											waddch(w,ch))
#  define		mvaddch(y,x,ch)			mvwaddch(stdscr,y,x,ch)
#endif

/*------------------------------------------------------------------------
 * output a char, attributes, and code (ecurs_caddch)
 */
extern int		wcaddch			(WINDOW *win, chtype ch, int code);

#if NOMACROS
extern int		caddch			(chtype ch, int code);

extern int		mvwcaddch		(WINDOW *w, int y, int x, chtype ch, int code);
extern int		mvcaddch		(           int y, int x, chtype ch, int code);
#else
#  define		caddch(ch,code) wcaddch(stdscr,ch,code)

#  define		mvwcaddch(w,y,x,ch,c)	(wmove(w,y,x) == ERR ? ERR : \
											wcaddch(w,ch,c))
#  define		mvcaddch(y,x,ch,c)		mvwcaddch(stdscr,y,x,ch,c)
#endif

/*------------------------------------------------------------------------
 * output a char string (ecurs_addstr)
 */
extern int		waddnstr		(WINDOW *win, const char *s, int n);
extern int		waddstr			(WINDOW *win, const char *s);
extern int		waddtstr		(WINDOW *win, const char *s, int c);
extern int		wcenter			(WINDOW *win, const char *s, int y);

#if NOMACROS
extern int		addnstr			(const char *s, int n);
extern int		addstr			(const char *s);
extern int		addtstr			(const char *s, int c);
extern int		center			(const char *s, int y);

extern int		mvwaddstr		(WINDOW *w, int y, int x, const char *s);
extern int		mvaddstr		(           int y, int x, const char *s);

extern int		mvwaddtstr		(WINDOW *w, int y, int x, const char *s, int c);
extern int		mvaddtstr		(           int y, int x, const char *s, int c);

extern int		mvwaddnstr		(WINDOW *w, int y, int x, const char *s, int n);
extern int		mvaddnstr		(           int y, int x, const char *s, int n);
#else
#  define		addnstr(s,n)	waddnstr(stdscr,s,n)
#  define		addstr(s)		waddstr(stdscr,s)
#  define		addtstr(s,c)	waddtstr(stdscr,s,c)
#  define		center(s,y)		wcenter(stdscr,s,y)

#  define		mvwaddstr(w,y,x,s)		(wmove(w,y,x) == ERR ? ERR : \
											waddstr(w,s))
#  define		mvaddstr(y,x,s)			mvwaddstr(stdscr,y,x,s)

#  define		mvwaddtstr(w,y,x,s,c)	(wmove(w,y,x) == ERR ? ERR : \
											waddtstr(w,s,c))
#  define		mvaddtstr(y,x,s,c)		mvwaddtstr(stdscr,y,x,s,c)

#  define		mvwaddnstr(w,y,x,s,n)	(wmove(w,y,x) == ERR ? ERR : \
											waddnstr(w,s,n))
#  define		mvaddnstr(y,x,s,n)		mvwaddnstr(stdscr,y,x,s,n)
#endif

/*------------------------------------------------------------------------
 * output a char string and code (ecurs_caddstr)
 */
extern int		wcaddnstr		(WINDOW *win, const char *s, int n, int code);
extern int		wcaddstr		(WINDOW *win, const char *s,        int code);
extern int		wccenter		(WINDOW *win, const char *s, int y, int code);

#if NOMACROS
extern int		caddnstr		(const char *s, int n, int code);
extern int		caddstr			(const char *s,        int code);
extern int		ccenter			(const char *s, int y, int code);

extern int		mvwcaddstr		(WINDOW *w, int y, int x, const char *s, int c);
extern int		mvcaddstr		(           int y, int x, const char *s, int c);

extern int		mvwcaddnstr		(WINDOW *w, int y, int x, const char *s, int n,
									int c);
extern int		mvcaddnstr		(           int y, int x, const char *s, int n,
									int c);
#else
#  define		caddnstr(s,n,c)	wcaddnstr(stdscr,s,n,c)
#  define		caddstr(s,c)	wcaddstr(stdscr,s,c)
#  define		ccenter(s,y,c)	wccenter(stdscr,s,y,c)

#  define		mvwcaddstr(w,y,x,s,c)	(wmove(w,y,x) == ERR ? ERR : \
											wcaddstr(w,s,c))
#  define		mvcaddstr(y,x,s,c)		mvcwaddstr(stdscr,y,x,s,c)

#  define		mvwcaddnstr(w,y,x,s,n,c) (wmove(w,y,x) == ERR ? ERR : \
											wcaddnstr(w,s,n,c))
#  define		mvcaddnstr(y,x,s,n,c)	mvcwaddnstr(stdscr,y,x,s,n,c)
#endif

/*------------------------------------------------------------------------
 * output a chtype string (ecurs_addchstr)
 */
extern int		waddchnstr		(WINDOW *win, const chtype *s, int n);

#if NOMACROS
extern int		waddchstr		(WINDOW *w, const chtype *s);

extern int		addchstr		(const chtype *s);
extern int		addchnstr		(const chtype *s, int n);

extern int		mvwaddchstr		(WINDOW *w, int y, int x, const chtype *s);
extern int		mvaddchstr		(           int y, int x, const chtype *s);

extern int		mvwaddchnstr	(WINDOW *w, int y, int x, const chtype *s,
									int n);
extern int		mvaddchnstr		(           int y, int x, const chtype *s,
									int n);
#else
#  define		waddchstr(w,s)	waddchnstr(w,s,-1)

#  define		addchstr(s)		waddchstr(stdscr,s)
#  define		addchnstr(s,n)	waddchnstr(stdscr,s,n)

#  define		mvwaddchstr(w,y,x,s)	(wmove(w,y,x) == ERR ? ERR : \
											waddchstr(w,s))
#  define		mvaddchstr(y,x,s)		mvwaddchstr(stdscr,y,x,s)

#  define		mvwaddchnstr(w,y,x,s,n)	(wmove(w,y,x) == ERR ? ERR : \
											waddchnstr(w,s,n))
#  define		mvaddchnstr(y,x,s,n)	mvwaddchnstr(stdscr,y,x,s,n)
#endif

/*------------------------------------------------------------------------
 * input a char (ecurs_getch)
 */
extern int		wgetch			(WINDOW *win);
extern int		wmgetch			(WINDOW *win);
extern int		wchkkbd			(WINDOW *win);
extern int		wungetch		(WINDOW *win, int c);

#if NOMACROS
extern int		getch			(void);
extern int		mgetch			(void);
extern int		chkkbd			(void);
extern int		ungetch			(int c);

extern int		mvwgetch		(WINDOW *w, int y, int x);
extern int		mvgetch			(           int y, int x);
#else
#  define		getch()			wgetch(stdscr)
#  define		mgetch()		wmgetch(stdscr)
#  define		chkkbd()		wchkkbd(stdscr)
#  define		ungetch(c)		wungetch(stdscr,c)

#  define		mvwgetch(w,y,x)			(wmove(w,y,x) == ERR ? ERR : \
											wgetch(w))
#  define		mvgetch(y,x)			mvwgetch(stdscr,y,x)
#endif

/*------------------------------------------------------------------------
 * input a char string (ecurs_getstr)
 */
extern int		wgetnstr		(WINDOW *win, char *s, int n);
extern int		wgetstr			(WINDOW *win, char *s);

#if NOMACROS
extern int		getnstr			(char *s, int n);
extern int		getstr			(char *s);

extern int		mvwgetstr		(WINDOW *w, int y, int x, char *s);
extern int		mvgetstr		(           int y, int x, char *s);
#else
#  define		getnstr(s,n)	wgetnstr(stdscr,s,n)
#  define		getstr(s)		wgetstr(stdscr,s)

#  define		mvwgetstr(w,y,x,s)		(wmove(w,y,x) == ERR ? ERR : \
											wgetstr(w,s))
#  define		mvgetstr(y,x,s)			mvwgetstr(stdscr,y,x,s)
#endif

/*------------------------------------------------------------------------
 * extract a chtype from a window (ecurs_inch)
 */
#if NOMACROS
extern chtype	wchat			(WINDOW *w, int y, int x);
extern chtype	chat			(           int y, int x);

extern chtype	winch			(WINDOW *w);
extern chtype	inch			(void);

extern chtype	mvwinch			(WINDOW *w, int y, int x);
extern chtype	mvinch			(           int y, int x);
#else
#  define		wchat(w,y,x)	((w)->_y[(y)][(x)])
#  define		chat(y,x)		wchat(stdscr,y,x)

#  define		winch(w)		wchat(w,getcury(w),getcurx(w))
#  define		inch()			winch(stdscr)

#  define		mvwinch(w,y,x)			(wmove(w,y,x) == ERR ? ERR : \
											winch(w))
#  define		mvinch(y,x)				mvwinch(stdscr,y,x)
#endif

/*------------------------------------------------------------------------
 * store a chtype into a window (ecurs_outch)
 */
#if NOMACROS
extern int		woutch			(WINDOW *w, int y, int x, chtype c);
extern int		outch			(           int y, int x, chtype c);

extern int		wouch			(WINDOW *w, chtype c);
extern int		ouch			(           chtype c);

extern int		mvwouch			(WINDOW *w, int y, int x, chtype c);
extern int		mvouch			(           int y, int x, chtype c);
#else
#  define		woutch(w,y,x,c)	((wchat(w,y,x) = (c)), OK)
#  define		outch(y,x,c)	woutch(stdscr,y,x,c)

#  define		wouch(w,c)		woutch(w,getcury(w),getcurx(w),c)
#  define		ouch(c)			wouch(stdscr,c)

#  define		mvwouch(w,y,x,c)		(wmove(w,y,x) == ERR ? ERR : \
											wouch(w,c))
#  define		mvouch(y,x,c)			mvwouch(stdscr,y,x,c)
#endif

/*------------------------------------------------------------------------
 * extract a char string from a window (ecurs_instr)
 */
extern int		winnstr			(WINDOW *win, char *s, int n);

#if NOMACROS
extern int		winstr			(WINDOW *w, char *s);

extern int		innstr			(char *s, int n);
extern int		instr			(char *s);

extern int		mvwinnstr		(WINDOW *w, int y, int x, char *s, int n);
extern int		mvinnstr		(           int y, int x, char *s, int n);

extern int		mvwinstr		(WINDOW *w, int y, int x, char *s);
extern int		mvinstr			(           int y, int x, char *s);
#else
#  define		winstr(w,s)		winnstr(w,s,-1)

#  define		innstr(s,n)		winnstr(stdscr,s,n)
#  define		instr(s)		winstr(stdscr,s)

#  define		mvwinnstr(w,y,x,s,n)	(wmove(w,y,x) == ERR ? ERR : \
											winnstr(w,s,n))
#  define		mvinnstr(y,x,s,n)		mvwinnstr(stdscr,y,x,s,n)

#  define		mvwinstr(w,y,x,s)		(wmove(w,y,x) == ERR ? ERR : \
											winstr(w,s))
#  define		mvinstr(y,x,s)			mvwinstr(stdscr,y,x,s)
#endif

/*------------------------------------------------------------------------
 * extract a chtype string from a window (ecurs_inchstr)
 */
extern int		winchnstr		(WINDOW *win, chtype *s, int n);

#if NOMACROS
extern int		winchstr		(WINDOW *w, chtype *s);

extern int		inchnstr		(chtype *s, int n);
extern int		inchstr			(chtype *s);

extern int		mvwinchnstr		(WINDOW *w, int y, int x, chtype *s, int n);
extern int		mvinchnstr		(           int y, int x, chtype *s, int n);

extern int		mvwinchstr		(WINDOW *w, int y, int x, chtype *s);
extern int		mvinchstr		(           int y, int x, chtype *s);
#else
#  define		winchstr(w,s)	winchnstr(w,s,-1)

#  define		inchnstr(s,n)	winchnstr(stdscr,s,n)
#  define		inchstr(s)		winchstr(stdscr,s)

#  define		mvwinchnstr(w,y,x,s,n)	(wmove(w,y,x) == ERR ? ERR : \
											winchnstr(w,s,n))
#  define		mvinchnstr(y,x,s,n)		mvwinchnstr(stdscr,y,x,s,n)

#  define		mvwinchstr(w,y,x,s)		(wmove(w,y,x) == ERR ? ERR : \
											winchstr(w,s))
#  define		mvinchstr(y,x,s)		mvwinchstr(stdscr,y,x,s)
#endif

/*------------------------------------------------------------------------
 * insert a chtype into a window (ecurs_insch)
 */
extern int		winsch			(WINDOW *win, chtype ch);

#if NOMACROS
extern int		insch			(chtype ch);

extern int		mvwinsch		(WINDOW *w, int y, int x, chtype ch);
extern int		mvinsch			(           int y, int x, chtype ch);
#else
#  define		insch(ch)		winsch(stdscr,ch)

#  define		mvwinsch(w,y,x,ch)		(wmove(w,y,x) == ERR ? ERR : \
											winsch(w,ch))
#  define		mvinsch(y,x,ch)			mvwinsch(stdscr,y,x,ch)
#endif

/*------------------------------------------------------------------------
 * insert a char string into a window (ecurs_insstr)
 */
extern int		winsnstr		(WINDOW *win, const char *s, int n);

#if NOMACROS
extern int		winsstr			(WINDOW *w, const char *s);

extern int		insnstr			(const char *s, int n);
extern int		insstr			(const char *s);

extern int		mvwinsnstr		(WINDOW *w, int y, int x, const char *s, int n);
extern int		mvinsnstr		(           int y, int x, const char *s, int n);

extern int		mvwinsstr		(WINDOW *w, int y, int x, const char *s);
extern int		mvinsstr		(           int y, int x, const char *s);
#else
#  define		winsstr(w,s)	winsnstr(w,s,-1)

#  define		insnstr(s,n)	winsnstr(stdscr,s,n)
#  define		insstr(s)		winsstr(stdscr,s)

#  define		mvwinsnstr(w,y,x,s,n)	(wmove(w,y,x) == ERR ? ERR : \
											winsnstr(w,s,n))
#  define		mvinsnstr(y,x,s,n)		mvwinsnstr(stdscr,y,x,s,n)

#  define		mvwinsstr(w,y,x,s)		(wmove(w,y,x) == ERR ? ERR : \
											winsstr(w,s))
#  define		mvinsstr(y,x,s)			mvwinsstr(stdscr,y,x,s)
#endif

/*------------------------------------------------------------------------
 * delete a char from a window (ecurs_delch)
 */
extern int		wdelch			(WINDOW *win);

#if NOMACROS
extern int		delch			(void);

extern int		mvwdelch		(WINDOW *w, int y, int x);
extern int		mvdelch			(           int y, int x);
#else
#  define		delch()			wdelch(stdscr)

#  define		mvwdelch(w,y,x)			(wmove(w,y,x) == ERR ? ERR : \
											wdelch(w))
#  define		mvdelch(y,x)			mvwdelch(stdscr,y,x)
#endif

/*------------------------------------------------------------------------
 * insert/delete lines in a window (ecurs_deleteln)
 */
extern int		wdeleteln		(WINDOW *win);
extern int		winsdelln		(WINDOW *win, int n);
extern int		winsertln		(WINDOW *win);

#if NOMACROS
extern int		deleteln		(void);
extern int		insdelln		(int n);
extern int		insertln		(void);
#else
#  define		deleteln()		wdeleteln(stdscr)
#  define		insdelln(n)		winsdelln(stdscr,n)
#  define		insertln()		winsertln(stdscr)
#endif

/*------------------------------------------------------------------------
 * alarm functions (ecurs_beep)
 */
/* the following use the current screen */

extern int		beep			(void);
extern int		flash			(void);

/* the following use the specified screen */

extern int		scrn_beep		(SCREEN *s);
extern int		scrn_flash		(SCREEN *s);

/*------------------------------------------------------------------------
 * manipulate overlapped windows (ecurs_overlay)
 */
extern int		copywin			(WINDOW *src, WINDOW *dst,
									int sminrow, int smincol,
									int dminrow, int dmincol,
									int dmaxrow, int dmaxcol, int flag);
extern int		overlap			(WINDOW *src, WINDOW *dst, int flag);

#if NOMACROS
extern int		overlay			(WINDOW *s, WINDOW *d);
extern int		overwrite		(WINDOW *s, WINDOW *d);
#else
#  define		overlay(s,d)	overlap(s,d,TRUE)
#  define		overwrite(s,d)	overlap(s,d,FALSE)
#endif

/*------------------------------------------------------------------------
 * scroll functions (ecurs_scroll)
 */
extern int		wscrl			(WINDOW *win, int n);

#if NOMACROS
extern int		scroll			(WINDOW *w);

extern int		scrl			(int n);
#else
#  define		scroll(w)		wscrl(w,1)

#  define		scrl(n)			wscrl(stdscr,n)
#endif

/*------------------------------------------------------------------------
 * refresh windows/lines (ecurs_refresh)
 */
extern int		wredrawln		(WINDOW *win, int y, int n);
extern int		wrefresh		(WINDOW *win);
extern int		wrefreshln		(WINDOW *win, int y);
extern int		wnoutrefresh	(WINDOW *win);
extern int		wnoutrefreshln	(WINDOW *win, int y);

/* the following use the current screen */

extern int		doupdate		(void);
extern int		doupdateln		(int y);

/* the following use the specified screen */

extern int		scrn_doupdate	(SCREEN *s);
extern int		scrn_doupdateln	(SCREEN *s, int y);


#if NOMACROS
extern int		redrawwin		(WINDOW *w);

extern int		noutrefresh		(void);
extern int		noutrefreshln	(int y);
extern int		refresh			(void);
extern int		refreshln		(int y);
#else
#  define		redrawwin(w)	wredrawln(w,0,getmaxy(w))

#  define		noutrefresh()		wnoutrefresh(stdscr)
#  define		noutrefreshln(y)	wnoutrefreshln(stdscr,y)
#  define		refresh()			wrefresh(stdscr)
#  define		refreshln(y)		wrefreshln(stdscr,y)
#endif

/*------------------------------------------------------------------------
 * refresh control functions (ecurs_touch)
 */
extern int		is_linetouched	(WINDOW *win, int y);
extern int		is_wintouched	(WINDOW *win);
extern int		wtouchln		(WINDOW *win, int y, int n, int flag);
extern int		wlntouch		(WINDOW *win, int y, int n, int xs, int xe);

#if NOMACROS
extern int		touchline		(WINDOW *w, int y, int n);
extern int		touchwin		(WINDOW *w);
extern int		untouchwin		(WINDOW *w);
#else
#  define		touchline(w,y,n) wtouchln(w,y,n,TRUE)
#  define		touchwin(w)		wtouchln(w,0,getmaxy(w),TRUE)
#  define		untouchwin(w)	wtouchln(w,0,getmaxy(w),FALSE)
#endif

/*------------------------------------------------------------------------
 * pad functions (ecurs_pad)
 */
extern WINDOW *	pad_new			(SCREEN *s, int nlines, int ncols);
extern WINDOW *	newpad			(int nlines, int ncols);
extern WINDOW *	subpad			(WINDOW *pad, int nlines, int ncols,
									int begin_y, int begin_x);
extern int		prefresh		(WINDOW *pad, int pminrow, int pmincol,
									int sminrow, int smincol,
									int smaxrow, int smaxcol);
extern int		pnoutrefresh	(WINDOW *pad, int pminrow, int pmincol,
									int sminrow, int smincol,
									int smaxrow, int smaxcol);
extern int		pechochar		(WINDOW *pad, chtype ch);

/*------------------------------------------------------------------------
 * soft-label-key functions (ecurs_slk)
 *
 * Note that, unlike "standard" curses, these routines allow for either
 * one or two lines of label data, and allow for different label layouts.
 * In all cases, however, the labels are numbered left-to-right,
 * top-to-bottom, starting with 1.
 */
#define SLK_LAYOUT_FMT_NONE		-1		/* no labels						*/
#define SLK_LAYOUT_FMT_1_323	0		/* 1 line : 3-2-3 pattern ( 8 keys) */
#define SLK_LAYOUT_FMT_1_44		1		/* 1 line : 4-4   pattern ( 8 keys) */
#define SLK_LAYOUT_FMT_2_222	2		/* 2 lines: 2-2-2 pattern (12 keys) */
#define SLK_LAYOUT_FMT_2_33		3		/* 2 lines: 3-3   pattern (12 keys) */
#define SLK_LAYOUT_FMT_2_323	4		/* 2 lines: 3-2-3 pattern (16 keys) */
#define SLK_LAYOUT_FMT_2_44		5		/* 2 lines: 4-4   pattern (16 keys) */

#define SLK_LABEL_FMT_LEFT		0		/* text in label is left-justified	*/
#define SLK_LABEL_FMT_CENTER	1		/* text in label is centered		*/
#define SLK_LABEL_FMT_RIGHT		2		/* text in label is right-justified	*/

#define SLK_LABEL_LEN			8		/* max chars in a label				*/

extern int		slk_init			(int layout_fmt);

/* the following use the current screen */

extern int		slk_set				(int labnum,
										const char *label, int label_fmt);
extern int		slk_set_code		(int labnum,
										const char *label, int label_fmt,
										int code);
extern char *	slk_label			(int labnum);

extern int		slk_attron			(attr_t attrs);
extern int		slk_attroff			(attr_t attrs);
extern int		slk_attrset			(attr_t attrs);
extern int		slk_attrbkgd		(attr_t attrs);

extern int		slk_refresh			(void);
extern int		slk_noutrefresh		(void);
extern int		slk_clear			(void);
extern int		slk_restore			(void);
extern int		slk_touch			(void);

/* the following use the specified screen */

extern int		scrn_slk_set		(SCREEN *s, int labnum,
										const char *label, int label_fmt);
extern int		scrn_slk_set_code	(SCREEN *s, int labnum,
										const char *label, int label_fmt,
										int code);
extern char *	scrn_slk_label		(SCREEN *s, int labnum);

extern int		scrn_slk_attron		(SCREEN *s, attr_t attrs);
extern int		scrn_slk_attroff	(SCREEN *s, attr_t attrs);
extern int		scrn_slk_attrset	(SCREEN *s, attr_t attrs);
extern int		scrn_slk_attrbkgd	(SCREEN *s, attr_t attrs);

extern int		scrn_slk_refresh	(SCREEN *s);
extern int		scrn_slk_noutrefresh(SCREEN *s);
extern int		scrn_slk_clear		(SCREEN *s);
extern int		scrn_slk_restore	(SCREEN *s);
extern int		scrn_slk_touch		(SCREEN *s);

/*------------------------------------------------------------------------
 * input options (ecurs_inopts)
 */
/* the following use the current screen */

extern int		nlinp			(void);
extern int		nonlinp			(void);

extern int		raw				(void);
extern int		noraw			(void);

extern int		cbreak			(void);
extern int		nocbreak		(void);

extern int		echo			(void);
extern int		noecho			(void);

extern int		metaon			(void);
extern int		metaoff			(void);

extern int		qiflush			(void);
extern int		noqiflush		(void);

extern int		typeahead		(int fildes);
extern int		halfdelay		(int tenths);

extern int		addkey			(const char *string, int code);

/* the following use the specified screen */

extern int		scrn_nlinp		(SCREEN *s);
extern int		scrn_nonlinp	(SCREEN *s);

extern int		scrn_raw		(SCREEN *s);
extern int		scrn_noraw		(SCREEN *s);

extern int		scrn_cbreak		(SCREEN *s);
extern int		scrn_nocbreak	(SCREEN *s);

extern int		scrn_echo		(SCREEN *s);
extern int		scrn_noecho		(SCREEN *s);

extern int		scrn_metaon		(SCREEN *s);
extern int		scrn_metaoff	(SCREEN *s);

extern int		scrn_qiflush	(SCREEN *s);
extern int		scrn_noqiflush	(SCREEN *s);

extern int		scrn_typeahead	(SCREEN *s, int fildes);
extern int		scrn_halfdelay	(SCREEN *s, int tenths);

extern int		scrn_addkey		(SCREEN *s, const char *string, int code);

#if NOMACROS
extern int		meta			(WINDOW *w, int bf);
extern int		intrflush		(WINDOW *w, int bf);

extern int		keypad			(WINDOW *w, int bf);
extern int		getkeypad		(WINDOW *w);

extern int		getdelay		(WINDOW *w);
extern int		setdelay		(WINDOW *w, int d);
extern int		nodelay			(WINDOW *w, int bf);
extern int		wtimeout		(WINDOW *w, int t);

extern int		notimeout		(WINDOW *w, int bf);
extern int		getnotimeout	(WINDOW *w);

extern int		timeout			(int t);
#else
#  define		meta(w,bf)		((bf) ? metaon()  : metaoff())
#  define		intrflush(w,bf)	((bf) ? qiflush() : noqiflush())

#  define		keypad(w,bf)	(((w)->_use_keypad = (bf) ? TRUE : FALSE), OK)
#  define		getkeypad(w)	((w)->_use_keypad)

#  define		getdelay(w)		((w)->_delay)
#  define		setdelay(w,d)	(((w)->_delay = (d)), OK)
#  define		nodelay(w,bf)	(((w)->_delay = (bf) ? 0 : -1), OK)
#  define		wtimeout(w,t)	setdelay(w,t)

#  define		notimeout(w,bf)	(((w)->_notimeout = (bf) ? TRUE : FALSE), OK)
#  define		getnotimeout(w)	((w)->_notimeout)

#  define		timeout(t)		wtimeout(stdscr,t)
#endif

/*------------------------------------------------------------------------
 * output options (ecurs_outopts)
 */
extern int		wsetscrreg		(WINDOW *win, int top, int bot);

/* the following use the current screen */

extern int		nl					(void);
extern int		nonl				(void);
extern int		nlout				(void);
extern int		nonlout				(void);
extern int		get_valid_acs		(int c);
extern int		set_valid_acs		(int bf);

/* the following use the specified screen */

extern int		scrn_nl				(SCREEN *s);
extern int		scrn_nonl			(SCREEN *s);
extern int		scrn_nlout			(SCREEN *s);
extern int		scrn_nonlout		(SCREEN *s);
extern int		scrn_get_valid_acs	(const SCREEN *s, int c);
extern int		scrn_set_valid_acs	(SCREEN *s, int bf);

#if NOMACROS
extern int		clearok			(WINDOW *w, int bf);
extern int		leaveok			(WINDOW *w, int bf);
extern int		scrollok		(WINDOW *w, int bf);
extern int		immedok			(WINDOW *w, int bf);
extern int		idcok			(WINDOW *w, int bf);
extern int		idlok			(WINDOW *w, int bf);

extern int		getclear		(WINDOW *w);
extern int		getleave		(WINDOW *w);
extern int		getscroll		(WINDOW *w);
extern int		getimmed		(WINDOW *w);
extern int		getidc			(WINDOW *w);
extern int		getidl			(WINDOW *w);

extern int		setscrreg		(int t, int b);
#else
#  define		clearok(w,bf)	(((w)->_clear   = (bf) ? TRUE : FALSE), OK)
#  define		leaveok(w,bf)	(((w)->_leaveit = (bf) ? TRUE : FALSE), OK)
#  define		scrollok(w,bf)	(((w)->_scroll  = (bf) ? TRUE : FALSE), OK)
#  define		immedok(w,bf)	(((w)->_immed   = (bf) ? TRUE : FALSE), OK)
#  define		idcok(w,bf)		(((w)->_use_idc = (bf) ? TRUE : FALSE), OK)
#  define		idlok(w,bf)		(((w)->_use_idl = (bf) ? TRUE : FALSE), OK)

#  define		getclear(w)		((w)->_clear)
#  define		getleave(w)		((w)->_leaveit)
#  define		getscroll(w)	((w)->_scroll)
#  define		getimmed(w)		((w)->_immed)
#  define		getidc(w)		((w)->_use_idc)
#  define		getidl(w)		((w)->_use_idl)

#  define		setscrreg(t,b)	wsetscrreg(stdscr,t,b)
#endif

/*------------------------------------------------------------------------
 * screen dump functions (ecurs_scrdump)
 */
/* the following use the current screen */

extern int		scr_dump			(const char *filename);
extern int		scr_init			(const char *filename);
extern int		scr_restore			(const char *filename);
extern int		scr_set				(const char *filename);

/* the following use the specified screen */

extern int		scrn_scr_dump		(SCREEN *s, const char *filename);
extern int		scrn_scr_init		(SCREEN *s, const char *filename);
extern int		scrn_scr_restore	(SCREEN *s, const char *filename);
extern int		scrn_scr_set		(SCREEN *s, const char *filename);

/*------------------------------------------------------------------------
 * terminal attribute functions (ecurs_termattrs)
 */
/* the following use the current screen */

extern int			baudrate			(void);
extern int			killchar			(void);
extern int			erasechar			(void);
extern int			quitchar			(void);
extern int			intrchar			(void);
extern int			has_ic				(void);
extern int			has_il				(void);
extern int			get_window_env		(void);
extern attr_t		termattrs			(void);
extern const char *	termname			(void);
extern const char *	longname			(void);
extern const char *	termtype			(void);
extern const char *	get_ttyname			(void);

extern int			get_curr_row		(void);
extern int			get_curr_col		(void);
extern int			get_max_cols		(void);
extern int			get_max_rows		(void);
extern int			get_tab_size		(void);
extern attr_t		get_orig_attr		(void);
extern attr_t		get_curr_attr		(void);

/* the following use the specified screen */

extern int			scrn_baudrate		(const SCREEN *s);
extern int			scrn_killchar		(const SCREEN *s);
extern int			scrn_erasechar		(const SCREEN *s);
extern int			scrn_quitchar		(const SCREEN *s);
extern int			scrn_intrchar		(const SCREEN *s);
extern int			scrn_has_ic			(const SCREEN *s);
extern int			scrn_has_il			(const SCREEN *s);
extern int			scrn_get_window_env	(const SCREEN *s);
extern attr_t		scrn_termattrs		(const SCREEN *s);
extern const char *	scrn_termname		(const SCREEN *s);
extern const char *	scrn_longname		(const SCREEN *s);
extern const char *	scrn_termtype		(const SCREEN *s);
extern const char *	scrn_get_ttyname	(const SCREEN *s);

extern int			scrn_get_curr_row	(const SCREEN *s);
extern int			scrn_get_curr_col	(const SCREEN *s);
extern int			scrn_get_max_cols	(const SCREEN *s);
extern int			scrn_get_max_rows	(const SCREEN *s);
extern int			scrn_get_tab_size	(const SCREEN *s);
extern attr_t		scrn_get_orig_attr	(const SCREEN *s);
extern attr_t		scrn_get_curr_attr	(const SCREEN *s);

/*------------------------------------------------------------------------
 * utility functions (ecurs_util)
 */
extern char *	unctrl				(chtype c);
extern char *	keyname				(int c);
extern void		use_env				(int bf);
extern int		putwin				(WINDOW *win, FILE *fp);

/* the following use the current screen */

extern WINDOW *	getwin				(FILE *fp);
extern int		flushinp			(void);
extern int		delay_output		(int ms);
extern int		filter				(void);

/* the following use the specified screen */

extern WINDOW *	scrn_getwin			(SCREEN *s, FILE *fp);
extern int		scrn_flushinp		(SCREEN *s);
extern int		scrn_delay_output	(SCREEN *s, int ms);
extern int		scrn_filter			(SCREEN *s);

/*------------------------------------------------------------------------
 * color functions (ecurs_color)
 */
/* the following use the current screen */

extern int		start_color				(void);
extern int		has_colors				(void);
extern int		init_pair				(short pair, short  f, short  b);
extern int		pair_content			(short pair, short *f, short *b);
extern int		init_all_pairs			(void);
extern int		color_content			(short color,
											short *r, short *g, short *b);
extern int		init_color				(short color,
											short  r, short  g, short  b);
extern int		can_change_color		(void);
extern int		PAIR_NUMBER				(attr_t a);
extern attr_t	COLOR_PAIR				(int n);

/* the following use the specified screen */

extern int		scrn_start_color		(SCREEN *s);
extern int		scrn_has_colors			(const SCREEN *s);
extern int		scrn_init_pair			(SCREEN *s, short pair,
											short  f, short  b);
extern int		scrn_pair_content		(const SCREEN *s, short pair,
											short *f, short *b);
extern int		scrn_init_all_pairs		(SCREEN *s);
extern int		scrn_color_content		(const SCREEN *s, short color,
											short *r, short *g, short *b);
extern int		scrn_init_color			(SCREEN *s, short color,
											short  r, short  g, short  b);
extern int		scrn_can_change_color	(const SCREEN *s);
extern int		scrn_pair_number		(const SCREEN *s, attr_t a);
extern attr_t	scrn_color_pair			(const SCREEN *s, int n);

/*------------------------------------------------------------------------
 * input formatted data (ecurs_scanw)
 */
extern int		vwscanw		(WINDOW *win, const char *fmt, va_list args);

extern int		wscanw		(WINDOW *win, const char *fmt, ...);
extern int		scanw		(             const char *fmt, ...);

extern int		mvwscanw	(WINDOW *win, int y, int x, const char *fmt, ...);
extern int		mvscanw		(             int y, int x, const char *fmt, ...);

/*------------------------------------------------------------------------
 * output formatted data (ecurs_printw)
 */
extern int		vwprintw	(WINDOW *win, const char *fmt, va_list args);

extern int		wprintw		(WINDOW *win, const char *fmt, ...);
extern int		printw		(             const char *fmt, ...);

extern int		mvwprintw	(WINDOW *win, int y, int x, const char *fmt, ...);
extern int		mvprintw	(             int y, int x, const char *fmt, ...);

/*------------------------------------------------------------------------
 * set & get window attributes (ecurs_attr)
 */
extern int		wattroff		(WINDOW *w, attr_t a);
extern int		wattron			(WINDOW *w, attr_t a);

#if NOMACROS
extern attr_t	wattrget		(WINDOW *w);
extern int		wattrset		(WINDOW *w, attr_t a);

extern attr_t	wstandget		(WINDOW *w);
extern int		wstandset		(WINDOW *w, attr_t a);

extern int		wstandout		(WINDOW *w);
extern int		wstandend		(WINDOW *w);

extern int		attroff			(attr_t a);
extern int		attron			(attr_t a);
extern attr_t	attrget			(void);
extern int		attrset			(attr_t a);
extern attr_t	standget		(void);
extern int		standset		(attr_t a);
extern int		standout		(void);
extern int		standend		(void);

extern attr_t	getattrs		(WINDOW *w);
extern chtype	getbkgd			(WINDOW *w);
extern int		getcode			(WINDOW *w);
extern int		getstmode		(WINDOW *w);

extern int		setbkgd			(WINDOW *w, attr_t a);
extern int		setcode			(WINDOW *w, int c);
extern int		setstmode		(WINDOW *w, int b);
extern int		setcolor		(WINDOW *w, int f, int b);
#else
#  define		wattrget(w)		((w)->_attrs)
#  define		wattrset(w,a)	(((w)->_attrs  =  (A_GETATTR(a))), OK)

#  define		wstandget(w)	((w)->_attro)
#  define		wstandset(w,a)	(((w)->_attro  =  (A_GETATTR(a))), OK)

#  define		wstandout(w)	(((w)->_stmode = TRUE),  OK)
#  define		wstandend(w)	(((w)->_stmode = FALSE), OK)

#  define		attroff(a)		wattroff(stdscr,a)
#  define		attron(a)		wattron(stdscr,a)
#  define		attrget()		wattrget(stdscr)
#  define		attrset(a)		wattrset(stdscr,a)
#  define		standget()		wstandget(stdscr)
#  define		standset(a)		wstandset(stdscr,a)
#  define		standout()		wstandout(stdscr)
#  define		standend()		wstandend(stdscr)

#  define		getattrs(w)		((w)->_stmode ? (w)->_attro : (w)->_attrs)
#  define		getbkgd(w)		((w)->_bkgd)
#  define		getcode(w)		A_GETCODE((w)->_code)
#  define		getstmode(w)	((w)->_stmode)

#  define		setbkgd(w,a)	(((w)->_bkgd   = (a)), OK)
#  define		setcode(w,c)	(((w)->_code   = A_SETCODE(c)), OK)
#  define		setstmode(w,b)	(((w)->_stmode = (b)), OK)
#  define		setcolor(w,f,b) (((w)->_attrs  = (A_CLR((f),(b)))), OK)
#endif

/*------------------------------------------------------------------------
 * get/set coordinates in the window structure (ecurs_getyx)
 */
#if NOMACROS
extern int		getcury			(WINDOW *w);
extern int		getcurx			(WINDOW *w);
extern int		getbegy			(WINDOW *w);
extern int		getbegx			(WINDOW *w);
extern int		getmaxy			(WINDOW *w);
extern int		getmaxx			(WINDOW *w);
extern int		getpary			(WINDOW *w);
extern int		getparx			(WINDOW *w);

extern int		setcury			(WINDOW *w, int y);
extern int		setcurx			(WINDOW *w, int x);
extern int		setbegy			(WINDOW *w, int y);
extern int		setbegx			(WINDOW *w, int x);
extern int		setmaxy			(WINDOW *w, int y);
extern int		setmaxx			(WINDOW *w, int x);
extern int		setpary			(WINDOW *w, int y);
extern int		setparx			(WINDOW *w, int x);
#else
#  define		getcury(w)		((w)->_cury)
#  define		getcurx(w)		((w)->_curx)
#  define		getbegy(w)		((w)->_begy)
#  define		getbegx(w)		((w)->_begx)
#  define		getmaxy(w)		((w)->_maxy)
#  define		getmaxx(w)		((w)->_maxx)
#  define		getpary(w)		((w)->_pary)
#  define		getparx(w)		((w)->_parx)

#  define		setcury(w,y)	((getcury(w) = (y)), OK)
#  define		setcurx(w,x)	((getcurx(w) = (x)), OK)
#  define		setbegy(w,y)	((getbegy(w) = (y)), OK)
#  define		setbegx(w,x)	((getbegx(w) = (x)), OK)
#  define		setmaxy(w,y)	((getmaxy(w) = (y)), OK)
#  define		setmaxx(w,x)	((getmaxx(w) = (x)), OK)
#  define		setpary(w,y)	((getpary(w) = (y)), OK)
#  define		setparx(w,x)	((getparx(w) = (x)), OK)
#endif

#define			getyx(w,y,x)	(((y) = getcury(w), (x) = getcurx(w)), OK)
#define			getcuryx(w,y,x)	(((y) = getcury(w), (x) = getcurx(w)), OK)
#define			getbegyx(w,y,x)	(((y) = getbegy(w), (x) = getbegx(w)), OK)
#define			getmaxyx(w,y,x)	(((y) = getmaxy(w), (x) = getmaxx(w)), OK)
#define			getparyx(w,y,x)	(((y) = getpary(w), (x) = getparx(w)), OK)

#if NOMACROS
extern int		setyx			(WINDOW *w, int y, int x);
extern int		setcuryx		(WINDOW *w, int y, int x);
extern int		setbegyx		(WINDOW *w, int y, int x);
extern int		setmaxyx		(WINDOW *w, int y, int x);
extern int		setparyx		(WINDOW *w, int y, int x);
#else
#  define		setyx(w,y,x)	((getcury(w) = (y), getcurx(w) = (x)), OK)
#  define		setcuryx(w,y,x)	((getcury(w) = (y), getcurx(w) = (x)), OK)
#  define		setbegyx(w,y,x)	((getbegy(w) = (y), getbegx(w) = (x)), OK)
#  define		setmaxyx(w,y,x)	((getmaxy(w) = (y), getmaxx(w) = (x)), OK)
#  define		setparyx(w,y,x)	((getpary(w) = (y), getparx(w) = (x)), OK)
#endif

/*------------------------------------------------------------------------
 * alarm functions (ecurs_alarm)
 */
typedef void WIN_ALARM_RTN (time_t t, void *data);

#define WIN_ALARM_KEY  		0		/* used by key input rtn				*/
#define WIN_ALARM_CLOCK		1		/* used by clock rtn					*/
#define WIN_ALARM_PING		2		/* used to ping something (heartbeat)	*/
#define WIN_ALARM_USER		3		/* available for users					*/

#define WIN_NUM_ALARMS		4		/* number of alarms defined				*/

extern int		win_alarm_set		(int atype,
										int interval,
										WIN_ALARM_RTN *funcptr,
										void *data,
										int on_heap,
										int returns,
										int start);
extern int		win_alarm_clr		(int atype);

extern int		win_alarm_on		(int atype);
extern int		win_alarm_off		(int atype);

extern int		win_alarm_check		(int atype);
extern int		win_alarm_trip		(int atype, time_t t);
extern int		win_alarm_active	(int atype);

extern int		win_alarm_stop		(void);
extern int		win_alarm_start		(int value);

/*------------------------------------------------------------------------
 * clock functions (ecurs_clock)
 *
 * Use these routines with care, since they can generate asynchronous
 * interrupts which can affect the user's use of system calls (which
 * can fail with an error code of *interrupted*).
 */
extern int		win_clock			(WINDOW *win, WIN_CLOCK_RTN *rtn,
										void *data, int interval);
extern void		win_clock_set		(int flag);
extern void		win_clock_check		(void);
extern int		win_clock_active	(int bf);

/*------------------------------------------------------------------------
 * printer functions (ecurs_printer)
 */
#ifndef PRINTER_T
#  define PRINTER_T			TRUE		/* indicate PRINTER defined		*/
  typedef struct printer	PRINTER;	/* opaque PRINTER pointer		*/
#  define P_SPOOL			0			/* mode: pipe to print spooler	*/
#  define P_PIPE			1			/* mode: pipe to cmd			*/
#  define P_FILE			2			/* mode: print to file			*/
#  define P_TERM			3			/* mode: print to terminal		*/
#endif

extern int		print_win			(WINDOW *win, PRINTER *p);

/* the following use the current screen */

extern PRINTER * prt_open			(int mode, const char *str, char *msgbuf);
extern int		prt_close			(PRINTER *p, char *msgbuf);
extern int		prt_output_str		(PRINTER *p, const char *str);
extern int		prt_output_fmt		(PRINTER *p, const char *fmt, ...);
extern int		prt_output_var		(PRINTER *p, const char *fmt, va_list args);

/* the following use the specified screen */

extern PRINTER * scrn_prt_open		(SCREEN *s, int mode,
										const char *str, char *msgbuf);
extern int		scrn_prt_close		(SCREEN *s, PRINTER *p,
										char *msgbuf);
extern int		scrn_prt_output_str	(SCREEN *s, PRINTER *p,
										const char *str);
extern int		scrn_prt_output_fmt	(SCREEN *s, PRINTER *p,
										const char *fmt, ...);
extern int		scrn_prt_output_var	(SCREEN *s, PRINTER *p,
										const char *fmt, va_list args);

/*------------------------------------------------------------------------
 * cmd-exec functions (ecurs_exec)
 */
/* the following use the current screen */

extern int		exec_argv		(const char *dir, char **argv,
									int interactive, char *msgbuf);

extern int		exec_cmd		(const char *dir, const char *cmd,
									int interactive, char *msgbuf);

/* the following use the specified screen */

extern int		scrn_exec_argv	(SCREEN *s, const char *dir, char **argv,
									int interactive, char *msgbuf);

extern int		scrn_exec_cmd	(SCREEN *s, const char *dir, const char *cmd,
									int interactive, char *msgbuf);

/*------------------------------------------------------------------------
 * scrollbar functions (ecurs_scrollbar)
 */
#define SCRLBAR_VL		1			/* type: vertical   left	*/
#define SCRLBAR_VR		2			/* type: vertical   right	*/
#define SCRLBAR_HT		3			/* type: horizontal top		*/
#define SCRLBAR_HB		4			/* type: horizontal bottom	*/

extern int		scrollbar		(WINDOW *brdr_win, WINDOW *win, int type,
									int cur, int total);

/*------------------------------------------------------------------------
 * mouse functions (ecurs_mouse)
 */
/* the following refer to a window */

extern int		mouse_setup				(WINDOW *win,
											attr_t ptr_attrs, attr_t cmd_attrs);
extern int		mouse_init				(WINDOW *win);
extern int		mouse_show				(WINDOW *win);
extern int		mouse_hide				(WINDOW *win);
extern int		mouse_get_display		(WINDOW *win);
extern int		mouse_get_event			(WINDOW *win);
extern int		mouse_x					(WINDOW *win);
extern int		mouse_y					(WINDOW *win);

extern int		mouse_c					(WINDOW *win);
extern int		mouse_is_in_win			(WINDOW *win);

/* the following refer to a screen */

extern int		scrn_mouse_setup		(SCREEN *s,
											attr_t ptr_attrs, attr_t cmd_attrs);
extern int		scrn_mouse_init			(SCREEN *s);
extern int		scrn_mouse_show			(SCREEN *s);
extern int		scrn_mouse_hide			(SCREEN *s);
extern int		scrn_mouse_get_display	(const SCREEN *s);
extern int		scrn_mouse_get_event	(const SCREEN *s);
extern int		scrn_mouse_x			(const SCREEN *s);
extern int		scrn_mouse_y			(const SCREEN *s);

/*------------------------------------------------------------------------
 * mono/color attribute functions (ecurs_attrvals)
 */
extern int			get_key_value			(const char *name);

extern const char *	get_key_name			(int code, char *buf);
extern const char *	get_char_name			(int code, char *buf);

extern const char *	get_attr_name_by_code	(attr_t code, char *buf);
extern const char *	get_color_name_by_code	(attr_t code, char *buf);
extern const char * get_fg_name_by_code		(attr_t code);
extern const char * get_bg_name_by_code		(attr_t code);

extern const char *	get_attr_name_by_num	(int n);
extern const char *	get_fg_name_by_num		(int n);
extern const char *	get_bg_name_by_num		(int n);

extern attr_t		get_attr_value_by_num	(int n);
extern attr_t		get_fg_value_by_num		(int n);
extern attr_t		get_bg_value_by_num		(int n);

extern int			get_attr_num_by_name	(const char *name);
extern int			get_fg_num_by_name		(const char *name);
extern int			get_bg_num_by_name		(const char *name);

extern int			get_attr_num_by_code	(attr_t code);
extern int			get_fg_num_by_code		(attr_t code);
extern int			get_bg_num_by_code		(attr_t code);

/* the following use the current screen */

extern int			get_default_attrs		(attr_t *pr, attr_t *ps,
												int use_color, int use_win);

/* the following use the specified screen */

extern int			scrn_get_default_attrs	(SCREEN *s, attr_t *pr, attr_t *ps,
												int use_color, int use_win);

/*------------------------------------------------------------------------
 * integer I/O functions (ecurs_intio)
 *
 * These routines provide a byte-order independent mechanism for reading
 * and writing binary files.
 */
extern unsigned short	get_2byte	(FILE *fp);
extern int				put_2byte	(FILE *fp, unsigned short s);

extern unsigned int		get_4byte	(FILE *fp);
extern int				put_4byte	(FILE *fp, unsigned int l);

extern time_t			get_4time	(FILE *fp);
extern int				put_4time	(FILE *fp, time_t t);

/*------------------------------------------------------------------------
 * signal routines (ecurs_signal)
 */
typedef void WIN_SIG_RTN	(int sig, void *data);

extern void				set_signal_rtn		(int sig,
												WIN_SIG_RTN *rtn, void *data);
extern WIN_SIG_RTN *	get_signal_rtn		(int sig);
extern void *			get_signal_data		(int sig);

/*------------------------------------------------------------------------
 * miscellaneous functions (ecurs_misc)
 */
#define WIN_SNAP_FILE		"screen.snp"	/* default snapshot filename */

typedef int		WIN_EVENT_RTN	(void *data, int ms);

typedef int		WIN_INPUT_RTN	(void *data, int mode, int ms);

#define WIN_INP_MODE_OPEN	1				/* input  rtn open  call	*/
#define WIN_INP_MODE_CLOSE	2				/* input  rtn close call	*/
#define WIN_INP_MODE_READ	3				/* input  rtn read  call	*/

typedef int		WIN_OUTPUT_RTN	(void *data, int mode,
									const unsigned char *buf, int n);

#define WIN_OUT_MODE_OPEN	1				/* output rtn open  call	*/
#define WIN_OUT_MODE_CLOSE	2				/* output rtn close call	*/
#define WIN_OUT_MODE_WRITE	3				/* output rtn write call	*/

extern int		win_snap		(WINDOW *win, const char *path);

/* the following use the current screen */

extern int		resize_screen		(void);
extern int		blank_screen		(const char **logo);
extern int		set_title			(const char *str);
extern int		set_async_key		(int key,
										WIN_ASYNC_KEY_RTN *rtn, void *data);
extern int		set_icon			(int width, int height,
										const unsigned char *bits);
extern int		set_debug_scr		(const char *path,
										int bf, int text);
extern int		set_debug_kbd		(const char *path,
										int bf, int text);

extern int		set_event_rtn		(WIN_EVENT_RTN  *rtn, void *data);
extern int		set_input_rtn		(WIN_INPUT_RTN  *rtn, void *data);
extern int		set_output_rtn		(WIN_OUTPUT_RTN *rtn, void *data);

/* the following use the specified screen */

extern int		scrn_resize_screen	(SCREEN *s);
extern int		scrn_blank_screen	(SCREEN *s, const char **logo);
extern int		scrn_set_title		(SCREEN *s, const char *str);
extern int		scrn_set_async_key	(SCREEN *s, int key,
										WIN_ASYNC_KEY_RTN *rtn, void *data);
extern int		scrn_set_icon		(SCREEN *s, int width, int height,
										const unsigned char *bits);
extern int		scrn_set_debug_scr	(SCREEN *s, const char *path,
										int bf, int text);
extern int		scrn_set_debug_kbd	(SCREEN *s, const char *path,
										int bf, int text);

extern int		scrn_set_event_rtn	(SCREEN *s,
										WIN_EVENT_RTN  *rtn, void *data);

extern int		scrn_set_input_rtn	(SCREEN *s,
										WIN_INPUT_RTN  *rtn, void *data);

extern int		scrn_set_output_rtn	(SCREEN *s,
										WIN_OUTPUT_RTN *rtn, void *data);

/*------------------------------------------------------------------------
 * old curses compatability entries
 */
#define garbagedlines	wredrawln
#define garbagedwin		redrawwin

#define	crmode			cbreak
#define nocrmode		nocbreak

#define saveterm		def_prog_mode
#define fixterm			reset_prog_mode
#define resetterm		reset_shell_mode

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* CURSES_H */
