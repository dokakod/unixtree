/*------------------------------------------------------------------------
 * VMS filename parsing
 */
#include "ftpcommon.h"

/*------------------------------------------------------------------------
 * ftp_parse_vms_ls_entry() - parse a VMS ls entry
 */
FTP_STAT *ftp_parse_vms_ls_entry (FTP_NODE *ftp_ptr,
	char *ls_line, const char *dir_name, FTP_STAT *stat)
{
	char filename[260];
	char owner[50];
	char group[50];
	char size_str[50];
	char date_str[50];
	char time_str[50];
	char owgr_str[50];
	char perm_str[50];
	int yy, mo, day, hour, mn, sec;
	const char *l;
	char *p;
	int i;
	int j;
	int mode;

	struct perm
	{
		int chr;
		int mde[4];
	};
	static const struct perm ftp_perms[] =
	{
		{ 'R', { 0, S_IRGRP, S_IRUSR, S_IROTH } },
		{ 'W', { 0, S_IWGRP, S_IWUSR, S_IWOTH } },
		{ 'E', { 0, S_IXGRP, S_IXUSR, S_IXOTH } },
		{ 'D', { 0, 0,       0,       0       } }
	};

	if (strchr(ls_line, '(') == (char *)NULL)
		return ((FTP_STAT *)NULL);

	memset(stat, 0, sizeof(*stat));

	/* skip over leading spaces */
	for (l=ls_line; *l; l++)
	{
		if (! isspace(*l))
			break;
	}

	/* get filename */
	p = filename;
	for (; *l; l++)
	{
		if (isspace(*l))
			break;
		*p++ = *l;
	}
	*p = 0;
	for (; *l; l++)
	{
		if (! isspace(*l))
			break;
	}

	/* get size */
	p = size_str;
	for (; *l; l++)
	{
		if (isspace(*l))
			break;
		*p++ = *l;
	}
	*p = 0;
	for (; *l; l++)
	{
		if (! isspace(*l))
			break;
	}

	/* get date */
	date_str[0] = '0';
	p = date_str+1;
	for (; *l; l++)
	{
		if (isspace(*l))
			break;
		*p++ = *l;
	}
	*p = 0;
	for (; *l; l++)
	{
		if (! isspace(*l))
			break;
	}

	/* get time */
	p = time_str;
	for (; *l; l++)
	{
		if (isspace(*l))
			break;
		*p++ = *l;
	}
	*p = 0;
	for (; *l; l++)
	{
		if (! isspace(*l))
			break;
	}

	/* get owners */
	p = owgr_str;
	for (; *l; l++)
	{
		if (isspace(*l))
			break;
		*p++ = *l;
	}
	*p = 0;
	for (; *l; l++)
	{
		if (! isspace(*l))
			break;
	}

	/* get permissions */
	p = perm_str;
	for (; *l; l++)
	{
		if (isspace(*l))
			break;
		*p++ = *l;
	}
	*p = 0;
	for (; *l; l++)
	{
		if (! isspace(*l))
			break;
	}

	/* process filename */
#if 0
	p = strchr(filename, ';');
	if (p != (char *)NULL)
		*p = 0;
#endif

	for (p=filename; *p; p++)
		*p = tolower(*p);

	/* process date */
	p = date_str;
	if (date_str[3] == '-')
		p++;
	day = ((p[0] - '0') * 10) + (p[1] - '0');
	mo  = ftp_find_month(p+3);
	yy  = atoi(p+7);

	/* process time */
	hour = ((time_str[0] - '0') * 10) + (time_str[1] - '0');
	mn   = ((time_str[3] - '0') * 10) + (time_str[4] - '0');
	sec  = 0;

	/* process owner & group */
	l = owgr_str;
	p = group;
	for (l++; *l; l++)
	{
		if (*l == ',' || *l == ']')
			break;
		*p++ = *l;
	}
	*p = 0;
	p = owner;
	if (*l == ',')
		l++;
	for (; *l; l++)
	{
		if (*l == ']')
			break;
		*p++ = *l;
	}
	*p = 0;
	if (!*owner)
	{
		strcpy(owner, group);
	}

	for (p=owner; *p; p++)
		*p = tolower(*p);

	for (p=group; *p; p++)
		*p = tolower(*p);

	/* process permissions */
	mode = 0;
	i = 0;
	for (l=perm_str+1; *l; l++)
	{
		if (*l == ')')
			break;
		if (*l == ',')
		{
			i++;
			continue;
		}

		for (j=0; j<4; j++)
		{
			if (ftp_perms[j].chr == *l)
			{
				mode |= ftp_perms[j].mde[i];
				break;
			}
		}
	}
	p = strstr(filename, ".dir");
	if (p)
	{
		*p = 0;
		mode |= S_IFDIR;
	}
	else
	{
		mode |= S_IFREG;
	}

	/* create entry */
	strcpy(stat->file_name, filename);
	stat->stbuf.st_dev   = 0;
	stat->stbuf.st_rdev  = 0;
	stat->stbuf.st_ino   = 0;
	stat->stbuf.st_nlink = 1;
	stat->stbuf.st_uid   = ftp_get_owner_ptr(ftp_ptr, owner);
	stat->stbuf.st_gid   = ftp_get_group_ptr(ftp_ptr, group);
	stat->stbuf.st_size  = atol(size_str);
	stat->stbuf.st_mode  = mode;
	stat->stbuf.st_mtime =
	stat->stbuf.st_atime =
	stat->stbuf.st_ctime =
		ftp_calc_secs_from_date(yy, mo, day, hour, mn, sec);

	return (stat);
}
