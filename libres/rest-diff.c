/*------------------------------------------------------------------------
 * diff 2 resource files
 */
#include "restool.h"

/*------------------------------------------------------------------------
 * diff 2 resource entries
 */
static int res_diff_entries (RES_ENTRY_DATA *re_this, RES_ENTRY_DATA *re_orig)
{
	char *	entry_name	= re_this->name;
	int		num_this	= re_this->num;
	int		num_orig	= re_orig->num;
	int		i;
	int		rc;

	/*--------------------------------------------------------------------
	 * compare number of entries
	 *
	 * Note that we already know that the name & type of both MUST be the same.
	 */
	if (num_this != num_orig)
	{
		printf("%-8s: Number of entries different (%d / %d)\n",
			entry_name, num_this, num_orig);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * compare names of entries
	 */
	rc = 0;
	for (i = 0; i < num_this; i++)
	{
		RES_ITEM *	ri_this	= (RES_ITEM *)re_this->array + i;
		RES_ITEM *	ri_orig	= (RES_ITEM *)re_orig->array + i;

		if (strcmp(ri_this->name, ri_orig->name) != 0)
		{
			printf("%-8s: entry %3d different (\"%s\" / \"%s\")\n",
				entry_name, i, ri_this->name, ri_orig->name);

			rc = -1;
		}
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * diff 2 resource structs
 */
static int res_diff_structs (RES_FILE *rf_this, RES_FILE *rf_orig)
{
	int rc = 0;
	int i;

	/*--------------------------------------------------------------------
	 * diff each entry
	 */
	for (i = 0; i < rf_this->hdr.num_ents; i++)
	{
		if ( res_diff_entries(rf_this->entries + i, rf_orig->entries + i) )
			rc = -1;
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * diff 2 resource files
 */
int res_diff (const char *name, const char *orig, char *msgbuf)
{
	char		res_path[256];
	RES_FILE *	rf_this;
	RES_FILE *	rf_orig;
	int			rc;

	/*--------------------------------------------------------------------
	 * load the resource file to diff
	 */
	sprintf(res_path, "%s.res", name);

	rf_this = res_file_load(res_path, 1);
	if (rf_this == 0)
	{
		sprintf(msgbuf, "Cannot load resource file %s", res_path);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * load the resource file to diff against
	 */
	sprintf(res_path, "%s.res", orig);

	rf_orig = res_file_load(res_path, 1);
	if (rf_orig == 0)
	{
		res_file_free(rf_this);
		sprintf(msgbuf, "Cannot load resource file %s", res_path);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * now diff them
	 */
	rc = res_diff_structs(rf_this, rf_orig);
	if (rc)
	{
		sprintf(msgbuf, "resource files differ");
	}

	/*--------------------------------------------------------------------
	 * free the structs
	 */
	res_file_free(rf_this);
	res_file_free(rf_orig);

	return (rc);
}
