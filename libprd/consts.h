/*------------------------------------------------------------------------
 * this file contains all const variables for this library
 */
#ifndef CONSTS_H
#define CONSTS_H

/*------------------------------------------------------------------------
 * consts struct
 */
struct pgm_consts
{
	/*--------------------------------------------------------------------
	 * support filename extensions
	 */
	const char *	hlp_ext;			/* help file */
	const char *	mnu_ext;			/* app menu file */
	const char *	res_ext;			/* resource file */
	const char *	dev_ext;			/* archive device list file */
	const char *	ckp_ext;			/* checkpoint file */
	const char *	hst_ext;			/* history file */
	const char *	cfg_ext;			/* configuration file */
	const char *	clr_ext;			/* file-colors file */

	/*--------------------------------------------------------------------
	 * other misc variables
	 */
	const int		time_interval;		/* clock display wait interval */
	const int		kbd_wait_interval;	/* wait time for esc-sequences */
	const int		border_type;		/* border line type */
	const int		default_dir_line;	/* default dir-scroll line */

	/*--------------------------------------------------------------------
	 * misc display chars
	 */
	const int		tag_disp_char;		/* file is tagged */
	const int		too_long_char;		/* name too long (put at end) */
	const int		unprintable_char;	/* char is unprintable */
	const int		selected_char;		/* marks selected dir */

	/*--------------------------------------------------------------------
	 * dir flag display chars
	 */
	const int		hidden_dir_char;	/* dir is hidden */
	const int		unlogged_dir_char;	/* dir is not logged */
	const int		changed_dir_char;	/* dir changed since last log */

	/*--------------------------------------------------------------------
	 * file type display chars
	 */
	const int		reg_display_char;	/* regular file */
	const int		blk_display_char;	/* block special */
	const int		chr_display_char;	/* char special */
	const int		ffo_display_char;	/* FIFO */
	const int		nam_display_char;	/* named pipe */
	const int		sck_display_char;	/* socket */
	const int		sym_display_char;	/* symlink */
	const int		syd_display_char;	/* dead symlink */
	const int		dor_display_char;	/* door */
};
typedef struct pgm_consts PGM_CONSTS;

/*------------------------------------------------------------------------
 * consts data
 */
extern const PGM_CONSTS		pgm_consts;

#define pgm_const(n)		(pgm_consts.n)

#endif /* CONSTS_H */
