/*------------------------------------------------------------------------
 * ftp_stat struct
 */
#ifndef FTPSTAT_H
#define FTPSTAT_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * ftp stat structure
 */
struct ftp_stat_struct
{
	char		file_name[256];		/* file name */
	char		sym_name[1024];		/* path symlink points to */
	struct stat stbuf;				/* stat buffer */
									/*
		dev_t	st_dev;					... N/A ... set to 0
		ino_t	st_ino;					... set to 0 if MSDOS_SERVER
		mode_t	st_mode;				... file type & perms
		short	st_nlink;				... set to 1 if MSDOS_SERVER
		uid_t	st_uid;					... set to pseudo owner entry index
		gid_t	st_gid;					... set to pseudo group entry index
		dev_t	st_rdev;				... set to 0 if not special
		off_t	st_size;				... size if not special
		time_t	st_atime;				... calc from ls(1) ... may not be GMT
		time_t	st_mtime;				... set to above
		time_t	st_ctime;				... set to above
									*/
};
typedef struct ftp_stat_struct FTP_STAT;

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* FTPSTAT_H */
