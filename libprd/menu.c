/*------------------------------------------------------------------------
 * application menu procedures
 */
#include "libprd.h"

/*------------------------------------------------------------------------
 * defines
 */

/* menu line buffer length (line+2) */
#define MAX_MENU_ENTRY_LENGTH		512

/* menu file entry subfield separator */
#define MENU_SEPARATOR_CHAR		 	':'

/* menu file entry subfield character to 'escape' field meta characters */
#define MENU_ESCAPE_CHAR			'\\'

/* menu file entry subfield members separator */
#define MENU_GRP_SEPARATOR_CHAR	 	','

/* menu file entry 24 hour time subfield separator */
#define MENU_TIME_SEPARATOR_CHAR	'-'

/* menu file start of comment line character */
#define MENU_COMMENT_CHAR			'#'

/* menu file entry subfield member exclusion character */
#define MENU_EXCLUDE_CHAR			'!'

/* menu exec substitution leadin char */
#define MENU_EXEC_SUB_CHAR			'$'

/* marker on collapsed categories */
#define COLLAPSED_CATEGORY_MARKER	'+'

/* how many spaces to indent a menu entry title (1 for categories) */
#define TITLE_INDENT                5

/*.
******************************************************************************
** FUNCTION:	position_menu_cursor()
**
** DESCRIPTION: This function positions the cursor if it is visible
**
** ENTRY:	   NONE
**
** EXIT:		NONE
**
** NOTES:
**
*****************************************************************************/
static void position_menu_cursor(void)
{
	MENU_INFO *	mi = &gbl(menu_info);

	if (!mi->menu_cursor_inv)
	{
		wmove(stdscr, getmaxy(stdscr)-1, 0);
		wrefresh(stdscr);
	}
}

/*.
******************************************************************************
** FUNCTION:	get_tagged_file_len
**
** DESCRIPTION: get length of all tagged filenames
**
** ENTRY:		NONE
**
** EXIT:		returns length
**
** NOTES:		NONE
**
*****************************************************************************/
static int get_tagged_file_len (int prepend_dir)
{
	BLIST *b;
	FBLK *f;
	char path[MAX_PATHLEN];
	int l;

	l = 0;
	for (b=gbl(scr_cur)->first_file; b; b=bnext(b))
	{
		f = (FBLK *)bid(b);
		if (is_file_tagged(f))
		{
			if (prepend_dir)
			{
				fblk_to_pathname(f, path);
				l += strlen(path) + 3;
			}
			else
			{
				l += strlen(FULLNAME(f)) + 3;
			}
		}
	}
	return (l);
}

/*.
******************************************************************************
** FUNCTION:	stryncmp
**
** DESCRIPTION: compare two strings for n chars & ignore case
**
** ENTRY:		a, b strings
**				n  length
**
** EXIT:		<0 a < b
**				0  a == b
**				>0 a > b
**
** NOTES:		NONE
**
*****************************************************************************/
static int stryncmp (const char *a, const char *b, int n)
{
	register int i;

	for (; n; n--, a++, b++)
	{
		i = tolower(*a) - tolower(*b);
		if (i)
			return (i);
	}
	return (0);
}

/*.
******************************************************************************
** FUNCTION:	menu_scrollbar
**
** DESCRIPTION: display scroll bar for menu
**
** ENTRY:		NONE
**
** EXIT:		NONE
**
** NOTES:		NONE
**
*****************************************************************************/
static int menu_scrollbar (void)
{
	MENU_INFO *	mi = &gbl(menu_info);

	wattrset(gbl(win_menu_reg), gbl(scr_menu_border_attr));
	scrollbar(gbl(win_menu_reg), gbl(win_menu_sub), SCRLBAR_VL,
		mi->menu_marked_item, mi->display_menu_count);
	mi->slider = mi->menu_marked_item;
	wattrset(gbl(win_menu_reg), gbl(scr_menu_lolite_attr));
	wrefresh(gbl(win_menu_reg));

	return (0);
}


/*.
******************************************************************************
** FUNCTION:	get_login_tty()
**
** DESCRIPTION: This function gets the device name that the user logged in on.
**
** ENTRY:	   NONE
**
** EXIT:		returns NULL on error, else pointer to string
**				  of device name.
**
** NOTES:	   NONE
**
*****************************************************************************/
static char *get_login_tty(void)
{
	return fn_basename((char *)get_ttyname());
}


/*.
******************************************************************************
** FUNCTION:	date_time_matches()
**
** DESCRIPTION: This function determines if the buffer's date and time
**				  parameters match the current date and time.  It is used
**				  twice in the program ... when a menu file line is read
**				  in and we just want to check the date/time subfield for
**				  the correct syntax (validate_only == TRUE) and when a
**				  menu item is selected from the menu and we need to
**				  determine if the current date/time is within the bounds
**				  of the menu entry date/time (validate_only == FALSE).
**
** ENTRY:	   char *entry_buffer- date/time string
**			  int validate_only- if just check for valid entry or not
**
** EXIT:		returns FALSE if no match or error, TRUE otherwise
**				(validate_only == FALSE);  returns FALSE if error or TRUE
**				otherwise (validate_only == TRUE).
**
** NOTES:	   The schedule field in the menu file indicates when the menu
**				  item can be executed from the user's menu.  The schedule
**				  field consists of two subfields: the day of the week and
**				  an optional time of day.  The day and time subfields are
**				  written with no intervening spaces.  The day subfield is
**				  required and is specified using the following keywords:
**					  Any means the menu item can be executed and displayed
**						  on any day.
**					  Wk  means any weekday.  You can also specify
**						  individual days of the week using one or more or
**						  the keywords Su, Mo, Tu, We, Th, Fr, and Sa.
**				  Note that AnyMoWkMo is not invalid, but redundant. SuSaWk
**				  is more easily written as Any. The optional time subfield
**				  is specified by two 24-hour clock times separated by a
**				  dash (-).  For example, 0800-1732 means that a menu item
**				  can be executed between 8:00 in the morning and 5:32 in
**				  the evening.  This range can span 0000, thus a time
**				  subfield of 0700-0500 means that a menu entry may be
**				  executed from 7:00 in the morning until 5:00 in the
**				  morning on the following day.  This also means any time
**				  except from 5:01 in the morning to 6:59 in the morning.
**				  Note that this really is interpreted as 0000-0500 and
**				  0700-2359 on the same day.  This is important in the
**				  following example:
**					  Wk2300-0700
**				  This means 0000-0700 and 2300-2359 on Monday through
**				  Friday.  It does not include 0000-0700 Saturday.  If the
**				  time subfield is omitted, the menu item may be executed
**				  at any time on the day(s) specified.
**
*****************************************************************************/
static int date_time_matches(char *entry_buffer, int validate_only)
{
	char date_time_string[26];
	time_t time_val;
	char *day_ptr;
	char *hour_ptr;
	char *entry_buffer_ptr;
	int have_date = FALSE;
	int have_time = TRUE;
	int hour, menu_hour1, menu_hour2;
	int ctr;
	int l;

	/*
		get string version of current date/time and separate it out into
		its individual elements ... day of week and time
	*/
	time_val = time ((time_t *)0);
	strcpy(date_time_string, ctime(&time_val));
	day_ptr = date_time_string;
	date_time_string[2] = '\0';
	memcpy(&date_time_string[13], &date_time_string[14], 2);
	hour_ptr = &date_time_string[11];
	date_time_string[15] = '\0';

	/*
		now parse out menu file entry date/time subfield and compare with
		above
	*/
	entry_buffer_ptr = entry_buffer;
	while (*entry_buffer_ptr)
	{
		/* check day part of subfield */
		if (!stryncmp(entry_buffer_ptr, "*", l=1))
		{
			have_date = TRUE;
			entry_buffer_ptr += l;
		}
		else if (isalpha(*entry_buffer_ptr))
		{
			if (!stryncmp(entry_buffer_ptr, msgs(m_menu_wk),
				l=strlen(msgs(m_menu_wk))))
			{
				if (stryncmp(day_ptr, msgs(m_menu_sat),
						strlen(msgs(m_menu_sat))) &&
				    stryncmp(day_ptr, msgs(m_menu_sun),
						strlen(msgs(m_menu_sun))))
				{
					have_date = TRUE;
				}
				entry_buffer_ptr += l;
			}
			else if (!stryncmp(entry_buffer_ptr, msgs(m_menu_sat),
						l=strlen(msgs(m_menu_sat))) ||
					 !stryncmp(entry_buffer_ptr, msgs(m_menu_sun),
						l=strlen(msgs(m_menu_sun))) ||
					 !stryncmp(entry_buffer_ptr, msgs(m_menu_mon),
						l=strlen(msgs(m_menu_mon))) ||
					 !stryncmp(entry_buffer_ptr, msgs(m_menu_tue),
						l=strlen(msgs(m_menu_tue))) ||
					 !stryncmp(entry_buffer_ptr, msgs(m_menu_wed),
						l=strlen(msgs(m_menu_wed))) ||
					 !stryncmp(entry_buffer_ptr, msgs(m_menu_thu),
						l=strlen(msgs(m_menu_thu))) ||
					 !stryncmp(entry_buffer_ptr, msgs(m_menu_fri),
						l=strlen(msgs(m_menu_fri))))
			{
				if (!stryncmp(day_ptr, entry_buffer_ptr, 2))
				{
					have_date = TRUE;
				}
				entry_buffer_ptr += l;
			}
			else
			{
				return (FALSE);
			}
		}
		else if (isdigit(*entry_buffer_ptr))
		{
			/* check time part of subfield */
			hour = atoi(hour_ptr);
			menu_hour1 = atoi(entry_buffer_ptr);
			if (menu_hour1 > 2359)
			{
				return (FALSE);
			}
			for (ctr = 0; ctr < 4; ctr++, entry_buffer_ptr++)
			{
				if (!isdigit(*entry_buffer_ptr))
				{
					return (FALSE);
				}
			}
			if (*entry_buffer_ptr != MENU_TIME_SEPARATOR_CHAR)
			{
				return (FALSE);
			}
			entry_buffer_ptr++;
			menu_hour2 = atoi(entry_buffer_ptr);
			if (menu_hour2 > 2359)
			{
				return (FALSE);
			}
			for (ctr = 0; ctr < 4; ctr++, entry_buffer_ptr++)
			{
				if (!isdigit(*entry_buffer_ptr))
				{
					return (FALSE);
				}
			}
			if (menu_hour1 <= menu_hour2 && !(hour >= menu_hour1
				&& hour <= menu_hour2))
			{
				have_time = FALSE;
			}
			else if (menu_hour1 >= menu_hour2
				&& !((hour >= menu_hour1 && hour >= menu_hour2)
					|| (hour <= menu_hour2 && hour <= menu_hour1)))
			{
				have_time = FALSE;
			}
			if (*entry_buffer_ptr)
			{
				/* more characters in date/time subfield ... invalid */
				return (FALSE);
			}
		}
		else
		{
			/* invalid character in date/time subfield */
			return (FALSE);
		}
	}
	if (validate_only)
	{
		return (TRUE);
	}

	return (have_date & have_time);
}


/*.
******************************************************************************
** FUNCTION:	generic_match()
**
** DESCRIPTION: This function determines if the buffer's parameters match the
**				  compare_string.  This function is used for user, group,
**				  tty, terminal, etc. comparisons.
**
** ENTRY:	   char *entry_buffer- format string
**			  char *compare_string- comparison string
**
** EXIT:		returns 0 if no match or error, 1 otherwise
**
** NOTES:	   The user(s) field limits the display of the menu item in the
**				  user's menu to those user's whose login name matches the
**				  users in the user(s) field.  The Any keyword means
**				  everyone.  If there is more than one user, each user is
**				  separated by a comma.  A user may be excluded by preceding
**				  the user name with the '!' character.  Any,uucp is
**				  redundant.  !jrp,joe,jrp gives permission to joe and jrp
**				  only.  Any,!jrp gives permission to all users except jrp.
**				  jrp,joe,!jrp gives permission to joe only.  The comma
**				  and '!' cannot be escaped by preceeding them with a '\'
**				  character.
**
**			  The group(s) field limits the display of the menu item in the
**				  user's menu to those user's whose real group matches the
**				  groups in the group(s) field.  The Any keyword means all
**				  groups.  If there is more than one group, each group is
**				  separated by a comma.  A group may be excluded by
**				  preceding the group name with the '!' character.
**				  Any,uucp is redundant.  !others,users,others gives
**				  permission to users in both the others and users groups.
**				  Any,!others gives permission to users in all groups except
**				  for those in the others group.  others,users,!others gives
**				  permission to users in the users group only.  The comma
**				  and '!' cannot be escaped by preceeding them with a '\'
**				  character.
**
**			  The tty(s) field limits the display of the menu item in the
**				  user's menu to those user's whose login tty matches the
**				  ttys in the tty(s) field.  The Any keyword means all ttys.
**				  If there is more than one tty, each tty is separated by a
**				  comma.  A tty may be excluded by preceding the tty name
**				  with the '!' character.  Any,tty01 is redundant.
**				  !tty01,tty02,tty01 gives permission to users logged on
**				  both tty01 and tty02.  Any,!tty01 gives permission to
**				  users logged on all ttys except for tty01.
**				  tty01,tty02,!tty01 gives permission to users logged on
**				  tty02 only.  The comma  and '!' cannot be escaped by
**				  preceeding them with a '\' character.
**
**			  The term(s) field limits the display of the menu item in the
**				  user's menu to those user's whose termimal ($TERM) matches
**				  the terms in the term(s) field.  The Any keyword means all
**				  terminals.  If there is more than one term, each term is
**				  separated by a comma.  A term may be excluded by preceding
**				  the term name with the '!' character.  Any,vt100 is
**				  redundant.  !vt100,vt102,vt100 gives permission to users
**				  logged on both vt100 and vt102.  Any,!vt100 gives
**				  permission to users logged on all terms except for vt100.
**				  vt102,vt100,!vt102 gives permission to users logged on
**				  vt100 only.  A term is represented by the user's $TERM
**				  environment variable. The comma and '!' cannot be escaped
**				  by preceeding them with a '\' character.
**
*****************************************************************************/
static int generic_match(char *entry_buffer, char *compare_string)
{
	char *ptr;
	char *item;
	int exclude;
	int have_match = FALSE;

	item = ptr = entry_buffer;
	while (*ptr)
	{
		if (*ptr == MENU_GRP_SEPARATOR_CHAR)
		{
			*ptr = '\0';
			exclude = 0;
			if (*item == MENU_EXCLUDE_CHAR)
			{
				item++;
				exclude++;
			}
			if (!stryncmp(item, "*", 1))
			{
				if (exclude)
				{
					return (0);
				}
				have_match = TRUE;
			}
			else if (!strcmp(compare_string, item))
			{
				have_match = !exclude;
			}
			ptr++;
			item = ptr;
		}
		ptr++;
	}
	exclude = 0;
	if (*item == MENU_EXCLUDE_CHAR)
	{
		item++;
		exclude++;
	}
	if (!stryncmp(item, "*", 1))
	{
		if (exclude)
		{
			return (0);
		}
		have_match = TRUE;
	}
	else if (!strcmp(compare_string, item))
	{
		have_match = !exclude;
	}

	return (have_match);
}


/*.
******************************************************************************
** FUNCTION:	make_menu_entry()
**
** DESCRIPTION: This function creates a menu entry
**
** ENTRY:	   none
**
** EXIT:		returns ptr to meny entry or NULL if error
**
** NOTES:	   NONE
**
*****************************************************************************/
static MENU *make_menu_entry (void)
{
	MENU *m;

	m = (MENU *)MALLOC(sizeof(MENU));
	if (m)
	{
		m->category         = 0;
		m->title            = 0;
		m->exec_path        = 0;
		m->date_time_string = 0;
		m->collapsed        = FALSE;
	}
	return (m);
}


/*.
******************************************************************************
** FUNCTION:	free_menu_entry()
**
** DESCRIPTION: This function frees a menu entry
**
** ENTRY:	   pointer to a menu entry
**
** EXIT:		none
**
** NOTES:	   NONE
**
*****************************************************************************/
static int free_menu_entry (MENU *m)
{
	if (m->category)
		FREE(m->category);
	if (m->title)
		FREE(m->title);
	if (m->exec_path)
		FREE(m->exec_path);
	if (m->date_time_string)
		FREE(m->date_time_string);

	return (0);
}

/*.
******************************************************************************
** FUNCTION:	use_this_menu_entry()
**
** DESCRIPTION: This function takes a menu file entry, parses it, determines
**				  if it passes our 'rules', and puts it in 'menu_entry'
**				  if it passes muster.
**
** ENTRY:	   MENU *menu_entry- place to put valid entry
**			  char *menu_string- the menu file string
**			  char *ltty- login tty string
**			  char *r_group- string representation of the user's real group
**			  int uid- the user's user id
**
** EXIT:		returns ptr to meny entry or NULL if error
**
** NOTES:	   NONE
**
*****************************************************************************/
static MENU  *use_this_menu_entry(char *menu_string,
					char *userid, char *ltty, char *r_group, int uid)
{
	int have_category = FALSE;
	int have_title = FALSE;
	int have_user = FALSE;
	int have_group = FALSE;
	int have_tty = FALSE;
	int have_term = FALSE;
	int have_exec = FALSE;
	int have_date_time = FALSE;
	int have_escape_char = FALSE;
	int have_user_match = FALSE;
	int have_group_match = FALSE;
	int have_tty_match = FALSE;
	int have_trm_match = FALSE;
	char entry_buffer[MAX_MENU_ENTRY_LENGTH];
	char *entry_buffer_ptr;
	char *ptr;
	char *category_ptr;
	char *title_ptr;
	char *exec_ptr;
	char *date_ptr;
	MENU *menu_entry;

	/* zap possible nl at end of line */
	ptr = strrchr(menu_string, '\n');
	if (ptr)
	{
		*ptr = 0;
	}
	ptr = menu_string;

	category_ptr = 0;
	title_ptr    = 0;
	exec_ptr     = 0;
	date_ptr     = 0;

	entry_buffer_ptr = entry_buffer;
	*entry_buffer_ptr = '\0';
	while (*ptr)
	{
		if (*ptr == MENU_SEPARATOR_CHAR && have_escape_char)
		{
			*entry_buffer_ptr++ = *ptr;
			*entry_buffer_ptr = '\0';
			have_escape_char = FALSE;
		}
		else if (*ptr == MENU_SEPARATOR_CHAR)
		{
			/* don't allow empty fields */
			if (!*entry_buffer)
			{
				break;
			}

			if (!have_category)
			{
				category_ptr = (char *)MALLOC(strlen(entry_buffer)+1);
				if (! category_ptr)
				{
					break;
				}
				strcpy(category_ptr, entry_buffer);
				have_category = TRUE;
			}
			else if (!have_title)
			{
				title_ptr = (char *)MALLOC(strlen(entry_buffer)+1);
				if (! title_ptr)
				{
					break;
				}
				strcpy(title_ptr, entry_buffer);
				have_title = TRUE;
			}
			else if (!have_user)
			{
				if (uid)
				{
					if (!(have_user_match
						= generic_match(entry_buffer, userid)))
						break;
				}
				else
					have_user_match = TRUE;
				have_user = TRUE;
			}
			else if (!have_group)
			{
				if (uid)
				{
					if (!(have_group_match
						= generic_match(entry_buffer, r_group)))
						break;
				}
				else
					have_group_match = TRUE;
				have_group = TRUE;
			}
			else if (!have_tty)
			{
				if (uid)
				{
					if (!(have_tty_match
						= generic_match(entry_buffer, ltty)))
						break;
				}
				else
					have_tty_match = TRUE;
				have_tty = TRUE;
			}
			else if (!have_term)
			{
				if (uid)
				{
					if (!(have_trm_match
						= generic_match(entry_buffer, getenv("TERM"))))
						break;
				}
				else
					have_trm_match = TRUE;
				have_term = TRUE;
			}
			else if (!have_date_time)
			{
				if (uid && !date_time_matches(entry_buffer, 1))
				{
					break;
				}
				date_ptr = (char *)MALLOC(strlen(entry_buffer)+1);
				if (! date_ptr)
				{
					break;
				}
				strcpy(date_ptr, entry_buffer);
				have_date_time = TRUE;
			}
			entry_buffer_ptr = entry_buffer;
			*entry_buffer_ptr = '\0';
		}
		else if (*ptr == MENU_COMMENT_CHAR && have_escape_char)
		{
			*entry_buffer_ptr++ = *ptr;
			*entry_buffer_ptr = '\0';
			have_escape_char = FALSE;
		}
		else if (*ptr == MENU_COMMENT_CHAR)
		{
			break;
		}
		else if (*ptr == MENU_ESCAPE_CHAR && have_escape_char)
		{
			*entry_buffer_ptr++ = *ptr;
			*entry_buffer_ptr = '\0';
			have_escape_char = FALSE;
		}
		else if (*ptr == MENU_ESCAPE_CHAR)
		{
			have_escape_char = TRUE;
		}
		else
		{
			*entry_buffer_ptr++ = *ptr;
			*entry_buffer_ptr = '\0';
			have_escape_char = FALSE;
		}
		ptr++;
	}
	if (have_category && have_user && have_group && have_tty && have_term
		&& have_title && have_date_time && !have_exec)
	{
		/* don't allow empty fields */
		if (*entry_buffer &&
			(exec_ptr = (char *)MALLOC(strlen(entry_buffer)+1)))
		{
			strcpy(exec_ptr, entry_buffer);
			have_exec = TRUE;
		}
	}

	if (have_category && have_user_match && have_group_match && have_title
		&& have_exec && have_date_time && have_tty_match && have_trm_match)
	{
		menu_entry = make_menu_entry();
		if (!menu_entry)
		{
			if (category_ptr)
				FREE(category_ptr);
			if (title_ptr)
				FREE(title_ptr);
			if (exec_ptr)
				FREE(exec_ptr);
			if (date_ptr)
				FREE(date_ptr);
			return (0);
		}
		menu_entry->category         = category_ptr;
		menu_entry->title            = title_ptr;
		menu_entry->exec_path        = exec_ptr;
		menu_entry->date_time_string = date_ptr;
		return (menu_entry);
	}
	else
	{
		if (category_ptr)
			FREE(category_ptr);
		if (title_ptr)
			FREE(title_ptr);
		if (exec_ptr)
			FREE(exec_ptr);
		if (date_ptr)
			FREE(date_ptr);

		return (0);
	}
}


/*.
******************************************************************************
** FUNCTION:	MENU *read_menu_file()
**
** DESCRIPTION: This function reads in menu file entries and builds an array
**				  of menu_struct's from valid entries.
**
** ENTRY:	   char *file_path- menu file path
**			  int *menu_count- address of # of elements in returned
**				  menu_struct array.
**
** EXIT:		returns array of valid menu structures from the menu file and
**				  sets 'menu_count' to the number of elements in that array;
**				  'menu_count' is set to 0 and NULL is returned on error
**				  or 0 valid menu items.
**
** NOTES:	   NONE
**
*****************************************************************************/
static MENU *read_menu_file(char *file_path, int *menu_count)
{
	char r_user[256]; /* real user name */
	char r_group[256]; /* real group name */
	FILE *menu_file_ptr;
	int r_gid;
	int uid;
	char menu_string[MAX_MENU_ENTRY_LENGTH];
	MENU *mp;
	MENU *m;
	BLIST *menu_list;
	BLIST *ml;
	char *ptr;
	int ctr;
	char *userid;
	char *ltty;

	/* get user/group ids of user */

	uid = os_get_uid();
	*r_user = 0;
	os_get_usr_name(r_user);
	userid = r_user;

	ltty = get_login_tty();
	r_gid = os_get_gid();
	*r_group = 0;
	os_get_grp_name_from_id(r_gid, r_group);
	if (*r_group == 0)
	{
		return (0);
	}

	/* open up menu file */

	menu_file_ptr = fopen(file_path, "rb");
	if (!menu_file_ptr)
	{
		errmsg(ER_NOMENU, "", ERR_ANY);
		return (0);
	}

	/*
		read each line of the menu file, putting valid entries into
		a menu array
	*/

	menu_list = 0;
	ptr = 0;
	while (fgets(menu_string, sizeof(menu_string), menu_file_ptr))
	{
		if (menu_string[0] != MENU_COMMENT_CHAR)
		{
			mp = use_this_menu_entry(menu_string, userid, ltty, r_group, uid);
			if (mp)
			{
				/* check if new category */

				if (!ptr || strcmp(ptr, mp->category))
				{
					m = make_menu_entry();
					if (m)
					{
						m->category = (char *)MALLOC(strlen(mp->category)+1);
						if (m->category)
						{
							strcpy(m->category, mp->category);
							*menu_count += 1;
							ml = BNEW(m);
							if (ml)
							{
								menu_list = bappend(menu_list, ml);
								ptr = m->category;
							}
							else
							{
								FREE(m->category);
								FREE(m);
							}
						}
						else
						{
							FREE(m);
						}
					}
				}

				/* add new entry to list */

				ml = BNEW(mp);
				if (!ml)
				{
					free_menu_entry(mp);
					FREE(mp);
				}
				else
				{
					menu_list = bappend(menu_list, ml);
					*menu_count += 1;
				}
			}
		}
	}
	fclose(menu_file_ptr);

	/* create array of menu entries */

	mp = (MENU *)MALLOC(*menu_count * sizeof(MENU));
	ctr = 0;
	for (ml=menu_list; ml; ml=bnext(ml))
	{
		m = (MENU *)bid(ml);
		if (mp)
		{
			mp[ctr].category         = m->category;
			mp[ctr].title            = m->title;
			mp[ctr].exec_path        = m->exec_path;
			mp[ctr].date_time_string = m->date_time_string;
			mp[ctr].collapsed        = m->collapsed;
			ctr++;
		}
		FREE(m);
	}
	BSCRAP(menu_list, FALSE);

	return (mp);
}


/*****************************************************************************
**
** MENU DISPLAY ROUTINES
**
*****************************************************************************/

/*.
******************************************************************************
** FUNCTION:	draw_menu_sub_window()
**
** DESCRIPTION: This function draws/redraws the menu list window.
**
** ENTRY:	   NONE
**
** EXIT:		NONE
**
** NOTES:	   NONE
**
*****************************************************************************/
static void draw_menu_sub_window(void)
{
	MENU_INFO *	mi = &gbl(menu_info);
	int loop_ctr, ctr, ctr1;

	if (mi->display_menu_count)
	{
		loop_ctr = ctr = mi->menu_top_item;
		while (loop_ctr < mi->menu_count && ctr < mi->display_menu_count
			&& ctr - mi->menu_top_item < mi->menu_item_lines)
		{
			wmove(gbl(win_menu_sub), ctr - mi->menu_top_item, 0);
			if (mi->menu_ptr[loop_ctr].title == 0)
			{
				/* category */
				if (ctr == mi->menu_marked_item)
				{
					wattrset(gbl(win_menu_sub), gbl(scr_menu_cursor_attr));
				}
				else
				{
					wattrset(gbl(win_menu_sub), gbl(scr_menu_lolite_attr));
				}
				if (mi->menu_ptr[loop_ctr].collapsed)
				{
					waddch(gbl(win_menu_sub), COLLAPSED_CATEGORY_MARKER);
				}
				else
				{
					waddch(gbl(win_menu_sub), ' ');
				}
				/* clip if necessary */
				if ((int)strlen(mi->menu_ptr[loop_ctr].category) + 1
					> mi->menu_window_width)
				{
					mi->menu_ptr[loop_ctr].category[mi->menu_window_width] = 0;
				}
				waddstr(gbl(win_menu_sub), mi->menu_ptr[loop_ctr].category);
				/* add blanks */
				if (ctr != mi->menu_marked_item)
				{
					wattrset(gbl(win_menu_sub), gbl(scr_menu_lolite_attr));
				}
				for (ctr1 = strlen(mi->menu_ptr[loop_ctr].category) + 1;
					ctr1 < mi->menu_window_width; ctr1++)
				{
					waddch(gbl(win_menu_sub), ' ');
				}
				ctr++;
			}
			else if (!mi->menu_ptr[loop_ctr].collapsed)
			{
				/* title */
				if (ctr == mi->menu_marked_item)
				{
					wattrset(gbl(win_menu_sub), gbl(scr_menu_cursor_attr));
				}
				else
				{
					wattrset(gbl(win_menu_sub), gbl(scr_menu_lolite_attr));
				}
				if (opt(show_appmenu_lines))
				{
					waddch(gbl(win_menu_sub), ' ');
					waddch(gbl(win_menu_sub), ' ');
					if (loop_ctr+1 == mi->menu_count ||
						mi->menu_ptr[loop_ctr+1].title == 0)
					{
						/* category follows or last menu item */
						wbox_chr(gbl(win_menu_sub), B_BL, B_SVSH);
					}
					else
					{
						wbox_chr(gbl(win_menu_sub), B_ML, B_SVSH);
					}
					for (ctr1 = 3; ctr1 < TITLE_INDENT; ctr1++)
					{
						wbox_chr(gbl(win_menu_sub), B_HO, B_SVSH);
					}
				}
				else
				{
					for (ctr1 = 0; ctr1 < TITLE_INDENT; ctr1++)
					{
						waddch(gbl(win_menu_sub), ' ');
					}
				}
				/* clip if necessary */
				if ((int)strlen(mi->menu_ptr[loop_ctr].title) + TITLE_INDENT
					> mi->menu_window_width)
				{
					mi->menu_ptr[loop_ctr].title[mi->menu_window_width] = '\0';
				}
				waddstr(gbl(win_menu_sub), mi->menu_ptr[loop_ctr].title);
				/* add blanks */
				if (ctr != mi->menu_marked_item)
				{
					wattrset(gbl(win_menu_sub), gbl(scr_menu_lolite_attr));
				}
				for (ctr1 = strlen(mi->menu_ptr[loop_ctr].title)+TITLE_INDENT;
					ctr1 < mi->menu_window_width; ctr1++)
				{
					waddch(gbl(win_menu_sub), ' ');
				}
				ctr++;
			}
		loop_ctr++;
		}
	}
	else
	{
		wmove(gbl(win_menu_sub), 0, 0);
		wattrset(gbl(win_menu_sub), gbl(scr_menu_cursor_attr));
		wclrtoeol(gbl(win_menu_sub));
		ctr = 1;
	}

	/* blank out unmarked lines */

	wattrset(gbl(win_menu_sub), gbl(scr_menu_lolite_attr));
	for (; ctr - mi->menu_top_item < mi->menu_item_lines; ctr++)
	{
		wmove(gbl(win_menu_sub), ctr - mi->menu_top_item, 0);
		wclrtoeol(gbl(win_menu_sub));
	}
	wrefresh(gbl(win_menu_sub));
	menu_scrollbar();
	position_menu_cursor();

	return;
}


/*.
******************************************************************************
** FUNCTION:	unmark_mark_items()
**
** DESCRIPTION: This function 'unmarks' one menu item and 'marks' the other.
**
** ENTRY:	   int unmark_item- item to 'unmark'
**			  int mark_item- item to 'mark'
**
** EXIT:		NONE
**
** NOTES:	   NONE
**
*****************************************************************************/
static void unmark_mark_items(int unmark_item, int mark_item)
{
	MENU_INFO *	mi = &gbl(menu_info);
	int ctr;
	chtype ch;
	chtype a;
	chtype t;
	int y;

	/* unmark */
	y = unmark_item - mi->menu_top_item;
	for (ctr = 0; ctr < mi->menu_window_width; ctr++)
	{
		ch = wchat(gbl(win_menu_sub), y, ctr);
		a = A_GETATTR(ch) & A_ALTCHARSET;
		t = A_GETTEXT(ch);
		ch = t | gbl(scr_menu_lolite_attr) | a;
		woutch(gbl(win_menu_sub), y, ctr, ch);
	}
	wtouchln(gbl(win_menu_sub), y, 1, TRUE);

	/* mark */
	y = mark_item - mi->menu_top_item;
	for (ctr = 0; ctr < mi->menu_window_width; ctr++)
	{
		ch = wchat(gbl(win_menu_sub), y, ctr);
		a = A_GETATTR(ch) & A_ALTCHARSET;
		t = A_GETTEXT(ch);
		ch = t | gbl(scr_menu_cursor_attr) | a;
		woutch(gbl(win_menu_sub), y, ctr, ch);
	}
	wtouchln(gbl(win_menu_sub), y, 1, TRUE);

	wrefresh(gbl(win_menu_sub));
	position_menu_cursor();

	return;
}


/*.
******************************************************************************
** FUNCTION:	redraw_menu_windows()
**
** DESCRIPTION: This function draws the menu.
**
** ENTRY:	   NONE
**
** EXIT:		NONE
**
** NOTES:	   NONE
**
*****************************************************************************/
static void redraw_menu_windows(void)
{
	MENU_INFO *	mi = &gbl(menu_info);
	int menu_border_type = B_DVDH;
	int l;

	mi->menu_item_lines = getmaxy(gbl(win_menu_sub));
	mi->menu_window_width = getmaxx(gbl(win_menu_sub));

	wattrset(gbl(win_menu_reg), gbl(scr_menu_lolite_attr));
	werase(gbl(win_menu_reg));

	/* draw boxes around windows */

	wattrset(gbl(win_menu_reg), gbl(scr_menu_border_attr));
	wbox(gbl(win_menu_reg), menu_border_type);
	box_around(gbl(win_menu_sub), gbl(win_menu_reg), menu_border_type);

	/* draw menu title */

	wattrset(gbl(win_menu_reg), gbl(scr_menu_hilite_attr));
	l = strlen(package_name()) + strlen(msgs(m_menu_title)) + 3;
	wmove(gbl(win_menu_reg), 0, (getmaxx(gbl(win_menu_reg))-l)/2);
	waddch(gbl(win_menu_reg),' ');
	waddstr(gbl(win_menu_reg), package_name());
	waddch(gbl(win_menu_reg),' ');
	waddstr(gbl(win_menu_reg), msgs(m_menu_title));
	waddch(gbl(win_menu_reg),' ');

	/* draw hot key commands */

	wattrset (gbl(win_menu_reg), gbl(scr_menu_lolite_attr));
	wstandset(gbl(win_menu_reg), gbl(scr_menu_hilite_attr));
	wmove(gbl(win_menu_reg), getmaxy(gbl(win_menu_reg))-2, 2);
	xcaddstr(gbl(win_menu_reg), CMDS_MENU_RUN, msgs(m_menu_run));
	waddstr(gbl(win_menu_reg), "	");
	xcaddstr(gbl(win_menu_reg), CMDS_COMMON_QUIT, msgs(m_menu_quit));
	wrefresh(gbl(win_menu_reg));

	mi->slider = -1;
	wattrset(gbl(win_menu_reg), gbl(scr_menu_border_attr));
	scrollbar(gbl(win_menu_reg), gbl(win_menu_sub), SCRLBAR_VL, -1, 0);
	wattrset(gbl(win_menu_reg), gbl(scr_menu_lolite_attr));
	draw_menu_sub_window();
}


/*.
******************************************************************************
** FUNCTION:	subst_exec_params()
**
** DESCRIPTION: This function sustitues current dir name for %D, current
**				file name for %F, and all tagged filenames for %T
**
** ENTRY:	   char *menu_entry_exec_string- menu item execute string
**
** EXIT:		returns 0 on failure ('format string' found and 'subst_string'
**				  is NULL) or 1 on success
**
** NOTES:	   NONE
**
*****************************************************************************/
static int subst_exec_params(char *menu_entry_exec_string)
{
	MENU_INFO *	mi = &gbl(menu_info);
	char *p;
	char *e;
	int need_subst = FALSE;
	int len;
	BLIST *b;
	FBLK *f;
	char path[MAX_PATHLEN];
	int prepend_dir = FALSE;

	/* loop thru cmd looking for subst parms */

	len = strlen(menu_entry_exec_string) + 1;
	for (p=menu_entry_exec_string; *p; p++)
	{
		if (*p == MENU_EXEC_SUB_CHAR)
		{
			p++;
			if (*p == *msgs(m_menu_esd))
			{
				if (p[1] == '/' || p[1] == '\\')
				{
					prepend_dir = TRUE;
				}
				else
				{
					if (!gbl(scr_cur)->cur_dir_tree)
					{
						errmsg(ER_RCDP, "", ERR_ANY);
						return (0);
					}
					dirtree_to_dirname(gbl(scr_cur)->cur_dir_tree, path);
					len += strlen(path) + 3;
				}

				need_subst = TRUE;
			}
			else if (*p == *msgs(m_menu_esf))
			{
				if (!gbl(scr_cur)->cur_file)
				{
					errmsg(ER_RSFP, "", ERR_ANY);
					return (0);
				}

				if (prepend_dir)
				{
					fblk_to_pathname(gbl(scr_cur)->cur_file, path);
					len += strlen(path) + 3;
				}
				else
				{
					len += strlen(FULLNAME(gbl(scr_cur)->cur_file)) + 3;
				}
				need_subst = TRUE;
				prepend_dir = FALSE;
			}
			else if (*p == *msgs(m_menu_est))
			{
				if (!gbl(scr_cur)->dir_tagged_count)
				{
					errmsg(ER_RTFP, "", ERR_ANY);
					return (0);
				}
				len += get_tagged_file_len(prepend_dir) + 3;
				need_subst = TRUE;
				prepend_dir = FALSE;
			}
			else if (*p != MENU_EXEC_SUB_CHAR)
			{
				return (0);
			}
		}
	}

	/* check if any substitution needed */

	if (!need_subst)
		return (1);

	/* loop thru again, making substitutions */

	mi->exec_buffer = (char *)MALLOC(len);
	if (mi->exec_buffer == 0)
	{
		return (0);
	}

	e = mi->exec_buffer;
	prepend_dir = FALSE;
	for (p=menu_entry_exec_string; *p; p++)
	{
		if (*p == MENU_EXEC_SUB_CHAR)
		{
			if (p[1] == *msgs(m_menu_esd))
			{
				p++;
				if (p[1] == '/' || p[1] == '\\')
				{
					prepend_dir = TRUE;
					p++;
					continue;
				}
				else
				{
					*e++ = '"';
					dirtree_to_dirname(gbl(scr_cur)->cur_dir_tree, e);
					e += strlen(e);
					*e++ = '"';
				}
			}
			else if (p[1] == *msgs(m_menu_esf))
			{
				p++;
				*e++ = '"';
				if (prepend_dir)
				{
					fblk_to_pathname(gbl(scr_cur)->cur_file, e);
				}
				else
				{
					strcpy(e, FULLNAME(gbl(scr_cur)->cur_file));
				}
				e += strlen(e);
				*e++ = '"';

				prepend_dir = FALSE;
			}
			else if (p[1] == *msgs(m_menu_est))
			{
				p++;
				for (b=gbl(scr_cur)->first_file; b; b=bnext(b))
				{
					f = (FBLK *)bid(b);
					if (is_file_tagged(f))
					{
						*e++ = '"';
						if (prepend_dir)
						{
							fblk_to_pathname(f, e);
						}
						else
						{
							strcpy(e, FULLNAME(f));
						}
						e += strlen(e);
						*e++ = '"';
						*e++ = ' ';
					}
				}

				prepend_dir = FALSE;
			}
			else if (p[1] == MENU_EXEC_SUB_CHAR)
			{
				p++;
				*e++ = *p;
			}
		}
		else
		{
			*e++ = *p;
		}
	}
	*e = 0;
	return (1);
}

/*.
******************************************************************************
** FUNCTION:	Menu_paint_menu()
**
** DESCRIPTION: This function creates the menu windows.
**
** ENTRY:	   NONE
**
** EXIT:		NONE
**
** NOTES:	   also can be called when get SIGWINCH
**
*****************************************************************************/
void Menu_paint_menu(void)
{
	MENU_INFO *	mi = &gbl(menu_info);

	/* turn off cursor if we can */

	if (curs_set(0) != ERR)
		mi->menu_cursor_inv = TRUE;
	else
		mi->menu_cursor_inv = FALSE;

	/* get window sizes */

	mi->menu_item_lines = getmaxy(gbl(win_menu_sub));
	mi->menu_window_width = getmaxx(gbl(win_menu_sub));

	/* setup attributes for message window */

	wattrset (gbl(win_message), gbl(scr_menu_lolite_attr));
	wstandset(gbl(win_message), gbl(scr_menu_hilite_attr));
	bang("");

	redraw_menu_windows();
}


/*.
******************************************************************************
** FUNCTION:	find_marked_items_menu_index()
**
** DESCRIPTION: This function finds the index of the currently marked menu
**				  item in 'menu_ptr' array.
**
** ENTRY:	   NONE
**
** EXIT:		returns index into 'menu_ptr'
**
** NOTES:	   necessary to support collapsable categories.
**
*****************************************************************************/
static int find_marked_items_menu_index(void)
{
	MENU_INFO *	mi = &gbl(menu_info);
	register int ctr, index;

	index = -1;
	ctr = 0;
	while (ctr < mi->menu_count)
	{
		if (mi->menu_ptr[ctr].title == 0)
		{
			/* category */
			index++;
		}
		else
		{
			/* title */
			if (!mi->menu_ptr[ctr].collapsed)
			{
				index++;
			}
		}
		if (index == mi->menu_marked_item)
		{
			break;
		}
		ctr++;
	}
	return (ctr);
}


/*.
******************************************************************************
** FUNCTION:	restore_branches()
**
** DESCRIPTION: This function restores all collapsed category branches in the
**				  menu display.
**
** ENTRY:	   NONE
**
** EXIT:		NONE
**
** NOTES:	   NONE
**
*****************************************************************************/
static void restore_branches(void)
{
	MENU_INFO *	mi = &gbl(menu_info);
	int marked_item_index;
	int collapsed = FALSE;
	int diff;
	register int ctr;

	diff = mi->menu_marked_item - mi->menu_top_item;
	marked_item_index = find_marked_items_menu_index();
	for (ctr = 0; ctr < mi->menu_count; ctr++)
	{
		if (mi->menu_ptr[ctr].collapsed)
		{
			mi->menu_ptr[ctr].collapsed = FALSE;
			collapsed = TRUE;
		}
	}

	if (collapsed)
	{
		/* redraw list window */
		mi->menu_marked_item = marked_item_index;
		mi->menu_top_item = mi->menu_marked_item - diff;
		mi->display_menu_count = mi->menu_count;
		/* adjust top item if necessary */
		if (mi->menu_top_item
			> mi->display_menu_count - mi->menu_item_lines)
		{
			if ((mi->menu_top_item
				= mi->display_menu_count - mi->menu_item_lines) < 0)
			{
				mi->menu_top_item = 0;
			}
		}
		draw_menu_sub_window();
	}
	else
	{
		do_beep();
	}
	return;
}


/*.
******************************************************************************
** FUNCTION:	collapse_branches()
**
** DESCRIPTION: This function collapses all category branches in the
**				  menu display.
**
** ENTRY:	   NONE
**
** EXIT:		NONE
**
** NOTES:	   NONE
**
*****************************************************************************/
static void collapse_branches(void)
{
	MENU_INFO *	mi = &gbl(menu_info);
	int marked_item_index;
	int real_marked_item_index = -1;
	int collapsed = FALSE;
	int window_count = 0;
	int diff;
	register int ctr;

	marked_item_index = find_marked_items_menu_index();
	for (ctr = marked_item_index; ctr >= 0; ctr--)
	{
		if (mi->menu_ptr[ctr].title == 0)
		{
			break;
		}
	}
	marked_item_index = ctr;
	diff = mi->menu_marked_item - mi->menu_top_item;
	for (ctr = 0; ctr < mi->menu_count; ctr++)
	{
		if (mi->menu_ptr[ctr].title == 0)
		{
			/* category */
			window_count++;
		}
		if (!mi->menu_ptr[ctr].collapsed)
		{
			mi->menu_ptr[ctr].collapsed = TRUE;
			collapsed = TRUE;
		}
	}
	for (ctr = 0; ctr < mi->menu_count; ctr++)
	{
		if (mi->menu_ptr[ctr].title == 0)
		{
			/* category */
			real_marked_item_index++;
		}
		if (marked_item_index == ctr)
		{
			break;
		}
	}

	if (collapsed)
	{
		/* redraw list window */
		mi->display_menu_count = window_count;
		mi->menu_marked_item = real_marked_item_index;
		mi->menu_top_item = mi->menu_marked_item - diff;
		/* adjust top item if necessary */
		if (mi->menu_top_item < 0)
		{
			mi->menu_top_item = 0;
		}

		draw_menu_sub_window();
	}
	else
	{
		do_beep();
	}
	return;
}


/*.
******************************************************************************
** FUNCTION:	collapse_restore_branch()
**
** DESCRIPTION: This function collapses a category branch or restores a
**				  collapsed category branch in the menu display.
**
** ENTRY:	   int collapse- restore branch == 0 and  collapse branch == 1
**
** EXIT:		NONE
**
** NOTES:	   NONE
**
*****************************************************************************/
static void collapse_restore_branch(int collapse)
{
	MENU_INFO *	mi = &gbl(menu_info);
	int marked_item_index;
	int category_collapsed;
	register int ctr;

	/* get 'menu_ptr' array index of current marked item */
	marked_item_index = find_marked_items_menu_index();

	/* check to make sure that marked item is a category and not collapsed */
	if (mi->menu_ptr[marked_item_index].title != 0
		|| collapse == mi->menu_ptr[marked_item_index].collapsed)
	{
		do_beep();
		return;
	}

	/* marked collapsed/uncollapsed category and member titles */
	category_collapsed = FALSE;
	mi->display_menu_count = 0;
	mi->menu_ptr[marked_item_index].collapsed = collapse;
	for (ctr = 0; ctr < mi->menu_count; ctr++)
	{
		if (mi->menu_ptr[ctr].title == 0)
		{
			/* category */
			category_collapsed = mi->menu_ptr[ctr].collapsed;
			mi->display_menu_count++;
		}
		else
		{
			/* title */
			mi->menu_ptr[ctr].collapsed = category_collapsed;
			if (! category_collapsed)
			{
				mi->display_menu_count++;
			}
		}
	}

	/* adjust top item if necessary */
	if (mi->menu_top_item
		> mi->display_menu_count - mi->menu_item_lines)
	{
		if ((mi->menu_top_item
			= mi->display_menu_count - mi->menu_item_lines) < 0)
		{
			mi->menu_top_item = 0;
		}
	}

	/* redraw list window */
	draw_menu_sub_window();
	return;
}


/*.
******************************************************************************
** FUNCTION:	do_user_io()
**
** DESCRIPTION: This function takes user input and acts accordingly.
**
** ENTRY:	   None
**
** EXIT:		NONE
**
** NOTES:	   NONE
**
*****************************************************************************/
static void do_user_io(void)
{
	MENU_INFO *	mi = &gbl(menu_info);
	int ch;
	int uid;
	int index;
	int m;
	int j;

	uid = os_get_uid();
	while (TRUE)
	{
		ch = get_input_char(gbl(win_menu_reg),
			redraw_menu_windows, 0, position_menu_cursor);

		if (ch == KEY_MOUSE)
		{
			m = mouse_get_event(gbl(win_menu_sub));
			if (m == MOUSE_DRAG || m == MOUSE_LBD || m == MOUSE_LBDBL)
			{
				j = check_mouse_in_win(gbl(win_menu_sub),
					mi->display_menu_count);
				if (j == 0)
				{
					continue;
				}
				else if (j == 1)
				{
					ch = KEY_UP;
				}
				else if (j == 2)
				{
					ch = KEY_DOWN;
				}
				else if (j == 3)
				{
					j = mouse_y(gbl(win_menu_sub)) - getbegy(gbl(win_menu_sub));
					j = mi->menu_top_item + j;
					unmark_mark_items(mi->menu_marked_item, j);
					mi->menu_marked_item = j;
					menu_scrollbar();
					if (m == MOUSE_LBDBL)
						ch = cmds(CMDS_MENU_RUN);
				}
				else if (j == 4)
				{
					j = read_vert_scrollbar(gbl(win_menu_sub),
						mi->display_menu_count);
					if (j != mi->slider)
					{
						if (j >= mi->menu_top_item &&
							j-mi->menu_top_item <
								mi->menu_item_lines)
						{
							unmark_mark_items(mi->menu_marked_item, j);
							mi->menu_marked_item = j;
							menu_scrollbar();
						}
						else
						{
							mi->menu_top_item = calculate_top_item(j,
								mi->display_menu_count, mi->menu_item_lines);
							mi->menu_marked_item = j;
							draw_menu_sub_window();
						}
					}
				}
			}
			else
			{
				continue;
			}
		}

		if (ch == KEY_ESCAPE || ch == cmds(CMDS_COMMON_QUIT))
		{
			break;
		}

		else if (mi->display_menu_count)
		{
			if (ch == KEY_UP || ch == KEY_LEFT)
			{
				/* line up */
				if (mi->menu_marked_item
					&& mi->menu_marked_item == mi->menu_top_item)
				{
					mi->menu_marked_item--;
					mi->menu_top_item--;
					draw_menu_sub_window();
				}
				else if (mi->menu_marked_item)
				{
					unmark_mark_items(mi->menu_marked_item,
										mi->menu_marked_item - 1);
					mi->menu_marked_item--;
					menu_scrollbar();
				}
				else
				{
					do_beep();
				}
			}

			else if (ch == KEY_DOWN || ch == KEY_RIGHT || ch == KEY_SPACE)
			{
				/* line down */
				if (mi->menu_marked_item < mi->display_menu_count - 1)
				{
					if (mi->menu_marked_item
						== mi->menu_top_item +
							mi->menu_item_lines - 1)
					{
						mi->menu_marked_item++;
						mi->menu_top_item++;
						draw_menu_sub_window();
					}
					else
					{
						unmark_mark_items(mi->menu_marked_item,
											mi->menu_marked_item + 1);
						mi->menu_marked_item++;
						menu_scrollbar();
					}
				}
				else
				{
					do_beep();
				}
			}

			else if (ch == KEY_HOME)
			{
				/* home */
				if (mi->menu_marked_item)
				{
					if (mi->menu_top_item)
					{
						mi->menu_marked_item = 0;
						mi->menu_top_item = 0;
						draw_menu_sub_window();
					}
					else
					{
						unmark_mark_items(mi->menu_marked_item, 0);
						mi->menu_marked_item = 0;
						menu_scrollbar();
					}
				}
				else
				{
					do_beep();
				}
			}

			else if (ch == KEY_END)
			{
				/* end */
				if (mi->menu_marked_item < mi->display_menu_count - 1)
				{
					if (mi->display_menu_count <= mi->menu_item_lines
						|| mi->menu_top_item ==
							mi->display_menu_count - (mi->menu_item_lines))
					{
						unmark_mark_items(mi->menu_marked_item,
											mi->display_menu_count - 1);
						mi->menu_marked_item = mi->display_menu_count - 1;
						menu_scrollbar();
					}
					else
					{
						mi->menu_marked_item = mi->display_menu_count - 1;
						mi->menu_top_item
							= mi->display_menu_count - (mi->menu_item_lines);
						draw_menu_sub_window();
					}
				}
				else
				{
					do_beep();
				}
			}

			else if (ch == KEY_PGUP)
			{
				/* previous page */
				if (!mi->menu_top_item
					|| mi->menu_top_item != mi->menu_marked_item)
				{
					/* top item = 0 or marked item != top item
						so marked item := top item */
					if (mi->menu_marked_item)
					{
						unmark_mark_items(mi->menu_marked_item,
							mi->menu_top_item);
						mi->menu_marked_item
							= mi->menu_top_item;
						menu_scrollbar();
					}
					else
					{
						do_beep();
					}
				}
				else
				{
					/* top item == marked item
						so bottom item := top item */
					mi->menu_top_item -= (mi->menu_item_lines-1);
					if (mi->menu_top_item < 0)
					{
						mi->menu_top_item = 0;
					}
					mi->menu_marked_item = mi->menu_top_item;
					draw_menu_sub_window();
				}
			}

			else if (ch == KEY_PGDN)
			{
				/* next page */
				if (mi->menu_top_item ==
					mi->display_menu_count - mi->menu_item_lines
					|| mi->display_menu_count < mi->menu_item_lines)
				{
					/*
						bottom item == last possible item or last possible item
						less than number of items can display
						so marked item := bottom item
					*/
					if (mi->menu_marked_item < mi->display_menu_count - 1)
					{
						unmark_mark_items(mi->menu_marked_item,
							mi->display_menu_count - 1);
						mi->menu_marked_item = mi->display_menu_count - 1;
						menu_scrollbar();
					}
					else
					{
						do_beep();
					}
				}
				else if (mi->menu_marked_item
					< mi->menu_top_item +
					(mi->menu_item_lines - 1))
				{
					/* marked item != bottom item
						so marked item := bottom item */
					unmark_mark_items(mi->menu_marked_item,
						mi->menu_top_item +
						(mi->menu_item_lines - 1));
					mi->menu_marked_item = mi->menu_top_item +
						(mi->menu_item_lines - 1);
					menu_scrollbar();
				}
				else
				{
					/* top item := marked item */
					mi->menu_top_item = mi->menu_marked_item;
					if (mi->menu_top_item
						> mi->display_menu_count - (mi->menu_item_lines))
					{
						mi->menu_top_item = mi->display_menu_count -
							(mi->menu_item_lines);
					}
					mi->menu_marked_item
						= mi->menu_top_item +
							(mi->menu_item_lines - 1);
					draw_menu_sub_window();
				}
			}

			else if (ch == KEY_RETURN || ch == cmds(CMDS_MENU_RUN))
			{
				index = find_marked_items_menu_index();
				mi->exec_buffer = 0;
				if (mi->menu_ptr[index].title == 0)
				{
					/* no menu entries or this is a category entry */
					goto no_exec;
				}
				if (uid &&
					!date_time_matches(mi->menu_ptr[index].date_time_string,0))
				{
					/* invalid date/time */
					errmsg(ER_DTOR, "", ERR_ANY);
					goto no_exec;
				}
				if (!subst_exec_params(mi->menu_ptr[index].exec_path))
				{
					goto no_exec;
				}

				if (mi->menu_cursor_inv)
					curs_set(1);
				if (mi->exec_buffer)
				{
					xsystem(mi->exec_buffer, "", TRUE);
				}
				else
				{
					xsystem(mi->menu_ptr[index].exec_path, "", TRUE);
				}
				if (mi->menu_cursor_inv)
					curs_set(0);
no_exec:
				if (mi->exec_buffer)
					FREE(mi->exec_buffer);
			}

			else if (ch == KEY_REFRESH)
			{
				/* redraw menus */
				redraw_menu_windows();
			}

			else if (ch == cmds(CMDS_MENU_RESTORE_ALL))
			{
				/* restore all branches */
				restore_branches();
			}

			else if (ch == cmds(CMDS_MENU_COLLAPSE_ALL))
			{
				/* collapse all branches */
				collapse_branches();
			}

			else if (ch == cmds(CMDS_MENU_RESTORE))
			{
				/* restore branch */
				collapse_restore_branch(FALSE);
			}

			else if (ch == cmds(CMDS_MENU_COLLAPSE))
			{
				/* collapse branch */
				collapse_restore_branch(TRUE);
			}

			else if (ch == cmds(CMDS_MENU_TOGGLE_TREE))
			{
				/* toggle tree */
				opt(show_appmenu_lines) = ! opt(show_appmenu_lines);
				draw_menu_sub_window();
			}

			else
			{
				do_beep();
			}
		}
	}

	return;
}

/*.
******************************************************************************
** FUNCTION:	menu_free_memory()
**
** DESCRIPTION: This function frees all memory associated with the menu.
**
** ENTRY:	   NONE
**
** EXIT:		NONE
**
** NOTES:	   only call this function if and when you want to free memory
**			  used by menu .. will cause reading of menu file on subsequent
**			  call to Menu_display_menu();  probably best to be called only
**			  by non-menu functions when leaving program.
**
*****************************************************************************/
void menu_free_memory(void)
{
	MENU_INFO *	mi = &gbl(menu_info);
	int i;

	if (mi->menu_count)
	{
		for (i=0; i<mi->menu_count; i++)
		{
			if (mi->menu_ptr[i].category)
				FREE(mi->menu_ptr[i].category);
			if (mi->menu_ptr[i].title)
				FREE(mi->menu_ptr[i].title);
			if (mi->menu_ptr[i].exec_path)
				FREE(mi->menu_ptr[i].exec_path);
			if (mi->menu_ptr[i].date_time_string)
				FREE(mi->menu_ptr[i].date_time_string);
		}
		FREE(mi->menu_ptr);
	}
	mi->menu_count = 0;
	mi->display_menu_count = 0;
	mi->menu_ptr = 0;
}


/*.
******************************************************************************
** FUNCTION:	Menu_display_menu()
**
** DESCRIPTION: This function display the latest version of the menu file.
**
** ENTRY:	   char *menu_file_path- menu file
**
** EXIT:		NONE
**
** NOTES:	   curr_dir, curr_file, and curr_tagged_files may all be NULL
**
*****************************************************************************/
static void Menu_display_menu(char *menu_file_path)
{
	MENU_INFO *	mi = &gbl(menu_info);
	char fullpath[MAX_PATHLEN];
	struct stat stat_buf;
	char *p;

	/*--------------------------------------------------------------------
	 * check if menu file exists
	 */
	p = os_get_path(menu_file_path, gbl(pgm_path), fullpath);
	if (p == 0 || os_stat(p, &stat_buf) != 0)
	{
		/* menu file does not exist or we can't read it */
		errmsg(ER_NOMENU, "", ERR_ANY);
		return;
	}

	/*--------------------------------------------------------------------
	 * check if our last read is older than the menu file
	 */
	if (stat_buf.st_mtime > mi->time_last_read)
	{
		menu_free_memory();
	}

	/*--------------------------------------------------------------------
	 * load menu file if not loaded yet
	 */
	if (mi->menu_ptr == 0)
	{
		mi->menu_ptr = read_menu_file(p, &mi->menu_count);
		mi->display_menu_count = mi->menu_count;
		mi->time_last_read = time((time_t *)0);
	}

	/*--------------------------------------------------------------------
	 * draw menu and handle user input
	 */
	Menu_paint_menu();
	do_user_io();

	/*--------------------------------------------------------------------
	 * turn cursor back on if it is off
	 */
	if (mi->menu_cursor_inv)
		curs_set(1);
}

/*.
******************************************************************************
** FUNCTION:	do_menu
**
** DESCRIPTION: interface routine between menu system main program
**
** ENTRY:		NONE
**
** EXIT:		NONE
**
** NOTES:		NONE
**
*****************************************************************************/
void do_menu(void)
{
	char filename[MAX_FILELEN];

	strcpy(filename, pgmi(m_pgm_program));
	fn_set_ext(filename, pgm_const(mnu_ext));

	gbl(scr_in_menu) = TRUE;
	win_clock_set(FALSE);
	Menu_display_menu(filename);
	gbl(scr_in_menu) = FALSE;
	setup_display();
	disp_cmds();
}
