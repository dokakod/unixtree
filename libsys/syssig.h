/*------------------------------------------------------------------------
 * Header for signal processing
 *
 * This library performs two functions:
 *
 * 1.	It provides an abstract interface to the OS signal routines.
 *
 * 2.	It provides a mechanism for a signal routine to be passed
 *		a pointer to user-data.
 */
#ifndef SYSSIG_H
#define SYSSIG_H

#include <signal.h>

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * some defines for common processing
 */
#if defined(SIGCLD) && ! defined(SIGCHLD)
#  define SIGCHLD	SIGCLD
#endif

#if defined(SIGCHLD) && ! defined(SIGCLD)
#  define SIGCLD	SIGCHLD
#endif

#define	SIG_ALL		(-1)				/* to reset all signals		*/

/*------------------------------------------------------------------------
 * OS-level signal routine
 */
typedef void		OS_SIG_RTN			(int sig);

/*------------------------------------------------------------------------
 * our signal routine
 */
typedef void		SIG_RTN				(int sig, void *data);

#define SIG_DEFAULT	((SIG_RTN *)0)
#define SIG_IGNORE	((SIG_RTN *)1)

/*------------------------------------------------------------------------
 * signal routines
 */
extern void			sys_sig_set			(int sig, SIG_RTN *rtn, void *data);
extern SIG_RTN *	sys_sig_get_rtn		(int sig);
extern void *		sys_sig_get_data	(int sig);

extern int			sys_sig_send		(int pid, int sig);

/*------------------------------------------------------------------------
 * signal name/desc routines
 */
extern char *		sys_sig_name		(int sig, char *namebuf);
extern char *		sys_sig_desc		(int sig, char *descbuf);
extern void			sys_sig_info		(int sig, char *namebuf, char *descbuf);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* SYSSIG_H */
