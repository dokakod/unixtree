/*------------------------------------------------------------------------
 * gzip interface to gzlib library
 */
#include <stdio.h>
#include <string.h>
#include "zlib.h"

static int gzip_c (const char *inp, const char *out, char *msgbuf)
{
	gzFile	zp;
	FILE *	fp;
	int		rc = 0;

	/*--------------------------------------------------------------------
	 * open input file
	 */
	fp = fopen(inp, "rb");
	if (fp == 0)
	{
		strcpy(msgbuf, "Cannot input open file");
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * open output file
	 */
	zp = gzopen(out, "wb");
	if (zp == 0)
	{
		fclose(fp);
		strcpy(msgbuf, "Cannot output open file");
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * do the compress
	 */
	while (1)
	{
		char	buf[BUFSIZ];
		int		n;

		n = fread(buf, 1, sizeof(buf), fp);
		if (n == 0)
			break;

		rc = gzwrite(zp, buf, n);
		if (rc < 0)
			break;
	}

	/*--------------------------------------------------------------------
	 * check for error
	 */
	if (rc < 0)
	{
		strcpy(msgbuf, gzerror(zp, &rc));
		rc = -1;
	}

	/*--------------------------------------------------------------------
	 * close files
	 */
	fclose(fp);
	gzclose(zp);

	return (rc >= 0 ? 0 : -1);
}

static int gzip_d (const char *inp, const char *out, char *msgbuf)
{
	gzFile	zp;
	FILE *	fp;
	int		rc = 0;

	/*--------------------------------------------------------------------
	 * open input file
	 */
	zp = gzopen(inp, "rb");
	if (zp == 0)
	{
		strcpy(msgbuf, "Cannot input open file");
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * open output file
	 */
	fp = fopen(out, "wb");
	if (fp == 0)
	{
		gzclose(zp);
		strcpy(msgbuf, "Cannot output open file");
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * do the de-compress
	 */
	while (1)
	{
		char	buf[BUFSIZ];
		int		n;

		n = gzread(zp, buf, sizeof(buf));
		if (n <= 0)
		{
			if (n < 0)
				rc = -1;
			break;
		}

		fwrite(buf, n, 1, fp);
	}

	/*--------------------------------------------------------------------
	 * check for error
	 */
	if (rc < 0)
	{
		strcpy(msgbuf, gzerror(zp, &rc));
		rc = -1;
	}

	/*--------------------------------------------------------------------
	 * close files
	 */
	fclose(fp);
	gzclose(zp);

	return (rc);
}

int	gzip (const char *pInpName, const char *pOutName, int bDecomp,
	char *msgbuf)
{
	if (bDecomp)
		return gzip_d(pInpName, pOutName, msgbuf);
	else
		return gzip_c(pInpName, pOutName, msgbuf);
}
