/*------------------------------------------------------------------------
 * header for VT100 commands
 */
#ifndef XVTVT100_H
#define XVTVT100_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * vt100 key tables
 */
extern const KEY_DATA	xvt_vt100_keys_fn_reg[];

extern const KEY_DATA	xvt_vt100_keys_ms_reg[];
extern const KEY_DATA	xvt_vt100_keys_ms_map[];

extern const KEY_DATA	xvt_vt100_keys_ct_reg[];
extern const KEY_DATA	xvt_vt100_keys_ct_map[];

extern const KEY_DATA	xvt_vt100_keys_kp_reg[];
extern const KEY_DATA	xvt_vt100_keys_kp_cur[];
extern const KEY_DATA	xvt_vt100_keys_kp_apl[];
extern const KEY_DATA	xvt_vt100_keys_kp_map[];

extern const KEY_DATA	xvt_vt100_keys_pf_reg[];
extern const KEY_DATA	xvt_vt100_keys_pf_map[];

extern const KEY_DATA	xvt_vt100_keys_st_up[];
extern const KEY_DATA	xvt_vt100_keys_st_dn[];

/*------------------------------------------------------------------------
 * escape-seq modes
 */
#define VT100_TOP		0						/* top-level processing		*/
#define VT100_CTL		1						/* control cmds				*/
#define VT100_CHS		2						/* char-set controls		*/
#define VT100_DEC		3						/* DEC mode					*/
#define VT100_GCS		4						/* Gx Char Set				*/
#define VT100_CSI		5						/* normal CSI found			*/
#define VT100_PRI		6						/* DECSET private			*/
#define VT100_OSC		7						/* Operating System Command	*/
#define VT100_DEV		8						/* special DEVICE mode		*/

/*------------------------------------------------------------------------
 * device-mode codes
 */
#define DEV_DCS			'P'						/* Device Control String	*/
#define DEV_PM			'^'						/* Privacy Message			*/
#define DEV_APC			'_'						/* App Program Command		*/
#define DEV_SOS			'W'						/* Start of String			*/

/*------------------------------------------------------------------------
 * XVT specific cmds
 *
 *	We define a number of XVT-specific cmds.  To keep compatible with
 *	xterm, we use undefined private codes, which are ignored by xterm
 *	(we hope).
 */
#define XVT_PRI_CODES		100					/* this range is unused		*/

#define XVT_PRI_KBD_OFF		(XVT_PRI_CODES + 0)	/* map-kbd off				*/
#define XVT_PRI_KBD_ONE		(XVT_PRI_CODES + 1)	/* map-kbd on				*/
#define XVT_PRI_KBD_TWO		(XVT_PRI_CODES + 2)	/* map-kbd on with prs/rls	*/

#define XVT_PRI_LBLINFO		(XVT_PRI_CODES + 3)	/* display info on label	*/

#define XVT_PRI_LOGGING		(XVT_PRI_CODES + 4)	/* generic logging			*/
#define XVT_PRI_LOG_EVT		(XVT_PRI_LOGGING + XVT_LOG_EVENTS)
#define XVT_PRI_LOG_ACT		(XVT_PRI_LOGGING + XVT_LOG_ACTION)
#define XVT_PRI_LOG_OUT		(XVT_PRI_LOGGING + XVT_LOG_OUTPUT)
#define XVT_PRI_LOG_SCR		(XVT_PRI_LOGGING + XVT_LOG_SCREEN)
#define XVT_PRI_LOG_INP		(XVT_PRI_LOGGING + XVT_LOG_INPKBD)
#define XVT_PRI_LOG_KEY		(XVT_PRI_LOGGING + XVT_LOG_KEYBRD)

#define XVT_CSI_TERM_CUR	10					/* set cursor type			*/

/*------------------------------------------------------------------------
 * misc routines
 */
extern XVT_CMD_RTN	xvt_vt100_init;

extern XVT_OUT_RTN	xvt_vt100_char_output;
extern XVT_OUT_RTN	xvt_vt100_char_print;

/*------------------------------------------------------------------------
 * These are cmd routines for different states
 */

/*------------------------------------------------------------------------
 * control-char routines
 */
extern XVT_CMD_RTN	xvt_vt100_CTL_ENQ;
extern XVT_CMD_RTN	xvt_vt100_CTL_BEL;
extern XVT_CMD_RTN	xvt_vt100_CTL_BS;
extern XVT_CMD_RTN	xvt_vt100_CTL_HT;
extern XVT_CMD_RTN	xvt_vt100_CTL_LF;
extern XVT_CMD_RTN	xvt_vt100_CTL_CR;
extern XVT_CMD_RTN	xvt_vt100_CTL_SO;
extern XVT_CMD_RTN	xvt_vt100_CTL_SI;

/*------------------------------------------------------------------------
 * TOP mode cmds	(top-level ESC seqs)
 */
extern XVT_CMD_RTN	xvt_vt100_TOP_DECSC;
extern XVT_CMD_RTN	xvt_vt100_TOP_DECRC;
extern XVT_CMD_RTN	xvt_vt100_TOP_DECKPAM;
extern XVT_CMD_RTN	xvt_vt100_TOP_DECKPNM;
extern XVT_CMD_RTN	xvt_vt100_TOP_RIS;
extern XVT_CMD_RTN	xvt_vt100_TOP_LS2;
extern XVT_CMD_RTN	xvt_vt100_TOP_LS3;
extern XVT_CMD_RTN	xvt_vt100_TOP_LS1R;
extern XVT_CMD_RTN	xvt_vt100_TOP_LS2R;
extern XVT_CMD_RTN	xvt_vt100_TOP_LS3R;
extern XVT_CMD_RTN	xvt_vt100_TOP_DEVEND;
extern XVT_CMD_RTN	xvt_vt100_TOP_DA;
extern XVT_CMD_RTN	xvt_vt100_TOP_CLL;
extern XVT_CMD_RTN	xvt_vt100_TOP_HTS;
extern XVT_CMD_RTN	xvt_vt100_TOP_SS2;
extern XVT_CMD_RTN	xvt_vt100_TOP_SS3;
extern XVT_CMD_RTN	xvt_vt100_TOP_IND;
extern XVT_CMD_RTN	xvt_vt100_TOP_RI;
extern XVT_CMD_RTN	xvt_vt100_TOP_NEL;
extern XVT_CMD_RTN	xvt_vt100_TOP_MEML;
extern XVT_CMD_RTN	xvt_vt100_TOP_MEMU;
extern XVT_CMD_RTN	xvt_vt100_TOP_SPA;
extern XVT_CMD_RTN	xvt_vt100_TOP_EPA;
extern XVT_CMD_RTN	xvt_vt100_TOP_DECFI;
extern XVT_CMD_RTN	xvt_vt100_TOP_DECBI;

/*------------------------------------------------------------------------
 * DEC mode cmds	(DEC specific cmds)
 */
extern XVT_CMD_RTN	xvt_vt100_DEC_DECALN;

/*------------------------------------------------------------------------
 * GCS mode cmds	(Graphic Character Set)
 */
extern XVT_CMD_RTN	xvt_vt100_GCS_CODE;

/*------------------------------------------------------------------------
 * CSI mode cmds	(Code Sequence Introducer)
 */
extern XVT_CMD_RTN	xvt_vt100_CSI_HPA;
extern XVT_CMD_RTN	xvt_vt100_CSI_VPA;
extern XVT_CMD_RTN	xvt_vt100_CSI_CUP;
extern XVT_CMD_RTN	xvt_vt100_CSI_ECH;
extern XVT_CMD_RTN	xvt_vt100_CSI_ED;
extern XVT_CMD_RTN	xvt_vt100_CSI_EL;
extern XVT_CMD_RTN	xvt_vt100_CSI_CUU;
extern XVT_CMD_RTN	xvt_vt100_CSI_CNL;
extern XVT_CMD_RTN	xvt_vt100_CSI_CUD;
extern XVT_CMD_RTN	xvt_vt100_CSI_CPL;
extern XVT_CMD_RTN	xvt_vt100_CSI_CUF;
extern XVT_CMD_RTN	xvt_vt100_CSI_CUB;
extern XVT_CMD_RTN	xvt_vt100_CSI_ICH;
extern XVT_CMD_RTN	xvt_vt100_CSI_DCH;
extern XVT_CMD_RTN	xvt_vt100_CSI_AL;
extern XVT_CMD_RTN	xvt_vt100_CSI_DL;
extern XVT_CMD_RTN	xvt_vt100_CSI_SM;
extern XVT_CMD_RTN	xvt_vt100_CSI_RM;
extern XVT_CMD_RTN	xvt_vt100_CSI_SGR;
extern XVT_CMD_RTN	xvt_vt100_CSI_DECSTBM;
extern XVT_CMD_RTN	xvt_vt100_CSI_DECSLRM;
extern XVT_CMD_RTN	xvt_vt100_CSI_TERM;
extern XVT_CMD_RTN	xvt_vt100_CSI_MC;
extern XVT_CMD_RTN	xvt_vt100_CSI_DA;
extern XVT_CMD_RTN	xvt_vt100_CSI_DSR;
extern XVT_CMD_RTN	xvt_vt100_CSI_TBC;
extern XVT_CMD_RTN	xvt_vt100_CSI_TBS;
extern XVT_CMD_RTN	xvt_vt100_CSI_CHT;
extern XVT_CMD_RTN	xvt_vt100_CSI_CBT;
extern XVT_CMD_RTN	xvt_vt100_CSI_SU;
extern XVT_CMD_RTN	xvt_vt100_CSI_SD;
extern XVT_CMD_RTN	xvt_vt100_CSI_SL;
extern XVT_CMD_RTN	xvt_vt100_CSI_SR;
extern XVT_CMD_RTN	xvt_vt100_CSI_DECSTR;
extern XVT_CMD_RTN	xvt_vt100_CSI_DECSCL;
extern XVT_CMD_RTN	xvt_vt100_CSI_DECSCA;
extern XVT_CMD_RTN	xvt_vt100_CSI_REP;
extern XVT_CMD_RTN	xvt_vt100_CSI_DECREQT;
extern XVT_CMD_RTN	xvt_vt100_CSI_DECDC;
extern XVT_CMD_RTN	xvt_vt100_CSI_DECIC;
extern XVT_CMD_RTN	xvt_vt100_CSI_DECTST;

/*------------------------------------------------------------------------
 * PRI mode cmds	(DEC private sequences)
 */
extern XVT_CMD_RTN	xvt_vt100_PRI_DECMC;
extern XVT_CMD_RTN	xvt_vt100_PRI_DECSED;
extern XVT_CMD_RTN	xvt_vt100_PRI_DECSEL;
extern XVT_CMD_RTN	xvt_vt100_PRI_DECDSR;
extern XVT_CMD_RTN	xvt_vt100_PRI_DECSET;
extern XVT_CMD_RTN	xvt_vt100_PRI_DECRST;
extern XVT_CMD_RTN	xvt_vt100_PRI_DECSAV;
extern XVT_CMD_RTN	xvt_vt100_PRI_DECRES;
extern XVT_CMD_RTN	xvt_vt100_PRI_DECTGL;

/*------------------------------------------------------------------------
 * OSC mode cmds	(Operating System Commands)
 */
extern XVT_CMD_RTN	xvt_vt100_OSC_LABEL;

/*------------------------------------------------------------------------
 * DEV mode cmds	(Device Specific Commands)
 */

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* XVTVT100_H */
