/*------------------------------------------------------------------------
 * interface routines for system calls which may be interrupted by
 * a signal and have to be restarted
 */
#ifndef OSINTFC_H
#define OSINTFC_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int	os_access	(const char *path, int amode);
extern int	os_chdir	(const char *path);
extern int	os_chmod	(const char *path, int mode);
extern int	os_chown	(const char *path, int owner, int group);
extern int	os_utime	(const char *path, struct utimbuf *times);

extern int	os_open		(const char *path, int oflag, int mode);
extern int	os_close	(int fd);
extern int	os_read		(int fd, char *buf, int nbytes);
extern int	os_write	(int fd, char *buf, int nbytes);

extern int	os_stat		(const char *path, struct stat *stbuf);
extern int	os_lstat	(const char *path, struct stat *stbuf);
extern int	os_fstat	(int         fd,   struct stat *stbuf);

extern int	os_link		(const char *path1, const char *path2);
extern int	os_unlink	(const char *path);

extern int	os_readlink	(const char *path, char *buf, int bufsize);
extern int	os_symlink	(const char *path1, const char *path2);

extern int	os_mknod	(const char *path, int mode, int rdev);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* OSINTFC_H */
