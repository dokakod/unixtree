/*------------------------------------------------------------------------
 * diff info
 */
#ifndef DIFF_H
#define DIFF_H

/*------------------------------------------------------------------------
 * diff info
 *
 * This data is static because the diff routines are called via
 * the standard input cmd routines.
 */
struct diff_info
{
	char		diff_path1[MAX_PATHLEN];
	char		diff_path2[MAX_PATHLEN];

	DIFF_LINE *	diff_lines;

	BLIST *		diff_all_lines;
	BLIST *		diff_dif_lines;
	BLIST *		diff_cur_lines;

	int			diff_all_cnt;
	int			diff_dif_cnt;
	int			diff_cur_cnt;

	int			diff_save_in_fv;
	int			diff_save_in_av;

	BLIST *		diff_top_line;
	int			diff_start_col;
	int			diff_top_lineno;
};
typedef struct diff_info DIFF_INFO;

#endif /* DIFF_H */
