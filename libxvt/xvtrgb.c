/*------------------------------------------------------------------------
 * X color database processing
 *
 * The X color database is a compiled database based on the
 * contents of the text file "rgb.txt", which contains the RGB value
 * for each specified color.
 *
 * X server-vendors are allowed to add entries to their list, but the "basic"
 * list of entries should always be there.
 *
 * X server-vendors are also allowed to "tweak" the RGB values to match the
 * characteristics of their hardware, but the names should not be changed
 * (to not protect the innocent).  For this reason, although we provide
 * access to the RGB values, we don't use them ourselves, but rather
 * depend on XParseColor().
 */
#include "xvtcommon.h"

/*------------------------------------------------------------------------
 * built-in table of default color entries
 *
 * This feature is disabled since it adds about 20K to the size of the code.
 */
#if 0
#  include "xvtrgb.i"
#else
static const char **xvt_rgb_colors = 0;
#endif

/*------------------------------------------------------------------------
 * list of possible rgb file pathnames
 */
static const char *	xvt_rgb_paths[] =
{
	XVT_RGB_FILES,
	0
};

/*------------------------------------------------------------------------
 * return an in-memory copy of the rgb file
 */
static char * xvt_rgb_file (void)
{
	const char **	path;

	for (path=xvt_rgb_paths; *path; path++)
	{
		struct stat		stbuf;
		int				rc;

		rc = stat(*path, &stbuf);
		if (rc == 0)
		{
			char *	cf;
			FILE *	fp;

			fp = fopen(*path, "r");
			if (fp == 0)
				return (0);

			cf = (char *)MALLOC(stbuf.st_size + 1);
			if (cf == 0)
			{
				fclose(fp);
				return (0);
			}
			cf[stbuf.st_size] = 0;

			fread(cf, stbuf.st_size, 1, fp);
			fclose(fp);
			return (cf);
		}
	}

	return (0);
}

/*------------------------------------------------------------------------
 * qsort compare routine
 */
static int xvt_rgb_compare (const void *v1, const void *v2)
{
	const RGB_ENTRY *	r1 = (const RGB_ENTRY *)v1;
	const RGB_ENTRY *	r2 = (const RGB_ENTRY *)v2;
	int					rc;

	rc = xvt_strccmp(r1->name, r2->name);
	if (rc == 0)
		rc = strcmp(r1->name, r2->name);

	return (rc);
}

/*------------------------------------------------------------------------
 * convert a line from the RGB file to an RGB struct
 *
 * line is in form " rrr ggg bbb name"
 *
 * returns: pointer to next line
 */
static const char * xvt_rgb_convert (RGB_ENTRY **rp, const char *line)
{
	RGB_ENTRY *		re	= *rp;
	const char *	lp	= line;
	char *			s;
	int				n;

	/*--------------------------------------------------------------------
	 * if comment line, just return ptr to next line
	 */
	if (*lp == '!')
	{
		for (; *lp; lp++)
		{
			if (*lp == '\n')
				break;
		}

		if (*lp != 0)
			lp++;

		return (lp);
	}

	/*--------------------------------------------------------------------
	 * collect red value
	 */
	for (; *lp; lp++)
	{
		if (! isspace(*lp))
			break;
	}

	n = 0;
	for (; *lp; lp++)
	{
		if (! isdigit(*lp))
			break;
		n = (10 * n) + (*lp - '0');
	}
	re->colors.red		= n;

	/*--------------------------------------------------------------------
	 * collect green value
	 */
	for (; *lp; lp++)
	{
		if (! isspace(*lp))
			break;
	}

	n = 0;
	for (; *lp; lp++)
	{
		if (! isdigit(*lp))
			break;
		n = (10 * n) + (*lp - '0');
	}
	re->colors.green	= n;

	/*--------------------------------------------------------------------
	 * collect blue value
	 */
	for (; *lp; lp++)
	{
		if (! isspace(*lp))
			break;
	}

	n = 0;
	for (; *lp; lp++)
	{
		if (! isdigit(*lp))
			break;
		n = (10 * n) + (*lp - '0');
	}
	re->colors.blue		= n;

	/*--------------------------------------------------------------------
	 * collect name (minus any white-space)
	 */
	for (; *lp; lp++)
	{
		if (! isspace(*lp))
			break;
	}

	for (s=re->name; *lp; lp++)
	{
		if (*lp == '\n')
			break;
		if (! isspace(*lp))
			*s++ = *lp;
	}
	*s = 0;

	/*--------------------------------------------------------------------
	 * return pointer to next line & bump entry pointer
	 */
	*rp = (re + 1);

	if (*lp != 0)
		lp++;

	return (lp);
}

/*------------------------------------------------------------------------
 * return an array of RGB entries
 * in sorted order with duplicate entries removed.
 */
RGB_ENTRY * xvt_rgb_list (void)
{
	RGB_ENTRY *		rl;
	RGB_ENTRY *		re;
	const char **	ct;
	const char *	cx;
	char *			cf;
	int				n;
	int				i;
	int				j;

	/*--------------------------------------------------------------------
	 * load the color file into memory
	 */
	cf = xvt_rgb_file();
	if (cf == 0)
	{
		/*----------------------------------------------------------------
		 * if no rgb file was found, check if we have a default table
		 */
		if (xvt_rgb_colors == 0 || *xvt_rgb_colors == 0)
			return (0);
	}

	/*--------------------------------------------------------------------
	 * count the number of lines in it (the file image or the table)
	 */
	n = 0;
	if (cf == 0)
	{
		for (ct=xvt_rgb_colors; *ct; ct++)
			n++;
	}
	else
	{
		for (cx=cf; *cx; cx++)
		{
			if (*cx == '\n')
				n++;
		}
	}

	/*--------------------------------------------------------------------
	 * allocate an array of RGB entries
	 */
	rl = (RGB_ENTRY *)MALLOC((n + 1) * sizeof(*rl));
	if (rl == 0)
	{
		FREE(cf);
		return (0);
	}

	/*--------------------------------------------------------------------
	 * now create each entry from the original file/table contents
	 */
	re = rl;

	if (cf == 0)
	{
		for (ct=xvt_rgb_colors; *ct; ct++)
			xvt_rgb_convert(&re, *ct);
	}
	else
	{
		cx = cf;
		for (i=0; i<n; i++)
			cx = xvt_rgb_convert(&re, cx);
	}

	n = (re - rl);

	/*--------------------------------------------------------------------
	 * we can now free our original file contents
	 */
	if (cf != 0)
		FREE(cf);

	/*--------------------------------------------------------------------
	 * now sort the array
	 */
	qsort(rl, n, sizeof(*rl), xvt_rgb_compare);

	/*--------------------------------------------------------------------
	 * Now eliminate duplicate colors (different only in case/white-space)
	 */
	for (j=1, i=1; i<n; i++)
	{
		if (xvt_strccmp(rl[i-1].name, rl[i].name) != 0)
		{
			memcpy(rl + j++, rl + i, sizeof(*rl));
		}
	}

	/*--------------------------------------------------------------------
	 * set an empty entry at end of the list (there is always room for it)
	 */
	rl[j].colors.red	= -1;
	rl[j].colors.green	= -1;
	rl[j].colors.blue	= -1;
	rl[j].name[0]		= 0;

	return (rl);
}

/*------------------------------------------------------------------------
 * free the rgb list
 */
void xvt_rgb_free (RGB_ENTRY *list)
{
	if (list != 0)
		FREE(list);
}

/*------------------------------------------------------------------------
 * get an rgb list, dump it, & free it
 */
void xvt_rgb_dump (FILE *fp, int verbose)
{
	RGB_ENTRY *	list	= xvt_rgb_list();

	if (list != 0)
	{
		RGB_ENTRY *	re;

		for (re=list; *re->name; re++)
		{
			if (verbose)
			{
				fprintf(fp, "%3d %3d %3d\t(%02x %02x %02x)\t",
					re->colors.red, re->colors.green, re->colors.blue,
					re->colors.red, re->colors.green, re->colors.blue);
			}
			fprintf(fp, "%s\n", re->name);
		}

		xvt_rgb_free(list);
	}
}

/*------------------------------------------------------------------------
 * validate a color name
 *
 * returns: TRUE / FALSE
 */
static int xvt_rgb_validate (RGB_ENTRY **list, const char *color)
{
	RGB_ENTRY *		colors	= *list;
	RGB_ENTRY *		re;
	const char *	cp;

	/*--------------------------------------------------------------------
	 * if color is in form "#.....", "rgb:...", or "None", it is OK
	 */
	if (*color == '#')
	{
		for (cp=color+1; *cp; cp++)
		{
			if (! isxdigit(*cp))
				return (FALSE);
		}

		return (TRUE);
	}

	if (xvt_strccmpn("rgb:", color) == 0)
	{
		for (cp=color+4; *cp; cp++)
		{
			if (! isxdigit(*cp) && *cp != '/')
				return (FALSE);
		}

		return (TRUE);
	}

	if (xvt_strccmp("None", color) == 0)
	{
		return (TRUE);
	}

	/*--------------------------------------------------------------------
	 * lookup name in list
	 */
	if (colors == 0)
	{
		/*----------------------------------------------------------------
		 * load the list if we haven't loaded it yet
		 */
		colors = xvt_rgb_list();
		*list = colors;

		/*----------------------------------------------------------------
		 * If we can't load the list, just assume the color is OK.
		 */
		if (colors == 0)
			return (TRUE);
	}

	for (re=colors; *re->name; re++)
	{
		if (xvt_strccmpc(re->name, color) == 0)
			return (TRUE);
	}

	return (FALSE);
}

/*------------------------------------------------------------------------
 * check a color value
 *
 * returns: XVT_OK if OK
 */
static int xvt_rgb_check_color (XVT_DATA *xd, RGB_ENTRY **list,
	const char *name, const char *color)
{
	int	rc;

	if (color == 0 || *color == 0)
		return (XVT_OK);

	rc = xvt_rgb_validate(list, color);
	if (! rc)
	{
		sprintf(xd->error_msg, "invalid color string \"%s\" for \"%s\"",
			color, name);
		xd->error_num = XVT_ERR_INVARG;
		return (-1);
	}

	return (XVT_OK);
}

/*------------------------------------------------------------------------
 * check any colors specified
 */
int xvt_rgb_check (XVT_DATA *xd)
{
	RGB_ENTRY *	colors	= 0;
	int			i;
	int			rc;

	for (i=0; i<XVT_CLR_NUM; i++)
	{
		rc = xvt_rgb_check_color(xd, &colors, color_name(i), xd->scr_color[i]);
		if (rc)
			break;
	}

	if (rc == XVT_OK)
		rc = xvt_rgb_check_color(xd, &colors, "foreground", xd->fg_color);

	if (rc == XVT_OK)
		rc = xvt_rgb_check_color(xd, &colors, "background", xd->bg_color);

	if (rc == XVT_OK)
		rc = xvt_rgb_check_color(xd, &colors, "cursor",     xd->cr_color);

	if (rc == XVT_OK)
		rc = xvt_rgb_check_color(xd, &colors, "pointer",    xd->pr_color);

	if (rc == XVT_OK)
		rc = xvt_rgb_check_color(xd, &colors, "border",     xd->fg_color);

	if (rc == XVT_OK)
		rc = xvt_rgb_check_color(xd, &colors, "scrollbar",  xd->sb_color);

	if (rc == XVT_OK)
		rc = xvt_rgb_check_color(xd, &colors, "trough",     xd->st_color);

	xvt_rgb_free(colors);

	return (rc);
}
