/*------------------------------------------------------------------------
 * integer I/O routines
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * tcap_put_2byte() - write a 2-byte integer to a stream
 */
int tcap_put_2byte (FILE *fp, unsigned short s)
{
	int n;

	X_STD2(&s);
	n = fwrite((char *)&s, sizeof(s), 1, fp);

	return (n == 1 ? 0 : -1);
}

/*------------------------------------------------------------------------
 * tcap_get_2byte() - read a 2-byte integer from a stream
 */
unsigned short tcap_get_2byte (FILE *fp)
{
	unsigned short s;

	fread((char *)&s, sizeof(s), 1, fp);
	X_LCL2(&s);

	return (s);
}

/*------------------------------------------------------------------------
 * tcap_put_4byte() - write a 4-byte integer to a stream
 */
int tcap_put_4byte (FILE *fp, unsigned int l)
{
	int n;

	X_STD4(&l);
	n = fwrite((char *)&l, sizeof(l), 1, fp);

	return (n == 1 ? 0 : -1);
}

/*------------------------------------------------------------------------
 * tcap_get_4byte() - read a 4-byte integer from a stream
 */
unsigned int tcap_get_4byte (FILE *fp)
{
	unsigned int l;

	fread((char *)&l, sizeof(l), 1, fp);
	X_LCL4(&l);

	return (l);
}

/*------------------------------------------------------------------------
 * tcap_put_4time() - write a time_t as a 4-byte integer to a stream
 */
int tcap_put_4time (FILE *fp, time_t t)
{
	unsigned int	l = (unsigned int)t;
	int				n;

	X_STD4(&l);
	n = fwrite((char *)&l, sizeof(l), 1, fp);

	return (n == 1 ? 0 : -1);
}

/*------------------------------------------------------------------------
 * tcap_get_4time() - read a time_t as a 4-byte integer from a stream
 */
time_t tcap_get_4time (FILE *fp)
{
	unsigned int l;

	fread((char *)&l, sizeof(l), 1, fp);
	X_LCL4(&l);

	return ((time_t)l);
}
