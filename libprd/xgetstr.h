/*------------------------------------------------------------------------
 * defines for xgetstr.c
 */
#ifndef XGETSTR_H
#define XGETSTR_H

/*------------------------------------------------------------------------
 * entry type
 */
#define XG_STRING		0	/* char string */
#define XG_FILENAME		1	/* filename pattern */
#define XG_FILESPEC		2	/* filespec pattern */
#define XG_PATHNAME		3	/* pathname */
#define XG_FILEPATH		4	/* full pathname of a file */
#define XG_NUMBER		5	/* number (dest-dir key aborts) */
#define XG_STRING_AB	6	/* char string (dest-dir key abort) */
#define XG_FILE_AB		7	/* pathname (dest-dir key aborts) */
#define XG_PASSWD		8	/* password (echo '*') */
#define XG_STRING_DS	9	/* string (dest-key ok) */
#define XG_FILENAME_NB	10	/* filename (no blank) */

/*------------------------------------------------------------------------
 * entry struct
 */
struct xg_info
{
	WINDOW *	xg_win;			/* window to display in */
	int			xg_y;			/* starting y position */
	int			xg_x;			/* starting x position */

	int			xg_esc_col;		/* column no for escape msg */
	int			xg_ins_col;		/* column no for insert msg */
	int			xg_kep_col;		/* column no for keep   msg */

	int			xg_slen;		/* max string length */
	int			xg_dlen;		/* max display length */

	int			xg_spos;		/* position in string */
	int			xg_dpos;		/* position in buffer */

	int			xg_count;		/* count of chars in input string */
	int			xg_offset;		/* offset to first displayed char in string */
};
typedef struct xg_info XG_INFO;

#endif /* XGETSTR_H */
