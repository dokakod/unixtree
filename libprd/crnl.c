/*------------------------------------------------------------------------
 * process the "add/remove" CR cmds (to-dos / un-dos)
 */
#include "libprd.h"

void dos_to_unix (void)
{
	char oldname[MAX_PATHLEN];
	char newname[MAX_PATHLEN];
	FBLK *f;
	FILE *inp_fp;
	FILE *out_fp;
	int c;
	int lastc;

	f = gbl(scr_cur)->cur_file;

	/* create pathnames */

	strcpy(oldname, gbl(scr_cur)->path_name);
	fn_append_filename_to_dir(oldname, FULLNAME(f));
	os_make_temp_name(newname, gbl(scr_cur)->path_name, NULL);

	/* strip out all cr chars and ^z chars */

	inp_fp = fopen(oldname, "rb");
	if (!inp_fp)
	{
		return;
	}

	out_fp = fopen(newname, "wb");
	if (!out_fp)
	{
		fclose(inp_fp);
		return;
	}

	lastc = 0;
	while ((c = getc(inp_fp)) != EOF)
	{
		if (c != 0x0d && c != 0x1a)
		{
			putc(c, out_fp);
			lastc = c;
		}
	}
	if (lastc != 0x0a)
		fputc(0x0a, out_fp);
	fclose(inp_fp);
	fclose(out_fp);

	os_file_delete(oldname);
	os_file_rename(newname, oldname);
	check_the_file(oldname);
}

void unix_to_dos (void)
{
	char oldname[MAX_PATHLEN];
	char newname[MAX_PATHLEN];
	FBLK *f;
	FILE *inp_fp;
	FILE *out_fp;
	int c;

	f = gbl(scr_cur)->cur_file;

	/* create pathnames */

	strcpy(oldname, gbl(scr_cur)->path_name);
	fn_append_filename_to_dir(oldname, FULLNAME(f));
	os_make_temp_name(newname, gbl(scr_cur)->path_name, NULL);

	/* add cr before all nl chars */

	inp_fp = fopen(oldname, "rb");
	if (!inp_fp)
	{
		return;
	}

	out_fp = fopen(newname, "wb");
	if (!out_fp)
	{
		fclose(inp_fp);
		return;
	}

	while ((c = getc(inp_fp)) != EOF)
	{
		if (c == 0x0a)
			putc(0x0d, out_fp);
		putc(c, out_fp);
	}
	fclose(inp_fp);
	fclose(out_fp);

	os_file_delete(oldname);
	os_file_rename(newname, oldname);
	check_the_file(oldname);
}
