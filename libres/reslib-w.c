/*------------------------------------------------------------------------
 * low-level routines to write resource-file headers
 */
#include <stdio.h>
#include <string.h>
#include "curses.h"
#include "libsys.h"
#include "res.h"

int res_hdr_write (FILE *fp, const RES_HDR *res_hdr)
{
	put_4byte(fp, res_hdr->magic);
	put_4byte(fp, res_hdr->num_ents);

	fwrite(res_hdr->name, sizeof(res_hdr->name), 1, fp);
	fwrite(res_hdr->file, sizeof(res_hdr->file), 1, fp);

	return (0);
}

int res_entry_write (FILE *fp, const RES_ENTRY_DATA *res_data)
{
	RES_ENTRY_HDR entry_hdr;
	const RES_ITEM *item_ptr;
	char *p;
	int v;
	int num;
	int type;
	int pos;
	int size;
	int	sum;
	int i;

	num  = res_data->num;
	type = res_data->type;

	memset(&entry_hdr, 0, sizeof(entry_hdr));

	entry_hdr.num  = num;
	entry_hdr.size = 0;
	entry_hdr.type = type;
	strcpy(entry_hdr.name, res_data->name);

	item_ptr = (RES_ITEM *)res_data->array;

	if (type == R_TYPE_STR)
	{
		size = 0;
		for (i=0; i<(int)entry_hdr.num; i++)
		{
			size += strlen((char *)item_ptr[i].data) + 1;
		}

		size = (size + 3) & ~3;
		entry_hdr.size = size;
	}

	put_4byte(fp, entry_hdr.num);
	put_4byte(fp, entry_hdr.size);
	put_4byte(fp, entry_hdr.type);
	fwrite(entry_hdr.name, sizeof(entry_hdr.name), 1, fp);

	pos = 0;
	for (i=0; i<num; i++)
	{
		fwrite(item_ptr[i].name, sizeof(item_ptr[i].name), 1, fp);

		if (type == R_TYPE_STR)
		{
			put_4byte(fp, pos);
			pos += strlen((char *)item_ptr[i].data) + 1;
		}
		else
		{
			v = ((int)item_ptr[i].data);
			put_4byte(fp, v);
		}
	}

	if (type == R_TYPE_STR)
	{
		sum = 0;
		for (i=0; i<num; i++)
		{
			p = (char *)item_ptr[i].data;
			v = strlen(p) + 1;
			fwrite(p, v, 1, fp);
			sum += v;
		}

		for (; sum < size; sum++)
			fputc(0, fp);
	}

	return (0);
}
