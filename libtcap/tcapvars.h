/*------------------------------------------------------------------------
 * Here are all non-const variables for this library
 *
 * This header is private to this library only.
 */
#ifndef TCAPVARS_H
#define TCAPVARS_H

/*------------------------------------------------------------------------
 * this struct contains all non-const data
 */
struct tcap_vars
{
	/*--------------------------------------------------------------------
	 * default term db type
	 */
	int				db_type;

	/*--------------------------------------------------------------------
	 * should environment variables be queried
	 */
	int				use_env_vars;

	/*--------------------------------------------------------------------
	 * default path to look for term-files
	 */
	const char *	term_path;

	/*--------------------------------------------------------------------
	 * pointer to current terminal struct
	 */
	TERMINAL *		cur_terminal;
};
typedef struct tcap_vars	TCAP_VARS;

/*------------------------------------------------------------------------
 * Here is the actual struct & macros to access it
 */
extern TCAP_VARS	tcap_vars;

#define tcap_db_type			tcap_vars.db_type
#define tcap_use_env_vars		tcap_vars.use_env_vars
#define tcap_term_path			tcap_vars.term_path
#define tcap_cur_terminal		tcap_vars.cur_terminal

#endif /* TCAPVARS_H */
