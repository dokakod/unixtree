/*------------------------------------------------------------------------
 * filename list parsing
 */
#include "ftpcommon.h"

/*------------------------------------------------------------------------
 * month info
 */
static const char *ftp_months[] =
{
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};

static const int ftp_days_in_months[] =
{
	31, /* Jan */
	28, /* Feb */
	31, /* Mar */
	30, /* Apr */
	31, /* May */
	30, /* Jun */
	31, /* Jul */
	31, /* Aug */
	30, /* Sep */
	31, /* Oct */
	30, /* Nov */
	31  /* Dec */
};

#define leap_year(year) (!(year % 4) && (year % 100))

/*------------------------------------------------------------------------
 * ftp_parse_directory_list() - parse a directory listing
 */
int ftp_parse_directory_list (FTP_NODE *ftp_ptr, const char *dir_name,
	int (*logrtn)(FTP_NODE *f, FTP_STAT *s, void *data), void *data)
{
	FILE *fp;
	FTP_STAT stat;
	FTP_STAT *ftp_stat;
	char ls_line[BUFSIZ];
	int linecnt = 0;
	int l;

	fp = fopen(ftp_ptr->tempname, "r");
	if (fp == (FILE *)NULL)
	{
		return (-1);
	}

	while (fgets(ls_line, BUFSIZ, fp) != (char *)NULL)
	{
		linecnt++;

		l = strlen(ls_line) - 1;
		for (; l>=0; l--)
		{
			if (! isspace(ls_line[l]))
				break;
		}
		ls_line[l+1] = 0;

		if (ftp_ptr->debug)
		{
			fprintf(ftp_ptr->debug, "<=== %s\n", ls_line);
			fflush(ftp_ptr->debug);
		}

		/* parse out reply string */

		if (ftp_ptr->server_type == UNIX_SERVER)
		{
			ftp_stat = ftp_parse_unix_ls_entry (ftp_ptr, ls_line, dir_name,
				&stat);
		}
		else if (ftp_ptr->server_type == MSDOS_SERVER)
		{
			ftp_stat = ftp_parse_dos_ls_entry (ftp_ptr, ls_line, dir_name,
				&stat);
		}
		else if (ftp_ptr->server_type == VMS_SERVER)
		{
			ftp_stat = ftp_parse_vms_ls_entry (ftp_ptr, ls_line, dir_name,
				&stat);
		}
		else
		{
			ftp_stat = (FTP_STAT *)NULL;
		}

		if (ftp_stat != (FTP_STAT *)NULL)
		{
			/* put in tree */

			logrtn(ftp_ptr, ftp_stat, data);
		}
	}

	fclose(fp);

	return (linecnt > 0 ? 0 : -1);
}

/*------------------------------------------------------------------------
 * ftp_find_month() - get month number for a month name
 */
int ftp_find_month (const char *string)
{
	int i;

	for (i=0; i<sizeof(ftp_months)/sizeof(*ftp_months); i++)
	{
		if (strncmp(ftp_months[i], string, 3) == 0)
		{
			return (i+1);
		}
	}
	return (0);
}

/*------------------------------------------------------------------------
 * ftp_calc_secs_from_date() - convert date values to a time_t
 */
time_t ftp_calc_secs_from_date (int year, int month, int day,
	int hour, int mins, int secs)
{
	time_t time_val;
	int i;

	time_val  = (secs);
	time_val += (mins * 60);
	time_val += (hour * 60L * 60L);
	time_val += ((day - 1) * 24L * 60L * 60L);

	month--;
	for (i=0; i<month; i++)
	{
		time_val += ((time_t)(i == 1 && leap_year(year) ?
					ftp_days_in_months[i] + 1 :
					ftp_days_in_months[i]) * 24L * 60L * 60L);
	}

	for (i=1970; i<year; i++)
	{
		time_val += ((leap_year(i) ? 366L : 365L) * 24L * 60L * 60L);
	}

	return (time_val);
}

/*------------------------------------------------------------------------
 * ftp_get_file_stat_mode() - convert a perm string to a mode
 */
int ftp_get_file_stat_mode (const char *perm_str)
{

	int mode;

	mode = 0;

	/* File type */

	switch (perm_str[0])
	{
	case '-':
		mode |= S_IFREG;
		break;

	case 'd':
		mode |= S_IFDIR;
		break;

	case 'c':
		mode |= S_IFCHR;
		break;

	case 'b':
		mode |= S_IFBLK;
		break;

	case 'p':
		mode |= S_IFIFO;
		break;

	case 'n':
		mode |= S_IFNAM;
		break;

	case 's':
		mode |= S_IFSOCK;
		break;

	case 'l':
		mode |= S_IFLNK;
		break;

	default:
		break;
	}

	/* USR perms */

	if (perm_str[1] == 'r')
	{
		mode |= S_IRUSR;
	}

	if (perm_str[2] == 'w')
	{
		mode |= S_IWUSR;
	}

	if (perm_str[3] == 'x')
	{
		mode |= S_IXUSR;
	}
	else if (perm_str[3] == 's')
	{
		mode |= (S_ISUID | S_IXUSR);
	}
	else if (perm_str[3] == 'S')
	{
		mode |= S_ISUID;
	}

	/* GRP perms */

	if (perm_str[4] == 'r')
	{
		mode |= S_IRGRP;
	}

	if (perm_str[5] == 'w')
	{
		mode |= S_IWGRP;
	}

	if (perm_str[6] == 'x')
	{
		mode |= S_IXGRP;
	}
	else if (perm_str[6] == 's')
	{
		mode |= (S_ISGID | S_IXGRP);
	}
	else if (perm_str[6] == 'S')
	{
		mode |= S_ISGID;
	}

	/* OTHER perms */

	if (perm_str[7] == 'r')
	{
		mode |= S_IROTH;
	}

	if (perm_str[8] == 'w')
	{
		mode |= S_IWOTH;
	}

	if (perm_str[9] == 'x')
	{
		mode |= S_IXOTH;
	}
	else if (perm_str[9] == 't')
	{
		mode |= (S_ISVTX | S_IXOTH);
	}
	else if (perm_str[9] == 'T')
	{
		mode |= S_ISVTX;
	}

	return (mode);
}

/*------------------------------------------------------------------------
 * ftp_get_owner_ptr() - find owner name in list & add it if not found
 */
int ftp_get_owner_ptr (FTP_NODE *ftp_ptr, const char *owner)
{
	BLIST *b;
	char *p;
	int i;

	i = 1;
	for (b=ftp_ptr->owners; b; b=bnext(b))
	{
		p = (char *)bid(b);
		if (strcmp(p, owner) == 0)
			return (i);
		i++;
	}

	p = (char *)MALLOC(strlen(owner)+1);
	if (!p)
		return (0);
	strcpy(p, owner);

	b = BNEW(p);
	if (!b)
	{
		FREE(p);
		return (0);
	}

	ftp_ptr->owners = bappend(ftp_ptr->owners, b);

	return (i);
}

/*------------------------------------------------------------------------
 * ftp_get_group_ptr() - find group name in list & add it if not found
 */
int ftp_get_group_ptr (FTP_NODE *ftp_ptr, const char *group)
{
	BLIST *b;
	char *p;
	int i;

	i = 1;
	for (b=ftp_ptr->groups; b; b=bnext(b))
	{
		p = (char *)bid(b);
		if (strcmp(p, group) == 0)
			return (i);
		i++;
	}

	p = (char *)MALLOC(strlen(group)+1);
	if (!p)
		return (0);
	strcpy(p, group);

	b = BNEW(p);
	if (!b)
	{
		FREE(p);
		return (0);
	}

	ftp_ptr->groups = bappend(ftp_ptr->groups, b);

	return (i);
}
