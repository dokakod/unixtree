/*------------------------------------------------------------------------
 * menu info
 */
#ifndef MENU_H
#define MENU_H

/*------------------------------------------------------------------------
 * the basic menu structure formed from each valid menu file entry
 */
struct menu_struct
{
	char *	category;			/* category string */
	char *	title;				/* title string */
	char *	exec_path;			/* execute string */
	char *	date_time_string;	/* date/time string */
	int		collapsed;			/* whether category is collapsed or not */
};
typedef struct menu_struct MENU;

/*------------------------------------------------------------------------
 * menu info
 *
 * This data is static because the menu array is only loaded once.
 */
struct menu_info
{
	MENU *	menu_ptr;				/* array of MENU structs		*/
	int		menu_count;				/* count of entries in menu_ptr	*/
	time_t	time_last_read;			/* last time file was read		*/

	int		display_menu_count;		/* count of uncollapsed items	*/
	int		slider;					/* current position in array	*/
	int		menu_item_lines;		/* # lines in list window		*/
	int		menu_window_width;		/* width of list window			*/
	int		menu_marked_item;		/* current marked item in list	*/
	int		menu_top_item;			/* current top item in list		*/
	int		menu_cursor_inv;		/* TRUE if cursor is invisible	*/

	char *	exec_buffer;			/* exec string cmd buffer		*/
};
typedef struct menu_info MENU_INFO;


#endif /* MENU_H */
