/*------------------------------------------------------------------------
 * function key routines
 *
 * When processing the termcap/terminfo database and any term files,
 * we create an array of KFUNC entries (key-value/string pairs).
 *
 * After all procesing is done, we create a tree of FKEY entries to
 * speed up the search for a function-key.
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * tcap_fk_new() - internal routine to create a new FK entry & add it to
 * the existing tree
 */
static FKEY *tcap_fk_new (TCAP_DATA *td)
{
	FKEY *f;

	f = (FKEY *)MALLOC(sizeof(*f));
	if (f != 0)
	{
		f->fk_char		= 0;
		f->fk_code		= 0;
		f->fk_sib		= 0;
		f->fk_child		= 0;
		f->fk_parent	= 0;
		f->fk_prev		= td->fks_last;	/* this makes a backward chain */

		td->fks_last	= f;
	}

	return (f);
}

/*------------------------------------------------------------------------
 * tcap_kf_new() - internal routine to create a new KFUNC entry
 */
KFUNC *tcap_kf_new (void)
{
	KFUNC *	k;

	k = (KFUNC *)MALLOC(sizeof(*k));
	if (k != 0)
	{
		k->kf_next	= 0;
		k->kf_prev	= 0;
		k->kf_code	= 0;
		k->kf_str	= 0;
	}

	return (k);
}

/*------------------------------------------------------------------------
 * tcap_fk_add_dumb_keys() - add pre-defined key entries to FKEY tree
 */
int tcap_fk_add_dumb_keys (TCAP_DATA *td)
{
	int i;
	int	rc = 0;

	/*	dumb terminal definitions anyone can use */

	for (i=0; tcap_dumb_keys[i].kf_code; i++)
	{
		int trc;

		trc = tcap_fk_addkey(td, tcap_dumb_keys[i].kf_str,
			tcap_dumb_keys[i].kf_code, TRUE);

		if (trc)
			rc = -1;
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_fk_addkey() - convert a key-value/string to an FKEY entry and
 * add it to the tree.
 */
int tcap_fk_addkey (TCAP_DATA *td, const char *string, int code, int debug)
{
	FKEY *f;
	const unsigned char *s;
	int c;

	/*--------------------------------------------------------------------
	 * create an empty entry in the tree if it is empty
	 */
	if (td->fks_start == 0)
		td->fks_start = tcap_fk_new(td);

	/*--------------------------------------------------------------------
	 * check if this entry is already in the tree or is part of
	 * an existing entry
	 */
	f = td->fks_start;
	for (s = (const unsigned char *)string; *s; s++)
	{
		c = *s;
		while (f)
		{
			if (f->fk_char == 0)
			{
				if (f->fk_code)
				{
					if (debug)
					{
						char buf1[64];
						char buf2[64];

						tcap_errmsg_add_fmt(td->errs,
							TCAP_ERR_KEY_CONT, TCAP_ERR_L_FATAL,
							"Key definition for \"%s\" contains \"%s\"",
							tcap_get_key_name(code, buf1),
							tcap_get_key_name(f->fk_code, buf2));
					}

					return (-1);
				}
				else
				{
					f->fk_char = c;
					f->fk_child = tcap_fk_new(td);
					(f->fk_child)->fk_parent = f;
					f = f->fk_child;
				}
				break;
			}
			else if (f->fk_char == c)
			{
				if (f->fk_child == 0)
				{
					f->fk_child = tcap_fk_new(td);
					(f->fk_child)->fk_parent = f;
				}
				f = f->fk_child;
				break;
			}
			else /* if (f->fk_char != c) */
			{
				if (f->fk_sib == 0)
				{
					f->fk_sib = tcap_fk_new(td);
					(f->fk_sib)->fk_parent = f->fk_parent;
				}
				f = f->fk_sib;
			}
		}
	}

	/*--------------------------------------------------------------------
	 * if we have an entry here, check if partial or a duplicate
	 */
	if (f)
	{
		if (f->fk_char)
		{
			/* Partial string <key-name> */
			if (debug)
			{
				char buf[64];

				tcap_errmsg_add_fmt(td->errs,
					TCAP_ERR_KEY_PART, TCAP_ERR_L_FATAL,
					"Partial string in key definition \"%s\"",
					tcap_get_key_name(code, buf));
			}

			return (-1);
		}
		else
		{
#if 0
			if (f->fk_code)
			{
				/* Note: we always replace dup codes with new code */

				if (f->fk_code != code)
				{
					if (debug)
					{
						char buf1[64];
						char buf2[64];

						tcap_errmsg_add_fmt(td->errs,
							TCAP_ERR_KEY_DUP, TCAP_ERR_L_WARNING,
							"Key definition \"%s\" duplicates \"%s\"",
							tcap_get_key_name(code, buf1),
							tcap_get_key_name(f->fk_code, buf2));
					}
				}
            }
#endif
			f->fk_code = code;
		}
	}

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_kf_addkey() - add a KFUNC entry for a key/string pair
 */
int tcap_kf_addkey (TCAP_DATA *td, const char *str, int code)
{
	KFUNC *	k;
	char *	s;

	/*--------------------------------------------------------------------
	 * bail if key or string is empty
	 */
	if (str == 0 || code == 0)
		return (0);

	/*--------------------------------------------------------------------
	 * check if already in table
	 */
	k = td->kfuncs;
	for (k=td->kfuncs; k; k=k->kf_next)
	{
		if (k->kf_code == code && strcmp(k->kf_str, str) == 0)
		{
			return (0);
		}
	}

	/*--------------------------------------------------------------------
	 * allocate new entry & fill it in
	 */
	k = tcap_kf_new();
	if (k == 0)
	{
		char buf[16];

		tcap_errmsg_add_fmt(td->errs,
			TCAP_ERR_NOMEM, TCAP_ERR_L_FATAL,
			"No memory for key entry \"%s\"",
			tcap_get_key_name(code, buf));
		return (-1);
	}

	s = tcap_trm_dup_str(str);
	if (s == 0)
	{
		char buf[16];

		FREE(k);
		tcap_errmsg_add_fmt(td->errs,
			TCAP_ERR_NOMEM, TCAP_ERR_L_FATAL,
			"No memory for key entry \"%s\"",
			tcap_get_key_name(code, buf));
		return (-1);
	}

	k->kf_code = code;
	k->kf_str  = s;

	/*--------------------------------------------------------------------
	 * add this entry to the KFUNC list
	 */
	if (td->kfuncs == 0)
	{
		td->kfuncs = k;
	}
	else
	{
		KFUNC *	t;

		for (t=td->kfuncs; t->kf_next; t=t->kf_next)
			;
		k->kf_prev = t;
		t->kf_next = k;
	}

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_kf_delkey() - delete all entries in the KFUNC list with a given code
 */
int tcap_kf_delkey (TCAP_DATA *td, int code)
{
	KFUNC *	k;
	KFUNC *	n;

	for (k=td->kfuncs; k; k=n)
	{
		n = k->kf_next;

		if (k->kf_code == code)
		{
			if (n != 0)
				n->kf_prev = k->kf_prev;

			if (k->kf_prev == 0)
				td->kfuncs = n;
			else
				(k->kf_prev)->kf_next = n;

			FREE(k->kf_str);
			FREE(k);
		}
	}

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_fk_setup() - build a search tree of function key string characters
 * to speed search of keys
 */
int tcap_fk_setup (TCAP_DATA *td)
{
	KFUNC *	k;
	int		rc = 0;

	for (k=td->kfuncs; k; k=k->kf_next)
	{
		int trc;

		if (k->kf_str != 0)
		{
			trc = tcap_fk_addkey(td, k->kf_str, k->kf_code, TRUE);
			if (trc)
				rc = -1;
		}
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_fk_find() - search the input string for a match with the function
 * key strings.
 *
 * returns -1 maybe
 *          0 no
 *         >0 key value
 */
int tcap_fk_find (const TCAP_DATA *td, const unsigned char *s)
{
	const unsigned char *p;
	unsigned char q;
	int	maybe;
	const FKEY *f;

	maybe = 0;
	f = td->fks_start;
	if (f == 0)
		return (0);

	p = s;
	q = f->fk_char;
	while (TRUE)
	{
		if (!q)
		{
			maybe = f->fk_code;		/* found! */
			break;
		}
		if (*p == 0)
		{
			maybe = -1;
			break;
		}
		if (*p == q)
		{
			p++;
			f = f->fk_child;
		}
		else
		{
			f = f->fk_sib;
		}
		if (f == 0)
			break;
		q = f->fk_char;
	}
	return (maybe);
}

/*------------------------------------------------------------------------
 * tcap_fk_free() - free the fks (function key search) tree
 */
void tcap_fk_free (TCAP_DATA *td)
{
	if (td != 0)
	{
		while (td->fks_last)
		{
			FKEY *f;

			f = td->fks_last->fk_prev;
			FREE(td->fks_last);
			td->fks_last = f;
		}

		td->fks_start = 0;
	}
}

/*------------------------------------------------------------------------
 * tcap_kf_free() - free the KFUNC array
 */
void tcap_kf_free (TCAP_DATA *td)
{
	if (td != 0)
	{
		KFUNC *	k;
		KFUNC *	n;

		for (k=td->kfuncs; k; k=n)
		{
			n = k->kf_next;

			FREE(k->kf_str);
			FREE(k);
		}

		td->kfuncs		= 0;
	}
}
