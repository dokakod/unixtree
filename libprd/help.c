/*------------------------------------------------------------------------
 * help menu procedures
 */
#include "libprd.h"
#include <setjmp.h>

/*------------------------------------------------------------------------
 * defines and structs
 */
#ifndef min
#define min(a,b)	((a) < (b) ? (a) : (b) )
#endif

#define ROW_CENTERED(length)	((getmaxy(stdscr) - (length)) / 2)
#define COL_CENTERED(width)		((getmaxx(stdscr) - (width) ) / 2)

/* help box */

#define HELP_LIST_WIN_POSX		2
#define HELP_LIST_WIN_POSY		2

/* glossary box */

#define GLOSSARY_WIN_DIMX		WIN_HELP_MAX_LINE + 4

#define MAX_BUTT_LENGTH			50  /* max string len of any button string */

/* Topic descriptor buffer */

#define TOPIC_BUF_COUNT		32
#define TOPIC_BUF_BYTES		(TOPIC_BUF_COUNT * sizeof (HELP_TOPIC))

/* Text buffer */

#define TEXT_BUF_BYTES		(WIN_HELP_MAX_LINE_BYTES*32)

/* Topic backtrack stack */

#define TOPIC_STACK_MAX		512
#define TOPIC_STACK_BYTES   (TOPIC_STACK_MAX * sizeof (STACK_BLK))

/* help viewer topic title position */

#define VIEWER_TOPIC_POSX   1
#define VIEWER_TOPIC_POSY   0

static void handle_user_io(void);

/*------------------------------------------------------------------------
 * pseudo-read routine
 */
static int help_read_buf (void *buf, int len, int loc)
{
	HELP_INFO *	hi = &gbl(help_info);

	if (loc >= hi->help_file_buf_len)
		return (-1);

	if ((loc + len) >= hi->help_file_buf_len)
		len = hi->help_file_buf_len - loc;

	fseek(hi->help_file_fp, loc, SEEK_SET);
	fread(buf, len, 1, hi->help_file_fp);

	return (len);
}

/*.
******************************************************************************
** FUNCTION:	draw_help_window_buttons()
**
** DESCRIPTION: This function displays the help window buttons.
**
** ENTRY:		NONE
**
** EXIT:		NONE
**
** NOTES:		NONE
**
*****************************************************************************/
static void draw_help_window_buttons(void)
{
	HELP_INFO *	hi = &gbl(help_info);
	int button_x_pos;
	int button_y_pos;
	int delta;
	char *m;

	button_x_pos = 1;
	button_y_pos = getmaxy(hi->win_help)-1;
	wattrset (hi->win_help, gbl(scr_help_box_lolite_attr));
	wstandset(hi->win_help, gbl(scr_help_box_hilite_attr));
	delta = 4;

	wmove(hi->win_help, button_y_pos, button_x_pos);
	m = msgs(m_help_index);
	xcaddstr(hi->win_help, CMDS_HELP_INDEX, m);
	button_x_pos += display_len(m) + delta;

	wmove(hi->win_help, button_y_pos, button_x_pos);
	m = msgs(m_help_next);
	xcaddstr(hi->win_help, CMDS_HELP_NEXT, m);
	button_x_pos += display_len(m) + delta;

	wmove(hi->win_help, button_y_pos, button_x_pos);
	m = msgs(m_help_prev);
	xcaddstr(hi->win_help, CMDS_HELP_PREV, m);
	button_x_pos += display_len(m) + delta;

	wmove(hi->win_help, button_y_pos, button_x_pos);
	m = msgs(m_help_linkfwd);
	xcaddstr(hi->win_help, CMDS_HELP_LINK_FWD, m);
	button_x_pos += display_len(m) + delta;

	wmove(hi->win_help, button_y_pos, button_x_pos);
	m = msgs(m_help_linkbck);
	xcaddstr(hi->win_help, CMDS_HELP_LINK_BCK, m);
	button_x_pos += display_len(m) + delta;

	wmove(hi->win_help, button_y_pos, button_x_pos);
	m = msgs(m_help_quit);
	xcaddstr(hi->win_help, CMDS_COMMON_QUIT, m);
	button_x_pos += display_len(m) + delta;

	wrefresh(hi->win_help);
	return;
}

/*.
******************************************************************************
** FUNCTION:	hbox()
**
** DESCRIPTION: This function draws a box around a window.
**
** ENTRY:		WINDOW *win- window
**				attr_t box_attr- box attribute
**				attr_t title_attr- title attribute
**				char *title- box title (can be NULL)
**
** EXIT:		NONE
**
** NOTES:		box() doesn't seem to work
**
*****************************************************************************/
/* ARGSUSED */
static void hbox(WINDOW *win, attr_t box_attr, attr_t title_attr, char *title)
{
	/*
	** draw box
	*/
	wattrset(win, box_attr);
	werase(win);
	wbox(win, B_DVDH);

#if 0
	/*
	** draw title
	*/
	if (title && *title)
	{
		wattrset(win, title_attr);
		wmove(win, 0, ((getmaxx(win) / 2) - (strlen(title) / 2)));
		waddstr(win, title);
	}
#endif

	wrefresh(win);
	return;
}

/*.
******************************************************************************
** FUNCTION:	get_text_ptr()
**
** DESCRIPTION: Read the specified line of title or explanatory text into
**					memory as necessary and return a pointer to it
**
** ENTRY:		line				index of text line:
**									- title line(s) are numbered
**									-WIN_HELP_TITLE_LINES to -1;
**									- explanatory text lines are numbered
**									0 to number of help text lines - 1
**
** EXIT:		returns				pointer to escape string -- valid only
**									until the next call to this function
**
** NOTES:		In order to scan backwards to the first line of help text
**					in the help file correctly, this function depends on a
**					feature of the help file builder utility, HLDBLD, which
**					places a null byte before the very first line of help text.
**
**				This function doesn't range check the line argument.
**					Give me junk and I'll puke.
**
*****************************************************************************/
static unsigned char *get_text_ptr (int line)
{
	HELP_INFO *	hi = &gbl(help_info);
	unsigned char			*new_line_ptr;
	int			terminator;

	/*
	**  If the buffer is empty, read the start of the help text
	*/

	if (hi->Help_Cur_Line < -WIN_HELP_TITLE_LINES)
	{
		/*
		**  Read starting at the help text offset minus one byte - so a
		**  terminator precedes the first line for backward scanning
		*/

		help_read_buf(hi->Help_Text_Buf, TEXT_BUF_BYTES,
			hi->Help_Text_Offset - 1);

		hi->Help_Cur_Line = -WIN_HELP_TITLE_LINES;
		hi->Help_Cur_Line_Ptr = hi->Help_Text_Buf + 1;
		hi->Help_Text_Buf_Offset = hi->Help_Text_Offset - 1;
	}

	/*
	**  If we're prior to the requested line, move forward to find it
	*/

	if (line > hi->Help_Cur_Line)
	{
		/*
		**  Search for the terminator of the requested line - this insures
		**  that the entire line is in the buffer
		*/

		new_line_ptr = hi->Help_Cur_Line_Ptr;
		while (TRUE)
		{
			/*
			**  If we've reached a line terminator, see if we've reached
			**  the end of the requested line
			*/

			terminator = (*new_line_ptr == 0);
			if (terminator  &&  hi->Help_Cur_Line == line)
			{
				break;
			}

			/*
			**  Update the buffer pointer; If we're out of buffer, read
			**  some more
			*/

			++new_line_ptr;
			if (new_line_ptr == hi->Help_Text_Buf + TEXT_BUF_BYTES)
			{
				/*
				**  Move the current line pointer to the low end of the buffer
				*/

				hi->Help_Text_Buf_Offset +=
					hi->Help_Cur_Line_Ptr - hi->Help_Text_Buf;
				new_line_ptr -= (hi->Help_Cur_Line_Ptr - hi->Help_Text_Buf);
				hi->Help_Cur_Line_Ptr = hi->Help_Text_Buf;

				help_read_buf(hi->Help_Text_Buf, TEXT_BUF_BYTES,
					hi->Help_Text_Buf_Offset);
			}

			/*
			**  If the last character scanned was a terminator, update
			**  the current line pointer and index
			*/

			if (terminator)
			{
				hi->Help_Cur_Line_Ptr = new_line_ptr;
				++hi->Help_Cur_Line;
			}
		}
	}

	/*
	**  If we're past the requested line, move backward
	*/

	else if (line < hi->Help_Cur_Line)
	{
		/*
		**  Scan for the terminator of the line which precedes the requested
		**  line
		*/

		new_line_ptr = hi->Help_Cur_Line_Ptr - 1;
		while (TRUE)
		{
			/*
			**  If we've run out of buffer, read some more
			**  (compare for above and below range, because the text buffer
			**  pointer may have a zero offset)
			*/

			if (new_line_ptr < hi->Help_Text_Buf  ||
				new_line_ptr >= hi->Help_Text_Buf + TEXT_BUF_BYTES)
			{
				/*
				**  Move the byte prior to the current line pointer to the
				**  high end of the buffer
				*/

				hi->Help_Text_Buf_Offset -= TEXT_BUF_BYTES -
					(hi->Help_Cur_Line_Ptr - hi->Help_Text_Buf);
				new_line_ptr += TEXT_BUF_BYTES - (hi->Help_Cur_Line_Ptr -
					hi->Help_Text_Buf);

				/*
				**  If we've adjusted prior to the beginning of the file,
				**  adjust to the beginning of the file.  (This would only
				**  happen if the topic array, which precedes the help text
				**  in the file, were small enough to fit entirely in the
				**  text buffer.)
				*/

				if (hi->Help_Text_Buf_Offset < 0)
				{
					new_line_ptr += hi->Help_Text_Buf_Offset;
					hi->Help_Text_Buf_Offset = 0;
				}

				help_read_buf(hi->Help_Text_Buf, TEXT_BUF_BYTES,
					hi->Help_Text_Buf_Offset);
			}

			/*
			**  If we've reached a line terminator, update the line pointer
			**  and see if the line index matches the requested line
			*/

			if (*new_line_ptr == 0)
			{
				hi->Help_Cur_Line_Ptr = new_line_ptr + 1;
				if (hi->Help_Cur_Line == line)
				{
					break;
				}
				--hi->Help_Cur_Line;
			}
			--new_line_ptr;
		}
	}

	return (hi->Help_Cur_Line_Ptr);
}


/*.
******************************************************************************
** FUNCTION:	cal_esc_string_offset_coords()
**
** DESCRIPTION: Calculate the relative display coordinates of a
**					specified byte offset in an escape string
**
** ENTRY:		unsigned char *str		pointer to escape string
**				int off					offset into escape string
**				HELP_POINT *pos				resultant coordinates
**
** EXIT:		HELP_POINT *pos				resultant coordinates
**
** NOTES:		NONE
**
*****************************************************************************/
static void cal_esc_string_offset_coords (unsigned char *string,
													unsigned int offset,
													HELP_POINT *pos)
{
	unsigned int count = 0;

	pos->x = 0;
	pos->y = 0;
	while (*string && count < offset)
	{
		if (*string == WIN_ESC_CHAR)
		{
			string++;
			count++;
			if (*string == WIN_ESC_ECR)
			{
				/* move to the start of the next line */
				(pos->y)++;
				pos->x = 0;
			}
			if (!(*string))
			{
				continue;
			}
			string++;
			count++;
		}
		else
		{
			(pos->x)++;
			count++;
		}
	}

	return;
}


/*.
******************************************************************************
** FUNCTION:	write_escape_string()
**
** DESCRIPTION: Display escape string in window.
**
** ENTRY:		WINDOW *win- display window
**				int x- starting x coordinate in 'win'
**				int y- starting y coordinate in 'win'
**				unsigned char *string- escape string
**				int lower- display as lower case
**				int attribute_count- number of attributes in 'va_dcl'
**				va_dcl- attribute list
**
** EXIT:		returns number of characters displayed
**
** NOTES:		NONE
**
*****************************************************************************/
static int write_escape_string (WINDOW *win,
	int x, int y, unsigned char *string,
	int lower, int attribute_count,
	...)
{
	HELP_INFO *	hi = &gbl(help_info);
	attr_t attribute = 0;
	attr_t ch;
	int ctr;
	int count = 0;
	int turn_attribute_off = FALSE;
	int attribute_on = FALSE;
	int attr_toggle = FALSE;
	va_list ap;
	int orig_x = x;
	attr_t save_attr;
	attr_t curr_attr;

	save_attr = wattrget(win);
	curr_attr = wattrget(win);
	while (*string)
	{
		if (*string == (unsigned char)WIN_ESC_CHAR)
		{
			string++;
			if ((*string & (unsigned char)0xf0) ==
				(unsigned char)WIN_ESC_EAC_LO)
			{
				if (attribute_on)
				{
					wattrset(win, save_attr);
				}
				/* Set the attribute for the next character only */
				va_start(ap, attribute_count);
				for (ctr = 0;
					ctr <= (int)(*string &
						(unsigned char)WIN_ESC_EA_INDEX_MASK); ctr++)
				{
					attribute = va_arg(ap, attr_t);
				}
				va_end(ap);
				turn_attribute_off = TRUE;
				attribute_on = TRUE;
				wattrset(win, attribute);
				curr_attr = attribute;
			}
			else if ((*string & (unsigned char)0xf0) ==
				(unsigned char)WIN_ESC_EAS_LO)
			{
				if (attribute_on)
				{
					attribute_on = FALSE;
					turn_attribute_off = FALSE;
					wattrset(win, save_attr);
					curr_attr = save_attr;
				}
				else
				{
					/* Set the attribute for all subsequent characters */
					va_start(ap, attribute_count);
					for (ctr = 0;
						ctr <= (int)(*string &
							(unsigned char)WIN_ESC_EA_INDEX_MASK); ctr++)
					{
						attribute = va_arg(ap, attr_t);
					}
					va_end(ap);
					attribute_on = TRUE;
					turn_attribute_off = FALSE;
					wattrset(win, attribute);
					curr_attr = attribute;
				}
			}
			else if (*string == (unsigned char)WIN_ESC_LINK)
			{
				if (!(*string))
				{
					continue;
				}
				string++;
				if (!(*string))
				{
					continue;
				}
				string++;
			}
			else if (*string == (unsigned char)WIN_ESC_ECR)
			{
				/* move to the start of the next line */
				y++;
				x = orig_x;

			}
			if (!(*string))
				continue;
			string++;
		}
		else
		{
			if (lower)
			{
				if (isalpha(*string) && isupper(*string))
					ch = tolower(*string);
				else
					ch = *string;
			}
			else
			{
				ch = *string;
			}
			string++;

			wmove(hi->win_help_list, y, x);
			if (! is_hilite_toggle(ch))
			{
				waddch(hi->win_help_list, ch);
				count++;
				x++;
			}
			else
			{
				if (attr_toggle)
					wattrset(win, curr_attr);
				else
					wattrset(win, gbl(scr_help_box_bold_attr));
				attr_toggle = ! attr_toggle;
			}

			if (turn_attribute_off)
			{
				turn_attribute_off = FALSE;
				attribute_on = FALSE;
				wattrset(win, save_attr);
				curr_attr = save_attr;
			}
		}
	}

	if (attribute_on)
	{
		wattrset(win, save_attr);
	}

	return(count);
}


/*.
******************************************************************************
** FUNCTION:	find_escape_sequence()
**
** DESCRIPTION: Find the first escape sequence in an escape string
**
** ENTRY:		string				escape string
**
** EXIT:		returns				pointer to escape sequence, NULL if none
**									was found
**
** NOTES:		NONE
**
*****************************************************************************/
static unsigned char *find_escape_sequence(unsigned char *string)
{
	while (*string)
	{
		if (*string == WIN_ESC_CHAR)
		{
			return(string);
		}
		string++;
	}
	return(0);
}


/*.
******************************************************************************
** FUNCTION:	draw_list_window_line()
**
** DESCRIPTION: Help viewer dialog list window item drawing function
**				Called by the standard list window definition function
**
**				Displays lines of help text in the help viewer dialog
**
** ENTRY:		lptr				list window block ptr
**				index				item index
**				x, y				item local coordinates
**
** EXIT:		NONE
**
** NOTES:		NONE
**
*****************************************************************************/
static void draw_list_window_line(int index)
{
	HELP_INFO *	hi = &gbl(help_info);
	unsigned char			*lineptr;
	unsigned char			*accumptr;
	unsigned char			*escptr;
	register int	line;
	int			offset;
	int x, y;

	/*
	**  Read the specified line into memory
	*/
	y = (int)(index - hi->help_list_top_item);
	x = 0;
	line = (int) index;
	lineptr = get_text_ptr (line);

	/*
	**  Display the line, searching for the current topic link and hilighting
	**  it as appropriate
	*/

	accumptr = lineptr;
	escptr = lineptr;
	while (TRUE)
	{
		/*
		**  Search for the next escape sequence
		*/

		escptr = find_escape_sequence (escptr);

		/*
		**  If we reached the end of the line, just display what's left
		**  and stop
		*/

		if (escptr == 0)
		{
			x += write_escape_string (hi->win_help_list, x, y, accumptr, FALSE,
				4,
				gbl(scr_help_box_lolite_attr),
				gbl(scr_help_box_hilite_attr),
				gbl(scr_help_box_hlink_attr),
				gbl(scr_help_box_glink_attr));
			break;				/* done displaying line */
		}

		/*
		**  If we've reached a topic link EAS2 or EAS3 escape sequence,
		**  see if it's part of the currently marked topic link
		**
		**  If there is a current link and this line and offset is
		**  between Cur_Link_Start and Cur_Link_End, it is part of
		**  the currently marked link
		**
		**  (This boolean expression is my entry into this month's
		**  F*ckingest Boolean Expression of the Month Contest.)
		*/

		offset = escptr - lineptr;
		if ((escptr [1] == WIN_ESC_EAS_LO + 2  ||
				escptr [1] == WIN_ESC_EAS_LO + 3)  &&
			hi->Cur_Link_Topic != WIN_HELP_NO_TOPIC  &&
			(line > hi->Cur_Link_Start.line  ||
				(line == hi->Cur_Link_Start.line  &&
				offset >= hi->Cur_Link_Start.offset))  &&
			(line < hi->Cur_Link_End.line  ||
				(line == hi->Cur_Link_End.line  &&
				offset < hi->Cur_Link_End.offset)))
		{
			/*
			**  Display the accumulated portion of the line normally
			*/

			*escptr = 0;
			x += write_escape_string (hi->win_help_list, x, y, accumptr, FALSE,
				4,
				gbl(scr_help_box_lolite_attr),
				gbl(scr_help_box_hilite_attr),
				gbl(scr_help_box_hlink_attr),
				gbl(scr_help_box_glink_attr));
			*escptr = WIN_ESC_CHAR;

			/*
			**  Scan for the terminating EAS0 escape sequence or the end
			**  of the line
			*/

			accumptr = escptr;
			escptr = find_escape_sequence (escptr + 2);

			/*
			**  Write out the marked link text
			*/

			if (escptr != 0)
			{
				*escptr = 0;
			}
			x += write_escape_string (hi->win_help_list, x, y, accumptr, FALSE,
				4,
				gbl(scr_help_box_hilite_attr),
				gbl(scr_help_box_hilite_attr),
				gbl(scr_help_box_hilite_attr),
				gbl(scr_help_box_hilite_attr));

			/*
			**  If end of line reached, we're done displaying the line;
			**  Otherwise, continue after the terminating escape sequence
			*/

			if (escptr == 0)
			{
				break;
			}
			*escptr = WIN_ESC_CHAR;
			escptr += 2;
			accumptr = escptr;
		}

		/*
		**  Otherwise this is not part of the currently marked topic link,
		**  so continue scanning
		*/

		else
		{
			escptr += 2;
		}
	}

	/*
	**  Pad the rest of the line with spaces
	*/

	wmove(hi->win_help_list, y, x);
	wattrset(hi->win_help_list, gbl(scr_help_box_lolite_attr));
	wclrtoeol(hi->win_help_list);
	return;
}


/*.
******************************************************************************
** FUNCTION:	draw_list_window ()
**
** DESCRIPTION: This function draws the list window.
**
** ENTRY:		NONE
**
** EXIT:		NONE
**
** NOTES:		NONE
**
*****************************************************************************/
static void draw_list_window (void)
{
	HELP_INFO *	hi = &gbl(help_info);
	int ctr;

	werase(hi->win_help_list);
	for (ctr = hi->help_list_top_item;
		ctr < hi->help_list_top_item + (int)hi->help_list_y_dim
			&& ctr < hi->help_list_item_count;
		ctr++)
	{
		draw_list_window_line(ctr);
	}
	wrefresh(hi->win_help_list);
	return;
}


/*.
******************************************************************************
** FUNCTION:	find_link()
**
** DESCRIPTION: Search the text lines visible in the viewer list window
**					for a topic link
**
** ENTRY:		line, offset		help text line and byte offset at which
**									to start search; must be in the correct
**									range
**				direction			forward = 1, backward = -1.
**				wrap				wrap from the window top or bottom to
**									the opposite end during search
**
** EXIT:		NONE
**
** NOTES:		Only the text currently visible in the list window is
**					searched.
**
**				Since this function looks for EAS2 or EAS3 escape sequences
**					in its search for a topic link, it works as desired when
**					Cur_Link_Start (which points to the link escape sequence)
**					is passed as the starting point for a backward search,
**					because Cur_Link_Start points to the LINK escape sequence
**					at the beginning of the current link.
**
*****************************************************************************/
static void find_link (register int line, register int offset,
									int direction, int wrap)
{
	HELP_INFO *	hi = &gbl(help_info);
	unsigned char			*lineptr;
	int			linecount;
	int			lastline;
	unsigned	short encoded;
	int			linkfound;

	/*
	**  Initialize: Read the first line of text; figure out which lines
	**  are visible in the window
	*/

	lineptr = get_text_ptr (line);

	lastline = (int) hi->help_list_top_item +
		min (hi->help_list_y_dim, (int)hi->help_list_item_count) - 1;

	linecount = hi->help_list_y_dim + 1;

	/*
	**  Scan forward or backward for an EAS2 or EAS3 escape sequence:
	**  the telltale unmistakable hallmark of a topic link
	*/

	if (direction == 1)
	{
		while (TRUE)
		{
			/*
			**  Check for an EAS2 or EAS3 escape sequence
			*/

			linkfound = (lineptr [offset] == WIN_ESC_CHAR  &&
							(lineptr [offset + 1] == WIN_ESC_EAS_LO + 2  ||
							lineptr [offset + 1] == WIN_ESC_EAS_LO + 3));
			if (linkfound)
			{
				break;
			}

			/*
			**  Move to the next character; if we're at the end of the line,
			**  go to the next line
			*/

			if (lineptr [offset++] == 0)
			{
				/*
				**  If we've scanned the whole window, there is no current
				**  topic link, so exit
				*/

				if (--linecount == 0)
				{
					break;
				}

				/*
				**  If we're at the bottom of the window, wrap to the top
				*/

				if (++line > lastline)
				{
					if (!wrap)
					{
						break;
					}
					line = (int) hi->help_list_top_item;
				}
				lineptr = get_text_ptr (line);
				offset = 0;
			}
		} /* while */
	}

	/*
	**  Else, scanning backward
	*/

	else
	{
		while (TRUE)
		{
			/*
			**  Check for an EAS2 or EAS3 escape sequence
			*/

			linkfound = (lineptr [offset] == WIN_ESC_CHAR  &&
							(lineptr [offset + 1] == WIN_ESC_EAS_LO + 2  ||
							lineptr [offset + 1] == WIN_ESC_EAS_LO + 3));
			if (linkfound)
			{
				break;
			}

			/*
			**  Move to the previous character; if we're at the beginning of
			**  the line, move to the previous line
			*/

			if (--offset < 0)
			{
				/*
				**  If we've scanned the whole window, there is no current
				**  topic link, so exit
				*/

				if (--linecount == 0)
				{
					break;
				}

				/*
				**  If we're at the top of the window, wrap to the bottom
				*/

				if (--line < (int) hi->help_list_top_item)
				{
					if (!wrap)
					{
						break;
					}
					line = lastline;
				}
				lineptr = get_text_ptr (line);
				offset = strlen((char *)lineptr) - 1;
			}
		} /* while */
	}

	/*
	**  If no link was found, there is no current topic link
	*/

	if (!linkfound)
	{
		hi->Cur_Link_Topic = WIN_HELP_NO_TOPIC;
		return;
	}

	/*
	**  A topic link's EAS2 or EAS3 escape sequence has been found.
	**  Scan backwards for the LINK escape sequence -- the beginning of
	**  the link -- which may not be visible in the window
	*/

	hi->Cur_Link_Start.line = line;
	hi->Cur_Link_Start.offset = offset;
	while (TRUE)
	{
		/*
		**  If we hit the link sequence, stop scanning
		*/

		if (lineptr [hi->Cur_Link_Start.offset] == WIN_ESC_CHAR  &&
			lineptr [hi->Cur_Link_Start.offset + 1] == WIN_ESC_LINK)
		{
			break;
		}

		/*
		**  Move to the previous character; if we're at the beginning of the
		**  line, move to the previous line
		*/

		if (--hi->Cur_Link_Start.offset < 0)
		{
			--hi->Cur_Link_Start.line;
			lineptr = get_text_ptr (hi->Cur_Link_Start.line);
			hi->Cur_Link_Start.offset = strlen((char *)lineptr) - 1;
		}
	}

	/*
	**  Decode the LINK sequence's topic number
	**  (Do this in two steps to avoid the perils of macro side effects.)
	*/

#if 0
	encoded = *((unsigned  short *)(&lineptr [hi->Cur_Link_Start.offset + 2]));
#else
	memcpy(&encoded, &lineptr[hi->Cur_Link_Start.offset + 2], sizeof(encoded));
	X_LCL2(&encoded);
#endif
	hi->Cur_Link_Topic = WIN_HELP_LINK_DECODE (encoded);
	hi->Cur_Link_Topic &= 0x00ff;

	/*
	**  Now scan forward for the end of the topic link -- the EAS0 sequence --
	**  which also may not be visible in the window
	*/

	lineptr = get_text_ptr (line);
	while (TRUE)
	{
		/*
		**  If we hit an EAS0, we're done
		*/

		if (lineptr [offset] == WIN_ESC_CHAR  &&
			lineptr [offset + 1] == WIN_ESC_EAS_LO + 0)
		{
			break;
		}

		/*
		**  Move to the next character; if we're at the end of the line,
		**  move to the next line
		*/

		if (lineptr [++offset] == 0)
		{
			++line;
			lineptr = get_text_ptr (line);
			offset = 0;
		}
	}
	hi->Cur_Link_End.line = line;
	hi->Cur_Link_End.offset = offset;
	return;
}


/*.
******************************************************************************
** FUNCTION:	mark_current_link()
**
** DESCRIPTION: Mark or unmark the current topic link in the
**					help viewer list window
**
** ENTRY:		mark				TRUE = mark, FALSE = unmark
**
** EXIT:		NONE
**
** NOTES:		If there is no current link (Cur_Link_Topic =
**					WIN_HELP_NO_TOPIC), nothing is done.
**
*****************************************************************************/
static void mark_current_link (int mark)
{
	HELP_INFO *	hi = &gbl(help_info);
	int			saved_topic;
	register int	line;

	/*
	**  If there is no current topic link, fuck off
	*/

	if (hi->Cur_Link_Topic == WIN_HELP_NO_TOPIC)
	{
		return;
	}

	/*
	**  If the mark is being removed, save the current topic link index
	**  and pretend there isn't one
	*/

	saved_topic = hi->Cur_Link_Topic;
	if (!mark)
	{
		hi->Cur_Link_Topic = WIN_HELP_NO_TOPIC;
	}

	/*
	**  Redisplay the visible text lines containing of the current link
	*/

	for (line = hi->Cur_Link_Start.line; line <= hi->Cur_Link_End.line; ++line)
	{
		if (line >= (int) hi->help_list_top_item  &&
			line < (int) hi->help_list_top_item +
					min ((int) hi->help_list_item_count, hi->help_list_y_dim))
		{
			draw_list_window_line ((int)line);
			wrefresh(hi->win_help_list);
		}
	}
	hi->Cur_Link_Topic = saved_topic;
	return;
}


/*.
******************************************************************************
** FUNCTION:	scroll_link()
**
** DESCRIPTION: See if the currently marked topic link must change
**					as a result of the viewer list window scrolling
**
** ENTRY:		old_top_item		list window top_item index in effect
**									prior to the scrolling of the window
**
** EXIT:		NONE
**
** NOTES:		NONE
**
*****************************************************************************/
static void scroll_link (int old_top_item)
{
	HELP_INFO *	hi = &gbl(help_info);
	register int	change;
	register int	bottom;
	register int	line;

	/*
	**  Calculate the relative change in window scroll position;
	**  Calculate the index of the bottom line in the window (note
	**  that we don't have to account for a window which isn't entirely
	**  filled with text since such a window will never scroll)
	*/

	change = (int)hi->help_list_top_item - old_top_item;
	bottom = (int)hi->help_list_top_item + hi->help_list_y_dim - 1;

	/*
	**  If there isn't a currently marked topic link
	*/

	if (hi->Cur_Link_Topic == WIN_HELP_NO_TOPIC)
	{
		/*
		**  If they scrolled towards the top of the help text,
		**  scan backwards through the newly revealed lines for
		**  a topic link
		*/

		if (change < 0)
		{
			line = (int) hi->help_list_top_item +
				min (-change, hi->help_list_y_dim) - 1;
			find_link (line, strlen((char *)get_text_ptr(line)) - 1,
							-1, FALSE);
		}

		/*
		**  If they scrolled towards the bottom of the help text,
		**  scan forward through the newly revealed lines
		**  for a topic link
		*/

		else if (change > 0)
		{
			line = bottom - min (change, hi->help_list_y_dim) + 1;
			find_link (line, 0, 1, FALSE);
		}
		mark_current_link (TRUE);
	}

	/*
	**  Otherwise there is a currently marked topic link
	*/

	else
	{
		/*
		**  If the current link scrolled off the bottom of the window,
		**  scan the entire window backwards from the bottom for another link
		*/

		if (hi->Cur_Link_Start.line > bottom)
		{
			find_link (bottom, strlen((char *)get_text_ptr(bottom)) - 1, -1,
				FALSE);
			mark_current_link (TRUE);
		}

		/*
		**  If the current link scrolled off the top of the window,
		**  scan the entire window forward from the top for another link
		*/

		else if (hi->Cur_Link_End.line < (int) hi->help_list_top_item)
		{
			find_link ((int) hi->help_list_top_item, 0, 1, FALSE);
			mark_current_link (TRUE);
		}
	}
}


/*.
******************************************************************************
** FUNCTION:	get_topic_ptr()
**
** DESCRIPTION: Read the specified help file topic descriptor
**					into memory as necessary and return a pointer to it
**
** ENTRY:		index				topic index: 0..x
**
** EXIT:		returns				pointer to specified topic -- valid only
**									until the next call to this function
**
** NOTES:		NONE
**
*****************************************************************************/
static HELP_TOPIC *get_topic_ptr (int index)
{
	HELP_INFO *	hi = &gbl(help_info);
	int i;

	/*
	**  If this topic isn't already in the buffer, read a new buffer
	*/

	if (hi->help_topic_buf_Index == WIN_HELP_NO_TOPIC  ||
		index < hi->help_topic_buf_Index  ||
		index >= hi->help_topic_buf_Index + TOPIC_BUF_COUNT)
	{
		/*
		**  Center the requested topic entry in the buffer - so we can
		**  move up or down without immediately reading again
		**
		**  Calculate the new topic buffer index
		*/

		hi->help_topic_buf_Index = index - (TOPIC_BUF_COUNT / 2);
		if (hi->help_topic_buf_Index < 0)
		{
			hi->help_topic_buf_Index = 0;
		}

		/*
		**  Seek to the new topic index and read the new buffer
		*/

		help_read_buf(hi->help_topic_buf, TOPIC_BUF_BYTES,
			((int)hi->help_topic_buf_Index * (int)sizeof(HELP_TOPIC)) +
			(int)sizeof(HELP_HDR));

		for (i=0; i<TOPIC_BUF_COUNT; i++)
		{
			X_LCL4(&hi->help_topic_buf[i].text_offset);
			X_LCL4(&hi->help_topic_buf[i].text_lines);
			X_LCL4(&hi->help_topic_buf[i].flags);
		}
	}

	/*
	**  Return a pointer to the topic in the buffer
	*/

	return (&hi->help_topic_buf [index-hi->help_topic_buf_Index]);
}


/*.
******************************************************************************
** FUNCTION:	reset_text_buf()
**
** DESCRIPTION: Reset the text buffer - invalid its contents because
**					a new topic is being read
**
** ENTRY:		NONE
**
** EXIT:		NONE
**
** NOTES:		NONE
**
*****************************************************************************/
static void reset_text_buf (void)
{
	HELP_INFO *	hi = &gbl(help_info);

	hi->Help_Cur_Line = -WIN_HELP_TITLE_LINES - 1;
	return;
}



/*.
******************************************************************************
** FUNCTION:	hide_cursor()
**
** DESCRIPTION: Move the cursor out of the way if not invisible
**
** ENTRY:
**
** EXIT:
**
** NOTES:		NONE
**
*****************************************************************************/
static void hide_cursor (void)
{
	HELP_INFO *	hi = &gbl(help_info);

	if (!hi->cursor_is_invisible)
	{
		wmove(stdscr, 0, 0);
		wrefresh(stdscr);
	}
}




/*.
******************************************************************************
** FUNCTION:	viewer_init()
**
** DESCRIPTION: Initialize the help viewer window for a new topic:
**					Set fields in the list window
**
** ENTRY:		NONE
**
** EXIT:		NONE
**
** NOTES:		NONE
**
*****************************************************************************/
static void viewer_init (void)
{
	HELP_INFO *	hi = &gbl(help_info);
	HELP_TOPIC   *topic;

	/*
	**  Initialize the list window based on the help text for this topic
	*/

	topic = get_topic_ptr (hi->current_topic);
	hi->help_list_item_count = topic->text_lines;
	hi->help_list_top_item = 0;

	/*
	**  Save the offset of this topic's help text and
	**  initialize the text buffer -- it is empty
	*/

	hi->Help_Text_Offset = topic->text_offset;
	reset_text_buf ();

	/*
	**  Find the first topic link in the viewer window, if any
	*/

	find_link (0, 0, 1, FALSE);

	return;

}

/*.
******************************************************************************
** FUNCTION:	viewer_draw_title()
**
** DESCRIPTION: Draw the topic title at the top of the help viewer dialog
**
** ENTRY:		NONE
**
** EXIT:		NONE
**
** NOTES:		viewer_init() must have already been called for
**					the current topic.
**
*****************************************************************************/
static void viewer_draw_title (void)
{
	HELP_INFO *	hi = &gbl(help_info);
	unsigned char			*title;
	int ctr;
	int length;

	/*
	**  Get a pointer to the title string and draw it
	*/

	title = get_text_ptr (-WIN_HELP_TITLE_LINES);
	wattrset(hi->win_help, gbl(scr_help_box_lolite_attr));
	wmove(hi->win_help, VIEWER_TOPIC_POSY, VIEWER_TOPIC_POSX);
	waddstr(hi->win_help, (char *)title);

	/*
	**  Pad the rest of the field with spaces
	*/
	wattrset(hi->win_help, gbl(scr_help_box_lolite_attr));
	for (length = WIN_HELP_MAX_TITLE - strlen((char *)title), ctr = 0;
		ctr < length; ctr++)
	{
		waddch(hi->win_help, ' ');
	}
	wrefresh(hi->win_help);
	return;
}



/*.
******************************************************************************
** FUNCTION:	show_topic()
**
** DESCRIPTION: Display a help or glossary topic
**
** ENTRY:		entry -		index				topic index
**
** EXIT:		NONE
**
** NOTES:		NONE
**
*****************************************************************************/
static void show_topic (int index)
{
	HELP_INFO *	hi = &gbl(help_info);
	HELP_TOPIC   *topic;
	HELP_POINT		linkpos;
	int dim_y;
	int pos_y;
	unsigned line;

	/*
	**  If this is a glossary topic, pop up the glossary dialog
	*/

	topic = get_topic_ptr (index);
	if (topic->flags & WIN_HELP_GLOS_BIT)
	{
		mark_current_link (FALSE);
		/*
		**  Determine the height of the glossary dialog from the
		**  number of lines of text
		*/
		dim_y = topic->text_lines + 2;

		/*
		**  Get the global display coords of the current topic link
		*/

		cal_esc_string_offset_coords (get_text_ptr (hi->Cur_Link_Start.line),
			hi->Cur_Link_Start.offset, &linkpos);
		linkpos.y += hi->Cur_Link_Start.line - (int) hi->help_list_top_item;
		linkpos.x += getbegx(hi->win_help_list);
		linkpos.y += getbegy(hi->win_help_list);

		/*
		**  If it will fit, position the glossary dialog above the current
		**  topic link; Otherwise, position it below
		*/

		if (linkpos.y >= dim_y + 1)
		{
			pos_y = linkpos.y - (dim_y + 1);
		}
		else
		{
			pos_y = linkpos.y + 2;
		}

		/*
		**  Create the glossary dialog
		**	+--------------------------------------------------+
		**	| Title Line                                       |
		**	|                                                  |
		**	| Now is the time for all bad men to go to Canada. |
		**	| This is a test of the emergency broadcast        |
		**	| system.                                          |
		**	+--------------------------------------------------+
		*/

		/*
		**  Initialize the text buffer to read the glossary topic
		*/

		hi->Help_Text_Offset = get_topic_ptr (hi->Cur_Link_Topic)->text_offset;
		reset_text_buf ();

		hi->gloss_window_created = TRUE;
		hi->win_help_gloss_brdr = newwin(dim_y+2, GLOSSARY_WIN_DIMX,
									pos_y-1, COL_CENTERED(GLOSSARY_WIN_DIMX));
		hi->win_help_gloss = derwin(hi->win_help_gloss_brdr,
								getmaxy(hi->win_help_gloss_brdr)-2,
								getmaxx(hi->win_help_gloss_brdr)-2,
								1, 1);
		leaveok(hi->win_help_gloss, TRUE);
		keypad(hi->win_help_gloss, TRUE);
		hbox(hi->win_help_gloss_brdr, gbl(scr_gloss_box_attr),
			gbl(scr_gloss_box_attr), 0);
		wattrset(hi->win_help_gloss, gbl(scr_gloss_box_attr));
		werase(hi->win_help_gloss);

		wattrset(hi->win_help_gloss, gbl(scr_gloss_box_attr));
		wmove(hi->win_help_gloss, 0, 1);
		waddstr(hi->win_help_gloss,
			(char *)get_text_ptr (-WIN_HELP_TITLE_LINES));

		wattrset(hi->win_help_gloss, gbl(scr_gloss_box_attr));
		for (line = 0; line < get_topic_ptr(hi->Cur_Link_Topic)->text_lines;
				++line)
		{
			wmove(hi->win_help_gloss, 2 + line, 1);
			waddstr(hi->win_help_gloss, (char *)get_text_ptr (line));
		}
		wrefresh(hi->win_help_gloss);
		hide_cursor();
		handle_user_io();
		delwin(hi->win_help_gloss);
		hi->Help_Text_Offset = get_topic_ptr (hi->current_topic)->text_offset;
		reset_text_buf ();
		mark_current_link (TRUE);
		/* glossary win can cover stdscr */
		touchwin(stdscr);
		wnoutrefresh(stdscr);
		touchwin(hi->win_help_brdr);
		wrefresh(hi->win_help_brdr);
		hide_cursor();
	}

	/*
	**  Otherwise this is a regular help topic, so view it
	*/

	else
	{
		hi->current_topic = index;
		viewer_init ();
		draw_list_window ();
		viewer_draw_title ();
		hide_cursor();
	}
	return;
}


/*.
******************************************************************************
** FUNCTION:	push_topic()
**
** DESCRIPTION: Push the current topic onto the topic backtrack stack.
**
** ENTRY:		NONE
**
** EXIT:		NONE
**
** NOTES:		NONE
**
*****************************************************************************/
static void push_topic (void)
{
	HELP_INFO *	hi = &gbl(help_info);

	/*
	**  Unless the stack is already full, update the stacked element count
	*/

	if (hi->Topic_Stack_Count < TOPIC_STACK_MAX)
	{
		++hi->Topic_Stack_Count;
	}

	/*
	**  Push and update stack pointer, wrapping if necessary
	*/

	hi->Topic_Stack [hi->Topic_Stack_Top].topic = hi->current_topic;
	hi->Topic_Stack [hi->Topic_Stack_Top].top_line =
		(int) hi->help_list_top_item;
	if (hi->Cur_Link_Topic == WIN_HELP_NO_TOPIC)
	{
		hi->Topic_Stack [hi->Topic_Stack_Top].link.line = -1;
	}
	else
	{
		hi->Topic_Stack [hi->Topic_Stack_Top].link = hi->Cur_Link_Start;
	}

	if (++hi->Topic_Stack_Top == TOPIC_STACK_MAX)
	{
		hi->Topic_Stack_Top = 0;
	}
	return;
}


/*.
******************************************************************************
** FUNCTION:	pop_topic()
**
** DESCRIPTION: Pop a topic from the topic backtrack stack, unless it's empty,
**				and display it.
**
** ENTRY:		NONE
**
** EXIT:		NONE
**
** NOTES:		NONE
**
*****************************************************************************/
static void pop_topic (void)
{
	HELP_INFO *	hi = &gbl(help_info);
	HELP_TOPIC   *topic;

	/*
	**  If the stack is empty, never mind
	*/

	if (hi->Topic_Stack_Count == 0)
	{
		return;
	}

	/*
	**  Snap, crackle, crackle, crackle, and pop
	*/

	if (--hi->Topic_Stack_Top < 0)
	{
		hi->Topic_Stack_Top = TOPIC_STACK_MAX - 1;
	}
	--hi->Topic_Stack_Count;

	hi->current_topic = hi->Topic_Stack [hi->Topic_Stack_Top].topic;

	/*
	**  Initialize the list window
	*/

	topic = get_topic_ptr (hi->current_topic);
	hi->help_list_item_count = topic->text_lines;
	hi->help_list_top_item =
		(int)hi->Topic_Stack[hi->Topic_Stack_Top].top_line;

	/*
	**  Reset the text buffer and draw the title
	*/

	hi->Help_Text_Offset = topic->text_offset;
	reset_text_buf ();

	viewer_draw_title ();

	/*
	**  Search for the marked topic link, if any
	*/

	if (hi->Topic_Stack [hi->Topic_Stack_Top].link.line != -1)
	{
		find_link (hi->Topic_Stack [hi->Topic_Stack_Top].link.line,
				hi->Topic_Stack [hi->Topic_Stack_Top].link.offset, 1, FALSE);
	}
	else
	{
		hi->Cur_Link_Topic = WIN_HELP_NO_TOPIC;
	}

	/*
	**  Display the topic text
	*/

	draw_list_window ();
	hide_cursor();

	return;
}

/*.
******************************************************************************
** FUNCTION:	handle_user_io()
**
** DESCRIPTION: This function accepts user keyboard input and performs
**					functions based on that input.
**
** ENTRY:		NONE
**
** EXIT:		NONE
**
** NOTES:		NONE
**
*****************************************************************************/
static void handle_user_io(void)
{
	HELP_INFO *	hi = &gbl(help_info);
	int finished = FALSE;
	int ch;
	int newtopic;
	int old_top_item;

	while (!finished)
	{
		if (!hi->win_help_gloss)
		{
			while (TRUE)
			{
				ch = xgetch(hi->win_help);
				if (ch != KEY_MOUSE)
					break;
			}
			ch = TO_LOWER(ch);

			if (ch == cmds(CMDS_REFRESH_SCREEN))
			{
				wrefresh(curscr);
			}

			else if (ch == KEY_LEFT || ch == KEY_PGUP)
			{
				if (hi->help_list_top_item)
				{
					old_top_item = (int) hi->help_list_top_item;
					hi->help_list_top_item -= ((int)hi->help_list_y_dim - 1);
					if (hi->help_list_top_item < 0L)
					{
						hi->help_list_top_item = 0L;
					}
					draw_list_window();
					scroll_link (old_top_item);
					hide_cursor();
					}
			}

			else if (ch == KEY_UP)
			{
				if (hi->help_list_top_item)
				{
					old_top_item = (int) hi->help_list_top_item;
					hi->help_list_top_item--;
					draw_list_window();
					scroll_link (old_top_item);
					hide_cursor();
				}
			}

			else if (ch == KEY_HOME)
			{
				if (hi->help_list_top_item)
				{
					old_top_item = (int) hi->help_list_top_item;
					hi->help_list_top_item = 0;
					old_top_item = (int) hi->help_list_top_item;
					draw_list_window();
					scroll_link (old_top_item);
					hide_cursor();
				}
			}

			else if (ch == KEY_DOWN)
			{
				if (hi->help_list_item_count > (int)hi->help_list_y_dim
					&& hi->help_list_top_item <
						hi->help_list_item_count - (int)hi->help_list_y_dim)
				{
					old_top_item = (int) hi->help_list_top_item;
					hi->help_list_top_item++;
					draw_list_window();
					scroll_link (old_top_item);
					hide_cursor();
				}
			}

			else if (ch == KEY_RIGHT || ch == KEY_PGDN)
			{
				if (hi->help_list_item_count > (int)hi->help_list_y_dim
					&& hi->help_list_top_item <
						hi->help_list_item_count - (int)hi->help_list_y_dim)
				{
					old_top_item = (int) hi->help_list_top_item;
					hi->help_list_top_item += ((int)hi->help_list_y_dim - 1);
					if (hi->help_list_top_item > hi->help_list_item_count -
						(int)hi->help_list_y_dim)
					{
						hi->help_list_top_item = hi->help_list_item_count -
							(int)hi->help_list_y_dim;
					}
					draw_list_window();
					scroll_link (old_top_item);
					hide_cursor();
				}
			}

			else if (ch == KEY_END)
			{
				if (hi->help_list_item_count > (int)hi->help_list_y_dim
					&& hi->help_list_top_item <
						hi->help_list_item_count - (int)hi->help_list_y_dim)
				{
					old_top_item = (int) hi->help_list_top_item;
					hi->help_list_top_item = hi->help_list_item_count -
						(int)hi->help_list_y_dim;
					draw_list_window();
					scroll_link (old_top_item);
					hide_cursor();
				}
			}

			else if (ch == cmds(CMDS_HELP_LINK_FWD) ||
					ch == KEY_TAB)
			{
				/*
				**  If there is no current link, ignore the tab key
				*/

				if (hi->Cur_Link_Topic != WIN_HELP_NO_TOPIC)
				{
					/*
					**  Search for the next link visible in the window
					*/

					mark_current_link (FALSE);
					find_link (hi->Cur_Link_End.line, hi->Cur_Link_End.offset,
						1, TRUE);
					mark_current_link (TRUE);
					hide_cursor();
				}
			}

			else if (ch == cmds(CMDS_HELP_LINK_BCK) ||
					ch == KEY_BTAB || ch == KEY_BS)
			{
				/*
				**  If there is a current link, search for the next
				**  link visible in the window
				*/

				if (hi->Cur_Link_Topic != WIN_HELP_NO_TOPIC)
				{
					mark_current_link (FALSE);
					find_link (hi->Cur_Link_Start.line,
									hi->Cur_Link_Start.offset, -1, TRUE);
					mark_current_link (TRUE);
					hide_cursor();
				}
			}

			else if (ch == KEY_SPACE || ch == KEY_RETURN)
			{
				/*
				**  If there is a current link, show its target topic
				*/

				if (hi->Cur_Link_Topic != WIN_HELP_NO_TOPIC)
				{
					if ((get_topic_ptr (hi->Cur_Link_Topic)->flags &
							WIN_HELP_GLOS_BIT) == 0)
					{
						push_topic ();
					}
					show_topic (hi->Cur_Link_Topic);
				}
			}

			else if (ch == cmds(CMDS_COMMON_QUIT) || ch == KEY_ESCAPE)
			{
				/*
				**  "Close": destroy myself -- exit the dialog
				*/

				finished = TRUE;
			}

			else if (ch == cmds(CMDS_HELP_INDEX))
			{
				/*
				**  "Index": select the index topic and redisplay
				*/

				show_topic (WIN_HELP_INDEX_TOPIC);
			}

			else if (ch == cmds(CMDS_HELP_NEXT))
			{
				/*
				**  Push this topic on the backtrack stack
				*/

				push_topic ();

				/*
				**  Go to the next non-glossary topic
				*/

				newtopic = hi->current_topic;
				do
				{
					if (++newtopic == hi->help_topics)
						newtopic = 0;
				} while ((get_topic_ptr (newtopic)->flags &
							WIN_HELP_GLOS_BIT) != 0);
				show_topic (newtopic);
			}

			else if (ch == cmds(CMDS_HELP_PREV))
			{
				/*
				**  Pop a topic from the backtrack stack, if any
				*/

				pop_topic ();
			}

			else
				do_beep();
		}
		else
		{
			while (TRUE)
			{
				ch = xgetch(hi->win_help_gloss);
				if (ch != KEY_MOUSE)
					break;
			}
			ch = TO_LOWER(ch);
			if (ch == cmds(CMDS_REFRESH_SCREEN))
				wrefresh(curscr);
			else
				finished = TRUE;
		}
	}
	return;
}


/*.
******************************************************************************
** FUNCTION:	set_help_attributes()
**
** DESCRIPTION: This function sets the window color attributes if it can.
**
** ENTRY:	   NONE
**
** EXIT:		NONE
**
** NOTES:	   NONE
**
*****************************************************************************/
static void set_help_attributes(void)
{
	HELP_INFO *	hi = &gbl(help_info);

	if (curs_set(0) != ERR)
		hi->cursor_is_invisible = TRUE;
	else
		hi->cursor_is_invisible = FALSE;
}

/*****************************************************************************
**					Functions to be used by other modules
*****************************************************************************/


void help_win_free (void)
{
	HELP_INFO *	hi = &gbl(help_info);

	if (hi->gloss_window_created)
	{
		/*
		** destroy glossary window
		*/

		hi->gloss_window_created = FALSE;

		delwin(hi->win_help_gloss);
		delwin(hi->win_help_gloss_brdr);
	}

	if (hi->help_window_created)
	{
		/*
		** destroy help window
		*/

		hi->help_window_created = FALSE;

		delwin(hi->win_help_list);
		delwin(hi->win_help_list_brdr);

		delwin(hi->win_help);
		delwin(hi->win_help_brdr);
	}
}

/*.
******************************************************************************
** FUNCTION:	help_open()
**
** DESCRIPTION: Open the help file and allocate the help buffer;
**					if error, display an alert
**
** ENTRY:		none
**
** EXIT:		returns TRUE if successful, FALSE if not
**
** NOTES:		NONE
**
*****************************************************************************/
int help_open (void)
{
	HELP_INFO *	hi = &gbl(help_info);

	struct stat		stbuf;
	char *			p;
	char			help_path[MAX_PATHLEN];
	char			file_name[MAX_FILELEN];

	hi->gloss_window_created = FALSE;
	hi->help_window_created = FALSE;

	/*
	**  get the filename to open
	*/

	strcpy(file_name, gbl(cur_res)->hdr.file);
	fn_set_ext(file_name, pgm_const(hlp_ext));

	/*
	**  Open the help file.  If an error occurs, display an alert
	**  and return FALSE
	*/

	p = os_get_path(file_name, gbl(pgm_path), help_path);
	if (p == 0)
		return (FALSE);

	hi->help_file_fp = fopen(p, "rb");
	if (hi->help_file_fp == 0)
		return (FALSE);

	os_fstat(fileno(hi->help_file_fp), &stbuf);
	hi->help_file_buf_len = stbuf.st_size;

	{
		HELP_HDR		header;
		unsigned		rc;

		/*
		**  Read and validate the help file header
		*/

		rc = help_read_buf(&header, sizeof(header), 0);
		if (rc != sizeof(header))
		{
			fclose(hi->help_file_fp);
			return (FALSE);
		}

		X_LCL4(&header.signature);
		X_LCL2(&header.topics);
		if (header.signature != WIN_HELP_SIGNATURE)
		{
			fclose(hi->help_file_fp);
			return (FALSE);
		}
		hi->help_topics = header.topics;

		/*
		**  Allocate and initialize the file buffers and the backtrack stack
		*/

		hi->help_topic_buf = (HELP_TOPIC *)MALLOC ((unsigned) TOPIC_BUF_BYTES);
		if (!hi->help_topic_buf)
		{
			fclose(hi->help_file_fp);
			return(FALSE);
		}

		hi->Help_Text_Buf = (unsigned char *)MALLOC((unsigned) TEXT_BUF_BYTES);
		if (hi->Help_Text_Buf == 0)
		{
			FREE(hi->help_topic_buf);
			fclose(hi->help_file_fp);
			return(FALSE);
		}

		hi->Topic_Stack = (STACK_BLK *)MALLOC ((unsigned) TOPIC_STACK_BYTES);
		if (hi->Topic_Stack == 0)
		{
			FREE(hi->help_topic_buf);
			FREE(hi->Help_Text_Buf);
			fclose(hi->help_file_fp);
			return(FALSE);
		}

		hi->help_topic_buf_Index = WIN_HELP_NO_TOPIC; /* make buffer empty */

		hi->Topic_Stack_Count = 0;			/* reset stack */
		hi->Topic_Stack_Top = 0;

		hi->help_available = TRUE;
		hi->first_time = TRUE;
	}

	return (TRUE);
}


/*.
******************************************************************************
** FUNCTION:	help_close()
**
** DESCRIPTION: Close the help file and free the help buffers
**
** ENTRY:		NONE
**
** EXIT:		NONE
**
** NOTES:		NONE
**
*****************************************************************************/
void help_close (void)
{
	HELP_INFO *	hi = &gbl(help_info);

	if (hi->help_available)
	{
		help_win_free();

		/*
		**  Deallocate buffers
		*/

		FREE (hi->help_topic_buf);
		FREE (hi->Help_Text_Buf);
		FREE (hi->Topic_Stack);

		/*
		**  Close the help file
		*/

		fclose(hi->help_file_fp);

		hi->help_available = FALSE;
	}
}


/*.
******************************************************************************
** FUNCTION:	help_topic()
**
** DESCRIPTION: Display the specified help topic in the help viewer window
**
** ENTRY:		index		help topic index or WIN_HELP_NO_TOPIC
**
** EXIT:		NONE
**
** NOTES:		If WIN_HELP_NO_TOPIC is specified, the help index window
**					is displayed.  If WIN_HELP_NO_HELP is specified, nothing
**					happens.
**				Also, help_open must already have been called.
*****************************************************************************/
void help_topic (unsigned index)
{
	HELP_INFO *	hi = &gbl(help_info);
	HELP_TOPIC   *topic;

	/*
	**	Check if we are already in help
	*/
	if (gbl(scr_in_help))
		return;

	/*
	**  If no help is specified, return immediately;
	*/
	if (index == WIN_HELP_NO_HELP)
		return;

	if (index == WIN_HELP_NO_TOPIC)
		index = WIN_HELP_INDEX_TOPIC;

	/*
	**  open help system
	*/
	if (! help_open())
	{
		errmsg(ER_HNA, "", ERR_ANY);
		return;
	}

	gbl(scr_in_help) = TRUE;

	/*
	**  If no topic is specified, just put up the main index
	*/
	if (index == WIN_HELP_NO_TOPIC)
		index = WIN_HELP_INDEX_TOPIC;

	/*
	**  set up initial curses parameters
	*/

	set_help_attributes();

	/*
	**  Setup the file I/O error longjmp target
	*/

	{
		int help_x_start;
		int help_y_start;
		int help_y_dim;
		int help_x_dim;

		/*
		**  Create the help viewer dialog window
		*/

		topic = get_topic_ptr (index);
		hi->current_topic = index;

/*
		+--------------- Help --------------------------------+
		| Topic Title Text                                    |
		| +-------------------------------------------------+ |
		| | Help text                                       | |
		| |                                                 | |
		| |                                                 | |
		| |                                                 | |
		| +-------------------------------------------------+ |
		| Index  Next  Prev  Quit                             |
		+-----------------------------------------------------+
*/
		viewer_init ();

		hi->help_window_created = TRUE;
		help_y_dim = getmaxy(stdscr) - 4;		/* 2 lines above & 2 below */
		help_x_dim = WIN_HELP_MAX_LINE + 6;	/* 3 cols on left & 3 on right */
		help_y_start = ROW_CENTERED(help_y_dim);
		help_x_start = COL_CENTERED(help_x_dim);
		hi->win_help_brdr = newwin(help_y_dim, help_x_dim,
							help_y_start, help_x_start);
		hi->win_help = derwin(hi->win_help_brdr,
							getmaxy(hi->win_help_brdr)-2,
							getmaxx(hi->win_help_brdr)-2,
							1, 1);
		leaveok(hi->win_help, TRUE);
		keypad(hi->win_help, TRUE);
		wattrset(hi->win_help, gbl(scr_help_box_lolite_attr));
		werase(hi->win_help);

		hi->win_help_list_brdr = derwin(hi->win_help_brdr,
									help_y_dim-4,
									help_x_dim-4,
									HELP_LIST_WIN_POSY,
									HELP_LIST_WIN_POSX);
		hi->win_help_list = derwin(hi->win_help_list_brdr,
									getmaxy(hi->win_help_list_brdr)-2,
									getmaxx(hi->win_help_list_brdr)-2,
									1, 1);
		leaveok(hi->win_help_list, TRUE);
		wattrset(hi->win_help_list, gbl(scr_help_box_lolite_attr));
		werase(hi->win_help_list);
		hi->help_list_y_dim = getmaxy(hi->win_help_list);

		hbox(hi->win_help_brdr, gbl(scr_help_box_lolite_attr),
			gbl(scr_help_box_lolite_attr), msgs(m_help_title));

		draw_help_window_buttons();

		hbox(hi->win_help_list_brdr, gbl(scr_help_box_lolite_attr),
			gbl(scr_help_box_lolite_attr), 0);

		viewer_draw_title ();

		draw_list_window();

		if (hi->first_time)
		{
			find_link(hi->Cur_Link_End.line, hi->Cur_Link_End.offset, 1, TRUE);
			hi->first_time = FALSE;
		}

		mark_current_link (TRUE);
		hide_cursor();

		handle_user_io();
	}

	/*
	**  Turn cursor back on
	*/

	if (hi->cursor_is_invisible)
		curs_set(1);

	help_win_free();

	gbl(scr_in_help) = FALSE;
	help_close();

	return;
}

void help_redisplay (void)
{
	HELP_INFO *	hi = &gbl(help_info);

	if (gbl(scr_in_help))
	{
		touchwin(hi->win_help_brdr);
		wrefresh(hi->win_help_brdr);
	}
}
