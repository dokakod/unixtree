/*------------------------------------------------------------------------
 * header for VT52 cmds
 */
#ifndef XVTVT52_H
#define XVTVT52_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * vt52 key tables
 */
extern const KEY_DATA	xvt_vt52_keys_fn_reg[];

extern const KEY_DATA	xvt_vt52_keys_ms_reg[];
extern const KEY_DATA	xvt_vt52_keys_ms_map[];

extern const KEY_DATA	xvt_vt52_keys_ct_reg[];
extern const KEY_DATA	xvt_vt52_keys_ct_map[];

extern const KEY_DATA	xvt_vt52_keys_kp_reg[];
extern const KEY_DATA	xvt_vt52_keys_kp_cur[];
extern const KEY_DATA	xvt_vt52_keys_kp_apl[];
extern const KEY_DATA	xvt_vt52_keys_kp_map[];

extern const KEY_DATA	xvt_vt52_keys_pf_reg[];
extern const KEY_DATA	xvt_vt52_keys_pf_map[];

extern const KEY_DATA	xvt_vt52_keys_st_up[];
extern const KEY_DATA	xvt_vt52_keys_st_dn[];

/*------------------------------------------------------------------------
 * escape-seq modes
 */
#define VT52_TOP		0						/* top-level processing		*/
#define VT52_PRM		1						/* cmds with params			*/

/*------------------------------------------------------------------------
 * misc routines
 */
extern XVT_CMD_RTN	xvt_vt52_init;

extern XVT_OUT_RTN	xvt_vt52_char_output;
extern XVT_OUT_RTN	xvt_vt52_char_print;

/*------------------------------------------------------------------------
 * These are cmd routines for different states
 */

/*------------------------------------------------------------------------
 * control-char routines
 */
extern XVT_CMD_RTN	xvt_vt52_CTL_ENQ;
extern XVT_CMD_RTN	xvt_vt52_CTL_BEL;
extern XVT_CMD_RTN	xvt_vt52_CTL_BS;
extern XVT_CMD_RTN	xvt_vt52_CTL_HT;
extern XVT_CMD_RTN	xvt_vt52_CTL_LF;
extern XVT_CMD_RTN	xvt_vt52_CTL_CR;
extern XVT_CMD_RTN	xvt_vt52_CTL_SO;
extern XVT_CMD_RTN	xvt_vt52_CTL_SI;

/*------------------------------------------------------------------------
 * TOP mode cmds	(top-level ESC seqs)
 */
extern XVT_CMD_RTN	xvt_vt52_TOP_CUU;
extern XVT_CMD_RTN	xvt_vt52_TOP_CUD;
extern XVT_CMD_RTN	xvt_vt52_TOP_CUF;
extern XVT_CMD_RTN	xvt_vt52_TOP_CUB;
extern XVT_CMD_RTN	xvt_vt52_TOP_CUH;
extern XVT_CMD_RTN	xvt_vt52_TOP_RI;
extern XVT_CMD_RTN	xvt_vt52_TOP_ED;
extern XVT_CMD_RTN	xvt_vt52_TOP_EL;
extern XVT_CMD_RTN	xvt_vt52_TOP_DA;
extern XVT_CMD_RTN	xvt_vt52_TOP_DECKPAM;
extern XVT_CMD_RTN	xvt_vt52_TOP_DECKPNM;
extern XVT_CMD_RTN	xvt_vt52_TOP_DECANM;
extern XVT_CMD_RTN	xvt_vt52_TOP_DECSAP;
extern XVT_CMD_RTN	xvt_vt52_TOP_DECEAP;
extern XVT_CMD_RTN	xvt_vt52_TOP_DECSPC;
extern XVT_CMD_RTN	xvt_vt52_TOP_DECEPC;
extern XVT_CMD_RTN	xvt_vt52_TOP_DECPSC;
extern XVT_CMD_RTN	xvt_vt52_TOP_DECPCL;
extern XVT_CMD_RTN	xvt_vt52_TOP_SGM;
extern XVT_CMD_RTN	xvt_vt52_TOP_EGM;

/*------------------------------------------------------------------------
 * PRM mode cmds	(parameter ESC seqs)
 */
extern XVT_CMD_RTN	xvt_vt52_PRM_CUP;

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* XVTVT52_H */
