/*------------------------------------------------------------------------
 * more misc utilities
 */
#include "libprd.h"

int get_rdev (struct stat *s)
{
	return (s->st_rdev);
}

int get_minor (struct stat *s)
{
	return os_get_minor(s);
}

int get_major (struct stat *s)
{
	return os_get_major(s);
}

int set_rdev (struct stat *s, int rdev)
{
	s->st_rdev = rdev;

	return (0);
}

int make_home_dir (void)
{
	int rc = os_dir_make(gbl(pgm_home));

	if (rc >= 0)
		rc = 0;

	return (rc);
}
