/*------------------------------------------------------------------------
 * libterm library header
 *
 * This library provides all O/S-dependent services.
 *
 * The data concerning a terminal device is kept in a TERM_DATA
 * struct, which is only known by routines in this library and
 * appears as an opaque struct to any users of this library.
 */
#ifndef LIBTERM_H
#define LIBTERM_H

/*------------------------------------------------------------------------
 * sub-library headers needed
 */
#include <stdarg.h>
#include "termsys.h"

/*------------------------------------------------------------------------
 * If TERM_DATA is not defined (which it won't be if this header is
 * included directly), define it now.
 */
#ifndef TERM_DATA_T
	typedef struct term_data TERM_DATA;		/* opaque term_data struct	*/
#	define TERM_DATA_T		TRUE			/* term_data defined		*/
#endif

/*------------------------------------------------------------------------
 * library headers needed
 */
#include "termattrs.h"
#include "termalarm.h"
#include "termclock.h"
#include "termexec.h"
#include "termexst.h"
#include "termkeys.h"
#include "termmse.h"
#include "termpipe.h"
#include "termprt.h"
#include "termtime.h"
#include "termkbd.h"
#include "termmode.h"
#include "termname.h"
#include "termnap.h"
#include "termscrn.h"
#include "termwinsz.h"

#endif /* LIBTERM_H */
