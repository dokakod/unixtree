/*------------------------------------------------------------------------
 * libxvt header
 *
 *	The libxvt library provides a terminal-emulator running in
 *	an X11 window, with a pseudo-tty providing input & output to
 *	that window.
 *
 *	This terminal-emulator is designed to emulate the xterm terminal-
 *	emulator, and uses the same escape sequences.  It is, however,
 *	a "slimmed" down version, and doesn't provide all the functionality
 *	of xterm, such as scroll bars, multi-byte support, background
 *	pixmaps, etc.  The big advantage of this library is that the
 *	terminal-emulator is implemented as a callable procedure.
 *
 *	This window is intended to provide an IBM-PC type of display for
 *	a character application, with support for full-color and mouse.
 *
 *------------------------------------------------------------------------
 *
 *	This library can be used on one of two ways:
 *
 *	1.	Run in "task" mode.
 *
 *		In "task" mode, the library runs a specified cmd as a
 *		sub-task and returns (with all display structs destroyed)
 *		when the sub-task dies.
 *
 *	2.	Run in "here" mode.
 *
 *		In "here" mode, the calling procedure keeps control, and is
 *		given an input & output file descriptor for doing input &
 *		output to the window.  In this mode, all initial setup is done,
 *		then either a sub-thread is run to process all events, or the caller
 *		is given a pointer to an event-routine which must be called frequently
 *		to "activate" the event-routine.
 *
 *		When in "here" mode, the calling program can request to be detached
 *		("detached-here" mode), which results in the calling program being
 *		converted to a pseudo-daemon, with all standard streams
 *		(stdin/stdout/stderr) pointing to the display window.
 *
 *------------------------------------------------------------------------
 *
 *	Notes:
 *
 *	1.	In either the case of running in "task" mode or in "detached-here"
 *		mode ("here" mode with "detach" set TRUE), the streams
 *		stdin/stdout/stderr are set to point to the pseudo-tty created.
 *
 *		This means that, if running in "detached-here" mode, your program has
 *		NO access to the original stdin/stdout/stderr your program was
 *		started with, unless you copied them somewhere.
 *
 *		If you are running in "task" mode, only the child's streams are
 *		changed.
 *
 *	2.	When running in "detached-here" mode, the calling program is completely
 *		detached from the original controlling terminal.  Thus, there is
 *		no "/dev/tty" available to output to, unless you copied the original
 *		/dev/tty prior to the call.
 *
 *		In this mode, the calling program acts like a pseudo-daemon, in that
 *		it has its own process group and has no parent process.  It is not a
 *		true daemon, however, since it does have a controlling terminal (the
 *		display window pty).
 *
 *	3.	When using threads, do NOT attempt to read from or write to the
 *		pseudo-tty (i.e. to/from stdin/stdout/stderr) when in a signal
 *		routine.  Some Unixes (e.g. Linux) cause all threads to stop when
 *		an interrupt occurs, and thus your program will "hang" with no way
 *		to stop it except via the "kill" command.
 *
 *		It is a bad idea to do any I/O in a signal routine anyway, since
 *		signal routines are called asynchronously and I/O may cause them
 *		to block.
 *
 *	4.	If any error occurs, the following values are returned in the
 *		XVT_DATA struct (except in the case of a NULL XVT_DATA pointer):
 *
 *			error_num		xvt error code
 *			error_msg		displayable error message
 *			error_quit		TRUE if it is not possible to continue
 *							(This happens in "here" mode if the std streams
 *							have been redirected and then an error occurs.)
 *
 *	5.	Users of the "ecurses" library can automate the calling of the
 *		event-routine when running in "here" mode with the call:
 *
 *			set_event_rtn(
 *				xvt_data_get_event_rtn(xd),
 *				xvt_data_get_event_data(xd) );
 *
 *		or
 *
 *			scrn_set_event_rtn(screen,
 *				xvt_data_get_event_rtn(xd),
 *				xvt_data_get_event_data(xd) );
 *
 *		This will cause the event-routine to be automatically called
 *		whenever a "keyboard" query is done or anything is written to
 *		the "screen", and while the caller is "shelled-out" to a sub-task.
 *
 *		Note that this call will result in nothing being done if threads
 *		are used.
 *
 *	6.	Although there are defines and variables defined for scrollbar
 *		processing, scrollbar processing is not yet implemented.
 *
 *	7.	Unlike xterm, this library does not do any utmp processing
 *		or termcap processing.
 *
 *	8.	This library is thread-safe.
 *
 *------------------------------------------------------------------------
 *
 *	Summary of libxvt calls:
 *
 *	---- misc calls			----------------------------------------------
 *
 *	xvt_available()			Check if xvt functionality is available
 *	xvt_main()				Internal main for terminal emulator
 *
 *	---- task calls			----------------------------------------------
 *
 *	xvt_task_run()			Run a sub-task within the emulator
 *
 *	---- here calls			----------------------------------------------
 *
 *	xvt_here_run()			Setup to run the emulator under program control
 *	xvt_here_task()			Run a sub-task but keep control of process
 *	xvt_here_close()		Close out here processing
 *	xvt_here_check()		Give the emulator a chance to check all events
 *
 *	xvt_here_clone()		Clone the calling program with a new display window
 *
 *	xvt_here_input()		Present buffer as input  as if from kbd
 *	xvt_here_output()		Present buffer as output to be displayed
 *
 *	---- toolkit calls		----------------------------------------------
 *
 *	xvt_tk_args()			Extract all toolkit options from the cmd-line
 *	xvt_tk_optlist()		Process a list of option values
 *	xvt_tk_resource()		Process all standard resource files
 *	xvt_tk_help()			Display toolkit usage messages
 *
 *	xvt_tk_resfile_load()	Load the contents of a resource file
 *	xvt_tk_resfile_save()	Save all/set options to a resource file
 *	xvt_tk_options_dump()	Dump all/set options to a stream
 *
 *	---- dump calls			----------------------------------------------
 *
 *	xvt_dump_colors()		Dump to a stream all color   names
 *	xvt_dump_pointers()		Dump to a stream all pointer names
 *	xvt_dump_resources()	Dump to a stream all resources to be queried
 *
 *	---- data calls			----------------------------------------------
 *
 *	xvt_data_alloc()		Allocate   an XVT_DATA struct
 *	xvt_data_free()			Free       an XVT_DATA struct
 *	xvt_data_init()			Initialize an XVT_DATA struct
 *	xvt_data_copy()			Copy       an XVT_DATA struct
 *
 *	xvt_data_set_*()		Set various data members
 *	xvt_data_get_*()		Get various data members
 *
 *------------------------------------------------------------------------
 */
#ifndef LIBXVT_H
#define LIBXVT_H

#include <stdio.h>

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * Miscellaneous generic definitions
 */
#ifndef TRUE
#  define TRUE				1
#endif

#ifndef FALSE
#  define FALSE				0
#endif

#ifndef EXIT_SUCCESS
#  define EXIT_SUCCESS		0
#endif

#ifndef EXIT_FAILURE
#  define EXIT_FAILURE		1
#endif

#define XVT_MAX_PATHLEN		256				/* max path length				*/

/*------------------------------------------------------------------------
 * Library version
 */
#define XVT_VERSION			"1.0"			/* current version				*/

/*------------------------------------------------------------------------
 * Library class names
 */
#define XVT_NAME			"pcxterm"		/* default name					*/
#define XVT_APPCLASS		"Pcxterm"		/* app class name for resources	*/

/*------------------------------------------------------------------------
 * Defines for system resources
 */
#define XVT_POINTER			0				/* default pointer type			*/
											/* NULL -> use existing ptr		*/
											/* (xterm uses "xterm")			*/

/*------------------------------------------------------------------------
 * Extended-capabilities term names
 *
 * Normally these names are used in conjunction with the ecurses package,
 * since they don't exist in either the termcap or terminfo databases,
 * and they define additional capabilities only used by the ecurses package.
 */
#define XVT_TERM_MONO		XVT_NAME "-m"	/* mono  term type to use		*/
#define XVT_TERM_COLOR		XVT_NAME "-c"	/* color term type to use		*/

/*------------------------------------------------------------------------
 * Miscellaneous defaults
 */
#define XVT_MIN_COLS		80				/* min # cols for window		*/
#define XVT_MIN_ROWS		24				/* min # rows for window		*/

#define XVT_NUM_FONTS		8				/* number of fonts supported	*/

#define XVT_SHELL			"sh"			/* default cmd if no $SHELL		*/

#define XVT_MAX_BITS		64				/* max width & height of icon	*/

/*------------------------------------------------------------------------
 * Scrollbar info (not-yet-implemented)
 */
#define XVT_SB_WIDTH		11				/* scrollbar width				*/
#define XVT_SB_SHADOW		2				/* scrollbar shadow width		*/
#define XVT_SB_MINHEIGHT	2				/* min scrollbar height			*/

/*------------------------------------------------------------------------
 * Color definitions
 */
#define XVT_CLR_BLACK		0
#define XVT_CLR_RED			1
#define XVT_CLR_GREEN		2
#define XVT_CLR_YELLOW		3
#define XVT_CLR_BLUE		4
#define XVT_CLR_MAGENTA		5
#define XVT_CLR_CYAN		6
#define XVT_CLR_WHITE		7

#define XVT_CLR_BRIGHT		8

#define XVT_CLR_GREY		(XVT_CLR_BLACK   | XVT_CLR_BRIGHT)
#define XVT_CLR_LTRED		(XVT_CLR_RED     | XVT_CLR_BRIGHT)
#define XVT_CLR_LTGREEN		(XVT_CLR_GREEN   | XVT_CLR_BRIGHT)
#define XVT_CLR_LTYELLOW	(XVT_CLR_YELLOW  | XVT_CLR_BRIGHT)
#define XVT_CLR_LTBLUE		(XVT_CLR_BLUE    | XVT_CLR_BRIGHT)
#define XVT_CLR_LTMAGENTA	(XVT_CLR_MAGENTA | XVT_CLR_BRIGHT)
#define XVT_CLR_LTCYAN		(XVT_CLR_CYAN    | XVT_CLR_BRIGHT)
#define XVT_CLR_LTWHITE		(XVT_CLR_WHITE   | XVT_CLR_BRIGHT)

#define XVT_CLR_NUM			16				/* number of colors				*/

/*------------------------------------------------------------------------
 * String option defaults
 */
#define XVT_S_OPT_DISPLAY	"$DISPLAY"		/* X server to contact			*/
#define XVT_S_OPT_TERMNAME	0				/* value for TERM variable		*/
#define XVT_S_OPT_RESNAME	XVT_APPCLASS	/* alt resource name			*/
#define XVT_S_OPT_GEOMETRY	0				/* window geometry				*/
#define XVT_S_OPT_ICONGEOM	0				/* icon geometry				*/
#define XVT_S_OPT_TERMMODE	0				/* stty modes for window		*/
#define XVT_S_OPT_ICONFILE	0				/* icon file to load			*/
#define XVT_S_OPT_POINTER	XVT_POINTER		/* cursor type					*/
#define XVT_S_OPT_ENVVAR	0				/* env var name for termtype	*/
#define XVT_S_OPT_TITLE		"<cmd>"			/* window title					*/
#define XVT_S_OPT_ITITLE	"<cmd>"			/* icon title					*/
#define XVT_S_OPT_PRINTER	"<spooler>"		/* printer specification		*/
#define XVT_S_OPT_RESSTR	0				/* resource string				*/
#define XVT_S_OPT_RESFILE	0				/* alternate resource file		*/
#define XVT_S_OPT_PTYSPEC	0				/* pty spec						*/
#define XVT_S_OPT_TERMINAL	"xterm"			/* default terminal type		*/

/*------------------------------------------------------------------------
 * Font option defaults
 */
#define XVT_F_OPT_REGFONT0	"7x14"			/* default norm font 0			*/
#define XVT_F_OPT_BLDFONT0	"7x14bold"		/* default bold font 0			*/

#define XVT_F_OPT_REGFONT1	"6x13"			/* default norm font 1			*/
#define XVT_F_OPT_BLDFONT1	"6x13bold"		/* default bold font 1			*/

#define XVT_F_OPT_REGFONT2	"7x13"			/* default norm font 2			*/
#define XVT_F_OPT_BLDFONT2	"7x13bold"		/* default bold font 2			*/

#define XVT_F_OPT_REGFONT3	"8x13"			/* default norm font 3			*/
#define XVT_F_OPT_BLDFONT3	"8x13bold"		/* default bold font 3			*/

#define XVT_F_OPT_REGFONT4	"9x15"			/* default norm font 4			*/
#define XVT_F_OPT_BLDFONT4	"9x15bold"		/* default bold font 4			*/

#define XVT_F_OPT_REGFONT5	0				/* default norm font 5			*/
#define XVT_F_OPT_BLDFONT5	0				/* default bold font 5			*/

#define XVT_F_OPT_REGFONT6	0				/* default norm font 6			*/
#define XVT_F_OPT_BLDFONT6	0				/* default bold font 6			*/

#define XVT_F_OPT_REGFONT7	0				/* default norm font 7			*/
#define XVT_F_OPT_BLDFONT7	0				/* default bold font 7			*/

/*------------------------------------------------------------------------
 * Color option defaults
 */
#define XVT_C_OPT_BLACK		"Black"			/* default black      color		*/
#define XVT_C_OPT_RED		"Red3"			/* default red        color		*/
#define XVT_C_OPT_GREEN		"Green3"		/* default green      color		*/
#define XVT_C_OPT_YELLOW	"Yellow3"		/* default yellow     color		*/
#define XVT_C_OPT_BLUE		"Blue3"			/* default blue       color		*/
#define XVT_C_OPT_MAGENTA	"Magenta3"		/* default magenta    color		*/
#define XVT_C_OPT_CYAN		"Cyan3"			/* default cyan       color		*/
#define XVT_C_OPT_WHITE		"AntiqueWhite"	/* default white      color		*/

#define XVT_C_OPT_GREY		"Grey25"		/* default grey       color		*/
#define XVT_C_OPT_LTRED		"Red"			/* default lt-red     color		*/
#define XVT_C_OPT_LTGREEN	"Green"			/* default lt-green   color		*/
#define XVT_C_OPT_LTYELLOW	"Yellow"		/* default lt-yellow  color		*/
#define XVT_C_OPT_LTBLUE	"Blue"			/* default lt-blue    color		*/
#define XVT_C_OPT_LTMAGENTA	"Magenta"		/* default lt-magenta color		*/
#define XVT_C_OPT_LTCYAN	"Cyan"			/* default lt-cyan    color		*/
#define XVT_C_OPT_LTWHITE	"White"			/* default lt-white   color		*/

#define XVT_C_OPT_FG		"Black"			/* default foreground color		*/
#define XVT_C_OPT_BG		"White"			/* default background color		*/

#define XVT_C_OPT_POINTER	"Black"			/* default pointer    color		*/
#define XVT_C_OPT_BORDER	"Black"			/* default border     color		*/
#define XVT_C_OPT_CURSOR	"None"			/* default cursor     color		*/

#define XVT_C_OPT_SB		"Black"			/* default scrollbar  color		*/
#define XVT_C_OPT_ST		"White"			/* default SB trough  color		*/

/*------------------------------------------------------------------------
 * Numeric option defaults
 */
#define XVT_N_OPT_COLS		80				/* starting number of columns	*/
#define XVT_N_OPT_ROWS		25				/* starting number of rows		*/
#define XVT_N_OPT_TABWIDTH	8				/* # spaces per tab stop		*/
#define XVT_N_OPT_BORDER	1				/* default border width			*/
#define XVT_N_OPT_MARGIN	0				/* default margin width			*/
#define XVT_N_OPT_MBCOLS	10				/* margin bell cols from right	*/
#define XVT_N_OPT_FONTNO	0				/* default initial font number	*/

#define XVT_N_OPT_SBLINES	300				/* number of lines to save		*/

/*------------------------------------------------------------------------
 * Boolean option defaults
 */
#define XVT_B_OPT_ICONIC	FALSE			/* start iconic					*/
#define XVT_B_OPT_MONO		FALSE			/* force display to mono		*/
#define XVT_B_OPT_LOGIN		FALSE			/* login shell					*/
#define XVT_B_OPT_HILIGHT	FALSE			/* always highlight cursor		*/
#define XVT_B_OPT_REVVID	FALSE			/* reverse video				*/
#define XVT_B_OPT_VISBELL	FALSE			/* visual bell					*/
#define XVT_B_OPT_AUTOWRAP	TRUE			/* auto wraparound				*/
#define XVT_B_OPT_REVWRAP	FALSE			/* reverse wraparound			*/
#define XVT_B_OPT_MBELL		FALSE			/* margin bell					*/
#define XVT_B_OPT_QUIET		FALSE			/* inhibit bell request (^G)	*/
#define XVT_B_OPT_IGNPTR	FALSE			/* ignore pointer events		*/
#define XVT_B_OPT_CONSOLE	FALSE			/* intercept console msgs		*/
#define XVT_B_OPT_LBLINFO	FALSE			/* debug info in window label	*/
#define XVT_B_OPT_KPFONT	TRUE			/* KP+/KP- changes fonts		*/
#define XVT_B_OPT_MAPOUTPUT	FALSE			/* map if output when iconic	*/
#define XVT_B_OPT_MAPALERT	FALSE			/* map if ^G output when iconic	*/
#define XVT_B_OPT_A132		FALSE			/* allow 80-132 col switching	*/
#define XVT_B_OPT_C132		FALSE			/* switch to 132 column mode	*/
#define XVT_B_OPT_XERRORS	FALSE			/* show X error messages		*/
#define XVT_B_OPT_PRTKEEP	FALSE			/* keep printer open until term	*/
#define XVT_B_OPT_CUFIX		FALSE			/* enable curses (more) fix		*/
#define XVT_B_OPT_ACTICON	FALSE			/* enable active icon			*/

#define XVT_B_OPT_SBENABLE	FALSE			/* enable scrollbar				*/
#define XVT_B_OPT_SBRIGHT	TRUE			/* scrollbar on right			*/
#define XVT_B_OPT_SBSHADOW	TRUE			/* shadow scrollbar				*/
#define XVT_B_OPT_SBSCROUT	TRUE			/* scroll on tty output			*/
#define XVT_B_OPT_SBSCRKEY	TRUE			/* scroll on keypress			*/

/*------------------------------------------------------------------------
 * Debug option types & log filenames
 */
#define XVT_D_OPT_EVENTS	"events"		/* txt: xlated all    events	*/
#define XVT_D_OPT_ACTION	"action"		/* txt: xlated window events	*/
#define XVT_D_OPT_OUTPUT	"output"		/* txt: xlated child output		*/
#define XVT_D_OPT_SCREEN	"screen"		/* bin: raw    child output		*/
#define XVT_D_OPT_INPKBD	"inpkbd"		/* txt: xlated child input		*/
#define XVT_D_OPT_KEYBRD	"keybrd"		/* bin: raw    child input		*/
#define XVT_D_OPT_RESFIL	"resfil"		/* txt: resource file entries	*/
#define XVT_D_OPT_TKOPTS	"tkopts"		/* txt: dump of all options		*/

#define XVT_D_OPT_EXT		"log"			/* extension for all log files	*/

#define XVT_D_OPT_EVENTS_LOG	XVT_D_OPT_EVENTS "." XVT_D_OPT_EXT
#define XVT_D_OPT_ACTION_LOG	XVT_D_OPT_ACTION "." XVT_D_OPT_EXT
#define XVT_D_OPT_OUTPUT_LOG	XVT_D_OPT_OUTPUT "." XVT_D_OPT_EXT
#define XVT_D_OPT_SCREEN_LOG	XVT_D_OPT_SCREEN "." XVT_D_OPT_EXT
#define XVT_D_OPT_INPKBD_LOG	XVT_D_OPT_INPKBD "." XVT_D_OPT_EXT
#define XVT_D_OPT_KEYBRD_LOG	XVT_D_OPT_KEYBRD "." XVT_D_OPT_EXT
#define XVT_D_OPT_RESFIL_LOG	XVT_D_OPT_RESFIL "." XVT_D_OPT_EXT
#define XVT_D_OPT_TKOPTS_LOG	XVT_D_OPT_TKOPTS "." XVT_D_OPT_EXT

/*------------------------------------------------------------------------
 * Default resource file locations
 *
 * Note that environment variable names are expanded at run-time,
 * with $HOSTNAME being automatically resolved to the hostname of
 * the machine (with any domain suffix removed).
 *
 * If an entry contains a reference to an environment variable which
 * is not defined or is empty, that entry is skipped.
 *
 * For directories (and path specifications, which specify a list of
 * directories), that directory (if it exists) will be searched for the
 * file "<appname>" (which is assumed to be in Xdefaults format) and the
 * file "<appname>.ini" (which is assumed to be in Windows "ini" format).
 *
 * If <appname> is not specified in the XVT_DATA struct, then the default
 * appname (XVT_NAME) will be used.
 *
 * Note that these lists are NULL-terminated.
 */
#define XVT_RESOURCE_FILES	\
							"$XENVIRONMENT",						\
							"~/.Xdefaults-$HOSTNAME",				\
							"~/.Xresources-$HOSTNAME",				\
							"~/.Xdefaults",							\
							"~/.Xresources"
 
#define XVT_RESOURCE_DIRS	\
							"$XAPPLRESDIR",							\
							"/usr/lib/X11/app-defaults",			\
							"/usr/X/lib/app-defaults",				\
							"/usr/X/lib/locale/$LANG/app-defaults"

#define XVT_RESOURCE_PATHS	\
							"$XUSERFILESEARCHPATH"

/*------------------------------------------------------------------------
 * List of possible filenames for the RGB table.  These are files which
 * list the red/green/blue values for all defined color names.
 */
#define XVT_RGB_FILES		\
							"/usr/lib/X11/rgb.txt",					\
							"/usr/X/lib/rgb.txt"					\

/*------------------------------------------------------------------------
 * Event routine definition
 *
 * This is the routine the user must call (or cause to be called) frequently
 * when in non-threaded "here" mode.
 *
 * <data> is the "event_data" pointer returned in the XVT_DATA struct
 * from the call to xvt_run_here().
 *
 * <ms> is the number of milliseconds to wait for an event.
 * Normal values would be:
 *
 *		0 ms	Just want to check if anything there.
 *				Normal non-threaded "here" type of call.
 *
 *				The idea here is to just "give" a cpu thread
 *				to the interface loop as often as possible.
 *				Normally, this is whenever the "here" routine does any
 *				I/O to the "terminal".
 *
 *		>0 ms	Want to wait while checking. (Usually 5-10 ms).
 *				Normal "task" type of call.
 *				Normal threaded "here" type of call.
 *
 *				Also used in non-threaded "here" mode when it is
 *				in "sub-task" mode. (i.e. when in "here" mode
 *				the program has shelled-out & just needs to keep
 *				the interface alive while waiting for the sub-task
 *				to die.)
 *
 * The pointer to this routine is returned in the "event_rtn" entry in
 * the XVT_DATA struct.  If this pointer is NULL, then a sub-thread was
 * started to process events and no call is needed.
 *
 * This routine returns 0 if OK and -1 otherwise (usually because of
 * EOF from child).  A -1 return indicates that the caller should
 * terminate processing (which includes calling xvt_here_close()).
 */
typedef int		XVT_EVENT_RTN	(void *data,	/* ptr to (our) user data	*/
									int ms);	/* from user - ms to wait	*/

/*------------------------------------------------------------------------
 * xvt-data struct
 *
 * This struct provides all options to the xvt routines, and receives
 * any returned values.
 */
struct xvt_data
{
	/*--------------------------------------------------------------------
	 * Default application name
	 *
	 * This is not set via any resource processing, it is used when
	 * displaying defaults.  If set by the user, it will override the
	 * default application name when reading resource files.
	 */
	const char *	appname;				/* default app name			*/

	/*--------------------------------------------------------------------
	 * String options
	 */
	const char *	display;				/* display name				*/
	const char *	termname;				/* TERM name to set			*/
	const char *	resname;				/* alternate resource name	*/
	const char *	geometry;				/* window geometry (chars)	*/
	const char *	icongeom;				/* icon geometry (pos only)	*/
	const char *	termmode;				/* stty mode settings		*/
	const char *	ico_file;				/* icon file to use			*/
	const char *	pointer;				/* pointer type				*/
	const char *	envvar;					/* name of env var to set	*/
	const char *	win_label;				/* window label				*/
	const char *	ico_label;				/* icon label				*/
	const char *	printer;				/* printer spec				*/
	const char *	resstr;					/* resource string			*/
	const char *	resfile;				/* alternate resource file	*/
	const char *	ptyspec;				/* pty spec					*/
	const char *	terminal;				/* terminal type			*/

	/*--------------------------------------------------------------------
	 * Font options
	 */
	const char *	reg_fname[XVT_NUM_FONTS];	/* reg  font list		*/
	const char *	bld_fname[XVT_NUM_FONTS];	/* bold font list		*/

	/*--------------------------------------------------------------------
	 * Color options
	 */
	const char *	scr_color[XVT_CLR_NUM];	/* screen colors			*/

	const char *	fg_color;				/* foreground color			*/
	const char *	bg_color;				/* background color			*/

	const char *	cr_color;				/* cursor     color			*/
	const char *	pr_color;				/* pointer    color			*/
	const char *	bd_color;				/* border     color			*/

	const char *	sb_color;				/* scrollbar  color			*/
	const char *	st_color;				/* SB trough  color			*/

	/*--------------------------------------------------------------------
	 * Numeric options
	 */
	int				scr_cols;				/* initial # cols in screen	*/
	int				scr_rows;				/* initial # rows in screen	*/
	int				scr_tabs;				/* tab width				*/
	int				border;					/* border width in pixels	*/
	int				margin;					/* margin width in pixels	*/
	int				mbcols;					/* margin bell cols from rt	*/
	int				fontno;					/* font number				*/

	int				sblines;				/* number of lines to save	*/

	/*--------------------------------------------------------------------
	 * Boolean options
	 */
	int				iconic;					/* TRUE to start iconic		*/
	int				monodisp;				/* TRUE to force mono		*/
	int				login;					/* TRUE for login shell		*/
	int				ahcursor;				/* TRUE to hilite cursor	*/
	int				reverse;				/* TRUE for reverse video	*/
	int				visual;					/* TRUE for visual bell		*/
	int				autowrap;				/* TRUE for auto wraparound	*/
	int				revwrap;				/* TRUE for reverse wrap	*/
	int				mbell;					/* TRUE for margin bell		*/
	int				quiet;					/* TRUE to inhibit bell		*/
	int				ignptr;					/* TRUE to ignore mouse		*/
	int				console;				/* TRUE to get console msgs	*/
	int				lblinfo;				/* TRUE to show label info	*/
	int				kpfont;					/* TRUE for KP+/- ch fonts	*/
	int				mapoutput;				/* TRUE to map on output	*/
	int				mapalert;				/* TRUE to map on ^G output	*/
	int				a132;					/* TRUE for 80-132 switch	*/
	int				c132;					/* TRUE to set 132 col mode	*/
	int				xerrors;				/* TRUE to show X errors	*/
	int				prtkeep;				/* TRUE to keep prtr open	*/
	int				cufix;					/* TRUE to enable more fix	*/
	int				acticon;				/* TRUE for active icon		*/

	int				sb_enable;				/* TRUE to enable SB		*/
	int				sb_right;				/* TRUE for SB on right		*/
	int				sb_shadow;				/* TRUE to shadow SB		*/
	int				sb_scrout;				/* TRUE to scroll on output	*/
	int				sb_scrkey;				/* TRUE to scroll on key	*/

	/*--------------------------------------------------------------------
	 * Debug streams
	 *
	 * These are used by the libxvt routines if they were opened
	 * by the caller.  Note that if you intend to run in "here" mode,
	 * these streams should not point to stderr.  Note that xvt_here_run()
	 * will automatically NULL out (NOT close) any entries pointing to the
	 * current stderr (since stderr is reset to point to the terminal window).
	 *
	 * These may also be opened or closed by libxvt if the appropriate escape
	 * sequence is received.  libxvt does NOT close these streams at close
	 * of processing, even if it opened them.
	 *
	 * Note that setting values for the debug log pathnames does NOT open
	 * the associated log streams, but merely specifies the pathname to use
	 * if the appropriate escape sequence to open the logs is received.
	 * If a pathname is a relative pathname, it will be considered relative
	 * to the current directory.  Setting a path to "-" will use stderr.
	 */
	FILE *			events_fp;				/* txt: xlated all events	*/
	FILE *			action_fp;				/* txt: xlated win events	*/
	FILE *			output_fp;				/* txt: xlated child output	*/
	FILE *			screen_fp;				/* bin: raw    child output	*/
	FILE *			inpkbd_fp;				/* txt: xlated child input	*/
	FILE *			keybrd_fp;				/* bin: raw    child input	*/
	FILE *			resfil_fp;				/* txt: resource file		*/
	FILE *			tkopts_fp;				/* txt: options dump		*/

	char			events_path[XVT_MAX_PATHLEN];	/* events log path	*/
	char			action_path[XVT_MAX_PATHLEN];	/* action log path	*/
	char			output_path[XVT_MAX_PATHLEN];	/* output log path	*/
	char			screen_path[XVT_MAX_PATHLEN];	/* screen log path	*/
	char			inpkbd_path[XVT_MAX_PATHLEN];	/* inpkbd log path	*/
	char			keybrd_path[XVT_MAX_PATHLEN];	/* keybrd log path	*/
	char			resfil_path[XVT_MAX_PATHLEN];	/* resfil log path	*/
	char			tkopts_path[XVT_MAX_PATHLEN];	/* tkopts log path	*/

	/*--------------------------------------------------------------------
	 * Resource lists
	 *
	 * If any of these are specified, they override the default entries.
	 */
	const char **	resfiles;				/* list of resource files	*/
	const char **	resdirs;				/* list of resource dirs	*/
	const char **	respaths;				/* list of resource paths	*/

	/*--------------------------------------------------------------------
	 * Values returned from xvt_tk_args()
	 */
	int				argc;					/* new argc value			*/

	char **			argv;					/* new argv vector			*/
											/* Note: this vector is		*/
											/* freed when the struct is	*/
											/* freed, and is not copied	*/
											/* by xvt_data_copy().		*/

	/*--------------------------------------------------------------------
	 * Values returned from xvt_run_here()
	 *
	 * If threads are not available, "event_rtn" will be set to non-zero.
	 * This is the event routine the caller must be prepared to call
	 * frequently to "activate" the event mechanism.
	 * If threads are available, "event_rtn" is set to zero.
	 *
	 * Note that "event_data" will always be set, even though event_rtn may
	 * be zero.  This enables the caller to query (xd->event_data != 0) to
	 * determine if he is in an "event-rtn" type of environment.
	 * (Also, xvt_here_close() & xvt_here_clone() uses event_data for its
	 * data pointer).
	 */
	char			term_type[64];			/* term type to use			*/
	const char **	term_defs;				/* list of term definitions	*/
											/* (used by libecurses)		*/

	int				fd_inp;					/* input  file descriptor	*/
	int				fd_out;					/* output file descriptor	*/

	XVT_EVENT_RTN *	event_rtn;				/* event routine to call	*/
	void *			event_data;				/* data to call it with		*/

	/*--------------------------------------------------------------------
	 * Values returned from xvt_task_run() & xvt_here_run()
	 * and updated asynchronously by the event routine whenever the
	 * window size changes.
	 */
	int				cur_cols;				/* current # of cols in win	*/
	int				cur_rows;				/* current # of rows in win	*/

	/*--------------------------------------------------------------------
	 * Values returned from xvt_task_run()
	 */
	int				exit_code;				/* sub-tasks's exit code	*/

	/*--------------------------------------------------------------------
	 * Returned error values
	 *
	 * Whenever possible, if a routine fails, an error code (XVT_ERR_*)
	 * will be stored in error_num and a printable error message will be
	 * stored in error_msg.
	 */
	int				error_num;				/* error code				*/
	int				error_quit;				/* TRUE if pgm should quit	*/
	char			error_msg[128];			/* error msg buffer			*/

	/*--------------------------------------------------------------------
	 * Toolkit option buffer (no user access to this)
	 */
	char			tkbuf[1024];			/* string buffer			*/
	int				tkbuf_index;			/* index to current buf pos	*/
};
typedef struct xvt_data XVT_DATA;

/*------------------------------------------------------------------------
 * xvt error codes (stored in error_num)
 */
#define XVT_OK					0		/* OK								*/
#define XVT_ERR_NOTSUPP			1		/* not supported					*/
#define XVT_ERR_NOTINWIN		2		/* not in windows environment		*/
#define XVT_ERR_CANTOPENDISP	3		/* cannot open display				*/
#define XVT_ERR_CANTOPENPTYS	4		/* cannot open pseudo-ttys			*/
#define XVT_ERR_CANTLOADFONTS	5		/* cannot load requested fonts		*/
#define XVT_ERR_CANTMAKEWIN		6		/* cannot create window				*/
#define XVT_ERR_CANTFORK		7		/* cannot fork subtask				*/
#define XVT_ERR_CANTDETACH		8		/* cannot detach process			*/
#define XVT_ERR_CANTEXEC		9		/* cannot execute cmd				*/
#define XVT_ERR_NOMEM			10		/* no memory						*/
#define XVT_ERR_NOTINIT			11		/* not initialized					*/
#define XVT_ERR_NULLDATA		12		/* null data 						*/
#define XVT_ERR_INVOPT			13		/* invalid option					*/
#define XVT_ERR_INVARG			14		/* invalid option argument			*/

/*------------------------------------------------------------------------
 * Emulator error exit code
 *
 * We return exit status 127 if an error occurs when running a sub-task
 * so that users can distinguish "failure to run a cmd" from
 * "invoked cmd exited with an error indication".
 *
 * The value 127 was chosen because it is not commonly
 * used for other meanings; most programs use small values for
 * normal error conditions and values of 128 or above can be
 * confused with termination due to receipt of a signal.
 */
#define XVT_ERR_EMULATOR		127		/* emulator failed to run cmd		*/

/*------------------------------------------------------------------------
 * Query whether XVT is available.
 *
 * Returns: TRUE or FALSE
 */
extern int	xvt_available		(void);

/*------------------------------------------------------------------------
 * Terminal-emulator program
 *
 * This is the working main of the terminal-emulator program.
 * The "real" main just calls this & returns.
 *
 * If this routine is not sucessful in running the specified command,
 * it will return XVT_ERR_EMULATOR with an error msg displayed to stderr.
 * Otherwise, it returns the exit code from the command which was run.
 */
extern int	xvt_main			(int argc, char **argv);

/*------------------------------------------------------------------------
 * Run in "task" mode
 *
 * This runs the "cmd" specified by the argv vector as a sub-task.
 * If argv is NULL or empty, then $SHELL is used.  If $SHELL is
 * NULL or empty, then XVT_SHELL is used.
 *
 * It returns when the sub-task dies.  If it failed in running the
 * specified command, it returns -1.
 * If it successfully ran the specified command, it returns 0 and the
 * exit code from the command will be stored in the "exit_code" field
 * of the XVT_DATA struct.
 */
extern int	xvt_task_run		(XVT_DATA *xd,
									char **argv);

/*------------------------------------------------------------------------
 * Run in "here" mode.
 *
 * This sets up the display and the pseudo-tty for the interface,
 * and either starts a thread to process the interface or
 * stores a pointer to an event routine to be called by the user.
 * The user should be prepared to call or have called the specified
 * event routine frequently if it is not zero.
 *
 * If "detach" is TRUE, then the program is made into a pseudo-daemon,
 * with stdin/stdout/stderr pointing to the pseudo-tty created.
 *
 * This routine returns after the initial setup. The return value is
 * 0 if successful, and -1 if not.
 *
 * If this call fails, then the entry "error_quit" will be set to
 * FALSE if it is possible to continue and TRUE if not.
 */
extern int	xvt_here_run		(XVT_DATA *xd,
									int detach);

/*------------------------------------------------------------------------
 * Run a sub-task but keep control of process
 *
 * This routine will run the sub-task specified, but return control to
 * the caller after spawning the sub-task.  It is the user's responsibility
 * to query for child termination (using xvt_here_check()).
 *
 * This routine returns the pid of the sub-task if successful, and -1 if not.
 */
extern int	xvt_here_task		(XVT_DATA *xd,
									int detach,
									char **argv);

/*------------------------------------------------------------------------
 * Close out processing when in "here" mode
 *
 * This call will shutdown any sub-thread which was started to process
 * events and will free all data structs which were allocated.
 *
 * This routine returns 0 if successful, and -1 if
 * xvt_here_run() was never called (or if the call failed).
 */
extern int	xvt_here_close		(XVT_DATA *xd);

/*------------------------------------------------------------------------
 * Perform a check when in "here" mode
 *
 * This call is the same as doing the call:
 *
 *	if (xvt_data_get_event_rtn(xd) != 0)
 *		(xvt_data_get_event_rtn(xd))(xvt_data_get_event_data(xd), ms)
 *
 * This routine returns 0 if successful, and -1 if the caller should terminate
 * processing (due to an EOF or sub-task termination).
 */
extern int	xvt_here_check		(XVT_DATA *xd,
									int ms);

/*------------------------------------------------------------------------
 * Create a "clone" of the current task with a new window
 *
 * Note that this routine has two returns: the "parent" and the "child"
 * returns.  After returning, the two tasks are equivalent (i.e. they
 * are both running as "pseudo-daemons").  This means that after a successful
 * return, there is no way to determine which task was the original one
 * unless you cached your PID prior to the call (if you even care ...).
 *
 * Note also that this routine is only valid when running in "here" mode
 * (i.e. after a succesful call to xvt_here_run() is made).
 *
 * This routine returns 0 if a clone was successfully created, and -1 if not.
 */
extern int	xvt_here_clone		(XVT_DATA *xd);

/*------------------------------------------------------------------------
 * Present a buffer as input to the sub-process
 *
 * This call passes the contents of the specified buffer as input to
 * the pty, thus sending it to the sub-process as if it were typed
 * on the keyboard.
 *
 * This routine returns 0 if successful and -1 if not.
 */
extern int	xvt_here_input		(XVT_DATA *xd,
									const void *buf,
									int len);

/*------------------------------------------------------------------------
 * Present a buffer as output from the sub-process
 *
 * This call passes the contents of the specified buffer as output from
 * the pty, thus treating it as if the sub-process output it.
 *
 * Note that this call behaves differently from xvt_here_input(), in that
 * in xvt_here_input() the data is presented to the sub-process, where here
 * the data is just displayed as if the sub-process output it.
 *
 * This routine returns 0 if successful and -1 if not.
 */
extern int	xvt_here_output		(XVT_DATA *xd,
									const void *buf,
									int len);

/*------------------------------------------------------------------------
 * Extract toolkit options from an argv vector
 *
 * This routine will extract all toolkit options from an argv vector
 * and will store a new argc & argv with the toolkit options removed.
 *
 * This routine returns 0 if successful and -1 if not.
 */
extern int	xvt_tk_args			(XVT_DATA *xd,
									int argc,
									char **argv);

/*------------------------------------------------------------------------
 * Process an option list
 *
 * <list> is in the form "<option>=<value>, ..."
 *
 * This routine returns 0 if successful and -1 if not.
 */
extern int	xvt_tk_optlist		(XVT_DATA *xd,
									const char *list);

/*------------------------------------------------------------------------
 * Process all known resources
 *
 * This routine will process all "standard" resource files, in the order
 * of "most-user-specific" to "most-generic".  Any value found in a resource
 * file will NOT be used unless the entry for that value is currently unset.
 *
 * This routine returns 0 if all option values are valid and -1 if not.
 */
extern int	xvt_tk_resource		(XVT_DATA *xd);

/*------------------------------------------------------------------------
 * Output a list of valid toolkit options
 *
 * Note that <xd> may be NULL in this call.  It is only used for the
 * <appname> entry, which will override the default appname.
 *
 * type:
 *			XVT_TK_HELP_SHORT		short    listing
 *			XVT_TK_HELP_LONG		long     listing
 *			XVT_TK_HELP_DETAIL		detailed listing
 *
 * mode:
 *			XVT_TK_HELP_TASK		running in "task" mode
 *									(i.e. show only those entries which are
 *									applicable to "task" mode)
 *			XVT_TK_HELP_HERE		running in "here" mode
 *									(i.e. show only those entries which are
 *									applicable to "here" mode)
 */
#define XVT_TK_HELP_SHORT	0
#define XVT_TK_HELP_LONG	1
#define XVT_TK_HELP_DETAIL	2

#define XVT_TK_HELP_TASK	0
#define XVT_TK_HELP_HERE	1

extern void	xvt_tk_help			(XVT_DATA *xd,
									FILE *fp,
									int type,
									int mode);

/*------------------------------------------------------------------------
 * Load all data from a resource file
 *
 * This routine returns 0 if all option values are valid, and -1 if not.
 */
extern int	xvt_tk_resfile_load	(XVT_DATA *xd,
									const char *path,
									const char *resname);

/*------------------------------------------------------------------------
 * Save all data to a resource file
 *
 * This routine returns 0 if all option values are valid, and -1 if not.
 */
extern int	xvt_tk_resfile_save	(XVT_DATA *xd,
									const char *path,
									const char *resname);

/*------------------------------------------------------------------------
 * Dump option data to a stream
 *
 * Normally, only options which were set are dumped, but if <show_dflt>
 * is set, then all values are shown, with values which are default being
 * shown in parens.
 *
 * This routine returns 0 if all option values are valid, and -1 if not.
 */
extern int	xvt_tk_options_dump	(XVT_DATA *xd,
									FILE *fp,
									int show_dflt);

/*------------------------------------------------------------------------
 * Various dump routines
 *
 *	xvt_dump_colors()		dump list of available color names
 *	xvt_dump_pointers()		dump list of pointer names
 *	xvt_dump_resources()	dump list of resources to be queried
 */
extern void	xvt_dump_colors		(FILE *fp, int verbose);
extern void xvt_dump_pointers	(FILE *fp, int verbose);
extern void xvt_dump_resources	(FILE *fp, int verbose, const char *resname);

/*------------------------------------------------------------------------
 * Routines for accessing the XVT_DATA struct.
 *
 * By using these routines, the XVT_DATA struct can be treated as opaque.
 *
 * Note that setting string values (except for debug log pathnames) only
 * stores pointers to the strings passed.  It is the caller's responsibility
 * to insure these pointers remain valid.
 *
 * Note also that xvt_data_alloc() will automatically initialize the data
 * struct to all "unset" values.
 */
extern XVT_DATA *	xvt_data_alloc				(void);
extern XVT_DATA *	xvt_data_free				(XVT_DATA *xd);
extern void			xvt_data_init				(XVT_DATA *xd, int set_dflt);
extern XVT_DATA *	xvt_data_copy				(XVT_DATA *xd);

extern void			xvt_data_set_scr_cols		(XVT_DATA *xd, int n);
extern void			xvt_data_set_scr_rows		(XVT_DATA *xd, int n);
extern void			xvt_data_set_scr_tabs		(XVT_DATA *xd, int n);
extern void			xvt_data_set_border			(XVT_DATA *xd, int n);
extern void			xvt_data_set_margin			(XVT_DATA *xd, int n);
extern void			xvt_data_set_mbcols			(XVT_DATA *xd, int n);
extern void			xvt_data_set_fontno			(XVT_DATA *xd, int n);
extern void			xvt_data_set_sblines		(XVT_DATA *xd, int n);

extern void			xvt_data_set_iconic			(XVT_DATA *xd, int b);
extern void			xvt_data_set_monodisp		(XVT_DATA *xd, int b);
extern void			xvt_data_set_login			(XVT_DATA *xd, int b);
extern void			xvt_data_set_ahcursor		(XVT_DATA *xd, int b);
extern void			xvt_data_set_reverse		(XVT_DATA *xd, int b);
extern void			xvt_data_set_visual			(XVT_DATA *xd, int b);
extern void			xvt_data_set_autowrap		(XVT_DATA *xd, int b);
extern void			xvt_data_set_revwrap		(XVT_DATA *xd, int b);
extern void			xvt_data_set_mbell			(XVT_DATA *xd, int b);
extern void			xvt_data_set_quiet			(XVT_DATA *xd, int b);
extern void			xvt_data_set_ignptr			(XVT_DATA *xd, int b);
extern void			xvt_data_set_console		(XVT_DATA *xd, int b);
extern void			xvt_data_set_lblinfo		(XVT_DATA *xd, int b);
extern void			xvt_data_set_kpfont			(XVT_DATA *xd, int b);
extern void			xvt_data_set_mapoutput		(XVT_DATA *xd, int b);
extern void			xvt_data_set_mapalert		(XVT_DATA *xd, int b);
extern void			xvt_data_set_a132			(XVT_DATA *xd, int b);
extern void			xvt_data_set_c132			(XVT_DATA *xd, int b);
extern void			xvt_data_set_xerrors		(XVT_DATA *xd, int b);
extern void			xvt_data_set_prtkeep		(XVT_DATA *xd, int b);
extern void			xvt_data_set_cufix			(XVT_DATA *xd, int b);
extern void			xvt_data_set_acticon		(XVT_DATA *xd, int b);
extern void			xvt_data_set_sb_enable		(XVT_DATA *xd, int b);
extern void			xvt_data_set_sb_right		(XVT_DATA *xd, int b);
extern void			xvt_data_set_sb_shadow		(XVT_DATA *xd, int b);
extern void			xvt_data_set_sb_scrout		(XVT_DATA *xd, int b);
extern void			xvt_data_set_sb_scrkey		(XVT_DATA *xd, int b);

extern void			xvt_data_set_appname		(XVT_DATA *xd, const char *s);

extern void			xvt_data_set_reg_fname		(XVT_DATA *xd, const char *s,
													int n);
extern void			xvt_data_set_bld_fname		(XVT_DATA *xd, const char *s,
													int n);

extern void			xvt_data_set_display		(XVT_DATA *xd, const char *s);
extern void			xvt_data_set_termname		(XVT_DATA *xd, const char *s);
extern void			xvt_data_set_resname		(XVT_DATA *xd, const char *s);
extern void			xvt_data_set_geometry		(XVT_DATA *xd, const char *s);
extern void			xvt_data_set_icongeom		(XVT_DATA *xd, const char *s);
extern void			xvt_data_set_termmode		(XVT_DATA *xd, const char *s);
extern void			xvt_data_set_ico_file		(XVT_DATA *xd, const char *s);
extern void			xvt_data_set_pointer		(XVT_DATA *xd, const char *s);
extern void			xvt_data_set_envvar			(XVT_DATA *xd, const char *s);
extern void			xvt_data_set_win_label		(XVT_DATA *xd, const char *s);
extern void			xvt_data_set_ico_label		(XVT_DATA *xd, const char *s);
extern void			xvt_data_set_printer		(XVT_DATA *xd, const char *s);
extern void			xvt_data_set_resstr			(XVT_DATA *xd, const char *s);
extern void			xvt_data_set_resfile		(XVT_DATA *xd, const char *s);
extern void			xvt_data_set_ptyspec		(XVT_DATA *xd, const char *s);
extern void			xvt_data_set_terminal		(XVT_DATA *xd, const char *s);

extern void			xvt_data_set_scr_color		(XVT_DATA *xd, const char *s,
													int n);

extern void			xvt_data_set_fg_color		(XVT_DATA *xd, const char *s);
extern void			xvt_data_set_bg_color		(XVT_DATA *xd, const char *s);
extern void			xvt_data_set_cr_color		(XVT_DATA *xd, const char *s);
extern void			xvt_data_set_pr_color		(XVT_DATA *xd, const char *s);
extern void			xvt_data_set_bd_color		(XVT_DATA *xd, const char *s);
extern void			xvt_data_set_sb_color		(XVT_DATA *xd, const char *s);
extern void			xvt_data_set_st_color		(XVT_DATA *xd, const char *s);

extern void			xvt_data_set_events_fp		(XVT_DATA *xd, FILE *fp);
extern void			xvt_data_set_action_fp		(XVT_DATA *xd, FILE *fp);
extern void			xvt_data_set_output_fp		(XVT_DATA *xd, FILE *fp);
extern void			xvt_data_set_screen_fp		(XVT_DATA *xd, FILE *fp);
extern void			xvt_data_set_inpkbd_fp		(XVT_DATA *xd, FILE *fp);
extern void			xvt_data_set_keybrd_fp		(XVT_DATA *xd, FILE *fp);
extern void			xvt_data_set_resfil_fp		(XVT_DATA *xd, FILE *fp);
extern void			xvt_data_set_tkopts_fp		(XVT_DATA *xd, FILE *fp);

extern void			xvt_data_set_events_path	(XVT_DATA *xd, const char *p);
extern void			xvt_data_set_action_path	(XVT_DATA *xd, const char *p);
extern void			xvt_data_set_output_path	(XVT_DATA *xd, const char *p);
extern void			xvt_data_set_screen_path	(XVT_DATA *xd, const char *p);
extern void			xvt_data_set_inpkbd_path	(XVT_DATA *xd, const char *p);
extern void			xvt_data_set_keybrd_path	(XVT_DATA *xd, const char *p);
extern void			xvt_data_set_resfil_path	(XVT_DATA *xd, const char *p);
extern void			xvt_data_set_tkopts_path	(XVT_DATA *xd, const char *p);

extern void			xvt_data_set_resfiles		(XVT_DATA *xd, const char **p);
extern void			xvt_data_set_resdirs		(XVT_DATA *xd, const char **p);
extern void			xvt_data_set_respaths		(XVT_DATA *xd, const char **p);

extern int			xvt_data_get_scr_cols		(XVT_DATA *xd);
extern int			xvt_data_get_scr_rows		(XVT_DATA *xd);
extern int			xvt_data_get_scr_tabs		(XVT_DATA *xd);
extern int			xvt_data_get_border			(XVT_DATA *xd);
extern int			xvt_data_get_margin			(XVT_DATA *xd);
extern int			xvt_data_get_mbcols			(XVT_DATA *xd);
extern int			xvt_data_get_fontno			(XVT_DATA *xd);
extern int			xvt_data_get_sblines		(XVT_DATA *xd);

extern int			xvt_data_get_iconic			(XVT_DATA *xd);
extern int			xvt_data_get_monodisp		(XVT_DATA *xd);
extern int			xvt_data_get_login			(XVT_DATA *xd);
extern int			xvt_data_get_ahcursor		(XVT_DATA *xd);
extern int			xvt_data_get_reverse		(XVT_DATA *xd);
extern int			xvt_data_get_visual			(XVT_DATA *xd);
extern int			xvt_data_get_autowrap		(XVT_DATA *xd);
extern int			xvt_data_get_revwrap		(XVT_DATA *xd);
extern int			xvt_data_get_mbell			(XVT_DATA *xd);
extern int			xvt_data_get_quiet			(XVT_DATA *xd);
extern int			xvt_data_get_ignptr			(XVT_DATA *xd);
extern int			xvt_data_get_console		(XVT_DATA *xd);
extern int			xvt_data_get_lblinfo		(XVT_DATA *xd);
extern int			xvt_data_get_kpfont			(XVT_DATA *xd);
extern int			xvt_data_get_mapoutput		(XVT_DATA *xd);
extern int			xvt_data_get_mapalert		(XVT_DATA *xd);
extern int			xvt_data_get_a132			(XVT_DATA *xd);
extern int			xvt_data_get_c132			(XVT_DATA *xd);
extern int			xvt_data_get_xerrors		(XVT_DATA *xd);
extern int			xvt_data_get_prtkeep		(XVT_DATA *xd);
extern int			xvt_data_get_cufix			(XVT_DATA *xd);
extern int			xvt_data_get_acticon		(XVT_DATA *xd);
extern int			xvt_data_get_sb_enable		(XVT_DATA *xd);
extern int			xvt_data_get_sb_right		(XVT_DATA *xd);
extern int			xvt_data_get_sb_shadow		(XVT_DATA *xd);
extern int			xvt_data_get_sb_scrout		(XVT_DATA *xd);
extern int			xvt_data_get_sb_scrkey		(XVT_DATA *xd);

extern const char *	xvt_data_get_appname		(XVT_DATA *xd);

extern const char *	xvt_data_get_reg_fname		(XVT_DATA *xd, int n);
extern const char *	xvt_data_get_bld_fname		(XVT_DATA *xd, int n);

extern const char *	xvt_data_get_display		(XVT_DATA *xd);
extern const char *	xvt_data_get_termname		(XVT_DATA *xd);
extern const char *	xvt_data_get_resname		(XVT_DATA *xd);
extern const char *	xvt_data_get_geometry		(XVT_DATA *xd);
extern const char *	xvt_data_get_icongeom		(XVT_DATA *xd);
extern const char *	xvt_data_get_termmode		(XVT_DATA *xd);
extern const char *	xvt_data_get_ico_file		(XVT_DATA *xd);
extern const char *	xvt_data_get_pointer		(XVT_DATA *xd);
extern const char *	xvt_data_get_envvar			(XVT_DATA *xd);
extern const char *	xvt_data_get_win_label		(XVT_DATA *xd);
extern const char *	xvt_data_get_ico_label		(XVT_DATA *xd);
extern const char *	xvt_data_get_printer		(XVT_DATA *xd);
extern const char *	xvt_data_get_resstr			(XVT_DATA *xd);
extern const char *	xvt_data_get_resfile		(XVT_DATA *xd);
extern const char *	xvt_data_get_ptyspec		(XVT_DATA *xd);
extern const char *	xvt_data_get_terminal		(XVT_DATA *xd);

extern const char *	xvt_data_get_scr_color		(XVT_DATA *xd, int n);
extern const char *	xvt_data_get_fg_color		(XVT_DATA *xd);
extern const char *	xvt_data_get_bg_color		(XVT_DATA *xd);
extern const char *	xvt_data_get_cr_color		(XVT_DATA *xd);
extern const char *	xvt_data_get_pr_color		(XVT_DATA *xd);
extern const char *	xvt_data_get_bd_color		(XVT_DATA *xd);
extern const char *	xvt_data_get_sb_color		(XVT_DATA *xd);
extern const char *	xvt_data_get_st_color		(XVT_DATA *xd);

extern FILE *		xvt_data_get_events_fp		(XVT_DATA *xd);
extern FILE *		xvt_data_get_action_fp		(XVT_DATA *xd);
extern FILE *		xvt_data_get_output_fp		(XVT_DATA *xd);
extern FILE *		xvt_data_get_screen_fp		(XVT_DATA *xd);
extern FILE *		xvt_data_get_inpkbd_fp		(XVT_DATA *xd);
extern FILE *		xvt_data_get_keybrd_fp		(XVT_DATA *xd);
extern FILE *		xvt_data_get_resfil_fp		(XVT_DATA *xd);
extern FILE *		xvt_data_get_tkopts_fp		(XVT_DATA *xd);

extern const char *	xvt_data_get_events_path	(XVT_DATA *xd);
extern const char *	xvt_data_get_action_path	(XVT_DATA *xd);
extern const char *	xvt_data_get_output_path	(XVT_DATA *xd);
extern const char *	xvt_data_get_screen_path	(XVT_DATA *xd);
extern const char *	xvt_data_get_inpkbd_path	(XVT_DATA *xd);
extern const char *	xvt_data_get_keybrd_path	(XVT_DATA *xd);
extern const char *	xvt_data_get_resfil_path	(XVT_DATA *xd);
extern const char *	xvt_data_get_tkopts_path	(XVT_DATA *xd);

extern const char **xvt_data_get_resfiles		(XVT_DATA *xd);
extern const char **xvt_data_get_resdirs		(XVT_DATA *xd);
extern const char **xvt_data_get_respaths		(XVT_DATA *xd);

extern int			xvt_data_get_argc			(XVT_DATA *xd);
extern char **		xvt_data_get_argv			(XVT_DATA *xd);

extern const char *	xvt_data_get_term_type		(XVT_DATA *xd);
extern const char **xvt_data_get_term_defs		(XVT_DATA *xd);
extern int			xvt_data_get_fd_inp			(XVT_DATA *xd);
extern int			xvt_data_get_fd_out			(XVT_DATA *xd);
extern XVT_EVENT_RTN *
					xvt_data_get_event_rtn		(XVT_DATA *xd);
extern void *		xvt_data_get_event_data		(XVT_DATA *xd);
extern int			xvt_data_get_cur_cols		(XVT_DATA *xd);
extern int			xvt_data_get_cur_rows		(XVT_DATA *xd);
extern int			xvt_data_get_exit_code		(XVT_DATA *xd);
extern int			xvt_data_get_error_num		(XVT_DATA *xd);
extern int			xvt_data_get_error_quit		(XVT_DATA *xd);
extern const char *	xvt_data_get_error_msg		(XVT_DATA *xd);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* LIBXVT_H */
