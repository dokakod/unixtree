/*------------------------------------------------------------------------
 * get attributes for file display
 */
#include "libprd.h"

attr_t get_file_attr (WINDOW *win, FBLK *f)
{
	if (opt(do_file_colors) && gbl(scr_is_color))
	{
		attr_t file_attr = fileattr_attr(gbl(fa_list), f);

		if (file_attr != A_UNSET)
		{
			if ((file_attr & (FA_RAINBOW | FA_RANDOM)) != 0)
			{
				return (file_attr);
			}
			else
			{
				attr_t save_attr = wattrget(win);
				int fg = A_FG_CLRNUM(file_attr);
				int bg = A_BG_CLRNUM(save_attr);

				if (fg != bg)
				{
					file_attr = A_CLR(fg, bg);
					return (file_attr);
				}
			}
		}
	}

	return (A_UNSET);
}

void fa_list_load (int *errs)
{
	char filename[MAX_FILELEN];
	char path[MAX_PATHLEN];
	char *p;

	strcpy(filename, pgmi(m_pgm_program));
	fn_set_ext(filename, pgm_const(clr_ext));

	p = os_get_path(filename, gbl(pgm_path), path);

	if (p != 0)
		gbl(fa_list) = fileattr_load(p, errs);
}

void fa_list_dump (void)
{
	if (gbl(fa_list) != 0)
	{
		fileattr_save("-", gbl(fa_list));
	}
}

void fa_list_free (void)
{
	if (gbl(fa_list) != 0)
	{
		gbl(fa_list) = fileattr_free(gbl(fa_list));
	}
}
