/*------------------------------------------------------------------------
 * UNIX filename parsing
 */
#include "ftpcommon.h"

/*------------------------------------------------------------------------
 * ftp_parse_unix_ls_entry() - parse a UNIX ls entry
 */
FTP_STAT *ftp_parse_unix_ls_entry (FTP_NODE *ftp_ptr,
	char *ls_line, const char *dir_name, FTP_STAT *stat)
{
	char filename[260];
	char permissions[50];
	char month[50];
	char owner[50];
	char group[50];
	char *lptr;
	char *ptr;
	struct tm *tm_ptr;
	time_t seconds;
	int ino, yy, mo, day, hour, mn, sec, links;
	long size;
	int retn;
	int Minor;
	int Major;
	int entry_type;
	int is_special;
	int i;

	/* strip all trailing white-space */

	strip(ls_line);

	/* skip total line */

	if (strncmp(ls_line, "total", 5) == 0)
		return ((FTP_STAT *)NULL);

	/* get file type */

	for (lptr = ls_line; *lptr; lptr++)
	{
		/* skip leading spaces */

		if (! isspace(*lptr))
			break;
	}

	for (ptr=lptr; *ptr; ptr++)
	{
		/* skip inode #, spaces */

		if (! isspace(*ptr) && ! isdigit(*ptr))
			break;
	}

	entry_type = *ptr;
	is_special = ! (entry_type == '-' ||
			entry_type == 'D' ||
			entry_type == 'p' ||
			entry_type == 'n' ||
			entry_type == 'd' ||
			entry_type == 's' ||
			entry_type == 'l');

	memset(stat, 0, sizeof(*stat));

	/* parse out line into components */

	Minor = Major = size = 0;

	if (isdigit(*lptr))
	{
		ino = atoi(lptr);

		for (; *lptr; lptr++)
		{
			if (isspace(*lptr))
				break;
		}

		for (; *lptr; lptr++)
		{
			if (! isspace(*lptr))
				break;
		}
	}
	else
	{
		ino = 0;
	}

	if (is_special)
		sscanf(lptr, "%10s %d %s %s %d, %d %s %d %d %s",
			permissions, &links, owner, group, &Major, &Minor,
			month, &day, &yy, filename);
	else
		sscanf(lptr, "%10s %d %s %s %ld %s %d %d %s",
			permissions, &links, owner, group, &size,
			month, &day, &yy, filename);

	if (yy < 1970)
	{
		if (is_special)
			sscanf(lptr, "%10s %d %s %s %d, %d %s %d %d:%d %s",
				permissions, &links, owner, group, &Major, &Minor,
				month, &day, &hour, &mn, filename);
		else
			sscanf(lptr, "%10s %d %s %s %ld %s %d %d:%d %s",
				permissions, &links, owner, group, &size,
				month, &day, &hour, &mn, filename);

		/* year == current year */
		time(&seconds);
		tm_ptr = gmtime(&seconds);
		yy = tm_ptr->tm_year + 1900;
	}
	else
	{
		/* time not given .... just month, day, year */
		hour = 0;
		mn = 0;
	}
	sec = 0;

	str_rem_ws(filename);
	str_rem_ws(owner);
	str_rem_ws(group);
	str_rem_ws(month);

	mo = ftp_find_month(month);

	if (permissions[0] == '-')
	{
		char cmd_str[128];

		/* regular file */

		/* get actual modification time of file if we can */

		fn_convert_dirname(dir_name, 0, FN_UNIX, cmd_str);
		strcat(cmd_str, "/");
		strcat(cmd_str, filename);
		retn = ftp_cmd(ftp_ptr, "MDTM %s", cmd_str);
		if (retn == COMPLETE)
		{
			sscanf(ftp_ptr->reply_string, "%*s %04d%02d%02d%02d%02d%02d",
				&yy, &mo, &day, &hour, &mn, &sec);
		}
	}

	if (permissions[0] == 'l')
	{
		/* sym link */

		ptr = strrchr(lptr, ' ');
		strcpy(stat->sym_name, ptr+1);
	}

	strcpy(stat->file_name, filename);
	stat->stbuf.st_dev   = 0;
	i = (sizeof(stat->stbuf.st_rdev) * 4);
	stat->stbuf.st_rdev  = Minor | (Major << i);
	stat->stbuf.st_ino   = ino;
	stat->stbuf.st_nlink = links;
	stat->stbuf.st_uid   = ftp_get_owner_ptr(ftp_ptr, owner);
	stat->stbuf.st_gid   = ftp_get_group_ptr(ftp_ptr, group);
	stat->stbuf.st_size  = size;
	stat->stbuf.st_mode  = ftp_get_file_stat_mode(permissions);
	stat->stbuf.st_mtime =
	stat->stbuf.st_atime =
	stat->stbuf.st_ctime =
		ftp_calc_secs_from_date(yy, mo, day, hour, mn, sec);

	return (stat);
}
