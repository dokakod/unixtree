/*------------------------------------------------------------------------
 * X Toolkit options
 */
#ifndef XVTTK_H
#define XVTTK_H

/*------------------------------------------------------------------------
 * option flags
 *
 * These flags are used in generating the help display and in generating
 * the toolkit options dump.
 */
#define TK_FLAG_NOTHERE		0x0001		/* not valid in "here" mode		*/
#define TK_FLAG_NOTTASK		0x0002		/* not valid in "task" mode		*/
#define TK_FLAG_APPOVR		0x0004		/* "appname" overrides default	*/
#define TK_FLAG_ENVNAME		0x0008		/* default is env name (with $)	*/
#define TK_FLAG_PHONYDFLT	0x0010		/* default is phony				*/
#define TK_FLAG_NOHELP_0	0x0020		/* don't show in help level 0	*/
#define TK_FLAG_NOHELP_1	0x0040		/* don't show in help level 0-1	*/
#define TK_FLAG_NOTINHELP	0x0080		/* don't show in help (testing)	*/

/*------------------------------------------------------------------------
 * option types
 */
#define XVT_TK_OPT_ERR		-1		/* error occurred	*/
#define XVT_TK_OPT_INV		0		/* invalid option	*/
#define XVT_TK_OPT_STR		1		/* string  option	*/
#define XVT_TK_OPT_CLR		2		/* color   option	*/
#define XVT_TK_OPT_FNT		3		/* font    option	*/
#define XVT_TK_OPT_NUM		4		/* numeric option	*/
#define XVT_TK_OPT_BLN		5		/* boolean option	*/
#define XVT_TK_OPT_DBG		6		/* debug   option	*/
#define XVT_TK_OPT_CMD		7		/* cmd list follows	*/

/*------------------------------------------------------------------------
 * toolkit option struct
 */
struct tk_options
{
	int				opt_type;			/* option type					*/
	int				opt_offset;			/* var offset in struct	or -1	*/
	int				opt_flags;			/* option flags					*/
	const char *	opt_string;			/* option string to look for	*/
	const char *	opt_help;			/* help msg						*/
	const char *	opt_desc;			/* description					*/
	const char *	opt_resname;		/* resource name (may be NULL)	*/
	const char *	opt_default;		/* default value (may be NULL)	*/
										/* Note: for bool & num entries	*/
										/* this is the actual value		*/
	const char **	opt_lines;			/* extra help    (may be NULL)	*/
};
typedef struct tk_options	TK_OPTIONS;

extern const TK_OPTIONS	xvt_tk_options[];

/*------------------------------------------------------------------------
 * functions
 */
extern const TK_OPTIONS *	xvt_tk_find_by_option	(const char *option);
extern const TK_OPTIONS *	xvt_tk_find_by_resname	(const char *resname);

extern int					xvt_tk_opt_set			(XVT_DATA *xd,
													const char *option,
													const char *arg,
													int resent,
													int *type);

extern char *				xvt_tk_data_value		(XVT_DATA *xd,
													const TK_OPTIONS *op,
													int use_dflt,
													char *buf);

extern void					xvt_tk_data_init		(XVT_DATA *xd,
													int set_dflt);

#endif /* XVTTK_H */
