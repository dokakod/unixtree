/*------------------------------------------------------------------------
 * process the "open" cmd
 */
#include "libprd.h"

void do_open (void)
{
	char fullpath[MAX_PATHLEN];

	/*--------------------------------------------------------------------
	 * get full path of filename
	 */
	strcpy(fullpath, gbl(scr_cur)->path_name);
	fn_append_filename_to_dir(fullpath, FULLNAME(gbl(scr_cur)->cur_file));

	do_open_file(fullpath);
}

void do_open_file (const char *fullpath)
{
	char buffer[BUFSIZ];
	char filename[BUFSIZ];
	char temppath[MAX_PATHLEN];
	char cmdpath[MAX_PATHLEN];
	char abs_cmd_path[MAX_PATHLEN];
	char directory[MAX_PATHLEN];
	char parent[MAX_PATHLEN];
	struct stat stbuf;
	char *p;
	char *e;
	int	comp_type = -1;

	/*--------------------------------------------------------------------
	 * check if file is compressed
	 */
	*temppath = 0;
	if (opt(exp_comp_files))
		comp_type = x_is_file_compressed(fullpath);

	/*--------------------------------------------------------------------
	 * if file is compressed, uncompress it first
	 */
	if (comp_type != -1)
	{
		char tempfile[MAX_FILELEN];
		char *ext;

		strcpy(tempfile, fn_basename(fullpath));
		x_make_uncompressed_name(tempfile, comp_type);
		ext = fn_ext(tempfile);

		os_make_temp_name(temppath, gbl(pgm_tmp), ext);

		if (x_decomp(fullpath, temppath, comp_type))
			return;

		fullpath = temppath;
	}

	/*--------------------------------------------------------------------
	 * if executable, just execute it
	 */
	if (os_stat(fullpath, &stbuf))
	{
		errsys(ER_CSN);
		return;
	}

	if (stbuf.st_mode & (S_IXUSR|S_IXGRP|S_IXOTH))
	{
		if (! can_we_exec(&stbuf))
		{
			errmsg(ER_NPTE, "", ERR_ANY);
		}
		else
		{
			char cmd[MAX_PATHLEN];

			strcpy(cmd, "\"");
			strcat(cmd, fullpath);
			strcat(cmd, "\"");
			xsystem(cmd, "", FALSE);
		}

		if (*temppath != 0)
			os_file_delete(temppath);

		return;
	}

	/*--------------------------------------------------------------------
	 * check if file has an extension
	 */
	e = fn_ext(fullpath);
	if (e == 0 || *e == 0)
		return;

	/* look for <ext>.sh (UNIX) or <ext>.cmd | <ext>.bat (NT) */

	strcpy(filename, e);
#if V_WINDOWS
	strcat(filename, ".cmd");
#else
	strcat(filename, ".sh");
#endif

	p = os_get_path(filename, gbl(pgm_path), cmdpath);
	if (p == 0)
	{
#if V_WINDOWS
		strcpy(filename, e);
		strcat(filename, ".bat");
		p = os_get_path(filename, gbl(pgm_path), cmdpath);
		if (p == 0)
		{
			int rc;

			/* look for DDE association */

			rc = os_dde_cmd_get(fullpath, buffer);
			if (rc == 0)
			{
				xsystem(buffer, "", FALSE);

				if (*temppath != 0)
					os_file_delete(temppath);
				return;
			}
		}
#endif
			return;
	}

	if (os_stat(p, &stbuf) || !can_we_exec(&stbuf))
	{
		errmsg(ER_NPTE, "", ERR_ANY);

		if (*temppath != 0)
			os_file_delete(temppath);
		return;
	}

	/*----------------------------------------------------------------
	 * exec the cmd "<script> <fullpath> <dir> <filename> <parent-dir>"
	 */
	fn_dirname(fullpath,  directory);
	fn_dirname(directory, parent);

	fn_get_abs_path(gbl(pgm_dir), cmdpath, abs_cmd_path);

	*buffer = 0;
#if V_WINDOWS
	strcpy(buffer, "cmd.exe /c ");
#endif
	strcat(buffer, abs_cmd_path);
	strcat(buffer, " \"");
	strcat(buffer, fullpath);
	strcat(buffer, "\" \"");
	strcat(buffer, directory);
	strcat(buffer, "\" \"");
	strcat(buffer, fn_basename(fullpath));
	strcat(buffer, "\" \"");
	strcat(buffer, parent);
	strcat(buffer, "\"");

	xsystem(buffer, directory, TRUE);

	if (*temppath != 0)
		os_file_delete(temppath);
}
