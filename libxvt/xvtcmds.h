/*------------------------------------------------------------------------
 * header for terminal emulator commands
 */
#ifndef XVTCMDS_H
#define XVTCMDS_H

/*------------------------------------------------------------------------
 * esc-seq data
 *
 *	For each entry:
 *
 *	1.	If mode is >= 0, then:
 *
 *		1.	vt_type is set to type if >= 0
 *		2.	vt_mode is set to that value
 *		3.	esc processing continues
 *
 *	2.	if mode == -1, then:
 *
 *		1.	vt_type is set to type if >= 0
 *		2.	if desc != 0, it is logged
 *		3.	if rtn  != 0, it is called
 *		4.	else invalid esc seq found
 *		5.	esc processing is terminated
 *
 * Note that these tables contain entries for all defined xterm
 * escape sequences, including those we do not process.
 * In general, an entry which just has a cmd-description is not
 * processed, but we log the sequence for debugging purposes.
 *
 * By adding a routine to call for a particular entry, you can
 * add processing for sequences not currently being processed.
 */
typedef void	XVT_CMD_RTN		(TERMDATA *td);
typedef void	XVT_OUT_RTN		(TERMDATA *td, int ch);

struct esc_data
{
	int				code;			/* letter code		*/
	int				mode;			/* new mode or -1	*/
	int				type;			/* type value		*/
	XVT_CMD_RTN *	rtn;			/* routine to call	*/
	const char *	desc;			/* cmd desc			*/
};
typedef struct esc_data ESC_DATA;

/*------------------------------------------------------------------------
 * escape cmd table
 */
#define DATA_NONE		0				/* no data						*/
#define DATA_CLEAN		1				/* data is clean				*/
#define DATA_ESCAPE		2				/* data ended by esc-seq		*/

struct esc_cmd
{
	const char *		name;			/* cmd name						*/
	const ESC_DATA *	cmds;			/* pointer to cmd table			*/
	const char *		codes;			/* char codes allowed			*/
	int					data;			/* data status					*/
	int					params;			/* max # params to process		*/
};
typedef struct esc_cmd ESC_CMD;

/*------------------------------------------------------------------------
 * control-char cmd tbl
 */
struct ctl_cmd
{
	int					ctl_char;		/* control char					*/
	XVT_CMD_RTN *		ctl_rtn;		/* routine to call				*/
};
typedef struct ctl_cmd	CTL_CMD;

/*------------------------------------------------------------------------
 * ANSI conformance levels
 */
#define ANSI_CONF_0		0x00
#define ANSI_CONF_1		0x01
#define ANSI_CONF_2		0x02
#define ANSI_CONF_3		0x04

#define CONF_VT52		( ANSI_CONF_0 )
#define CONF_VT100		( ANSI_CONF_1 )
#define CONF_VT200		( ANSI_CONF_1 | ANSI_CONF_2 )
#define CONF_VT300		( ANSI_CONF_1 | ANSI_CONF_2 | ANSI_CONF_3 )

/*------------------------------------------------------------------------
 * command table struct
 */
struct cmd_tbl
{
	const char *	tbl_name;			/* name of this table			*/
	const char *	term_type;			/* default TERM type			*/
	XVT_CMD_RTN *	init_rtn;			/* initialization routine		*/
	XVT_OUT_RTN *	out_rtn;			/* routine to output a char		*/
	XVT_OUT_RTN *	prt_rtn;			/* routine to print  a char		*/
	const CTL_CMD *	ctl_tbl;			/* table of control cmds		*/
	const ESC_CMD *	esc_tbl;			/* table of escape  cmds		*/
	int				conformance;		/* ANSI conformance level		*/
};

/*------------------------------------------------------------------------
 * command tables
 */
extern const CMD_TBL	xvt_vt52_cmdtbl;
extern const CMD_TBL	xvt_vt100_cmdtbl;
extern const CMD_TBL	xvt_vt200_cmdtbl;
extern const CMD_TBL	xvt_vt300_cmdtbl;
extern const CMD_TBL	xvt_xterm_cmdtbl;

extern const CMD_TBL *	xvt_cmds[];

#endif /* XVTCMDS_H */
