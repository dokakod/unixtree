/*------------------------------------------------------------------------
 * main header for libprd library
 */
#ifndef LIBPRD_H
#define LIBPRD_H

/*------------------------------------------------------------------------
 * system headers
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

/*------------------------------------------------------------------------
 * library headers
 */
#include "curses.h"
#include "libos.h"
#include "libutils.h"
#include "libftp.h"
#include "libdiff.h"
#include "libgzip.h"
#include "libmagic.h"
#include "libhelp.h"
#include "libres.h"
#include "libxvt.h"

/*------------------------------------------------------------------------
 * program structs & defines
 */
#include "vfcb.h"
#include "hxcb.h"
#include "structs.h"
#include "arch.h"
#include "optenums.h"
#include "config.h"
#include "diff.h"
#include "errmsg.h"
#include "help.h"
#include "menu.h"
#include "print.h"
#include "taginfo.h"
#include "xglist.h"
#include "xgetstr.h"

/*------------------------------------------------------------------------
 * program variables
 */
#include "consts.h"
#include "options.h"
#include "gblvars.h"
#include "resource.h"

/*------------------------------------------------------------------------
 * program functions
 */
#include "externs.h"

/*------------------------------------------------------------------------
 * not all systems define these consts
 */
#ifndef SEEK_SET
#  define SEEK_SET	0
#  define SEEK_CUR	1
#  define SEEK_END	2
#endif

/*------------------------------------------------------------------------
 * ctype.h extensions
 */
#define IS_ACHAR(c)	(((unsigned int)(c) & ~0xff) == 0)

#define TO_LOWER(c)	(IS_ACHAR(c) && isupper(c) ? tolower(c) : (c))

#define TO_HEX(c)	("0123456789abcdef"[c])

/*------------------------------------------------------------------------
 * constants
 */
#define ON			TRUE
#define OFF			FALSE

#define RESET		0x1000					/* arbitrary value */

#define	MIN_WIN_COLS	80					/* min window width  */
#define MIN_WIN_ROWS	24					/* min window height */

#endif /* LIBPRD_H */
