/*------------------------------------------------------------------------
 *	Routine to create a termio struct from a char string
 */
#include "xvtcommon.h"
#include "xvttioflgs.h"

/*------------------------------------------------------------------------
 * return codes from xvt_tio_add_field
 */
#define ADD_OK		0
#define ADD_BAD		-1
#define ADD_MORE	1

/*------------------------------------------------------------------------
 * get next word in buffer
 *
 * str		const char *	string to parse
 * buffer	char *			buffer to store word in
 *
 * Returns:	updated pointer in str or NULL if no words left
 */
static const char * xvt_tio_next_word (const char *str, char *buffer)
{
	const char *s = str;

	/*----------------------------------------------------------------
	 * skip over white space
	 */
	while (isspace(*s))
		s++;

	/*----------------------------------------------------------------
	 * check if at end of string
	 */
	if (*s == 0)
		return (0);

	/*----------------------------------------------------------------
	 * copy word into buffer
	 */
	while (TRUE)
	{
		if (isspace(*s) || *s == 0)
			break;

		*buffer++ = *s++;
	}
	*buffer = 0;

	/*----------------------------------------------------------------
	 * skip over white space
	 */
	while (isspace(*s))
		s++;

	return (s);
}

/*------------------------------------------------------------------------
 * find a field entry in a TIOFLGS table
 *
 * tbl		const TIOFLGS *		pointer to table
 * field	char *				field to lookup
 *
 * Returns:	pointer to table entry or NULL
 */
static const TIOFLGS * xvt_tio_find_field (const TIOFLGS *tbl,
	const char *field)
{
	const TIOFLGS *t = tbl;

	for (t=tbl; t->string; t++)
	{
		if (xvt_strccmp(field, (char *)t->string) == 0)
			return (t);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * Get an argument for a field: must be 1 char long
 *
 * str		const char *		field to parse
 *
 * Returns:	value or -1
 */
static int xvt_tio_get_arg (const char *str)
{
	char buf[128];
	char *b = buf;

	for (; *str; str++)
	{
		if (*str == '"' || *str == '\\')
			continue;

		if (*str == '^' && str[1] != 0)
		{
			int c;

			c = *++str;
			*b++ = (c == '?' ? 0x7f : c & 0x1f);
		}
		else
		{
			*b++ = *str;
		}
	}
	*b = 0;

	if (buf[0] == 0 || buf[1] != 0)
		return (-1);

	return (*buf);
}

/*------------------------------------------------------------------------
 * add a field to a termio struct
 *
 * ttyp		TTY_STRUCT *	pointer to termio struct
 * field	const char *		field to add
 * more		const char *		field argument
 * msgbuf	char *			error msg buffer
 *
 * Returns:	0 if OK
 */
static int xvt_tio_add_field (TTY_STRUCT *ttyp, const char *field,
	const char *more, char *msgbuf)
{
	int set_on = TRUE;
	const TIOFLGS *	t;

	/*----------------------------------------------------------------
	 * check for sane
	 */
	if (xvt_strccmp(TIO_SANE, field) == 0)
	{
		ttyp->c_iflag |= TIO_ISANE;
		ttyp->c_lflag |= TIO_LSANE;
		ttyp->c_cflag |= TIO_CSANE;
		ttyp->c_oflag |= TIO_OSANE;

		return (ADD_OK);
	}

	/*----------------------------------------------------------------
	 * check for baud rate
	 */
	t = xvt_tio_find_field(xvt_tio_baudrate_words, field);
	if (t != 0)
	{
		if (ttyp != 0)
			ttyp->c_cflag |= (t->mode);

		return (ADD_OK);
	}

	/*----------------------------------------------------------------
	 * check if canonical cc
	 */
	t = xvt_tio_find_field(xvt_tio_ccflag_words_can, field);
	if (t != 0)
	{
		int arg;

		if (more == 0)
			return (ADD_MORE);

		arg = xvt_tio_get_arg(more);
		if (arg == -1)
		{
			sprintf(msgbuf, "Invalid argument %s for field %s",
				more, field);
			return (ADD_BAD);
		}

		if (ttyp != 0)
			ttyp->c_cc[t->mode] = (unsigned char)arg;

		return (ADD_OK);
	}

	/*----------------------------------------------------------------
	 * check if non-canonical cc
	 */
	t = xvt_tio_find_field(xvt_tio_ccflag_words_unc, field);
	if (t != 0)
	{
		int arg;

		if (more == 0)
			return (ADD_MORE);

		arg = xvt_tio_get_arg(more);
		if (arg == -1)
		{
			sprintf(msgbuf, "Invalid argument %s for field %s",
				more, field);
			return (ADD_BAD);
		}

		if (ttyp != 0)
			ttyp->c_cc[t->mode] = (unsigned char)arg;

		return (ADD_OK);
	}

	/*----------------------------------------------------------------
	 * check for special case "evenp"
	 */
	if (xvt_strccmp(field, "evenp") == 0)
	{
		field = "-parodd";
	}

	/*----------------------------------------------------------------
	 * check if setting on or off
	 */
	if (*field == '-')
	{
		set_on = FALSE;
		field++;
	}

	/*----------------------------------------------------------------
	 * check if in i-flags
	 */
	t = xvt_tio_find_field(xvt_tio_iflag_words, field);
	if (t != 0)
	{
		if (ttyp != 0)
		{
			if (set_on)
				ttyp->c_iflag |=  (t->mode);
			else
				ttyp->c_iflag &= ~(t->mode);
		}

		return (ADD_OK);
	}

	/*----------------------------------------------------------------
	 * check if in l-flags
	 */
	t = xvt_tio_find_field(xvt_tio_lflag_words, field);
	if (t != 0)
	{
		if (ttyp != 0)
		{
			if (set_on)
				ttyp->c_lflag |=  (t->mode);
			else
				ttyp->c_lflag &= ~(t->mode);
		}

		return (ADD_OK);
	}

	/*----------------------------------------------------------------
	 * check if in c-flags
	 */
	t = xvt_tio_find_field(xvt_tio_cflag_words, field);
	if (t != 0)
	{
		if (ttyp != 0)
		{
			if (set_on)
				ttyp->c_cflag |=  (t->mode);
			else
				ttyp->c_cflag &= ~(t->mode);
		}

		return (ADD_OK);
	}

	/*----------------------------------------------------------------
	 * check if in o-flags
	 */
	t = xvt_tio_find_field(xvt_tio_oflag_words, field);
	if (t != 0)
	{
		if (ttyp != 0)
		{
			if (set_on)
				ttyp->c_oflag |=  (t->mode);
			else
				ttyp->c_oflag &= ~(t->mode);
		}

		return (ADD_OK);
	}

	/*----------------------------------------------------------------
	 * bogus field name
	 */
	sprintf(msgbuf, "Invalid field %s", field);

	return (ADD_BAD);
}

/*------------------------------------------------------------------------
 * Create a termio struct from a char string
 *
 * ttyp		TTY_STRUCT *	pointer to termio struct to fill in
 *							If NULL, a check only is done.
 * str		const char *	pointer to string
 * create	int				TRUE to create a new struct
 * msgbuf	char *			error msg buffer
 *
 * Returns:	0 if OK
 */
int xvt_tio_build (TTY_STRUCT *ttyp, const char *str, int create, char *msgbuf)
{
	char tmp_msgbuf[128];

	/*----------------------------------------------------------------
	 * sanity checks
	 */
	if (msgbuf == 0)
		msgbuf = tmp_msgbuf;

	if (str == 0)
		str = "";

	*msgbuf = 0;

	/*----------------------------------------------------------------
	 * clear out struct if requested
	 */
	if (create && ttyp != 0)
	{
		const TIOFLGS *t;

		memset((void *)ttyp, 0, sizeof(*ttyp));

		/*------------------------------------------------------------
		 * set some defaults
		 */
		ttyp->c_lflag |= (ISIG | ICANON);
		ttyp->c_oflag |= (OPOST);
		ttyp->c_cflag |= (CREAD | HUPCL);

		/*------------------------------------------------------------
		 * set cc-flags
		 */
		for (t=xvt_tio_ccflag_words_can; t->string; t++)
		{
			ttyp->c_cc[t->mode] = t->mask;
		}
	}

	/*----------------------------------------------------------------
	 * process all fields in string
	 */
	while (TRUE)
	{
		char field[128];
		char more[128];
		int rc;

		str = xvt_tio_next_word(str, field);
		if (str == 0)
			break;

		rc = xvt_tio_add_field(ttyp, field, 0, msgbuf);
		if (rc == ADD_BAD)
		{
			/* error msg already set */
			return (-1);
		}

		if (rc == ADD_OK)
			continue;

		str = xvt_tio_next_word(str, more);
		if (str == 0)
		{
			sprintf(msgbuf, "field %s needs an argument",
				field);
			return (-1);
		}
		rc = xvt_tio_add_field(ttyp, field, more, msgbuf);
		if (rc == ADD_BAD)
		{
			/* error msg already set */
			return (-1);
		}
	}

	/*----------------------------------------------------------------
	 * perform a few sanity checks
	 */
	if (ttyp != 0)
	{
		if ((ttyp->c_cflag & CSIZE) == 0)
			ttyp->c_cflag |= CS8;

		if ((ttyp->c_cflag & CSIZE) == CS8)
			ttyp->c_cflag &= ~PARENB;

#ifdef CBAUD
		if ((ttyp->c_cflag & CBAUD) == 0)
			ttyp->c_cflag |= B9600;
#endif
	}

	return (0);
}
