/*------------------------------------------------------------------------
 * header for binary I/O processing
 *
 * This header is public.
 */
#ifndef TCINTIO_H
#define TCINTIO_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * read/write a 2-byte number in "standard" format
 */
extern unsigned short	tcap_get_2byte	(FILE *fp);
extern int				tcap_put_2byte	(FILE *fp, unsigned short s);

/*------------------------------------------------------------------------
 * read/write a 4-byte number in "standard" format
 */
extern unsigned int		tcap_get_4byte	(FILE *fp);
extern int				tcap_put_4byte	(FILE *fp, unsigned int l);

/*------------------------------------------------------------------------
 * read/write a time_t as a 4-byte number in "standard" format
 */
extern time_t			tcap_get_4time	(FILE *fp);
extern int				tcap_put_4time	(FILE *fp, time_t t);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCINTIO_H */
