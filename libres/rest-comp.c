/*------------------------------------------------------------------------
 * program to compile a resource file
 */
#include "restool.h"

/*------------------------------------------------------------------------
 * process a text file line data as key data
 */
static int res_compile_key_process (char *data, RES_ITEM *ri, int num,
	char *msgbuf)
{
	int		entry;

	if (isalpha(*data))
	{
		entry = get_key_value(data);
		if (entry == -1)
		{
			sprintf(msgbuf, "Invalid key name \"%s\"", data);
			return (-1);
		}
	}
	else if (isdigit(*data))
	{
		entry = atoi(data);
	}
	else
	{
		char *d = data;

		if (*d != '\'')
		{
			sprintf(msgbuf, "Invalid key data \"%s\"", data);
			return (-1);
		}

		if (*++d == '\\')
			d++;

		if (d[1] != '\'')
		{
			sprintf(msgbuf, "Invalid key data \"%s\"", data);
			return (-1);
		}

		entry = *d;
	}

	ri->data = (void *)entry;

	return (0);
}

/*------------------------------------------------------------------------
 * process a text file line data as num data
 */
static int res_compile_num_process (char *data, RES_ITEM *ri, int num,
	char *msgbuf)
{
	int		entry;

	if (isdigit(*data))
	{
		entry = atoi(data);
	}
	else
	{
		char *d = data;

		if (*d != '\'')
		{
			sprintf(msgbuf, "Invalid numeric data \"%s\"", data);
			return (-1);
		}

		if (*++d == '\\')
			d++;

		if (d[1] != '\'')
		{
			sprintf(msgbuf, "Invalid numeric data \"%s\"", data);
			return (-1);
		}

		entry = *d;
	}

	ri->data = (void *)entry;

	return (0);
}

/*------------------------------------------------------------------------
 * process a text file line data as str data
 */
static int res_compile_str_process (char *data, RES_ITEM *ri, int num,
	char *msgbuf)
{
	char *	entry;
	char *	s;
	char *	t;
	int		l;

	l = strlen(data) - 1;

	if (data[0] != '"' || data[l] != '"')
	{
		sprintf(msgbuf, "Invalid string data \"%s\"", data);
		return (-1);
	}

	data[l] = 0;
	data++;

	t = data;
	for (s=data; *s; s++)
	{
		if (*s == '\\')
			s++;
		*t++ = *s;
	}
	*t = 0;

	l = strlen(data);
	entry = (char *)MALLOC(l+1);
	if (entry == 0)
	{
		sprintf(msgbuf, "Cannot allocate string entry \"%s\"", data);
		return (-1);
	}

	strcpy(entry, data);
	ri->data = (void *)entry;

	return (0);
}

/*------------------------------------------------------------------------
 * process a text file line
 */
static int res_compile_line_process (char *line, RES_ENTRY_DATA *re, int num,
	char *msgbuf)
{
	RES_ITEM *	ri = (RES_ITEM *)re->array + num;
	char *		part1;
	char *		part2;
	int			l;
	int			rc;

	/*--------------------------------------------------------------------
	 * strip off trailing white-space
	 */
	l = strlen(line) - 1;
	for (; l >= 0; l--)
	{
		if (! isspace(line[l]))
			break;
	}
	line[l+1] = 0;

	/*--------------------------------------------------------------------
	 * split line into two parts
	 */
	part1 = line;

	for (part2 = line; *part2; part2++)
	{
		if (isspace(*part2))
			break;
	}

	if (*part2 == 0)
	{
		sprintf(msgbuf, "No data in line \"%s\"", line);
		return (-1);
	}
	*part2++ = 0;

	for (; *part2; part2++)
	{
		if (! isspace(*part2))
			break;
	}

	if ((int)strlen(part1) >= sizeof(ri->name))
	{
		sprintf(msgbuf, "Name too long: \"%s\"", part1);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * process this data
	 */
	strcpy(ri->name, part1);

	switch (re->type)
	{
	case R_TYPE_KEY:
		rc = res_compile_key_process(part2, ri, num, msgbuf);
		break;

	case R_TYPE_NUM:
		rc = res_compile_num_process(part2, ri, num, msgbuf);
		break;

	case R_TYPE_STR:
		rc = res_compile_str_process(part2, ri, num, msgbuf);
		break;
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * process a text file
 */
static int res_compile_text_process (FILE *fp, RES_ENTRY_DATA *re,
	char *msgbuf)
{
	int	count = 0;
	int	rc;

	while (TRUE)
	{
		char	line[256];

		if (fgets(line, sizeof(line), fp) == 0)
			break;

		if (*line == '#' || isspace(*line))
			continue;

		rc = res_compile_line_process(line, re, count++, msgbuf);
		if (rc)
			break;
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * count the entries in a text file
 */
static int res_compile_text_count (FILE *fp)
{
	int	count = 0;

	while (TRUE)
	{
		char	line[256];

		if (fgets(line, sizeof(line), fp) == 0)
			break;

		if (*line == '#' || isspace(*line))
			continue;

		count++;
	}

	return (count);
}

/*------------------------------------------------------------------------
 * compile a text file
 */
static int res_compile_text (const char *name, int type, RES_ENTRY_DATA *re,
	char *msgbuf)
{
	char	text_file[256];
	FILE *	fp;
	void *	array;
	int		num;
	int		len;
	int		rc;

	/*--------------------------------------------------------------------
	 * open the text file
	 */
	sprintf(text_file, "%s.txt", name);

	fp = fopen(text_file, "r");
	if (fp == 0)
	{
		sprintf(msgbuf, "Cannot open text file %s", text_file);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * count the entries in the file
	 */
	num = res_compile_text_count(fp);
	if (num <= 0)
	{
		fclose(fp);

		sprintf(msgbuf, "Invalid text file %s", text_file);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * allocate the data array
	 */
	len = num * sizeof(RES_ITEM);
	array = MALLOC(len);
	if (array == 0)
	{
		fclose(fp);

		sprintf(msgbuf, "Cannot allocate data array for %s", text_file);
		return (-1);
	}

	memset(array, 0, len);

	/*--------------------------------------------------------------------
	 * fill in the struct
	 */
	re->array	= array;
	re->num		= num;
	re->type	= type;
	strcpy(re->name, name);

	/*--------------------------------------------------------------------
	 * now actually process the text file
	 */
	rewind(fp);

	rc = res_compile_text_process(fp, re, msgbuf);
	fclose(fp);

	return (rc);
}

/*------------------------------------------------------------------------
 * get basename of a path
 */
const char *res_basename (const char *path)
{
	const char *	p;
	const char *	s;

	p = path;
	while (TRUE)
	{
		s = strpbrk(p, "/\\");
		if (s == 0)
			break;
		p = s + 1;
	}

	return (p);
}

/*------------------------------------------------------------------------
 * compile a resource file
 */
int res_compile (const char *name, char *msgbuf)
{
	RES_LIST			res_list;
	RES_LIST *			rl = &res_list;
	RES_FILE *			rf;
	RES_ENTRY_DATA *	entries;
	FILE *				fp;
	int					i;
	int					rc;

	/*--------------------------------------------------------------------
	 * load res list file
	 */
	fp = fopen(RES_FILENAME ".txt", "r");
	if (fp == 0)
	{
		sprintf(msgbuf, "cannor load resource list");
		return (-1);
	}

	rc = res_list_load(fp, rl, msgbuf);
	fclose(fp);
	if (rc)
		return (-1);

	/*--------------------------------------------------------------------
	 * allocate res-file & entries
	 */
	rf = (RES_FILE *)MALLOC(sizeof(*rf));
	if (rf == 0)
	{
		sprintf(msgbuf, "Cannot allocate resource-file");
		return (-1);
	}
	memset(rf, 0, sizeof(*rf));

	entries = (RES_ENTRY_DATA *)MALLOC(rl->num_ents * sizeof(*entries));
	if (entries == 0)
	{
		FREE(rf);
		sprintf(msgbuf, "Cannot allocate resource-file entries");
		return (-1);
	}
	memset(entries, 0, rl->num_ents * sizeof(*entries));

	/*--------------------------------------------------------------------
	 * fill in res-file struct
	 */
	rf->hdr.magic		= RES_MAGIC;
	rf->hdr.num_ents	= rl->num_ents;
	strcpy(rf->hdr.name, rl->lang_name);
	strcpy(rf->hdr.file, res_basename(name));

	rf->loaded			= FALSE;
	rf->has_names		= TRUE;
	rf->entries			= entries;

	/*--------------------------------------------------------------------
	 * process each text file
	 */
	for (i = 0; i < rl->num_ents; i++)
	{
		RES_LIST_ENTRY *	r = rl->entries + i;
		RES_ENTRY_DATA *	e = rf->entries + i;

		rc = res_compile_text(r->name, r->rt->type_code, e, msgbuf);
		if (rc)
			break;
	}

	/*--------------------------------------------------------------------
	 * if successful, save the resource file
	 */
	if (rc == 0)
	{
		char filename[256];

		sprintf(filename, "%s.res", name);
		rc = res_file_save(filename, rf);
		if (rc)
		{
			sprintf(msgbuf, "Cannot save resource file %s", filename);
		}
	}

	/*--------------------------------------------------------------------
	 * free the resource file struct
	 */
	for (i = 0; i < rf->hdr.num_ents; i++)
	{
		RES_ENTRY_DATA *	e = rf->entries + i;

		if (e->type == R_TYPE_STR)
		{
			RES_ITEM *	item_list = (RES_ITEM *)e->array;
			int			j;

			for (j = 0; j < e->num; j++)
			{
				FREE(item_list[j].data);
			}
		}

		FREE(e->array);
	}

	FREE(rf->entries);
	FREE(rf);

	return (rc);
}
