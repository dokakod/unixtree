/*------------------------------------------------------------------------
 * extract a resource file
 */
#include "restool.h"

/*------------------------------------------------------------------------
 * print a section entry
 */
static void res_extract_entry (RES_ENTRY_DATA *re, int num, FILE *fp)
{
	RES_ITEM *	ri = (RES_ITEM *)re->array + num;
	char *		s;
	int			n;

	fprintf(fp, "%-32s ", ri->name);

	switch (re->type)
	{
	case R_TYPE_KEY:
		n = (int)ri->data;
		if (n == 0)
		{
			fprintf(fp, "0");
		}
		else
		{
			if (n >= 0x20 && n <= 0xff)
			{
				if (n == '\\')
					fprintf(fp, "'\\\\'");
				else
					fprintf(fp, "'%c'", n);
			}
			else
			{
				char buf[64];
				char *p;

				get_key_name(n, buf);
				for (p=buf; *p; p++)
					*p = toupper(*p);

				fprintf(fp, "KEY_%s", buf);
			}
		}
		break;

	case R_TYPE_NUM:
		n = (int)ri->data;
		if (n < 32)
			fprintf(fp, "%d", n);
		else
			fprintf(fp, "'%c'", n);
		break;

	case R_TYPE_STR:
		fputc('"', fp);
		for (s = (char *)ri->data; *s; s++)
		{
			switch (*s)
			{
			case '"':
			case '\\':
				fputc('\\', fp);
				break;
			}
			fputc(*s, fp);
		}
		fputc('"', fp);
		break;
	}

	fprintf(fp, "\n");
}

/*------------------------------------------------------------------------
 * extract a resource file section
 */
static int res_extract_section (RES_ENTRY_DATA *re, char *msgbuf)
{
	char	sect_path[256];
	FILE *	fp;
	int		i;

	/*--------------------------------------------------------------------
	 * open section file
	 */
	sprintf(sect_path, "%s.txt", re->name);

	fp = fopen(sect_path, "w");
	if (fp == 0)
	{
		sprintf(msgbuf, "Cannot open file %s", sect_path);
		return (-1);
	}

	fprintf(fp, "# resource file section: %s\n", re->name);
	fprintf(fp, "\n");

	/*--------------------------------------------------------------------
	 * print each entry
	 */
	for (i = 0; i < re->num; i++)
	{
		res_extract_entry(re, i, fp);
	}

	/*--------------------------------------------------------------------
	 * close section file
	 */
	fclose(fp);

	return (0);
}

/*------------------------------------------------------------------------
 * extract a resource file
 */
int res_extract (const char *name, char *msgbuf)
{
	char				res_path[256];
	RES_FILE *			rf;
	RES_ENTRY_DATA *	re;
	int					i;
	int					rc;

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
	 * extract each section
	 */
	for (i=0; i<rf->hdr.num_ents; i++)
	{
		re = rf->entries + i;

		rc = res_extract_section(re, msgbuf);
		if (rc)
			break;
	}

	/*--------------------------------------------------------------------
	 * if no errors, write out the resource list
	 */
	if (rc == 0)
	{
		FILE *	fp;

		fp = fopen(RES_FILENAME ".txt", "w");
		if (fp == 0)
		{
			rc = -1;
		}
		else
		{
			fprintf(fp, "# resource file list\n");
			fprintf(fp, "\n");
			fprintf(fp, "name\t%s\n", rf->hdr.name);
			fprintf(fp, "\n");

			for (i=0; i<rf->hdr.num_ents; i++)
			{
				const RES_TYPE *	rt;

				re = rf->entries + i;
				rt = res_find_by_type(re->type);
				fprintf(fp, "file\t%s:%s\n", re->name, rt->type_name);
			}

			fclose(fp);
		}
	}

	/*--------------------------------------------------------------------
	 * free the resource file
	 */
	res_file_free(rf);

	return (rc);
}
