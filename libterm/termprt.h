/*------------------------------------------------------------------------
 *	Printer header file
 */
#ifndef TERMPRT_H
#define TERMPRT_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 *	PRINTER struct definition
 */
struct printer
{
	int			prt_mode;		/* printer mode				*/
	int			prt_pid;		/* PID for printer pipes	*/
	FILE *		prt_fp;			/* pointer to output stream	*/
	char		prt_str[256];	/* cached string			*/
};
typedef struct printer PRINTER;

#define PRINTER_T	TRUE		/* PRINTER struct defined	*/

/*------------------------------------------------------------------------
 *	values for prt_mode
 */
#define P_SPOOL			0		/* pipe to system spooler	*/
#define P_PIPE			1		/* pipe to specified cmd	*/
#define P_FILE			2		/* output to file			*/
#define P_NONE			3		/* no output				*/

/*------------------------------------------------------------------------
 * function prototypes
 */
extern PRINTER *	term_prt_open		(int mode, const char *str,
											char *msgbuf);

extern PRINTER *	term_prt_open_spool	(const char *dest, char *msgbuf);
extern PRINTER *	term_prt_open_pipe	(const char *cmd,  char *msgbuf);
extern PRINTER *	term_prt_open_file	(const char *file, char *msgbuf);
extern PRINTER *	term_prt_open_none	(const char *str,  char *msgbuf);

extern int			term_prt_close		(PRINTER *p, char *msgbuf);

extern int			term_prt_output_str	(PRINTER *p,
											const char *str);
extern int			term_prt_output_fmt	(PRINTER *p,
											const char *fmt, ...);
extern int			term_prt_output_var	(PRINTER *p,
											const char *fmt, va_list args);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif	/* TERMPRT_H */
