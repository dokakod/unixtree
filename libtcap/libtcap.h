/*------------------------------------------------------------------------
 * libtcap library header
 *
 *	This header is included by "tcapcommon.h" for files in
 *	this library, and is included  directly by files outside
 *	of this library.
 *
 * This library defines a TERMINAL struct, which encapsulates all
 * knowledge about a terminal. This struct is known only by routines
 * in this library, and appears as an opaque struct outside of this
 * library.
 *
 *	The TERMINAL struct basically has 3 parts:
 *
 *	1.	screen data
 *
 *		This part keeps any information on the current state of the screen,
 *		such as where the cursor is, mouse position & button state,
 *		what char set is currently displayed, input & output buffers, etc.
 *		The contents of this part change during use of the display.
 *
 *		This part is totally independent of any O/S or terminal type.
 *
 *	2.	terminal capabilities data
 *
 *		This part keeps all information about the terminal type and
 *		any "escape" sequences needed for a particular capability.
 *		Data in this struct comes from a combination of terminfo database
 *		entries, termcap database entries, and term-files.
 *		The contents of this part is set at initialization time and
 *		remain constant during use of the display.
 *
 *		This part is independent of any O/S type, although knowledge
 *		of the O/S type may have been used in obtaining the info in this
 *		part.
 *
 *	3.	terminal device data
 *
 *		This is the TERM_DATA struct defined and used by the libterm
 *		routines. It encapsulates all actual device information & state.
 *
 *		This struct is very O/S dependent and appears as an opaque struct
 *		at this level.  It is only used by the libterm routines.
 *
 *	Note that a program may create many TERMINAL structs, but only one
 *	is considered "active" at any given time.  The "active" TERMINAL
 *	is the one which will be processed if any signals are received
 *	(such as SIGTERM, SIGINT, SIGWINCH, etc.).
 *
 *	This library does not track TERMINAL structs created, but it does
 *	cache a pointer to the currently active TERMINAL, so the asynchronous
 *	signal routines can access it.
 */
#ifndef LIBTCAP_H
#define LIBTCAP_H

/*------------------------------------------------------------------------
 * system headers needed
 */
#include <stdio.h>
#include <sys/types.h>

/*------------------------------------------------------------------------
 * sub-library headers needed
 */
#include "libterm.h"

/*------------------------------------------------------------------------
 * If TERMINAL not defined (which it won't be if this header is included
 * directly), then define it here.
 */
#ifndef TERMINAL_T
	typedef struct terminal	TERMINAL;		/* opaque type	*/
#	define TERMINAL_T			TRUE		/* type defined	*/
#endif

/*------------------------------------------------------------------------
 * library headers needed
 */
#include "tcapdefs.h"
#include "tcattrs.h"
#include "tcscrncmds.h"

#include "tcapdb.h"
#include "tcapexec.h"
#include "tcapprt.h"
#include "tcbeep.h"
#include "tcblnkscr.h"
#include "tccinit.h"
#include "tccolor.h"
#include "tctrmio.h"
#include "tcdevout.h"
#include "tcerrmsg.h"
#include "tcintio.h"
#include "tckeyinp.h"
#include "tcmouse.h"
#include "tcputtrm.h"
#include "tcscrout.h"
#include "tcsetterm.h"
#include "tcmode.h"
#include "tcapvals.h"
#include "tcaprtns.h"
#include "tcsig.h"
#include "tcsignal.h"
#include "tcwinsiz.h"

#endif /* LIBTCAP_H */
