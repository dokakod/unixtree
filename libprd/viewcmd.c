/*------------------------------------------------------------------------
 * view cmds
 */
#include "libprd.h"

void view_command (VFCB *v, int cmd)
{
	int i;
	int c;
	long p;

	switch (cmd)
	{
	/* movement commands */

	case V_CMD_UP:
		if (!v->v_fd)
			break;
		if (!v->v_top_pos)
			break;
		{
			switch (v->v_mode & V_MODE)
			{
			case V_DUMP:
				if (v->v_top_pos < v->v_dump_width)
					v->v_top_pos = 0;
				else
					v->v_top_pos = (v->v_top_pos & ~3) - v->v_dump_width;
				break;

			case V_HEX:
				if (v->v_top_pos < v->v_hex_width)
					v->v_top_pos = 0;
				else
					v->v_top_pos = (v->v_top_pos & ~3) - v->v_hex_width;
				break;

			case V_ASCII:
			case V_WRAP:
				v->v_cur_pos = v->v_top_pos;
				get_buffer(v);
				up_line(v);
				v->v_top_pos = v->v_cur_pos;
				break;
			}
			v_display(v);
		}
		break;
	case V_CMD_DOWN:
		if (!v->v_fd)
			break;
		if (v->v_cur_ptr)
		{
			switch (v->v_mode & V_MODE)
			{
			case V_DUMP:
				if (v->v_top_pos + v->v_dump_width < v->v_filesize)
				{
					v->v_top_pos = (v->v_top_pos & ~3) +
						v->v_dump_width;
				}
				break;

			case V_HEX:
				if (v->v_top_pos + v->v_hex_width < v->v_filesize)
					v->v_top_pos = (v->v_top_pos & ~3) + v->v_hex_width;
				break;

			case V_ASCII:
			case V_WRAP:
				v->v_cur_pos = v->v_top_pos;
				get_buffer(v);
				down_line(v);
				v->v_top_pos = v->v_cur_pos;
				break;
			}
			v_display(v);
		}
		break;
	case V_CMD_LEFT:
		if (!v->v_fd)
			break;
		/* no defined action yet */
		break;
	case V_CMD_RIGHT:
		if (!v->v_fd)
			break;
		/* no defined action yet */
		break;
	case V_CMD_HOME:
		if (!v->v_fd)
			break;
		if (v->v_top_pos)
		{
			v->v_top_pos = 0;
			v_display(v);
		}
		break;
	case V_CMD_END:
		if (!v->v_fd)
			break;
		if (v->v_cur_ptr)
		{
			/*
			 *  Note: to get here there must be more than
			 *  a pagefull of text in the file past v_top_pos.
			 */
			switch (v->v_mode & V_MODE)
			{
			case V_DUMP:
				c = v->v_dump_width * getmaxy(v->v_win);
				v->v_top_pos = ((v->v_filesize+3)&~3) - c;
				break;

			case V_HEX:
				c = v->v_hex_width * getmaxy(v->v_win);
				v->v_top_pos = ((v->v_filesize+3)&~3) - c;
				break;

			case V_ASCII:
			case V_WRAP:
				v->v_top_pos = v->v_filesize-1;
				get_buffer(v);
				v->v_cur_pos++;		/* pseudo position */
				v->v_cur_ptr++;
				goto do_pgup;
			}
			v_display(v);
		}
		break;
	case V_CMD_PGUP:
	do_pgup:
		if (!v->v_fd)
			break;
		{
			switch (v->v_mode & V_MODE)
			{
			case V_DUMP:
				c = v->v_dump_width * getmaxy(v->v_win);
				if (v->v_top_pos < c)
					v->v_top_pos = 0;
				else
					v->v_top_pos = (v->v_top_pos & ~3) - c;
				break;

			case V_HEX:
				c = v->v_hex_width * getmaxy(v->v_win);
				if (v->v_top_pos < c)
					v->v_top_pos = 0;
				else
					v->v_top_pos = (v->v_top_pos & ~3) - c;
				break;

			case V_ASCII:
				for (i=0; i<getmaxy(v->v_win); i++)
				{
					v->v_cur_pos = v->v_top_pos;
					get_buffer(v);
					up_line(v);
					v->v_top_pos = v->v_cur_pos;
					if (!v->v_cur_pos)
						break;
				}
				break;

			case V_WRAP:
				for (i=0; i<getmaxy(v->v_win); )
				{
					v->v_cur_pos = v->v_top_pos;
					get_buffer(v);
					p = v->v_cur_pos;
					up_line(v);
					v->v_top_pos = v->v_cur_pos;
					if (!v->v_cur_pos)
						break;
					i += (p-v->v_cur_pos + v->v_ascii_width-1) /
					     v->v_ascii_width;
				}
				break;
			}
			v_display(v);
		}
		break;
	case V_CMD_PGDN:
		if (!v->v_fd)
			break;
		if (v->v_cur_ptr)
		{
			switch (v->v_mode & V_MODE)
			{
			case V_DUMP:
				c = v->v_dump_width * getmaxy(v->v_win);
				if (v->v_top_pos + c < v->v_filesize)
					v->v_top_pos = (v->v_top_pos & ~3) + c;
				break;

			case V_HEX:
				c = v->v_hex_width * getmaxy(v->v_win);
				if (v->v_top_pos + c < v->v_filesize)
					v->v_top_pos = (v->v_top_pos & ~3) + c;
				break;

			case V_ASCII:
				for (i=0; i<getmaxy(v->v_win); i++)
				{
					v->v_cur_pos = v->v_top_pos;
					get_buffer(v);
					down_line(v);
					if (v->v_cur_ptr)
						v->v_top_pos = v->v_cur_pos;
					else
						break;
				}
				break;

			case V_WRAP:
				for (i=0; i<getmaxy(v->v_win); )
				{
					p = v->v_top_pos;
					v->v_cur_pos = v->v_top_pos;
					get_buffer(v);
					down_line(v);
					if (v->v_cur_ptr)
						v->v_top_pos = v->v_cur_pos;
					else
						break;
					i += (v->v_cur_pos-p + v->v_ascii_width-1) /
					     v->v_ascii_width;
				}
				break;
			}
			v_display(v);
		}
		break;

	/* mode commands */

	case V_CMD_ASCII:
		if ((v->v_mode & V_MODE) != V_ASCII)
		{
			v->v_mode &= ~V_MODE;
			v->v_mode |=  V_ASCII;
			v_display(v);
		}
		break;
	case V_CMD_DUMP:
		if ((v->v_mode & V_MODE) != V_DUMP)
		{
			v->v_mode &= ~V_MODE;
			v->v_mode |=  V_DUMP;
			v_display(v);
		}
		break;
	case V_CMD_HEX:
		if ((v->v_mode & V_MODE) != V_HEX)
		{
			v->v_mode &= ~V_MODE;
			v->v_mode |=  V_HEX;
			v_display(v);
		}
		break;
	case V_CMD_WRAP:
		if ((v->v_mode & V_MODE) != V_WRAP)
		{
			v->v_mode &= ~V_MODE;
			v->v_mode |=  V_WRAP;
			v_display(v);
		}
		break;
	case V_KEEP_MODE:
		if (v->v_mode & V_AUTO)
			v->v_mode &= ~V_AUTO;
		else
			v->v_mode |= V_AUTO;
		break;
	case V_CMD_TOGGLE_MASK:
		opt(display_mask) = ! opt(display_mask);
		if (v->v_mode & V_MASKED)
			v->v_mode &= ~V_MASKED;
		else
			v->v_mode |= V_MASKED;
		v_display(v);
		break;

	/* misc commands */

	case V_CMD_REDISPLAY:
		v_display(v);
		break;
	case V_CMD_REDISP_NR:
		v_display_no_refresh(v);
		break;
	case V_CMD_OPEN_RO:
		view_open(v, FALSE);
		break;
	case V_CMD_OPEN_RW:
		view_open(v, TRUE);
		break;
	case V_CMD_CLOSE:
		view_close(v);
		break;

	/* search commands */

	case V_SRCH_NEXT:
		view_next(v);
		break;
	case V_SRCH_PREV:
		view_prev(v);
		break;
	}
}
