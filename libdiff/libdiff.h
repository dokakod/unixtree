/*------------------------------------------------------------------------
 * diff structs & definitions
 */
#ifndef LIBDIFF_H
#define LIBDIFF_H

/*------------------------------------------------------------------------
 * line class
 */
enum line_class
{
	LEFT = 1,			/* Lines taken from just the left  file		*/
	RIGHT,				/* Lines taken from just the right file		*/
	COMMON,				/* Lines common to both files				*/
	CHANGED				/* A hunk containing both old and new lines	*/
};
typedef enum line_class LINE_CLASS;

/*------------------------------------------------------------------------
 * output is a list containing n diff-line structs
 */
typedef struct diff_line DIFF_LINE;
struct diff_line
{
	/*--------------------------------------------------------------------
	 * fwd & back pointers
	 */
	DIFF_LINE *	next;					/* ptr to next entry			*/
	DIFF_LINE *	prev;					/* ptr to prev entry			*/

	/*--------------------------------------------------------------------
	 * data items in the struct
	 */
	int			size;					/* size of entry (& lines)		*/

	LINE_CLASS	type;					/* type of entry				*/

	int			num_lf;					/* line number of left  side	*/
	int			num_rt;					/* line number of right side	*/

	int			len_lf;					/* # bytes in left  side		*/
	int			len_rt;					/* # bytes in right side		*/

	/*--------------------------------------------------------------------
	 * implied data items (immediately following)
	 */
#if 0
	char		line_lf[len_lf + 1];	/* left  line (plus NUL)		*/
	char		line_rt[len_rt + 1];	/* right line (plus NUL)		*/
#endif
};

#define LINE_SIZE(d)	((d)->size)
#define LINE_TYPE(d)	((d)->type)

#define LINE_PTR_LF(d)	((char *)(d) + sizeof(*(d)))
#define LINE_NUM_LF(d)	((d)->num_lf)
#define LINE_LEN_LF(d)	((d)->len_lf)

#define LINE_PTR_RT(d)	((char *)(d) + sizeof(*(d)) + ((d)->len_lf + 1))
#define LINE_NUM_RT(d)	((d)->num_rt)
#define LINE_LEN_RT(d)	((d)->len_rt)

/*------------------------------------------------------------------------
 * compare options struct
 */
typedef struct diff_opts DIFF_OPTS;
struct diff_opts
{
	/*----------------------------------------------------------------
	 * Consider all files as text files.
	 */
	int		always_text_flag;

	/*----------------------------------------------------------------
	 * Number of lines to keep in identical prefix and suffix.
	 */
	int		horizon_lines;

	/*----------------------------------------------------------------
	 * Ignore changes in horizontal white space.
	 */
	int		ignore_space_change_flag;

	/*----------------------------------------------------------------
	 * Ignore all horizontal white space.
	 */
	int		ignore_all_space_flag;

	/*----------------------------------------------------------------
	 * Ignore changes that affect only blank lines.
	 */
	int		ignore_blank_lines_flag;

	/*----------------------------------------------------------------
	 * 1 if lines may match even if their contents do not match exactly.
	 * This depends on various options.
	 */
	int		ignore_some_line_changes;

	/*----------------------------------------------------------------
	 * Ignore differences in case of letters.
	 */
	int		ignore_case_flag;

	/*----------------------------------------------------------------
	 * Nonzero means use heuristics for better speed.
	 */
	int		heuristic;

	/*----------------------------------------------------------------
	 * don't do discard_confusing_lines
	 */
	int		no_discards;
};

/*------------------------------------------------------------------------
 * return codes
 */
#define DIFF_ERROR		-1
#define DIFF_SAME_FILE	0
#define DIFF_TXT_SAME	1
#define DIFF_TXT_DIFF	2
#define DIFF_BIN_SAME	3
#define DIFF_BIN_DIFF	4

/*------------------------------------------------------------------------
 * diff engine
 */
extern DIFF_LINE *	diff_engine			(const char *file1,
											const char *file2,
											DIFF_OPTS *opts,
											int *rc,
											char *msgbuf);

extern DIFF_LINE *	diff_free_lines		(DIFF_LINE *dl);

/*------------------------------------------------------------------------
 * diff output routines
 */
typedef void	DIFF_OUTPUT_RTN	(void *data, const char *line);

extern void			diff_print_diff		(DIFF_OUTPUT_RTN *rtn, void *data,
											DIFF_LINE *lines);

extern void			diff_print_side		(DIFF_OUTPUT_RTN *rtn, void *data,
											DIFF_LINE *lines,
											int changed,
											int page_width,
											int show_numbers,
											int tab_width);

#endif /* LIBDIFF_H */
