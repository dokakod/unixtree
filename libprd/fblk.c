/*------------------------------------------------------------------------
 * FBLK routines
 */
#include "libprd.h"

FBLK *fblk_make (void)
{
	FBLK *f;

	f = (FBLK *)MALLOC(sizeof(*f));
	if (f != 0)
	{
		memset(f, 0, sizeof(*f));
	}

	return (f);
}

void fblk_free (FBLK *f)
{
	if (f->name != f->fullname)
		FREE(f->fullname);
	if (f->sym_name != 0)
		FREE(f->sym_name);
	FREE(f);
}

void fblk_set_name (FBLK *f, const char *filename)
{
	int m = sizeof(f->name);
	int i;
	int l;

	l = strlen(filename);
	if (l < m)
	{
		strcpy(f->name, filename);
		f->fullname = f->name;
	}
	else
	{
		f->fullname = (char *)MALLOC((unsigned)l+1);
		if (!f->fullname)
			f->fullname = f->name;
		else
			strcpy(f->fullname, filename);
		for (i=0; i<m-2; i++)
			f->name[i] = filename[i];
		f->name[i++] = pgm_const(too_long_char);
		f->name[i] = 0;
	}
}

void fblk_change_name (FBLK *f, const char *filename)
{
	if (f->fullname != f->name)
		FREE(f->fullname);
	fblk_set_name(f, filename);
}
