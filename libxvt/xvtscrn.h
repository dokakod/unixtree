/*------------------------------------------------------------------------
 * TERMSCRN header
 */
#ifndef XVTSCRN_H
#define XVTSCRN_H

/*------------------------------------------------------------------------
 * char-set info
 */
#define CHAR_SET_NONE		-1
#define CHAR_SET_G0			0
#define CHAR_SET_G1			1
#define CHAR_SET_G2			2
#define CHAR_SET_G3			3

#define NUM_CHAR_SETS		4

/*------------------------------------------------------------------------
 * cell struct
 */
struct cell
{
	unsigned char	fg;				/* foreground color					*/
	unsigned char	bg;				/* background color					*/
	unsigned char	at;				/* attribute						*/
	unsigned char	ch;				/* char code						*/
};
typedef struct cell CELL;

/*------------------------------------------------------------------------
 * screen struct
 */
struct termscrn
{
	/*--------------------------------------------------------------------
	 * TERMSCRN pointer & size
	 */
	CELL *			scr_ptr;		/* ptr to array of (w * h) cells	*/
	int				scr_cols;		/* # cols in in each line			*/
	int				scr_rows;		/* # rows of lines					*/
	int				scr_tabs;		/* tab width						*/

	/*--------------------------------------------------------------------
	 * TERMSCRN margins
	 */
	int				scr_top;		/* top    margin					*/
	int				scr_bot;		/* bottom margin					*/

	int				scr_left;		/* left   margin					*/
	int				scr_right;		/* right  margin					*/

	/*--------------------------------------------------------------------
	 * TERMSCRN tab array
	 */
	unsigned char *	scr_tabstops;	/* 0 == unset, 1 == set				*/

	/*--------------------------------------------------------------------
	 * TERMSCRN attributes
	 */
	int				scr_mode;		/* current screen mode				*/
	int				scr_fg;			/* current fg color					*/
	int				scr_bg;			/* current bg color					*/
	int				scr_at;			/* current attribute				*/

	int				scr_gcs[NUM_CHAR_SETS];	/* current  char sets		*/
	int				scr_cs;			/* current character set			*/
	int				scr_tmpcs;		/* temp    character set			*/

	int				scr_x;			/* current x position				*/
	int				scr_y;			/* current y position				*/

	/*--------------------------------------------------------------------
	 * TERMSCRN saved attributes
	 */
	int				scr_save_mode;	/* saved screen mode				*/
	int				scr_save_fg;	/* saved fg color					*/
	int				scr_save_bg;	/* saved bg color					*/
	int				scr_save_at;	/* saved attribute					*/

	int				scr_save_gcs[NUM_CHAR_SETS];	/* saved char sets	*/
	int				scr_save_cs;	/* saved character set				*/

	int				scr_save_x;		/* saved x position					*/
	int				scr_save_y;		/* saved y position					*/

	/*--------------------------------------------------------------------
	 * TERMSCRN modes
	 */
	int				scr_ins;		/* TRUE if insert mode				*/
	int				scr_org;		/* TRUE if in origin mode			*/
	int				scr_lock;		/* lock all lines above if > 0		*/

	int				scr_csv;		/* TRUE if cursor is visible		*/
	int				scr_con;		/* TRUE if cursor is on				*/
};
typedef struct termscrn TERMSCRN;

/*------------------------------------------------------------------------
 * accessor macros
 */
#define SCRN_COLS(s)		( (s)->scr_cols )
#define SCRN_ROWS(s)		( (s)->scr_rows )
#define SCRN_TABS(s)		( (s)->scr_tabs )

#define SCRN_TOP(s)			( (s)->scr_top )
#define SCRN_BOT(s)			( (s)->scr_bot )
#define SCRN_LEFT(s)		( (s)->scr_left )
#define SCRN_RIGHT(s)		( (s)->scr_right )

#define SCRN_DISP(s)		( (s)->scr_ptr )
#define SCRN_LINE(s,y)		( SCRN_DISP(s) + ((y) * SCRN_COLS(s)) )
#define SCRN_CHAR(s,y,x)	( SCRN_LINE(s,y) + (x) )

#define SCRN_TABSTOPS(s)	( (s)->scr_tabstops )
#define SCRN_TABSTOP(s,x)	( SCRN_TABSTOPS(s)[x] )

#define SCRN_MODE(s)		( (s)->scr_mode )
#define SCRN_FG(s)			( (s)->scr_fg )
#define SCRN_BG(s)			( (s)->scr_bg )
#define SCRN_AT(s)			( (s)->scr_at )

#define SCRN_GCS(s, n)		( (s)->scr_gcs[n] )
#define SCRN_CS(s)			( (s)->scr_cs )
#define SCRN_TMPCS(s)		( (s)->scr_tmpcs )

#define SCRN_X(s)			( (s)->scr_x )
#define SCRN_Y(s)			( (s)->scr_y )

#define SCRN_SAVE_MODE(s)	( (s)->scr_save_mode )
#define SCRN_SAVE_FG(s)		( (s)->scr_save_fg )
#define SCRN_SAVE_BG(s)		( (s)->scr_save_bg )
#define SCRN_SAVE_AT(s)		( (s)->scr_save_at )

#define SCRN_SAVE_GCS(s, n)	( (s)->scr_save_gcs[n] )
#define SCRN_SAVE_CS(s)		( (s)->scr_save_cs )

#define SCRN_SAVE_X(s)		( (s)->scr_save_x )
#define SCRN_SAVE_Y(s)		( (s)->scr_save_y )

#define SCRN_INS(s)			( (s)->scr_ins )
#define SCRN_ORG(s)			( (s)->scr_org )
#define SCRN_LOCK(s)		( (s)->scr_lock )

#define SCRN_CSV(s)			( (s)->scr_csv )
#define SCRN_CON(s)			( (s)->scr_con )

/*------------------------------------------------------------------------
 * tab-set modes
 */
#define SCRN_TAB_SET_CUR	1
#define SCRN_TAB_SET_ALL	2
#define SCRN_TAB_CLR_CUR	3
#define SCRN_TAB_CLR_ALL	4

/*------------------------------------------------------------------------
 * functions
 */
extern TERMSCRN *	xvt_scrn_init			(int rows, int cols, int tabs,
												int mode, int fg, int bg,
												int at);

extern TERMSCRN *	xvt_scrn_free			(TERMSCRN *s);

extern void			xvt_scrn_reset			(TERMSCRN *s);

extern void			xvt_scrn_set_vmargins	(TERMSCRN *s, int top,  int bot);
extern void			xvt_scrn_set_hmargins	(TERMSCRN *s, int left, int right);
extern void			xvt_scrn_clear			(TERMSCRN *s, int prot);
extern void			xvt_scrn_copy			(TERMSCRN *t, const TERMSCRN *s);
extern void			xvt_scrn_copy_lines		(TERMSCRN *s, int yt, int yf,
												int n);
extern void			xvt_scrn_clear_lines	(TERMSCRN *s, int y, int n,
												int prot);

extern void			xvt_scrn_tab_set		(TERMSCRN *s, int mode);

extern void			xvt_scrn_print_line		(TERMSCRN *s, PRINTER *p,
												int y);

extern void			xvt_scrn_print			(TERMSCRN *s, PRINTER *p,
												int top, int bot,
												int left, int right, int ff);

#endif /* XVTSCRN_H */
