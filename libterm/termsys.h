/*------------------------------------------------------------------------
 * header for additional O/S specific stuff
 */
#ifndef TERMSYS_H
#define TERMSYS_H

#include "libsys.h"

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * Additional O/S stuff
 */
#if V_UNIX
#  ifndef INVALID_HANDLE_VALUE
	typedef int						HANDLE;
#    define INVALID_HANDLE_VALUE	(-1)
#  endif

#  ifndef NO_POSIX_TERMIO
#    define POSIX_TERMIO		TRUE
#  endif
#endif

/*------------------------------------------------------------------------
 * event routine definition
 */
typedef int		TERM_EVT_RTN	(void *data, int ms);

/*------------------------------------------------------------------------
 * input event routine
 *
 * If specified, this routine is called prior to doing a read.
 *
 * Returns:
 *	<0	don't do the read, return with -1
 *	 0	do the read
 *	>0	don't do the read, return this char
 */
#define TERM_INP_MODE_OPEN		1		/* open  call	*/
#define TERM_INP_MODE_CLOSE		2		/* close call	*/
#define TERM_INP_MODE_READ		3		/* read  call	*/

typedef int		TERM_INP_RTN	(void *data, int mode, int ms);

/*------------------------------------------------------------------------
 * output event routine
 *
 * If specified, this routine is called prior to doing a write.
 *
 * Returns:
 *	<0	don't do the write, return with -1
 *	 0	do the write
 *	>0	don't do the write, return with 0
 */
#define TERM_OUT_MODE_OPEN		1		/* open  mode	*/
#define TERM_OUT_MODE_CLOSE		2		/* close mode	*/
#define TERM_OUT_MODE_WRITE		3		/* write mode	*/

typedef int		TERM_OUT_RTN	(void *data, int mode,
									const unsigned char *buf, int n);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TERMSYS_H */
