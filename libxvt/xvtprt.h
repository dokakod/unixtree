/*------------------------------------------------------------------------
 *	Printer header file
 */
#ifndef XVTPRT_H
#define XVTPRT_H

/*------------------------------------------------------------------------
 *	PRINTER struct definition
 */
struct printer
{
	FILE *		prt_fp;						/* pointer to output stream		*/
	int			prt_pid;					/* PID for printer pipes		*/
	int			prt_rc;						/* status from pipe				*/
	char		prt_str[XVT_MAX_PATHLEN];	/* cached string (for dbg)		*/
};
typedef struct printer PRINTER;

/*------------------------------------------------------------------------
 *	values for prt_mode
 */
#define P_NONE			0					/* open according to string		*/
#define P_SPOOL			1					/* pipe to system spooler		*/
#define P_PIPE			2					/* pipe to specified cmd		*/
#define P_FILE			3					/* output to file				*/

/*------------------------------------------------------------------------
 * function prototypes
 */
extern PRINTER *	xvt_prt_open		(int mode, const char *str);

extern PRINTER *	xvt_prt_open_spool	(const char *dest);
extern PRINTER *	xvt_prt_open_pipe	(const char *cmd);
extern PRINTER *	xvt_prt_open_file	(const char *filename);

extern int			xvt_prt_close		(PRINTER *p);

extern int			xvt_prt_output_chr	(PRINTER *p, int ch);
extern int			xvt_prt_output_str	(PRINTER *p, const char *str);
extern int			xvt_prt_output_fmt	(PRINTER *p, const char *fmt, ...);
extern int			xvt_prt_output_var	(PRINTER *p, const char *fmt,
											va_list args);
extern int			xvt_prt_flush		(PRINTER *p);

#endif	/* XVTPRT_H */
