/*------------------------------------------------------------------------
 * printer interface
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * prt_open() - open a printer
 */
PRINTER * prt_open (int mode, const char *str, char *msgbuf)
{
	return scrn_prt_open(win_cur_screen, mode, str, msgbuf);
}

/*------------------------------------------------------------------------
 * prt_close() - close a printer
 */
int prt_close (PRINTER *p, char *msgbuf)
{
	return scrn_prt_close(win_cur_screen, p, msgbuf);
}

/*------------------------------------------------------------------------
 * prt_output_str() - output a string to a printer
 */
int prt_output_str (PRINTER *p, const char *str)
{
	return scrn_prt_output_str(win_cur_screen, p, str);
}

/*------------------------------------------------------------------------
 * prt_output_fmt() - output a formatted string to a printer
 */
int prt_output_fmt (PRINTER *p, const char *fmt, ...)
{
	va_list args;
	int		rc;

	va_start(args, fmt);
	rc = scrn_prt_output_var(win_cur_screen, p, fmt, args);
	va_end(args);

	return (rc);
}

/*------------------------------------------------------------------------
 * prt_output_var() - output a variadic string to a printer
 */
int prt_output_var (PRINTER *p, const char *fmt, va_list args)
{
	return scrn_prt_output_var(win_cur_screen, p, fmt, args);
}
