/*------------------------------------------------------------------------
 * hexedit-control-block struct
 */
#ifndef HXCB_H
#define HXCB_H

struct hxcb
{
	WINDOW *		x_win;						/* display window */

	char			x_pathname[MAX_PATHLEN];	/* pathname to edit */
	char			x_dispname[MAX_PATHLEN];	/* pathname to display */
	char			x_origname[MAX_PATHLEN];	/* original pathname */

	int				x_comp;						/* compr type of orig file */
	int				x_filesize;					/* file size */

	int				x_top_offs;					/* offset to top-left */
	int				x_cur_offs;					/* current offset */

	int				x_cur_y;					/* line number */
	int				x_cur_x;					/* byte # on line */

	int				x_width;					/* # chars per line */
	int				x_lines;					/* # lines in window */
	int				x_chars;					/* # bytes to display in win */

	int				x_hex;						/* TRUE if in hex side */
	int				x_dirty;					/* TRUE if changes made */
	int				x_left;						/* left/right part of byte */

	unsigned char *	x_disp_buf;					/* file being displayed */
	unsigned char *	x_orig_buf;					/* orig file contents */
};
typedef struct hxcb HXCB;

#endif /* HXCB_H */
