/*------------------------------------------------------------------------
 * stat info routines
 */
#include "oscommon.h"

#if V_UNIX
#  if defined (HAVE_SYSMACROS) || defined(linux)
#    include <sys/sysmacros.h>
#  elif defined(HAVE_MKDEV) || defined(SUN)
#    include <sys/mkdev.h>
#  else
#    ifndef major
#      define major(d)	((d) >> 8)
#    endif
#    ifndef minor
#      define minor(d)	((d) & 0xff)
#    endif
#  endif
#else
#  define major(d)		((d) >> 8)
#  define minor(d)		((d) & 0xff)
#endif

/*------------------------------------------------------------------------
 * os_get_major() - get major number from a stat struct
 */
int os_get_major (struct stat *s)
{
	return major(s->st_rdev);
}

/*------------------------------------------------------------------------
 * os_get_minor() - get major number from a stat struct
 */
int os_get_minor (struct stat *s)
{
	return minor(s->st_rdev);
}
