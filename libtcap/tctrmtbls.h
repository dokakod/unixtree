/*------------------------------------------------------------------------
 *	This header file describes the structures which are intended
 *	to be private to this library.
 *
 * This header is private to this library only.
 */
#ifndef TCTRMTBLS_H
#define TCTRMTBLS_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * key function table
 */
struct kf_table
{
	const char *	kf_tcname;		/* termcap  name		*/
	const char *	kf_tiname;		/* terminfo name		*/
	const char *	kf_str;			/* string definition	*/
	int				kf_code;		/* key value			*/
};
typedef struct kf_table KF_TABLE;

/*------------------------------------------------------------------------
 * misc term-file keywords
 */
extern const char *	tcap_keyword_term;
extern const char *	tcap_keyword_map_fg;
extern const char *	tcap_keyword_map_bg;
extern const char *	tcap_keyword_map;
extern const char *	tcap_keyword_dbg;
extern const char *	tcap_keyword_key;
extern const char *	tcap_keyword_dbtype;

/*------------------------------------------------------------------------
 * default key-function tables
 */
extern const KF_TABLE tcap_tc_keys[];
extern const KF_TABLE tcap_dumb_keys[];

/*------------------------------------------------------------------------
 * termcap table
 */
extern const TC_DATA	tcap_termcap_data[];

/*------------------------------------------------------------------------
 * VT100 symbols
 */
#define VT100_BLOCK		'0'				/* solid square block */
#define VT100_DIAMOND	'`'				/* diamond */
#define VT100_CKBOARD	'a'				/* checker board (stipple) */
#define VT100_HT		'b'				/* HT - horizontal tab */
#define VT100_FF		'c'				/* FF - form feed */
#define VT100_CR		'd'				/* CR - carriage return */
#define VT100_LF		'e'				/* LF - line feed */
#define VT100_DEGREE	'f'				/* degree symbol */
#define VT100_PLMINUS	'g'				/* plus/minus */
#define VT100_NL		'h'				/* NL - new line */
#define VT100_VT		'i'				/* VT - vertical tab */
#define VT100_LRCORNER	'j'				/* lower right corner */
#define VT100_URCORNER	'k'				/* upper right corner */
#define VT100_ULCORNER	'l'				/* upper left corner */
#define VT100_LLCORNER	'm'				/* lower left corner */
#define VT100_PLUS		'n'				/* plus sign */
#define VT100_S1		'o'				/* scan line 1 */
#define VT100_S3		'p'				/* scan line 3 */
#define VT100_HLINE		'q'				/* horizontal line */
#define VT100_S7		'r'				/* scan line 7 */
#define VT100_S9		's'				/* scan line 9 */
#define VT100_LTEE		't'				/* left tee */
#define VT100_RTEE		'u'				/* right tee */
#define VT100_BTEE		'v'				/* bottom tee */
#define VT100_TTEE		'w'				/* top tee */
#define VT100_VLINE		'x'				/* vertical line */
#define VT100_LE		'y'				/* less than or equal */
#define VT100_GE		'z'				/* greater than or equal */
#define VT100_PI		'{'				/* PI */
#define VT100_NE		'|'				/* not equal */
#define VT100_POUND		'}'				/* English pound sign */
#define VT100_BULLET	'~'				/* bullet */

/*------------------------------------------------------------------------
 * ACS char struct
 *
 *	This table is used to map vt100 graphic chars
 *	to the appropriate PC char set.
 *
 *	Note that entries may appear more than once,
 *	since the PC set is richer.
 */
struct acs_chrs
{
	unsigned char	vt_char;		/* VT100 char code	*/
	unsigned char	pc_char;		/* PC    char code	*/
};
typedef struct acs_chrs ACS_CHRS;

extern const ACS_CHRS	tcap_acs_chars[];

/*------------------------------------------------------------------------
 * char translation struct
 *
 *	This struct maps table names to the appropriate table & font number.
 */
#define FONT_TBL_INP	0
#define FONT_TBL_REG	1
#define FONT_TBL_ALT	2

struct xlate_list
{
	const char *	tbl_name;		/* table entry name	*/
	int				font_type;		/* font type		*/
	int				font_no;		/* font number		*/
};
typedef struct xlate_list XLATE_LIST;

extern const XLATE_LIST	tcap_xlate_list[];

/*------------------------------------------------------------------------
 * font change command struct
 */
struct font_cmd
{
	SCRN_CMD		on;				/* on  command	*/
	SCRN_CMD		off;			/* off command	*/
};
typedef struct font_cmd FONT_CMD;

extern const FONT_CMD	tcap_reg_font_cmds[];
extern const FONT_CMD	tcap_alt_font_cmds[];

/*------------------------------------------------------------------------
 * parameter info
 */
struct param_info
{
	SCRN_CMD		cmd;			/* cmd id					*/
	SCRN_CMD		use;			/* cmd id to use			*/
	int				data;			/* data value for this cmd	*/
	int				need;			/* needs database entry		*/
	int				offs;			/* offset into parm-data	*/
	int				type;			/* database type			*/
};
typedef struct param_info PARAM_INFO;

extern const PARAM_INFO tcap_param_info[];

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCTRMTBLS_H */
