/*------------------------------------------------------------------------
 * common header
 *
 * This header is private to this library only.
 */
#ifndef TCAPCOMMON_H
#define TCAPCOMMON_H

/*------------------------------------------------------------------------
 * system headers
 */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

/*------------------------------------------------------------------------
 * libterm header
 */
#include "libterm.h"

/*------------------------------------------------------------------------
 * internal libtcap headers
 */
#include "tcapdefs.h"
#include "tcerrmsg.h"
#include "tcscrncmds.h"
#include "tcscrndata.h"
#include "tcapdata.h"
#include "tcterminal.h"
#include "tcasync.h"
#include "tcfndpth.h"
#include "tcfkeys.h"
#include "tcdata.h"
#include "tcapkeys.h"
#include "tctermcap.h"
#include "tcterminfo.h"
#include "tctermdef.h"
#include "tctrmstr.h"
#include "tctrmtbls.h"
#include "tctrmfile.h"
#include "tcapvars.h"

/*------------------------------------------------------------------------
 * external libtcap headers
 */
#include "tcattrs.h"
#include "tcapdb.h"
#include "tcapexec.h"
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

#endif /* TCAPCOMMON_H */
