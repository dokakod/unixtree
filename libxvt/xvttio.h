/*------------------------------------------------------------------------
 * header for termio routines
 */
#ifndef XVTTIO_H
#define XVTTIO_H

/*------------------------------------------------------------------------
 * callback routine for dumping
 */
typedef void TIO_DUMP_RTN	(void *data, const char *buf);

/*------------------------------------------------------------------------
 * functions
 */
extern int	xvt_tio_build	(TTY_STRUCT *ttyp,
								const char *str,
								int clr,
								char *msgbuf);

extern void	xvt_tio_dump	(const TTY_STRUCT *ttyp,
								void *data,
								TIO_DUMP_RTN *rtn);

extern void	xvt_tio_buf		(const TTY_STRUCT *ttyp,
								char *buf);

#endif /* XVTTIO_H */
