/*------------------------------------------------------------------------
 * string functions
 */
#ifndef XVTSTRINGS_H
#define XVTSTRINGS_H

/*------------------------------------------------------------------------
 * like strcmp() but case-insensitive
 */
extern int			xvt_strccmp			(const char *s1, const char *s2);

/*------------------------------------------------------------------------
 * like strcmp() but case-insensitive.
 * White-space, "-", and "_" are ignored.
 */
extern int			xvt_strccmpc		(const char *s1, const char *s2);

/*------------------------------------------------------------------------
 * like strncmp() but case-insensitive
 */
extern int			xvt_strnccmp		(const char *s1, const char *s2,
											int n);

/*------------------------------------------------------------------------
 * like strcmp() but case-insensitive & for length of first string
 */
extern int			xvt_strccmpn		(const char *s1, const char *s2);

/*------------------------------------------------------------------------
 * like strcmp() but case-insensitive & for length of first string.
 * White-space, "-", and "_" are ignored.
 */
extern int			xvt_strccmpcn		(const char *s1, const char *s2);

/*------------------------------------------------------------------------
 * like strchr() but skips chars in quotes
 */
extern char *		xvt_strchr			(const char *s, int c);

/*------------------------------------------------------------------------
 * like strpbrk() but skips chars in quotes
 */
extern char *		xvt_strpbrk			(const char *s1, const char *s2);

/*------------------------------------------------------------------------
 * like strstr() but case insensitive
 */
extern char *		xvt_strstr			(const char *s1, const char *s2);

/*------------------------------------------------------------------------
 * like strcpy() but ignores leading/trailing white-space & quotes
 *
 * returns # chars copied
 */
extern int			xvt_strcpy			(char *s1, const char *s2);

/*------------------------------------------------------------------------
 * like strncpy() but ignores leading/trailing white-space & quotes
 *
 * returns # chars copied
 */
extern int			xvt_strncpy			(char *s1, const char *s2, int n);

/*------------------------------------------------------------------------
 * strip leading/trailing white-space from a string
 */
extern char *		xvt_strip			(char *str, int do_leading);

/*------------------------------------------------------------------------
 * convert a string to a boolean value
 */
extern int			xvt_str_to_bool		(const char *str);

#endif /* XVTSTRINGS_H */
