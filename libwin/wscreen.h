/*------------------------------------------------------------------------
 * screen struct definition
 */
#ifndef WSCREEN_H
#define WSCREEN_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * WINDOW typedef
 *
 * We provide this as a forward reference so we can use it in
 * the following structs.
 */
#ifndef WINDOW_T
#  define WINDOW_T		TRUE			/* indicate WINDOW defined		*/
typedef	struct _win_st	WINDOW;
#endif

/*------------------------------------------------------------------------
 * ripoff struct
 */
struct ripoff_entry
{
	int				line;				/* top/bottom ripoff			*/
	WINDOW *		win;				/* window pointer				*/
	int				(*init)(WINDOW *win, int cols);
};
typedef struct ripoff_entry RIPOFF_ENTRY;

struct ripoff
{
	int				num_ents;			/* number of used entries		*/
	int				num_top;			/* number to take off top		*/
	int				num_bot;			/* number to take off bot		*/
	RIPOFF_ENTRY	entries[5];			/* entry table					*/
};
typedef struct ripoff RIPOFF;

/*------------------------------------------------------------------------
 * slk structs
 */
struct slk_label
{
	int			label_fmt;				/* format of label				*/
	int			label_code;				/* mouse code					*/
	char		label_text[8 + 1];		/* label text					*/
	char		label_disp[8 + 1];		/* label display				*/
};
typedef struct slk_label SLK_LABEL;

struct slk_data
{
	WINDOW *	win;					/* slk window					*/
	int			layout_fmt;				/* layout format				*/
	int			num_labels;				/* number of labels				*/
	int			lbls_per_line;			/* labels per line				*/
	attr_t		lb_attrs;				/* label attribute				*/
	attr_t		sp_attrs;				/* space attribute				*/
	SLK_LABEL	labels[16];				/* labels						*/
};
typedef struct slk_data SLK_DATA;

/*------------------------------------------------------------------------
 * WINDOW list
 */
typedef struct winlist WINLIST;
struct winlist
{
	WINLIST *	prev;					/* ptr to prev entry in list	*/
	WINLIST *	next;					/* ptr to next entry in list	*/
	WINDOW *	win;					/* window pointer				*/
};

/*------------------------------------------------------------------------
 * clock routine definitions
 *
 * This routine is called asyncronously with the local time as described
 * below.
 *
 * This routine does not need to refresh the window (that will done by the
 * calling routine).
 * Note that the window is erased prior to the call.
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

#define WIN_CLOCK_T		TRUE		/* WIN_CLOCK defined				*/
#endif

/*------------------------------------------------------------------------
 * async clock data
 */
typedef struct win_clock WIN_CLOCK;
struct win_clock
{
	WINDOW *		clk_win;		/* window for clock routine		*/
	WIN_CLOCK_RTN *	clk_rtn;		/* clock routine				*/
	void *			clk_data;		/* user data to pass to rtn		*/
	int				clk_intvl;		/* update interval				*/
	time_t			clk_defer;		/* deferred time				*/
};

/*------------------------------------------------------------------------
 * MOUSE_INFO struct
 */
struct mouse_info
{
	/*--------------------------------------------------------------------
	 * previous mouse info
	 */
	int				prev_y;		/* prev x position				*/
	int				prev_x;		/* prev y position				*/
	int				prev_c;		/* prev code value				*/

	/*--------------------------------------------------------------------
	 * display attributes
	 */
	attr_t			ptr_attrs;	/* attrs for mouse position		*/
	attr_t			cmd_attrs;	/* attrs for cmd string			*/

	/*--------------------------------------------------------------------
	 * flags
	 */
	int				process;	/* TRUE if processing the mouse	*/
	int				display;	/* TRUE if mouse is displayed	*/
};
typedef struct mouse_info MOUSE_INFO;

/*------------------------------------------------------------------------
 * SCREEN struct
 */
struct screen
{
	/*--------------------------------------------------------------------
	 * status flags
	 */
	int			in_curses;				/* TRUE if in curses mode		*/
	int			in_refresh;				/* TRUE if doing a refresh		*/
	int			do_echo;				/* TRUE if echo of input wanted	*/
	int			do_inp_nl;				/* TRUE if cr -> nl wanted		*/
	int			do_out_nl;				/* TRUE if nl -> cr/nl wanted	*/

	/*--------------------------------------------------------------------
	 * window pointers
	 */
	WINDOW *	stdscr_ptr;				/* pointer to stdscr			*/
	WINDOW *	curscr_ptr;				/* pointer to curscr			*/
	WINDOW *	trmscr_ptr;				/* pointer to trmscr			*/

	/*--------------------------------------------------------------------
	 * window list
	 */
	WINLIST *	winlist;				/* list of windows				*/

	/*--------------------------------------------------------------------
	 * slk data
	 */
	SLK_DATA	slk_data;				/* slk data struct				*/

	/*--------------------------------------------------------------------
	 * ripoff data
	 */
	RIPOFF		ripoff_tbl;				/* copy of global ripoff tbl	*/

	/*--------------------------------------------------------------------
	 * mouse data
	 */
	MOUSE_INFO	mouse_info;				/* mouse data					*/

	/*--------------------------------------------------------------------
	 * clock data
	 */
	WIN_CLOCK	clock_tbl;				/* clock data					*/

	/*--------------------------------------------------------------------
	 * low-level data
	 */
	TERMINAL *	term;					/* pointer to terminal			*/

	/*--------------------------------------------------------------------
	 * user data
	 */
	void *		usrdata;				/* pointer to user data			*/
	int			data_on_heap;			/* TRUE if on heap				*/
};
typedef struct screen SCREEN;

#define SCREEN_T	TRUE				/* SCREEN defined				*/

/*------------------------------------------------------------------------
 * SCREEN list
 */
typedef struct scrlist SCRLIST;
struct scrlist
{
	SCRLIST *	prev;					/* ptr to prev entry in list	*/
	SCRLIST *	next;					/* ptr to next entry in list	*/
	SCREEN *	screen;					/* ptr to screen entry			*/
};

/*------------------------------------------------------------------------
 * accessor macros
 */
#define SCR_TERM(s)			((s)->term)

#define SCR_STDSCR(s)		((s)->stdscr_ptr)
#define SCR_CURSCR(s)		((s)->curscr_ptr)
#define SCR_TRMSCR(s)		((s)->trmscr_ptr)

#define SCR_SLKDATA(s)		(&(s)->slk_data)
#define SCR_RIPOFF(s)		(&(s)->ripoff_tbl)
#define SCR_CLOCK(s)		(&(s)->clock_tbl)

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* WSCREEN_H */
