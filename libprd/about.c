/*------------------------------------------------------------------------
 * about box
 */
#include "libprd.h"

/*------------------------------------------------------------------------
 * about box (via help key)
 */
typedef char *	LINE_RTN	(char *line);

struct about
{
	int				pmsg;
	int				mmsg;
	LINE_RTN *		lrtn;
	const char *	text;
};
typedef struct about ABOUT;

static const ABOUT about[] =
{
	{ m_pgm_package,	-1,					0,
		0												  },
	{ -1,				-1,					ver_line,
		0												  },
	{ -1,				m_about_formerly,	0,
		0												  },
	{ m_pgm_slogan,		-1,					0,
		0												  },
	{ -1,				-1,					0,
		""												  },
	{ -1,				m_about_produced,	0,
		"~                     Rob Juergens             " },
	{ -1,				m_about_porting,	0,
		"~                     Rick Juergens            " },
	{ -1,				m_about_thanks,		0,
		"~                     Becky Juergens           " },
	{ -1,				-1,					0,
		"                      Ray Tayek                " },
	{ -1,				-1,					0,
		"                      Mathias Winkler          " },
	{ -1,				-1,					0,
		""												  },
	{ m_pgm_emladdr,	m_about_comments,	0,
		"~                     ^                        " },
	{ m_pgm_wwwaddr,	m_about_homepage,	0,
		"~                     ^                        " },
	{ -1,				-1,					0,
		""												  },
	{ -1,				m_about_legalbs1,	0,
		0												  },
	{ -1,				m_about_legalbs2,	0,
		0												  },
	{ m_pgm_copyrte,		-1,					0,
		0												  }
};

#define NUM_ABOUTS	sizeof(about) / sizeof(*about)

static const char *about_line (int i, char *buf)
{
	const ABOUT *	l = about + i;

	if (l->lrtn != 0)
	{
		return (l->lrtn)(buf);
	}
	else if (l->pmsg == -1 && l->mmsg == -1 && l->text != 0)
	{
		return (l->text);
	}
	else if (l->pmsg >= 0 && l->text == 0)
	{
		return (pgmi(l->pmsg));
	}
	else if (l->mmsg >= 0 && l->text == 0)
	{
		return (msgs(l->mmsg));
	}
	else
	{
		char *p;
		char *s;

		strcpy(buf, l->text);

		p = strchr(buf, '^');
		if (p != 0)
		{
			for (s = pgmi(l->pmsg); *s; s++)
				*p++ = *s;
		}

		p = strchr(buf, '~');
		if (p != 0)
		{
			for (s = msgs(l->mmsg); *s; s++)
				*p++ = *s;
		}

		return (buf);
	}
}

static void lines_in_box (attr_t attr)
{
	WINDOW *		msg_box_win;
	char			line_buf[128];
	const char *	line;
	int				l;
	int				i;
	int				len;
	int				num;
	int				y, x;
	int				c;
	int				old_cur;

	/*--------------------------------------------------------------------
	 * get number of lines & length of longest line
	 */
	num = 0;
	len = 0;
	for (i=0; i < NUM_ABOUTS; i++)
	{
		line = about_line(i, line_buf);
		if (line == 0)
			break;

		num++;
		l = strlen(line);
		if (l > len)
			len = l;
	}

	len += 2;		/* for space before and after	*/
	num += 2;		/* for "press any key" line		*/

	/*--------------------------------------------------------------------
	 * create the window to hold the list
	 */
	y = (getmaxy(stdscr) - (num + 4)) / 2;
	x = (getmaxx(stdscr) - (len + 4)) / 2;

	if (y == 0)
		y = 1;

	msg_box_win = newwin(num+4, len+4, y, x);
	if (msg_box_win == 0)
		return;

	wattrset(msg_box_win, attr);
	werase(msg_box_win);

	/*--------------------------------------------------------------------
	 * draw box around windo
	 */
	wbox(msg_box_win, B_DVDH);

	/*--------------------------------------------------------------------
	 * display all lines
	 */
	for (i=0; i < NUM_ABOUTS; i++)
	{
		line = about_line(i, line_buf);
		if (line == 0)
			break;

		l = strlen(line);
		wcenter(msg_box_win, line, i+2);
	}
	wcenter(msg_box_win, msgs(m_credits_anykey), i+3);
	wrefresh(msg_box_win);

	/*--------------------------------------------------------------------
	 * wait for a keypress
	 */
	flushinp();
	old_cur = curs_set(0);

	wmove(stdscr, 0, 0);
	wrefresh(stdscr);

	while (TRUE)
	{
		c = xgetch(msg_box_win);
		if (c != KEY_MOUSE && c != ERR)
			break;

		if (mouse_get_event(msg_box_win) == MOUSE_LBD)
			break;
	}

	curs_set(old_cur);

	/*--------------------------------------------------------------------
	 * delete the window
	 */
	delwin(msg_box_win);
}

int do_about (int key, void *data)
{
	int sx, sy;

	/*--------------------------------------------------------------------
	 * don't show box if box is currently showing
	 */
	if (gbl(scr_in_about_box))
		return (1);
	gbl(scr_in_about_box) = TRUE;

	/*--------------------------------------------------------------------
	 * cache current cursor position
	 */
	sy = getcury(curscr);
	sx = getcurx(curscr);

	/*--------------------------------------------------------------------
	 * display our about box
	 */
	lines_in_box(gbl(scr_error_attr));

	/*--------------------------------------------------------------------
	 * re-display screen as it was
	 */
	touchwin(stdscr);
	wrefresh(stdscr);

	help_redisplay();

	/*--------------------------------------------------------------------
	 * restore current cursor position
	 */
	mvcur(-1, -1, sy, sx);
	doupdate();

	gbl(scr_in_about_box) = FALSE;
	return (0);
}
