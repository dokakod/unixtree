/*------------------------------------------------------------------------
 * libftp header
 */

#ifndef LIBFTP_H
#define LIBFTP_H

#include "ftpstat.h"
#include "ftpnode.h"

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * file transfer modes
 */
#define BINARY_TRANSFER_MODE	1
#define TENEX_TRANSFER_MODE		2
#define ASCII_TRANSFER_MODE		3

/*------------------------------------------------------------------------
 * supported remote ftp server types
 */
#define UNIX_SERVER				1
#define MSDOS_SERVER			2
#define VMS_SERVER				3

/*------------------------------------------------------------------------
 * function ptototypes
 */
extern FTP_NODE *	ftp_make_node		(const char *hostname,
											const char *username,
											const char *password,
											void (*logmsg)(const char *msg),
											int (*errmsg)(const char *msg));
extern int		ftp_free_node			(FTP_NODE *f);

extern int		ftp_set_debug			(int flag);
extern char *	ftp_hostname			(FTP_NODE *f);
extern char *	ftp_hostaddr			(FTP_NODE *f);
extern int		ftp_server_type			(FTP_NODE *f);

extern int		ftp_open				(FTP_NODE *f, int timeout, int debug);
extern int		ftp_close				(FTP_NODE *f);

extern int		ftp_set_cwd				(FTP_NODE *f,
											const char *dir);
extern int		ftp_get_cwd				(FTP_NODE *f,
											char *dir);

extern int		ftp_make_dir			(FTP_NODE *f,
											const char *dir);
extern int		ftp_delete_dir			(FTP_NODE *f,
											const char *dir);

extern int		ftp_log_dir				(FTP_NODE *f,
											const char *dir,
											int (*logrtn)(FTP_NODE *f,
												FTP_STAT *s, void *data),
											void *data);

extern int		ftp_list_file			(FTP_NODE *f,
											const char *dir,
											const char *file,
											int (*logrtn)(FTP_NODE *f,
												FTP_STAT *s, void *data),
											void *data);

extern int		ftp_copy_file_to		(FTP_NODE *f,
											const char *local,
											const char *remote);
extern int		ftp_copy_file_from		(FTP_NODE *f,
											const char *local,
											const char *remote);
extern int		ftp_move_file_to		(FTP_NODE *f,
											const char *local,
											const char *remote);
extern int		ftp_move_file_from		(FTP_NODE *f,
											const char *local,
											const char *remote);
extern int		ftp_delete_file			(FTP_NODE *f,
											const char *file);
extern int		ftp_chmod_file			(FTP_NODE *f,
											const char *file,
											int mode);
extern int		ftp_rename_file			(FTP_NODE *f,
											const char *from,
											const char *to);

extern int		ftp_copy_file_between	(FTP_NODE *src,
											FTP_NODE *dst,
											const char *srcf,
											const char *dstf);
extern int		ftp_move_file_between	(FTP_NODE *src,
											FTP_NODE *dst,
											const char *srcf,
											const char *dstf);

extern int		ftp_cmd					(FTP_NODE *f, const char *fmt, ...);

extern int		ftp_get_idle			(FTP_NODE *f);
extern int		ftp_set_idle			(FTP_NODE *f, time_t current_time);
extern int		ftp_chk_idle			(FTP_NODE *f, time_t current_time);

extern int		ftp_set_transfer_mode	(FTP_NODE *f);

extern int		ftp_set_umask			(FTP_NODE *f, int cmask);

extern int		ftp_login				(FTP_NODE *f);

extern int		ftp_getreply			(FTP_NODE *f,
											int expecteof);
extern int		ftp_initconn				(FTP_NODE *f);
extern int		ftp_dataconn				(FTP_NODE *f);

extern int		ftp_recvrequest				(FTP_NODE *f,
											const char *cmd,
											const char *local,
											const char *remote);

extern int		ftp_setpeer					(FTP_NODE *f, int timeout);
extern int		ftp_detect_server_type		(FTP_NODE *f);


extern FTP_STAT *	ftp_parse_dos_ls_entry	(FTP_NODE *f,
											char *ls_line,
											const char *dir_name,
											FTP_STAT *stat);

extern FTP_STAT *	ftp_parse_unix_ls_entry	(FTP_NODE *f,
											char *ls_line,
											const char *dir_name,
											FTP_STAT *stat);

extern FTP_STAT *	ftp_parse_vms_ls_entry	(FTP_NODE *f,
											char *ls_line,
											const char *dir_name,
											FTP_STAT *stat);

extern int			ftp_parse_directory_list	(FTP_NODE *f,
												const char *dirname,
												int (*logrtn)(FTP_NODE *f,
													FTP_STAT *s, void *data),
												void *data);

extern int			ftp_get_owner_ptr		(FTP_NODE *f, const char *owner);
extern int			ftp_get_group_ptr		(FTP_NODE *f, const char *group);

extern int			ftp_find_month			(const char *string);

extern time_t		ftp_calc_secs_from_date	(int year, int month, int day,
												int hour, int mins, int secs);

extern int			ftp_get_file_stat_mode	(const char *perm_str);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* LIBFTP_H */
