/*------------------------------------------------------------------------
 * error message stuff
 */
#ifndef TCERRMSG_H
#define TCERRMSG_H

#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * tcap error codes
 */
#define TCAP_ERR_NONE			0			/* no error					*/
#define TCAP_ERR_NOMEM			1			/* no memory				*/
#define TCAP_ERR_NOTERM			2			/* no term variable			*/
#define TCAP_ERR_CANTOPEN		3			/* can't open term/file		*/
#define TCAP_ERR_NODBS			4			/* database not specified	*/
#define TCAP_ERR_NOTFND			5			/* no entry found for TERM	*/

/* missing variables */

#define TCAP_ERR_NOCM			10			/* no CM string found		*/
#define TCAP_ERR_NOLINES		11			/* no LINES found			*/
#define TCAP_ERR_NOCOLS			12			/* no COLS found			*/
#define TCAP_ERR_NOFG			13			/* missing FG entry			*/
#define TCAP_ERR_NOBG			14			/* missing BG entry			*/

/* function-key codes */

#define TCAP_ERR_KEY_DUP		20			/* dup key definition		*/
#define TCAP_ERR_KEY_PART		21			/* partial string in def	*/
#define TCAP_ERR_KEY_CONT		22			/* key contains other key	*/

/* term-file codes */

#define TCAP_ERR_UNKNOWN		30			/* unknown term-file entry	*/
#define TCAP_ERR_NOTERMFILE		31			/* no term-file found		*/
#define TCAP_ERR_CYCLIC			32			/* cyclic TERM references	*/
#define TCAP_ERR_INV_SYNTAX		33			/* invalid syntax  in entry	*/
#define TCAP_ERR_INV_KEYWRD		34			/* invalid keyword in entry	*/
#define TCAP_ERR_INV_DATA		35			/* invalid data    in entry	*/

/*------------------------------------------------------------------------
 * tcap error levels
 */
#define TCAP_ERR_L_NONE			0
#define TCAP_ERR_L_INFO			1
#define TCAP_ERR_L_WARNING		2
#define TCAP_ERR_L_FATAL		3

/*------------------------------------------------------------------------
 * error msg entry
 */
typedef struct tcap_errmsg TCAP_ERRMSG;
struct tcap_errmsg
{
	TCAP_ERRMSG *	next;					/* pointer to next entry	*/

	int				code;					/* error code				*/
	int				level;					/* error level				*/
	char			msg[128];				/* message					*/
};

/*------------------------------------------------------------------------
 * error msg struct
 */
typedef struct tcap_errs TCAP_ERRS;
struct tcap_errs
{
	TCAP_ERRMSG *	msgs;					/* pointer to msg list		*/
	int				num;					/* number of msgs in list	*/
	int				max_lvl;				/* highest msg level		*/
};

/*------------------------------------------------------------------------
 * functions
 */
extern void		tcap_errmsg_init		(TCAP_ERRS *te);
extern void		tcap_errmsg_clear		(TCAP_ERRS *te);

extern void		tcap_errmsg_add			(TCAP_ERRS *te, TCAP_ERRMSG *tm);
extern void		tcap_errmsg_add_fmt		(TCAP_ERRS *te, int code, int level,
											const char *fmt, ...);

extern void		tcap_errmsg_add_var		(TCAP_ERRS *te, int code, int level,
											const char *fmt, va_list args);

extern void		tcap_errmsg_print_list	(const TCAP_ERRS   *te,
											FILE *fp, int detail);
extern void		tcap_errmsg_print_entry	(const TCAP_ERRMSG *tm,
											FILE *fp, int detail);

extern const char *	tcap_errmsg_name	(int code);
extern const char *	tcap_errmsg_desc	(int code);

#ifdef __cplusplus
}
#endif

#endif /* TCERRMSG_H */
