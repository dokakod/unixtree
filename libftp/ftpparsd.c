/*------------------------------------------------------------------------
 * DOS filename parsing
 */
#include "ftpcommon.h"

/*------------------------------------------------------------------------
 * ftp_parse_dos_ls_entry() - parse a DOS ls entry
 */
FTP_STAT *ftp_parse_dos_ls_entry (FTP_NODE *ftp_ptr,
	char *ls_line, const char *dir_name, FTP_STAT *stat)
{
	char filename[260];
	char permissions[50];
	char *ptr;
	int yy, mo, day, hour, mn, sec, size;
	int have_directory;
	char am_pm;

	if (ls_line[0] == ' ')
	{
		return ((FTP_STAT *)NULL);
	}

	memset(stat, 0, sizeof(*stat));

	have_directory = 0;
	if ((ptr = strstr(&ls_line[13], "<DIR>")) != (char *)NULL)
	{
		/* directory */
		ptr += 5;
		strncpy(filename, ls_line, 13);
		sscanf(ptr, "%d-%d-%d %d:%d%c",
			&mo, &day, &yy, &hour, &mn, &am_pm);
		size = 0;
		have_directory = 1;
	}
	else
	{
		/* file */
		sscanf(ls_line, "%13s %d %d-%d-%d %d:%d%c%s",
			filename, &size, &mo, &day, &yy, &hour, &mn, &am_pm, permissions);
	}
	if (filename[0] == ' ')
	{
		return ((FTP_STAT *)NULL);
	}

	if (am_pm == 'p')
	{
		hour += 12;
	}
	yy += 1900;
	sec = 0;

	/* get rid of blanks in filename */
	if ((ptr = strchr(filename, ' ')) != (char *)NULL)
	{
		*ptr = '\0';
	}
	if (!strcmp(filename, ".."))
	{
		return ((FTP_STAT *)NULL);
	}

	for (ptr=filename; *ptr; ptr++)
		*ptr = tolower(*ptr);

	for (ptr=permissions; *ptr; ptr++)
		*ptr = tolower(*ptr);

	if (have_directory)
	{
		/* directory */

		if (strchr(permissions, 'h') != (char *)NULL)
		{
			/* hidden */
			strcpy(permissions, "d--x--x--x");
		}
		else
		{
			/* regular */
			strcpy(permissions, "drwxrwxrwx");
		}
	}
	else
	{
		/* file */

		if (strchr(permissions, 'r') != (char *)NULL ||
			strchr(permissions, 's') != (char *)NULL)
		{
			/* read only */
			strcpy(permissions, "-r--r--r--");
		}
		else if (strchr(permissions, 'h') != (char *)NULL)
		{
			/* hidden */
			strcpy(permissions, "----------");
		}
		else
		{
			/* regular */
			strcpy(permissions, "-rw-rw-rw-");
		}

		if (strstr(filename, ".com") != (char *)NULL ||
			strstr(filename, ".bat") != (char *)NULL ||
			strstr(filename, ".exe") != (char *)NULL)
		{
			/* executable */
			permissions[3] = 'x';
			permissions[6] = 'x';
			permissions[9] = 'x';
		}
	}

	strcpy(stat->file_name, filename);
	stat->stbuf.st_dev   = 0;
	stat->stbuf.st_rdev  = 0;
	stat->stbuf.st_ino   = 0;
	stat->stbuf.st_nlink = 1;
	stat->stbuf.st_uid   = 0;
	stat->stbuf.st_gid   = 0;
	stat->stbuf.st_size  = size;
	stat->stbuf.st_mode  = ftp_get_file_stat_mode(permissions);
	stat->stbuf.st_mtime =
	stat->stbuf.st_atime =
	stat->stbuf.st_ctime =
		ftp_calc_secs_from_date(yy, mo, day, hour, mn, sec);

	return (stat);
}
