/*------------------------------------------------------------------------
 * Key name definitions
 */
#ifndef WKEYS_H
#define WKEYS_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*@ +++API+++ */
/*------------------------------------------------------------------------
 * Control characters
 */
#define	CTL_AT			0x00
#define	CTL_A			0x01
#define	CTL_B			0x02
#define	CTL_C			0x03
#define	CTL_D			0x04
#define	CTL_E			0x05
#define	CTL_F			0x06
#define	CTL_G			0x07
#define	CTL_H			0x08
#define	CTL_I			0x09
#define	CTL_J			0x0a
#define	CTL_K			0x0b
#define	CTL_L			0x0c
#define	CTL_M			0x0d
#define	CTL_N			0x0e
#define	CTL_O			0x0f
#define	CTL_P			0x10
#define	CTL_Q			0x11
#define	CTL_R			0x12
#define	CTL_S			0x13
#define	CTL_T			0x14
#define	CTL_U			0x15
#define	CTL_V			0x16
#define	CTL_W			0x17
#define	CTL_X			0x18
#define	CTL_Y			0x19
#define	CTL_Z			0x1a

/*------------------------------------------------------------------------
 * Names for keys which match control (and other) characters
 */
#define KEY_BL				CTL_G			/* bell			*/
#define KEY_BS				CTL_H			/* backspace	*/
#define KEY_TAB				CTL_I			/* horiz tab	*/
#define KEY_LF				CTL_J			/* line feed	*/
#define KEY_VT				CTL_K			/* vert tab		*/
#define KEY_FF				CTL_L			/* form feed	*/
#define KEY_RETURN			CTL_M			/* return		*/

#define KEY_ESCAPE			0x1b			/* escape key	*/
#define KEY_DELETE			0x7f			/* delete key	*/

#define KEY_SPACE			' '

/*------------------------------------------------------------------------
 * Masks for different categories of keys
 */
#define MASK_REG			0x0100		/* regular special keys			*/
#define MASK_SHIFT			0x0200		/* shifted special keys			*/
#define MASK_CTRL			0x0400		/* control special keys			*/
#define MASK_ALT			0x0800		/* alt     special keys			*/
#define MASK_OTHER			0x1000		/* other   special keys			*/
#define MASK_CUR			0x2000		/* old curses compatible defs	*/
#define MASK_MOUSE			0x4000		/* mouse keys					*/
#define MASK_USER			0x8000		/* reserved for user use		*/

#define MASK_META			0xff00		/* mask for all special codes	*/

/*------------------------------------------------------------------------
 * Basic key definitions
 */
#define K_INSERT			0x00
#define K_END				0x01
#define K_DOWN				0x02
#define K_PGDN				0x03
#define K_LEFT				0x04
#define K_MIDDLE			0x05
#define K_RIGHT				0x06
#define K_HOME				0x07
#define K_UP				0x08
#define K_PGUP				0x09

#define K_USER1				0x0b		/* these keys available for users */
#define K_USER2				0x0c
#define K_USER3				0x0d
#define K_USER4				0x0e
#define K_USER5				0x0f

#define K_KP0				0x10		/* keypad codes */
#define K_KP1				0x11
#define K_KP2				0x12
#define K_KP3				0x13
#define K_KP4				0x14
#define K_KP5				0x15
#define K_KP6				0x16
#define K_KP7				0x17
#define K_KP8				0x18
#define K_KP9				0x19
#define K_KPPERIOD			0x1a

#define K_TAB				0x1b
#define K_BS				0x1c
#define K_RETURN			0x1d
#define K_PRESS				0x1e
#define K_RELEASE			0x1f

#define K_F1				0x20		/* function keys */
#define K_F2				0x21
#define K_F3				0x22
#define K_F4				0x23
#define K_F5				0x24
#define K_F6				0x25
#define K_F7				0x26
#define K_F8				0x27
#define K_F9				0x28
#define K_F10				0x29
#define K_F11				0x2a
#define K_F12				0x2b
#define K_F13				0x2c
#define K_F14				0x2d
#define K_F15				0x2e
#define K_F16				0x2f

/*------------------------------------------------------------------------
 * keypad keys
 */
#define KEY_KP0				( MASK_REG | K_KP0 )
#define KEY_KP1				( MASK_REG | K_KP1 )
#define KEY_KP2				( MASK_REG | K_KP2 )
#define KEY_KP3				( MASK_REG | K_KP3 )
#define KEY_KP4				( MASK_REG | K_KP4 )
#define KEY_KP5				( MASK_REG | K_KP5 )
#define KEY_KP6				( MASK_REG | K_KP6 )
#define KEY_KP7				( MASK_REG | K_KP7 )
#define KEY_KP8				( MASK_REG | K_KP8 )
#define KEY_KP9				( MASK_REG | K_KP9 )
#define KEY_KPPERIOD		( MASK_REG | K_KPPERIOD )

#define KEY_INSERT			( MASK_REG | K_INSERT )

#define KEY_UP				( MASK_REG | K_UP )
#define KEY_DOWN			( MASK_REG | K_DOWN )
#define KEY_LEFT			( MASK_REG | K_LEFT )
#define KEY_RIGHT			( MASK_REG | K_RIGHT )
#define KEY_HOME			( MASK_REG | K_HOME )
#define KEY_END				( MASK_REG | K_END )
#define KEY_PGUP			( MASK_REG | K_PGUP )
#define KEY_PGDN			( MASK_REG | K_PGDN )
#define KEY_MIDDLE			( MASK_REG | K_MIDDLE )

#define KEY_USER1			( MASK_REG | K_USER1 )
#define KEY_USER2			( MASK_REG | K_USER2 )
#define KEY_USER3			( MASK_REG | K_USER3 )
#define KEY_USER4			( MASK_REG | K_USER4 )
#define KEY_USER5			( MASK_REG | K_USER5 )

#define KEY_SHIFT_KP0		( MASK_SHIFT | K_KP0 )
#define KEY_SHIFT_KP1		( MASK_SHIFT | K_KP1 )
#define KEY_SHIFT_KP2		( MASK_SHIFT | K_KP2 )
#define KEY_SHIFT_KP3		( MASK_SHIFT | K_KP3 )
#define KEY_SHIFT_KP4		( MASK_SHIFT | K_KP4 )
#define KEY_SHIFT_KP5		( MASK_SHIFT | K_KP5 )
#define KEY_SHIFT_KP6		( MASK_SHIFT | K_KP6 )
#define KEY_SHIFT_KP7		( MASK_SHIFT | K_KP7 )
#define KEY_SHIFT_KP8		( MASK_SHIFT | K_KP8 )
#define KEY_SHIFT_KP9		( MASK_SHIFT | K_KP9 )

#define KEY_SHIFT_UP		( MASK_SHIFT | K_UP )
#define KEY_SHIFT_DOWN		( MASK_SHIFT | K_DOWN )
#define KEY_SHIFT_RIGHT		( MASK_SHIFT | K_RIGHT )
#define KEY_SHIFT_LEFT		( MASK_SHIFT | K_LEFT )
#define KEY_SHIFT_HOME		( MASK_SHIFT | K_HOME )
#define KEY_SHIFT_END		( MASK_SHIFT | K_END )
#define KEY_SHIFT_PGUP		( MASK_SHIFT | K_PGUP )
#define KEY_SHIFT_PGDN		( MASK_SHIFT | K_PGDN )
#define KEY_SHIFT_MIDDLE	( MASK_SHIFT | K_MIDDLE )

#define KEY_SHIFT_USER1		( MASK_SHIFT | K_USER1 )
#define KEY_SHIFT_USER2		( MASK_SHIFT | K_USER2 )
#define KEY_SHIFT_USER3		( MASK_SHIFT | K_USER3 )
#define KEY_SHIFT_USER4		( MASK_SHIFT | K_USER4 )
#define KEY_SHIFT_USER5		( MASK_SHIFT | K_USER5 )

#define KEY_SHIFT_BS		( MASK_SHIFT | K_BS )
#define KEY_SHIFT_TAB		( MASK_SHIFT | K_TAB )
#define KEY_SHIFT_RETURN	( MASK_SHIFT | K_RETURN )

#define KEY_SHIFT_PRESS		( MASK_SHIFT | K_PRESS )
#define KEY_SHIFT_RELEASE	( MASK_SHIFT | K_RELEASE )

#define KEY_CTRL_KP0		( MASK_CTRL | K_KP0 )
#define KEY_CTRL_KP1		( MASK_CTRL | K_KP1 )
#define KEY_CTRL_KP2		( MASK_CTRL | K_KP2 )
#define KEY_CTRL_KP3		( MASK_CTRL | K_KP3 )
#define KEY_CTRL_KP4		( MASK_CTRL | K_KP4 )
#define KEY_CTRL_KP5		( MASK_CTRL | K_KP5 )
#define KEY_CTRL_KP6		( MASK_CTRL | K_KP6 )
#define KEY_CTRL_KP7		( MASK_CTRL | K_KP7 )
#define KEY_CTRL_KP8		( MASK_CTRL | K_KP8 )
#define KEY_CTRL_KP9		( MASK_CTRL | K_KP9 )

#define KEY_CTRL_UP			( MASK_CTRL | K_UP )
#define KEY_CTRL_DOWN		( MASK_CTRL | K_DOWN )
#define KEY_CTRL_RIGHT		( MASK_CTRL | K_RIGHT )
#define KEY_CTRL_LEFT		( MASK_CTRL | K_LEFT )
#define KEY_CTRL_HOME		( MASK_CTRL | K_HOME )
#define KEY_CTRL_END		( MASK_CTRL | K_END )
#define KEY_CTRL_PGUP		( MASK_CTRL | K_PGUP )
#define KEY_CTRL_PGDN		( MASK_CTRL | K_PGDN )
#define KEY_CTRL_MIDDLE		( MASK_CTRL | K_MIDDLE )

#define KEY_CTRL_USER1		( MASK_CTRL | K_USER1 )
#define KEY_CTRL_USER2		( MASK_CTRL | K_USER2 )
#define KEY_CTRL_USER3		( MASK_CTRL | K_USER3 )
#define KEY_CTRL_USER4		( MASK_CTRL | K_USER4 )
#define KEY_CTRL_USER5		( MASK_CTRL | K_USER5 )

#define KEY_CTRL_BS			( MASK_CTRL | K_BS )
#define KEY_CTRL_TAB		( MASK_CTRL | K_TAB )
#define KEY_CTRL_RETURN		( MASK_CTRL | K_RETURN )

#define KEY_CTRL_PRESS		( MASK_CTRL | K_PRESS )
#define KEY_CTRL_RELEASE	( MASK_CTRL | K_RELEASE )

#define KEY_ALT_KP0			( MASK_ALT | K_KP0 )
#define KEY_ALT_KP1			( MASK_ALT | K_KP1 )
#define KEY_ALT_KP2			( MASK_ALT | K_KP2 )
#define KEY_ALT_KP3			( MASK_ALT | K_KP3 )
#define KEY_ALT_KP4			( MASK_ALT | K_KP4 )
#define KEY_ALT_KP5			( MASK_ALT | K_KP5 )
#define KEY_ALT_KP6			( MASK_ALT | K_KP6 )
#define KEY_ALT_KP7			( MASK_ALT | K_KP7 )
#define KEY_ALT_KP8			( MASK_ALT | K_KP8 )
#define KEY_ALT_KP9			( MASK_ALT | K_KP9 )

#define KEY_ALT_UP			( MASK_ALT | K_UP )
#define KEY_ALT_DOWN		( MASK_ALT | K_DOWN )
#define KEY_ALT_RIGHT		( MASK_ALT | K_RIGHT )
#define KEY_ALT_LEFT		( MASK_ALT | K_LEFT )
#define KEY_ALT_HOME		( MASK_ALT | K_HOME )
#define KEY_ALT_END			( MASK_ALT | K_END )
#define KEY_ALT_PGUP		( MASK_ALT | K_PGUP )
#define KEY_ALT_PGDN		( MASK_ALT | K_PGDN )
#define KEY_ALT_MIDDLE		( MASK_ALT | K_MIDDLE )

#define KEY_ALT_USER1		( MASK_ALT | K_USER1 )
#define KEY_ALT_USER2		( MASK_ALT | K_USER2 )
#define KEY_ALT_USER3		( MASK_ALT | K_USER3 )
#define KEY_ALT_USER4		( MASK_ALT | K_USER4 )
#define KEY_ALT_USER5		( MASK_ALT | K_USER5 )

#define KEY_ALT_BS			( MASK_ALT | K_BS )
#define KEY_ALT_TAB			( MASK_ALT | K_TAB )
#define KEY_ALT_RETURN		( MASK_ALT | K_RETURN )

#define KEY_ALT_PRESS		( MASK_ALT | K_PRESS )
#define KEY_ALT_RELEASE		( MASK_ALT | K_RELEASE )

/*------------------------------------------------------------------------
 * function keys
 */
#define KEY_F1				( MASK_REG   | K_F1 )
#define KEY_F2				( MASK_REG   | K_F2 )
#define KEY_F3				( MASK_REG   | K_F3 )
#define KEY_F4				( MASK_REG   | K_F4 )
#define KEY_F5				( MASK_REG   | K_F5 )
#define KEY_F6				( MASK_REG   | K_F6 )
#define KEY_F7				( MASK_REG   | K_F7 )
#define KEY_F8				( MASK_REG   | K_F8 )
#define KEY_F9				( MASK_REG   | K_F9 )
#define KEY_F10				( MASK_REG   | K_F10 )
#define KEY_F11				( MASK_REG   | K_F11 )
#define KEY_F12				( MASK_REG   | K_F12 )
#define KEY_F13				( MASK_REG   | K_F13 )
#define KEY_F14				( MASK_REG   | K_F14 )
#define KEY_F15				( MASK_REG   | K_F15 )
#define KEY_F16				( MASK_REG   | K_F16 )

#define KEY_SHIFT_F1		( MASK_SHIFT | K_F1 )
#define KEY_SHIFT_F2		( MASK_SHIFT | K_F2 )
#define KEY_SHIFT_F3		( MASK_SHIFT | K_F3 )
#define KEY_SHIFT_F4		( MASK_SHIFT | K_F4 )
#define KEY_SHIFT_F5		( MASK_SHIFT | K_F5 )
#define KEY_SHIFT_F6		( MASK_SHIFT | K_F6 )
#define KEY_SHIFT_F7		( MASK_SHIFT | K_F7 )
#define KEY_SHIFT_F8		( MASK_SHIFT | K_F8 )
#define KEY_SHIFT_F9		( MASK_SHIFT | K_F9 )
#define KEY_SHIFT_F10		( MASK_SHIFT | K_F10 )
#define KEY_SHIFT_F11		( MASK_SHIFT | K_F11 )
#define KEY_SHIFT_F12		( MASK_SHIFT | K_F12 )
#define KEY_SHIFT_F13		( MASK_SHIFT | K_F13 )
#define KEY_SHIFT_F14		( MASK_SHIFT | K_F14 )
#define KEY_SHIFT_F15		( MASK_SHIFT | K_F15 )
#define KEY_SHIFT_F16		( MASK_SHIFT | K_F16 )

#define KEY_CTRL_F1			( MASK_CTRL  | K_F1 )
#define KEY_CTRL_F2			( MASK_CTRL  | K_F2 )
#define KEY_CTRL_F3			( MASK_CTRL  | K_F3 )
#define KEY_CTRL_F4			( MASK_CTRL  | K_F4 )
#define KEY_CTRL_F5			( MASK_CTRL  | K_F5 )
#define KEY_CTRL_F6			( MASK_CTRL  | K_F6 )
#define KEY_CTRL_F7			( MASK_CTRL  | K_F7 )
#define KEY_CTRL_F8			( MASK_CTRL  | K_F8 )
#define KEY_CTRL_F9			( MASK_CTRL  | K_F9 )
#define KEY_CTRL_F10		( MASK_CTRL  | K_F10 )
#define KEY_CTRL_F11		( MASK_CTRL  | K_F11 )
#define KEY_CTRL_F12		( MASK_CTRL  | K_F12 )
#define KEY_CTRL_F13		( MASK_CTRL  | K_F13 )
#define KEY_CTRL_F14		( MASK_CTRL  | K_F14 )
#define KEY_CTRL_F15		( MASK_CTRL  | K_F15 )
#define KEY_CTRL_F16		( MASK_CTRL  | K_F16 )

#define KEY_ALT_F1			( MASK_ALT   | K_F1 )
#define KEY_ALT_F2			( MASK_ALT   | K_F2 )
#define KEY_ALT_F3			( MASK_ALT   | K_F3 )
#define KEY_ALT_F4			( MASK_ALT   | K_F4 )
#define KEY_ALT_F5			( MASK_ALT   | K_F5 )
#define KEY_ALT_F6			( MASK_ALT   | K_F6 )
#define KEY_ALT_F7			( MASK_ALT   | K_F7 )
#define KEY_ALT_F8			( MASK_ALT   | K_F8 )
#define KEY_ALT_F9			( MASK_ALT   | K_F9 )
#define KEY_ALT_F10			( MASK_ALT   | K_F10 )
#define KEY_ALT_F11			( MASK_ALT   | K_F11 )
#define KEY_ALT_F12			( MASK_ALT   | K_F12 )
#define KEY_ALT_F13			( MASK_ALT   | K_F13 )
#define KEY_ALT_F14			( MASK_ALT   | K_F14 )
#define KEY_ALT_F15			( MASK_ALT   | K_F15 )
#define KEY_ALT_F16			( MASK_ALT   | K_F16 )

/*------------------------------------------------------------------------
 * alt number
 */
#define KEY_ALT_0			( MASK_ALT | '0' )
#define KEY_ALT_1			( MASK_ALT | '1' )
#define KEY_ALT_2			( MASK_ALT | '2' )
#define KEY_ALT_3			( MASK_ALT | '3' )
#define KEY_ALT_4			( MASK_ALT | '4' )
#define KEY_ALT_5			( MASK_ALT | '5' )
#define KEY_ALT_6			( MASK_ALT | '6' )
#define KEY_ALT_7			( MASK_ALT | '7' )
#define KEY_ALT_8			( MASK_ALT | '8' )
#define KEY_ALT_9			( MASK_ALT | '9' )

/*------------------------------------------------------------------------
 * alt letter
 */
#define KEY_ALT_A			( MASK_ALT | 'A' )
#define KEY_ALT_B			( MASK_ALT | 'B' )
#define KEY_ALT_C			( MASK_ALT | 'C' )
#define KEY_ALT_D			( MASK_ALT | 'D' )
#define KEY_ALT_E			( MASK_ALT | 'E' )
#define KEY_ALT_F			( MASK_ALT | 'F' )
#define KEY_ALT_G			( MASK_ALT | 'G' )
#define KEY_ALT_H			( MASK_ALT | 'H' )
#define KEY_ALT_I			( MASK_ALT | 'I' )
#define KEY_ALT_J			( MASK_ALT | 'J' )
#define KEY_ALT_K			( MASK_ALT | 'K' )
#define KEY_ALT_L			( MASK_ALT | 'L' )
#define KEY_ALT_M			( MASK_ALT | 'M' )
#define KEY_ALT_N			( MASK_ALT | 'N' )
#define KEY_ALT_O			( MASK_ALT | 'O' )
#define KEY_ALT_P			( MASK_ALT | 'P' )
#define KEY_ALT_Q			( MASK_ALT | 'Q' )
#define KEY_ALT_R			( MASK_ALT | 'R' )
#define KEY_ALT_S			( MASK_ALT | 'S' )
#define KEY_ALT_T			( MASK_ALT | 'T' )
#define KEY_ALT_U			( MASK_ALT | 'U' )
#define KEY_ALT_V			( MASK_ALT | 'V' )
#define KEY_ALT_W			( MASK_ALT | 'W' )
#define KEY_ALT_X			( MASK_ALT | 'X' )
#define KEY_ALT_Y			( MASK_ALT | 'Y' )
#define KEY_ALT_Z			( MASK_ALT | 'Z' )

/*------------------------------------------------------------------------
 * other special keys
 */
#define KEY_KSHIFT			( MASK_OTHER | 0x10 )
#define KEY_KCTRL			( MASK_OTHER | 0x11 )
#define KEY_KALT			( MASK_OTHER | 0x12 )
#define KEY_KFUNC			( MASK_OTHER | 0x13 )

#define KEY_NUM_LOCK		( MASK_OTHER | 0x14 )
#define KEY_CAPS_LOCK		( MASK_OTHER | 0x15 )
#define KEY_SCROLL_LOCK		( MASK_OTHER | 0x16 )

#define KEY_REFRESH			( MASK_OTHER | 0x17 )
#define KEY_PRINT_SCREEN	( MASK_OTHER | 0x18 )
#define KEY_SNAP			( MASK_OTHER | 0x19 )

#define KEY_HELP			( MASK_OTHER | 0x1a )
#define KEY_MOUSE			( MASK_OTHER | 0x1b )

#define KEY_WINRESIZE		( MASK_OTHER | 0x1c )
#define KEY_TIMER			( MASK_OTHER | 0x1d )

#define KEY_UNKNOWN			( MASK_OTHER | 0x1e )
#define KEY_READ_AGAIN		( MASK_OTHER | 0x1f )

#define KEY_SIGNAL			( MASK_OTHER | 0x20 )

/*------------------------------------------------------------------------
 * "standard" curses compatability definitions
 */
#define	KEY_BREAK		( MASK_CUR | 0x01 )		/* break key (unreliable)	*/
/* #define	KEY_DOWN	( MASK_CUR | 0x02 ) */	/* terminal down arrow key	*/
/* #define	KEY_UP		( MASK_CUR | 0x03 ) */	/* terminal up arrow key	*/
/* #define	KEY_LEFT	( MASK_CUR | 0x04 ) */	/* terminal left arrow key	*/
/* #define	KEY_RIGHT	( MASK_CUR | 0x05 ) */	/* terminal right arrow key	*/
/* #define	KEY_HOME	( MASK_CUR | 0x06 ) */	/* home key.				*/
#define	KEY_BACKSPACE	( MASK_CUR | 0x07 )		/* backspace key			*/
/* #define	KEY_F0		( MASK_CUR | 0x08 ) */	/* function key f0.			*/
/* #define	KEY_F(n)	(KEY_F0+(n)) */			/* Space for 64 func keys	*/
#define	KEY_DL			( MASK_CUR | 0x48 )		/* delete line key.			*/
#define	KEY_IL			( MASK_CUR | 0x49 )		/* insert line.				*/
#define	KEY_DC			( MASK_CUR | 0x4a )		/* delete character key.	*/
#define	KEY_IC			( MASK_CUR | 0x4b )		/* ins char/enter mode key.	*/
#define	KEY_EIC			( MASK_CUR | 0x4c )		/* rmir or smir in ins mode	*/
#define	KEY_CLEAR		( MASK_CUR | 0x4d )		/* clear screen/erase key	*/
#define	KEY_EOS			( MASK_CUR | 0x4e )		/* clear-to-end-of-screen	*/
#define	KEY_EOL			( MASK_CUR | 0x4f )		/* clear-to-end-of-line key	*/
#define	KEY_SF			( MASK_CUR | 0x50 )		/* scroll-forward/down key	*/
#define	KEY_SR			( MASK_CUR | 0x51 )		/* scroll-backward/up key	*/
#define	KEY_NPAGE		KEY_PGDN				/* next-page key			*/
#define	KEY_PPAGE		KEY_PGUP				/* previous-page key		*/
#define	KEY_STAB		( MASK_CUR | 0x54 )		/* set-tab key				*/
#define	KEY_CTAB		( MASK_CUR | 0x55 )		/* clear-tab key			*/
#define	KEY_CATAB		( MASK_CUR | 0x56 )		/* clear-all-tabs key.		*/
#define	KEY_ENTER		( MASK_CUR | 0x57 )		/* Enter/send (unreliable)	*/
#define	KEY_SRESET		( MASK_CUR | 0x58 )		/* soft reset (unreliable)	*/
#define	KEY_RESET		( MASK_CUR | 0x59 )		/* hard reset (unreliable)	*/
#define	KEY_PRINT		( MASK_CUR | 0x5a )		/* print or copy			*/
#define	KEY_LL			( MASK_CUR | 0x5b )		/* home-down key			*/
#define	KEY_A1			( MASK_CUR | 0x5c )		/* Upper left of keypad		*/
#define	KEY_A3			( MASK_CUR | 0x5d )		/* Upper right of keypad	*/
#define	KEY_B2			( MASK_CUR | 0x5e )		/* Center of keypad			*/
#define	KEY_C1			( MASK_CUR | 0x5f )		/* Lower left of keypad		*/
#define	KEY_C3			( MASK_CUR | 0x60 )		/* Lower right of keypad	*/
#define	KEY_BTAB		( MASK_CUR | 0x61 )		/* Back tab key				*/
#define	KEY_BEG			( MASK_CUR | 0x62 )		/* beg(inning) key			*/
#define	KEY_CANCEL		KEY_ESCAPE				/* cancel key				*/
#define	KEY_CLOSE		( MASK_CUR | 0x64 )		/* close key				*/
#define	KEY_COMMAND		( MASK_CUR | 0x65 )		/* cmd (command) key		*/
#define	KEY_COPY		( MASK_CUR | 0x66 )		/* copy key					*/
#define	KEY_CREATE		( MASK_CUR | 0x67 )		/* create key				*/
/* #define	KEY_END		( MASK_CUR | 0x68 ) */	/* end key					*/
#define	KEY_EXIT		( MASK_CUR | 0x69 )		/* exit key					*/
#define	KEY_FIND		( MASK_CUR | 0x6a )		/* find key					*/
/* #define	KEY_HELP	( MASK_CUR | 0x6b ) */	/* help key					*/
#define	KEY_MARK		( MASK_CUR | 0x6c )		/* mark key					*/
#define	KEY_MESSAGE		( MASK_CUR | 0x6d )		/* message key				*/
#define	KEY_MOVE		( MASK_CUR | 0x6e )		/* move key					*/
#define	KEY_NEXT		( MASK_CUR | 0x6f )		/* next object key			*/
#define	KEY_OPEN		( MASK_CUR | 0x70 )		/* open key					*/
#define	KEY_OPTIONS		( MASK_CUR | 0x71 )		/* options key				*/
#define	KEY_PREVIOUS	( MASK_CUR | 0x72 )		/* previous object key		*/
#define	KEY_REDO		( MASK_CUR | 0x73 )		/* redo key					*/
#define	KEY_REFERENCE	( MASK_CUR | 0x74 )		/* ref(erence) key			*/
/* #define	KEY_REFRESH	( MASK_CUR | 0x75 ) */	/* refresh key				*/
#define	KEY_REPLACE		( MASK_CUR | 0x76 )		/* replace key				*/
#define	KEY_RESTART		( MASK_CUR | 0x77 )		/* restart key				*/
#define	KEY_RESUME		( MASK_CUR | 0x78 )		/* resume key				*/
#define	KEY_SAVE		( MASK_CUR | 0x79 )		/* save key					*/
#define	KEY_SBEG		( MASK_CUR | 0x7a )		/* shifted beginning key	*/
#define	KEY_SCANCEL		( MASK_CUR | 0x7b )		/* shifted cancel key		*/
#define	KEY_SCOMMAND	( MASK_CUR | 0x7c )		/* shifted command key		*/
#define	KEY_SCOPY		( MASK_CUR | 0x7d )		/* shifted copy key			*/
#define	KEY_SCREATE		( MASK_CUR | 0x7e )		/* shifted create key		*/
#define	KEY_SDC			( MASK_CUR | 0x7f )		/* shifted delete char key	*/
#define	KEY_SDL			( MASK_CUR | 0x80 )		/* shifted delete line key	*/
#define	KEY_SELECT		( MASK_CUR | 0x81 )		/* select key				*/
#define	KEY_SEND		( MASK_CUR | 0x82 )		/* shifted end key			*/
#define	KEY_SEOL		( MASK_CUR | 0x83 )		/* shifted clear line key	*/
#define	KEY_SEXIT		( MASK_CUR | 0x84 )		/* shifted exit key			*/
#define	KEY_SFIND		( MASK_CUR | 0x85 )		/* shifted find key			*/
#define	KEY_SHELP		( MASK_CUR | 0x86 )		/* shifted help key			*/
#define	KEY_SHOME		( MASK_CUR | 0x87 )		/* shifted home key			*/
#define	KEY_SIC			( MASK_CUR | 0x88 )		/* shifted input key		*/
#define	KEY_SLEFT		( MASK_CUR | 0x89 )		/* shifted left arrow key	*/
#define	KEY_SMESSAGE	( MASK_CUR | 0x8a )		/* shifted message key		*/
#define	KEY_SMOVE		( MASK_CUR | 0x8b )		/* shifted move key			*/
#define	KEY_SNEXT		( MASK_CUR | 0x8c )		/* shifted next key			*/
#define	KEY_SOPTIONS	( MASK_CUR | 0x8d )		/* shifted options key		*/
#define	KEY_SPREVIOUS	( MASK_CUR | 0x8e )		/* shifted prev key			*/
#define	KEY_SPRINT		( MASK_CUR | 0x8f )		/* shifted print key		*/
#define	KEY_SREDO		( MASK_CUR | 0x90 )		/* shifted redo key			*/
#define	KEY_SREPLACE	( MASK_CUR | 0x91 )		/* shifted replace key		*/
#define	KEY_SRIGHT		( MASK_CUR | 0x92 )		/* shifted right arrow		*/
#define	KEY_SRSUME		( MASK_CUR | 0x93 )		/* shifted resume key		*/
#define	KEY_SSAVE		( MASK_CUR | 0x94 )		/* shifted save key			*/
#define	KEY_SSUSPEND	( MASK_CUR | 0x95 )		/* shifted suspend key		*/
#define	KEY_SUNDO		( MASK_CUR | 0x96 )		/* shifted undo key			*/
#define	KEY_SUSPEND		( MASK_CUR | 0x97 )		/* suspend key				*/
#define	KEY_UNDO		( MASK_CUR | 0x98 )		/* undo key					*/
/* #define	KEY_MOUSE	( MASK_CUR | 0x99 ) */	/* Mouse event occured		*/

/*------------------------------------------------------------------------
 * min & max special key definitions
 */
#define KEY_MIN				( MASK_REG  )		/* start key def arena	*/
#define KEY_MAX				( MASK_USER )		/* user  key def arena	*/

/*------------------------------------------------------------------------
 * generic function key definition
 *
 * Generated keys are:
 *
 *	KEY_F	KEY_FUNC	key-value
 *	-----	--------	---------------------
 *	 0-15	0-15, 0		      F1 to       F16
 *	16-31	0-15, 1		SHIFT-F1 to SHIFT-F16
 *	32-47	0-15, 2		 CTRL-F1 to  CTRL-F16
 *	48-63	0-15, 3		  ALT-F1 to   ALT-F16
 *
 * The compiler should reduce these macros to a constant, so they
 * can be used in a switch statement.
 */
#define KEY_FUNC(n,t)		( (K_F1 + (n)) | (MASK_REG << (t)) )
#define KEY_F(n)			KEY_FUNC((n) & 0x0f, ((n) & 0x30) >> 4)

#define KEY_F0				KEY_F(0)

/*------------------------------------------------------------------------
 * mouse actions (available when key input is KEY_MOUSE)
 */
#define MOUSE_LBU			( MASK_MOUSE | 0x00 )
#define MOUSE_LBD			( MASK_MOUSE | 0x01 )
#define MOUSE_LBDBL			( MASK_MOUSE | 0x02 )
#define MOUSE_MBU			( MASK_MOUSE | 0x03 )
#define MOUSE_MBD			( MASK_MOUSE | 0x04 )
#define MOUSE_MBDBL			( MASK_MOUSE | 0x05 )
#define MOUSE_RBU			( MASK_MOUSE | 0x06 )
#define MOUSE_RBD			( MASK_MOUSE | 0x07 )
#define MOUSE_RBDBL			( MASK_MOUSE | 0x08 )

#define MOUSE_MOVE			( MASK_MOUSE | 0x09 )
#define MOUSE_DRAG			( MASK_MOUSE | 0x0a )

#define MOUSE_SCROLLUP		( MASK_MOUSE | 0x0b )
#define MOUSE_SCROLLDN		( MASK_MOUSE | 0x0c )

#define MOUSE_LEFT			( MASK_MOUSE | 0x10 )
#define MOUSE_RIGHT			( MASK_MOUSE | 0x11 )
#define MOUSE_UP			( MASK_MOUSE | 0x12 )
#define MOUSE_DOWN			( MASK_MOUSE | 0x13 )

#define MOUSE_UL			( MASK_MOUSE | 0x14 )
#define MOUSE_UR			( MASK_MOUSE | 0x15 )
#define MOUSE_DL			( MASK_MOUSE | 0x16 )
#define MOUSE_DR			( MASK_MOUSE | 0x17 )

/*@ ---API--- */
/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* WKEYS_H */
