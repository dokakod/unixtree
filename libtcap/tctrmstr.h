/*------------------------------------------------------------------------
 * misc string routines
 *
 * This header is private to this library only.
 */
#ifndef TCTRMSTR_H
#define TCTRMSTR_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern char *	tcap_print_str		(const char *s, char *buf);

extern int		tcap_trmcmp			(const char *a, const char *b);
extern int		tcap_trmncmp		(const char *a, const char *b);

extern int		tcap_trans_term_str	(const char *str_in,
										unsigned char *str_out, int count);

extern int		tcap_trans_str		(const char *str_in,
										unsigned char *str_out, int count);

extern char *	tcap_trm_dup_str	(const char *str);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCTRMSTR_H */
