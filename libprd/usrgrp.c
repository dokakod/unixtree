/*------------------------------------------------------------------------
 * user/group routines
 */
#include "libprd.h"

static BLIST *add_ug_ent (BLIST *list, int ug_id, const char *name)
{
	BLIST *l;
	USR_GRP *u;
	int i;
	int	max_chars = (UG_LEN - 1);

	u = (USR_GRP *)MALLOC(sizeof(*u));
	if (u == 0)
		return (list);

	u->ug_id = ug_id;
	if (name)
	{
		for (i=0; name[i] && i<max_chars; i++)
			u->ug_name[i] = name[i];
		for (; i<max_chars; i++)
			u->ug_name[i] = ' ';
		u->ug_name[i] = 0;
	}
	else
	{
		sprintf(u->ug_name, "%-*.*d", UG_LEN-1, UG_LEN-1, ug_id);
	}

	l = BNEW(u);
	if (l == 0)
	{
		FREE(u);
		return (list);
	}

	list = bappend(list, l);
	return (list);
}

char *get_owner (int uid, char *buf)
{
	BLIST *l;
	USR_GRP *u;

	for (l=gbl(owner_list); l; l=bnext(l))
	{
		u = (USR_GRP *)bid(l);
		if (u->ug_id == uid)
			return (u->ug_name);
	}

	sprintf(buf, "%-*.*d", UG_LEN-1, UG_LEN-1, uid);
	return (buf);
}

char *get_group (int gid, char *buf)
{
	BLIST *l;
	USR_GRP *u;

	for (l=gbl(group_list); l; l=bnext(l))
	{
		u = (USR_GRP *)bid(l);
		if (u->ug_id == gid)
			return (u->ug_name);
	}

	sprintf(buf, "%-*.*d", UG_LEN-1, UG_LEN-1, gid);
	return (buf);
}

void check_owner (int uid)
{
	BLIST *l;
	USR_GRP *u;
	char *name;
	char name_buf[128];

	/* Check if already in list */

	for (l=gbl(owner_list); l; l=bnext(l))
	{
		u = (USR_GRP *)bid(l);
		if (u->ug_id == uid)
			return;
	}

	/* Get password table entry, if any */

	if (uid == -1)
	{
		name = msgs(m_userid_noown);
	}
	else
	{
		name = os_get_usr_name_from_id(uid, name_buf);
	}

	gbl(owner_list) = add_ug_ent(gbl(owner_list), uid, name);
}

void check_group (int gid)
{
	BLIST *l;
	USR_GRP *u;
	char *name;
	char name_buf[128];

	/* Check if already in list */

	for (l=gbl(group_list); l; l=bnext(l))
	{
		u = (USR_GRP *)bid(l);
		if (u->ug_id == gid)
			return;
	}

	/* Get group table entry, if any */

	if (gid == -1)
	{
		name = msgs(m_userid_nogrp);
	}
	else
	{
		name = os_get_grp_name_from_id(gid, name_buf);
	}

	gbl(group_list) = add_ug_ent(gbl(group_list), gid, name);
}

int owner_len (void)
{
	return (UG_LEN - 1);
}

int group_len (void)
{
	return (UG_LEN - 1);
}

BLIST * owner_free (BLIST *owners)
{
	return BSCRAP(owners, TRUE);
}

BLIST * group_free (BLIST *groups)
{
	return BSCRAP(groups, TRUE);
}

