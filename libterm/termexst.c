/*------------------------------------------------------------------------
 *	Description:
 *
 *	This routine takes a two-byte status value returned from wait(2)
 *	or waitpid(2), formats it to a string, and returns this string.
 *	The string is suitable for printing in debug messages and the like.
 *
 *	From the wait(2) manual page:
 *
 *		If the child process stopped, the high order 8 bits will
 *		contain the number of the signal that caused the process
 *		to stop and the low order 8 bits will be set to 0x7f.
 *
 *	     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *	     |    s i g n a l  n u m b e r   | 0   1   1   1   1   1   1   1 |
 *	     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *
 *		If the child process terminated due to an exit call, the low
 *		order 8 bits of status will be zero and the high order 8
 *		bits will contain the low order 8 bits of the argument that
 *		the child process passed to exit; see exit(2).
 *
 *	    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *	    |      e x i t  s t a t u s     | 0   0   0   0   0   0   0   0 |
 *	    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *
 *		If the child process terminated due to a signal, the high
 *		order 8 bits of status will be zero and the low order 8 bits
 *		will contain the number of the signal that caused the
 *		termination.  In addition, if the low order seventh bit
 *		(i.e., bit 0x80) is set, a "core image" will have been
 *		produced; see signal(2).
 *
 *	     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *	     | 0   0   0   0   0   0   0   0 | 0 core?     s i g n a l #     |
 *	     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *
 */
#include "termcommon.h"

/*------------------------------------------------------------------------
 * term_exit_status() - decipher a status word into a string
 */
char * term_exit_status (unsigned int status, char *buf)
{
	char signame_buf[128];
	unsigned int hibyte;		/* exit status			*/
	unsigned int lobyte;		/* termination status	*/

	hibyte = (status >> 8) & 0xff;
	lobyte =  status       & 0xff;

	if (lobyte == 0x7f)
	{
		sprintf(buf, "Stopped with signal %s",
			sys_sig_name(hibyte, signame_buf));
	}
	else if (lobyte == 0)
	{
		sprintf(buf, "Exited with status %d", hibyte);
	}
	else
	{
		sprintf(buf, "Terminated by signal %s%s",
			sys_sig_name(lobyte & 0x7f, signame_buf),
			((lobyte & 0x80) ? " (core dumped)" : "") );
	}

	return (buf);
}
