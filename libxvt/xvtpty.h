/*------------------------------------------------------------------------
 * header for pty stuff
 */
#ifndef XVTPTY_H
#define XVTPTY_H

/*------------------------------------------------------------------------
 * tty struct indirection
 */
typedef struct termios	TTY_STRUCT;

/*------------------------------------------------------------------------
 * pty struct
 */
struct pty_data
{
	char			mst_name[XVT_MAX_PATHLEN];	/* pathname of master	*/
	char			slv_name[XVT_MAX_PATHLEN];	/* pathname of slave	*/

	int				mst_fd;						/* fd of master			*/
	int				slv_fd;						/* fd of slave			*/

	pid_t			pgid;						/* process group id		*/

	TTY_STRUCT		tty;						/* tty-data of slave 	*/
};
typedef struct pty_data PTY_DATA;

/*------------------------------------------------------------------------
 * functions
 */
extern PTY_DATA *	xvt_pty_init		(const char *spec);
extern int			xvt_pty_open		(PTY_DATA *pd, int bDetach,
											const char *sttymode);
extern void			xvt_pty_close		(PTY_DATA *pd);
extern int			xvt_pty_getmode		(PTY_DATA *pd, TTY_STRUCT *t);
extern int			xvt_pty_setmode		(PTY_DATA *pd, TTY_STRUCT *t);
extern int			xvt_pty_resize		(PTY_DATA *pd, int w, int h, int pid);

#endif /* XVTPTY_H */
