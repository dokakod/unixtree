/*------------------------------------------------------------------------
 * print screen routines
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * win_key_prtscrn() - async key routine for KEY_PRINT_SCREEN
 */
int win_key_prtscrn (int key, void *data)
{
	SCREEN *	s	= (SCREEN *)data;
	WINDOW *	cs	= SCR_CURSCR(s);
	PRINTER *	p;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (cs == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * open the printer
	 */
	p = prt_open(P_SPOOL, NULL, NULL);
	if (p == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * print curscr
	 */
	print_win(cs, p);

	/*--------------------------------------------------------------------
	 * close the printer
	 */
	prt_close(p, NULL);

	return (OK);
}

/*------------------------------------------------------------------------
 * print_win() - print a window to a printer
 */
int print_win (WINDOW *win, PRINTER *p)
{
	int y, x;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0 || p == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * print each line
	 *
	 * Note that any alt-chars encountered are converted to their
	 * ascii equivalents.
	 */
	for (y=0; y<getmaxy(win); y++)
	{
		char buffer[BUFSIZ];
		char *b = buffer;

		for (x=0; x<getmaxx(win); x++)
		{
			chtype	c	= wchat(win, y, x);
			int		ch	= A_GETTEXT(c);
			int		at	= A_GETATTR(c);

			/*------------------------------------------------------------
			 * If the char is an alt-char, convert it to its ASCII equiv.
			 */
			if (at & A_ALTCHARSET)
			{
				int i;
				int j;
				int found = FALSE;

				/*--------------------------------------------------------
				 * First lookup the char in the various line-drawing sets
				 */
				for (i = B_SHSV; i<= B_BLOCK; i++)
				{
					for (j=0; j<11; j++)
					{
						if (ch == box_chrs[i][j])
						{
							found = TRUE;
							ch    = box_chrs[B_ASCII][j];
						}
					}

					if (found)
						break;
				}

				/*--------------------------------------------------------
				 * If not a line-drawing char, check if it is a scrollbar
				 * char.  Note that, if it is a scrollbar char, we don't know
				 * whether it is for a vert or horz scrollbar, so we use
				 * a generic char representation.
				 */
				if (! found)
				{
					if (ch == A_GETTEXT(ACS_BOARD))
					{
						ch = '@';
					}
					else if (ch == A_GETTEXT(ACS_CKBOARD))
					{
						ch = '*';
					}
					else if (ch < 0x20 || ch > 0x7f)
					{
						ch = '.';
					}
				}
			}

			*b++ = ch;
		}
		*b++ = '\r';
		*b++ = '\n';
		*b = 0;

		prt_output_str(p, buffer);
	}

	return (OK);
}
