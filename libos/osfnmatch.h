/*------------------------------------------------------------------------
 * filename matching functions
 */
#ifndef	OSFNMATCH_H
#define	OSFNMATCH_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * We #undef these before defining them because some losing systems
 * (HP-UX A.08.07 for example) define these in <unistd.h>.
 */
#undef	FNM_PATHNAME
#undef	FNM_NOESCAPE
#undef	FNM_PERIOD

/*------------------------------------------------------------------------
 * Bits set in the FLAGS argument.
 */
#define	FNM_PATHNAME	(1 << 0) /* No wildcard can ever match `/'.  */
#define	FNM_NOESCAPE	(1 << 1) /* Backslashes don't quote special chars.  */
#define	FNM_PERIOD		(1 << 2) /* Leading `.' is matched only explicitly.  */

#define	FNM_LEADING_DIR	(1 << 3) /* Ignore `/...' after a match.  */
#define	FNM_CASEFOLD	(1 << 4) /* Compare without regard to case.  */

/*------------------------------------------------------------------------
 * Value returned by `fnmatch' if STRING does not match PATTERN.
 */
#define	FNM_NOMATCH		1

/*------------------------------------------------------------------------
 * Match STRING against the filename pattern PATTERN,
 * returning zero if it matches, FNM_NOMATCH if not.
 */
extern int os_fn_match (const char *pattern, const char *string, int flags);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* OSFNMATCH_H */
