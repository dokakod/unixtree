/*------------------------------------------------------------------------
 * DBLK routines
 */
#include "libprd.h"

DBLK *dblk_make (void)
{
	DBLK *d;

	d = (DBLK *)MALLOC(sizeof(*d));
	if (d != 0)
	{
		memset(d, 0, sizeof(*d));
	}

	return (d);
}

void dblk_free (DBLK *d)
{
	if (d->name != d->fullname)
		FREE(d->fullname);
	FREE(d);
}

void dblk_set_name (DBLK *d, const char *dir_name)
{
	int m = sizeof(d->name);
	int i;
	int l;

	l = strlen(dir_name);
	if (l < m)
	{
		strcpy(d->name, dir_name);
		d->fullname = d->name;
	}
	else
	{
		d->fullname = (char *)MALLOC((unsigned)l+2);	/* just in case */
		if (!d->fullname)
			d->fullname = d->name;
		else
			strcpy(d->fullname, dir_name);
		for (i=0; i<m-2; i++)
			d->name[i] = dir_name[i];
		d->name[i++] = pgm_const(too_long_char);
		d->name[i] = 0;
	}
}

void dblk_change_name (DBLK *d, const char *dir_name)
{
	if (d->fullname != d->name)
		FREE(d->fullname);
	dblk_set_name(d, dir_name);
}
