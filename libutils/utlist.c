/*------------------------------------------------------------------------
 *	list processing routines
 */
#include "utcommon.h"

/*------------------------------------------------------------------------
 * lnew() - create a list element
 */
LIST *lnew (void *a, const char *file, int line)
{
	LIST *l;

	l = (LIST *)SYSMEM_MALLOC(sizeof(*l), "lnew", file, line);
	if (l != 0)
	{
		lnext(l) = 0;
		lid(l)   = a;
	}

	return (l);
}

/*------------------------------------------------------------------------
 * lfree() - free a list element
 */
LIST *lfree (LIST *l, int delid, const char *file, int line)
{
	if (l)
	{
		if (delid && lid(l) != 0)
			SYSMEM_FREE(lid(l), "lfree", file, line);
		SYSMEM_FREE(l, "lfree", file, line);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * lscrap() - delete an entire list
 */
LIST *lscrap (LIST *l, int delid, const char *file, int line)
{
	LIST *e;

	for (; l; l=e)
	{
		e = lnext(l);
		lfree(l, delid, file, line);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * ldelete() - delete an element from a list
 */
LIST *ldelete (LIST *l, LIST *e, int delid, const char *file, int line)
{
	l = lremove(l, e);
	lfree(e, delid, file, line);

	return (l);
}

/*------------------------------------------------------------------------
 * ldup() - dup a list
 */
LIST *ldup (LIST *l, const char *file, int line)
{
	LIST *x = 0;
	LIST *y = 0;
	LIST *t;

	for (; l; l=lnext(l))
	{
		t = lnew(lid(l), file, line);
		if (t == 0)
			return lscrap(x, FALSE, file, line);

		if (y)
			lnext(y) = t;
		else
			x = t;
		y = t;
	}

	return (x);
}

/*------------------------------------------------------------------------
 * lappend() - append a list element to a list
 */
LIST *lappend (LIST *l, LIST *e)
{
	if (l)
	{
		linsert(llast(l), e);
		return (l);
	}

	return (e);
}

/*------------------------------------------------------------------------
 * lapplyf() - apply a function to each element in a list
 */
void lapplyf (LIST *l, LIST_FUNC *f, void *a)
{
	for (; l; l=lnext(l))
	{
		(*f)(l, a);
	}
}

/*------------------------------------------------------------------------
 * lcount() - count the elements in a list
 */
int lcount (LIST *l)
{
	int n = 0;

	for (; l; l=lnext(l))
	{
		n++;
	}

	return (n);
}

/*------------------------------------------------------------------------
 * lfind() - find an element in a list
 */
LIST *lfind (LIST *l, void *a)
{
	for (; l; l=lnext(l))
	{
		if (a == lid(l))
			return (l);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * lnth() - get the nth element in a list
 */
LIST *lnth (LIST *l, int n)
{
	if (n < 0)
		return (0);

	for (; n; n--)
	{
		l = lnext(l);
		if (l == 0)
			break;
	}

	return (l);
}

/*------------------------------------------------------------------------
 * linsert() - insert a list into a list
 */
LIST *linsert (LIST *l, LIST *e)
{
	if (l == 0)
		return (e);

	if (e)
	{
		lnext(llast(e)) = lnext(l);
		lnext(l) = e;
	}

	return (l);
}

/*------------------------------------------------------------------------
 * llast() - get the last element in a list
 */
LIST *llast (LIST *l)
{
	if (l == 0)
		return (0);

	while (lnext(l))
		l = lnext(l);

	return (l);
}

/*------------------------------------------------------------------------
 * lindex() - get the index of an element in a list
 */
int lindex (LIST *l, LIST *e)
{
	int i;

	for (i=0; l; i++, l=lnext(l))
	{
		if (l == e)
			return (i);
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * lremove() - remove an element from a list
 */
LIST *lremove (LIST *l, LIST *e)
{
	LIST *t;

	if (l == 0 || e == 0)
		return (l);

	if (l==e)
	{
		l = lnext(l);
	}
	else
	{
		t = l;
		while (lnext(t) && lnext(t) != e)
			t = lnext(t);

		if (lnext(t))
			lnext(t) = lnext(e);
	}
	lnext(e) = 0;

	return (l);
}

/*------------------------------------------------------------------------
 * lsort() - sort a list
 *
 *	This version of lsort copies the ids to an array
 *	and uses a ssort or qsort routine to sort the ids,
 *	then copies the ids back into the list.
 */
LIST *lsort (LIST *p, LIST_COMP *f)
{
	void **	list_array;
	LIST *	l;
	int		n;
	int		i;

	/*--------------------------------------------------------------------
	 * get number of elements and bail if zero or one
	 */
	n = lcount(p);
	if (n <= 1)
		return (p);

	/*--------------------------------------------------------------------
	 * allocate an array
	 */
	list_array = (void **)MALLOC(n * sizeof(*list_array));
	if (list_array == 0)
	{
		/* if can't get memory, just don't sort */
		return (p);
	}

	/*--------------------------------------------------------------------
	 * copy ids into the array
	 */
	for (l=p, i=0; i<n; l=lnext(l), i++)
		list_array[i] = lid(l);

	/*--------------------------------------------------------------------
	 * sort it
	 */
	ssort((char *)list_array, n, sizeof(*list_array), f);

	/*--------------------------------------------------------------------
	 * copy ids back into list
	 */
	for (l=p, i=0; i<n; l=lnext(l), i++)
		lid(l) = list_array[i];

	/*--------------------------------------------------------------------
	 * free the array
	 */
	FREE((void *)list_array);

	return (p);
}
