/*------------------------------------------------------------------------
 * block letter routines
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * wblk_ltr() - display a block letter
 */
int wblk_ltr (WINDOW *win, int y, int x, int c, int t)
{
	const BLK_LETS *	bl;
	int i;
	int j;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0)
		return (ERR);

	if ((y + BLK_HEIGHT) > getmaxy(win))
		return (ERR);

	if ((y + BLK_WIDTH)  > getmaxx(win))
		return (ERR);

	/*--------------------------------------------------------------------
	 * get block letter struct for this char
	 */
	if (isalpha(c) && islower(c))
		c = toupper(c);

	if (isdigit(c))
	{
		bl = blk_nums + (c - '0');
	}
	else if (isalpha(c))
	{
		bl = blk_ltrs + (c - 'A' + 1);
	}
	else
	{
		bl = blk_ltrs;
	}

	/*--------------------------------------------------------------------
	 * display char one line at a time
	 */
	for (i=0; i<BLK_HEIGHT; i++)
	{
		wmove(win, y+i, x);

		for (j=0; j<BLK_WIDTH; j++)
		{
			int k = bl->line[i][j];

			if (k == ' ')
				waddch(win, ' ');
			else
				wblk_chr(win, B_BC, t ? c : 0);
		}
	}

	return (OK);
}

/*------------------------------------------------------------------------
 * wblk_str() - add a string as block letters
 */
int wblk_str (WINDOW *win, int y, int x, const char *str, int t)
{
	int	len;
	int	l;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (win == 0 || str == 0)
		return (ERR);

	if ((y + BLK_HEIGHT) > getmaxy(win))
		return (ERR);

	len = strlen(str);

	/*--------------------------------------------------------------------
	 * if x < 0, then center string
	 */
	if (x < 0)
	{
		if ((len * (BLK_WIDTH+1)) > getmaxx(win))
			return (ERR);

		x = (getmaxx(win) - (len * (BLK_WIDTH + 1))) / 2;
	}
	else
	{
		if (x + (len * (BLK_WIDTH+1)) >= getmaxx(win))
			return (ERR);
	}

	/*--------------------------------------------------------------------
	 * now do each char
	 */
	for (l=0; l<len; l++)
	{
		wblk_ltr(win, y, x, str[l], t);

		x += (BLK_WIDTH + 1);
	}

	return (OK);
}
