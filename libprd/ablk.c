/*------------------------------------------------------------------------
 * ABLK routines
 */
#include "libprd.h"

ABLK *ablk_make (void)
{
	ABLK *a;

	a = (ABLK *)MALLOC(sizeof(*a));
	if (a != 0)
	{
		memset(a, 0, sizeof(*a));
	}

	return (a);
}

void ablk_free (ABLK *a)
{
	FREE(a);
}
