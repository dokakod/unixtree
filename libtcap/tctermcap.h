/*------------------------------------------------------------------------
 * header for low-level termcap-database access routines
 *
 * This header is private to this library only.
 */
#ifndef TCTERMCAP_H
#define TCTERMCAP_H

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int			tcap_tcgetent	(char *buf, const char *term,
										const char *path);

extern int			tcap_tcgetflag	(const char *buf, const char *name);
extern int			tcap_tcgetnum	(const char *buf, const char *name);
extern const char *	tcap_tcgetstr	(const char *buf, const char *name);

extern char *		tcap_tcgoto		(char *buf, const char *str, const int *p);

extern int			tcap_tcputs		(TERMINAL *tp, const char *str,
										int (*rtn)(TERMINAL *tp, int c));

extern void			tcap_tcdump		(const char *buf, const char *term,
										FILE *fp);

#ifdef __cplusplus
}
#endif

#endif /* TCTERMCAP_H */
