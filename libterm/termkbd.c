/*------------------------------------------------------------------------
 * keyboard input routines
 */
#include "termcommon.h"

#if V_UNIX
/*------------------------------------------------------------------------
 * UNIX: no critical section handling
 */
#define kbd_cs_enter(pCS)	/* nothing */
#define kbd_cs_leave(pCS)	/* nothing */

/*------------------------------------------------------------------------
 * BSD systems apparently don't handle termio correctly...
 */
#  if defined(__OpenBSD__) || defined(__NetBSD__) || defined(__FreeBSD__)
#    define NO_VMIN_VTIME	1
#  endif

/*------------------------------------------------------------------------
 * UNIX: alarm signal handler (only used if not using VMIN/VTIME)
 */
#  if NO_VMIN_VTIME
#  include <setjmp.h>

static void kbd_timeout (time_t t, void *data)
{
	jmp_buf *	j = (jmp_buf *)data;
	longjmp(*j, 1);
}
#  endif
#endif

#if V_WINDOWS
/*------------------------------------------------------------------------
 * NT: use critical sections
 */
#define kbd_cs_enter(pCS)	EnterCriticalSection(pCS)
#define kbd_cs_leave(pCS)	LeaveCriticalSection(pCS)

/*------------------------------------------------------------------------
 * NT: convert a KEY_EVENT record to a KEY_* value
 */
static DWORD nt_translate_key (KEY_EVENT_RECORD *ker)
{
	DWORD	key;

	if (ker->uChar.AsciiChar != 0)
	{
		key = ker->uChar.AsciiChar;
	}
	else if (ker->wVirtualKeyCode != 0)
	{
		switch (ker->wVirtualKeyCode)
		{
		case VK_PRIOR:		key = KEY_PGUP;			break;
		case VK_NEXT:		key = KEY_PGDN;			break;
		case VK_END:		key = KEY_END;			break;
		case VK_HOME:		key = KEY_HOME;			break;
		case VK_LEFT:		key = KEY_LEFT;			break;
		case VK_UP:			key = KEY_UP;			break;
		case VK_RIGHT:		key = KEY_RIGHT;		break;
		case VK_DOWN:		key = KEY_DOWN;			break;
		case VK_INSERT:		key = KEY_INSERT;		break;
		case VK_DELETE:		key = KEY_DELETE;		break;

		case VK_NUMPAD0:	key = KEY_KP0;			break;
		case VK_NUMPAD1:	key = KEY_KP1;			break;
		case VK_NUMPAD2:	key = KEY_KP2;			break;
		case VK_NUMPAD3:	key = KEY_KP3;			break;
		case VK_NUMPAD4:	key = KEY_KP4;			break;
		case VK_NUMPAD5:	key = KEY_KP5;			break;
		case VK_NUMPAD6:	key = KEY_KP6;			break;
		case VK_NUMPAD7:	key = KEY_KP7;			break;
		case VK_NUMPAD8:	key = KEY_KP8;			break;
		case VK_NUMPAD9:	key = KEY_KP9;			break;
		case VK_DECIMAL:	key = KEY_KPPERIOD;		break;
		case VK_MULTIPLY:	key = '*';				break;
		case VK_ADD:		key = '+';				break;
		case VK_SUBTRACT:	key = '-';				break;
		case VK_DIVIDE:		key = '/';				break;

		case VK_F1:			key = KEY_F1;			break;
		case VK_F2:			key = KEY_F2;			break;
		case VK_F3:			key = KEY_F3;			break;
		case VK_F4:			key = KEY_F4;			break;
		case VK_F5:			key = KEY_F5;			break;
		case VK_F6:			key = KEY_F6;			break;
		case VK_F7:			key = KEY_F7;			break;
		case VK_F8:			key = KEY_F8;			break;
		case VK_F9:			key = KEY_F9;			break;
		case VK_F10:		key = KEY_F10;			break;
		case VK_F11:		key = KEY_F11;			break;
		case VK_F12:		key = KEY_F12;			break;
		case VK_F13:		key = KEY_F13;			break;
		case VK_F14:		key = KEY_F14;			break;
		case VK_F15:		key = KEY_F15;			break;
		case VK_F16:		key = KEY_F16;			break;

		default:			return (0);
		}
	}
	else
	{
		return (0);
	}

	switch (ker->dwControlKeyState)
	{
	case RIGHT_ALT_PRESSED:
	case LEFT_ALT_PRESSED:
		key &= ~MASK_META;
		key |=  MASK_ALT;
		break;

	case RIGHT_CTRL_PRESSED:
	case LEFT_CTRL_PRESSED:
		if (key <= 0xff)
		{
			key &= 0x1f;
		}
		else
		{
			key &= ~MASK_META;
			key |=  MASK_CTRL;
		}
		break;

	case SHIFT_PRESSED:
		if (key <= 0xff)
		{
			key = toupper(key);
		}
		else
		{
			key &= ~MASK_META;
			key |=  MASK_SHIFT;
		}
		break;
	}

	return (key);
}

/*------------------------------------------------------------------------
 * NT: process a keyboard-input record
 */
static int nt_kbd_record (TERM_DATA *t, KEY_EVENT_RECORD *ker)
{
	int num_queued = 0;

	if (ker->bKeyDown)
	{
		DWORD	vky;

		vky = nt_translate_key(ker);
		if (vky > 0)
		{
			WORD j;

			for (j = ker->wRepeatCount; j; j--)
			{
				term_kbd_enq(t, vky);
				num_queued++;
			}
		}
	}

	return (num_queued);
}

/*------------------------------------------------------------------------
 * NT: process a mouse-input record
 */
static int nt_mouse_record (TERM_DATA *t, MOUSE_EVENT_RECORD *mer)
{
	int num_queued = 0;

	if (mer->dwEventFlags != MOUSE_WHEELED)
	{
		MOUSE_STATUS *	m 	= &t->mouse_stat;
		int		old_bl	=
					m->button[BUTTON_LEFT];
		int		old_br	=
					m->button[BUTTON_RIGHT];
		int		old_any_button =
					(old_bl == BUTTON_DOWN) |
					(old_br == BUTTON_DOWN);
		int		new_bl =
					mer->dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED ?
					BUTTON_DOWN : BUTTON_UP;
		int		new_br =
					mer->dwButtonState & RIGHTMOST_BUTTON_PRESSED ?
					BUTTON_DOWN : BUTTON_UP;
		int		new_any_button =
					(new_bl == BUTTON_DOWN) |
					(new_br == BUTTON_DOWN);
		int		dbl_clk =
					(mer->dwEventFlags == DOUBLE_CLICK);
		int		key;

		m->button[BUTTON_LEFT]  = new_bl;
		m->button[BUTTON_RIGHT] = new_br;

		m->old_x = m->x;
		m->old_y = m->y;

		m->x = mer->dwMousePosition.X;
		m->y = mer->dwMousePosition.Y;

		if (m->x != m->old_x || m->y != m->old_y)
		{
			key = old_any_button ? MOUSE_DRAG : MOUSE_MOVE;
			term_kbd_enq(t, key);
			num_queued++;
		}

		{
			if (old_bl != new_bl)
			{
				if (new_bl == BUTTON_DOWN)
				{
					key = dbl_clk ?
						term_button_dbl[BUTTON_LEFT] :
						term_button_down[BUTTON_LEFT];
				}
				else
				{
					key = term_button_up[BUTTON_LEFT];
				}

				term_kbd_enq(t, key);
				num_queued++;
			}
			else if (old_br != new_br)
			{
				if (new_br == BUTTON_DOWN)
				{
					key = dbl_clk ?
						term_button_dbl[BUTTON_RIGHT] :
						term_button_down[BUTTON_RIGHT];
				}
				else
				{
					key = term_button_up[BUTTON_RIGHT];
				}

				term_kbd_enq(t, key);
				num_queued++;
			}
		}
	}
	else
	{
		int state = (int)mer->dwButtonState;

		term_kbd_enq(t, state < 0 ? MOUSE_SCROLLDN : MOUSE_SCROLLUP);
		num_queued++;
	}

	return (num_queued);
}
#endif

#if V_WINDOWS

/*------------------------------------------------------------------------
 * term_kbd_get1c() - return the next input char (Windows version)
 */
int term_kbd_get1c (TERM_DATA *t, int interval)
{
	int key;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (t == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * Check if any chars in kbd buffer.
	 * If so, just return with next char.
	 */
	key = term_kbd_deq(t);
	if (key > 0)
		return (key);

	/*--------------------------------------------------------------------
	 * clear out buffer
	 */
	term_kbd_empty(t);

	/*--------------------------------------------------------------------
	 * now read as many chars as we can
	 */
	{
		HANDLE	fd	= t->tty_inp;

		/*----------------------------------------------------------------
		 * Nothing queued up.  Check if anything in the event queue.
		 */
		if (interval < 0)
			interval = INFINITE;

		while (TRUE)
		{
			int		num_queued	= 0;
			DWORD	num_events	= 0;
			DWORD	event;
			DWORD	i;
			BOOL	bRC;

			/*------------------------------------------------------------
			 * wait for input event or timeout
			 */
			event = WaitForSingleObject(fd, interval);
			if (event == WAIT_OBJECT_0)
			{
				GetNumberOfConsoleInputEvents(fd, &num_events);
			}

			/*------------------------------------------------------------
			 * process any events we have pending
			 */
			for (i=0; i<num_events; i++)
			{
				INPUT_RECORD			record;
				KEY_EVENT_RECORD *		ker	= &record.Event.KeyEvent;
				MOUSE_EVENT_RECORD *	mer	= &record.Event.MouseEvent;
				DWORD					num	= 0;

				bRC = ReadConsoleInput(fd, &record, 1, &num);
				if (! bRC || num == 0)
					break;

				switch (record.EventType)
				{
				case KEY_EVENT:
					num_queued += nt_kbd_record(t, ker);
					break;

				case MOUSE_EVENT:
					num_queued += nt_mouse_record(t, mer);
					break;

				case WINDOW_BUFFER_SIZE_EVENT:
					term_kbd_enq(t, KEY_WINRESIZE);
					break;
				}
			}

			/*------------------------------------------------------------
			 * break if anything was read or we aren't waiting forever
			 */
			if (interval != INFINITE || num_queued > 0)
				break;
		}
	}

	/*--------------------------------------------------------------------
	 * dequeue first char & return it if there is one
	 */
	return term_kbd_deq(t);
}

#else

/*------------------------------------------------------------------------
 * term_kbd_get1c() - return the next input char (Unix version)
 */
int term_kbd_get1c (TERM_DATA *t, int interval)
{
	int	orig_interval	= interval;
	int max_intvl		= 10;			/* max ms to wait in "event" mode */
	int	evt_intvl		= 0;
	int key;
	int	rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (t == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * Check if any chars in kbd buffer.
	 * If so, just return with next char.
	 */
	key = term_kbd_deq(t);
	if (key > 0)
		return (key);

	/*--------------------------------------------------------------------
	 * clear out buffer
	 */
	term_kbd_empty(t);

	/*--------------------------------------------------------------------
	 * The "interval" value is specified as follows:
	 *
	 *	-1	wait "forever"	(return only when we get a key)
	 *	 0	don't wait		(return key only if one pending)
	 *	>0	wait <n>ms		(return either if we gt a key or <n>ms are used)
	 *
	 * However, if we are using an "event" routine, we cannot wait
	 * for too-long a period of time, or events don't get processed.
	 * Thus, if we are using an "event" routine, we chop up the wait
	 * into chunks to accumulate.
	 */
	if (t->evt_rtn != 0)
	{
		if (interval < 0 || interval > max_intvl)
		{
			if (interval < 0 || interval > max_intvl)
				evt_intvl = max_intvl;
			else
				evt_intvl = interval;
			interval = 0;
		}
	}
	else
	{
		evt_intvl = interval;
	}

	/*--------------------------------------------------------------------
	 * now loop until we get a key or until "orig_interval" is reached
	 */
	while (TRUE)
	{
		unsigned char	key_buf[128];
		int				num_read	= 0;
		int				fd			= t->tty_inp;
		int				i;

		/*----------------------------------------------------------------
		 * run the event routine if specified
		 */
		rc = term_screen_dev_busy(t, TERM_EVENT_READ, evt_intvl);
		if (rc < 0)
			return (-1);

		/*----------------------------------------------------------------
		 * call the input routine if specified
		 */
		if (t->inp_rtn != 0)
		{
			rc = (t->inp_rtn)(t->inp_data, TERM_INP_MODE_READ, evt_intvl);
			if (rc < 0)
				return (-1);
			if (rc > 0)
				return (rc);
		}

		/*----------------------------------------------------------------
		 * Now read as many chars as we can.
		 */
		{

#if NO_VMIN_VTIME
			{
				jmp_buf timeout;

				/*--------------------------------------------------------
				 *	System V does not have an ioctl for querying
				 *	the number of pending chars, so we set the
				 *	O_NDELAY flag on or off depending on whether
				 *	interval is zero or not.
				 *
				 *	Note that this code would also work in a BSD env.
				 */
				term_set_wait(t, interval != 0);

				if (setjmp(timeout) == 0)
				{
					/*----------------------------------------------------
				     * set alarm for timeout
					 */
					if (interval > 0)
					{
						term_alarm_set(ALARM_KEY, interval, kbd_timeout,
							&timeout, FALSE, FALSE, TRUE);
					}

					/*----------------------------------------------------
				     * read the chars
					 */
					do
					{
						if (t->kbd_buf.kbd_sig)
						{
							num_read = 0;
							break;
						}

						errno = 0;
						num_read = read(fd, key_buf, sizeof(key_buf));
						if (num_read > 0)
						{
							term_alarm_clr(ALARM_KEY);
							break;
						}
					} while (errno == EINTR && interval != 0);
				}
			}
#else
			{
				/*--------------------------------------------------------
			     * set read mode
				 */
				term_set_read_mode(t, interval);

				/*--------------------------------------------------------
			     * read the chars
				 */
				do
				{
					if (t->kbd_buf.kbd_sig)
					{
						num_read = 0;
						break;
					}

					errno = 0;
					num_read = read(fd, key_buf, sizeof(key_buf));
	
					if (num_read > 0)
					{
						break;
					}
				} while (errno == EINTR && interval != 0);
			}
#endif
		}

		/*----------------------------------------------------------------
		 * enqueue all chars read in
		 */
		for (i=0; i<num_read; i++)
		{
			term_kbd_enq(t, key_buf[i]);
		}

		/*----------------------------------------------------------------
		 * check if we are ready to quit
		 */
		if (num_read > 0 || orig_interval == 0 || t->kbd_buf.kbd_sig)
			break;

		if (orig_interval > 0)
		{
			orig_interval -= evt_intvl;
			if (orig_interval <= 0)
				break;
		}
	}

	/*--------------------------------------------------------------------
	 * dequeue first char & return it if there is one
	 */
	return term_kbd_deq(t);
}

#endif

/*------------------------------------------------------------------------
 * term_kbd_enq() - enqueue a char onto the buffer
 */
int term_kbd_enq (TERM_DATA *t, int key)
{
	KBD_BUFFER *	kbd;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (t == 0)
		return (-1);

	kbd = &t->kbd_buf;

	/*--------------------------------------------------------------------
	 * enqueue char onto buffer
	 */
	kbd_cs_enter(&kbd->kbd_crit);

	if (key > 0)
	{
		if (kbd->kbd_num_chars < sizeof(kbd->kbd_buf) / sizeof(*kbd->kbd_buf))
		{
			kbd->kbd_buf[kbd->kbd_num_chars++] = key;
		}
	}

	kbd_cs_leave(&kbd->kbd_crit);

	return (0);
}

/*------------------------------------------------------------------------
 * term_kbd_deq() - dequeue a char from the buffer
 */
int term_kbd_deq (TERM_DATA *t)
{
	KBD_BUFFER *	kbd;
	int				key;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (t == 0)
		return (-1);

	kbd = &t->kbd_buf;

	/*--------------------------------------------------------------------
	 * dequeue next key off buffer
	 */
	kbd_cs_enter(&kbd->kbd_crit);

	if (kbd->kbd_num_chars <= 0)
	{
		key = -1;
	}
	else
	{
		kbd->kbd_num_chars--;
		key = kbd->kbd_buf[kbd->kbd_buf_ptr++];
	}

	kbd_cs_leave(&kbd->kbd_crit);

	return (key);
}

/*------------------------------------------------------------------------
 * term_kbd_empty() - flush the input queue
 */
int term_kbd_empty (TERM_DATA *t)
{
	KBD_BUFFER *	kbd;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (t == 0)
		return (-1);

	kbd = &t->kbd_buf;

	/*--------------------------------------------------------------------
	 * clear the buffer
	 */
	kbd_cs_enter(&kbd->kbd_crit);

	kbd->kbd_num_chars = 0;
	kbd->kbd_buf_ptr   = 0;

	kbd_cs_leave(&kbd->kbd_crit);

	return (0);
}

/*------------------------------------------------------------------------
 * term_kbd_sig() - get/set pending signal
 */
int term_kbd_sig (TERM_DATA *t, int sig)
{
	KBD_BUFFER *	kbd		= &t->kbd_buf;
	int				old_sig	= kbd->kbd_sig;

	if (sig != -1)
		kbd->kbd_sig = sig;

	return (old_sig);
}
