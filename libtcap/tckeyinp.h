/*------------------------------------------------------------------------
 * header for key input processing
 *
 * This header is public.
 */
#ifndef TCKEYINP_H
#define TCKEYINP_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * key log filenames
 */
#define TCAP_KEYS_LOG_BIN	"keys.log"	/* binary log file name	*/
#define TCAP_KEYS_LOG_TXT	"keys.txt"	/* text   log file name	*/

/*------------------------------------------------------------------------
 * functions
 */
extern int		tcap_kbd_get_key		(TERMINAL *tp, int interval,
											int mode, int xlate, int keypad);
extern int		tcap_kbd_chk_key		(TERMINAL *tp, int mode);

extern int		tcap_kbd_get_timeout	(const TERMINAL *tp);
extern int		tcap_kbd_set_timeout	(TERMINAL *tp, int bf);

extern int		tcap_kbd_debug			(TERMINAL *tp, const char *path,
											int bf, int text);

extern void		tcap_kbd_clr_ta			(TERMINAL *tp);
extern int		tcap_kbd_enq_ta			(TERMINAL *tp, int key, int front);
extern int		tcap_kbd_deq_ta			(TERMINAL *tp);

extern int		tcap_kbd_add_key		(TERMINAL *tp, const char *str,
											int code);

extern int		tcap_kbd_add_async		(TERMINAL *tp, int key,
											ASYNC_RTN *rtn, void *data);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCKEYINP_H */
