/*------------------------------------------------------------------------
 * printer interface
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * tcap_prt_open() - open a printer
 */
PRINTER * tcap_prt_open (TERMINAL *tp,
	int mode, const char *str, char *msgbuf)
{
	if (mode == P_NONE)
	{
		TCAP_DATA *	td;

		if (tp == 0)
		{
			sprintf(msgbuf, "NULL terminal pointer");
			return (0);
		}

		td = tp->tcap;
		if (! is_cmd_pres(td->strs.pro) ||
		    ! is_cmd_pres(td->strs.prf))
		{
			sprintf(msgbuf, "No printer strings defined");
			return (0);
		}
	}

	return term_prt_open(mode, str, msgbuf);
}

/*------------------------------------------------------------------------
 * tcap_prt_close() - close a printer
 */
int tcap_prt_close (TERMINAL *tp,
	PRINTER *p, char *msgbuf)
{
	return term_prt_close(p, msgbuf);
}

/*------------------------------------------------------------------------
 * tcap_prt_output_str() - output a string to a printer
 */
int tcap_prt_output_str (TERMINAL *tp,
	PRINTER *p, const char *str)
{
	if (p == 0 || str == 0)
		return (-1);

	if (*str == 0)
		return (0);

	if (p->prt_mode == P_NONE)
	{
		TCAP_DATA *		td;
		const char *	s;

		if (tp == 0)
			return (-1);

		TERMINAL_LOCK(tp);
		{
			td = tp->tcap;
			tcap_outcmd(tp, S_PRO, td->strs.pro);
			for (s=str; *s; s++)
				tcap_outch(tp, *s, FALSE);
			tcap_outcmd(tp, S_PRO, td->strs.prf);
			tcap_outflush(tp);
		}
		TERMINAL_UNLOCK(tp);

		return (0);
	}

	return term_prt_output_str(p, str);
}

/*------------------------------------------------------------------------
 * tcap_prt_output_var() - output a variadic string to a printer
 */
int tcap_prt_output_var (TERMINAL *tp,
	PRINTER *p, const char *fmt, va_list args)
{
	char	buffer[BUFSIZ];

	if (p == 0 || fmt == 0)
		return (-1);

	vsprintf(buffer, fmt, args);
	return tcap_prt_output_str(tp, p, buffer);
}

/*------------------------------------------------------------------------
 * tcap_prt_output_fmt() - output a formatted string to a printer
 */
int tcap_prt_output_fmt (TERMINAL *tp,
	PRINTER *p, const char *fmt, ...)
{
	va_list args;
	int		rc;

	va_start(args, fmt);
	rc = tcap_prt_output_var(tp, p, fmt, args);
	va_end(args);

	return (rc);
}
