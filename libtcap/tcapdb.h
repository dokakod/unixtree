/*------------------------------------------------------------------------
 * header for term database routines
 *
 * This header is private to this library only.
 */
#ifndef TCAPDB_H
#define TCAPDB_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * database types
 */

/* pseudo entries used in load/dump calls only */

#define TCAP_DB_AUTO		0		/* auto-detect			*/
#define TCAP_DB_TERMFILES	1		/* no database			*/

/* entries stored in TCAP_DATA struct */

#define TCAP_DB_TERMINFO	2		/* use terminfo			*/
#define TCAP_DB_TERMCAP		3		/* use termcap			*/
#define TCAP_DB_WINDOWS		4		/* windows				*/

#define TCAP_DB_NONE		-1		/* not defined			*/

/*------------------------------------------------------------------------
 * pseudo-term name for windows
 */
#define TCAP_TERM_WINDOWS	"windows"

/*------------------------------------------------------------------------
 * generic functions
 */
extern int			tcap_db_name_to_val	(const char *name);

extern const char *	tcap_db_num_to_name	(int num);
extern const char *	tcap_db_num_to_desc	(int num);

extern const char *	tcap_db_val_to_name	(int type);
extern const char *	tcap_db_val_to_desc	(int type);

extern int			tcap_db_set_type	(int db_type);
extern int			tcap_db_get_type	(void);

/*------------------------------------------------------------------------
 * specific functions
 */
extern int			tcap_db_load_term	(int db_type, TERMINAL *tp,
											const char *term);

extern int			tcap_db_dump_term	(int db_type, const char *term,
											FILE *fp);

extern char *		tcap_db_calc		(int db_type, char *buf,
											const char *s, int p1, int p2);

extern int			tcap_db_outs		(TERMINAL *tp, const char *str,
										int (*outrtn)(TERMINAL *tp, int c));

/*------------------------------------------------------------------------
 * low-level stuff
 */
extern char *		tcap_db_bufptr		(TERMINAL *tp);
extern int			tcap_db_dbtype		(TERMINAL *tp);

extern int			tcap_db_getent		(int db_type, char *bp,
											const char *term);
extern int			tcap_db_getflag		(int db_type, const char *bp,
											const char *id);
extern int			tcap_db_getnum		(int db_type, const char *bp,
											const char *id);
extern const char *	tcap_db_getstr		(int db_type, const char *bp,
											const char *id);
extern char *		tcap_db_goto		(int db_type, char *buf,
											const char *str, int p1, int p2);

extern void			tcap_db_dump		(int db_type, const char *bp,
											const char *term, FILE *fp);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCAPDB_H */
