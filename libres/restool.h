/*------------------------------------------------------------------------
 * header for restool program
 */
#ifndef RESTOOL_H
#define RESTOOL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "curses.h"
#include "libsys.h"

#include "res.h"

/*------------------------------------------------------------------------
 * functions
 */
extern int	res_compile		(const char *name, char *msgbuf);
extern int	res_extract		(const char *name, char *msgbuf);
extern int	res_headers		(const char *name, char *msgbuf);
extern int	res_print		(const char *name, const char *section,
								char *msgbuf);
extern int	res_diff		(const char *name, const char *orig, char *msgbuf);

#endif /* RESTOOL_H */
