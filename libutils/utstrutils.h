/*------------------------------------------------------------------------
 * header for string utilities
 */
#ifndef UTSTRUTILS_H
#define UTSTRUTILS_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern int		strccmp			(const char *s1, const char *s2);
extern int		strccmpn		(const char *s1, const char *s2);
extern int		strxcmp			(const char *s1, const char *s2);
extern int		strxcmpn		(const char *s1, const char *s2);
extern int		istrcpy			(      char *s1, const char *s2);

extern char *	str_lwr			(char *string);
extern char *	str_upr			(char *string);

extern char *	strip			(char *string);
extern char *	str_rem_ws		(char *string);

extern int		stoi			(const char **in_str);
extern long		stol			(const char **in_str);
extern int		stoo			(const char **in_str);
extern int		stox			(const char **in_str);

extern int		atox			(const char  *in_str);

extern char *	itosf			(char *buf, int num, int sep_char);
extern char *	itosfs			(char *buf, int num, int sep_char);
extern char *	itosl			(char *buf, int num, int len);
extern char *	itos			(char *buf, int num);

extern int		str_expand_tabs	(const char *str, int tab_width, char *buf);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* UTSTRUTILS_H */
