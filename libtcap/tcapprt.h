/*------------------------------------------------------------------------
 *	Printer header file
 */
#ifndef TCAPPRT_H
#define TCAPPRT_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * function prototypes
 */
extern PRINTER *	tcap_prt_open		(TERMINAL *tp,
											int mode, const char *str,
											char *msgbuf);

extern int			tcap_prt_close		(TERMINAL *tp,
											PRINTER *p, char *msgbuf);

extern int			tcap_prt_output_str	(TERMINAL *tp,
											PRINTER *p, const char *str);
extern int			tcap_prt_output_fmt	(TERMINAL *tp,
											PRINTER *p, const char *fmt, ...);
extern int			tcap_prt_output_var	(TERMINAL *tp,
											PRINTER *p, const char *fmt,
											va_list args);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif	/* TCAPPRT_H */
