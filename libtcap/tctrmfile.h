/*------------------------------------------------------------------------
 * header for term-file processing
 *
 * This header is private to this library only.
 */
#ifndef TCTRMFILE_H
#define TCTRMFILE_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * term-list to catch cyclic references
 */
typedef struct term_list TERM_LIST;
struct term_list
{
	TERM_LIST *	next;
	char *		term;
};

/*------------------------------------------------------------------------
 * functions
 */
extern int		tcap_process_trm_file	(TERMINAL *tp, const char *term,
											const char *path, int flag,
											TERM_LIST *term_list);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCTRMFILE_H */
