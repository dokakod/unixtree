/*------------------------------------------------------------------------
 * event structure
 */
#ifndef XVTEVENT_H
#define XVTEVENT_H

/*------------------------------------------------------------------------
 * keypress event struct
 */
struct xvt_event_keypress
{
	int		kbd_state;		/* prefix key state		*/
	int		key_state;		/* TRUE if down			*/
	int		key_value;		/* value of key			*/
	int		key_symbol;		/* internal key value	*/
};
typedef struct xvt_event_keypress	XVT_EVENT_KEYPRESS;

/*------------------------------------------------------------------------
 * keymap event struct
 */
struct xvt_event_keymap
{
	int		kbd_state;		/* prefix key state		*/
};
typedef struct xvt_event_keymap		XVT_EVENT_KEYMAP;

/*------------------------------------------------------------------------
 * button event struct
 */
struct xvt_event_button
{
	int		button;			/* button value			*/
	int		btn_state;		/* TRUE if down			*/
	int		col;			/* col value			*/
	int		row;			/* row value			*/
	int		x;				/* x-value (pixels)		*/
	int		y;				/* y-value (pixels)		*/
};
typedef struct xvt_event_button		XVT_EVENT_BUTTON;

/*------------------------------------------------------------------------
 * motion event struct
 */
struct xvt_event_motion
{
	int		col;			/* col value			*/
	int		row;			/* row value			*/
	int		x;				/* x-value (pixels)		*/
	int		y;				/* y-value (pixels)		*/
};
typedef struct xvt_event_motion		XVT_EVENT_MOTION;

/*------------------------------------------------------------------------
 * focus event struct
 */
struct xvt_event_focus
{
	int		focus;			/* TRUE if in focus		*/
};
typedef struct xvt_event_focus		XVT_EVENT_FOCUS;

/*------------------------------------------------------------------------
 * expose event struct
 */
#define XVT_EXPOSE_TYPE_WIN		1
#define XVT_EXPOSE_TYPE_ICON	2

struct xvt_event_expose
{
	int		x;				/* x-pos of top-left	*/
	int		y;				/* y-pos of top-left	*/
	int		w;				/* width  of box		*/
	int		h;				/* height of box		*/
	int		type;			/* window type			*/
};
typedef struct xvt_event_expose		XVT_EVENT_EXPOSE;

/*------------------------------------------------------------------------
 * config event struct
 */
struct xvt_event_config
{
	int		w;				/* width  of window		*/
	int		h;				/* height of window		*/
};
typedef struct xvt_event_config		XVT_EVENT_CONFIG;

/*------------------------------------------------------------------------
 * winmsg event struct
 */
struct xvt_event_winmsg
{
	int		winmsg;			/* window manager msg	*/
};
typedef struct xvt_event_winmsg		XVT_EVENT_WINMSG;

/*------------------------------------------------------------------------
 * map event struct
 */
struct xvt_event_map
{
	int		mapped;			/* TRUE if mapped (not iconic)	*/
};
typedef struct xvt_event_map		XVT_EVENT_MAP;

/*------------------------------------------------------------------------
 * generic event struct
 */
union event_data
{
	XVT_EVENT_KEYPRESS	keypress;
	XVT_EVENT_KEYMAP	keymap;
	XVT_EVENT_BUTTON	button;
	XVT_EVENT_MOTION	motion;
	XVT_EVENT_FOCUS		focus;
	XVT_EVENT_EXPOSE	expose;
	XVT_EVENT_CONFIG	config;
	XVT_EVENT_WINMSG	winmsg;
	XVT_EVENT_MAP		map;
};
typedef union event_data	EVENT_DATA;

struct xvt_event
{
	int			event_type;	/* event type			*/
	EVENT_DATA	event_data;	/* event data			*/
};
typedef struct xvt_event			XVT_EVENT;

/*------------------------------------------------------------------------
 * event defines
 */
#define XVT_E_IGNORE		0
#define XVT_E_KEYPRESS		1
#define XVT_E_KEYMAP		2
#define XVT_E_BUTTON		3
#define XVT_E_MOTION		4
#define XVT_E_FOCUS			5
#define XVT_E_EXPOSE		6
#define XVT_E_CONFIG		7
#define XVT_E_WINMSG		8
#define XVT_E_MAP			9

/*------------------------------------------------------------------------
 * window messages
 */
#define XVT_W_IGNORE		0		/* ignore this msg	*/
#define XVT_W_DELETE		1		/* WM delete msg	*/

#endif /* XVTEVENT_H */
