/*------------------------------------------------------------------------
 * process input
 */
#include "libprd.h"

/* this is the main input loop */

static void pgm_redisp(void)
{
	setup_display();
	win_clock_check();
	disp_cmds();
	disp_prompt_line();
}

static int pgm_loop (int count)
{
	int rc = 0;

	if (count > 0 && gbl(scr_in_autoview) && ! gbl(scr_in_hexedit))
	{
		if (gbl(scr_cur)->cur_file != gbl(scr_cur)->av_file_displayed)
		{
			close_av_file();
			open_av_file(gbl(scr_cur)->cur_file);
			display_av();
			gbl(scr_cur)->av_file_displayed = gbl(scr_cur)->cur_file;
			rc = 1;
		}
	}

	return (rc);
}

static void pgm_position (void)
{
	position_cursor();
	wrefresh(stdscr);
}

static void pgm_bls (void)
{
	const char *logo[3];

	logo[0] = package_name();
	logo[1] = msgs(m_pinput_anykey);
	logo[2] = 0;

	blank_screen(logo);
}

static void print_key (int c)
{
	if (c == KEY_MOUSE)
		c = mouse_get_event(gbl(win_commands));
	bang(keyname(c));
}

static int pgm_main_loop_input (void)
{
	return (get_input_char(stdscr, pgm_redisp, pgm_loop, pgm_position));
}

void process_input (int c)
{
	gbl(scr_cur_cmd) = c;
	gbl(scr_cur_sub_cmd) = 0;

	while (TRUE)
	{
		if (! cmdopt(debug_kbd))
			disp_prompt_line();

		if (c == 0)
			c = pgm_main_loop_input();

		process_cmd(c);
		c = 0;
	}
}

int get_input_char (WINDOW *win, void (*redisplay)(void),
	int (*loop)(int count), void (*curpos)(void))
{
	int input_wait_count;		/* count in secs since last keypress */
	int c;
	int save_delay;

	input_wait_count = 0;
	while (TRUE)
	{
		if (trm_is_changed(TRUE))
		{
			resize_term();
			wclear(stdscr);
			wrefresh(stdscr);
			redisplay();
			input_wait_count = 0;
			return (ERR);
		}

		if (loop != 0)
		{
			if (loop(input_wait_count))
				input_wait_count = 0;
		}

		/* blank the screen if enough time has elapsed */

		if (! gbl(scr_in_window) && opt(screen_blank_interval) &&
			(input_wait_count > opt(screen_blank_interval)))
		{
			pgm_bls();
			input_wait_count = 0;
		}

		if (curpos != 0)
			curpos();

		save_delay = getdelay(win);
		setdelay(win, pgm_const(kbd_wait_interval));
		c = rgetch(win);
		setdelay(win, save_delay);
		if (c != ERR)
			break;
		input_wait_count++;
	}

	if (cmdopt(debug_kbd))
		print_key(c);

	input_wait_count = 0;

	return (c);
}
