/*------------------------------------------------------------------------
 * user/group name/id routines
 */
#include "oscommon.h"

#if V_UNIX
#  include <pwd.h>
#  include <grp.h>
#endif

int os_get_usr_id_from_name (const char *name)
{
#if V_UNIX
	struct passwd *pwd;

	pwd = getpwnam(name);
	if (pwd)
	{
		return (pwd->pw_uid);
	}
#endif

	return (-1);
}

int os_get_grp_id_from_name (const char *name)
{
#if V_UNIX
	struct group *grp;

	grp = getgrnam(name);
	if (grp)
	{
		return (grp->gr_gid);
	}
#endif

	return (-1);
}

char *os_get_usr_name_from_id (int id, char *buf)
{
	*buf = 0;

	if (id < 0)
		return ((char *)NULL);

#if V_UNIX
	{
		struct passwd *pwd;

		pwd = getpwuid(id);
		if (pwd)
		{
			strcpy(buf, pwd->pw_name);
			return (buf);
		}
	}
#endif

	return ((char *)NULL);
}

char *os_get_grp_name_from_id (int id, char *buf)
{
	*buf = 0;

	if (id < 0)
		return ((char *)NULL);

#if V_UNIX
	{
		struct group *grp;

		grp = getgrgid(id);
		if (grp)
		{
			strcpy(buf, grp->gr_name);
			return (buf);
		}
	}
#endif

	return ((char *)NULL);
}

char *os_get_usr_home (const char *name, char *buf)
{
	*buf = 0;

#if V_UNIX
	{
		struct passwd *pwd;

		pwd = getpwnam(name);
		if (pwd)
		{
			strcpy(buf, pwd->pw_dir);
			return (buf);
		}
	}
#endif

	return ((char *)NULL);
}

char *os_get_usr_name (char *buf)
{
	*buf = 0;

#if V_UNIX
	{
		return os_get_usr_name_from_id(os_get_uid(), buf);
	}
#else
	{
		DWORD dwSize = 64;

		GetUserName(buf, &dwSize);
		return (buf);
	}
#endif
}

int os_get_uid (void)
{
#if V_UNIX
	return getuid();
#else
	return (-1);
#endif
}

int os_get_gid (void)
{
#if V_UNIX
	return getgid();
#else
	return (-1);
#endif
}

int os_get_euid (void)
{
#if V_UNIX
	return geteuid();
#else
	return (-1);
#endif
}

int os_get_egid (void)
{
#if V_UNIX
	return getegid();
#else
	return (-1);
#endif
}
