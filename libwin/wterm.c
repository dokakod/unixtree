/*------------------------------------------------------------------------
 * terminfo/termcap database interface routines
 */
#include "wincommon.h"
#include "term.h"
#include "termcap.h"
#include "wtcinfo.h"

/*------------------------------------------------------------------------
 * user-supplied output routine
 */
typedef int PUTRTN (int c);

/*------------------------------------------------------------------------
 * static data
 */
static const char *	bufptr		= 0;	/* cached buffer  pointer		*/
static int			dbtype		= -1;	/* cachec database type			*/
static PUTRTN *		outrtn		= 0;	/* cached routine pointer		*/

static char			strbuf[128]	= "";	/* cached string return buffer	*/

/*------------------------------------------------------------------------
 * tputs() - output a parameterized string
 */
static int tputs_int (TERMINAL *sp, int c)
{
	return (*outrtn)(c);
}

int tputs (const char *str, int affcnt, PUTRTN *prtn)
{
	if (prtn == 0)
		prtn = putchar;

	if (str == 0 || *str == 0)
		return (OK);

	tcap_outflush(win_cur_term);

	outrtn = prtn;
	return tcap_db_outs(win_cur_term, str, tputs_int);
}

/*------------------------------------------------------------------------
 * vidputs() - output attributes to the screen
 */
int vidputs (attr_t attrs, PUTRTN *prtn)
{
	char	attr_buf[BUFSIZ];

	if (win_cur_term == 0)
		return (ERR);

	if (attrs == A_UNSET)
		return (ERR);

	tcap_attrbuf(win_cur_term, attrs, ' ', attr_buf);
	return tputs(attr_buf, 1, prtn);
}

/*------------------------------------------------------------------------
 * vidattr() - output an attribute string
 */
int vidattr (attr_t attrs)
{
	return vidputs(attrs, 0);
}

/*------------------------------------------------------------------------
 * putp() - output a parameterized string
 */
int putp (const char *str)
{
	return tputs(str, 1, 0);
}

/*------------------------------------------------------------------------
 * tigetent() - terminfo-style load of database
 */
int tigetent (char *bp, const char *term)
{
	int rc;

	if (bp == 0)
		return (ERR);

	rc = tcap_db_getent(TCAP_DB_TERMINFO, bp, term);
	if (rc)
		return (ERR);

	bufptr = bp;
	dbtype = TCAP_DB_TERMINFO;
	return (OK);
}

/*------------------------------------------------------------------------
 * tigetflag() - terminfo-style get of boolean value
 */
int tigetflag (const char *id)
{
	int	n;

	n = tc_id_flag(id);
	if (n < 0)
		return (ERR);

	return tcgetflag(n);
}

/*------------------------------------------------------------------------
 * tigetnum() - terminfo-style get of numeric value
 */
int tigetnum (const char *id)
{
	int	n;

	n = tc_id_num(id);
	if (n < 0)
		return (ERR);

	return tcgetnum(n);
}

/*------------------------------------------------------------------------
 * tigetstr() - terminfo-style get of string value
 */
const char * tigetstr (const char *id)
{
	int	n;

	n = tc_id_str(id);
	if (n < 0)
		return (0);

	return tcgetstr(n);
}

/*------------------------------------------------------------------------
 * tparm() - terminfo-style evaluate of parameterized string
 */
char * tparm (const char *str, ...)
{
	va_list	args;
	int		p1;
	int		p2;

	va_start(args, str);
	p1 = va_arg(args, int);
	p2 = va_arg(args, int);
	va_end(args);

	return tcap_db_goto(TCAP_DB_TERMINFO, strbuf, str, p1, p2);
}

/*------------------------------------------------------------------------
 * tgetent() - termcap-style load of database info
 */
int tgetent (char *bp, const char *term)
{
	int rc;

	if (bp == 0)
		return (ERR);

	rc = tcap_db_getent(TCAP_DB_TERMCAP, bp, term);
	if (rc)
		return (ERR);

	bufptr = bp;
	dbtype = TCAP_DB_TERMCAP;
	return (OK);
}

/*------------------------------------------------------------------------
 * tgetflag() - termcap-style get of boolean value
 */
int tgetflag (const char *id)
{
	int	n;

	n = tc_id_flag(id);
	if (n < 0)
		return (ERR);

	return tcgetflag(n);
}

/*------------------------------------------------------------------------
 * tgetnum() - termcap-style get of numeric value
 */
int tgetnum (const char *id)
{
	int	n;

	n = tc_id_num(id);
	if (n < 0)
		return (ERR);

	return tcgetnum(n);
}

/*------------------------------------------------------------------------
 * tgetstr() - termcap-style get of string value
 */
const char * tgetstr (const char *id, char **area)
{
	const char *p;
	int			n;

	n = tc_id_str(id);
	if (n < 0)
		return (0);

	p = tcgetstr(n);
	if (p == 0 || p == (char *)-1)
		return (p);

	if (area != 0 && *area != 0)
	{
		p = strcpy(*area, p);
		*area += (strlen(p) + 1);
	}

	return (p);
}

/*------------------------------------------------------------------------
 * tgoto() - termcap-style evaluate of parameterized string
 */
char * tgoto (const char *str, int col, int row)
{
	return tcap_db_goto(TCAP_DB_TERMCAP, strbuf, str, row, col);
}

/*------------------------------------------------------------------------
 * tcdumpent() - dump a database
 */
void tcdumpent (FILE *fp, const char *term)
{
	const char *	bp	= bufptr;
	int				db	= dbtype;

	if (bp == 0)
	{
		if (win_cur_term == 0)
			return;

		bp = tcap_db_bufptr(win_cur_term);
		db = tcap_db_dbtype(win_cur_term);
	}

	tcap_db_dump(db, bp, term, fp);
}

/*------------------------------------------------------------------------
 * tcget*() - database-independent get routines
 */
int tcgetflag (int id)
{
	const char *	bp	= bufptr;
	int				db	= dbtype;

	if (id < 0 || id >= num_tc_bools)
		return (ERR);

	if (bp == 0)
	{
		if (win_cur_term == 0)
			return (ERR);

		bp = tcap_db_bufptr(win_cur_term);
		db = tcap_db_dbtype(win_cur_term);
	}

	if (db == TCAP_DB_TERMINFO)
		return tcap_db_getflag(TCAP_DB_TERMINFO, bp, tc_bools[id].ti_name);
	else
		return tcap_db_getflag(TCAP_DB_TERMCAP,  bp, tc_bools[id].tc_name);
}

int tcgetnum (int id)
{
	const char *	bp	= bufptr;
	int				db	= dbtype;

	if (id < 0 || id >= num_tc_nums)
		return (ERR);

	if (bp == 0)
	{
		if (win_cur_term == 0)
			return (ERR);

		bp = tcap_db_bufptr(win_cur_term);
		db = tcap_db_dbtype(win_cur_term);
	}

	if (db == TCAP_DB_TERMINFO)
		return tcap_db_getnum(TCAP_DB_TERMINFO, bp, tc_nums[id].ti_name);
	else
		return tcap_db_getnum(TCAP_DB_TERMCAP,  bp, tc_nums[id].tc_name);
}

const char * tcgetstr (int id)
{
	const char *	bp	= bufptr;
	int				db	= dbtype;

	if (id < 0 || id >= num_tc_strs)
		return (0);

	if (bp == 0)
	{
		if (win_cur_term == 0)
			return (0);

		bp = tcap_db_bufptr(win_cur_term);
		db = tcap_db_dbtype(win_cur_term);
	}

	if (db == TCAP_DB_TERMINFO)
		return tcap_db_getstr(TCAP_DB_TERMINFO, bp, tc_strs[id].ti_name);
	else
		return tcap_db_getstr(TCAP_DB_TERMCAP,  bp, tc_strs[id].tc_name);
}

/*------------------------------------------------------------------------
 * tcparm() - parameterize an entry
 */
char * tcparm (char *buf, const char *str, ...)
{
	va_list	args;
	int		p1;
	int		p2;
	int		db	= dbtype;

	if (dbtype == -1)
	{
		if (win_cur_term == 0)
		{
			*buf = 0;
			return (buf);
		}

		dbtype = tcap_db_dbtype(win_cur_term);
	}

	va_start(args, str);
	p1 = va_arg(args, int);
	p2 = va_arg(args, int);
	va_end(args);

	if (dbtype == TCAP_DB_TERMINFO)
		return tcap_db_goto(TCAP_DB_TERMINFO, buf, str, p1, p2);
	else
		return tcap_db_goto(TCAP_DB_TERMCAP,  buf, str, p2, p1);
}
