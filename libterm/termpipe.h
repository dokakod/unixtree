/*------------------------------------------------------------------------
 * PIPE routines
 */
#ifndef TERMPIPE_H
#define TERMPIPE_H

#include <stdio.h>

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * PIPE struct
 */
typedef struct pipe PIPE;
struct pipe
{
	FILE *	fp;
	int		pid;
};

/*------------------------------------------------------------------------
 * function prototypes
 */
extern int	term_pipe_open	(PIPE *p, const char *cmd, const char *mode,
								char *msgbuf);
extern int	term_pipe_close	(PIPE *p, char *msgbuf);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TERMPIPE_H */
