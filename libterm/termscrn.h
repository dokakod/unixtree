/*------------------------------------------------------------------------
 * screen device i/o stuff
 */
#ifndef TERMSCRN_H
#define TERMSCRN_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * event codes for busy routine
 */
#define TERM_EVENT_INIT		0			/* called when set				*/
#define TERM_EVENT_READ		1			/* prior to a read				*/
#define TERM_EVENT_WRITE	2			/* prior to a write				*/
#define TERM_EVENT_WAIT		3			/* just waiting for an event	*/

/*------------------------------------------------------------------------
 * function prototypes
 */
extern TERM_DATA *		term_screen_dev_init	(void);
extern void				term_screen_dev_free	(TERM_DATA *t);

extern int				term_screen_dev_open	(TERM_DATA *t,
													int inp, int out);
extern int				term_screen_dev_close	(TERM_DATA *t);

extern int				term_screen_dev_write	(TERM_DATA *t,
													const unsigned char *buf,
													int num);

extern int				term_screen_dev_busy	(TERM_DATA *t,
													int event, int ms);

extern int				term_screen_dev_evt		(TERM_DATA *t,
													TERM_EVT_RTN *rtn,
													void *data);

extern int				term_screen_dev_inp		(TERM_DATA *t,
													TERM_INP_RTN *rtn,
													void *data);

extern int				term_screen_dev_out		(TERM_DATA *t,
													TERM_OUT_RTN *rtn,
													void *data);

extern TERM_EVT_RTN *	term_screen_dev_evt_rtn	(TERM_DATA *t);
extern void *			term_screen_dev_evt_data(TERM_DATA *t);

extern TERM_INP_RTN *	term_screen_dev_inp_rtn	(TERM_DATA *t);
extern void *			term_screen_dev_inp_data(TERM_DATA *t);

extern TERM_OUT_RTN *	term_screen_dev_out_rtn	(TERM_DATA *t);
extern void *			term_screen_dev_out_data(TERM_DATA *t);

extern MOUSE_STATUS *	term_screen_dev_mouse	(TERM_DATA *t);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TERMSCRN_H */
