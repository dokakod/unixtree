/*------------------------------------------------------------------------
 * low-level routines to read resource-file headers
 */
#include <stdio.h>
#include <string.h>
#include "curses.h"
#include "libsys.h"
#include "res.h"

int res_hdr_read (FILE *fp, RES_HDR *res_hdr)
{
	res_hdr->magic		= get_4byte(fp);
	res_hdr->num_ents	= get_4byte(fp);
	fread(res_hdr->name, sizeof(res_hdr->name), 1, fp);
	fread(res_hdr->file, sizeof(res_hdr->file), 1, fp);

	if (res_hdr->magic != RES_MAGIC)
		return (-1);

	return (0);
}

int res_entry_read (FILE *fp, RES_ENTRY_DATA *res_data, int load_names)
{
	RES_ENTRY_HDR entry_hdr;
	void *array;
	int i;
	int l;

	entry_hdr.num	= get_4byte(fp);
	entry_hdr.size	= get_4byte(fp);
	entry_hdr.type	= get_4byte(fp);
	i = fread(entry_hdr.name, sizeof(entry_hdr.name), 1, fp);
	if (i != 1)
	{
		return (-1);
	}

	res_data->num   = entry_hdr.num;
	res_data->type  = entry_hdr.type;
	strcpy(res_data->name, entry_hdr.name);

	if (entry_hdr.type == R_TYPE_STR)
	{
		char *		str_ptr;
		char **		char_list;
		RES_ITEM *	item_list;

		if (load_names)
			l = (entry_hdr.num * sizeof(RES_ITEM));
		else
			l = (entry_hdr.num * sizeof(char *));
		array = MALLOC(l + entry_hdr.size);
		if (array == 0)
		{
			return (-1);
		}

		str_ptr = (char *)array + l;
		if (load_names)
			item_list = (RES_ITEM *)array;
		else
			char_list = (char **)array;

		for (i=0; i<(int)entry_hdr.num; i++)
		{
			RES_ITEM	item;

			fread(item.name, sizeof(item.name), 1, fp);
			item.data = (void *)get_4byte(fp);

			if (load_names)
			{
				memcpy(item_list + i, &item, sizeof(*item_list));
				item_list[i].data = str_ptr + (int)item.data;
			}
			else
			{
				char_list[i] = str_ptr + (int)item.data;
			}
		}

		i = fread(str_ptr, (int)entry_hdr.size, 1, fp);
		if (i != 1)
		{
			FREE(array);
			return (-1);
		}

		res_data->array = array;
	}
	else
	{
		int *		word_list;
		RES_ITEM *	item_list;

		if (load_names)
			l = (entry_hdr.num * sizeof(RES_ITEM));
		else
			l = (entry_hdr.num * sizeof(int));
		array = MALLOC(l);
		if (array == 0)
		{
			return (-1);
		}

		if (load_names)
			item_list = (RES_ITEM *)array;
		else
			word_list = (int *)array;

		for (i=0; i<(int)entry_hdr.num; i++)
		{
			RES_ITEM	item;

			fread(item.name, sizeof(item.name), 1, fp);
			item.data = (void *)get_4byte(fp);

			if (load_names)
			{
				memcpy(item_list + i, &item, sizeof(*item_list));
			}
			else
			{
				word_list[i] = (int)item.data;
			}
		}

		res_data->array = array;
	}

	return (0);
}
