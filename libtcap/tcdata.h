/*------------------------------------------------------------------------
 * header file for termcap data
 *
 * This is the lookup table for termcap/terminfo names & entries.
 *
 * This header is private to this library only.
 */
#ifndef TCDATA_H
#define TCDATA_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * TC_DATA struct
 */
struct _termcap_data
{
	const char *	trmc_label;			/* name to look for in term file	*/
	int				trmc_flags;			/* entry type & flags				*/
	const char *	trmc_tcname;		/* termcap  name to look up, if any	*/
	const char *	trmc_tiname;		/* terminfo name to look up, if any	*/
	int				trmc_varptr;		/* displacement of var in struct	*/
	SCRN_CMD		trmc_cmd;			/* command code						*/
};
typedef struct _termcap_data TC_DATA;

/*------------------------------------------------------------------------
 * termcap_data flag values
 */
#define TC_NUM		0x0001				/* numeric entry					*/
#define TC_BLN		0x0002				/* boolean entry					*/
#define TC_STR		0x0004				/* string  entry					*/
#define TC_PRM		0x000c				/* parameter (& string) entry		*/

#define TC_TYPE		0x0007				/* type mask						*/

/*------------------------------------------------------------------------
 * data pointer macros
 */
#define TCAP_TC_NUM_SP(td, tc)	( (int   *)((char *)(td) + tc->trmc_varptr) )
#define TCAP_TC_BLN_SP(td, tc)	( (int   *)((char *)(td) + tc->trmc_varptr) )
#define TCAP_TC_STR_SP(td, tc)	( (char **)((char *)(td) + tc->trmc_varptr) )
#define TCAP_TC_PRM_SP(td, tc)	( (char **)((char *)(td) + tc->trmc_varptr) )

#define TCAP_TC_NUM_GP(td, tc)	\
				( (const int   *)((const char *)(td) + tc->trmc_varptr) )
#define TCAP_TC_BLN_GP(td, tc)	\
				( (const int   *)((const char *)(td) + tc->trmc_varptr) )
#define TCAP_TC_STR_GP(td, tc)	\
				( (const char **)((const char *)(td) + tc->trmc_varptr) )
#define TCAP_TC_PRM_GP(td, tc)	\
				( (const char **)((const char *)(td) + tc->trmc_varptr) )

/*------------------------------------------------------------------------
 * termcap_data functions
 */
extern int				tcap_tc_bool_val	(const char *str);
extern const TC_DATA *	tcap_tc_find_entry	(const char *name);
extern const TC_DATA *	tcap_tc_find_cmd	(SCRN_CMD cmd);
extern int				tcap_tc_set_entry	(TCAP_DATA *td, const TC_DATA *tc,
												const char *value);
extern void				tcap_tc_free		(TCAP_DATA *td);
extern const char *		tcap_tc_get_cmd		(const TCAP_DATA *td, SCRN_CMD cmd);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCDATA_H */
