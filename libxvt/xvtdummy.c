/*------------------------------------------------------------------------
 * dummy routines for Windows (or any non-X11 platform)
 */
#include "xvtcommon.h"

#define ERRMSG	"Running in a separate window not supported"

int xvt_available (void)
{
	return (FALSE);
}

static int xvt_dummy (XVT_DATA *xd)
{
	if (xd != 0)
	{
		xd->error_num = XVT_ERR_NOTSUPP;
		sprintf(xd->error_msg, ERRMSG);
	}

	return (-1);
}

int xvt_task_run (XVT_DATA *xd, char **argv)
{
	return xvt_dummy(xd);
}

int xvt_here_run (XVT_DATA *xd, int bDetach)
{
	return xvt_dummy(xd);
}

int xvt_here_close (XVT_DATA *xd)
{
	return xvt_dummy(xd);
}

int xvt_here_clone (XVT_DATA *xd)
{
	return xvt_dummy(xd);
}

int xvt_here_check (XVT_DATA *xd, int ms)
{
	return xvt_dummy(xd);
}

int xvt_here_input (XVT_DATA *xd, const void *buf, int len)
{
	return xvt_dummy(xd);
}

int xvt_here_output (XVT_DATA *xd, const void *buf, int len)
{
	return xvt_dummy(xd);
}

int xvt_tio_build (TTY_STRUCT *ttyp, const char *str, int clr, char *msgbuf)
{
	return (0);
}

void xvt_dump_colors (FILE *fp, int verbose)
{
}

void xvt_dump_pointers (FILE *fp, int verbose)
{
}
