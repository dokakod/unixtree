/*------------------------------------------------------------------------
 * ftp_node struct
 */
#ifndef FTPNODE_H
#define FTPNODE_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * ftp connection structure
 */
struct ftp_node
{
	void *				services;		/* actually a FTP_SVC * */
	int					sendport;		/* if use PORT cmd for each connect */
	int					connected;		/* if connected to server */
	int					data;			/* connection socket */
	int					crflag;			/* if 1, strip CR on ascii gets */
	int					code;			/* return/reply code for ftp command */
	int					curtype;		/* current file transfer type */
	int					server_type;	/* server type ... */
	int					cpend;			/* flag: != 0 -> pending srvr reply */
	FILE *				debug;			/* debug file */
	int					cin;			/* socket input file pointer */
	int					cout;			/* socket output file pointer */
	time_t				mark_idle_time;	/* last marked time for idle send */
	int					idle_time;		/* seconds to send idle */
	int					xfer_mode;		/* transfer mode */

	char				tempname[MAX_PATHLEN];	/* temp file for ls proc */
	char				pasv[64];		/* passive port for proxy data conn */
	char				hostname[64];	/* name of host connected to */
	char				user[16];		/* user name */
	char				password[16];	/* password */

	char				reply_string[256];

	char				ibuf[1024];		/* input buffer */
	int					ibuf_len;		/* # bytes in ibuf */
	int					ibuf_ptr;		/* position in ibuf */

	BLIST *				owners;			/* list of owner names */
	BLIST *				groups;			/* list of group names */
	BLIST *				symfiles;		/* list of files that are symlinks */
	BLIST *				symdirs;		/* list of dirs  that are symlinks */

	void				(*ftp_logmsg)(const char *msg);
	int					(*ftp_errmsg)(const char *msg);
};
typedef struct ftp_node FTP_NODE;

/*------------------------------------------------------------------------
 * symlink entry for symfiles & symdirs
 */
struct sym_entry
{
	char	sym_name[MAX_PATHLEN];			/* name symlink points to */
	void *	sym_file;						/* struct it points to */
};
typedef struct sym_entry SYM_ENTRY;

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* FTPNODE_H */
