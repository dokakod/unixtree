/*------------------------------------------------------------------------
 *	Routine to create c & h files for a resource file
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "res.h"

/*------------------------------------------------------------------------
 * get list of res entries
 */
static res_get_list (RES_LIST *rl, char *msgbuf)
{
	FILE *	fp;
	int		rc;

	fp = fopen(RES_FILENAME ".txt", "r");
	if (fp == 0)
	{
		sprintf(msgbuf, "cannot open resource list");
		return (-1);
	}

	rc = res_list_load(fp, rl, msgbuf);
	fclose(fp);

	return (rc);
}

/*------------------------------------------------------------------------
 * make C resfile
 */
static int res_make_c_resfile (RES_LIST *rl, const char *filename)
{
	RES_LIST_ENTRY *	rle;
	FILE *				fp;
	int					i;

	/*--------------------------------------------------------------------
	 * open the C file
	 */
	fp = fopen(RES_FILENAME ".c", "w");
	if (fp == 0)
	{
		fprintf(stderr, "cannot open %s.c\n", RES_FILENAME);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * preamble & headers
	 */
	fprintf(fp, "/* ALL EDITS WILL BE LOST - This is a generated file. */\n");
	fprintf(fp, "\n");
	fprintf(fp, "#include \"curses.h\"\n");
	fprintf(fp, "#include \"res.h\"\n");
	fprintf(fp, "\n");

	for (i=0; i<rl->num_ents; i++)
	{
		rle = rl->entries + i;

		fprintf(fp, "#include \"%s.h\"\n", rle->name);
	}
	fprintf(fp, "\n");

	/*--------------------------------------------------------------------
	 * array refs
	 */
	for (i=0; i<rl->num_ents; i++)
	{
		rle = rl->entries + i;

		fprintf(fp, "extern %s res_%s_tbl[];\n",
			rle->rt->type_decl, rle->name);
	}
	fprintf(fp, "\n");

	/*--------------------------------------------------------------------
	 * array of entries
	 */
	fprintf(fp, "static const RES_ENTRY_DATA res_entries[] =\n");
	fprintf(fp, "{\n");
	for (i=0; i<rl->num_ents; i++)
	{
		rle = rl->entries + i;

		fprintf(fp, "\t{\n");

		fprintf(fp, "\t\t(void *)res_%s_tbl,\n", rle->name);
		fprintf(fp, "\t\tNUM_%s,\n", rle->caps);
		switch (rle->rt->type_code)
		{
		case R_TYPE_KEY:	fprintf(fp, "\t\tR_TYPE_KEY,\n");	break;
		case R_TYPE_NUM:	fprintf(fp, "\t\tR_TYPE_NUM,\n");	break;
		case R_TYPE_STR:	fprintf(fp, "\t\tR_TYPE_STR,\n");	break;
		}
		fprintf(fp, "\t\t\"%s\"\n", rle->name);

		fprintf(fp, "\t}%s\n", i == rl->num_ents-1 ? "" : ",\n");
	}
	fprintf(fp, "};\n");
	fprintf(fp, "\n");

	/*--------------------------------------------------------------------
	 * now the actual resource file struct
	 */
	fprintf(fp, "const RES_FILE res_file =\n");
	fprintf(fp, "{\n");
	fprintf(fp, "\t{\n");
	fprintf(fp, "\t\tRES_MAGIC,\n");
	fprintf(fp, "\t\tsizeof(res_entries) / sizeof(*res_entries),\n");
	fprintf(fp, "\t\t\"%s-builtin\",\n", rl->lang_name);
	fprintf(fp, "\t\t\"%s\"\n", filename);
	fprintf(fp, "\t},\n");
	fprintf(fp, "\t0,\n");
	fprintf(fp, "\t0,\n");
	fprintf(fp, "\t(RES_ENTRY_DATA *)res_entries\n");
	fprintf(fp, "};\n");

	fclose(fp);

	return (0);
}

/*------------------------------------------------------------------------
 * make H resfile
 */
static int res_make_h_resfile (RES_LIST *rl)
{
	RES_LIST_ENTRY *	rle;
	FILE *				fp;
	int					i;

	/*--------------------------------------------------------------------
	 * open the C file
	 */
	fp = fopen(RES_FILENAME ".h", "w");
	if (fp == 0)
	{
		fprintf(stderr, "cannot open %s.h\n", RES_FILENAME);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * preamble
	 */
	fprintf(fp, "/* ALL EDITS WILL BE LOST - This is a generated file. */\n");
	fprintf(fp, "\n");

	fprintf(fp, "#ifndef RESFILE_H\n");
	fprintf(fp, "#define RESFILE_H\n");
	fprintf(fp, "\n");

	/*--------------------------------------------------------------------
	 * other includes
	 */
	for (i=0; i<rl->num_ents; i++)
	{
		rle = rl->entries + i;

		fprintf(fp, "#include \"%s.h\"\n", rle->name);
	}
	fprintf(fp, "\n");

	/*--------------------------------------------------------------------
	 * index defines
	 */
	for (i=0; i<rl->num_ents; i++)
	{
		rle = rl->entries + i;

		fprintf(fp, "#define RES_INDEX_%s\t%d\n", rle->caps, i);
	}
	fprintf(fp, "\n");

	/*--------------------------------------------------------------------
	 * pointers to resource arrays
	 */
	for (i=0; i<rl->num_ents; i++)
	{
		rle = rl->entries + i;

		fprintf(fp,
		"#define %s_LIST(rf)\t( (%s*)((rf)->entries[RES_INDEX_%s].array) )\n",
			rle->caps, rle->rt->type_decl, rle->caps);
	}
	fprintf(fp, "\n");

	/*--------------------------------------------------------------------
	 * access macros
	 */
	for (i=0; i<rl->num_ents; i++)
	{
		rle = rl->entries + i;

		fprintf(fp, "#define %s_ENTRY(rf,n)\t( %s_LIST(rf)[n] )\n",
			rle->caps, rle->caps);
	}
	fprintf(fp, "\n");

	fprintf(fp, "#endif /* RESFILE_H */\n");
	fclose(fp);

	return (0);
}

/*------------------------------------------------------------------------
 * main program
 */
int main (int argc, char **argv)
{
	RES_LIST		res_list;
	RES_LIST *		rl = &res_list;
	char			msgbuf[128];
	const char *	filename;
	int				i;
	int				rc;

	/*--------------------------------------------------------------------
	 * get filename
	 */
	if (argc <= 1)
	{
		fprintf(stderr, "%s: no filename specified\n", argv[0]);
		return (1);
	}

	filename = argv[1];

	/*--------------------------------------------------------------------
	 * get list of resource entries
	 */
	rc = res_get_list(rl, msgbuf);
	if (rc)
	{
		fprintf(stderr, "%s: %s\n", argv[0], msgbuf);
		return (1);
	}

	/*--------------------------------------------------------------------
	 * now create all entry c & h files
	 */
	for (i=0; i<rl->num_ents; i++)
	{
		RES_LIST_ENTRY *	rle	= rl->entries + i;

		rc = res_text_to_ch(rle, msgbuf);
		if (rc)
		{
			fprintf(stderr, "%s: %s\n", argv[0], msgbuf);
			return (1);
		}

	}

	/*--------------------------------------------------------------------
	 * now create the main file
	 */
	rc = res_make_c_resfile(rl, filename);
	if (rc == 0)
		rc = res_make_h_resfile(rl);

	return (rc == 0 ? 0 : 1);
}
