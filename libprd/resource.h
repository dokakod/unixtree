/*------------------------------------------------------------------------
 * resource file defines
 */
#ifndef RESOURCE_H
#define RESOURCE_H

/*------------------------------------------------------------------------
 * macros for resource arrays
 */
#define cmds(n)		CMDS_ENTRY(gbl(cur_res),n)
#define dflt(n)		DFLT_ENTRY(gbl(cur_res),n)
#define errs(n)		ERRS_ENTRY(gbl(cur_res),n)
#define msgs(n)		MSGS_ENTRY(gbl(cur_res),n)
#define pgmi(n)		PGMI_ENTRY(gbl(cur_res),n)

/*------------------------------------------------------------------------
 * resource file info struct
 */
struct res_info
{
	char		disp_name[32];		/* display name			*/
	char		file_name[32];		/* base name of file	*/
	RES_FILE *	res_ptr;			/* pointer to resource	*/
};
typedef struct res_info RES_INFO;

/*------------------------------------------------------------------------
 * default (built-in) resource file
 */
extern RES_FILE		res_file;

#endif /* RESOURCE_H */
