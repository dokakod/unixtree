/*------------------------------------------------------------------------
 * output a termio struct via a call-back routine
 */

#include "xvtcommon.h"
#include "xvttioflgs.h"

/*------------------------------------------------------------------------
 *	Given a word of mask bits, a table of possible mask bit values,
 *	and an output buffer, fill the buffer with all values we know about.
 *
 * returns: # chars added to buffer
 */
static int xvt_tio_cvt_flags (unsigned int word, const TIOFLGS *tbl, char *buf)
{
	*buf = 0;

	while (tbl->string)
	{
		unsigned int	mask = (tbl->mask == 0) ? tbl->mode : tbl->mask;
		int		yes;

		/*--------------------------------------------------------
		 * mask off the word and see if matches our given mode.
		 * If so, consider it a match
		 */
		yes = ((word & mask) == tbl->mode);
		if (yes)
		{
			if (*buf != 0)
				strcat(buf, " ");
			strcat(buf, tbl->string);
		}
		else if (tbl->mask == 0)
		{
			if (*buf != 0)
				strcat(buf, " ");
			strcat(buf, "-");
			strcat(buf, tbl->string);
		}

		tbl++;
	}

	return strlen(buf);
}

/*------------------------------------------------------------------------
 * the same, but for the CC array
 */
static int xvt_tio_cvt_cc (const TTY_STRUCT *ttyp, char *buf)
{
	const TIOFLGS *	t;
	int		dec;

	/*----------------------------------------------------------------
	 * Now we need to print the value of the various characters in
	 * the c_cc array.  If we are in input canonicalization mode, then
	 * all the various characters have a special meaning, but otherwise
	 * all we care about is VMIN and VTIME.
	 */
	if (ttyp->c_lflag & ICANON)
	{
		t = xvt_tio_ccflag_words_can;
		dec = FALSE;
	}
	else
	{
		t = xvt_tio_ccflag_words_unc;
		dec = TRUE;
	}

	*buf = 0;
	for (; t->string; t++)
	{
		int m = ttyp->c_cc[t->mode];
		char ccbuf[10];

		if (*buf != 0)
			strcat(buf, " ");

		strcat(buf, t->string);
		strcat(buf, "=");

		if (m < 0)
			m &= 0xff;

		if (dec)
		{
			sprintf(ccbuf, "%d", m);
		}
		else
		{
			strcat(buf, " '");

			if (m < 0x20)
			{
				ccbuf[0] = '^';
				ccbuf[1] = m + '@';
				ccbuf[2] = 0;
			}
			else if (m < 0x7f)
			{
				ccbuf[0] = m;
				ccbuf[1] = 0;
			}
			else if (m == 0x7f)
			{
				ccbuf[0] = '^';
				ccbuf[1] = '?';
				ccbuf[2] = 0;
			}
			else
			{
				sprintf(ccbuf, "%02x", m);
			}
		}
		strcat(buf, ccbuf);
		if (! dec)
			strcat(buf, "'");
	}

	return strlen(buf);
}

/*------------------------------------------------------------------------
 * dummy output routine
 */
static void xvt_tio_dummy_cb (void *data, const char *buf)
{
	FILE *	fp = (FILE *)data;

	fprintf(fp, "%s\n", buf);
}

/*------------------------------------------------------------------------
 * dump a termio struct via a call-back routine
 *
 * Note that the call-back routine is responsible for any new-lines.
 *
 * If <rtn> is NULL, then <data> is interpreted to be a stream which
 * is written to.
 */
void xvt_tio_dump (const TTY_STRUCT *ttyp, void *data, TIO_DUMP_RTN *rtn)
{
	char	line[256];
	char	buf[256];

	/*----------------------------------------------------------------
	 * sanity checks
	 */
	if (ttyp == 0)
		return;

	if (rtn == 0)
		rtn = xvt_tio_dummy_cb;

	/*----------------------------------------------------------------
	 * print out flag words
	 */
	xvt_tio_cvt_flags((unsigned int)ttyp->c_iflag, xvt_tio_iflag_words, buf);
	sprintf(line, "c_iflags = %s", buf);
	(rtn)(data, line);

	xvt_tio_cvt_flags((unsigned int)ttyp->c_cflag, xvt_tio_cflag_words, buf);
	sprintf(line, "c_cflags = %s", buf);
	(rtn)(data, line);

	xvt_tio_cvt_flags((unsigned int)ttyp->c_oflag, xvt_tio_oflag_words, buf);
	sprintf(line, "c_oflags = %s", buf);
	(rtn)(data, line);

	xvt_tio_cvt_flags((unsigned int)ttyp->c_lflag, xvt_tio_lflag_words, buf);
	sprintf(line, "c_lflags = %s", buf);
	(rtn)(data, line);

	xvt_tio_cvt_cc(ttyp, buf);
	sprintf(line, "cc       = %s", buf);
	(rtn)(data, line);
}

/*------------------------------------------------------------------------
 * convert a termio struct to a "stty"-like buffer
 */
void xvt_tio_buf (const TTY_STRUCT *ttyp, char *buf)
{
	char *	bp	= buf;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (ttyp == 0 || *buf == 0)
		return;

	/*--------------------------------------------------------------------
	 * now build up the buffer
	 */
	bp += xvt_tio_cvt_flags(ttyp->c_iflag, xvt_tio_iflag_words, bp);
	bp += xvt_tio_cvt_flags(ttyp->c_cflag, xvt_tio_cflag_words, bp);
	bp += xvt_tio_cvt_flags(ttyp->c_oflag, xvt_tio_oflag_words, bp);
	bp += xvt_tio_cvt_flags(ttyp->c_lflag, xvt_tio_lflag_words, bp);
	bp += xvt_tio_cvt_cc(ttyp, bp);
}
