/*------------------------------------------------------------------------
 * toolkit resource file processing
 *
 * These routines will process two different types of resource files:
 * X-resource files and INI-resource files.  Both are text files, and
 * may contain optional "package" info which qualify entries.  An
 * entry contains a "name" and a "value".  Any "package" or "name"
 * specified is treated case-insensitive, but "value" is not.
 *
 * X-resource files are in the form:
 *
 *	<name>: <value>
 *	*<name>: <value>
 *	.<name>: <value>
 *
 *	<package>.<name>: <value>
 *	<package>*<name>: <value>
 *
 *	Lines with no "package" qualifier apply to any application.
 *	Lines with a "package" qualifier apply to those applications which
 *	specify that "package".
 *
 * INI-resource files are in the form:
 *
 *	[<package>]
 *	<name> = <value>
 *
 *	Any "<name> = <value>" entries found before any "[<package>]" lines
 *	or any "<name> = <value>" lines following a "[common]" line apply
 *	to all applications.
 */
#include "xvtcommon.h"

/*------------------------------------------------------------------------
 * list of files to process for resources
 *
 * These are read first.
 */
static const char *	xvt_tk_res_files[] =
{
	XVT_RESOURCE_FILES,
	0
};

/*------------------------------------------------------------------------
 * list of directories to look for resource file in
 *
 * File looked for is "<dir>/<resource-name>"
 *
 * These are read after all files above.
 */
static const char *	xvt_tk_res_dirs[] =
{
	XVT_RESOURCE_DIRS,
	0
};

/*------------------------------------------------------------------------
 * list of paths to look for resource file in
 *
 * File looked for is "<dir>/<resource-name>"
 *
 * These are read after the dirs above.
 */
static const char *	xvt_tk_res_paths[] =
{
	XVT_RESOURCE_PATHS,
	0
};

/*------------------------------------------------------------------------
 * INI file common entry
 */
#define INI_COMMON		"common"

/*------------------------------------------------------------------------
 * process a resource file data entry
 *
 * returns: 0 if used, -1 if not
 */
static int xvt_tk_resdata (XVT_DATA *xd, const char *name, const char *data,
	const char *resname)
{
	const TK_OPTIONS *	op;

	/*--------------------------------------------------------------------
	 * Check if this name is known.
	 */
	op = xvt_tk_find_by_resname(name);
	if (op == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * ignore this entry if already set
	 */
	switch (op->opt_type)
	{
	case XVT_TK_OPT_STR:
	case XVT_TK_OPT_CLR:
	case XVT_TK_OPT_FNT:
		{
			const char ** pstr = (const char **)((char *)xd + op->opt_offset);

			if (*pstr != 0)
				return (-1);
		}
		break;

	case XVT_TK_OPT_NUM:
	case XVT_TK_OPT_BLN:
		{
			int * pnum = (int *)((char *)xd + op->opt_offset);

			if (*pnum != -1)
				return (-1);
		}
		break;

	case XVT_TK_OPT_DBG:
	case XVT_TK_OPT_CMD:
		/*----------------------------------------------------------------
		 * these entries not allowed in a resource file
		 */
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * OK - set this entry
	 */
	xvt_tk_opt_set(xd, op->opt_string, data, FALSE, 0);

	return (0);
}

/*------------------------------------------------------------------------
 * load a resource file
 */
static int xvt_tk_resfile_load_x (XVT_DATA *xd, const char *path,
	const char *resname)
{
	FILE *	fp;

	/*--------------------------------------------------------------------
	 * open the resource file
	 *
	 * Note that a non-existent file is OK.
	 */
	fp = fopen(path, "r");
	if (fp == 0)
		return (XVT_OK);

	/*--------------------------------------------------------------------
	 * process all lines in the file
	 */
	while (TRUE)
	{
		char	line[BUFSIZ];
		char *	lp;
		char *	p;
		char *	s;
		char *	vn;
		int		rc;

		/*----------------------------------------------------------------
		 * read in the next line
		 *
		 * For now, we aren't dealing with continued lines
		 */
		if (fgets(line, sizeof(line), fp) == 0)
			break;

		/*----------------------------------------------------------------
		 * Strip leading/trailing white-space & remove any comments.
		 */
		lp = xvt_strip(line, TRUE);

		p = xvt_strchr(lp, '!');
		if (p != 0)
		{
			*p = 0;
			xvt_strip(lp, FALSE);
		}

		/*----------------------------------------------------------------
		 * Ignore line if only a comment (or blank) line
		 */
		if (*lp == 0)
			continue;

		/*----------------------------------------------------------------
		 * check for include line
		 *
		 * Note: relative pathnames are relative to dir of prev path
		 */
		if (*lp == '#')
		{
			char	inc_name[XVT_MAX_PATHLEN];
			char	inc_path[XVT_MAX_PATHLEN];

			if (xvt_strccmpn("#include", lp) == 0)
			{
				p = strchr(lp, '"');
				if (p != 0)
				{
					xvt_strcpy(inc_path, p);
					if (*inc_path != '/')
					{
						strcpy(inc_name, inc_path);
						xvt_path_dirname(inc_path, path);
						strcat(inc_path, "/");
						strcat(inc_path, inc_name);
					}
					xvt_tk_resfile_load(xd, inc_path, resname);
				}
			}
			continue;
		}

		/*----------------------------------------------------------------
		 * If not in form of "<name>:<data>", ignore the line.
		 */
		p = xvt_strchr(lp, ':');
		if (p == 0)
			continue;

		*p++ = 0;
		xvt_strip(lp, FALSE);
		p = xvt_strip(p, TRUE);

		/*----------------------------------------------------------------
		 * <name> *should* be in one of the following forms:
		 *
		 *		<var-name>
		 *		*<var-name>
		 *		.<var-name>
		 *		<resname>.<var-name>
		 *		<resname>*<var-name>
		 *
		 * Any other forms are considered invalid.
		 */
		s = xvt_strpbrk(lp, "*.");
		if (s != 0)
		{
			/*------------------------------------------------------------
			 * second level is invalid
			 */
			if (xvt_strpbrk(s+1, ".*") != 0)
				continue;

			/*------------------------------------------------------------
			 * if not at beginning, check resname
			 */
			if (s != lp)
			{
				if (resname == 0)
					continue;

				if (xvt_strnccmp(lp, resname, s-lp) != 0)
					continue;
			}

			vn = (s+1);
		}
		else
		{
			vn = lp;
		}

		/*----------------------------------------------------------------
		 * we got a real line - process it
		 */
		rc = xvt_tk_resdata(xd, vn, p, resname);

		/*----------------------------------------------------------------
		 * If we used this entry, check if resource file debugging is on
		 */
		if (rc == 0 && xd->resfil_fp != 0)
		{
			fprintf(xd->resfil_fp, "%s: %s: %s\n", path, lp, p);
		}
	}

	fclose(fp);
	return (0);
}

static int xvt_tk_resfile_load_i (XVT_DATA *xd, const char *path,
	const char *resname)
{
	char	package[256];
	FILE *	fp;
	int		skip = FALSE;

	/*--------------------------------------------------------------------
	 * open the resource file
	 *
	 * Note that a non-existent file is OK.
	 */
	fp = fopen(path, "r");
	if (fp == 0)
		return (XVT_OK);

	/*--------------------------------------------------------------------
	 * process all lines in the file
	 */
	strcpy(package, INI_COMMON);
	if (resname == 0)
		resname = "";

	while (TRUE)
	{
		char	line[BUFSIZ];
		char *	lp;
		char *	p;
		int		rc;

		/*----------------------------------------------------------------
		 * read in the next line
		 *
		 * For now, we aren't dealing with continued lines
		 */
		if (fgets(line, sizeof(line), fp) == 0)
			break;

		/*----------------------------------------------------------------
		 * Strip leading/trailing white-space & remove any comments.
		 */
		lp = xvt_strip(line, TRUE);

		p = xvt_strpbrk(lp, "#;!");
		if (p != 0)
		{
			*p = 0;
			xvt_strip(lp, FALSE);
		}

		/*----------------------------------------------------------------
		 * Ignore line if only a comment (or blank) line
		 */
		if (*lp == 0)
			continue;

		/*----------------------------------------------------------------
		 * Check if this is a package entry
		 */
		if (*lp == '[')
		{
			p = strchr(++lp, ']');
			if (p == 0)
				continue;

			*p = 0;
			strcpy(package, lp);
			if (xvt_strccmp(package, INI_COMMON) == 0 ||
			    xvt_strccmp(package, resname)    == 0)
			{
				skip = FALSE;
			}
			else
			{
				skip = TRUE;
			}
			continue;
		}

		/*----------------------------------------------------------------
		 * check if we are skipping this section
		 */
		if (skip)
			continue;

		/*----------------------------------------------------------------
		 * If not in form of "<name>=<data>", ignore the line.
		 */
		p = xvt_strchr(lp, '=');
		if (p == 0)
			continue;

		*p++ = 0;
		xvt_strip(lp, FALSE);
		p = xvt_strip(p, TRUE);

		/*----------------------------------------------------------------
		 * we got a real line - process it
		 */
		rc = xvt_tk_resdata(xd, lp, p, 0);

		/*----------------------------------------------------------------
		 * If we used this entry, check if resource file debugging is on
		 */
		if (rc == 0 && xd->resfil_fp != 0)
		{
			fprintf(xd->resfil_fp, "%s: %s: %s\n", path, lp, p);
		}
	}

	fclose(fp);
	return (0);
}

int xvt_tk_resfile_load (XVT_DATA *xd, const char *path,
	const char *resname)
{
	char			res_path[XVT_MAX_PATHLEN];
	const char *	ext;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (xd == 0)
		return (-1);

	if (path == 0 || *path == 0)
	{
		sprintf(xd->error_msg, "NULL path");
		xd->error_num = XVT_ERR_INVARG;
		return (-1);
	}

	path = xvt_path_resolve(res_path, path);
	if (path == 0)
		return (0);

	/*--------------------------------------------------------------------
	 * check extension of file
	 */
	ext = xvt_path_extension(path);
	if (ext != 0 && xvt_strccmp(ext, "ini") == 0)
		return xvt_tk_resfile_load_i(xd, path, resname);
	else
		return xvt_tk_resfile_load_x(xd, path, resname);
}

/*------------------------------------------------------------------------
 * save all data to a resource file
 */
static int xvt_tk_resfile_save_x (XVT_DATA *xd, const char *path,
	const char *resname)
{
	const TK_OPTIONS *	op;
	char				res_temp[XVT_MAX_PATHLEN];
	char				resbuf[128];
	char				data_buf[256];
	FILE *				fp_inp;
	FILE *				fp_out;
	char *				p;
	const char *		s;

	/*--------------------------------------------------------------------
	 * update or rewrite?
	 */
	if (resname == 0 || *resname == 0)
	{
		/*----------------------------------------------------------------
		 * rewrite entire file
		 */
		{
			/*------------------------------------------------------------
			 * open resource file
			 */
			fp_out = fopen(path, "w");
			if (fp_out == 0)
			{
				sprintf(xd->error_msg, "cannot open file %s", path);
				xd->error_num = XVT_ERR_INVARG;
				return (-1);
			}

			/*------------------------------------------------------------
			 * write all set values as "<name>: <value>"
			 */
			for (op=xvt_tk_options; op->opt_type; op++)
			{
				p = xvt_tk_data_value(xd, op, FALSE, data_buf);
				if (p != 0)
				{
					fprintf(fp_out, "%s: %s\n", op->opt_resname, p);
				}
			}
			fclose(fp_out);
		}
	}
	else
	{
		/*----------------------------------------------------------------
		 * update file
		 */
		{
			/*------------------------------------------------------------
			 * get resource name as all lower-case
			 */
			for (p=resbuf, s=resname; *s; s++)
			{
				*p++ = tolower(*s);
			}
			*p = 0;

			/*------------------------------------------------------------
			 * open resource file as input (failure is OK)
			 */
			fp_inp = fopen(path, "r");

			/*------------------------------------------------------------
			 * open temp output file
			 */
			strcpy(res_temp, path);
			strcat(res_temp, ".tmp");
			fp_out = fopen(res_temp, "w");
			if (fp_out == 0)
			{
				if (fp_inp != 0)
					fclose(fp_inp);

				sprintf(xd->error_msg, "cannot open file %s", res_temp);
				xd->error_num = XVT_ERR_INVARG;
				return (-1);
			}

			/*------------------------------------------------------------
			 * copy all lines from orig file which don't contain resname
			 */
			while (fp_inp != 0)
			{
				char	line[BUFSIZ];

				if (fgets(line, sizeof(line), fp_inp) == 0)
					break;

				if (xvt_strstr(line, resbuf) == 0)
					fputs(line, fp_out);
			}
			if (fp_inp != 0)
				fclose(fp_inp);

			/*------------------------------------------------------------
			 * write all set entries as "<resname>.<name>: <value>"
			 */
			for (op=xvt_tk_options; op->opt_type; op++)
			{
				p = xvt_tk_data_value(xd, op, FALSE, data_buf);
				if (p != 0)
				{
					fprintf(fp_out, "%s.%s: %s\n", resbuf, op->opt_resname, p);
				}
			}
			fclose(fp_out);

			/*------------------------------------------------------------
			 * rename "<resfile>.tmp" as "<resfile>"
			 */
			if (rename(res_temp, path) < 0)
			{
				sprintf(xd->error_msg, "Cannot rename %s as %s",
					res_temp, path);
				xd->error_num = XVT_ERR_INVARG;
				return (-1);
			}
		}
	}

	return (0);
}

static int xvt_tk_resfile_save_i (XVT_DATA *xd, const char *path,
	const char *resname)
{
	const TK_OPTIONS *	op;
	char				res_temp[XVT_MAX_PATHLEN];
	char				resbuf[128];
	char				data_buf[256];
	FILE *				fp_inp;
	FILE *				fp_out;
	char *				p;
	const char *		s;
	int					count;
	int					skip;

	/*--------------------------------------------------------------------
	 * check if resname specified & copy as all upper-case
	 */
	if (resname == 0)
		resname = INI_COMMON;

	p = resbuf;
	*p++ = '[';
	for (s=resname; *s; s++)
	{
		*p++ = toupper(*s);
	}
	*p++ = ']';
	*p = 0;

	resname = resbuf;

	/*--------------------------------------------------------------------
	 * open resource file as input (failure is OK)
	 */
	fp_inp = fopen(path, "r");

	/*--------------------------------------------------------------------
	 * open temp output file
	 */
	strcpy(res_temp, path);
	strcat(res_temp, ".tmp");
	fp_out = fopen(res_temp, "w");
	if (fp_out == 0)
	{
		if (fp_inp != 0)
			fclose(fp_inp);

		sprintf(xd->error_msg, "cannot open file %s", res_temp);
		xd->error_num = XVT_ERR_INVARG;
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * If input file exists, copy all sections except the relevant one
	 */
	count = 0;
	skip  = FALSE;

	if (fp_inp != 0)
	{
		while (TRUE)
		{
			char	line[BUFSIZ];
			char *	lp;

			if (fgets(line, sizeof(line), fp_inp) == 0)
				break;

			lp = xvt_strip(line, TRUE);
			if (*lp == '[')
			{
				if (xvt_strccmp(resbuf, lp) == 0)
					skip = TRUE;
				else
					skip = FALSE;
			}

			if (! skip)
			{
				fprintf(fp_inp, "%s\n", lp);
				count++;
			}
		}

		fclose(fp_inp);
	}

	/*--------------------------------------------------------------------
	 * write out section header
	 */
	if (count > 0)
		fprintf(fp_out, "\n");

	fprintf(fp_out, "[%s]\n", resname);

	/*--------------------------------------------------------------------
	 * write all set entries as "<name> = <value>"
	 */
	for (op=xvt_tk_options; op->opt_type; op++)
	{
		p = xvt_tk_data_value(xd, op, FALSE, data_buf);
		if (p != 0)
		{
			fprintf(fp_out, "%s = %s\n", op->opt_resname, p);
		}
	}
	fclose(fp_out);

	/*--------------------------------------------------------------------
	 * rename "<resfile>.tmp" as "<resfile>"
	 */
	if (rename(res_temp, path) < 0)
	{
		sprintf(xd->error_msg, "Cannot rename %s as %s",
			res_temp, path);
		xd->error_num = XVT_ERR_INVARG;
		return (-1);
	}

	return (0);
}

int xvt_tk_resfile_save (XVT_DATA *xd, const char *path,
	const char *resname)
{
	char			res_path[XVT_MAX_PATHLEN];
	const char *	ext;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (xd == 0)
		return (-1);

	if (path == 0 || *path == 0)
	{
		sprintf(xd->error_msg, "NULL path");
		xd->error_num = XVT_ERR_INVARG;
		return (-1);
	}

	path = xvt_path_resolve(res_path, path);
	if (path == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * check extension of file
	 */
	ext = xvt_path_extension(path);
	if (ext != 0 && xvt_strccmp(ext, "ini") == 0)
		return xvt_tk_resfile_save_i(xd, path, resname);
	else
		return xvt_tk_resfile_save_x(xd, path, resname);
}

/*------------------------------------------------------------------------
 * process all known resources
 *
 * Note: We assume that all cmd-line options have already been processed,
 * and those are the *most* specific.  We then go from *more* to *less*
 * specific sources, and we don't store an entry if it already has a value.
 *
 * We initialized string pointers to 0 and all bool/num values to -1,
 * so we know if a value has already been stored in it.
 */
int xvt_tk_resource (XVT_DATA *xd)
{
	const char *	resname;
	const char **	p;
	const char **	list;
	char			res_skel[XVT_MAX_PATHLEN];
	char			res_path[XVT_MAX_PATHLEN];
	int				rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (xd == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * check if a resource string was specified
	 */
	if (xd->resstr != 0 && *xd->resstr != 0)
	{
		rc = xvt_tk_optlist(xd, xd->resstr);
		if (rc)
			return (rc);
	}

	/*--------------------------------------------------------------------
	 * get resource name to use
	 */
	if (xd->resname != 0 && *xd->resname != 0)
		resname = xd->resname;
	else if (xd->appname != 0 && *xd->appname != 0)
		resname = xd->appname;
	else
		resname = XVT_S_OPT_RESNAME;

	/*--------------------------------------------------------------------
	 * check if a resource file was specified
	 */
	if (xd->resfile != 0)
	{
		xvt_path_resolve(res_path, xd->resfile);
		if (res_path != 0)
			xvt_tk_resfile_load(xd, res_path, resname);
		return (XVT_OK);
	}

	/*--------------------------------------------------------------------
	 * process all files first
	 */
	list = xd->resfiles;
	if (list == 0)
		list = xvt_tk_res_files;

	for (p=list; *p; p++)
	{
		xvt_path_resolve(res_path, *p);
		if (*res_path != 0)
			xvt_tk_resfile_load(xd, res_path, resname);
	}

	/*--------------------------------------------------------------------
	 * now process all dirs
	 */
	list = xd->resdirs;
	if (list == 0)
		list = xvt_tk_res_dirs;

	for (p=list; *p; p++)
	{
		sprintf(res_skel, "%s/%s", *p, resname);
		xvt_path_resolve(res_path, res_skel);
		if (*res_path != 0)
		{
			xvt_tk_resfile_load(xd, res_path, resname);
			strcat(res_path, ".ini");
			xvt_tk_resfile_load(xd, res_path, resname);
		}
	}

	/*--------------------------------------------------------------------
	 * now process all paths
	 */
	list = xd->respaths;
	if (list == 0)
		list = xvt_tk_res_paths;

	for (p=list; *p; p++)
	{
		const char * path = *p;
		const char * ptr;

		if (*path == '$')
		{
			path = getenv(path+1);
			if (path == 0 || *path == 0)
				continue;
		}

		ptr = path;
		while (*ptr != 0)
		{
			char *	t;

			for (t=res_skel; *ptr; ptr++)
			{
				if (*ptr == ':')
					break;
				*t++ = *ptr;
			}
			*t = 0;

			if (*ptr == ':')
				ptr++;

			if (*res_skel == 0)
				strcpy(res_skel, ".");
			strcat(res_skel, "/");
			strcat(res_skel, resname);
			xvt_path_resolve(res_path, res_skel);
			if (*res_path != 0)
			{
				xvt_tk_resfile_load(xd, res_path, resname);
				strcat(res_path, ".ini");
				xvt_tk_resfile_load(xd, res_path, resname);
			}
		}
	}

	return (XVT_OK);
}

/*------------------------------------------------------------------------
 * dump all resources
 */
void xvt_dump_resources (FILE *fp, int verbose, const char *resname)
{
	char			res_path[XVT_MAX_PATHLEN];
	const char *	env;
	const char **	lp;

	if (resname == 0 || *resname == 0)
		resname = XVT_S_OPT_RESNAME;

	fprintf(fp, "Files processed:\n");
	for (lp=xvt_tk_res_files; *lp; lp++)
	{
		if (verbose)
		{
			xvt_path_resolve(res_path, *lp);
			fprintf(fp, "  %-40s  %s\n", *lp, res_path);
		}
		else
		{
			fprintf(fp, "  %s\n", *lp);
		}
	}

	fprintf(fp, "Directories searched (for %s or %s.ini):\n", resname, resname);
	for (lp=xvt_tk_res_dirs; *lp; lp++)
	{
		if (verbose)
		{
			xvt_path_resolve(res_path, *lp);
			fprintf(fp, "  %-40s  %s\n", *lp, res_path);
		}
		else
		{
			fprintf(fp, "  %s\n", *lp);
		}
	}

	fprintf(fp, "Paths processed:\n");
	for (lp=xvt_tk_res_paths; *lp; lp++)
	{
		if (verbose)
		{
			if ((*lp)[0] == '$')
			{
				env = getenv((*lp)+1);
				if (env == 0)
					env = "";
			}
			else
			{
				env = *lp;
			}
			fprintf(fp, "  %-40s %s\n", *lp, env);
		}
		else
		{
			fprintf(fp, "  %s\n", *lp);
		}
	}
}
