/*------------------------------------------------------------------------
 * common includes
 */
#ifndef XVTCOMMON_H
#define XVTCOMMON_H

/*------------------------------------------------------------------------
 * common system headers
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>

/*------------------------------------------------------------------------
 * OS-dependent system headers
 */
#if defined(_WIN32) || defined(_MSC_VER)

#  include <windows.h>
struct termios	{ int foo; };
typedef int		pid_t;

#else

#  include <unistd.h>
#  include <termios.h>
#  include <sys/wait.h>
#  include <sys/time.h>
#  include <sys/socket.h>

#  if (defined(SVR4) || defined(__SVR4)) && ! defined(__svr4__)
#    define __svr4__	1
#  endif

#  if (defined(sun) || defined(SUN)) && ! defined(__sun__)
#    define __sun__		1
#  endif

#  if ! (defined(__sun__) || defined(__svr4__))
	/*--------------------------------------------------------------------
	 * SUN has problems when <sys/ioctl.h> & <termios.h> are both included
	 */
#    include <sys/ioctl.h>
#  endif

#  if (defined(__sun__) || defined(__svr4__)) && ! defined(V_NO_REDIR)
	/*--------------------------------------------------------------------
	 * need this for SRIOCSREDIR to redirect console messages
	 */
#    include <sys/strredir.h>
#  endif

#endif

/*------------------------------------------------------------------------
 * our system headers
 */
#include "sysmem.h"
#include "syssig.h"
#include "systhread.h"

/*------------------------------------------------------------------------
 * libxvt external header
 */
#include "libxvt.h"

/*------------------------------------------------------------------------
 * libxvt internal headers
 */
#include "xvtcolor.h"
#include "xvtpty.h"
#include "xvtpipe.h"
#include "xvtprt.h"
#include "xvtscrn.h"
#include "xvtevent.h"
#include "xvtdisplay.h"
#include "xvtgcs.h"
#include "xvtkeys.h"
#include "xvtterm.h"
#include "xvtlogs.h"
#include "xvtcmds.h"
#include "xvtcurscrn.h"
#include "xvtemul.h"
#include "xvtstrings.h"
#include "xvtpath.h"
#include "xvttermdefs.h"
#include "xvtchild.h"
#include "xvttk.h"
#include "xvttio.h"
#include "xvtrgb.h"

#endif /* XVTCOMMON_H */
