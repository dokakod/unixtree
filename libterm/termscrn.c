/*------------------------------------------------------------------------
 * term device routines
 */
#include "termcommon.h"

/*------------------------------------------------------------------------
 * term_screen_dev_init() - initialize a device struct
 */
TERM_DATA * term_screen_dev_init (void)
{
	TERM_DATA *t;

	t = (TERM_DATA *)MALLOC(sizeof(*t));
	if (t != 0)
	{
		memset(t, 0, sizeof(*t));

		t->tty_inp					= INVALID_HANDLE_VALUE;
		t->tty_out					= INVALID_HANDLE_VALUE;
		t->tty_i_isatty				= -1;
		t->tty_o_isatty				= -1;
		t->old_interval				= -1;
		t->state_saved				= FALSE;
		t->mouse_stat.saved_buttons	= 3;
	}

	return (t);
}

/*------------------------------------------------------------------------
 * term_screen_dev_free() - free a device struct
 */
void term_screen_dev_free (TERM_DATA *t)
{
	if (t != 0)
	{
		FREE(t);
	}
}

/*------------------------------------------------------------------------
 * term_screen_dev_open() - open a device
 */
int term_screen_dev_open (TERM_DATA *t, int inp, int out)
{
	if (t == 0)
		return (-1);

#if V_UNIX
	t->tty_inp		= (inp == INVALID_HANDLE_VALUE ? fileno(stdin)  : inp);
	t->tty_out		= (out == INVALID_HANDLE_VALUE ? fileno(stdout) : out);

	t->tty_i_isatty	= isatty(t->tty_inp);
	if (t->tty_i_isatty <= 0 && errno == EBADF)
		return (-1);

	t->tty_o_isatty	= isatty(t->tty_out);
	if (t->tty_o_isatty <= 0 && errno == EBADF)
		return (-1);
#else
	t->tty_inp		= GetStdHandle(STD_INPUT_HANDLE);
	t->tty_out		= GetStdHandle(STD_OUTPUT_HANDLE);

	t->tty_i_isatty	= TRUE;
	t->tty_o_isatty	= TRUE;

	if (! t->kbd_buf.kbd_crit_init)
	{
		InitializeCriticalSection(&t->kbd_buf.kbd_crit);
		t->kbd_buf.kbd_crit_init = TRUE;
	}
#endif

	return (0);
}

/*------------------------------------------------------------------------
 * term_screen_dev_close() - close a device
 */
int term_screen_dev_close (TERM_DATA *t)
{
	if (t != 0)
	{
		t->tty_inp		= INVALID_HANDLE_VALUE;
		t->tty_out		= INVALID_HANDLE_VALUE;
		t->tty_i_isatty	= -1;
		t->tty_o_isatty	= -1;
#if V_WINDOWS
		if (t->kbd_buf.kbd_crit_init)
		{
			DeleteCriticalSection(&t->kbd_buf.kbd_crit);
			t->kbd_buf.kbd_crit_init = FALSE;
		}
#endif
	}

	return (0);
}

/*------------------------------------------------------------------------
 * term_screen_dev_write() - output a buffer to a device
 */
int term_screen_dev_write (TERM_DATA *t, const unsigned char *buf, int num)
{
	int	rc	= 0;
	int	erc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (t == 0)
		return (-1);

	if (t->tty_out == INVALID_HANDLE_VALUE)
		return (-1);

	/*--------------------------------------------------------------------
	 * call any output routine if set
	 */
	if (t->out_rtn != 0)
	{
		rc = (t->out_rtn)(t->out_data, TERM_OUT_MODE_WRITE, buf, num);
		if (rc < 0)
			return (-1);
		if (rc > 0)
			return (0);
	}

	/*--------------------------------------------------------------------
	 * write out any data
	 */
#if V_UNIX
	while (num > 0)
	{
		int num_written;

		num_written = write(t->tty_out, buf, num);
		if (num_written < 0)
		{
			if (errno == EINTR)
				continue;

			rc = -1;
			break;
		}

		buf += num_written;
		num -= num_written;
	}
#else
	while (num > 0)
	{
		BOOL	bRC;
		DWORD	num_written;

		bRC = WriteConsole(t->tty_out, buf, num, &num_written, 0);
		if (! bRC)
		{
			rc = -1;
			break;
		}

		buf += num_written;
		num -= num_written;
	}
#endif

	/*--------------------------------------------------------------------
	 * now always call the event routine if set
	 */
	erc = term_screen_dev_busy(t, TERM_EVENT_WRITE, 0);
	if (erc < 0)
		rc = erc;

	return (rc);
}

/*------------------------------------------------------------------------
 * term_screen_dev_busy() - call any event routine defined
 */
int term_screen_dev_busy (TERM_DATA *t, int event, int ms)
{
	int rc;

	if (t == 0)
		return (-1);

	if (t->evt_rtn == 0)
		return (0);

	rc = (t->evt_rtn)(t->evt_data, ms);

	return (rc);
}

/*------------------------------------------------------------------------
 * term_screen_dev_evt() - set event routine to call
 */
int term_screen_dev_evt (TERM_DATA *t, TERM_EVT_RTN *rtn, void *data)
{
	int	rc;

	if (t == 0)
		return (-1);

	t->evt_rtn	= rtn;
	t->evt_data	= data;

	rc = term_screen_dev_busy(t, TERM_EVENT_INIT, 0);
	if (rc < 0)
		return (-1);

	return (0);
}

/*------------------------------------------------------------------------
 * term_screen_dev_evt_rtn () - get event-routine pointer
 * term_screen_dev_evt_data() - get event-data    pointer
 */
TERM_EVT_RTN * term_screen_dev_evt_rtn (TERM_DATA *t)
{
	if (t == 0)
		return (0);

	return (t->evt_rtn);
}

void * term_screen_dev_evt_data (TERM_DATA *t)
{
	if (t == 0)
		return (0);

	return (t->evt_data);
}

/*------------------------------------------------------------------------
 * term_screen_dev_inp() - set output routine to call
 */
int term_screen_dev_inp (TERM_DATA *t, TERM_INP_RTN *rtn, void *data)
{
	if (t == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * check if rtn was already specified & turn off if so
	 */
	if (t->inp_rtn != 0)
	{
		(t->inp_rtn)(t->inp_data, TERM_INP_MODE_CLOSE, 0);
	}

	/*--------------------------------------------------------------------
	 * store pointers
	 */
	t->inp_rtn	= rtn;
	t->inp_data	= data;

	/*--------------------------------------------------------------------
	 * check if rtn is specified & turn on if so
	 */
	if (t->inp_rtn != 0)
	{
		(t->inp_rtn)(t->inp_data, TERM_INP_MODE_OPEN, 0);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * term_screen_dev_inp_rtn () - get output-routine pointer
 * term_screen_dev_inp_data() - get output-data    pointer
 */
TERM_INP_RTN * term_screen_dev_inp_rtn (TERM_DATA *t)
{
	if (t == 0)
		return (0);

	return (t->inp_rtn);
}

void * term_screen_dev_inp_data (TERM_DATA *t)
{
	if (t == 0)
		return (0);

	return (t->inp_data);
}

/*------------------------------------------------------------------------
 * term_screen_dev_out() - set input routine to call
 */
int term_screen_dev_out (TERM_DATA *t, TERM_OUT_RTN *rtn, void *data)
{
	if (t == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * check if rtn was already specified & turn off if so
	 */
	if (t->out_rtn != 0)
	{
		(t->out_rtn)(t->out_data, TERM_OUT_MODE_CLOSE, 0, 0);
	}

	/*--------------------------------------------------------------------
	 * store pointers
	 */
	t->out_rtn	= rtn;
	t->out_data	= data;

	/*--------------------------------------------------------------------
	 * check if rtn is specified & turn on if so
	 */
	if (t->out_rtn != 0)
	{
		(t->out_rtn)(t->out_data, TERM_OUT_MODE_OPEN, 0, 0);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * term_screen_dev_out_rtn () - get input-routine pointer
 * term_screen_dev_out_data() - get input-data    pointer
 */
TERM_OUT_RTN * term_screen_dev_out_rtn (TERM_DATA *t)
{
	if (t == 0)
		return (0);

	return (t->out_rtn);
}

void * term_screen_dev_out_data (TERM_DATA *t)
{
	if (t == 0)
		return (0);

	return (t->out_data);
}

/*------------------------------------------------------------------------
 * term_screen_dev_mouse() - return pointer to MOUSE_STATUS struct
 */
MOUSE_STATUS * term_screen_dev_mouse (TERM_DATA *t)
{
	if (t == 0)
		return (0);

	return (&t->mouse_stat);
}
