/*------------------------------------------------------------------------
 *	Screen definition header
 *
 *	This is where all info regarding the current screen state
 *	is kept, i.e. this struct contains all information which
 *	changes during the use of the display.
 *
 * This header is private to this library only.
 */
#ifndef TCSCRNDATA_H
#define TCSCRNDATA_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * ASKEY struct
 */
typedef struct askey ASKEY;
struct askey
{
	ASKEY *			next;		/* pointer to next entry		*/
	int				key;		/* key value					*/
	ASYNC_RTN *		rtn;		/* routine to call				*/
	void *			data;		/* user data					*/
};

/*------------------------------------------------------------------------
 * SIG_INFO struct
 */
struct sig_info
{
	SIG_RTN *	sig_rtn;		/* routine to call				*/
	void *		sig_data;		/* pointer to user data			*/
};
typedef struct sig_info SIG_INFO;

/*------------------------------------------------------------------------
 * screen data structure
 */
struct scrn_data
{
	/*--------------------------------------------------------------------
	 * current mode
	 */
	int				mode;					/* TRUE if not in orig mode	*/

	/*--------------------------------------------------------------------
	 * keyboard state info
	 */
	int				num_lock;				/* TRUE if num-lock    on	*/
	int				caps_lock;				/* TRUE if caps-lock   on	*/
	int				scroll_lock;			/* TRUE if scroll-lock on	*/

	/*--------------------------------------------------------------------
	 * char-set info
	 */
	int				alt_chars_on;			/* TRUE if alt char set	on	*/
	int				curr_reg_font;			/* current reg font number	*/
	int				curr_alt_font;			/* current alt font number	*/

	/*--------------------------------------------------------------------
	 * screen attributes
	 */
	int				visibility;				/* cursor state				*/

	attr_t			curr_attr;				/* current  attrs of screen	*/
	attr_t			orig_attr;				/* original attrs of screen	*/

	int				color_flag;				/* TRUE if in color			*/
	int				disp_in_window;			/* TRUE if in window (X11)	*/

	/*--------------------------------------------------------------------
	 * screen position
	 */
	int				curr_col;				/* current col position		*/
	int				curr_row;				/* current row position		*/

	/*--------------------------------------------------------------------
	 * timers
	 */
	int				cur_prefix_intvl;		/* current prefix interval	*/
	int				alarm_time_left;		/* cached alarm time		*/

	/*--------------------------------------------------------------------
	 * mouse info
	 */
	int				mouse_restore;			/* TRUE if mouse being used	*/

	/*--------------------------------------------------------------------
	 * input/output debug files
	 */
	char			debug_keys_path[MAX_PATHLEN];	/* keys pathname	*/
	FILE *			debug_keys_fp;			/* key debug stream			*/
	int				debug_keys_text;		/* TRUE for text debug log	*/

	char			debug_scrn_path[MAX_PATHLEN];	/* scrn pathname	*/
	FILE *			debug_scrn_fp;			/* screen debug stream		*/
	int				debug_scrn_text;		/* TRUE for text debug log	*/
	int				debug_scrn_mode;		/* TRUE if cmd was last out	*/

	/*--------------------------------------------------------------------
	 * output buffer
	 */
	int				out_buf_count;			/* # chars in out_buf		*/
	int				out_buf_size;			/* size of out_buf			*/
	unsigned char	out_buf[BUFSIZ];		/* output buffer for device	*/

	/*--------------------------------------------------------------------
	 * input key buffer
	 */
	int				ta_count;				/* # keys in ta_buf			*/
	int				ta_buf[256];			/* type ahead buffer		*/

	/*--------------------------------------------------------------------
	 * function key buffer
	 */
	int				fk_count;				/* # chars in fk_buf		*/
	unsigned char	fk_buf[64];				/* queued key buffer		*/

	/*--------------------------------------------------------------------
	 * cached window title buffer
	 */
	char			win_title[64];			/* cached window title		*/

	/*--------------------------------------------------------------------
	 * async key table
	 */
	ASKEY *			askey_tbl;				/* list of async keys		*/

	/*--------------------------------------------------------------------
	 * signal table
	 */
	SIG_INFO		sig_tbl[NSIG];			/* signal table				*/
};
typedef struct scrn_data SCRN_DATA;

/*------------------------------------------------------------------------
 * functions
 */
extern SCRN_DATA *	tcap_init_screen_data	(void);
extern void			tcap_free_screen_data	(SCRN_DATA *s);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCSCRNDATA_H */
