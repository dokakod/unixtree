/*------------------------------------------------------------------------
 * low-level routines to read resource-file headers
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "libsys.h"
#include "res.h"

/*------------------------------------------------------------------------
 * resource type table
 */
static const RES_TYPE	res_types[] =
{
	{ R_TYPE_STR,	"str",	"char *"	},
	{ R_TYPE_NUM,	"num",	"int   "	},
	{ R_TYPE_KEY,	"key",	"int   "	},

	{ -1, 0, 0 }
};

/*------------------------------------------------------------------------
 * strip()
 *
 * Strip trailing white space from a string
 */
static char * strip (char *str)
{
	char *	lastnsp = str;
	char *	s;

	for (s = str; *s; s++)
	{
		if (! isspace(*s))
			lastnsp = s + 1;
	}
	*lastnsp = 0;

	return (str);
}

/*------------------------------------------------------------------------
 * compare 2 files
 *
 * returns:	TRUE if same, FALSE if different
 */
static int res_file_compare (const char *path1, const char *path2)
{
	struct stat	stbuf1;
	struct stat	stbuf2;
	FILE *		fp1;
	FILE *		fp2;
	int			eof1;
	int			eof2;
	int			rc;

	/*--------------------------------------------------------------------
	 * if paths are the same, the files are the same
	 */
	if (strcmp(path1, path2) == 0)
	{
		return (TRUE);
	}

	/*--------------------------------------------------------------------
	 * get stat structs - if either doesn't exist, they are different
	 */
	rc = stat(path1, &stbuf1);
	if (rc)
	{
		return (FALSE);
	}

	rc = stat(path2, &stbuf2);
	if (rc)
	{
		return (FALSE);
	}

	/*--------------------------------------------------------------------
	 * if links to each other, they are the same
	 */
	if (stbuf1.st_ino == stbuf2.st_ino &&
	    stbuf1.st_dev == stbuf2.st_dev)
	{
		return (TRUE);
	}

	/*--------------------------------------------------------------------
	 * I guess we gotta read the file now.
	 */
	fp1 = fopen(path1, "r");
	if (fp1 == 0)
	{
		return (FALSE);
	}

	fp2 = fopen(path2, "r");
	if (fp2 == 0)
	{
		fclose(fp1);
		return (FALSE);
	}

	rc = TRUE;
	eof1 = eof2 = FALSE;
	while (TRUE)
	{
		/*----------------------------------------------------------------
		 * Compare lines after removing comments & blank lines.
		 */
		char	line1[256];
		char	line2[256];
		char *	p;

		while (TRUE)
		{
			if (fgets(line1, sizeof(line1), fp1) == 0)
			{
				eof1 = TRUE;
				break;
			}

			p = strstr(line1, "/*");
			if (p != 0)
				*p = 0;

			strip(line1);
			if (*line1 != 0)
				break;
		}

		while (TRUE)
		{
			if (fgets(line2, sizeof(line2), fp2) == 0)
			{
				eof2 = TRUE;
				break;
			}

			p = strstr(line2, "/*");
			if (p != 0)
				*p = 0;

			strip(line2);
			if (*line2 != 0)
				break;
		}

		if (eof1 && eof2)
			break;

		if (eof1 || eof2)
		{
			rc = FALSE;
			break;
		}

		if (strcmp(line1, line2) != 0)
		{
			rc = FALSE;
			break;
		}
	}

	fclose(fp1);
	fclose(fp2);

	return (rc);
}

/*------------------------------------------------------------------------
 * find a type entry by type
 */
const RES_TYPE * res_find_by_type (int type)
{
	const RES_TYPE *	rt;

	for (rt = res_types; rt->type_code >= 0; rt++)
	{
		if (rt->type_code == type)
			return (rt);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * find a type entry by name
 */
const RES_TYPE * res_find_by_name (const char *name)
{
	const RES_TYPE *	rt;

	for (rt = res_types; rt->type_code >= 0; rt++)
	{
		if (strcmp(name, rt->type_name) == 0)
			return (rt);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * process a text file into a c & h file
 */
int res_text_to_ch (RES_LIST_ENTRY *rle, char *msgbuf)
{
	char	t_name[256];
	char	c_name[256];
	char	h_name[256];
	char	H_name[256];
	char	line[256];
	char *	part1;
	char *	part2;
	FILE *	inp;
	FILE *	out_c;
	FILE *	out_h;
	int		count = 0;
	int		rc;

	/*--------------------------------------------------------------------
	 * open the input text file
	 */
	strcpy(t_name, rle->name);
	strcat(t_name, ".txt");

	inp = fopen(t_name, "r");
	if (inp == 0)
	{
		sprintf(msgbuf, "cannot open input file \"%s\"", t_name);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * open the output c-program file
	 */
	strcpy(c_name, rle->name);
	strcat(c_name, ".c");

	out_c = fopen(c_name, "w");
	if (out_c == 0)
	{
		fclose(inp);

		sprintf(msgbuf, "cannot open output file \"%s\"", c_name);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * open the output c-header file
	 */
	strcpy(h_name, rle->name);
	strcat(h_name, ".h");

	strcpy(H_name, rle->name);
	strcat(H_name, ".hh");

	out_h = fopen(H_name, "w");
	if (out_h == 0)
	{
		fclose(out_c);
		fclose(inp);

		sprintf(msgbuf, "cannot open output file \"%s\"", H_name);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * output file header info
	 */
	fprintf(out_c,
		"/* ALL EDITS WILL BE LOST - This is a generated file. */\n");
	fprintf(out_c, "\n");
	if (rle->rt->type_code == R_TYPE_KEY)
	{
		fprintf(out_c, "#include \"curses.h\"\n");
		fprintf(out_c, "\n");
	}
	fprintf(out_c, "const %s res_%s_tbl[] =\n", rle->rt->type_decl, rle->name);
	fprintf(out_c, "{\n");

	fprintf(out_h,
		"/* ALL EDITS WILL BE LOST - This is a generated file. */\n");
	fprintf(out_h, "\n");
	fprintf(out_h, "#ifndef %s_H\n", rle->caps);
	fprintf(out_h, "#define %s_H\n", rle->caps);
	fprintf(out_h, "\n");

	/*--------------------------------------------------------------------
	 * process all input text lines
	 */
	while (fgets(line, sizeof(line), inp))
	{
		/*----------------------------------------------------------------
		 * zap newline at end
		 */
		line[strlen(line)-1] = 0;

		/*----------------------------------------------------------------
		 * check for blank lines
		 */
		if (*line == 0)
		{
			fprintf(out_c, "\n");
			fprintf(out_h, "\n");
			continue;
		}

		/*----------------------------------------------------------------
		 * check for comments
		 */
		if (*line == '#')
		{
			fprintf(out_c, "/*%s */\n", line+1);
			fprintf(out_h, "/*%s */\n", line+1);
			continue;
		}

		/*----------------------------------------------------------------
		 * split line into two parts
		 */
		part1 = line;
		for (part2=line; *part2 && ! isspace(*part2); part2++)
			;
		if (*part2 == 0)
			continue;

		*part2 = 0;
		for (part2++; *part2 && isspace(*part2); part2++)
			;
		if (*part2 == 0)
			continue;

		/*----------------------------------------------------------------
		 * write out parts in both files
		 */
		fprintf(out_c, "\t%s,\t/* %s */\n", part2, part1);
		fprintf(out_h, "#define %-24s\t%d\n", part1, count++);
	}

	/*--------------------------------------------------------------------
	 * output file trailer info
	 */
	fprintf(out_c, "\t0\n");
	fprintf(out_c, "};\n");

	fprintf(out_h, "\n");
	fprintf(out_h, "#define NUM_%s\t%d\n", rle->caps, count);
	fprintf(out_h, "\n");
	fprintf(out_h, "#endif /* %s_H */\n", rle->caps);

	/*--------------------------------------------------------------------
	 * close all files
	 */
	fclose(inp);
	fclose(out_c);
	fclose(out_h);

	/*--------------------------------------------------------------------
	 * now compare the H file with the h file
	 *
	 * This is done because if a header file changes, we have to rebuild
	 * everything.  So, if only the text is changed, and the header
	 * are the same, don't update the header.
	 */
	rc = res_file_compare(H_name, h_name);
	if (rc)
	{
		/*----------------------------------------------------------------
		 * headers are the same - delete the new one.
		 */
		remove(H_name);
	}
	else
	{
		/*----------------------------------------------------------------
		 * headers are different - update the old one.
		 */
		rc = rename(H_name, h_name);
		if (rc)
		{
			sprintf(msgbuf, "Cannot rename %s to %s", H_name, h_name);
			return (-1);
		}
	}

	return (0);
}

/*------------------------------------------------------------------------
 * get next name/type entry from a resource list
 */
static int res_get_entry (FILE *fp, char *name, char *data1, char *data2)
{
	char	line[256];
	char *	l;
	char *	p;

	*name	= 0;
	*data1	= 0;
	*data2	= 0;

	/*--------------------------------------------------------------------
	 * read in lines until we get a real line
	 */
	for (;;)
	{
		/*----------------------------------------------------------------
		 * read in next line
		 */
		l = fgets(line, sizeof(line), fp);
		if (l == 0)
			return (-1);

		/*----------------------------------------------------------------
		 * zap nl at end
		 */
		if (*l != 0)
			strip(l);

		/*----------------------------------------------------------------
		 * skip blank lines & comment lines
		 */
		if (*l == 0 || *l == '#')
			continue;

		/*----------------------------------------------------------------
		 * If we get here, we should have a valid line.
		 * Split it up into name & data.
		 */
		p = name;
		for (; *l; l++)
		{
			if (! isalnum(*l))
				break;
			*p++ = *l;
		}
		*p = 0;

		for (; *l; l++)
		{
			if (isalnum(*l))
				break;
		}

		p = data1;
		for (; *l; l++)
		{
			if (! isalnum(*l))
				break;
			*p++ = *l;
		}
		*p = 0;

		for (; *l; l++)
		{
			if (isalnum(*l))
				break;
		}

		p = data2;
		for (; *l; l++)
		{
			if (! isalnum(*l))
				break;
			*p++ = *l;
		}
		*p = 0;

		/*----------------------------------------------------------------
		 * Check if we have a valid entry
		 */
		if (*name != 0 || *data1 != 0)
			break;
	}

	return (0);
}

/*------------------------------------------------------------------------
 * read in a resource list
 */
int res_list_load (FILE *fp, RES_LIST *rl, char *msgbuf)
{
	/*--------------------------------------------------------------------
	 * clear res-list
	 */
	memset(rl, 0, sizeof(*rl));

	/*--------------------------------------------------------------------
	 * read in lines from resource-file list
	 */
	for (;;)
	{
		char	name[128];
		char	data[128];
		char	type[128];
		int		rc;

		rc = res_get_entry(fp, name, data, type);
		if (rc)
			break;

		if (strcmp(name, "name") == 0)
		{
			strcpy(rl->lang_name, data);
		}
		else if (strcmp(name, "file") == 0)
		{
			RES_LIST_ENTRY *	rle = rl->entries + rl->num_ents;
			char *				p;

			if (*type == 0)
			{
				sprintf(msgbuf, "invalid file entry \"%s:%s\"", data, type);
				return (-1);
			}

			strcpy(rle->name, data);
			strcpy(rle->type, type);
			strcpy(rle->caps, rle->name);

			for (p=rle->caps; *p; p++)
				*p = toupper(*p);

			rle->rt = res_find_by_name(rle->type);
			if (rle->rt == 0)
			{
				sprintf(msgbuf, "invalid type %s", rle->type);
				return (-1);
			}

			rl->num_ents++;
		}
		else
		{
			sprintf(msgbuf, "invalid list entry \"%s %s\"", name, data);
			return (-1);
		}
	}

	/*--------------------------------------------------------------------
	 * check if valid entries read in
	 */
	if (rl->lang_name[0] == 0)
	{
		sprintf(msgbuf, "no lang name found");
		return (-1);
	}

	if (rl->num_ents == 0)
	{
		sprintf(msgbuf, "no file entries found");
		return (-1);
	}

	return (0);
}
