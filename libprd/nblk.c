/*------------------------------------------------------------------------
 * NBLK routines
 */
#include "libprd.h"

NBLK *nblk_make (void)
{
	NBLK *n;

	n = (NBLK *)MALLOC(sizeof(*n));
	if (n != 0)
	{
		memset(n, 0, sizeof(*n));
	}

	return (n);
}

void nblk_free (NBLK *n)
{
	FREE(n);
}
