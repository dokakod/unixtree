/*------------------------------------------------------------------------
 * keyboard stuff
 */
#ifndef TERMKBD_H
#define TERMKBD_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * function prototypes
 */
extern int		term_kbd_get1c		(TERM_DATA *t, int interval);
extern int		term_kbd_enq		(TERM_DATA *t, int key);
extern int		term_kbd_deq		(TERM_DATA *t);
extern int		term_kbd_empty		(TERM_DATA *t);
extern int		term_kbd_sig		(TERM_DATA *t, int sig);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TERMKBD_H */
