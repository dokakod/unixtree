/*------------------------------------------------------------------------
 * mono/color attribute tables
 *
 * This header is private to this library only.
 */
#ifndef TCTERMDEF_H
#define TCTERMDEF_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * structure containing names & codes of attributes
 */
struct attr_tbl
{
	const char *	attr_name;
	attr_t			attr_code;
};
typedef struct attr_tbl ATTR_TBL;

/*------------------------------------------------------------------------
 * mono attributes tables
 */
extern const ATTR_TBL	tcap_common_attrs[];
extern const ATTR_TBL	tcap_mono_attrs[];

/*------------------------------------------------------------------------
 * color attributes tables
 */
extern const ATTR_TBL	tcap_foregrnd_attrs[];
extern const ATTR_TBL	tcap_backgrnd_attrs[];

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCTERMDEF_H */
