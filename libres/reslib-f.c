/*------------------------------------------------------------------------
 *  routines to load/save/free a resource-file
 */
#include <stdio.h>
#include <string.h>
#include "curses.h"
#include "libsys.h"
#include "res.h"

RES_FILE *res_file_load (const char *res_path, int load_names)
{
	RES_FILE *rf;
	RES_ENTRY_DATA *entries;
	FILE *fp;
	int i;
	int rc;

	if (res_path == 0 || *res_path == 0)
		return (0);

	fp = fopen(res_path, "rb");
	if (fp == 0)
		return (0);

	rf = (RES_FILE *)MALLOC(sizeof(*rf));
	if (rf == 0)
	{
		fclose(fp);
		return (0);
	}

	rc = res_hdr_read(fp, &rf->hdr);
	if (rc)
	{
		FREE(rf);
		fclose(fp);
		return (0);
	}

	entries = (RES_ENTRY_DATA *)MALLOC(rf->hdr.num_ents * sizeof(*entries));
	if (entries == 0)
	{
		FREE(rf);
		fclose(fp);
		return (0);
	}
	memset(entries, 0, rf->hdr.num_ents * sizeof(*entries));

	rf->entries = entries;

	for (i=0; i<rf->hdr.num_ents; i++)
	{
		rc = res_entry_read(fp, entries + i, load_names);
		if (rc)
		{
			res_file_free(rf);
			fclose(fp);
			return (0);
		}
	}

	fclose(fp);

	rf->loaded		= TRUE;
	rf->has_names	= load_names;

	return (rf);
}

int res_file_save (const char *res_path, const RES_FILE *rf)
{
	FILE *fp;
	int i;

	if (res_path == 0 || *res_path == 0 || rf == 0)
		return (-1);

	fp = fopen(res_path, "wb");
	if (fp == 0)
		return (-1);

	res_hdr_write(fp, &rf->hdr);
	for (i=0; i<rf->hdr.num_ents; i++)
		res_entry_write(fp, rf->entries + i);

	fclose(fp);
	return (0);
}

void res_file_free (RES_FILE *rf)
{
	int i;

	if (rf == 0 || rf->loaded == 0)
		return;

	for (i=0; i<rf->hdr.num_ents; i++)
	{
		if (rf->entries[i].array != 0)
			FREE(rf->entries[i].array);
	}

	FREE(rf->entries);
	FREE(rf);
}
