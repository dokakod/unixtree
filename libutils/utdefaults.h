/*------------------------------------------------------------------------
 *	Defaults definition header file
 */
#ifndef UTDEFAULTS_H
#define UTDEFAULTS_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * list of option entries
 */
#define OPT_OPT		0			/* option entry		*/
#define	OPT_DEF		1			/* default value	*/
#define OPT_SAV		2			/* save slot		*/

/*------------------------------------------------------------------------
 * option structs
 */
struct opt_bool
{
	int		opts[3];
};
typedef struct opt_bool OPT_BOOL;

struct opt_int
{
	int		opts[3];
};
typedef struct opt_int OPT_INT;

struct opt_attr
{
	attr_t	opts[3];
};
typedef struct opt_attr OPT_ATTR;

struct opt_str
{
	char	opts[3][MAX_PATHLEN];
};
typedef struct opt_str OPT_STR;

/*------------------------------------------------------------------------
 * list of entry types
 */
#define DEF_BOOL	1			/* OPT_BOOL	boolean value	*/
#define DEF_NUM		2			/* OPT_INT	numeric value	*/
#define DEF_CHAR	3			/* OPT_INT	single char		*/
#define DEF_ENUM	4			/* OPT_INT	enum list		*/
#define DEF_LIST	5			/* OPT_INT	BLIST index		*/
#define DEF_COLOR	6			/* OPT_ATTR	color attribute	*/
#define DEF_MONO	7			/* OPT_ATTR	mono attribute	*/
#define DEF_STR		8			/* OPT_STR	string value	*/
#define DEF_TITLE	9			/* n/a		title entry		*/

/*------------------------------------------------------------------------
 * routine to get a message
 */
typedef const char *	DEF_MSG		(int n);

/*------------------------------------------------------------------------
 * var changed call-back routine
 *
 * returns:
 *		<  0	don't use new value
 *		>= 0	use the new value
 *				If entry type is DEF_LIST, returned value is new value
 *				to set.
 */
typedef int				DEF_RTN		(const void *valp);

/*------------------------------------------------------------------------
 * default entry struct
 */
struct def_struct
{
	int			type;		/* one of the above entry types				*/

	int			f_str;		/* msg # of char string in config file		*/
	int			m_str;		/* msg # of menu display line				*/
	int			p_str;		/* msg # of prompt string					*/
	int			v_str;		/* msg # of default value string			*/

	const void *limit;		/* value depends on type as follows:		*/
							/* DEF_TITLE	n/a							*/
							/* DEF_BOOL		n/a							*/
							/* DEF_NUM		ptr to max value.			*/
							/*				A max of 0 -> no limit.		*/
							/* DEF_CHAR		n/a							*/
							/* DEF_ENUM		ptr to array of DEFCs.		*/
							/*              Value stored is index.		*/
							/* DEF_LIST		ptr to list of strings.		*/
							/*              Value stored is index.		*/
							/* DEF_COLOR	n/a							*/
							/* DEF_MONO		n/a							*/
							/* DEF_STR		ptr to max len of string.	*/

	int			opt_off;	/* offset in data array to opts entry		*/
	DEF_RTN *	rtn;		/* routine to call if value changes			*/
};
typedef struct def_struct DEFS;

/*------------------------------------------------------------------------
 * macro to access base of options struct
 */
#define DFLT_BASE(tbl)		( (char *)(*tbl->tbl_ptr) + tbl->tbl_off )

/*------------------------------------------------------------------------
 * macro to access options struct
 */
#define DFLT_OPTS(tbl,ds)	( (void *)(DFLT_BASE(tbl) + ds->opt_off) )

/*------------------------------------------------------------------------
 * macro to access DEF_LIST list
 */
#define DFLT_LPTR(tbl,ds)	(*(BLIST **)((char *)(*tbl->tbl_ptr) + \
								(int)ds->limit))

/*------------------------------------------------------------------------
 * DEF_ENUM list entry (pointed to by DEFS->limit)
 */
struct def_choice
{
	int		f_str;			/* msg # of char string in config file		*/
	int		m_str;			/* msg # of menu display line				*/
};
typedef struct def_choice DEFC;

/*------------------------------------------------------------------------
 * default table
 */
struct def_table
{
	DEF_MSG *		msg_rtn;	/* routine to get a message				*/
	const DEFC *	yes_tbl;	/* list of yes entries					*/
	const DEFC *	nos_tbl;	/* list of no  entries					*/
	const DEFC *	fg_names;	/* list of color fg names				*/
	const DEFC *	bg_names;	/* list of color bg names				*/
	const DEFC *	attr_names;	/* atttribute names						*/
	const DEFS *	def_tbl;	/* list of default entries				*/
	void **			tbl_ptr;	/* ptr to ptr to struct with opts tbl	*/
	int				tbl_off;	/* offset in struct to options table	*/
};
typedef struct def_table DEFT;

/*------------------------------------------------------------------------
 * function prototypes
 */
extern int			dflt_read				(const DEFT *tbl,
												const char *filename,
												const char *syspath);

extern int			dflt_write				(const DEFT *tbl,
												const char *pathname,
												int all);

extern int			dflt_process_entry		(const DEFT *tbl, const DEFS *ds,
												const char *s);

extern void			dflt_set_opt			(const DEFT *tbl, const DEFS *ds,
												const void *value);
extern void			dflt_set_def			(const DEFT *tbl, const DEFS *ds,
												const void *value, int reset);
extern int			dflt_is_opt_default		(const DEFT *tbl, const DEFS *ds);

extern void			dflt_save_opt_entry		(const DEFT *tbl, const DEFS *ds);
extern void			dflt_save_opts			(const DEFT *tbl);

extern void			dflt_restore_opt_entry	(const DEFT *tbl, const DEFS *ds);
extern void			dflt_restore_opts		(const DEFT *tbl);

extern void			dflt_init_opt_entry		(const DEFT *tbl, const DEFS *ds);
extern void			dflt_init_opts			(const DEFT *tbl);

extern void			dflt_init_def_entry		(const DEFT *tbl, const DEFS *ds,
												int reset);
extern void			dflt_init_defs			(const DEFT *tbl, int reset);

extern void			dflt_format				(const DEFT *tbl, const DEFS *ds,
												int file, char *buf);

extern const DEFS *	dflt_find_entry			(const DEFT *tbl, int opt_off);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* UTDEFAULTS_H */
