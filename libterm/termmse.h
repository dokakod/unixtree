/*------------------------------------------------------------------------
 *	Basic mouse header file
 */
#ifndef TERMMSE_H
#define TERMMSE_H

#include "termtime.h"

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * mouse button definitions
 */
#define NUM_BUTTONS		3

#define BUTTON_LEFT		0
#define BUTTON_MIDDLE	1
#define BUTTON_RIGHT	2
#define BUTTON_SCRLUP	3
#define BUTTON_SCRLDN	4

#define BUTTON_UP		0
#define BUTTON_DOWN		1

/*------------------------------------------------------------------------
 * mouse status struct
 */
struct mouse_status
{
	int			x;						/* curr x-position (-1 == unknown)	*/
	int			y;						/* curr y-position (-1 == unknown)	*/
	int			old_x;					/* prev x-position (-1 == unknown)	*/
	int			old_y;					/* prev y-position (-1 == unknown)	*/
	int			max_x;					/* max x position (user set)		*/
	int			max_y;					/* max y position (user set)		*/
	int			button[NUM_BUTTONS];	/* up/dn state of all buttons		*/
	TIME_VAL	btime[NUM_BUTTONS];		/* time since last button event		*/
	int			dbl_clk_interval;		/* double-click interval (in ms)	*/
	int			event;					/* curr mouse event					*/
	int			queued_event;			/* prev mouse event					*/
	int			changes;				/* curr button states				*/
	int			saved_buttons;			/* prev button states				*/
	int			is_on;					/* TRUE if on						*/
	int			is_init;				/* TRUE if initialized				*/
};
typedef struct mouse_status MOUSE_STATUS;

/*------------------------------------------------------------------------
 * button tables
 */
extern const int term_button_down[NUM_BUTTONS];
extern const int term_button_up  [NUM_BUTTONS];
extern const int term_button_dbl [NUM_BUTTONS];

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TERMMSE_H */
