/*------------------------------------------------------------------------
 * header for low-level terminfo-database access routines
 *
 * This header is private to this library only.
 */
#ifndef TCTERMINFO_H
#define TCTERMINFO_H

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int			tcap_tigetent	(char *buf, const char *term,
										const char *path);

extern int			tcap_tigetflag	(const char *buf, const char *name);
extern int			tcap_tigetnum	(const char *buf, const char *name);
extern const char *	tcap_tigetstr	(const char *buf, const char *name);

extern char *		tcap_tigoto		(char *buf, const char *str,
										const int *p);

extern int			tcap_tiputs		(TERMINAL *tp, const char *str,
										int (*rtn)(TERMINAL *tp, int c));

extern void			tcap_tidump		(const char *buf, const char *term,
										FILE *fp);

#ifdef __cplusplus
}
#endif

#endif /* TCTERMINFO_H */
