/*------------------------------------------------------------------------
 * X event routines
 */
#include "xvtcommon.h"
#include "xvtx.h"

/*------------------------------------------------------------------------
 * decode keyboard state
 */
static int xvt_w_event_kbd_get_state (TERMWIN *tw, char *keys)
{
	int	kbd_state	= KEY_MASK_NONE;
	int i;

	for (i=0; i<32; i++)
	{
		if (keys[i] != 0)
		{
			int c = (unsigned char)(keys[i]);
			int j;

			for (j=0; j<8; j++)
			{
				int b = c & (0x01 << j);

				if (b != 0)
				{
					int		m = (i * 8) + j;
					KeySym	k = XKeycodeToKeysym(tw->X->display, m, 0);

					switch (k)
					{
					case XK_Shift_L:
					case XK_Shift_R:	kbd_state |= KEY_MASK_SHFT;	break;

					case XK_Control_L:
					case XK_Control_R:	kbd_state |= KEY_MASK_CTRL;	break;

					case XK_Meta_L:
					case XK_Meta_R:
					case XK_Alt_L:
					case XK_Alt_R:		kbd_state |= KEY_MASK_META;	break;

					case XK_Num_Lock:	kbd_state |= KEY_MASK_NUML;	break;
					}
				}
			}
		}
	}

	return (kbd_state);
}

/*------------------------------------------------------------------------
 * get current state of keyboard prefix keys
 */
int xvt_w_event_kbd_query (TERMWIN *tw)
{
	char	keys[32];
	int		kbd_state;

	/*--------------------------------------------------------------------
	 * get key states
	 */
	XQueryKeymap(tw->X->display, keys);

	/*--------------------------------------------------------------------
	 * now decode it
	 */
	kbd_state = xvt_w_event_kbd_get_state(tw, keys);

	return (kbd_state);
}

/*------------------------------------------------------------------------
 * translate a key event into a keysym & buffer
 */
static int xvt_w_event_xlate_key (XKeyEvent *xe, int *kp)
{
	unsigned char	buffer[64];
	KeySym			keysym;
	int				nbytes;

	/*--------------------------------------------------------------------
	 * do initial lookup of key event
	 */
	nbytes = XLookupString(xe, (char *)buffer, sizeof(buffer), &keysym, 0);
	if (nbytes < 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * make sure buffer ends with a NULL
	 */
	buffer[nbytes] = 0;

	/*--------------------------------------------------------------------
	 * make sure keysym is in proper range
	 */
	keysym &= 0xffff;

	/*--------------------------------------------------------------------
	 * It seems that (Sun at least) sends wrong info for F11 & F12,
	 * so we will try to fix it here.
	 *
	 * Also, Linux (& maybe others) mix up the DEL & BS keys, so we will
	 * try to make the BS & DEL key correct here.
	 */
	switch (keysym)
	{
	case 0xff10:
		nbytes = 0;
		keysym = XK_F11;
		break;

	case 0xff11:
		nbytes = 0;
		keysym = XK_F12;
		break;

	case XK_BackSpace:
		nbytes = 1;
		buffer[0] = 0x08;
		buffer[1] = 0;
		break;

#if defined(XK_KP_Add)
	case XK_KP_Add:
#endif
#if defined(XK_KP_Subtract)
	case XK_KP_Subtract:
#endif
#if defined(XK_KP_Multiply)
	case XK_KP_Multiply:
#endif
#if defined(XK_KP_Divide)
	case XK_KP_Divide:
#endif
	case XK_Delete:
		nbytes = 0;
		break;
	}

	/*--------------------------------------------------------------------
	 * store keysym if requested
	 */
	if (kp != 0)
		*kp = keysym;

	/*--------------------------------------------------------------------
	 * check if translation is in buffer
	 */
	if (nbytes > 0)
	{
		return (buffer[0]);
	}

	/*--------------------------------------------------------------------
	 * translate special key to our internal representation
	 */
	switch (keysym)
	{
		/*----------------------------------------------------------------
		 * function keys
		 */
		case XK_F1:				return (XVT_KEY_FN_01);
		case XK_F2:				return (XVT_KEY_FN_02);
		case XK_F3:				return (XVT_KEY_FN_03);
		case XK_F4:				return (XVT_KEY_FN_04);
		case XK_F5:				return (XVT_KEY_FN_05);
		case XK_F6:				return (XVT_KEY_FN_06);
		case XK_F7:				return (XVT_KEY_FN_07);
		case XK_F8:				return (XVT_KEY_FN_08);
		case XK_F9:				return (XVT_KEY_FN_09);
		case XK_F10:			return (XVT_KEY_FN_10);
		case XK_F11:			return (XVT_KEY_FN_11);
		case XK_F12:			return (XVT_KEY_FN_12);
		case XK_F13:			return (XVT_KEY_FN_13);
		case XK_F14:			return (XVT_KEY_FN_14);
		case XK_F15:			return (XVT_KEY_FN_15);
		case XK_F16:			return (XVT_KEY_FN_16);

		/*----------------------------------------------------------------
		 * keypad keys
		 */
		case XK_Insert:			return (XVT_KEY_CS_0);
#if defined(XK_KP_Insert)
		case XK_KP_Insert:		return (XVT_KEY_KP_0);
#endif
		case XK_KP_0:			return (XVT_KEY_NL_0);

		case XK_End:			return (XVT_KEY_CS_1);
#if defined(XK_KP_End)
		case XK_KP_End:			return (XVT_KEY_KP_1);
#endif
		case XK_R13:			return (XVT_KEY_KP_1);
		case XK_KP_1:			return (XVT_KEY_NL_1);

		case XK_Down:			return (XVT_KEY_CS_2);
#if defined(XK_KP_Down)
		case XK_KP_Down:		return (XVT_KEY_KP_2);
#endif
		case XK_R14:			return (XVT_KEY_KP_2);
		case XK_KP_2:			return (XVT_KEY_NL_2);

#if defined(XK_Page_Down)
		case XK_Page_Down:		return (XVT_KEY_CS_3);
#endif
#if defined(XK_KP_Page_Down)
		case XK_KP_Page_Down:	return (XVT_KEY_KP_3);
#endif
		case XK_R15:			return (XVT_KEY_KP_3);
		case XK_KP_3:			return (XVT_KEY_NL_3);

		case XK_Left:			return (XVT_KEY_CS_4);
#if defined(XK_KP_Left)
		case XK_KP_Left:		return (XVT_KEY_KP_4);
#endif
		case XK_R10:			return (XVT_KEY_KP_4);
		case XK_KP_4:			return (XVT_KEY_NL_4);

#if defined(XK_KP_Begin)
		case XK_KP_Begin:		return (XVT_KEY_KP_5);
#endif
		case XK_R11:			return (XVT_KEY_KP_5);
		case XK_KP_5:			return (XVT_KEY_NL_5);

		case XK_Right:			return (XVT_KEY_CS_6);
#if defined(XK_KP_Right)
		case XK_KP_Right:		return (XVT_KEY_KP_6);
#endif
		case XK_R12:			return (XVT_KEY_KP_6);
		case XK_KP_6:			return (XVT_KEY_NL_6);
	
		case XK_Home:			return (XVT_KEY_CS_7);
#if defined(XK_KP_Home)
		case XK_KP_Home:		return (XVT_KEY_KP_7);
#endif
		case XK_R7:				return (XVT_KEY_KP_7);
		case XK_KP_7:			return (XVT_KEY_NL_7);

		case XK_Up:				return (XVT_KEY_CS_8);
#if defined(XK_KP_Up)
		case XK_KP_Up:			return (XVT_KEY_KP_8);
#endif
		case XK_R8:				return (XVT_KEY_KP_8);
		case XK_KP_8:			return (XVT_KEY_NL_8);

#if defined(XK_Page_Up)
		case XK_Page_Up:		return (XVT_KEY_CS_9);
#endif
#if defined(XK_KP_Page_up)
		case XK_KP_Page_Up:		return (XVT_KEY_KP_9);
#endif
		case XK_R9:				return (XVT_KEY_KP_9);
		case XK_KP_9:			return (XVT_KEY_KP_9);

		case XK_Delete:			return (XVT_KEY_CS_DELETE);
#if defined(XK_KP_Delete)
		case XK_KP_Delete:		return (XVT_KEY_KP_DELETE);
#endif
		case XK_KP_Decimal:		return (XVT_KEY_NL_DECIMAL);

#if defined(XK_KP_Subtract)
		case XK_KP_Subtract:	return (XVT_KEY_KP_SUB);
#endif
		case XK_R4:				return (XVT_KEY_KP_SUB);

#if defined(XK_KP_Add)
		case XK_KP_Add:			return (XVT_KEY_KP_ADD);
#endif

#if defined(XK_KP_Divide)
		case XK_KP_Divide:		return (XVT_KEY_KP_DIV);
#endif
		case XK_R5:				return (XVT_KEY_KP_DIV);

#if defined(XK_KP_Multiply)
		case XK_KP_Multiply:	return (XVT_KEY_KP_MUL);
#endif
		case XK_R6:				return (XVT_KEY_KP_MUL);

#if defined (XK_KP_ENTER)
		case XK_KP_ENTER:		return (XVT_KEY_KP_ENTER);
#endif

		/*----------------------------------------------------------------
		 * misc keys
		 */
#if defined(XK_Pause)
		case XK_Pause:
#endif
		case XK_R1:				return (XVT_KEY_MS_PAUSE);

#if defined(XK_Print)
		case XK_Print:
#endif
		case XK_R2:				return (XVT_KEY_MS_PRTSCR);

#if defined(XK_Scroll_Lock)
		case XK_Scroll_Lock:
#endif
		case XK_R3:				return (XVT_KEY_MS_SCRLCK);

#if defined(XK_Sys_Req)
		case XK_Sys_Req:		return (XVT_KEY_MS_SYSREQ);
#endif

#if defined(XK_Break)
		case XK_Break:			return (XVT_KEY_MS_BREAK);
#endif

		/*----------------------------------------------------------------
		 * prefix keys
		 */
		case XK_Shift_L:
		case XK_Shift_R:		return (XVT_KEY_PF_SHFT);

		case XK_Control_L:
		case XK_Control_R:		return (XVT_KEY_PF_CTRL);

		case XK_Meta_L:
		case XK_Meta_R:
		case XK_Alt_L:
		case XK_Alt_R:			return (XVT_KEY_PF_META);

		case XK_Num_Lock:		return (XVT_KEY_PF_NUML);
	}

	return (XVT_KEY_OT_UNKNOWN);
}

/*------------------------------------------------------------------------
 * convert an X event to our event
 */
static void xvt_w_event_cvt (TERMWIN *tw, XEvent*xe, XVT_EVENT *xp)
{
	switch (xe->type)
	{
	/*--------------------------------------------------------------------
	 * Key press/release events
	 */
	case KeyPress:
	case KeyRelease:
		xp->event_type = XVT_E_KEYPRESS;
		{
			XVT_EVENT_KEYPRESS *	xk = &xp->event_data.keypress;
			XKeyEvent *				xt = &xe->xkey;

			/*------------------------------------------------------------
			 * Get the state of the keyboard prefix keys.
			 *
			 * It seems that the Mod3Mask is used to indicate the
			 * state of the NumLock key, but this is not documented.
			 */
											xk->kbd_state  = KEY_MASK_NONE;
			if (xt->state & ShiftMask)		xk->kbd_state |= KEY_MASK_SHFT;
			if (xt->state & ControlMask)	xk->kbd_state |= KEY_MASK_CTRL;
			if (xt->state & Mod1Mask)		xk->kbd_state |= KEY_MASK_META;
			if (xt->state & Mod3Mask)		xk->kbd_state |= KEY_MASK_NUML;

			/*------------------------------------------------------------
			 * cache up/down state
			 */
			xk->key_state = (xt->type == KeyPress);

			/*------------------------------------------------------------
			 * now translate the key value
			 */
			xk->key_value = xvt_w_event_xlate_key(xt, &xk->key_symbol);
		}
		break;

	/*--------------------------------------------------------------------
	 * Keyboard state changed event
	 */
	case KeymapNotify:
		xp->event_type = XVT_E_KEYMAP;
		{
			XVT_EVENT_KEYMAP *		xk = &xp->event_data.keymap;
			XKeymapEvent *			xt = &xe->xkeymap;

			/*------------------------------------------------------------
			 * This event happens when we get focus, so we don't know
			 * what the state of the keyboard is.
			 */
			xk->kbd_state = xvt_w_event_kbd_get_state(tw, xt->key_vector);
		}
		break;

	/*--------------------------------------------------------------------
	 * Button press/release events
	 */
	case ButtonPress:
	case ButtonRelease:
		xp->event_type = XVT_E_BUTTON;
		{
			XVT_EVENT_BUTTON *		xk = &xp->event_data.button;
			XButtonEvent *			xt = &xe->xbutton;

			/*------------------------------------------------------------
			 * We are only interested in button events inside the window.
			 */
			if (X_ON_BORDER(tw, xt->x) || Y_ON_BORDER(tw, xt->y))
			{
				xp->event_type = XVT_E_IGNORE;
			}
			else
			{
				switch (xt->button)
				{
				case Button1:	xk->button = BUT_L;	break;
				case Button2:	xk->button = BUT_M;	break;
				case Button3:	xk->button = BUT_R;	break;
				case Button4:	xk->button = BUT_U;	break;
				case Button5:	xk->button = BUT_D;	break;
				}

				xk->btn_state = (xt->type == ButtonPress);

				xk->x	= xt->x;
				xk->y	= xt->y;

				xk->col	= X_TO_COL(tw, xt->x);
				xk->row	= Y_TO_ROW(tw, xt->y);
			}
		}
		break;

	/*--------------------------------------------------------------------
	 * Pointer movement events
	 */
	case MotionNotify:
		xp->event_type = XVT_E_MOTION;
		{
			XVT_EVENT_MOTION *		xk = &xp->event_data.motion;
			XMotionEvent *			xt = &xe->xmotion;

			/*------------------------------------------------------------
			 * We are only interested in motion events inside the window.
			 */
			if (X_ON_BORDER(tw, xt->x) || Y_ON_BORDER(tw, xt->y))
			{
				xp->event_type = XVT_E_IGNORE;
			}
			else
			{
				xk->x	= xt->x;
				xk->y	= xt->y;

				xk->col	= X_TO_COL(tw, xt->x);
				xk->row	= Y_TO_ROW(tw, xt->y);
			}
		}
		break;

	/*--------------------------------------------------------------------
	 * Focus change events
	 */
	case FocusIn:
	case FocusOut:
		xp->event_type = XVT_E_FOCUS;
		{
			XVT_EVENT_FOCUS *		xk = &xp->event_data.focus;
			XFocusChangeEvent *		xt = &xe->xfocus;

			xk->focus	= (xt->type == FocusIn);
		}
		break;

	/*--------------------------------------------------------------------
	 * Exposure (window area repaint) events
	 */
	case Expose:
		xp->event_type = XVT_E_EXPOSE;
		{
			XVT_EVENT_EXPOSE *		xk = &xp->event_data.expose;
			XExposeEvent *			xt = &xe->xexpose;
			int						need_border	= FALSE;
			int						x, y;
			int						w, h;

			if (xt->window == tw->X->win_icon)
			{
				xk->type = XVT_EXPOSE_TYPE_ICON;
				xk->x = 0;
				xk->y = 0;
				xk->w = tw->X->ico_width;
				xk->h = tw->X->ico_height;
				break;
			}

			xk->type = XVT_EXPOSE_TYPE_WIN;
			if (X_ON_L_BORDER(tw, xt->x))
			{
				x = 0;
				need_border = TRUE;
			}
			else
			{
				x = X_TO_COL(tw, xt->x);
			}

			if (Y_ON_T_BORDER(tw, xt->y))
			{
				y = 0;
				need_border = TRUE;
			}
			else
			{
				y = Y_TO_ROW(tw, xt->y);
			}

			if (X_ON_R_BORDER(tw, xt->x + xt->width))
			{
				w = tw->win_cols; 
				need_border = TRUE;
			}
			else
			{
				w = X_TO_COL(tw, xt->x + xt->width  + COL_TO_W(tw, 1));
			}

			if (Y_ON_B_BORDER(tw, xt->y + xt->height))
			{
				h = tw->win_rows;
				need_border = TRUE;
			}
			else
			{
				h = Y_TO_ROW(tw, xt->y + xt->height + ROW_TO_H(tw, 1));
			}

			w = (w - x) + 1;
			h = (h - y) + 1;

			xk->x	= x;
			xk->y	= y;
			xk->w	= w;
			xk->h	= h;

			if (need_border)
				xvt_w_draw_border(tw);
		}
		break;

	/*--------------------------------------------------------------------
	 * Window size change events
	 */
	case ConfigureNotify:
		xp->event_type = XVT_E_CONFIG;
		{
			XVT_EVENT_CONFIG *		xk = &xp->event_data.config;
			XConfigureEvent *		xt = &xe->xconfigure;

			xk->w	= W_TO_COL(tw, xt->width  - (2 * tw->extra) - tw->sb_size);
			xk->h	= H_TO_ROW(tw, xt->height - (2 * tw->extra));

			if (tw->win_cols != xk->w || tw->win_rows != xk->h)
			{
				tw->win_cols = xk->w;
				tw->win_rows = xk->h;

				xvt_w_resize_win(tw, -1, -1);
				xvt_w_draw_border(tw);
			}
		}
		break;

	/*--------------------------------------------------------------------
	 * Iconify/Uniconify events
	 */
	case MapNotify:
	case UnmapNotify:
		xp->event_type = XVT_E_MAP;
		{
			XVT_EVENT_MAP *			xk = &xp->event_data.map;

			xk->mapped	= (xe->type == MapNotify);
		}
		tw->iconified = (xe->type == UnmapNotify);
		break;

	/*--------------------------------------------------------------------
	 * cut/paste events
	 */
	case SelectionClear:
		{
			XSelectionClearEvent *	xt	= &xe->xselectionclear;

			if (tw->selection_ptr != 0)
			{
				FREE(tw->selection_ptr);
				tw->selection_ptr = 0;
				tw->selection_len = 0;
			}
		}
		break;

	case SelectionNotify:
		{
			XSelectionEvent *		xt	= &xe->xselection;
		}
		break;

	case SelectionRequest:
		{
			XSelectionRequestEvent *xt	= &xe->xselectionrequest;
			XEvent					ev;

			ev.xselection.type		= SelectionNotify;
			ev.xselection.property	= None;
			ev.xselection.display	= xt->display;
			ev.xselection.requestor	= xt->requestor;
			ev.xselection.selection	= xt->selection;
			ev.xselection.target	= xt->target;
			ev.xselection.time		= xt->time;

			if (tw->selection_ptr != 0)
			{
				ev.xselection.property	= xt->property;
				XChangeProperty(tw->X->display, xt->requestor, xt->property,
					tw->X->cb_text, 8, PropModeReplace, tw->selection_ptr,
					tw->selection_len);
			}

			XSendEvent(tw->X->display, xt->requestor, False, 0, &ev);
		}
		break;

	/*--------------------------------------------------------------------
	 * Window manager events
	 */
	case ClientMessage:
		xp->event_type = XVT_E_WINMSG;
		{
			XVT_EVENT_WINMSG *		xk = &xp->event_data.winmsg;
			XClientMessageEvent *	xt = &xe->xclient;

			/*------------------------------------------------------------
			 * These are window manager messages.  Currently, the only
			 * message we are interested in is "WM_DELETE_WINDOW".
			 */
			if (xt->format == 32)
			{
				if (xt->data.l[0] == tw->X->wm_delete)
				{
					xk->winmsg	= XVT_W_DELETE;
				}
				else
				{
					xk->winmsg	= XVT_W_IGNORE;
				}
			}
			else
			{
				xk->winmsg	= XVT_W_IGNORE;
			}
		}
		break;

	/*--------------------------------------------------------------------
	 * Unknown events
	 */
	default:
		xp->event_type = XVT_E_IGNORE;
		break;
	}
}

/*------------------------------------------------------------------------
 * get next event if there is one
 */
int xvt_w_event_get (TERMWIN *tw, XVT_EVENT *xe)
{
	XEvent	ev;

	if (XPending(tw->X->display) > 0)
	{
		XNextEvent(tw->X->display, &ev);
		xvt_w_event_cvt(tw, &ev, xe);
		return (TRUE);
	}

	return (FALSE);
}
