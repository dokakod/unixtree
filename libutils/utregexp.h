/*------------------------------------------------------------------------
 * Definitions etc. for regexp(3) routines.
 *
 * Caveat:  this is V8 regexp(3) [actually, a reimplementation thereof],
 * not the System V one.
 */
#ifndef REGEXP_H
#define REGEXP_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * regexp struct
 */
#define NSUBEXP  10
typedef struct regexp
{
	const char *	startp[NSUBEXP];
	const char *	endp  [NSUBEXP];
	char			regstart;
	char			reganch;
	const char *	regmust;
	int				regmlen;
	int				igncase;
	char			program[1];
} REGEXP;

/*------------------------------------------------------------------------
 * functions
 */
extern REGEXP *		ut_regcomp	(const char *exp, int nocase, int *rc);
extern const char *	ut_regexec	(REGEXP *prog, const char *str,
								const char **endp);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* REGEXP_H */
