/*------------------------------------------------------------------------
 *	header for termcap data
 *
 *	This struct contains all "termcap" data for a display, i.e.
 *	all information about input/output escape sequences, screen
 *	size, etc.
 *
 *	This information is loaded at screen initialization time and
 *	remains constant for the duration.
 *
 * This header is private to this library only.
 */
#ifndef TCAPDATA_H
#define TCAPDATA_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * macro for checking if a string is present
 */
#define is_cmd_pres(s)	( (s != (0)) && (s != (char *)(-1)) )

/*------------------------------------------------------------------------
 * number of font tables defined
 */
#define NUM_FONTS		4			/* number of fonts					*/

/*------------------------------------------------------------------------
 * size of buffer for load routines
 */
#define TCAP_BUFLEN			4096

/*------------------------------------------------------------------------
 * KFUNC struct
 *
 *	This struct represents a list of all key definitions.
 */
typedef struct kfunc KFUNC;
struct kfunc
{
	KFUNC *	kf_next;				/* ptr to next entry				*/
	KFUNC *	kf_prev;				/* ptr to prev entry				*/

	int		kf_code;				/* key code							*/
	char *	kf_str;					/* escape-string					*/
};

/*------------------------------------------------------------------------
 * FKEY struct
 *
 *	This struct represents a tree of all key definitions.
 *	This tree is built from the KFUNC list after all initialization is
 *	done to provide faster escape-sequence checking.
 */
typedef struct fkey FKEY;
struct fkey
{
	int		fk_char;				/* next char in escape-sequence		*/
	int		fk_code;				/* key-value if leaf, 0 if branch	*/

	FKEY *	fk_sib;					/* branch: ptr to next seq entry	*/
	FKEY *	fk_child;				/* branch: ptr to next char in seq	*/
	FKEY *	fk_parent;				/* pointer to parent entry			*/

	FKEY *	fk_prev;				/* prev entry (makes a bkwd list)	*/
};

/*------------------------------------------------------------------------
 * TERM_INTS struct
 *
 *	These are all numeric entries from term-databases & term-files.
 */
struct term_ints
{
	/*--------------------------------------------------------------------
	 * screen geometry
	 */
	int		maxrows;				/* number of lines					*/
	int		maxcols;				/* number of columns				*/
	int		tabsize;				/* width of a tab					*/

	/*--------------------------------------------------------------------
	 * mouse info
	 */
	int		nb;						/* num of mouse buttons				*/
	int		dblclk_intvl;			/* mouse double-click interval		*/

	/*--------------------------------------------------------------------
	 * function-key info
	 */
	int		prefix_intvl;			/* interval for esc seq processing	*/

	/*--------------------------------------------------------------------
	 * color info
	 */
	int		mc;						/* max number of colors				*/
	int		max_fg;					/* max fg color						*/
	int		max_bg;					/* max bg color						*/
	int		color_fg_map[NUM_COLORS];	/* color fg map table			*/
	int		color_bg_map[NUM_COLORS];	/* color bg map table			*/
};
typedef struct term_ints TERM_INTS;

/*------------------------------------------------------------------------
 * TERM_BOOLS struct
 *
 *	These are all boolean entries from term-databases & term-files.
 */
struct term_bools
{
	int		am;						/* automatic margins	*/
	int		alc;					/* allow colors			*/
	int		md;						/* display mouse 		*/
	int		ifc;					/* insert final char	*/
	int		ala;					/* allow alt chars		*/
	int		scn;					/* use scancodes		*/
};
typedef struct term_bools TERM_BOOLS;

/*------------------------------------------------------------------------
 * TERM_PARMS struct
 *
 *	These are all parameterized entries from term-databases & term-files.
 */
struct term_parms
{
	/*--------------------------------------------------------------------
	 * CM strings
	 */
	char *	cm;						/* cursor move			*/
	char *	cm_tc;					/* termcap  cm			*/
	char *	cm_ti;					/* terminfo cm			*/

	/*--------------------------------------------------------------------
	 * CS strings
	 */
	char *	csr;					/* reg cs				*/
	char *	csr_tc;					/* reg termcap  cs		*/
	char *	csr_ti;					/* reg terminfo cs		*/

	char *	csm;					/* map cs				*/
	char *	csm_tc;					/* map termcap  cs		*/
	char *	csm_ti;					/* map terminfo cs		*/

	/*--------------------------------------------------------------------
	 * FG strings
	 */
	char *	fgr;					/* reg fg				*/
	char *	fgr_tc;					/* reg termcap  fg		*/
	char *	fgr_ti;					/* reg terminfo fg		*/

	char *	fgm;					/* map fg				*/
	char *	fgm_tc;					/* map termcap  fg		*/
	char *	fgm_ti;					/* map terminfo fg		*/

	/*--------------------------------------------------------------------
	 * BG strings
	 */
	char *	bgr;					/* reg bg				*/
	char *	bgr_tc;					/* reg termcap  bg		*/
	char *	bgr_ti;					/* reg terminfo bg		*/

	char *	bgm;					/* map bg				*/
	char *	bgm_tc;					/* map termcap  bg		*/
	char *	bgm_ti;					/* map terminfo bg		*/
};
typedef struct term_parms TERM_PARMS;

/*------------------------------------------------------------------------
 * TERM_STRS struct
 *
 *	These are all string entries from term-databases & term-files.
 */
struct term_strs
{
	/*--------------------------------------------------------------------
	 * insert/delete lines
	 */
	char *	al;						/* add line				*/
	char *	dl;						/* delete line			*/

	/*--------------------------------------------------------------------
	 * insert/delete chars
	 */
	char *	ic;						/* insert character		*/
	char *	dc;						/* delete character		*/

	/*--------------------------------------------------------------------
	 * bells
	 */
	char *	bl;						/* bell					*/
	char *	vb;						/* visible bell			*/

	/*--------------------------------------------------------------------
	 * clear cmds
	 */
	char *	cl;						/* clear screen			*/
	char *	ce;						/* clear to EOL			*/
	char *	cd;						/* clear to EOS			*/

	/*--------------------------------------------------------------------
	 * cursor display
	 */
	char *	vi;						/* cursor invisible		*/
	char *	ve;						/* cursor visible		*/
	char *	vs;						/* cursor very visible	*/

	/*--------------------------------------------------------------------
	 * session init/end
	 */
	char *	ti;						/* terminal init		*/
	char *	te;						/* terminal end			*/

	/*--------------------------------------------------------------------
	 * window title
	 */
	char *	tts;					/* title begin			*/
	char *	tte;					/* title end			*/

	/*--------------------------------------------------------------------
	 * iconification on/off
	 */
	char *	icc;					/* icon close			*/
	char *	ico;					/* icon open			*/

	char *	icbs;					/* beg icon bits		*/
	char *	icbe;					/* end icon bits		*/

	/*--------------------------------------------------------------------
	 * alt-chars
	 */
	char *	ace;					/* alt-chars enable		*/
	char *	acd;					/* alt-chars disable	*/

	/*--------------------------------------------------------------------
	 * keyboard mapping on/off
	 */
	char *	kbo;					/* keyboard map on		*/
	char *	kbf;					/* keyboard map off		*/

	/*--------------------------------------------------------------------
	 * printer on/off
	 */
	char *	pro;					/* printer on			*/
	char *	prf;					/* printer off			*/

	/*--------------------------------------------------------------------
	 * attributes on/off
	 */
	char *	sts;					/* beg standout			*/
	char *	ste;					/* end standout			*/

	char *	uls;					/* beg underline		*/
	char *	ule;					/* end underline		*/

	char *	bds;					/* beg bold				*/
	char *	bde;					/* end bold				*/

	char *	bls;					/* beg blink			*/
	char *	ble;					/* end blink			*/

	char *	dms;					/* beg dim				*/
	char *	dme;					/* end dim				*/

	char *	ivs;					/* beg invisible		*/
	char *	ive;					/* end invisible		*/

	char *	prs;					/* beg protected		*/
	char *	pre;					/* end protected		*/

	/*--------------------------------------------------------------------
	 * reg fonts on/off
	 */
	char *	rns[NUM_FONTS];			/* beg reg char set n	*/
	char *	rne[NUM_FONTS];			/* end reg char set n	*/

	/*--------------------------------------------------------------------
	 * alt fonts on/off
	 */
	char *	ans[NUM_FONTS];			/* beg alt char set n	*/
	char *	ane[NUM_FONTS];			/* end alt char set n	*/

	/*--------------------------------------------------------------------
	 * mouse on/off/input
	 */
	char *	mbeg;					/* mouse on				*/
	char *	mend;					/* mouse off			*/
	char *	mstr;					/* mouse input string	*/

	/*--------------------------------------------------------------------
	 * miscellaneous
	 */
	char *	snap;					/* screen snap taken	*/
	char *	desc;					/* terminal description	*/
	char *	acs;					/* ACS char string		*/

	/*--------------------------------------------------------------------
	 * colors
	 */
	char *	clr_fg[NUM_COLORS];		/* fg color table		*/
	char *	clr_bg[NUM_COLORS];		/* bg color table		*/
};
typedef struct term_strs TERM_STRS;

/*------------------------------------------------------------------------
 * parameterized data
 */
struct parm_entry
{
	SCRN_CMD	cmd;				/* command name			*/
	SCRN_CMD	use;				/* cmd being used		*/
	int			type;				/* database type		*/
	int			data;				/* str-specific data	*/
	char *		str;				/* actual string to use	*/
};
typedef struct parm_entry PARM_ENTRY;

struct parm_data
{
	/*--------------------------------------------------------------------
	 * cursor movement entries
	 */
	PARM_ENTRY	cm;					/* cm entry				*/

	/*--------------------------------------------------------------------
	 * color string entries
	 */
	PARM_ENTRY	cs;					/* cs entry				*/
	PARM_ENTRY	fg;					/* fg entry				*/
	PARM_ENTRY	bg;					/* bg entry				*/
};
typedef struct parm_data PARM_DATA;

/*------------------------------------------------------------------------
 * TCAP_DATA struct
 */
struct tcap_data
{
	TCAP_ERRS *		errs;					/* pointer to msg struct	*/
	const char **	defs;					/* ptr to def list			*/

	char *			tty_type;				/* terminal name			*/
	int				db_type;				/* database type			*/

	TERM_INTS		ints;					/* term int    values		*/
	TERM_BOOLS		bools;					/* term bool   values		*/
	TERM_PARMS		parms;					/* term parm   values		*/
	TERM_STRS		strs;					/* term string values		*/

	PARM_DATA		pdata;					/* parameterized entries	*/

	unsigned char	inp_tbl[256];			/* input  xlate tbl			*/
	unsigned char	reg_tbl[256];			/* output xlate tbl - reg	*/
	unsigned char	alt_tbl[256];			/* output xlate tbl - alt	*/

	unsigned char	reg_font_tbl[256];		/* output font tbl - reg	*/
	unsigned char	alt_font_tbl[256];		/* output font tbl - alt	*/

	attr_t			clr_pairs[COLOR_PAIRS];	/* color pairs				*/

	FKEY *			fks_start;				/* start of fk tree			*/
	FKEY *			fks_last;				/* ptr to last fk tree node	*/

	KFUNC *			kfuncs;					/* function key list		*/

	char			db_buf[TCAP_BUFLEN];	/* database buffer			*/
};
typedef struct tcap_data TCAP_DATA;

extern const TCAP_DATA tcap_def_tcap_data;

/*------------------------------------------------------------------------
 * functions
 */
extern TCAP_DATA *	tcap_init_tcap_data	(void);
extern void			tcap_free_tcap_data	(TCAP_DATA *td);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCAPDATA_H */
