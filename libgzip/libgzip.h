/*------------------------------------------------------------------------
 * header for gzip
 */

#ifndef LIBGZIP_H
#define LIBGZIP_H

#ifdef __cplusplus
extern "C" {
#endif

extern int	gzip	(const char *pInpName,
						const char *pOutname,
						int bDecomp,
						char *msgbuf);

#ifdef __cplusplus
}
#endif

#endif /* LIBGZIP_H */
