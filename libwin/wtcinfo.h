/*------------------------------------------------------------------------
 * termcap/terminfo database names
 */
#ifndef WTCINFO_H
#define WTCINFO_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * termcap/terminfo info struct
 */
struct tc_info
{
	const char *	ti_name;
	const char *	tc_name;
	const char *	desc;
};
typedef struct tc_info	TC_INFO;

extern const TC_INFO	tc_bools[];
extern const TC_INFO	tc_nums[];
extern const TC_INFO	tc_strs[];

extern const int		num_tc_bools;
extern const int		num_tc_nums;
extern const int		num_tc_strs;

/*------------------------------------------------------------------------
 * functions
 */
extern int		tc_id_flag	(const char *id);
extern int		tc_id_num	(const char *id);
extern int		tc_id_str	(const char *id);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* WTCINFO_H */
