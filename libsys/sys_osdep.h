/*------------------------------------------------------------------------
 * OS dependent stuff
 *
 * This header includes all O/S pertinent headers for doing file I/O.
 */
#ifndef SYS_OSDEP_H
#define SYS_OSDEP_H

#include <sys/types.h>
#include <time.h>
#include <fcntl.h>

#if V_WINDOWS
#  include <process.h>
#  include <io.h>
#  include <direct.h>
#  include <sys/locking.h>
#  include <sys/utime.h>
#else
#  include <unistd.h>
#  include <sys/wait.h>
#  include <sys/time.h>
#  include <utime.h>
#endif

#endif /* SYS_OSDEP_H */
