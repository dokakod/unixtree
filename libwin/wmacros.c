/*------------------------------------------------------------------------
 * Here we implement all macros as functions
 */
#define COMPILING_MACROS	1
#include "wincommon.h"

/*------------------------------------------------------------------------
 * initialization functions (ecurs_initscr)
 */
int endwin (void)
{
	return endscr(FALSE);
}

/*------------------------------------------------------------------------
 * window functions (ecurs_window)
 */
int syncok (WINDOW *w, int bf)
{
	if (w == 0)
		return (ERR);

	w->_sync = bf ? TRUE : FALSE;

	return (OK);
}

int getsync (WINDOW *w)
{
	if (w == 0)
		return (ERR);

	return (w->_sync);
}

/*------------------------------------------------------------------------
 * window background functions (ecurs_bkgd)
 */
int bkgd (chtype ch)
{
	return wbkgd(stdscr, ch);
}

void bkgdset (chtype ch)
{
	wbkgdset(stdscr, ch);
}

/*------------------------------------------------------------------------
 * cursor move functions (ecurs_move)
 */
int move (int y, int x)
{
	return wmove(stdscr, y, x);
}

/*------------------------------------------------------------------------
 * window clear functions (ecurs_clear)
 */
int werase (WINDOW *w)
{
	if (wmove(w, 0, 0) == ERR)
		return (ERR);

	return  wclrtobot(w);
}

int wclear (WINDOW *w)
{
	if (clearok(w, TRUE) == ERR)
		return (ERR);

	return  werase(w);
}

int clear (void)
{
	return wclear(stdscr);
}

int clrtobot (void)
{
	return wclrtobot(stdscr);
}

int clrtoeol (void)
{
	return wclrtoeol(stdscr);
}

int erase (void)
{
	return werase(stdscr);
}

/*------------------------------------------------------------------------
 * alternate-char functions (ecurs_acs)
 */
int box_chr (int c, int type)
{
	return wbox_chr(stdscr, c, type);
}

int blk_chr (int b, int c)
{
	return wblk_chr(stdscr, b, c);
}

int blk_str (int y, int x, const char *s, int t)
{
	return wblk_str(stdscr, y, x, s, t);
}

int blk_ltr (int y, int x, int c, int t)
{
	return wblk_ltr(stdscr, y, x, c, t);
}

/*------------------------------------------------------------------------
 * box/line functions (ecurs_border)
 */
int box (WINDOW *w, chtype v, chtype h)
{
	return wborder(w, v, v, h, h, 0, 0, 0, 0);
}

int border (chtype ls, chtype rs, chtype ts, chtype bs,
	chtype tl, chtype tr, chtype bl, chtype br)
{
	return wborder(stdscr, ls, rs, ts, bs, tl, tr, bl, br);
}

int hline (chtype c, int n)
{
	return whline(stdscr, c, n);
}

int vline (chtype c, int n)
{
	return wvline(stdscr, c, n);
}

int mvwhline (WINDOW *w, int y, int x, chtype c, int n)
{
	if (wmove(w, y, x) == ERR)
		return (ERR);

	return whline(w, c, n);
}

int mvhline (int y, int x, chtype c, int n)
{
	return mvwhline(stdscr, y, x, c, n);
}

int mvwvline (WINDOW *w, int y, int x, chtype c, int n)
{
	if (wmove(w, y, x) == ERR)
		return (ERR);

	return wvline(w, c, n);
}

int mvvline (int y, int x, chtype c, int n)
{
	return mvwvline(stdscr, y, x, c, n);
}

/*------------------------------------------------------------------------
 * output a char and attributes (ecurs_addch)
 */
int addch (chtype ch)
{
	return waddch(stdscr, ch);
}

int echochar (chtype ch)
{
	return wechochar(stdscr, ch);
}

int mvwaddch (WINDOW *w, int y, int x, chtype ch)
{
	if (wmove(w, y, x) == ERR)
		return (ERR);

	return waddch(w, ch);
}

int mvaddch (int y, int x, chtype ch)
{
	return mvwaddch(stdscr, y, x, ch);
}

/*------------------------------------------------------------------------
 * output a char, attributes, and code (ecurs_caddch)
 */
int caddch (chtype ch, int code)
{
	return wcaddch(stdscr, ch, code);
}

int mvwcaddch (WINDOW *w, int y, int x, chtype ch, int c)
{
	if (wmove(w, y, x) == ERR)
		return (ERR);

	return wcaddch(w, ch, c);
}

int mvcaddch (int y, int x, chtype ch, int c)
{
	return mvwcaddch(stdscr, y, x, ch, c);
}

/*------------------------------------------------------------------------
 * output a char string (ecurs_addstr)
 */
int addnstr (const char *s, int n)
{
	return waddnstr(stdscr, s, n);
}

int addstr (const char *s)
{
	return waddstr(stdscr, s);
}

int addtstr (const char *s, int c)
{
	return waddtstr(stdscr, s, c);
}

int center (const char *s, int y)
{
	return wcenter(stdscr, s, y);
}

int mvwaddstr (WINDOW *w, int y, int x, const char *s)
{
	if (wmove(w, y, x) == ERR)
		return (ERR);

	return waddstr(w, s);
}

int mvaddstr (int y, int x, const char *s)
{
	return mvwaddstr(stdscr, y, x, s);
}

int mvwaddtstr (WINDOW *w, int y, int x, const char *s, int c)
{
	if (wmove(w, y, x) == ERR)
		return (ERR);

	return waddtstr(w, s, c);
}

int mvaddtstr (int y, int x, const char *s, int c)
{
	return mvwaddtstr(stdscr, y, x, s, c);
}

int mvwaddnstr (WINDOW *w, int y, int x, const char *s, int n)
{
	if (wmove(w, y, x) == ERR)
		return (ERR);

	return waddnstr(w, s, n);
}

int mvaddnstr (int y, int x, const char *s, int n)
{
	return mvwaddnstr(stdscr, y, x, s, n);
}

/*------------------------------------------------------------------------
 * output a char string and code (ecurs_caddstr)
 */
int caddnstr (const char *s, int n, int code)
{
	return wcaddnstr(stdscr, s, n, code);
}

int caddstr (const char *s, int code)
{
	return wcaddstr(stdscr, s, code);
}

int ccenter (const char *s, int y, int code)
{
	return wccenter(stdscr, s, y, code);
}

int mvwcaddstr (WINDOW *w, int y, int x, const char *s, int c)
{
	if (wmove(w, y, x) == ERR)
		return (ERR);

	return wcaddstr(w, s, c);
}

int mvcaddstr (int y, int x, const char *s, int c)
{
	return mvwcaddstr(stdscr, y, x, s, c);
}

int mvwcaddnstr (WINDOW *w, int y, int x, const char *s, int n, int c)
{
	if (wmove(w, y, x) == ERR)
		return (ERR);

	return wcaddnstr(w, s, n, c);
}

int mvcaddnstr (int y, int x, const char *s, int n, int c)
{
	return mvwcaddnstr(stdscr, y, x, s, n, c);
}

/*------------------------------------------------------------------------
 * output a chtype string (ecurs_addchstr)
 */
int waddchstr (WINDOW *w, const chtype *s)
{
	return waddchnstr(w, s, -1);
}

int addchstr (const chtype *s)
{
	return waddchstr(stdscr, s);
}

int addchnstr (const chtype *s, int n)
{
	return waddchnstr(stdscr, s, n);
}

int mvwaddchstr (WINDOW *w, int y, int x, const chtype *s)
{
	if (wmove(w, y, x) == ERR)
		return (ERR);

	return waddchstr(w, s);
}

int mvaddchstr (int y, int x, const chtype *s)
{
	return mvwaddchstr(stdscr, y, x, s);
}

int mvwaddchnstr (WINDOW *w, int y, int x, const chtype *s, int n)
{
	if (wmove(w, y, x) == ERR)
		return (ERR);

	return waddchnstr(w, s, n);
}

int mvaddchnstr (int y, int x, const chtype *s, int n)
{
	return mvwaddchnstr(stdscr, y, x, s, n);
}

/*------------------------------------------------------------------------
 * input a char (ecurs_getch)
 */
int chkkbd (void)
{
	return wchkkbd(stdscr);
}

int getch (void)
{
	return wgetch(stdscr);
}

int mgetch (void)
{
	return wmgetch(stdscr);
}

int ungetch (int ch)
{
	return wungetch(stdscr, ch);
}

int mvwgetch (WINDOW *w, int y, int x)
{
	if (wmove(w, y, x) == ERR)
		return (ERR);

	return wgetch(w);
}

int mvgetch (int y, int x)
{
	return mvwgetch(stdscr, y, x);
}

/*------------------------------------------------------------------------
 * input a char string (ecurs_getstr)
 */
int getnstr (char *s, int n)
{
	return wgetnstr(stdscr, s, n);
}

int getstr (char *s)
{
	return wgetstr(stdscr, s);
}

int mvwgetstr (WINDOW *w, int y, int x, char *s)
{
	if (wmove(w, y, x) == ERR)
		return (ERR);

	return wgetstr(w, s);
}

int mvgetstr (int y, int x, char *s)
{
	return mvwgetstr(stdscr, y, x, s);
}

/*------------------------------------------------------------------------
 * extract a chtype from a window (ecurs_inch)
 */
chtype wchat (WINDOW *w, int y, int x)
{
	if (w == 0)
		return (chtype)(ERR);

	if (y < 0 || y >= getmaxy(w) ||
	    x < 0 || x >= getmaxx(w))
	{
		return (chtype)(ERR);
	}

	return (w->_y[(y)][(x)]);
}

chtype chat (int y, int x)
{
	return wchat(stdscr, y, x);
}

chtype winch (WINDOW *w)
{
	return wchat(w, getcury(w), getcurx(w));
}

chtype inch (void)
{
	return winch(stdscr);
}

chtype mvwinch (WINDOW *w, int y, int x)
{
	if (wmove(w, y, x) == ERR)
		return (chtype)(ERR);

	return winch(w);
}

chtype mvinch (int y, int x)
{
	return mvwinch(stdscr, y, x);
}

/*------------------------------------------------------------------------
 * store a chtype into a window (ecurs_outch)
 */
int woutch (WINDOW *w, int y, int x, chtype c)
{
	if (w == 0)
		return (ERR);

	if (y < 0 || y >= getmaxy(w) ||
	    x < 0 || x >= getmaxx(w))
	{
		return (ERR);
	}

	w->_y[(y)][(x)] = c;

	return (OK);
}

int outch (int y, int x, chtype c)
{
	return woutch(stdscr, y, x, c);
}

int wouch (WINDOW *w, chtype c)
{
	return woutch(w, getcury(w), getcurx(w), c);
}

int ouch (chtype c)
{
	return wouch(stdscr, c);
}

int mvwouch (WINDOW *w, int y, int x, chtype c)
{
	if (wmove(w, y, x) == ERR)
		return (ERR);

	return wouch(w, c);
}

int mvouch (int y, int x, chtype c)
{
	return mvwouch(stdscr, y, x, c);
}

/*------------------------------------------------------------------------
 * extract a char string from a window (ecurs_instr)
 */
int winstr (WINDOW *w, char *s)
{
	return winnstr(w, s, -1);
}

int innstr (char *s, int n)
{
	return winnstr(stdscr, s, n);
}

int instr (char *s)
{
	return winstr(stdscr, s);
}

int mvwinnstr (WINDOW *w, int y, int x, char *s, int n)
{
	if (wmove(w, y, x) == ERR)
		return (ERR);

	return winnstr(w, s, n);
}

int mvinnstr (int y, int x, char *s, int n)
{
	return mvwinnstr(stdscr, y, x, s, n);
}

int mvwinstr (WINDOW *w, int y, int x, char *s)
{
	if (wmove(w, y, x) == ERR)
		return (ERR);

	return winstr(w, s);
}

int mvinstr (int y, int x, char *s)
{
	return mvwinstr(stdscr, y, x, s);
}

/*------------------------------------------------------------------------
 * extract a chtype string from a window (ecurs_inchstr)
 */
int winchstr (WINDOW *w, chtype *s)
{
	return winchnstr(w, s, -1);
}

int inchnstr (chtype *s, int n)
{
	return winchnstr(stdscr, s, n);
}

int inchstr (chtype *s)
{
	return winchstr(stdscr, s);
}

int mvwinchnstr (WINDOW *w, int y, int x, chtype *s, int n)
{
	if (wmove(w, y, x) == ERR)
		return (ERR);

	return winchnstr(w, s, n);
}

int mvinchnstr (int y, int x, chtype *s, int n)
{
	return mvwinchnstr(stdscr, y, x, s, n);
}

int mvwinchstr (WINDOW *w, int y, int x, chtype *s)
{
	if (wmove(w, y, x) == ERR)
		return (ERR);

	return winchstr(w, s);
}

int mvinchstr (int y, int x, chtype *s)
{
	return mvwinchstr(stdscr, y, x, s);
}

/*------------------------------------------------------------------------
 * insert a chtype into a window (ecurs_insch)
 */
int insch (chtype ch)
{
	return winsch(stdscr, ch);
}

int mvwinsch (WINDOW *w, int y, int x, chtype ch)
{
	if (wmove(w, y, x) == ERR)
		return (ERR);

	return winsch(w, ch);
}

int mvinsch (int y, int x, chtype ch)
{
	return mvwinsch(stdscr, y, x, ch);
}

/*------------------------------------------------------------------------
 * insert a char string into a window (ecurs_insstr)
 */
int winsstr (WINDOW *w, const char *s)
{
	return winsnstr(w, s, -1);
}

int insnstr (const char *s, int n)
{
	return winsnstr(stdscr, s, n);
}

int insstr (const char *s)
{
	return winsstr(stdscr, s);
}

int mvwinsnstr (WINDOW *w, int y, int x, const char *s, int n)
{
	if (wmove(w, y, x) == ERR)
		return (ERR);

	return winsnstr(w, s, n);
}

int mvinsnstr (int y, int x, const char *s, int n)
{
	return mvwinsnstr(stdscr, y, x, s, n);
}

int mvwinsstr (WINDOW *w, int y, int x, const char *s)
{
	if (wmove(w, y, x) == ERR)
		return (ERR);

	return winsstr(w, s);
}

int mvinsstr (int y, int x, const char *s)
{
	return mvwinsstr(stdscr, y, x, s);
}

/*------------------------------------------------------------------------
 * delete a char from a window (ecurs_delch)
 */
int delch (void)
{
	return wdelch(stdscr);
}

int mvwdelch (WINDOW *w, int y, int x)
{
	if (wmove(w, y, x) == ERR)
		return (ERR);

	return wdelch(w);
}

int mvdelch (int y, int x)
{
	return mvwdelch(stdscr, y, x);
}

/*------------------------------------------------------------------------
 * insert/delete lines in a window (ecurs_deleteln)
 */
int deleteln (void)
{
	return wdeleteln(stdscr);
}

int insdelln (int n)
{
	return winsdelln(stdscr, n);
}

int insertln (void)
{
	return winsertln(stdscr);
}

/*------------------------------------------------------------------------
 * manipulate overlapped windows (ecurs_overlay)
 */
int overlay (WINDOW *s, WINDOW *d)
{
	return overlap(s, d, TRUE);
}

int overwrite (WINDOW *s, WINDOW *d)
{
	return overlap(s, d, FALSE);
}

/*------------------------------------------------------------------------
 * scroll functions (ecurs_scroll)
 */
int scroll (WINDOW *w)
{
	return wscrl(w, 1);
}

int scrl (int n)
{
	return wscrl(stdscr, n);
}

/*------------------------------------------------------------------------
 * refresh windows/lines (ecurs_refresh)
 */
int redrawwin (WINDOW *w)
{
	return wredrawln(w, 0, getmaxy(w));
}

int noutrefresh (void)
{
	return wnoutrefresh(stdscr);
}

int	noutrefreshln (int y)
{
	return wnoutrefreshln(stdscr, y);
}

int refresh (void)
{
	return wrefresh(stdscr);
}

int refreshln (int y)
{
	return wrefreshln(stdscr, y);
}

/*------------------------------------------------------------------------
 * refresh control functions (ecurs_touch)
 */
int touchline (WINDOW *w, int y, int n)
{
	return wtouchln(w, y, n, TRUE);
}

int touchwin (WINDOW *w)
{
	return wtouchln(w, 0, getmaxy(w), TRUE);
}

int untouchwin (WINDOW *w)
{
	return wtouchln(w, 0, getmaxy(w), FALSE);
}

/*------------------------------------------------------------------------
 * input options (ecurs_inopts)
 */
int meta (WINDOW *w, int bf)
{
	return (bf ? metaon()  : metaoff());
}

int intrflush (WINDOW *w, int bf)
{
	return (bf ? qiflush() : noqiflush());
}

int keypad (WINDOW *w, int bf)
{
	if (w == 0)
		return (ERR);

	w->_use_keypad = bf ? TRUE : FALSE;

	return (OK);
}

int getkeypad (WINDOW *w)
{
	if (w == 0)
		return (ERR);

	return (w->_use_keypad);
}

int getdelay (WINDOW *w)
{
	if (w == 0)
		return (ERR);

	return (w->_delay);
}

int setdelay (WINDOW *w, int d)
{
	if (w == 0)
		return (ERR);

	w->_delay = d;

	return (OK);
}

int nodelay (WINDOW *w, int bf)
{
	if (w == 0)
		return (ERR);

	w->_delay = bf ? 0 : -1;

	return (OK);
}

int wtimeout (WINDOW *w, int t)
{
	return setdelay(w, t);
}

int notimeout (WINDOW *w, int bf)
{
	if (w == 0)
		return (ERR);

	w->_notimeout = bf ? TRUE : FALSE;

	return (OK);
}

int getnotimeout (WINDOW *w)
{
	if (w == 0)
		return (ERR);

	return (w->_notimeout);
}

int timeout (int t)
{
	return wtimeout(stdscr, t);
}

/*------------------------------------------------------------------------
 * output options (ecurs_outopts)
 */
int clearok (WINDOW *w, int bf)
{
	if (w == 0)
		return (ERR);

	w->_clear = bf ? TRUE : FALSE;

	return (OK);
}

int leaveok (WINDOW *w, int bf)
{
	if (w == 0)
		return (ERR);

	w->_leaveit = bf ? TRUE : FALSE;

	return (OK);
}

int scrollok (WINDOW *w, int bf)
{
	if (w == 0)
		return (ERR);

	w->_scroll = bf ? TRUE : FALSE;

	return (OK);
}

int immedok (WINDOW *w, int bf)
{
	if (w == 0)
		return (ERR);

	w->_immed = bf ? TRUE : FALSE;

	return (OK);
}

int idcok (WINDOW *w, int bf)
{
	if (w == 0)
		return (ERR);

	w->_use_idc = bf ? TRUE : FALSE;

	return (OK);
}

int idlok (WINDOW *w, int bf)
{
	if (w == 0)
		return (ERR);

	w->_use_idl = bf ? TRUE : FALSE;

	return (OK);
}

int getclear (WINDOW *w)
{
	if (w == 0)
		return (ERR);

	return (w->_clear);
}

int getleave (WINDOW *w)
{
	if (w == 0)
		return (ERR);

	return (w->_leaveit);
}

int getscroll (WINDOW *w)
{
	if (w == 0)
		return (ERR);

	return (w->_scroll);
}

int getimmed (WINDOW *w)
{
	if (w == 0)
		return (ERR);

	return (w->_immed);
}

int getidc (WINDOW *w)
{
	if (w == 0)
		return (ERR);

	return (w->_use_idc);
}

int getidl (WINDOW *w)
{
	if (w == 0)
		return (ERR);

	return (w->_use_idl);
}

int setscrreg (int t, int b)
{
	return wsetscrreg(stdscr, t, b);
}

/*------------------------------------------------------------------------
 * set & get window attributes (ecurs_attr)
 */
attr_t wattrget (WINDOW *w)
{
	if (w == 0)
		return (chtype)(ERR);

	return (w->_attrs);
}

int wattrset (WINDOW *w, attr_t a)
{
	if (w == 0)
		return (ERR);

	w->_attrs = A_GETATTR(a);

	return (OK);
}

attr_t wstandget (WINDOW *w)
{
	if (w == 0)
		return (chtype)(ERR);

	return (w->_attro);
}

int wstandset (WINDOW *w, attr_t a)
{
	if (w == 0)
		return (ERR);

	w->_attro = A_GETATTR(a);

	return (OK);
}

int wstandout (WINDOW *w)
{
	if (w == 0)
		return (ERR);

	w->_stmode = TRUE;

	return (OK);
}

int wstandend (WINDOW *w)
{
	if (w == 0)
		return (ERR);

	w->_stmode = FALSE;

	return (OK);
}

int attroff (attr_t a)
{
	return wattroff(stdscr, a);
}

int attron (attr_t a)
{
	return wattron(stdscr, a);
}

attr_t attrget (void)
{
	return wattrget(stdscr);
}

int attrset (attr_t a)
{
	return wattrset(stdscr, a);
}

attr_t standget (void)
{
	return wstandget(stdscr);
}

int standset (attr_t a)
{
	return wstandset(stdscr, a);
}

int standout (void)
{
	return wstandout(stdscr);
}

int standend (void)
{
	return wstandend(stdscr);
}

attr_t getattrs (WINDOW *w)
{
	if (w == 0)
		return (chtype)(ERR);

	return (w->_stmode ? w->_attro : w->_attrs);
}

chtype getbkgd (WINDOW *w)
{
	if (w == 0)
		return (chtype)(ERR);

	return (w->_bkgd);
}

int getcode (WINDOW *w)
{
	if (w == 0)
		return (ERR);

	return A_GETCODE(w->_code);
}

int getstmode (WINDOW *w)
{
	if (w == 0)
		return (ERR);

	return (w->_stmode);
}

int setbkgd (WINDOW *w, chtype a)
{
	if (w == 0)
		return (ERR);

	w->_bkgd = a;

	return (OK);
}

int setcode (WINDOW *w, int c)
{
	if (w == 0)
		return (ERR);

	w->_code = A_SETCODE(c);

	return (OK);
}

int setstmode (WINDOW *w, int b)
{
	if (w == 0)
		return (ERR);

	w->_stmode = b;

	return (OK);
}

int setcolor (WINDOW *w, int f, int b)
{
	if (w == 0)
		return (ERR);

	w->_attrs = A_CLR(f, b);

	return (OK);
}

/*------------------------------------------------------------------------
 * get/set coordinates in a window structure (ecurs_getyx)
 */
int getcury (WINDOW *w)
{
	if (w == 0)
		return (ERR);

	return (w->_cury);
}

int getcurx (WINDOW *w)
{
	if (w == 0)
		return (ERR);

	return (w->_curx);
}

int getbegy (WINDOW *w)
{
	if (w == 0)
		return (ERR);

	return (w->_begy);
}

int getbegx (WINDOW *w)
{
	if (w == 0)
		return (ERR);

	return (w->_begx);
}

int getmaxy (WINDOW *w)
{
	if (w == 0)
		return (ERR);

	return (w->_maxy);
}

int getmaxx (WINDOW *w)
{
	if (w == 0)
		return (ERR);

	return (w->_maxx);
}

int getpary (WINDOW *w)
{
	if (w == 0)
		return (ERR);

	return (w->_pary);
}

int getparx (WINDOW *w)
{
	if (w == 0)
		return (ERR);

	return (w->_parx);
}

int setcury (WINDOW *w, int y)
{
	if (w == 0)
		return (ERR);

	if (y < 0 || y >= getmaxy(w))
		return (ERR);

	w->_cury = y;

	return (OK);
}

int setcurx (WINDOW *w, int x)
{
	if (w == 0)
		return (ERR);

	if (x < 0 || x >= getmaxx(w))
		return (ERR);

	w->_curx = x;

	return (OK);
}

int setbegy (WINDOW *w, int y)
{
	if (w == 0)
		return (ERR);

	if (y < 0 || y >= getmaxy(w))
		return (ERR);

	w->_begy = y;

	return (OK);
}

int setbegx (WINDOW *w, int x)
{
	if (w == 0)
		return (ERR);

	if (x < 0 || x >= getmaxx(w))
		return (ERR);

	w->_begx = x;

	return (OK);
}

int setmaxy (WINDOW *w, int y)
{
	if (w == 0)
		return (ERR);

	if (y < 0)
		return (ERR);

	w->_maxy = y;

	return (OK);
}

int setmaxx (WINDOW *w, int x)
{
	if (w == 0)
		return (ERR);

	if (x < 0)
		return (ERR);

	w->_maxx = x;

	return (OK);
}

int setpary (WINDOW *w, int y)
{
	if (w == 0)
		return (ERR);

	if (y < 0)
		return (ERR);

	w->_pary = y;

	return (OK);
}

int setparx (WINDOW *w, int x)
{
	if (w == 0)
		return (ERR);

	if (x < 0)
		return (ERR);

	w->_parx = x;

	return (OK);
}

int setyx (WINDOW *w, int y, int x)
{
	return setcuryx(w, y, x);
}

int setcuryx (WINDOW *w, int y, int x)
{
	if (w == 0)
		return (ERR);

	if (y < 0 || y >= getmaxy(w))
		return (ERR);
	if (x < 0 || x >= getmaxx(w))
		return (ERR);

	w->_cury = y;
	w->_curx = x;

	return (OK);
}

int setbegyx (WINDOW *w, int y, int x)
{
	if (w == 0)
		return (ERR);

	if (y < 0 || y >= getmaxy(w))
		return (ERR);
	if (x < 0 || x >= getmaxx(w))
		return (ERR);

	w->_begy = y;
	w->_begx = x;

	return (OK);
}

int setmaxyx (WINDOW *w, int y, int x)
{
	if (w == 0)
		return (ERR);

	if (y < 0)
		return (ERR);
	if (x < 0)
		return (ERR);

	w->_maxy = y;
	w->_maxx = x;

	return (OK);
}

int setparyx (WINDOW *w, int y, int x)
{
	if (w == 0)
		return (ERR);

	if (y < 0)
		return (ERR);
	if (x < 0)
		return (ERR);

	w->_pary = y;
	w->_parx = x;

	return (OK);
}
