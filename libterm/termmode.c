/*------------------------------------------------------------------------
 * terminal mode routines
 */
#include "termcommon.h"

/*------------------------------------------------------------------------
 * term_getstate() - internal get device state routine
 */
static int term_getstate (TERM_DATA *t, TSTATE *tstate)
{
	int rc = 0;

#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
	{
		tstate->tty_iflags = fcntl(t->tty_inp, F_GETFL, 0);
		if (tstate->tty_iflags == -1)
			rc = -1;

		tstate->tty_oflags = fcntl(t->tty_out, F_GETFL, 0);
		if (tstate->tty_oflags == -1)
			rc = -1;

#  if POSIX_TERMIO
		if (t->tty_i_isatty)
		{
			if (tcgetattr(t->tty_inp, &tstate->tty_istruct) == -1)
				rc = -1;
		}

		if (t->tty_o_isatty)
		{
			if (tcgetattr(t->tty_out, &tstate->tty_ostruct) == -1)
				rc = -1;
		}
#  else
		if (t->tty_i_isatty)
		{
			if (ioctl(t->tty_inp, TCGETA, &tstate->tty_istruct) == -1)
				rc = -1;
		}

		if (t->tty_o_isatty)
		{
			if (ioctl(t->tty_out, TCGETA, &tstate->tty_ostruct) == -1)
				rc = -1;
		}
#  endif
	}
#else
	/*--------------------------------------------------------------------
	 * Windows version
	 */
	{
		BOOL bRC;

		bRC = GetConsoleMode(t->tty_inp, &tstate->tty_iflags);
		if (! bRC)
			return (-1);

		bRC = GetConsoleMode(t->tty_out, &tstate->tty_oflags);
		if (! bRC)
			return (-1);

		bRC = GetConsoleScreenBufferInfo(t->tty_out, &tstate->tty_struct);
		if (! bRC)
			return (-1);
	}
#endif

	return (rc);
}

/*------------------------------------------------------------------------
 * term_setstate() - internal set device state routine
 */
static int term_setstate (TERM_DATA *t, TSTATE *tstate)
{
	int rc = 0;

#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
	{
		if (fcntl(t->tty_inp, F_SETFL, tstate->tty_iflags) == -1)
			rc = -1;

		if (fcntl(t->tty_out, F_SETFL, tstate->tty_oflags) == -1)
			rc = -1;

#  if POSIX_TERMIO
		if (t->tty_i_isatty)
		{
			if (tcsetattr(t->tty_inp, TCSANOW, &tstate->tty_istruct) == -1)
				rc = -1;
		}

		if (t->tty_o_isatty)
		{
			if (tcsetattr(t->tty_out, TCSANOW, &tstate->tty_ostruct) == -1)
				rc = -1;
		}
#  else
		if (t->tty_i_isatty)
		{
			if (ioctl(t->tty_inp, TCSETAW, &tstate->tty_istruct) == -1)
				rc = -1;
		}

		if (t->tty_o_isatty)
		{
			if (ioctl(t->tty_out, TCSETAW, &tstate->tty_ostruct) == -1)
				rc = -1;
		}
#  endif
	}
#else
	/*--------------------------------------------------------------------
	 * Windows version
	 */
	{
		BOOL bRC;

		bRC = SetConsoleTextAttribute(t->tty_out,
			tstate->tty_struct.wAttributes);
		if (! bRC)
			return (-1);

		bRC = SetConsoleMode(t->tty_inp, tstate->tty_iflags);
		if (! bRC)
			return (-1);

		bRC = SetConsoleMode(t->tty_out, tstate->tty_oflags);
		if (! bRC)
			return (-1);
	}
#endif

	return (rc);
}

/*------------------------------------------------------------------------
 * term_get_orig_mode() - get original terminal state
 */
int term_get_orig_mode (TERM_DATA *t)
{
	if (t == 0)
		return (-1);

	return term_getstate(t, &t->orig_st);
}

/*------------------------------------------------------------------------
 * term_get_prog_mode() - setup program terminal state
 */
int term_get_prog_mode (TERM_DATA *t)
{
	int rc;

	if (t == 0)
		return (-1);

	rc = term_getstate(t, &t->curr_st);

#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
	if (rc == 0)
	{
		/*----------------------------------------------------------------
		 * set_meta(off)
		 */
		t->curr_st.tty_istruct.c_iflag		&= ~ISTRIP;
		t->curr_st.tty_ostruct.c_iflag		&= ~ISTRIP;

		/*----------------------------------------------------------------
		 * set_echo(off)
		 */
		t->curr_st.tty_istruct.c_lflag		&= ~ECHO;
		t->curr_st.tty_ostruct.c_lflag		&= ~ECHO;

		/*----------------------------------------------------------------
		 * set_out_nl(off)
		 */
		t->curr_st.tty_istruct.c_oflag		&= ~OPOST;
		t->curr_st.tty_ostruct.c_oflag		&= ~OPOST;

		/*----------------------------------------------------------------
		 * set_inp_nl(off)
		 */
		t->curr_st.tty_istruct.c_iflag		&= ~ICRNL;
		t->curr_st.tty_ostruct.c_iflag		&= ~ICRNL;

		/*----------------------------------------------------------------
		 * set_raw(off)
		 */
		t->curr_st.tty_istruct.c_iflag		|=  (IXON | IXOFF);
		t->curr_st.tty_ostruct.c_iflag		|=  (IXON | IXOFF);

		t->curr_st.tty_istruct.c_lflag		|=  ISIG;
		t->curr_st.tty_ostruct.c_lflag		|=  ISIG;

		/*----------------------------------------------------------------
		 * set_cbreak(on)
		 */
		t->curr_st.tty_istruct.c_lflag		&=  ~ICANON;
		t->curr_st.tty_ostruct.c_lflag		&=  ~ICANON;

		t->curr_st.tty_istruct.c_cc[VMIN]	 = 1;
		t->curr_st.tty_ostruct.c_cc[VMIN]	 = 1;

		t->curr_st.tty_istruct.c_cc[VTIME]	 = 0;
		t->curr_st.tty_ostruct.c_cc[VTIME]	 = 0;

		/*----------------------------------------------------------------
		 * set_qiflush(off)
		 */
		t->curr_st.tty_istruct.c_lflag		|=   NOFLSH;
		t->curr_st.tty_ostruct.c_lflag		|=   NOFLSH;

		/*----------------------------------------------------------------
		 * clear interval
		 */
		t->old_interval						 = -1;
	}
#else
	/*--------------------------------------------------------------------
	 * Windows version
	 */
	if (rc == 0)
	{
		/*----------------------------------------------------------------
		 * set input mode
		 */
		t->curr_st.tty_iflags				 = ENABLE_WINDOW_INPUT |
												ENABLE_PROCESSED_INPUT;
		if (t->mouse_stat.is_on)
			t->curr_st.tty_iflags			|= ENABLE_MOUSE_INPUT;

		/*----------------------------------------------------------------
		 * set output mode
		 */
		t->curr_st.tty_oflags				 = 0;

		/*----------------------------------------------------------------
		 * clear interval
		 */
		t->old_interval						 = -1;
	}
#endif

	return (rc);
}

/*------------------------------------------------------------------------
 * term_get_save_mode() - save current terminal state
 */
int term_get_save_mode (TERM_DATA *t)
{
	int rc;

	if (t == 0)
		return (-1);

	rc = term_getstate(t, &t->save_st);
	if (rc == 0)
		t->state_saved = TRUE;

	return (rc);
}

/*------------------------------------------------------------------------
 * term_set_orig_mode() - set terminal to original state
 */
int term_set_orig_mode (TERM_DATA *t)
{
	if (t == 0)
		return (-1);

	if (t->inp_rtn != 0)
		(t->inp_rtn)(t->inp_data, TERM_INP_MODE_CLOSE, 0);

	if (t->out_rtn != 0)
		(t->out_rtn)(t->out_data, TERM_OUT_MODE_CLOSE, 0, 0);

	return term_setstate(t, &t->orig_st);
}

/*------------------------------------------------------------------------
 * term_set_prog_mode() - set terminal to program state
 */
int term_set_prog_mode (TERM_DATA *t)
{
	if (t == 0)
		return (-1);

	if (t->inp_rtn != 0)
		(t->inp_rtn)(t->inp_data, TERM_INP_MODE_OPEN, 0);

	if (t->out_rtn != 0)
		(t->out_rtn)(t->out_data, TERM_OUT_MODE_OPEN, 0, 0);

	return term_setstate(t, &t->curr_st);
}

/*------------------------------------------------------------------------
 * term_set_save_mode() - set terminal to saved state
 */
int term_set_save_mode (TERM_DATA *t)
{
	if (t == 0)
		return (-1);

	if (! t->state_saved)
		return (-1);

	return term_setstate(t, &t->save_st);
}

/*------------------------------------------------------------------------
 * term_flush_inp() - flush all input
 */
int term_flush_inp (TERM_DATA *t)
{
	if (t == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * clear keyboard buffer
	 */
	term_kbd_empty(t);

#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
#  if POSIX_TERMIO
	return (tcflush(t->tty_inp, TCIFLUSH));
#  else
	return (ioctl(t->tty_inp, TCFLSH, 0));
#  endif

#else
	/*--------------------------------------------------------------------
	 * Windows version
	 */
	return FlushConsoleInputBuffer(t->tty_inp) ? 0 : -1;
#endif
}

/*------------------------------------------------------------------------
 * term_flush_out() - flush all output
 */
int term_flush_out (TERM_DATA *t)
{
	if (t == 0)
		return (-1);

#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
#  if POSIX_TERMIO
	return (tcflush(t->tty_out, TCOFLUSH));
#  else
	return (ioctl(t->tty_out, TCFLSH, 1));
#  endif

#else
	/*--------------------------------------------------------------------
	 * Windows version
	 */
	return (0);
#endif
}

/*------------------------------------------------------------------------
 * term_get_wait() - get wait state (blocking/non-blocking)
 */
int term_get_wait (const TERM_DATA *t)
{
	if (t == 0)
		return (FALSE);

#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
#  if POSIX_TERMIO
	return ((t->curr_st.tty_iflags & O_NONBLOCK) == 0);
#  else
	return ((t->curr_st.tty_iflags & O_NDELAY)   == 0);
#  endif

#else
	/*--------------------------------------------------------------------
	 * Windows version
	 */
	return (FALSE);
#endif
}

/*------------------------------------------------------------------------
 * term_set_wait() - set wait state (blocking/non-blocking)
 */
int term_set_wait (TERM_DATA *t, int flag)
{
	int old_state;

	if (t == 0)
		return (-1);

	old_state = term_get_wait(t);

	if (old_state != flag)
	{
#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
#  if POSIX_TERMIO
		if (flag)
		{
			t->curr_st.tty_iflags &= ~O_NONBLOCK;
			t->curr_st.tty_oflags &= ~O_NONBLOCK;
		}
		else
		{
			t->curr_st.tty_iflags |=  O_NONBLOCK;
			t->curr_st.tty_oflags |=  O_NONBLOCK;
		}
#  else
		if (flag)
		{
			t->curr_st.tty_iflags &= ~O_NDELAY;
			t->curr_st.tty_oflags &= ~O_NDELAY;
		}
		else
		{
			t->curr_st.tty_iflags |=  O_NDELAY;
			t->curr_st.tty_oflags |=  O_NDELAY;
		}
#  endif

		fcntl(t->tty_inp, F_SETFL, t->curr_st.tty_iflags);
		fcntl(t->tty_out, F_SETFL, t->curr_st.tty_oflags);
#else
	/*--------------------------------------------------------------------
	 * Windows version
	 */
#endif
	}

	return (old_state);
}

/*------------------------------------------------------------------------
 * term_get_baud_rate() - get current baudrate
 */
int term_get_baud_rate (const TERM_DATA *t)
{
#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
	int c;

	if (t == 0 || ! t->tty_i_isatty)
		return (-1);

#  if POSIX_TERMIO
	c = cfgetispeed(&t->curr_st.tty_istruct);
#  else
	c = t->curr_st.tty_istruct.c_cflag & CBAUD;
#  endif

	switch (c)
	{
		case B0:		return (0);
		case B50:		return (50);
		case B75:		return (75);
		case B110:		return (110);
		case B134:		return (134);
		case B150:		return (150);
		case B200:		return (200);
		case B300:		return (300);
		case B600:		return (600);
		case B1200:		return (1200);
		case B1800:		return (1800);
		case B2400:		return (2400);
		case B4800:		return (4800);
		case B9600:		return (9600);
#  ifdef B19200
		case B19200:	return (19200);
#  else
		case EXTA:		return (19200);
#  endif
#  ifdef B38400
		case B38400:	return (38400);
#  else
		case EXTB:		return (38400);
#  endif
#  ifdef B57600
		case B57600:	return (57600);
#  endif
#  ifdef B76800
		case B76800:	return (76800);
#  endif
#  ifdef B115200
		case B115200:	return (115200);
#  endif
#  ifdef B153600
		case B153600:	return (153600);
#  endif
#  ifdef B230400
		case B230400:	return (230400);
#  endif
#  ifdef B307200
		case B307200:	return (307200);
#  endif
#  ifdef B460800
		case B460800:	return (460800);
#  endif
		default:		return (-1);
	}
#else
	/*--------------------------------------------------------------------
	 * Windows version
	 */
	return (-1);
#endif
}

/*------------------------------------------------------------------------
 * term_get_kill_char() - get current kill character
 */
int term_get_kill_char (const TERM_DATA *t)
{
#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
	if (t == 0 || ! t->tty_i_isatty)
		return (-1);

	return (t->curr_st.tty_istruct.c_cc[VKILL]);
#else
	/*--------------------------------------------------------------------
	 * Windows version
	 */
	return (0);
#endif
}

/*------------------------------------------------------------------------
 * term_get_quit_char() - get current quit character
 */
int term_get_quit_char (const TERM_DATA *t)
{
#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
	if (t == 0 || ! t->tty_i_isatty)
		return (-1);

	return (t->curr_st.tty_istruct.c_cc[VQUIT]);
#else
	/*--------------------------------------------------------------------
	 * Windows version
	 */
	return (0);
#endif
}

/*------------------------------------------------------------------------
 * term_get_intr_char() - get current interrupt character
 */
int term_get_intr_char (const TERM_DATA *t)
{
#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
	if (t == 0 || ! t->tty_i_isatty)
		return (-1);

	return (t->curr_st.tty_istruct.c_cc[VINTR]);
#else
	/*--------------------------------------------------------------------
	 * Windows version
	 */
	return (0);
#endif
}

/*------------------------------------------------------------------------
 * term_get_erase_char() - get current erase character
 */
int term_get_erase_char (const TERM_DATA *t)
{
#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
	if (t == 0 || ! t->tty_i_isatty)
		return (-1);

	return (t->curr_st.tty_istruct.c_cc[VERASE]);
#else
	/*--------------------------------------------------------------------
	 * Windows version
	 */
	return (0);
#endif
}

/*------------------------------------------------------------------------
 * term_set_baud_rate() - set current baudrate (both input & output)
 */
int term_set_baud_rate (TERM_DATA *t, int br)
{
#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
	int c;
	int rc;

	if (t == 0 || ! t->tty_i_isatty)
		return (-1);

	switch (br)
	{
		case 0:			c = B0;				break;
		case 50:		c = B50;			break;
		case 75:		c = B75;			break;
		case 110:		c = B110;			break;
		case 134:		c = B134;			break;
		case 150:		c = B150;			break;
		case 200:		c = B200;			break;
		case 300:		c = B300;			break;
		case 600:		c = B600;			break;
		case 1200:		c = B1200;			break;
		case 1800:		c = B1800;			break;
		case 2400:		c = B2400;			break;
		case 4800:		c = B4800;			break;
		case 9600:		c = B9600;			break;
#  ifdef B19200
		case 19200:		c = B19200;			break;
#  else
		case 19200:		c = EXTA;			break;
#  endif
#  ifdef B38400
		case 38400:		c = B38400;			break;
#  else
		case 38400:		c = EXTB;			break;
#  endif
#  ifdef B57600
		case 57600:		c = B57600;			break;
#  endif
#  ifdef B76800
		case 76800:		c = B76800;			break;
#  endif
#  ifdef B115200
		case 115200:	c = B115200;		break;
#  endif
#  ifdef B153600
		case 153600:	c = B153600;		break;
#  endif
#  ifdef B230400
		case 230400:	c = B230400;		break;
#  endif
#  ifdef B307200
		case 307200:	c = B307200;		break;
#  endif
#  ifdef B460800
		case 460800:	c = B460800;		break;
#  endif
		default:		return (-1);
	}

#  if POSIX_TERMIO
	rc = cfsetispeed(&t->curr_st.tty_istruct, c);
	if (rc)
		return (-1);

	rc = cfsetospeed(&t->curr_st.tty_istruct, c);
	if (rc)
		return (-1);

	rc = cfsetispeed(&t->curr_st.tty_ostruct, c);
	if (rc)
		return (-1);

	rc = cfsetospeed(&t->curr_st.tty_ostruct, c);
	if (rc)
		return (-1);
#  else
	t->curr_st.tty_istruct.c_cflag &= ~CBAUD;
	t->curr_st.tty_istruct.c_cflag |= c;

	t->curr_st.tty_ostruct.c_cflag &= ~CBAUD;
	t->curr_st.tty_ostruct.c_cflag |= c;
#  endif

	rc = term_set_prog_mode(t);
	return (rc);
#else
	/*--------------------------------------------------------------------
	 * Windows version
	 */
	return (-1);
#endif
}

/*------------------------------------------------------------------------
 * term_set_kill_char() - set current kill character
 */
int term_set_kill_char (TERM_DATA *t, int ch)
{
#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
	if (t == 0 || ! t->tty_i_isatty)
		return (-1);

	t->curr_st.tty_istruct.c_cc[VKILL] = ch;
	t->curr_st.tty_ostruct.c_cc[VKILL] = ch;

	return term_set_prog_mode(t);
#else
	/*--------------------------------------------------------------------
	 * Windows version
	 */
	return (-1);
#endif
}

/*------------------------------------------------------------------------
 * term_set_quit_char() - set current quit character
 */
int term_set_quit_char (TERM_DATA *t, int ch)
{
#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
	if (t == 0 || ! t->tty_i_isatty)
		return (-1);

	t->curr_st.tty_istruct.c_cc[VQUIT] = ch;
	t->curr_st.tty_ostruct.c_cc[VQUIT] = ch;

	return term_set_prog_mode(t);
#else
	/*--------------------------------------------------------------------
	 * Windows version
	 */
	return (-1);
#endif
}

/*------------------------------------------------------------------------
 * term_set_intr_char() - set current interrupt character
 */
int term_set_intr_char (TERM_DATA *t, int ch)
{
#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
	if (t == 0 || ! t->tty_i_isatty)
		return (-1);

	t->curr_st.tty_istruct.c_cc[VINTR] = ch;
	t->curr_st.tty_ostruct.c_cc[VINTR] = ch;

	return term_set_prog_mode(t);
#else
	/*--------------------------------------------------------------------
	 * Windows version
	 */
	return (-1);
#endif
}

/*------------------------------------------------------------------------
 * term_set_erase_char() - set current erase character
 */
int term_set_erase_char (TERM_DATA *t, int ch)
{
#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
	if (t == 0 || ! t->tty_i_isatty)
		return (-1);

	t->curr_st.tty_istruct.c_cc[VERASE] = ch;
	t->curr_st.tty_ostruct.c_cc[VERASE] = ch;

	return term_set_prog_mode(t);
#else
	/*--------------------------------------------------------------------
	 * Windows version
	 */
	return (-1);
#endif
}

/*------------------------------------------------------------------------
 * term_set_inp_nl() - set input nl processing on/off
 */
int term_set_inp_nl (TERM_DATA *t, int bf)
{
	int rc = 0;

#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
	if (t == 0 || ! t->tty_i_isatty)
		return (-1);

	if (bf)
	{
		/*----------------------------------------------------------------
		 * map cr to nl on input
		 */
		t->curr_st.tty_istruct.c_iflag |=  ICRNL;
		t->curr_st.tty_ostruct.c_iflag |=  ICRNL;
	}
	else
	{
		/*----------------------------------------------------------------
		 * no map cr to nl on input
		 */
		t->curr_st.tty_istruct.c_iflag &= ~ICRNL;
		t->curr_st.tty_ostruct.c_iflag &= ~ICRNL;
	}

	rc = term_setstate(t, &t->curr_st);
#else
	/*--------------------------------------------------------------------
	 * Windows version
	 */
	rc = 0;
#endif

	return (rc);
}

/*------------------------------------------------------------------------
 * term_set_out_nl() - set output nl processing on/off
 */
int term_set_out_nl (TERM_DATA *t, int bf)
{
	int rc = 0;

#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
	if (t == 0 || ! t->tty_i_isatty)
		return (-1);

	if (bf)
	{
		/*----------------------------------------------------------------
		 * postprocess output
		 */
		t->curr_st.tty_istruct.c_oflag |=  OPOST;
		t->curr_st.tty_ostruct.c_oflag |=  OPOST;
	}
	else
	{
		/*----------------------------------------------------------------
		 * no postprocess output
		 */
		t->curr_st.tty_istruct.c_oflag &= ~OPOST;
		t->curr_st.tty_ostruct.c_oflag &= ~OPOST;
	}

	rc = term_setstate(t, &t->curr_st);
#else
	/*--------------------------------------------------------------------
	 * Windows version
	 */
	t->curr_st.tty_oflags = (bf ? ENABLE_PROCESSED_OUTPUT : 0);
	rc = term_set_prog_mode(t);
#endif

	return (rc);
}

/*------------------------------------------------------------------------
 * term_set_raw() - set input raw mode on/off
 */
int term_set_raw (TERM_DATA *t, int bf)
{
	int rc = 0;

#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
	if (t == 0 || ! t->tty_i_isatty)
		return (-1);

	if (bf)
	{
		/*----------------------------------------------------------------
		 * turn off signals
		 */
		t->curr_st.tty_istruct.c_lflag &= ~ISIG;
		t->curr_st.tty_ostruct.c_lflag &= ~ISIG;

		/*----------------------------------------------------------------
		 * turn off canonical processing
		 */
		t->curr_st.tty_istruct.c_lflag &= ~ICANON;
		t->curr_st.tty_ostruct.c_lflag &= ~ICANON;

		/*----------------------------------------------------------------
		 * turn off x-on/x-off
		 */
		t->curr_st.tty_istruct.c_iflag &= ~(IXON | IXOFF);
		t->curr_st.tty_ostruct.c_iflag &= ~(IXON | IXOFF);
	}
	else
	{
		/*----------------------------------------------------------------
		 * turn on signals
		 */
		t->curr_st.tty_istruct.c_lflag |=  ISIG;
		t->curr_st.tty_ostruct.c_lflag |=  ISIG;

		/*----------------------------------------------------------------
		 * turn on canonical processing
		 */
		t->curr_st.tty_istruct.c_lflag |=  ICANON;
		t->curr_st.tty_ostruct.c_lflag |=  ICANON;

		/*----------------------------------------------------------------
		 * turn on x-on/x-off
		 */
		t->curr_st.tty_istruct.c_iflag |=  (IXON | IXOFF);
		t->curr_st.tty_ostruct.c_iflag |=  (IXON | IXOFF);
	}

	rc = term_set_prog_mode(t);
#endif

	return (rc);
}

/*------------------------------------------------------------------------
 * term_set_cbreak() - set input cbreak mode on/off
 */
int term_set_cbreak (TERM_DATA *t, int bf)
{
	int rc = 0;

#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
	if (t == 0)
		return (-1);

	if (bf)
	{
		/*----------------------------------------------------------------
		 * cbreak on  - turn off canonical processing
		 */
		t->curr_st.tty_istruct.c_lflag    &= ~ICANON;
		t->curr_st.tty_istruct.c_cc[VMIN]  = 1;
		t->curr_st.tty_istruct.c_cc[VTIME] = 0;

		t->curr_st.tty_ostruct.c_lflag    &= ~ICANON;
		t->curr_st.tty_ostruct.c_cc[VMIN]  = 1;
		t->curr_st.tty_ostruct.c_cc[VTIME] = 0;
	}
	else
	{
		/*----------------------------------------------------------------
		 * cbreak off - turn on canonical processing
		 */
		t->curr_st.tty_istruct.c_lflag    |= ICANON;
		t->curr_st.tty_istruct.c_cc[VEOF]  = 0x04;
		t->curr_st.tty_istruct.c_cc[VEOL]  = 0x00;

		t->curr_st.tty_ostruct.c_lflag    |= ICANON;
		t->curr_st.tty_ostruct.c_cc[VEOF]  = 0x04;
		t->curr_st.tty_ostruct.c_cc[VEOL]  = 0x00;
	}

	rc = term_set_prog_mode(t);
#endif

	return (rc);
}

/*------------------------------------------------------------------------
 * term_set_echo() - set input echo mode on/off
 */
int term_set_echo (TERM_DATA *t, int bf)
{
	int rc = 0;

#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
	if (t == 0)
		return (-1);

	if (bf)
	{
		/*----------------------------------------------------------------
		 * echo input
		 */
		t->curr_st.tty_istruct.c_lflag |=  ECHO;
		t->curr_st.tty_ostruct.c_lflag |=  ECHO;
	}
	else
	{
		/*----------------------------------------------------------------
		 * no echo of input
		 */
		t->curr_st.tty_istruct.c_lflag &= ~ECHO;
		t->curr_st.tty_ostruct.c_lflag &= ~ECHO;
	}

	rc = term_set_prog_mode(t);
#endif

	return (rc);
}

/*------------------------------------------------------------------------
 * term_set_meta() - set input bit count (7/8 bits)
 */
int term_set_meta (TERM_DATA *t, int bf)
{
	int rc = 0;

#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
	if (t == 0)
		return (-1);

	if (bf)
	{
		/*----------------------------------------------------------------
		 * strip eighth bit
		 */
		t->curr_st.tty_istruct.c_iflag |=  ISTRIP;
		t->curr_st.tty_ostruct.c_iflag |=  ISTRIP;
	}
	else
	{
		/*----------------------------------------------------------------
		 * no strip of eighth bit
		 */
		t->curr_st.tty_istruct.c_iflag &= ~ISTRIP;
		t->curr_st.tty_ostruct.c_iflag &= ~ISTRIP;
	}

	rc = term_set_prog_mode(t);
#endif

	return (rc);
}

/*------------------------------------------------------------------------
 * term_set_qiflush() - set output flush-on-interrupt mode on/off
 */
int term_set_qiflush (TERM_DATA *t, int bf)
{
	int rc = 0;

#if V_UNIX
	/*--------------------------------------------------------------------
	 * Unix version
	 */
	if (t == 0)
		return (-1);

	if (bf)
	{
		/*----------------------------------------------------------------
		 * flush after interrupt
		 */
		t->curr_st.tty_istruct.c_lflag &= ~NOFLSH;
		t->curr_st.tty_ostruct.c_lflag &= ~NOFLSH;
	}
	else
	{
		/*----------------------------------------------------------------
		 * no flush after interrupt
		 */
		t->curr_st.tty_istruct.c_lflag |=  NOFLSH;
		t->curr_st.tty_ostruct.c_lflag |=  NOFLSH;
	}

	rc = term_set_prog_mode(t);
#endif

	return (rc);
}

/*------------------------------------------------------------------------
 * term_get_read_mode() - get input read mode (-1 / 0 / n)
 */
int term_get_read_mode (const TERM_DATA *t)
{
	if (t == 0)
		return (-1);

	return (t->old_interval);
}

/*------------------------------------------------------------------------
 * term_set_read_mode() - Set read mode
 *
 * This routine will set the VMIN & VTIME entries in the termio struct
 * as follows:
 *
 *	interval < 0	VMIN = 1	VTIME = 0
 *	interval = 0	VMIN = 0	VTIME = 0
 *	interval > 0	VMIN = 0	VTIME = interval / 100 (tenths of sec)
 */
int term_set_read_mode (TERM_DATA *t, int interval)
{
	int i;

	if (t == 0)
		return (-1);

	i = t->old_interval;
	if (interval != i)
	{
#if V_UNIX
		int vmin;
		int vtim;

		if (interval < 0)
		{
			vmin = 1;
			vtim = 0;
		}
		else if (interval == 0)
		{
			vmin = 0;
			vtim = 0;
		}
		else /* if (interval > 0) */
		{
			vmin = 0;
			vtim = (interval + 99) / 100;
		}

		t->curr_st.tty_istruct.c_cc[VMIN]  = vmin;
		t->curr_st.tty_istruct.c_cc[VTIME] = vtim;

		t->curr_st.tty_ostruct.c_cc[VMIN]  = vmin;
		t->curr_st.tty_ostruct.c_cc[VTIME] = vtim;

		term_set_prog_mode(t);
#endif

		t->old_interval = interval;
	}

	return (i);
}

#if V_WINDOWS
/*------------------------------------------------------------------------
 * WINDOWS color table
 * These tables convert between WINDOWS color values and a color index.
 */
#  define FG_I			FOREGROUND_INTENSITY
#  define FG_R			FOREGROUND_RED
#  define FG_G			FOREGROUND_GREEN
#  define FG_B			FOREGROUND_BLUE

#  define FG_A			(FG_I | FG_R | FG_G | FG_B)

#  define BG_I			BACKGROUND_INTENSITY
#  define BG_R			BACKGROUND_RED
#  define BG_G			BACKGROUND_GREEN
#  define BG_B			BACKGROUND_BLUE

#  define BG_A			(BG_I | BG_R | BG_G | BG_B)

#  define NUM_NT_COLORS	16

static const WORD term_win_fg_colors[] =
{
/*    INT    RED    GRN    BLU							*/
	( 0    | 0    | 0    | 0    ),		/* black		*/
	( 0    | FG_R | 0    | 0    ),		/* red			*/
	( 0    | 0    | FG_G | 0    ),		/* green		*/
	( 0    | FG_R | FG_G | 0    ),		/* yellow		*/
	( 0    | 0    | 0    | FG_B ),		/* blue			*/
	( 0    | FG_R | 0    | FG_B ),		/* magenta		*/
	( 0    | 0    | FG_G | FG_B ),		/* cyan			*/
	( 0    | FG_R | FG_G | FG_B ),		/* white		*/

	( FG_I | 0    | 0    | 0    ),		/* grey			*/
	( FG_I | FG_R | 0    | 0    ),		/* lt-red		*/
	( FG_I | 0    | FG_G | 0    ),		/* lt-green		*/
	( FG_I | FG_R | FG_G | 0    ),		/* lt-yellow	*/
	( FG_I | 0    | 0    | FG_B ),		/* lt-blue		*/
	( FG_I | FG_R | 0    | FG_B ),		/* lt-magenta	*/
	( FG_I | 0    | FG_G | FG_B ),		/* lt-cyan		*/
	( FG_I | FG_R | FG_G | FG_B )		/* lt-white		*/
};

static const WORD term_win_bg_colors[] =
{
/*    INT    RED    GRN    BLU							*/
	( 0    | 0    | 0    | 0    ),		/* black		*/
	( 0    | BG_R | 0    | 0    ),		/* red			*/
	( 0    | 0    | BG_G | 0    ),		/* green		*/
	( 0    | BG_R | BG_G | 0    ),		/* yellow		*/
	( 0    | 0    | 0    | BG_B ),		/* blue			*/
	( 0    | BG_R | 0    | BG_B ),		/* magenta		*/
	( 0    | 0    | BG_G | BG_B ),		/* cyan			*/
	( 0    | BG_R | BG_G | BG_B ),		/* white		*/

	( BG_I | 0    | 0    | 0    ),		/* grey			*/
	( BG_I | BG_R | 0    | 0    ),		/* lt-red		*/
	( BG_I | 0    | BG_G | 0    ),		/* lt-green		*/
	( BG_I | BG_R | BG_G | 0    ),		/* lt-yellow	*/
	( BG_I | 0    | 0    | BG_B ),		/* lt-blue		*/
	( BG_I | BG_R | 0    | BG_B ),		/* lt-magenta	*/
	( BG_I | 0    | BG_G | BG_B ),		/* lt-cyan		*/
	( BG_I | BG_R | BG_G | BG_B	)		/* lt-white		*/
};

/*------------------------------------------------------------------------
 * term_win_get_fg_num() - get the fg num from a WINDOWS attribute value
 */
static int term_win_get_fg_num (WORD attr)
{
	WORD	attr_fg	= attr & FG_A;
	int		f;

	for (f=0; f < NUM_NT_COLORS; f++)
	{
		if (attr_fg == term_win_fg_colors[f])
			break;
	}

	return (f);
}

/*------------------------------------------------------------------------
 * term_win_get_bg_num() - get the bg num from a WINDOWS attribute value
 */
static int term_win_get_bg_num (WORD attr)
{
	WORD	attr_bg = attr & BG_A;
	int		b;

	for (b=0; b < NUM_NT_COLORS; b++)
	{
		if (attr_bg == term_win_bg_colors[b])
			break;
	}

	return (b);
}

/*------------------------------------------------------------------------
 * term_win_get_fg_val() - get the WINDOWS fg value from a fg num
 */
static WORD term_win_get_fg_val (int fg)
{
	return term_win_fg_colors[fg % NUM_NT_COLORS];
}

/*------------------------------------------------------------------------
 * term_win_get_bg_val() - get the WINDOWS bg value from a bg num
 */
static WORD term_win_get_bg_val (int bg)
{
	return term_win_bg_colors[bg % NUM_NT_COLORS];
}

/*------------------------------------------------------------------------
 * term_win_set_cursor() - Windows set cursor value
 */
int term_win_set_cursor (TERM_DATA *t, int visibility)
{
	CONSOLE_CURSOR_INFO	cci;
	BOOL				bRC;

	bRC = GetConsoleCursorInfo(t->tty_out, &cci);
	if (bRC)
	{
		cci.bVisible = visibility;
		bRC = SetConsoleCursorInfo(t->tty_out, &cci);
	}

	return (bRC ? 0 : -1);
}

/*------------------------------------------------------------------------
 * term_win_set_mouse() - Windows set mouse on|off
 */
int term_win_set_mouse (TERM_DATA *t, int bf)
{
	BOOL bRC;

	if (bf)
		t->curr_st.tty_iflags |=  ENABLE_MOUSE_INPUT;
	else
		t->curr_st.tty_iflags &= ~ENABLE_MOUSE_INPUT;

	bRC = SetConsoleMode(t->tty_inp, t->curr_st.tty_iflags);

	return (bRC ? 0 : -1);
}

/*------------------------------------------------------------------------
 * term_win_set_position() - Windows set cursor position
 */
int term_win_set_position (TERM_DATA *t, int y, int x)
{
	COORD	pos;
	BOOL	bRC;

	pos.Y	= y;
	pos.X	= x;

	bRC = SetConsoleCursorPosition(t->tty_out, pos);

	return (bRC ? 0 : -1);
}

/*------------------------------------------------------------------------
 * term_win_set_color() - Windows set color
 */
int term_win_set_color (TERM_DATA *t, int fg, int bg)
{
	WORD	attr;
	BOOL	bRC;

	attr = term_win_get_fg_val(fg) | term_win_get_bg_val(bg);
	bRC = SetConsoleTextAttribute(t->tty_out, attr);
	if (bRC)
		t->curr_st.tty_struct.wAttributes = attr;

	return (bRC ? 0 : -1);
}

/*------------------------------------------------------------------------
 * term_win_get_color() - Windows get color
 */
int term_win_get_color (TERM_DATA *t, int *fg, int *bg)
{
	CONSOLE_SCREEN_BUFFER_INFO	csbi;
	BOOL	bRC;
	WORD	attr;

	bRC = GetConsoleScreenBufferInfo(t->tty_out, &csbi);
	if (bRC)
	{
		attr = csbi.wAttributes;
		*fg  = term_win_get_fg_num(attr);
		*bg  = term_win_get_bg_num(attr);
	}

	return (bRC ? 0 : -1);
}

/*------------------------------------------------------------------------
 * term_win_set_title() - Windows set window title
 */
int term_win_set_title (TERM_DATA *t, const char *str)
{
	BOOL bRC;

	bRC = SetConsoleTitle(str);

	return (bRC ? 0 : -1);
}

#endif
