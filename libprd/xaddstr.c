/*------------------------------------------------------------------------
 * routines to display strings
 */
#include "libprd.h"

int is_hilite_toggle (int c)
{
	return (c == *msgs(m_pgm_hilite));
}

void xaddch (WINDOW *window, int c)
{
	if (is_hilite_toggle(c))
	{
		if (! getstmode(window))
			wstandout(window);
		else
			wstandend(window);
	}
	else
	{
		waddch(window, c);
	}
}

void xaddstr (WINDOW *window, const char *string)
{
	unsigned char *s = (unsigned char *)string;

	for (; *s; s++)
	{
		if (*s == *msgs(m_pgm_hilite))
		{
			if (! getstmode(window))
				wstandout(window);
			else
				wstandend(window);
		}
		else
		{
			waddch(window, *s);
		}
	}
}

void xcaddstr (WINDOW *window, int code, const char *string)
{
	unsigned char *s = (unsigned char *)string;

	for (; *s; s++)
	{
		if (*s == *msgs(m_pgm_hilite))
		{
			if (! getstmode(window))
				wstandout(window);
			else
				wstandend(window);
		}
		else
		{
			wcaddch(window, *s, code);
		}
	}
}

void xiaddstr (WINDOW *window, const char *string)
{
	unsigned char *s = (unsigned char *)string;

	for (; *s; s++)
	{
		if (*s != *msgs(m_pgm_hilite))
			waddch(window, *s);
	}
}

void xciaddstr (WINDOW *window, int code, const char *string)
{
	unsigned char *s = (unsigned char *)string;

	for (; *s; s++)
	{
		if (*s != *msgs(m_pgm_hilite))
			wcaddch(window, *s, code);
	}
}

void zaddstr (WINDOW *window, const char *string)
{
	unsigned char *s = (unsigned char *)string;

	for (; *s; s++)
	{
		int c = *s;

		if (iscntrl(c))
		{
			waddch(window, '^');
			waddch(window, c+'@');
		}
		else
		{
			waddch(window, c);
		}
	}
}

char *zstring (const char *string, char *buf)
{
	unsigned char *s = (unsigned char *)string;
	char *b;

	b = buf;
	for (; *s; s++)
	{
		int c = *s;

		if (iscntrl(c))
		{
			*b++ = '^';
			*b++ = c+'A'-1;
		}
		else
		{
			*b++ = c;
		}
	}
	*b = 0;

	return (buf);
}

int fill_fw_str (char *buf, const char *str, int len)
{
	const unsigned char *s = (const unsigned char *)str;
	char *b = buf;
	int i;
	int l;

	l = strlen(str);
	for (i=0; *s; i++, s++)
	{
		int c = *s;

		if (i == len-1)
		{
			*b++ = pgm_const(too_long_char);
			i++;
			break;
		}

		if (iscntrl(c))
		{
			if (l < len)
			{
				l++;
				i++;
				*b++ = '^';
				*b++ = c+'@';
			}
			else
			{
				*b++ = pgm_const(unprintable_char);
			}
		}
		else
		{
			*b++ = c;
		}
	}

	for (; i<len; i++)
		*b++ = ' ';
	*b = 0;

	return (len);
}

void disp_fw_str (WINDOW *win, const char *str, int len)
{
	const unsigned char *s = (const unsigned char *)str;
	int i;
	int l;

	l = strlen(str);
	for (i=0; *s; i++, s++)
	{
		int c = *s;

		if (i == len-1)
		{
			waddch(win, pgm_const(too_long_char));
			i++;
			break;
		}

		if (iscntrl(c))
		{
			if (l < len)
			{
				l++;
				i++;
				waddch(win, '^');
				waddch(win, c+'@');
			}
			else
			{
				waddch(win, pgm_const(unprintable_char));
			}
		}
		else
		{
			waddch(win, c);
		}
	}

	for (; i<len; i++)
		waddch(win, ' ');
}

void disp_fw_str_m (WINDOW *win, const char *str, int len, int code, int pad)
{
	const unsigned char *s = (const unsigned char *)str;
	int i;
	int l;

	l = strlen(str);
	for (i=0; *s; i++, s++)
	{
		int c = *s;

		if (i == len-1)
		{
			wcaddch(win, pgm_const(too_long_char), code);
			i++;
			break;
		}

		if (iscntrl(c))
		{
			if (l < len)
			{
				l++;
				i++;
				wcaddch(win, '^', code);
				wcaddch(win, c+'@', code);
			}
			else
			{
				wcaddch(win, pgm_const(unprintable_char), code);
			}
		}
		else
		{
			wcaddch(win, c, code);
		}
	}

	for (; i<len; i++)
	{
		if (pad)
			wcaddch(win, ' ', code);
		else
			waddch(win, ' ');
	}
}

/*
 * length of display of a string (sans any hilite toggles)
 */

int display_len (const char *s)
{
	register int len = 0;

	for (; *s; s++)
	{
		if (*s != *msgs(m_pgm_hilite))
			len++;
	}

	return (len);
}
