/*------------------------------------------------------------------------
 * generalized error handler
 */
#include "libprd.h"

int errsys (int str1)
{
	char err_buf[128];

	strcpy(err_buf, " (");
	strcat(err_buf, strerror(errno));
	strcat(err_buf, ")");

	return errmsg(str1, err_buf, ERR_ANY);
}

int errmsg (int str1, const char *str2, int mode)
{
	return (err_message(errs(str1), str2, mode));
}

int errmsgi (int str1, const char *str2, int mode)
{
	return (err_message(str2, errs(str1), mode));
}

int err_message (const char *str1, const char *str2, int mode)
{
	int c;
	int save_delay;

	do_beep();

	werase(gbl(win_error));
	wmove(gbl(win_error), 0, 0);
	xaddstr(gbl(win_error), str1);
	waddstr(gbl(win_error), str2);
	wrefresh(gbl(win_error));

	save_delay = getdelay(stdscr);
	setdelay(stdscr, -1);

	switch (mode)
	{
	case ERR_YESNO:
		c = wyesno(gbl(win_error));
		break;

	case ERR_CHAR:
		while (TRUE)
		{
			c = xgetch(gbl(win_error));
			if (c == ERR)
				continue;
			if (c != KEY_MOUSE)
				break;
		}
		break;

	case ERR_ANY:
		c = wanykey(gbl(win_error));
		break;

	default:
		c = -1;
		break;
	}

	setdelay(stdscr, save_delay);
	touchwin(gbl(win_message));
	wrefresh(gbl(win_message));

	return (c);
}
