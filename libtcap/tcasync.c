/*------------------------------------------------------------------------
 * async key routines
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * tcap_async_add() - add a key to the async-key table
 */
int tcap_async_add (SCRN_DATA *sd, int key, ASYNC_RTN *rtn, void *data)
{
	ASKEY *a;
	ASKEY *p = 0;

	/*--------------------------------------------------------------------
	 * check if already in list
	 */
	for (a=sd->askey_tbl; a; a=a->next)
	{
		if (a->key == key)
			break;

		p = a;
	}

	/*--------------------------------------------------------------------
	 * if not in list and no rtn - return
	 */
	if (a == 0 && rtn == 0)
		return (0);

	/*--------------------------------------------------------------------
	 * if already in list, just store new rtn
	 */
	if (a)
	{
		if (rtn != 0)
		{
			/*------------------------------------------------------------
			 * just store new rtn & data
			 */
			a->rtn	= rtn;
			a->data	= data;
		}
		else
		{
			/*------------------------------------------------------------
			 * delete entry
			 */
			if (p == 0)
				sd->askey_tbl = a->next;
			else
				p->next = a->next;

			FREE(a);
		}

		return (0);
	}

	/*--------------------------------------------------------------------
	 * create new entry
	 */
	a = (ASKEY *)MALLOC(sizeof(*a));
	if (a == 0)
		return (-1);

	a->next = 0;
	a->key  = key;
	a->rtn  = rtn;
	a->data	= data;

	/*--------------------------------------------------------------------
	 * add entry to list
	 */
	if (p == 0)
		sd->askey_tbl = a;
	else
		p->next = a;

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_async_check() - check if a key has an async entry in the table
 */
int tcap_async_check (const SCRN_DATA *sd, int key)
{
	const ASKEY *a;

	/*--------------------------------------------------------------------
	 * find key entry in table
	 */
	for (a=sd->askey_tbl; a; a=a->next)
	{
		if (a->key == key && a->rtn != 0)
			return (TRUE);
	}

	return (FALSE);
}

/*------------------------------------------------------------------------
 * tcap_async_process() - invoke the async routine associated with a key
 */
int tcap_async_process (const SCRN_DATA *sd, int key)
{
	const ASKEY *a;

	/*--------------------------------------------------------------------
	 * find key entry in table
	 */
	for (a=sd->askey_tbl; a; a=a->next)
	{
		if (a->key == key)
		{
			if (a->rtn == (ASYNC_RTN *)-1 || a->rtn == 0)
				return (0);

			return (*a->rtn)(key, a->data);
		}
	}

	return (key);
}

/*------------------------------------------------------------------------
 * tcap_async_free() - free an async-key table
 */
void tcap_async_free (SCRN_DATA *sd)
{
	ASKEY *a;
	ASKEY *n;

	for (a=sd->askey_tbl; a; a=n)
	{
		n = a->next;
		FREE(a);
	}

	sd->askey_tbl = 0;
}
