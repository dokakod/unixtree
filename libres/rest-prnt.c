/*------------------------------------------------------------------------
 * print a resource file section
 */
#include "restool.h"

/*------------------------------------------------------------------------
 * print a single entry
 */
static void res_print_entry (RES_ENTRY_DATA *res_entry, int num, int show_name)
{
	RES_ITEM *	ri = (RES_ITEM *)res_entry->array + num;
	char	buf[64];

	if (show_name)
		printf("%-32s ", ri->name);

	switch (res_entry->type)
	{
	case R_TYPE_STR:
		printf("%s\n", (char *)ri->data);
		break;

	case R_TYPE_NUM:
		printf("%d\n", (int)ri->data);
		break;

	case R_TYPE_KEY:
		printf("%s\n", get_key_name((int)ri->data, buf));
		break;
	}
}

/*------------------------------------------------------------------------
 * print a resource file section
 */
int res_print (const char *name, const char *section, char *msgbuf)
{
	char				res_path[256];
	char				sect[256];
	char				item[256];
	const char *		s;
	char *				t;
	RES_FILE *			rf;
	RES_ENTRY_DATA *	re;
	int					num;
	int					i;

	/*--------------------------------------------------------------------
	 * get section & num
	 */
	t = sect;
	for (s = section; *s; s++)
	{
		if (*s == ':')
			break;
		*t++ = *s;
	}
	*t = 0;

	if (*s == 0)
		*item = 0;
	else
		strcpy(item, s+1);

	/*--------------------------------------------------------------------
	 * load the resource file
	 */
	sprintf(res_path, "%s.res", name);

	rf = res_file_load(res_path, TRUE);
	if (rf == 0)
	{
		sprintf(msgbuf, "Cannot load resource file %s", res_path);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * find section wanted
	 */
	for (i=0; i<rf->hdr.num_ents; i++)
	{
		re = rf->entries + i;

		if (strcmp(sect, re->name) == 0)
			break;
	}

	if (i == rf->hdr.num_ents)
	{
		sprintf(msgbuf, "Invalid section name %s", sect);
		res_file_free(rf);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * check if a particular number is wanted or all
	 */
	if (*item != 0)
	{
		if (isdigit(*item))
		{
			num = atoi(item);

			if (num <= 0 || num >= re->num)
			{
				sprintf(msgbuf, "Invalid entry number %d", num);
				res_file_free(rf);
				return (-1);
			}
		}
		else
		{
			RES_ITEM *item_list = (RES_ITEM *)re->array;

			for (num = 0; num < re->num; num++)
			{
				if (strcmp(item, item_list[num].name) == 0)
					break;
			}

			if (num >= re->num)
			{
				sprintf(msgbuf, "Invalid entry name \"%s\"", item);
				res_file_free(rf);
				return (-1);
			}
		}

		res_print_entry(re, num, FALSE);
	}
	else
	{
		for (i = 0; i < re->num; i++)
			res_print_entry(re, i, TRUE);
	}

	/*--------------------------------------------------------------------
	 * free the resource file
	 */
	res_file_free(rf);

	return (0);
}
