/*------------------------------------------------------------------------
 * file utilities
 */
#include "libprd.h"

int can_we_read (struct stat *stbuf)
{
	return (gbl(pgm_euid)  == 0 ||
			(stbuf->st_mode & S_IROTH) ||
			(stbuf->st_mode & S_IRUSR && stbuf->st_uid == gbl(pgm_euid)) ||
			(stbuf->st_mode & S_IRGRP && stbuf->st_gid == gbl(pgm_egid)) );
}

int can_we_write (struct stat *stbuf)
{
	return (gbl(pgm_euid) == 0 ||
			(stbuf->st_mode & S_IWOTH) ||
			(stbuf->st_mode & S_IWUSR && stbuf->st_uid == gbl(pgm_euid)) ||
			(stbuf->st_mode & S_IWGRP && stbuf->st_gid == gbl(pgm_egid)) );
}

int can_we_exec (struct stat *stbuf)
{
	return (gbl(pgm_euid) == 0 ||
			(stbuf->st_mode & S_IXOTH) ||
			(stbuf->st_mode & S_IXUSR && stbuf->st_uid == gbl(pgm_euid)) ||
			(stbuf->st_mode & S_IXGRP && stbuf->st_gid == gbl(pgm_egid)) );
}
