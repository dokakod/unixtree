/*------------------------------------------------------------------------
 * key input routines
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * defines
 */
#define IS_ACHAR(c)	(((unsigned int)c & ~0xff) == 0)

/*------------------------------------------------------------------------
 * static routines
 */
static int tcap_xlate_key		(TERMINAL *tp, int key, int xlate, int keypad);

/*------------------------------------------------------------------------
 * tcap_kbd_get1c() - get one key from the terminal (internal)
 */
static int tcap_kbd_get1c (TERMINAL *tp, int interval)
{
	int c;

	/*--------------------------------------------------------------------
	 * read in a key
	 */
	c = term_kbd_get1c(tp->term, interval);

	/*--------------------------------------------------------------------
	 * debug print
	 */
	if (c != -1)
	{
		if (tp->scrn->debug_keys_fp != 0 && ! tp->scrn->debug_keys_text)
		{
			putc(c, tp->scrn->debug_keys_fp);
			fflush(tp->scrn->debug_keys_fp);
		}
	}

	return (c);
}

/*------------------------------------------------------------------------
 * tcap_kbd_read_key() - get a key from the terminal (internal)
 */
static int tcap_kbd_read_key (TERMINAL *tp, int interval, int mode, int xlate,
	int keypad)
{
	int c;
	int	i;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * if any chars in type_ahead buffer, just return first one
	 */
	if (tp->scrn->ta_count)
	{
		goto getkey;
	}

	/*--------------------------------------------------------------------
	 * No type_ahead. If no translation is wanted, just get the next key.
	 */
	if (! mode)
	{
		if (tp->scrn->fk_count)
		{
			c = *tp->scrn->fk_buf;
			strcpy((char *)(tp->scrn->fk_buf), (char *)(tp->scrn->fk_buf)+1);
			tp->scrn->fk_count--;
		}
		else
		{
			c = tcap_kbd_get1c(tp, interval);
		}
		goto gotkey;
	}

	/*--------------------------------------------------------------------
	 * Read key or fk string and translate
	 */
	if (tp->scrn->fk_count == 0)
	{
		/*----------------------------------------------------------------
		 * prime the queue
		 */
		c = tcap_kbd_get1c(tp, interval);
		if (c == -1)
			goto gotkey;

		/*----------------------------------------------------------------
		 * if this key is not a simple char, we have an already-translated
		 * key value
		 */
		if (! IS_ACHAR(c))
		{
			c = tcap_xlate_key(tp, c, TRUE, keypad);
			goto gotkey;
		}

		/*----------------------------------------------------------------
		 * store key in buffer
		 */
		tp->scrn->fk_buf[0] = (unsigned char)c;
		tp->scrn->fk_buf[1] = 0;
		tp->scrn->fk_count  = 1;
	}

	/*--------------------------------------------------------------------
	 * now collect more key values, until we we can determine whether we
	 * have a valid function key or not
	 */
	while (TRUE)
	{
		/*----------------------------------------------------------------
		 * check if current key buffer matches a known function-key
		 */
		i = tcap_fk_find(tp->tcap, tp->scrn->fk_buf);
		switch (i)
		{
		case -1:	/* maybe, get another char */

					c = tcap_kbd_get1c(tp, tp->scrn->cur_prefix_intvl);
					if (c != -1)
					{
						if (! IS_ACHAR(c))
						{
							tp->scrn->fk_count = 0;
							goto gotkey;
						}

						if (tp->scrn->fk_count < sizeof(tp->scrn->fk_buf)-1)
						{
							tp->scrn->fk_buf[tp->scrn->fk_count++] =
								(unsigned char)c;
							tp->scrn->fk_buf[tp->scrn->fk_count  ] = 0;
							continue;
						}
					}
					/* timeout or EOF */
					/*FALLTHROUGH*/

		case 0:		/* nope, feed first char in queue */

					/* check first if possible mouse string */

					c = tcap_mouse_get_input(tp, tp->scrn->fk_buf);
					switch (c)
					{
					default:
						tp->scrn->fk_count = 0;
						c = tcap_xlate_key(tp, c, xlate, keypad);
						goto gotkey;

					case -1:
						c = tcap_kbd_get1c(tp, tp->scrn->cur_prefix_intvl);
						if (c != -1)
						{
							tp->scrn->fk_buf[tp->scrn->fk_count++] =
								(unsigned char)c;
							tp->scrn->fk_buf[tp->scrn->fk_count  ] = 0;
							continue;
						}
						/*FALLTHROUGH*/

					case 0:
						break;
					}

					c = *tp->scrn->fk_buf;
					strcpy((char *)(tp->scrn->fk_buf),
						(char *)(tp->scrn->fk_buf)+1);
					tp->scrn->fk_count--;
					c = tcap_xlate_key(tp, c, xlate, keypad);
					goto gotkey;

		default:	/* yup, feed fk code */
					tp->scrn->fk_count = 0;
					c = tcap_xlate_key(tp, i, xlate, keypad);
					goto gotkey;
		}
	}

	/*--------------------------------------------------------------------
	 * If we get here we either have a simple char or a valid function-key.
	 * Push it onto our stack.
	 */
gotkey:
	tcap_kbd_enq_ta(tp, c, FALSE);

	/*--------------------------------------------------------------------
	 * Pull next char off the stack.
	 * We do this push/pull because we may have gotten a KEY_WINRESIZE
	 * key asyncronously.
	 */
getkey:
	c = tcap_kbd_deq_ta(tp);

	/*--------------------------------------------------------------------
	 * if key is KEY_MOUSE, get queued mouse info for it
	 */
	if (c == KEY_MOUSE)
		tcap_mouse_deq_event(tp);

	/*--------------------------------------------------------------------
	 * if a simple char, translate it from our inp xlate table
	 */
	if (IS_ACHAR(c))
		c = tp->tcap->inp_tbl[c];

	/*--------------------------------------------------------------------
	 * if we have a KEY_WINRESIZE, translate it to do any async processing
	 */
	if (c == KEY_WINRESIZE)
	{
		c = tcap_xlate_key(tp, c, TRUE, TRUE);
	}

	/*--------------------------------------------------------------------
	 * go back and do it again if told to
	 */
	switch (c)
	{
	case KEY_UNKNOWN:
		tcap_beep(tp);
		/*FALLTHROUGH*/

	case KEY_READ_AGAIN:
		return tcap_kbd_read_key(tp, interval, mode, xlate, keypad);
	}

	/*--------------------------------------------------------------------
	 * return found key value (may be -1)
	 */
	return (c);
}

/*------------------------------------------------------------------------
 * tcap_check_locks() - adjust key with numlock and/or capslock status
 */
static int tcap_check_locks (TERMINAL *tp, int key, int keypad)
{
	if (tp == 0)
		return (key);

	/*--------------------------------------------------------------------
	 * check if regular char
	 */
	if (IS_ACHAR(key))
	{
		if (isalpha(key))
		{
			if (tp->scrn->caps_lock && islower(key))
				return (toupper(key));
		}

		return (key);
	}

	/*--------------------------------------------------------------------
	 * special char
	 */
	if (keypad)
	{
		if (tp->scrn->num_lock)
		{
			switch (key)
			{
			case KEY_KP0:	key = '0';			break;
			case KEY_KP1:	key = '1';			break;
			case KEY_KP2:	key = '2';			break;
			case KEY_KP3:	key = '3';			break;
			case KEY_KP4:	key = '4';			break;
			case KEY_KP5:	key = '5';			break;
			case KEY_KP6:	key = '6';			break;
			case KEY_KP7:	key = '7';			break;
			case KEY_KP8:	key = '8';			break;
			case KEY_KP9:	key = '9';			break;
			case KEY_KPPERIOD:
							key = '.';			break;
			}
		}
		else
		{
			switch (key)
			{
			case KEY_KP0:	key = KEY_INSERT;	break;
			case KEY_KP1:	key = KEY_END;		break;
			case KEY_KP2:	key = KEY_DOWN;		break;
			case KEY_KP3:	key = KEY_PGDN;		break;
			case KEY_KP4:	key = KEY_LEFT;		break;
			case KEY_KP5:	key = KEY_MIDDLE;	break;
			case KEY_KP6:	key = KEY_RIGHT;	break;
			case KEY_KP7:	key = KEY_HOME;		break;
			case KEY_KP8:	key = KEY_UP;		break;
			case KEY_KP9:	key = KEY_PGUP;		break;
			case KEY_KPPERIOD:
							key = KEY_DELETE;	break;
			}
		}
	}

	return (key);
}

/*------------------------------------------------------------------------
 * tcap_xlate_key() - translate a key
 */
static int tcap_xlate_key (TERMINAL *tp, int key, int xlate, int keypad)
{
	int shift_down = FALSE;
	int ctrl_down  = FALSE;
	int alt_down   = FALSE;

	/*--------------------------------------------------------------------
	 * check if we should translate this key
	 */
	if (! xlate)
		return (key);

	/*--------------------------------------------------------------------
	 * check numlock / capslock
	 */
	key = tcap_check_locks(tp, key, keypad);

	/*--------------------------------------------------------------------
	 * check if mouse event
	 */
	switch (key)
	{
#if V_WINDOWS
	/*--------------------------------------------------------------------
	 * NT keys
	 */
	case MOUSE_LBD:
	case MOUSE_MBD:
	case MOUSE_RBD:

	case MOUSE_LBU:
	case MOUSE_MBU:
	case MOUSE_RBU:

	case MOUSE_LBDBL:
	case MOUSE_MBDBL:
	case MOUSE_RBDBL:

	case MOUSE_MOVE:
	case MOUSE_DRAG:

	case MOUSE_SCROLLUP:
	case MOUSE_SCROLLDN:
		tcap_mouse_set_event(tp, key);
		key = KEY_MOUSE;
		break;
#else
	/*--------------------------------------------------------------------
	 * SCO usemouse keys
	 */
	case MOUSE_LEFT:
		if (tcap_mouse_move_left(tp) == 0)
			key = KEY_MOUSE;
		else
			return (KEY_READ_AGAIN);
		break;

	case MOUSE_RIGHT:
		if (tcap_mouse_move_right(tp) == 0)
			key = KEY_MOUSE;
		else
			return (KEY_READ_AGAIN);
		break;

	case MOUSE_UP:
		if (tcap_mouse_move_up(tp) == 0)
			key = KEY_MOUSE;
		else
			return (KEY_READ_AGAIN);
		break;

	case MOUSE_DOWN:
		if (tcap_mouse_move_down(tp) == 0)
			key = KEY_MOUSE;
		else
			return (KEY_READ_AGAIN);
		break;

	case MOUSE_UL:
		if (tcap_mouse_move_ul(tp) == 0)
			key = KEY_MOUSE;
		else
			return (KEY_READ_AGAIN);
		break;

	case MOUSE_UR:
		if (tcap_mouse_move_ur(tp) == 0)
			key = KEY_MOUSE;
		else
			return (KEY_READ_AGAIN);
		break;

	case MOUSE_DL:
		if (tcap_mouse_move_dl(tp) == 0)
			key = KEY_MOUSE;
		else
			return (KEY_READ_AGAIN);
		break;

	case MOUSE_DR:
		if (tcap_mouse_move_dr(tp) == 0)
			key = KEY_MOUSE;
		else
			return (KEY_READ_AGAIN);
		break;

	case MOUSE_LBD:
		if (tcap_mouse_set_button(tp, BUTTON_LEFT, BUTTON_DOWN) == 0)
			key = KEY_MOUSE;
		else
			return (KEY_READ_AGAIN);
		break;

	case MOUSE_LBU:
		if (tcap_mouse_set_button(tp, BUTTON_LEFT, BUTTON_UP) == 0)
			key = KEY_MOUSE;
		else
			return (KEY_READ_AGAIN);
		break;

	case MOUSE_MBD:
		if (tcap_mouse_set_button(tp, tp->tcap->ints.nb == 2 ?
			BUTTON_RIGHT : BUTTON_MIDDLE, BUTTON_DOWN) == 0)
		{
			key = KEY_MOUSE;
		}
		else
		{
			return (KEY_READ_AGAIN);
		}
		break;

	case MOUSE_MBU:
		if (tcap_mouse_set_button(tp, tp->tcap->ints.nb == 2 ?
			BUTTON_RIGHT : BUTTON_MIDDLE, BUTTON_UP) == 0)
		{
			key = KEY_MOUSE;
		}
		else
		{
			return (KEY_READ_AGAIN);
		}
		break;

	case MOUSE_RBD:
		if (tcap_mouse_set_button(tp, BUTTON_RIGHT, BUTTON_DOWN) == 0)
			key = KEY_MOUSE;
		else
			return (KEY_READ_AGAIN);
		break;

	case MOUSE_RBU:
		if (tcap_mouse_set_button(tp, BUTTON_RIGHT, BUTTON_UP) == 0)
			key = KEY_MOUSE;
		else
			return (KEY_READ_AGAIN);
		break;
#endif
	}

	/*--------------------------------------------------------------------
	 * special case of window changed size
	 */
	if (key == KEY_WINRESIZE)
	{
		tcap_mouse_invalidate(tp);
		if (! tcap_winsize_check(tp))
			return (KEY_READ_AGAIN);
	}

	/*--------------------------------------------------------------------
	 * translate this key
	 */
	switch (key)
	{
	case KEY_KSHIFT:
	do_kshift:
		/*----------------------------------------------------------------
		 * get next key & shift to uppercase
		 */
		key = tcap_kbd_read_key(tp, -1, TRUE, xlate, keypad);

		switch (key)
		{
		case KEY_KSHIFT:	return (KEY_READ_AGAIN);
		case KEY_KCTRL:		goto do_kctrl;
		case KEY_KALT:		goto do_kalt;
		case KEY_KFUNC:		shift_down = TRUE;
							goto do_kfunc;

		case KEY_SHIFT_PRESS:
		case KEY_SHIFT_RELEASE:
		case KEY_CTRL_PRESS:
		case KEY_CTRL_RELEASE:
		case KEY_ALT_PRESS:
		case KEY_ALT_RELEASE:
							tcap_async_process(tp->scrn, key);
							goto do_kshift;

		case KEY_F1:		return (KEY_SHIFT_F1);
		case KEY_F2:		return (KEY_SHIFT_F2);
		case KEY_F3:		return (KEY_SHIFT_F3);
		case KEY_F4:		return (KEY_SHIFT_F4);
		case KEY_F5:		return (KEY_SHIFT_F5);
		case KEY_F6:		return (KEY_SHIFT_F6);
		case KEY_F7:		return (KEY_SHIFT_F7);
		case KEY_F8:		return (KEY_SHIFT_F8);
		case KEY_F9:		return (KEY_SHIFT_F9);
		case KEY_F10:		return (KEY_SHIFT_F10);
		case KEY_F11:		return (KEY_SHIFT_F11);
		case KEY_F12:		return (KEY_SHIFT_F12);
		case KEY_F13:		return (KEY_SHIFT_F13);
		case KEY_F14:		return (KEY_SHIFT_F14);
		case KEY_F15:		return (KEY_SHIFT_F15);
		case KEY_F16:		return (KEY_SHIFT_F16);

		case KEY_UP:		return (KEY_SHIFT_UP);
		case KEY_DOWN:		return (KEY_SHIFT_DOWN);
		case KEY_LEFT:		return (KEY_SHIFT_LEFT);
		case KEY_RIGHT:		return (KEY_SHIFT_RIGHT);
		case KEY_HOME:		return (KEY_SHIFT_HOME);
		case KEY_END:		return (KEY_SHIFT_END);
		case KEY_PGUP:		return (KEY_SHIFT_PGUP);
		case KEY_PGDN:		return (KEY_SHIFT_PGDN);
		case KEY_MIDDLE:	return (KEY_SHIFT_MIDDLE);

		case KEY_USER1:		return (KEY_SHIFT_USER1);
		case KEY_USER2:		return (KEY_SHIFT_USER2);
		case KEY_USER3:		return (KEY_SHIFT_USER3);
		case KEY_USER4:		return (KEY_SHIFT_USER4);
		case KEY_USER5:		return (KEY_SHIFT_USER5);

		case KEY_BS:		return (KEY_SHIFT_BS);
		case KEY_TAB:		return (KEY_SHIFT_TAB);
		case KEY_RETURN:	return (KEY_SHIFT_RETURN);

		case '`':			return ('~');
		case '1':			return ('!');
		case '2':			return ('@');
		case '3':			return ('#');
		case '4':			return ('$');
		case '5':			return ('%');
		case '6':			return ('^');
		case '7':			return ('&');
		case '8':			return ('*');
		case '9':			return ('(');
		case '0':			return (')');
		case '-':			return ('_');
		case '=':			return ('+');
		case '[':			return ('{');
		case ']':			return ('}');
		case '\\':			return ('|');
		case ';':			return (':');
		case '\'':			return ('"');
		case ',':			return ('<');
		case '.':			return ('>');
		case '/':			return ('?');
		}

		/*----------------------------------------------------------------
		 *	Convert lower-case letter to upper-case
		 */
		if (IS_ACHAR(key) && isalpha(key))
		{
			if (islower(key))
				return (toupper(key));
			else
				return (key);
		}

		/*----------------------------------------------------------------
		 * nothing we know, ignore shift like a kbd would
		 */
		return (key);

	case KEY_KCTRL:
	do_kctrl:
		/*----------------------------------------------------------------
		 * get next key & treat as a control key
		 */
		key = tcap_kbd_read_key(tp, -1, TRUE, xlate, keypad);

		switch (key)
		{
		case KEY_KSHIFT:	goto do_kshift;
		case KEY_KCTRL:		return (KEY_READ_AGAIN);
		case KEY_KALT:		goto do_kalt;
		case KEY_KFUNC:		ctrl_down = TRUE;
							goto do_kfunc;

		case KEY_SHIFT_PRESS:
		case KEY_SHIFT_RELEASE:
		case KEY_CTRL_PRESS:
		case KEY_CTRL_RELEASE:
		case KEY_ALT_PRESS:
		case KEY_ALT_RELEASE:
							tcap_async_process(tp->scrn, key);
							goto do_kctrl;

		case KEY_F1:		return (KEY_CTRL_F1);
		case KEY_F2:		return (KEY_CTRL_F2);
		case KEY_F3:		return (KEY_CTRL_F3);
		case KEY_F4:		return (KEY_CTRL_F4);
		case KEY_F5:		return (KEY_CTRL_F5);
		case KEY_F6:		return (KEY_CTRL_F6);
		case KEY_F7:		return (KEY_CTRL_F7);
		case KEY_F8:		return (KEY_CTRL_F8);
		case KEY_F9:		return (KEY_CTRL_F9);
		case KEY_F10:		return (KEY_CTRL_F10);
		case KEY_F11:		return (KEY_CTRL_F11);
		case KEY_F12:		return (KEY_CTRL_F12);
		case KEY_F13:		return (KEY_CTRL_F13);
		case KEY_F14:		return (KEY_CTRL_F14);
		case KEY_F15:		return (KEY_CTRL_F15);
		case KEY_F16:		return (KEY_CTRL_F16);

		case KEY_UP:		return (KEY_CTRL_UP);
		case KEY_DOWN:		return (KEY_CTRL_DOWN);
		case KEY_LEFT:		return (KEY_CTRL_LEFT);
		case KEY_RIGHT:		return (KEY_CTRL_RIGHT);
		case KEY_HOME:		return (KEY_CTRL_HOME);
		case KEY_END:		return (KEY_CTRL_END);
		case KEY_PGUP:		return (KEY_CTRL_PGUP);
		case KEY_PGDN:		return (KEY_CTRL_PGDN);
		case KEY_MIDDLE:	return (KEY_CTRL_MIDDLE);

		case KEY_USER1:		return (KEY_CTRL_USER1);
		case KEY_USER2:		return (KEY_CTRL_USER2);
		case KEY_USER3:		return (KEY_CTRL_USER3);
		case KEY_USER4:		return (KEY_CTRL_USER4);
		case KEY_USER5:		return (KEY_CTRL_USER5);

		case KEY_BS:		return (KEY_CTRL_BS);
		case KEY_TAB:		return (KEY_CTRL_TAB);
		case KEY_RETURN:	return (KEY_CTRL_RETURN);

		case '@':			return (0x00);
		case '[':			return (0x1b);
		case '\\':			return (0x1c);
		case ']':			return (0x1d);
		case '^':			return (0x1e);
		case '_':			return (0x1f);
		case '?':			return (KEY_DELETE);
		}

		/*----------------------------------------------------------------
		 * Convert letter to ctrl-letter
		 */
		if (IS_ACHAR(key) && isalpha(key))
			return (toupper(key)-'A'+1);

		return (KEY_UNKNOWN);

	case KEY_KALT:
	do_kalt:
		/*----------------------------------------------------------------
		 * get next key & treat as an alt key
		 */
		key = tcap_kbd_read_key(tp, -1, TRUE, xlate, FALSE);

		switch (key)
		{
		case KEY_KSHIFT:	goto do_kshift;
		case KEY_KCTRL:		goto do_kctrl;
		case KEY_KALT:		return (KEY_READ_AGAIN);
		case KEY_KFUNC:		alt_down = TRUE;
							goto do_kfunc;

		case KEY_SHIFT_PRESS:
		case KEY_SHIFT_RELEASE:
		case KEY_CTRL_PRESS:
		case KEY_CTRL_RELEASE:
		case KEY_ALT_PRESS:
		case KEY_ALT_RELEASE:
							tcap_async_process(tp->scrn, key);
							goto do_kalt;

		case KEY_F1:		return (KEY_ALT_F1);
		case KEY_F2:		return (KEY_ALT_F2);
		case KEY_F3:		return (KEY_ALT_F3);
		case KEY_F4:		return (KEY_ALT_F4);
		case KEY_F5:		return (KEY_ALT_F5);
		case KEY_F6:		return (KEY_ALT_F6);
		case KEY_F7:		return (KEY_ALT_F7);
		case KEY_F8:		return (KEY_ALT_F8);
		case KEY_F9:		return (KEY_ALT_F9);
		case KEY_F10:		return (KEY_ALT_F10);
		case KEY_F11:		return (KEY_ALT_F11);
		case KEY_F12:		return (KEY_ALT_F12);
		case KEY_F13:		return (KEY_ALT_F13);
		case KEY_F14:		return (KEY_ALT_F14);
		case KEY_F15:		return (KEY_ALT_F15);
		case KEY_F16:		return (KEY_ALT_F16);

#if 0
		case KEY_UP:		return (KEY_ALT_UP);
		case KEY_DOWN:		return (KEY_ALT_DOWN);
		case KEY_LEFT:		return (KEY_ALT_LEFT);
		case KEY_RIGHT:		return (KEY_ALT_RIGHT);
		case KEY_HOME:		return (KEY_ALT_HOME);
		case KEY_END:		return (KEY_ALT_END);
		case KEY_PGUP:		return (KEY_ALT_PGUP);
		case KEY_PGDN:		return (KEY_ALT_PGDN);
		case KEY_MIDDLE:	return (KEY_ALT_MIDDLE);
#endif

		case KEY_USER1:		return (KEY_ALT_USER1);
		case KEY_USER2:		return (KEY_ALT_USER2);
		case KEY_USER3:		return (KEY_ALT_USER3);
		case KEY_USER4:		return (KEY_ALT_USER4);
		case KEY_USER5:		return (KEY_ALT_USER5);

		case KEY_BS:		return (KEY_ALT_BS);
		case KEY_TAB:		return (KEY_ALT_TAB);
		case KEY_RETURN:	return (KEY_ALT_RETURN);
		}

		/*----------------------------------------------------------------
		 * Convert letter to alt-letter
		 */
		if (IS_ACHAR(key) && isalpha(key))
		{
			if (isupper(key))
				return (MASK_ALT | key);
			else
				return (MASK_ALT | toupper(key));
		}

		/*----------------------------------------------------------------
		 * Convert number to alt-number
		 */
		if (IS_ACHAR(key) && isdigit(key))
		{
			int alt_num;

			/*------------------------------------------------------------
			 * first digit
			 */
			alt_num = (key - '0');

			/*------------------------------------------------------------
			 * second digit
			 */
			while (TRUE)
			{
				key = tcap_kbd_read_key(tp, -1, TRUE, FALSE, keypad);

				/*--------------------------------------------------------
				 * ignore any more ALT-type keys
				 */
				if (key == KEY_KALT)
				{
					continue;
				}
				else if (key == KEY_KFUNC)
				{
					key = tcap_kbd_read_key(tp, -1, TRUE, xlate, keypad);
					if (key == 'a')
						continue;
					else
						return (KEY_UNKNOWN);
				}
				else
				{
					break;
				}
			}

			if (key < '0' || key > '9')
				return (KEY_UNKNOWN);

			alt_num = (10 * alt_num) + (key - '0');

			/*------------------------------------------------------------
			 * third digit
			 */
			while (TRUE)
			{
				key = tcap_kbd_read_key(tp, -1, TRUE, FALSE, keypad);

				/*--------------------------------------------------------
				 * ignore any more ALT-type keys
				 */
				if (key == KEY_KALT)
					continue;
				else if (key == KEY_KFUNC)
				{
					key = tcap_kbd_read_key(tp, -1, TRUE, xlate, keypad);
					if (key == 'a')
						continue;
					else
						return (KEY_UNKNOWN);
				}
				else
					break;
			}

			if (key < '0' || key > '9')
				return (KEY_UNKNOWN);

			alt_num = (10 * alt_num) + (key - '0');

			alt_num %= 0xff;

			return (alt_num);
		}

		/*----------------------------------------------------------------
		 * check for kalt-3 keypad numbers
		 */
		if (key >= KEY_KP0 && key <= KEY_KP9)
		{
			int alt_num;

			/*------------------------------------------------------------
			 * first digit
			 */
			alt_num = (key - KEY_KP0);

			/*------------------------------------------------------------
			 * second digit
			 */
			while (TRUE)
			{
				key = tcap_kbd_read_key(tp, -1, TRUE, FALSE, keypad);

				/*--------------------------------------------------------
				 * ignore any more ALT-type keys
				 */
				if (key == KEY_KALT)
				{
					continue;
				}
				else if (key == KEY_KFUNC)
				{
					key = tcap_kbd_read_key(tp, -1, TRUE, xlate, keypad);
					if (key == 'a')
						continue;
					else
						return (KEY_UNKNOWN);
				}
				else
				{
					break;
				}
			}

			if (key < KEY_KP0 || key > KEY_KP9)
				return (KEY_UNKNOWN);

			alt_num = (10 * alt_num) + (key - KEY_KP0);

			/*------------------------------------------------------------
			 * third digit
			 */
			while (TRUE)
			{
				key = tcap_kbd_read_key(tp, -1, TRUE, FALSE, keypad);

				/*--------------------------------------------------------
				 * ignore any more ALT-type keys
				 */
				if (key == KEY_KALT)
				{
					continue;
				}
				else if (key == KEY_KFUNC)
				{
					key = tcap_kbd_read_key(tp, -1, TRUE, xlate, keypad);
					if (key == 'a')
						continue;
					else
						return (KEY_UNKNOWN);
				}
				else
					break;
			}

			if (key < KEY_KP0 || key > KEY_KP9)
				return (KEY_UNKNOWN);

			alt_num = (10 * alt_num) + (key - KEY_KP0);

			alt_num %= 0xff;

			return (alt_num);
		}

		return (KEY_UNKNOWN);

	case KEY_KFUNC:
	do_kfunc:
		/*----------------------------------------------------------------
		 * get next key & treat as a KFUNC key
		 */
		key = tcap_kbd_read_key(tp, -1, TRUE, xlate, keypad);
		key = tolower(key);

		switch (key)
		{
		case KEY_KSHIFT:	goto do_kshift;
		case KEY_KCTRL:		goto do_kctrl;
		case KEY_KALT:		goto do_kalt;
		case KEY_KFUNC:		return (KEY_READ_AGAIN);

		case KEY_SHIFT_PRESS:
		case KEY_SHIFT_RELEASE:
		case KEY_CTRL_PRESS:
		case KEY_CTRL_RELEASE:
		case KEY_ALT_PRESS:
		case KEY_ALT_RELEASE:
							tcap_async_process(tp->scrn, key);
							goto do_kfunc;

		case '1':	key = KEY_F1;		break;
		case '2':	key = KEY_F2;		break;
		case '3':	key = KEY_F3;		break;
		case '4':	key = KEY_F4;		break;
		case '5':	key = KEY_F5;		break;
		case '6':	key = KEY_F6;		break;
		case '7':	key = KEY_F7;		break;
		case '8':	key = KEY_F8;		break;
		case '9':	key = KEY_F9;		break;
		case '0':	key = KEY_F10;		break;
		case '-':	key = KEY_F11;		break;
		case '=':	key = KEY_F12;		break;

		case 'u':	key = KEY_UP;		break;
		case 'd':	key = KEY_DOWN;		break;
		case 'l':	key = KEY_LEFT;		break;
		case 'r':	key = KEY_RIGHT;	break;

		case 'h':	key = KEY_HOME;		break;
		case 'e':	key = KEY_END;		break;
		case 'p':	key = KEY_PGUP;		break;
		case 'n':	key = KEY_PGDN;		break;

		case 'b':	key = KEY_BS;		break;
		case 't':	key = KEY_TAB;		break;

		case 'i':	return (KEY_INSERT);
		case 'x':	return (KEY_ESCAPE);

		case 'w':	return tcap_xlate_key(tp, KEY_REFRESH,      xlate, keypad);
		case 'q':	return tcap_xlate_key(tp, KEY_PRINT_SCREEN, xlate, keypad);
		case 'z':	return tcap_xlate_key(tp, KEY_SNAP,         xlate, keypad);
		case 'k':	return tcap_xlate_key(tp, KEY_CAPS_LOCK,    xlate, keypad);
		case 'm':	return tcap_xlate_key(tp, KEY_NUM_LOCK,     xlate, keypad);
		case '?':	return tcap_xlate_key(tp, KEY_HELP,         xlate, keypad);

		case '~':	tcap_xlate_key(tp, KEY_WINRESIZE, xlate, keypad);
					tcap_async_process(tp->scrn, KEY_WINRESIZE);
					return (KEY_READ_AGAIN);

		case 's':	goto do_kshift;
		case 'a':	goto do_kalt;
		case 'c':	goto do_kctrl;

		case '!':	tcap_exec_shell(tp);
					return (KEY_READ_AGAIN);

		case '*':	tcap_cexit(tp, TRUE);
					exit(0);
					break;

		case '<':
					tcap_kbd_debug(tp, 0, TRUE, FALSE);
					return (KEY_READ_AGAIN);

		case '>':
					tcap_kbd_debug(tp, 0, FALSE, FALSE);
					return (KEY_READ_AGAIN);

		case '(':
					tcap_kbd_debug(tp, 0, TRUE, TRUE);
					return (KEY_READ_AGAIN);

		case ')':
					tcap_kbd_debug(tp, 0, FALSE, TRUE);
					return (KEY_READ_AGAIN);

		case '[':
					tcap_out_debug(tp, 0, TRUE, FALSE);
					return (KEY_READ_AGAIN);

		case ']':
					tcap_out_debug(tp, 0, FALSE, FALSE);
					return (KEY_READ_AGAIN);

		case '{':
					tcap_out_debug(tp, 0, TRUE, TRUE);
					return (KEY_READ_AGAIN);

		case '}':
					tcap_out_debug(tp, 0, FALSE, TRUE);
					return (KEY_READ_AGAIN);

		case '^':	key = tcap_kbd_read_key(tp, -1, TRUE, xlate, keypad);
					return (key & 0x1f);

		default:	return (KEY_UNKNOWN);
		}

		/*----------------------------------------------------------------
		 * add any other current modes
		 */
		if (shift_down)
			key |= MASK_SHIFT;
		else if (ctrl_down)
			key |= MASK_CTRL;
		else if (alt_down)
			key |= MASK_ALT;

		return (key);

	/*--------------------------------------------------------------------
	 * Note that if the user specifies these keys as async
	 * keys, then the following processing will not be done.
	 */
	case KEY_NUM_LOCK:
		tp->scrn->num_lock		= ! tp->scrn->num_lock;
		return (KEY_READ_AGAIN);

	case KEY_CAPS_LOCK:
		tp->scrn->caps_lock		= ! tp->scrn->caps_lock;
		return (KEY_READ_AGAIN);

	case KEY_SCROLL_LOCK:
		tp->scrn->scroll_lock	= ! tp->scrn->scroll_lock;
		return (KEY_READ_AGAIN);
	}

	return (key);
}

/*------------------------------------------------------------------------
 * tcap_kbd_get_key() - get a key from the terminal
 */
int tcap_kbd_get_key (TERMINAL *tp, int interval, int mode, int xlate,
	int keypad)
{
	int c;

	/*----------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0)
		return (-1);

	/*----------------------------------------------------------------
	 * now read in one key stroke to return to the caller
	 */
#if 0	/* don't lock reads - only writes */
	TERMINAL_LOCK(tp);
#endif
	{
		/*----------------------------------------------------------------
		 * flush all output
		 */
		tcap_outflush(tp);

		/*----------------------------------------------------------------
		 * now get a key
		 */
		while (TRUE)
		{
			int async;

			/*------------------------------------------------------------
			 * read in next key
			 */
			c = tcap_kbd_read_key(tp, interval, mode, xlate, keypad);
			if (c == -1)
				break;

			/*------------------------------------------------------------
			 * check if async key
			 */
			async = tcap_async_check(tp->scrn, c);

			/*------------------------------------------------------------
			 * debug printout
			 */
			if (c != -1 &&
			    tp->scrn->debug_keys_fp != 0 &&
			    tp->scrn->debug_keys_text)
			{
				char			kbuf[16];
				const char *	kn;

				if (async)
				{
					fputs("{async} ", tp->scrn->debug_keys_fp);
				}

				if (IS_ACHAR(c))
					kn = tcap_get_char_name(c, kbuf);
				else
					kn = tcap_get_key_name(c, kbuf);

				fputs(kn, tp->scrn->debug_keys_fp);

				if (c == KEY_MOUSE)
				{
					int k = tcap_mouse_get_event(tp);
					int x = tcap_mouse_get_x(tp);
					int y = tcap_mouse_get_y(tp);
					int b = tcap_mouse_get_buttons(tp);

					kn = tcap_get_key_name(k, kbuf);
					fprintf(tp->scrn->debug_keys_fp,
						" {%s: x=%2d y=%2d b=%d}",
						kn, x, y, b);
				}
				else if (c == KEY_WINRESIZE)
				{
					fprintf(tp->scrn->debug_keys_fp,
						" {%d,%d}",
						tp->tcap->ints.maxrows,
						tp->tcap->ints.maxcols);
				}

				putc('\n', tp->scrn->debug_keys_fp);
				fflush(tp->scrn->debug_keys_fp);
			}

			/*------------------------------------------------------------
			 * process if async key
			 */
			if (async)
			{
				c = tcap_async_process(tp->scrn, c);
				if (c <= 0)
					continue;
			}

			break;
		}
	}
#if 0	/* don't lock reads - only writes */
	TERMINAL_UNLOCK(tp);
#endif

	return (c);
}

/*------------------------------------------------------------------------
 * tcap_kbd_chk_key() - check if keypress pending
 *
 *		mode     = TRUE   translate pseudo-keys
 *		         = FALSE  no translation of pseudo-keys
 *
 *		returns  = TRUE  if at least 1 keypress is pending
 *			     = FALSE if no keypress is pending
 */
int tcap_kbd_chk_key (TERMINAL *tp, int mode)
{
	int k;
	int rc;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0)
		return (FALSE);

	/*--------------------------------------------------------------------
	 * now check if anything pending
	 */
#if 0	/* don't lock reads - only writes */
	TERMINAL_LOCK(tp);
#endif
	{
		/*----------------------------------------------------------------
		 * if any key is queued, then we got one
		 */
		if (tp->scrn->ta_count)
		{
			rc = TRUE;
		}
		else
		{
			/*------------------------------------------------------------
			 * query keyboard with no wait
			 */
			k = tcap_kbd_read_key(tp, 0, mode, FALSE, FALSE);
			if (k <= 0)
			{
				rc = FALSE;
			}
			else
			{
				/*--------------------------------------------------------
				 * we got a key: queue it up at end of queue
				 */
				tcap_kbd_enq_ta(tp, k, FALSE);
				rc = TRUE;
			}
		}
	}
#if 0	/* don't lock reads - only writes */
	TERMINAL_UNLOCK(tp);
#endif

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_kbd_enq_ta() - push key k onto type-ahead buffer.
 *
 *		f        = TRUE  push k onto front of buffer
 *		         = FALSE push k onto end of buffer
 *
 *		returns  = number of chars in type-ahead buffer
 */
int tcap_kbd_enq_ta (TERMINAL *tp, int k, int f)
{
	int i;
	int cnt;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * add key to queue if real key
	 */
	TERMINAL_LOCK(tp);
	{
		if (k > 0)
		{
			/*------------------------------------------------------------
			 * if buffer overrun, silently drop it on the floor
			 */
			if (tp->scrn->ta_count < (int)sizeof(tp->scrn->ta_buf))
			{
				/*--------------------------------------------------------
				 * push onto front or add to end
				 */
				if (f)
				{
					/*----------------------------------------------------
					 * add to front
					 */
					for (i=tp->scrn->ta_count; i; i--)
						tp->scrn->ta_buf[i] = tp->scrn->ta_buf[i-1];
	
					tp->scrn->ta_buf[0] = k;
				}
				else
				{
					/*----------------------------------------------------
					 * add to end
					 */
					tp->scrn->ta_buf[tp->scrn->ta_count] = k;
				}

				/*--------------------------------------------------------
				 * bump count
				 */
				tp->scrn->ta_count++;
			}
		}

		cnt = tp->scrn->ta_count;
	}
	TERMINAL_UNLOCK(tp);

	return (cnt);
}

/*------------------------------------------------------------------------
 * tcap_kbd_deq_ta() - get key from type-ahead buffer
 */
int tcap_kbd_deq_ta (TERMINAL *tp)
{
	int c;
	int i;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		if (tp->scrn->ta_count == 0)
		{
			c = -1;
		}
		else
		{
			c = tp->scrn->ta_buf[0];
			for (i=1; i<tp->scrn->ta_count; i++)
				tp->scrn->ta_buf[i-1] = tp->scrn->ta_buf[i];
			tp->scrn->ta_count--;
		}
	}
	TERMINAL_UNLOCK(tp);

	return (c);
}

/*------------------------------------------------------------------------
 * tcap_kbd_clr_ta() - clear type-ahead buffer and any pending input.
 */
void tcap_kbd_clr_ta (TERMINAL *tp)
{
	if (tp != 0)
	{
		TERMINAL_LOCK(tp);
		{
			tp->scrn->ta_count = 0;
			tcap_flush_inp(tp);
		}
		TERMINAL_UNLOCK(tp);
	}
}

/*------------------------------------------------------------------------
 * tcap_kbd_set_timeout() - set current timeout state
 */
int tcap_kbd_set_timeout (TERMINAL *tp, int bf)
{
	int old_bf;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		old_bf = (tp->scrn->cur_prefix_intvl != 0);

		if (bf)
			tp->scrn->cur_prefix_intvl = tp->tcap->ints.prefix_intvl;
		else
			tp->scrn->cur_prefix_intvl = 0;
	}
	TERMINAL_UNLOCK(tp);

	return (old_bf);
}

/*------------------------------------------------------------------------
 * tcap_kbd_get_timeout() - get current timeout state
 */
int tcap_kbd_get_timeout (const TERMINAL *tp)
{
	if (tp == 0)
		return (FALSE);

	return (tp->scrn->cur_prefix_intvl != 0);
}

/*------------------------------------------------------------------------
 * tcap_kbd_debug() - debug key presses
 */
int tcap_kbd_debug (TERMINAL *tp, const char *path, int flag, int text)
{
	int rc;

	if (tp == 0)
		return (FALSE);

	TERMINAL_LOCK(tp);
	{
		tp->scrn->debug_keys_text = text;

		if (path == 0)
		{
			if (*tp->scrn->debug_keys_path == 0)
				path = text ? TCAP_KEYS_LOG_TXT : TCAP_KEYS_LOG_BIN;
			else
				path = tp->scrn->debug_keys_path;
		}
		strcpy(tp->scrn->debug_keys_path, path);

		if (flag)
		{
			if (tp->scrn->debug_keys_fp == 0)
			{
				tp->scrn->debug_keys_fp = fopen(path, "w");

				if (tp->scrn->mode)
					tcap_set_window_title(tp, 0);
			}
		}
		else
		{
			if (tp->scrn->debug_keys_fp != 0)
			{
				fclose(tp->scrn->debug_keys_fp);
				tp->scrn->debug_keys_fp = 0;

				if (tp->scrn->mode)
					tcap_set_window_title(tp, 0);
			}
		}

		rc = (tp->scrn->debug_keys_fp != 0);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_kbd_add_key() - add a key definition
 */
int tcap_kbd_add_key (TERMINAL *tp, const char *str, int code)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = tcap_fk_addkey(tp->tcap, str, code, FALSE);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_kbd_add_async() - add an async key
 */
int tcap_kbd_add_async (TERMINAL *tp, int key, ASYNC_RTN *rtn, void *data)
{
	int rc;

	if (tp == 0)
		return (-1);

	TERMINAL_LOCK(tp);
	{
		rc = tcap_async_add(tp->scrn, key, rtn, data);
	}
	TERMINAL_UNLOCK(tp);

	return (rc);
}
