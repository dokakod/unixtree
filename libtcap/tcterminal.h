/*------------------------------------------------------------------------
 * Header file for screen
 *
 *	A TERMINAL struct contains pointers to four sub-structs:
 *
 *	crit	THREAD_MUTEX	This struct contains the mutex for
 *							controlling threaded-access to this struct.
 *
 *	scrn	SCRN_DATA		This struct contains all info concerning
 *							the screen's state during execution.
 *							The contents of this struct changes during
 *							use of the display.
 *
 *	tcap	TCAP_DATA		This struct contains all "termcap" data such
 *							as escape sequences, screen-size, etc.
 *							This struct is loaded at initialization time
 *							and remains constant during use.
 *
 *	term	TERM_DATA		This struct contains all terminal-specific
 *							data such as the file descriptors, O/S
 *							specific terminal-state structs, etc.
 *							This struct is only accessed by the libterm
 *							routines, and is considered opaque here.
 *
 *	This entire struct is only used by the libtcap routines, and is
 *	considered opaque by anyone outside of this library, as follows:
 *
 *		C files in this library all include "tcapcommon.h", which
 *		provides the definition of this struct and includes all other
 *		headers in this library.
 *
 *		C files outside of this library include "libtcap.h", which
 *		only includes those headers needed for accessing this library
 *		and only includes a typedef for a TERMINAL, but does NOT actually
 *		define the struct.  This is because users of this library only
 *		need to have an opaque pointer to this struct, and it avoids
 *		them having to load umpteen system headers which will only clog
 *		up their name-space.
 *
 * This header is private to this library only.
 */
#ifndef TCTERMINAL_H
#define TCTERMINAL_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * TERMINAL struct
 */
struct terminal
{
	THREAD_MUTEX *	crit;			/* critical section	*/
	SCRN_DATA *		scrn;			/* screen  data		*/
	TCAP_DATA *		tcap;			/* termcap data		*/
	TERM_DATA *		term;			/* term    data		*/
};
typedef struct terminal TERMINAL;

#define TERMINAL_T	TRUE			/* TERMINAL struct defined */

/*------------------------------------------------------------------------
 * macros for thread access to a TERMINAL
 */
#define TERMINAL_LOCK(t)	(t == 0 ? -1 : (sysmutex_enter((t)->crit), 0))
#define TERMINAL_UNLOCK(t)	(t == 0 ? -1 : (sysmutex_leave((t)->crit), 0))

/*------------------------------------------------------------------------
 * functions
 */
extern TERMINAL *	tcap_init_terminal	(void);
extern void			tcap_free_terminal	(TERMINAL *tp);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCTERMINAL_H */
