/*------------------------------------------------------------------------
 * Tree processing routines
 */
#include "utcommon.h"

/*------------------------------------------------------------------------
 * tnew() - create a new tree element
 */
TREE *tnew (void *a, const char *file, int line)
{
	TREE *t;

	t = (TREE *)SYSMEM_MALLOC(sizeof(*t), "tnew", file, line);
	if (t != 0)
	{
		tright(t)  = 0;
		tleft(t)   = 0;
		tparent(t) = 0;
		tid(t)     = a;
	}

	return (t);
}

/*------------------------------------------------------------------------
 * tfree() - free a tree element
 */
TREE *tfree (TREE *t, int delid, const char *file, int line)
{
	if (t)
	{
		if (delid && tid(t) != 0)
			SYSMEM_FREE(tid(t), "tfree", file, line);
		SYSMEM_FREE(t, "tfree", file, line);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * tscrap() - delete an entire tree
 */
TREE * tscrap (TREE *t, int delid, const char *file, int line)
{
	if (t != 0)
	{
		tscrap(tleft(t),  delid, file, line);
		tscrap(tright(t), delid, file, line);

		tfree(t, delid, file, line);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * tdelete[lr]() - delete a tree element from the left/right
 */
TREE *tdeletel (TREE *t, TREE *e, int delid, const char *file, int line)
{
	t = tremovel(t, e);
	tfree(e, delid, file, line);

	return (t);
}

TREE *tdeleter (TREE *t, TREE *e, int delid, const char *file, int line)
{
	t = tremover(t, e);
	tfree(e, delid, file, line);

	return (t);
}

/*------------------------------------------------------------------------
 * troot() - get the root of a tree
 */
TREE * troot (TREE *t)
{
	if (t != 0)
	{
		while (tparent(t))
			t = tparent(t);
	}

	return (t);
}

/*------------------------------------------------------------------------
 * tdepth() - get the depth of an entry in a tree
 */
int tdepth (TREE *t)
{
	int depth = 0;

	if (t != 0)
	{
		while (tparent(t))
		{
			depth++;
			t = tparent(t);
		}
	}

	return (depth);
}

/*------------------------------------------------------------------------
 * tappend[lr]() - append an entry to the left/right of a tree
 */
TREE *tappendl (TREE *t, TREE *e)
{
	if (t)
	{
		tinsertl(tlastl(t), e);
		return (t);
	}

	return (e);
}

TREE *tappendr (TREE *t, TREE *e)
{
	if (t)
	{
		tinsertr(tlastr(t), e);
		return (t);
	}

	return (e);
}

/*------------------------------------------------------------------------
 * tapplyf[lr]() - apply a function to each element to the left/right
 */
void tapplyfl (TREE *t, TREE_FUNC *f, void *a)
{
	for (; t; t=tleft(t))
	{
		(*f)(t, a);
	}
}

void tapplyfr (TREE *t, TREE_FUNC *f, void *a)
{
	for (; t; t=tright(t))
	{
		(*f)(t, a);
	}
}

/*------------------------------------------------------------------------
 * tcount[lr]() - count the elements to the left/right
 */
int tcountl (TREE *t)
{
	int n = 0;

	for (; t; t=tleft(t))
	{
		n++;
	}

	return (n);
}

int tcountr (TREE *t)
{
	int n = 0;

	for (; t; t=tright(t))
	{
		n++;
	}

	return (n);
}

/*------------------------------------------------------------------------
 * tfind[lr]() - find an element to the left/right
 */
TREE *tfindl (TREE *t, void *a)
{
	for (; t; t=tleft(t))
	{
		if (a == tid(t))
			return (t);
	}

	return (0);
}

TREE *tfindr (TREE *t, void *a)
{
	for (; t; t=tright(t))
	{
		if (a == tid(t))
			return (t);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * tnth[lr]() - get the nth element from the left/right
 */
TREE *tnthl (TREE *t, int n)
{
	if (n < 0)
		return (0);

	for (; n; n--)
	{
		t = tleft(t);
		if (t == 0)
			return (0);
	}

	return (t);
}

TREE *tnthr (TREE *t, int n)
{
	if (n < 0)
		return (0);

	for (; n; n--)
	{
		t = tright(t);
		if (t == 0)
			return (0);
	}

	return (t);
}

/*------------------------------------------------------------------------
 * tinsert[lr]() - insert an element to the left/right
 */
TREE *tinsertl (TREE *t, TREE *e)
{
	if (t == 0)
		return (e);

	if (e)
	{
		TREE *te;

		te        = tlastl(e);
		tleft(te) = tleft(t);
		tleft(t)  = e;
	}

	return (t);
}

TREE *tinsertr (TREE *t, TREE *e)
{
	if (t == 0)
		return (e);

	if (e)
	{
		TREE *te;

		te         = tlastr(e);
		tright(te) = tright(t);
		tright(t)  = e;
	}

	return (t);
}

/*------------------------------------------------------------------------
 * tinsert[lr]f() - insert an element to the left/right according to a function
 */
TREE *tinsertlf (TREE *t, TREE *e, TREE_COMP *f)
{
	TREE *	p;
	int		rc;

	if (t == 0)
		return (e);

	if (e == 0)
		return (t);

	rc = (*f)(e, t);
	if (rc < 0)
	{
		tleft(e) = t;

		return (e);
	}

	for (p=t; tleft(p); p=tleft(p))
	{
		rc = (*f)(e, tleft(p));
		if (rc < 0)
			break;
	}

	tleft(e) = tleft(p);
	tleft(p) = e;

	return (t);
}

TREE *tinsertrf (TREE *t, TREE *e, TREE_COMP *f)
{
	TREE *	p;
	int		rc;

	if (t == 0)
		return (e);

	if (e == 0)
		return (t);

	rc = (*f)(e, t);
	if (rc < 0)
	{
		tright(e) = t;

		return (e);
	}

	for (p=t; tright(p); p=tright(p))
	{
		rc = (*f)(e, tright(p));
		if (rc < 0)
			break;
	}

	tright(e) = tright(p);
	tright(p) = e;

	return (t);
}

/*------------------------------------------------------------------------
 * tlast[lr]() - get the last element to the left/right
 */
TREE *tlastl (TREE *t)
{
	TREE *e = t;

	while (t)
	{
		e = t;
		t = tleft(t);
	}

	return (e);
}

TREE *tlastr (TREE *t)
{
	TREE *e = t;

	while (t)
	{
		e = t;
		t = tright(t);
	}

	return (e);
}

/*------------------------------------------------------------------------
 * tindex[lr]() - get the index of an element to the left/right
 */
int tindexl (TREE *t, TREE *e)
{
	int i;

	for (i=0; t; i++, t=tleft(t))
	{
		if (t == e)
			return (i);
	}

	return (-1);
}

int tindexr (TREE *t, TREE *e)
{
	int i;

	for (i=0; t; i++, t=tright(t))
	{
		if (t == e)
			return (i);
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * tremove[lr]() - remove an element from the left/right
 */
TREE *tremovel (TREE *t, TREE *e)
{
	if (t == 0 || e == 0)
		return (t);

	if (t == e)
	{
		t = tleft(t);
	}
	else
	{
		TREE *te = t;

		while (tleft(te) && tleft(te) != e)
			te = tleft(te);

		if (tleft(te))
			tleft(te) = tleft(e);
	}
	tleft(e) = 0;

	return (t);
}

TREE *tremover (TREE *t, TREE *e)
{
	if (t == 0 || e == 0)
		return (t);

	if (t == e)
	{
		t = tright(t);
	}
	else
	{
		TREE *te = t;

		while (tright(te) && tright(te) != e)
			te = tright(te);

		if (tright(te))
			tright(te) = tright(e);
	}
	tright(e) = 0;

	return (t);
}

/*------------------------------------------------------------------------
 * tpreorder(), tendorder(), tpostorder() - apply a function to a tree
 *
 *	tpreorder()		- do entry, go left,  go right
 *	tendorder()		- go left,  do entry, go right
 *	tpostorder()	- go left,  go right, do entry
 */
void tpreorder (TREE *t, TREE_FUNC *f, void *a)
{
	if (t)
	{
		(*f)(t, a);
		tpreorder(tleft(t),  f, a);
		tpreorder(tright(t), f, a);
	}
}

void tendorder (TREE *t, TREE_FUNC *f, void *a)
{
	if (t)
	{
		tendorder(tleft(t),  f, a);
		tendorder(tright(t), f, a);
		(*f)(t, a);
	}
}

void tpostorder (TREE *t, TREE_FUNC *f, void *a)
{
	if (t)
	{
		tpostorder(tleft(t),  f, a);
		(*f)(t, a);
		tpostorder(tright(t), f, a);
	}
}

/*------------------------------------------------------------------------
 * tsortr() - sort the right pointer list of a tree
 *
 *	This version of tsortr copies the ids to an array
 *	and uses a ssort or qsort routine to sort the ids,
 *	then copies the ids back into the list.
 */
TREE *tsortr (TREE *p, TREE_COMP *f)
{
	TREE *	tree_array;
	TREE *	t;
	TREE *	a;
	int		n;

	/*--------------------------------------------------------------------
	 * get number of elements and bail if zero or one
	 */
	n = tcountr(p);
	if (n <= 1)
		return (p);

	/*--------------------------------------------------------------------
	 * allocate array
	 */
	tree_array = (TREE *)MALLOC(n * sizeof(*tree_array));
	if (tree_array == 0)
	{
		/* if can't get memory, just don't sort */
		return (p);
	}

	/*--------------------------------------------------------------------
	 * copy trees into the array
	 */
	a = tree_array;
	for (t=p; t; t=t->right)
	{
		tparent(a) = tparent(t);
		tleft(a)   = tleft(t);
		tid(a)     = tid(t);

		a++;
	}

	/*--------------------------------------------------------------------
	 * sort it
	 */
	ssort((char *)tree_array, n, sizeof(*tree_array), f);

	/*--------------------------------------------------------------------
	 * copy ids back into list
	 */
	a = tree_array;
	for (t=p; t; t=t->right)
	{
		tparent(t) = tparent(a);
		tleft(t)   = tleft(a);
		tid(t)     = tid(a);

		a++;
	}

	/*--------------------------------------------------------------------
	 * free the array
	 */
	FREE(tree_array);

	return (p);
}
