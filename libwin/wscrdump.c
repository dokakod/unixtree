/*------------------------------------------------------------------------
 * screen dump routines
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * scrn_scr_dump() - dump curscr to a file
 */
int scrn_scr_dump (SCREEN *s, const char *filename)
{
	FILE *	fp;
	int		rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (s == 0 || filename == 0 || *filename == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * open the file
	 */
	fp = fopen(filename, "wb");
	if (fp == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * save curscr
	 */
	rc = putwin(SCR_CURSCR(s), fp);

	/*--------------------------------------------------------------------
	 * close the file
	 */
	fclose(fp);

	return (rc);
}

/*------------------------------------------------------------------------
 * scrn_scr_restore() - read in curscr from a file
 */
int scrn_scr_restore (SCREEN *s, const char *filename)
{
	FILE *			fp;
	unsigned int	m;
	int				maxy;
	int				maxx;
	int				begy;
	int				begx;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (s == 0 || filename == 0 || *filename == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * open the file
	 */
	fp = fopen(filename, "rb");
	if (fp == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * check for magic number
	 */
	m = get_4byte(fp);
	if (m != WIN_MAGIC)
	{
		fclose(fp);
		return (ERR);
	}

	/*--------------------------------------------------------------------
	 * header info
	 */
	maxy = get_4byte(fp);
	maxx = get_4byte(fp);
	begy = get_4byte(fp);
	begx = get_4byte(fp);

	/*--------------------------------------------------------------------
	 * check if header matches curscr parameters
	 */
	if (maxy != getmaxy(SCR_CURSCR(s)) || maxx != getmaxx(SCR_CURSCR(s)))
	{
		fclose(fp);
		return (ERR);
	}

	/*--------------------------------------------------------------------
	 * read file into curscr
	 */
	win_load(SCR_CURSCR(s), fp);

	/*--------------------------------------------------------------------
	 * close the file
	 */
	fclose(fp);

	return (OK);
}

/*------------------------------------------------------------------------
 * scrn_scr_init() - read a file into stdscr
 */
int scrn_scr_init (SCREEN *s, const char *filename)
{
	FILE *			fp;
	unsigned int	m;
	int				maxy;
	int				maxx;
	int				begy;
	int				begx;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (s == 0 || filename == 0 || *filename == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * open the file
	 */
	fp = fopen(filename, "rb");
	if (fp == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * check for magic number
	 */
	m = get_4byte(fp);
	if (m != WIN_MAGIC)
	{
		fclose(fp);
		return (ERR);
	}

	/*--------------------------------------------------------------------
	 * header info
	 */
	maxy = get_4byte(fp);
	maxx = get_4byte(fp);
	begy = get_4byte(fp);
	begx = get_4byte(fp);

	/*--------------------------------------------------------------------
	 * check if header info matches stdscr parameters
	 */
	if (maxy != getmaxy(SCR_STDSCR(s)) || maxx != getmaxx(SCR_STDSCR(s)))
	{
		fclose(fp);
		return (ERR);
	}

	/*--------------------------------------------------------------------
	 * read in stdscr & clear curscr
	 */
	win_load(SCR_STDSCR(s), fp);
	clearok(SCR_CURSCR(s), TRUE);

	/*--------------------------------------------------------------------
	 * close the file
	 */
	fclose(fp);

	return (OK);
}

/*------------------------------------------------------------------------
 * scrn_scr_set() - restore a file & believe it it what is currently displayed
 */
int scrn_scr_set (SCREEN *s, const char *filename)
{
	int rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (s == 0 || filename == 0 || *filename == 0)
		return (ERR);

	/*--------------------------------------------------------------------
	 * restore curscr
	 */
	rc = scrn_scr_restore(s, filename);
	if (rc)
		return (ERR);

	/*--------------------------------------------------------------------
	 * copy its contents to stdscr
	 */
	copywin(SCR_CURSCR(s), SCR_TRMSCR(s), 0, 0, 0, 0,
		getmaxy(SCR_CURSCR(s)), getmaxx(SCR_CURSCR(s)), TRUE);

	return (OK);
}
