/*------------------------------------------------------------------------
 * help info
 */
#ifndef HELP_H
#define HELP_H

/*------------------------------------------------------------------------
 * Help text location descriptor
 */
typedef struct
{
	int		line;				/* line number */
	int		offset;				/* byte offset into line */
} TEXTLOC;

/*------------------------------------------------------------------------
 * Topic backtrack stack element
 */
typedef struct
{
	int		topic;				/* topic number */
	int		top_line;			/* list window top_item number */
	TEXTLOC link;				/* location of current link; -1 if none */
} STACK_BLK;

/*------------------------------------------------------------------------
 * help info
 *
 * This data is static because the help routine may be called
 * asyncronously via an async call for KEY_HELP.
 */
struct help_info
{
	/*--------------------------------------------------------------------
	 * Help window attributes
	 */
	int				cursor_is_invisible;

	/*--------------------------------------------------------------------
	 * glossary/help window variables
	 */
	int				help_window_created;
	int				gloss_window_created;

	int				help_available;
	int				first_time;

	int				help_list_y_dim;

	int				help_list_item_count;
	int				help_list_top_item;

	/*--------------------------------------------------------------------
	 * Help file variables
	 */
	int				help_topics;			/* number of topics in file */
	int				current_topic;			/* current topic index */

	/*--------------------------------------------------------------------
	 * Help file stream
	 */
	FILE *			help_file_fp;			/* file stream */
	int				help_file_buf_len;		/* length of buffer */

	/*--------------------------------------------------------------------
	 * Topic descriptor buffer
	 */
	HELP_TOPIC *	help_topic_buf;			/* buffer pointer */
	int				help_topic_buf_Index;   /* index of first topic in topic
												buffer; WIN_HELP_NO_TOPIC if
												buffer empty */

	/*--------------------------------------------------------------------
	 * Text buffer
	 */
	unsigned char *	Help_Text_Buf;			/* buffer pointer */
	int				Help_Text_Offset;		/* offset into help file of
												current topic text */
	int				Help_Text_Buf_Offset;   /* offset into help file of
												current text buffer */
	unsigned char *	Help_Cur_Line_Ptr;		/* pointer to current line in
												text buffer */
	int				Help_Cur_Line;			/* index of current line in text
												buffer */

	/*--------------------------------------------------------------------
	 * Topic backtrack stack
	 */
	STACK_BLK *		Topic_Stack;			/* base address of stack */
	int				Topic_Stack_Count;		/* number of stacked elements */
	int				Topic_Stack_Top;		/* index at which next new item
												should be stacked */

	/*--------------------------------------------------------------------
	 * Currently marked topic link
	 */
	TEXTLOC			Cur_Link_Start;			/* start of topic link - points
												to LINK escape sequence */
	TEXTLOC			Cur_Link_End;			/* end of topic link - points
												to terminating EAS0 escape
												sequence */
	int				Cur_Link_Topic;			/* decoded topic index,
												WIN_HELP_NO_TOPIC
												if there is no current
												link */

	/*--------------------------------------------------------------------
	 * windows
	 */
	WINDOW *	win_help_brdr;
	WINDOW *	win_help;
	WINDOW *	win_help_list_brdr;
	WINDOW *	win_help_list;
	WINDOW *	win_help_gloss_brdr;
	WINDOW *	win_help_gloss;
};
typedef struct help_info HELP_INFO;

#endif /* HELP_H */
