/*------------------------------------------------------------------------
 * header for terminal emulator routines
 */
#ifndef XVTEMUL_H
#define XVTEMUL_H

extern void			xvt_emul_char_input	(TERMDATA *td, int ch);
extern void			xvt_emul_local_echo	(TERMDATA *td, int ch);
extern int			xvt_emul_set_tbl	(TERMDATA *td, const char *name);
extern const char *	xvt_emul_get_term	(TERMDATA *td);

#endif /* XVTEMUL_H */
