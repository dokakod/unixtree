/*------------------------------------------------------------------------
 * current screen routines
 */
#ifndef XVTCURSCRN_H
#define XVTCURSCRN_H

/*------------------------------------------------------------------------
 * functions (these not only do the screen array but also the display)
 */
extern void		xvt_curscrn_set_vmargins	(TERMDATA *td, int top,  int bot);
extern void		xvt_curscrn_set_hmargins	(TERMDATA *td, int left, int right);
extern void		xvt_curscrn_clear			(TERMDATA *td, int prot);
extern void		xvt_curscrn_set				(TERMDATA *td, int c);

extern void		xvt_curscrn_chars_clr		(TERMDATA *td, int mode, int prot);
extern void		xvt_curscrn_chars_era		(TERMDATA *td, int n, int prot);
extern void		xvt_curscrn_chars_del		(TERMDATA *td, int n);
extern void		xvt_curscrn_chars_ins		(TERMDATA *td, int n);

extern void		xvt_curscrn_lines_clr		(TERMDATA *td, int y, int n,
												int prot);
extern void		xvt_curscrn_lines_cpy		(TERMDATA *td, int yt, int yf,
												int n);
extern void		xvt_curscrn_lines_del		(TERMDATA *td, int n);
extern void		xvt_curscrn_lines_ins		(TERMDATA *td, int n);

extern void		xvt_curscrn_cols_ins		(TERMDATA *td, int n);
extern void		xvt_curscrn_cols_del		(TERMDATA *td, int n);

extern void		xvt_curscrn_scroll			(TERMDATA *td, int n);

#endif /* XVTCURSCRN_H */
