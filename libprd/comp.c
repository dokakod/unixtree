/*------------------------------------------------------------------------
 * compress/uncompress a file
 */
#include "libprd.h"

static int is_file_compressed (FBLK *f)
{
	char path[MAX_PATHLEN];

	fblk_to_pathname(f, path);

	return (x_is_file_compressed(path));
}

static void replace_cur_file (const char *oldname, const char *newname)
{
	int rc;
	struct stat stbuf;

	/* remove old file */

	rc = os_stat(oldname, &stbuf);
	if (rc == 0)
		rc = os_file_delete(oldname);

	/*
	 * replace old entry with new.
	 */
	{
		fblk_change_name(gbl(scr_cur)->cur_file, fn_basename(newname));
		hilite_file(ON);
		disp_cur_file();
	}
	if (rc == 0)
		os_chmod(newname, stbuf.st_mode);
	check_the_file(newname);
}

static void compress_cur_file (int type)
{
	char oldname[MAX_PATHLEN];
	char newname[MAX_PATHLEN];
	FBLK *f;
	int rc;

	f = gbl(scr_cur)->cur_file;

	/* create pathnames */

	strcpy(oldname, gbl(scr_cur)->path_name);
	fn_append_filename_to_dir(oldname, FULLNAME(f));
	strcpy(newname, oldname);

	x_make_compressed_name(newname, type);

	/* create a compressed file */

	rc = x_comp(oldname, newname, type);
	if (rc)
		return;

	/* replace current file with compressed file */

	replace_cur_file(oldname, newname);
}

static void uncompress_cur_file (int type)
{
	char oldname[MAX_PATHLEN];
	char newname[MAX_PATHLEN];
	FBLK *f;
	int rc;

	f = gbl(scr_cur)->cur_file;

	/* create pathnames */

	strcpy(oldname, gbl(scr_cur)->path_name);
	fn_append_filename_to_dir(oldname, FULLNAME(f));
	strcpy(newname, oldname);

	x_make_uncompressed_name(newname, type);

	/* create an uncompressed file */

	rc = x_decomp(oldname, newname, type);
	if (rc)
		return;

	/* replace current file with compressed file */

	replace_cur_file(oldname, newname);
}

void toggle_compress_cur_file (void)
{
	int type;

	type = is_file_compressed(gbl(scr_cur)->cur_file);

	if (type != -1)
		uncompress_cur_file(type);
	else
		compress_cur_file(opt(comp_type));
}

int x_is_file_compressed (const char *filename)
{
	char *ext = fn_ext(filename);

	if (ext != 0)
	{
		/* check for compress type */

		if (strcmp(ext, "Z") == 0)
			return (comp_lzw);

		/* check for pack type */

		if (strcmp(ext, "z") == 0)
			return (comp_pack);

		/* check for gzip type */

		if (strcmp(ext, "gz") == 0 || strcmp(ext, "tgz") == 0)
			return (comp_gzip);
	}

	return (-1);
}

void x_make_compressed_name (char *filename, int type)
{
	char *ext;

	switch (type)
	{
	case comp_lzw:
		strcat(filename, ".Z");
		break;

	case comp_gzip:
		ext = fn_ext(filename);
		if (ext != 0 && strcmp(ext, "tar") == 0)
			strcpy(ext, "tgz");
		else
			strcat(filename, ".gz");
		break;

	case comp_pack:
		strcat(filename, ".z");
		break;
	}
}

void x_make_uncompressed_name (char *filename, int type)
{
	char *ext;

	switch (type)
	{
	case comp_lzw:
		filename[strlen(filename)-2] = 0;	/* strip ".Z" */
		break;

	case comp_pack:
		filename[strlen(filename)-2] = 0;	/* strip ".z" */
		break;

	case comp_gzip:
		ext = fn_ext(filename);
		if (strcmp(ext, "tgz") == 0)
			strcpy(ext, "tar");
		else
			ext[-1] = 0;
		break;
	}
}

int x_comp (const char *old_name, const char *new_name, int type)
{
	char msgbuf[128];
	int rc;

	switch (type)
	{
	case comp_lzw:
		rc = comp(old_name, new_name);
		break;

	case comp_gzip:
		rc = gzip(old_name, new_name, FALSE, msgbuf);
		if (rc)
			err_message(msgbuf, "", ERR_ANY);
		break;

	case comp_pack:
		errmsg(ER_NOPACK, "", ERR_ANY);
		rc = -1;
		break;
	}

	return (rc);
}

int x_decomp (const char *old_name, const char *new_name, int type)
{
	char msgbuf[128];
	int rc;

	switch (type)
	{
	case comp_lzw:
		rc = decomp(old_name, new_name);
		break;

	case comp_gzip:
		rc = gzip(old_name, new_name, TRUE, msgbuf);
		if (rc)
			err_message(msgbuf, "", ERR_ANY);
		break;

	case comp_pack:
		rc = gzip(old_name, new_name, TRUE, msgbuf);
		if (rc)
			err_message(msgbuf, "", ERR_ANY);
		break;
	}

	return (rc);
}
