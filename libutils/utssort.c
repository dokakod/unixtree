/*------------------------------------------------------------------------
 *	ssort()
 *
 *	Works just like qsort() except that a shell sort, rather than a
 *	quick sort, is used.
 *	This is more efficient than quicksort for small numbers
 *	of elements and it's not recursive so will use much less stack space.
 *
 *	This routine started out as the one in K&R.
 */
#include "utcommon.h"

int ssort (char *base, int nel, int width,
	int (*cmp)(const void *p1, const void *p2))
{
	int		i;
	int		j;
	int		k;
	int		gap;
	char *	p1;
	char *	p2;
	char	tmp;

	if (nel <= 1)
		return (0);

	for (gap=1; gap<=nel; gap=3*gap+1)
		;

	for (gap/=3; gap>0; gap/=3)
	{
		for (i=gap; i<nel; i++)
		{
			for (j=i-gap; j>=0; j-=gap)
			{
				p1 = base + ( j      * width);
				p2 = base + ((j+gap) * width);

				if ((*cmp)(p1, p2) <= 0)
					break;

				for (k=width; --k>=0; )
				{
					tmp   = *p1;
					*p1++ = *p2;
					*p2++ = tmp;
				}
			}
		}
	}

	return (0);
}
