/*------------------------------------------------------------------------
 * PIPE routines
 */
#ifndef XVTPIPE_H
#define XVTPIPE_H

/*------------------------------------------------------------------------
 * PIPE struct
 */
typedef struct pipe PIPE;
struct pipe
{
	FILE *	fp;			/* stream to output to	*/
	int		pid;		/* PID of subtask		*/
};

/*------------------------------------------------------------------------
 * function prototypes
 */
extern int	xvt_pipe_open	(PIPE *p, const char *cmd, const char *mode);
extern int	xvt_pipe_close	(PIPE *p);

#endif /* XVTPIPE_H */
