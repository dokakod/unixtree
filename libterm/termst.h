/*------------------------------------------------------------------------
 *	terminal state definition header
 *
 *	This header is private to this library only.
 */
#ifndef TERMST_H
#define TERMST_H

#include "termmse.h"

/*------------------------------------------------------------------------
 * O/S dependent terminal definitions
 */
#if V_UNIX
#  if POSIX_TERMIO
#    include <termios.h>
#    include <sys/ioctl.h>
	typedef struct termios				TERM_ST;
#  else
#    include <termio.h>
	typedef struct termio				TERM_ST;
#  endif
#else
#  include <windows.h>
#  include <wincon.h>
	typedef	CONSOLE_SCREEN_BUFFER_INFO	TERM_ST;
#endif

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * terminal state
 */
struct tstate
{
#if V_UNIX
	/*--------------------------------------------------------------------
	 * UNIX version
	 */
	int			tty_iflags;				/* input  flags					*/
	int			tty_oflags;				/* output flags					*/

	TERM_ST		tty_istruct;			/* input  i/o struct			*/
	TERM_ST		tty_ostruct;			/* output i/o struct			*/
#else
	/*--------------------------------------------------------------------
	 * NT version
	 */
	DWORD		tty_iflags;				/* input  flags					*/
	DWORD		tty_oflags;				/* output flags					*/

	TERM_ST		tty_struct;				/* screen info struct			*/
#endif
};
typedef struct tstate TSTATE;

/*------------------------------------------------------------------------
 * keyboard buffer
 */
struct kbd_buffer
{
	unsigned int		kbd_buf[128];	/* input buffer					*/
	int					kbd_buf_ptr;	/* index of current position	*/
	int					kbd_num_chars;	/* num chars in buffer			*/

	int					kbd_sig;		/* cached pending signal		*/

#if V_WINDOWS
	CRITICAL_SECTION	kbd_crit;		/* enq/deq critical section		*/
	int					kbd_crit_init;	/* TRUE if initialized			*/
#endif
};
typedef struct kbd_buffer KBD_BUFFER;

/*------------------------------------------------------------------------
 * TERM_DATA struct
 */
struct term_data
{
	HANDLE			tty_inp;				/* fd of input				*/
	HANDLE			tty_out;				/* fd of output				*/

	int				tty_i_isatty;			/* TRUE if a tty			*/
	int				tty_o_isatty;			/* TRUE if a tty			*/

	int				old_interval;			/* saved interval			*/
	int				state_saved;			/* TRUE if save_st present	*/

	KBD_BUFFER		kbd_buf;				/* keyboard buffer			*/

	MOUSE_STATUS	mouse_stat;				/* mouse status				*/

	TSTATE			orig_st;				/* original terminal state	*/
	TSTATE			curr_st;				/* current  terminal state	*/
	TSTATE			save_st;				/* saved    terminal state	*/

	TERM_EVT_RTN *	evt_rtn;				/* event routine to call	*/
	void *			evt_data;				/* data to call it with		*/

	TERM_INP_RTN *	inp_rtn;				/* inp routine to call		*/
	void *			inp_data;				/* data to call it with		*/

	TERM_OUT_RTN *	out_rtn;				/* out routine to call		*/
	void *			out_data;				/* data to call it with		*/
};
typedef struct term_data TERM_DATA;

#define TERM_DATA_T		TRUE				/* TERM_DATA struct defined */

#ifdef __cplusplus
}
#endif

#endif /* TERMST_H */
