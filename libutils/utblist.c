/*------------------------------------------------------------------------
 *	blist processing routines
 */
#include "utcommon.h"

/*------------------------------------------------------------------------
 * bnew() - create a new list element
 */
BLIST *bnew (void *a, const char *file, int line)
{
	BLIST *l;

	l = (BLIST *)SYSMEM_MALLOC(sizeof(*l), "bnew", file, line);
	if (l != 0)
	{
		bnext(l) = 0;
		bprev(l) = 0;
		bid(l)   = a;
	}

	return (l);
}

/*------------------------------------------------------------------------
 * bfree() - free a list element
 */
BLIST *bfree (BLIST *l, int delid, const char *file, int line)
{
	if (l)
	{
		if (delid && bid(l) != 0)
			SYSMEM_FREE(bid(l), "bfree", file, line);
		SYSMEM_FREE(l, "bfree", file, line);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * bscrap() - delete an entire list
 */
BLIST *bscrap (BLIST *l, int delid, const char *file, int line)
{
	BLIST *e;

	for (; l; l=e)
	{
		e = bnext(l);
		bfree(l, delid, file, line);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * bdelete() - delete an element from a list
 */
BLIST *bdelete (BLIST *l, BLIST *e, int delid, const char *file, int line)
{
	l = bremove(l, e);
	bfree(e, delid, file, line);

	return (l);
}

/*------------------------------------------------------------------------
 * bdup() - dup a list element
 */
BLIST *bdup (BLIST *l, const char *file, int line)
{
	BLIST *x = 0;
	BLIST *y = 0;
	BLIST *t;

	for (; l; l=bnext(l))
	{
		t = bnew(bid(l), file, line);
		if (t == 0)
			return bscrap(x, FALSE, file, line);

		if (y)
		{
			bprev(t) = y;
			bnext(y) = t;
		}
		else
		{
			x = t;
		}
		y = t;
	}

	return (x);
}

/*------------------------------------------------------------------------
 * bnextc() - get next element in a list, cycling if necessary
 */
BLIST *bnextc (BLIST *l)
{
	if (bnext(l))
	{
		return (bnext(l));
	}
	else
	{
		for (; bprev(l); l=bprev(l))
			;
		return (l);
	}
}

/*------------------------------------------------------------------------
 * bprevc() - get prev element in a list, cycling if necessary
 */
BLIST *bprevc (BLIST *l)
{
	if (bprev(l))
	{
		return (bprev(l));
	}
	else
	{
		for (; bnext(l); l=bnext(l))
			;
		return (l);
	}
}

/*------------------------------------------------------------------------
 * bappend() - append an element to a list
 */
BLIST *bappend (BLIST *l, BLIST *e)
{
	if (l)
	{
		binsert(blast(l), e);
		return (l);
	}
	return (e);
}

/*------------------------------------------------------------------------
 * bapplyf() - apply a function to each element in a list
 */
void bapplyf (BLIST *l, BLIST_FUNC *f, void *a)
{
	for (; l; l=bnext(l))
	{
		(*f)(l, a);
	}
}

/*------------------------------------------------------------------------
 * bcount() - count the elements in a list
 */
int bcount (BLIST *l)
{
	int n = 0;

	for (; l; l=bnext(l))
	{
		n++;
	}

	return (n);
}

/*------------------------------------------------------------------------
 * bfind() - find an element in a list
 */
BLIST *bfind (BLIST *l, void *a)
{
	for (; l; l=bnext(l))
	{
		if (a == bid(l))
			return (l);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * bnth() - get the nth element in a list
 */
BLIST *bnth (BLIST *l, int n)
{
	if (n < 0)
		return (0);

	for (; n; n--)
	{
		l = bnext(l);
		if (l == 0)
			break;
	}

	return (l);
}

/*------------------------------------------------------------------------
 * binsert() - insert a list into a list
 */
BLIST *binsert (BLIST *l, BLIST *e)
{
	BLIST *t;

	if (l == 0)
		return (e);

	if (e)
	{
		t = blast(e);
		bnext(t) = bnext(l);
		bnext(l) = e;
		bprev(e) = l;
		if (bnext(t))
			bprev(bnext(t)) = t;
	}

	return (l);
}

/*------------------------------------------------------------------------
 * binsertf() - insert a list into a list according to a function
 */
BLIST *binsertf (BLIST *l, BLIST *e, BLIST_COMP *f)
{
	BLIST *	t;
	int		rc;

	if (l == 0)
		return (e);

	if (e == 0)
		return (l);

	rc = (*f)(bid(e), bid(l));
	if (rc < 0)
	{
		bprev(e) = 0;
		bnext(e) = l;
		bprev(l) = e;

		return (e);
	}

	for (t=l; bnext(t); t=bnext(t))
	{
		rc = (*f)(bid(e), bid(bnext(t)));
		if (rc < 0)
			break;
	}

	bnext(e) = bnext(t);
	bnext(t) = e;
	bprev(e) = t;
	if (bnext(e))
		bprev(bnext(e)) = e;

	return (l);
}

/*------------------------------------------------------------------------
 * blast() - get the last element in a list
 */
BLIST *blast (BLIST *l)
{
	if (l == 0)
		return (0);

	while (bnext(l))
		l = bnext(l);

	return (l);
}

/*------------------------------------------------------------------------
 * bindex() - get the index of an element in a list
 */
int bindex (BLIST *l, BLIST *e)
{
	int i;

	for (i=0; l; i++, l=bnext(l))
	{
		if (l == e)
			return (i);
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * bremove() - remove an element from a list
 */
BLIST *bremove (BLIST *l, BLIST *e)
{
	BLIST *t;

	if (l == 0 || e == 0)
		return (l);

	if (l==e)
	{
		l = bnext(l);
		if (l)
			bprev(l) = 0;
	}
	else
	{
		t = l;
		while (bnext(t) && bnext(t) != e)
			t = bnext(t);

		if (bnext(t))
		{
			bnext(t) = bnext(e);
			if (bnext(e))
				bprev(bnext(e)) = t;
		}
	}

	e->next = 0;
	e->prev = 0;

	return (l);
}

/*------------------------------------------------------------------------
 * bsort() - sort a list
 *
 *	This version of bsort copies the ids to an array
 *	and uses a ssort or qsort routine to sort the ids,
 *	then copies the ids back into the list.
 */
BLIST *bsort (BLIST *p, BLIST_COMP *f)
{
	void **	blist_array;
	BLIST *	l;
	int		n;
	int		i;

	/*--------------------------------------------------------------------
	 * get number of elements and bail if zero or one
	 */
	n = bcount(p);
	if (n <= 1)
		return (p);

	/*--------------------------------------------------------------------
	 * allocate an array
	 */
	blist_array = (void **)MALLOC(n * sizeof(*blist_array));
	if (blist_array == 0)
	{
		/* if can't get memory, just don't sort */
		return (p);
	}

	/*--------------------------------------------------------------------
	 * copy ids into the array
	 */
	for (l=p, i=0; i<n; l=bnext(l), i++)
		blist_array[i] = bid(l);

	/*--------------------------------------------------------------------
	 * sort it
	 */
	ssort((char *)blist_array, n, sizeof(*blist_array), f);

	/*--------------------------------------------------------------------
	 * copy ids back into list
	 */
	for (l=p, i=0; i<n; l=bnext(l), i++)
		bid(l) = blist_array[i];

	/*--------------------------------------------------------------------
	 * free the array
	 */
	FREE((void *)blist_array);

	return (p);
}
