/*------------------------------------------------------------------------
 * xvt logging routines
 */
#ifndef XVTLOGS_H
#define XVTLOGS_H

/*------------------------------------------------------------------------
 * log types
 */
#define XVT_LOG_EVENTS		0			/* text		- all events			*/
#define XVT_LOG_ACTION		1			/* text		- xlated X events		*/
#define XVT_LOG_OUTPUT		2			/* text		- xlated child output	*/
#define XVT_LOG_SCREEN		3			/* binary	- raw    child output	*/
#define XVT_LOG_INPKBD		4			/* text		- xlated child input	*/
#define XVT_LOG_KEYBRD		5			/* binary	- raw    child input	*/
#define XVT_LOG_RESFIL		6			/* resource file log				*/
#define XVT_LOG_TKOPTS		7			/* tk options dump					*/

/*------------------------------------------------------------------------
 * set a path for a log file
 */
extern void		xvt_log_path	(TERMDATA *td, int log, const char *path);

/*------------------------------------------------------------------------
 * change log state
 */
extern void		xvt_log_set		(TERMDATA *td, int log, int on);

/*------------------------------------------------------------------------
 * functions to query logging
 */
extern int		xvt_log_is_on	(TERMDATA *td, int log);

/*------------------------------------------------------------------------
 * events logging
 *
 * text log of all events
 */
extern void		xvt_log_events_fmt		(TERMDATA *td, const char *event,
											const char *fmt, ...);

extern void		xvt_log_events_var		(TERMDATA *td, const char *event,
											const char *fmt, va_list args);

extern void		xvt_log_events_buf		(TERMDATA *td, const char *event,
											const unsigned char *buf, int num);

extern void		xvt_log_events_str		(TERMDATA *td, const char *event,
											const char *str);

/*------------------------------------------------------------------------
 * output logging
 *
 * text log of child output
 */
extern void		xvt_log_output_seq		(TERMDATA *td,
											const char *fmt, ...);

extern void		xvt_log_output_chr		(TERMDATA *td,
											const char *fmt, ...);

/*------------------------------------------------------------------------
 * action logging
 *
 * text log of translated X actions
 */
extern void		xvt_log_action_var		(TERMDATA *td,
											const char *fmt, va_list args);

/*------------------------------------------------------------------------
 * screen logging
 *
 * binary log of child output
 */
extern void		xvt_log_screen_buf		(TERMDATA *td,
											const unsigned char *buf, int num);

/*------------------------------------------------------------------------
 * inpkbd logging
 *
 * text log of child input
 */
extern void		xvt_log_inpkbd_fmt		(TERMDATA *td,
											const char *fmt, ...);

/*------------------------------------------------------------------------
 * keybrd logging
 *
 * binary log of child input
 */
extern void		xvt_log_keybrd_buf		(TERMDATA *td,
											const char *buf, int num);

#endif /* XVTLOGS_H */
