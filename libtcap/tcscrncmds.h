/*------------------------------------------------------------------------
 * list of screen cmds
 *
 * This header is public.
 */
#ifndef TCSCRNCMDS_H
#define TCSCRNCMDS_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * commands sent to the "screen"
 */
enum screen_cmd
{
	S_ZERO,

	/*--------------------------------------------------------------------
	 * misc entries
	 */
	S_BEG_MISC,

		S_DB,			/* database type			*/

	S_END_MISC,

	/*--------------------------------------------------------------------
	 * integer entries
	 */
	S_BEG_INTS,

		S_COLS,			/* max columns				*/
		S_ROWS,			/* max rows					*/
		S_TABS,			/* tab size					*/
		S_NB,			/* number of mouse btns		*/
		S_MDBL,			/* mouse dblclk-intvl		*/
		S_PRFX,			/* prefix interval			*/
		S_MC,			/* max colors				*/

	S_END_INTS,

	/*--------------------------------------------------------------------
	 * boolean entries
	 */
	S_BEG_BOOLS,

		S_AM,			/* auto-margin				*/
		S_ALC,			/* allow color				*/
		S_MD,			/* mouse display			*/
		S_IFC,			/* insert final char		*/
		S_ALA,			/* allow alt chars			*/
		S_SCN,			/* use scancodes			*/

	S_END_BOOLS,

	/*--------------------------------------------------------------------
	 * parameterized entries
	 */
	S_BEG_PARMS,

		/*----------------------------------------------------------------
		 * move cursor
		 */
		S_CM,			/* cursor motion			*/
		S_CM_TC,		/* cursor motion (termcap)	*/
		S_CM_TI,		/* cursor motion (terminfo)	*/

		/*----------------------------------------------------------------
		 * color string
		 */
		S_CS,			/* generic color			*/

		S_CSR,			/* reg color				*/
		S_CSR_TC,		/* reg color (termcap)		*/
		S_CSR_TI,		/* reg color (terminfo)		*/

		S_CSM,			/* map color				*/
		S_CSM_TC,		/* map color (termcap)		*/
		S_CSM_TI,		/* map color (terminfo)		*/

		/*----------------------------------------------------------------
		 * foreground color
		 */
		S_FG,			/* generic FG				*/

		S_FGR,			/* reg FG					*/
		S_FGR_TC,		/* reg FG (termcap)			*/
		S_FGR_TI,		/* reg FG (terminfo)		*/

		S_FGM,			/* map FG					*/
		S_FGM_TC,		/* map FG (termcap)			*/
		S_FGM_TI,		/* map FG (terminfo)		*/

		/*----------------------------------------------------------------
		 * background color
		 */
		S_BG,			/* generic BG				*/

		S_BGR,			/* reg BG					*/
		S_BGR_TC,		/* reg BG (termcap)			*/
		S_BGR_TI,		/* reg BG (terminfo)		*/

		S_BGM,			/* map BG					*/
		S_BGM_TC,		/* map BG (termcap)			*/
		S_BGM_TI,		/* map BG (terminfo)		*/

	S_END_PARMS,

	/*--------------------------------------------------------------------
	 * string entries
	 */
	S_BEG_STRINGS,

		/*----------------------------------------------------------------
		 * insert/delete lines
		 */
		S_AL,			/* add line					*/
		S_DL,			/* delete line				*/

		/*----------------------------------------------------------------
		 * insert/delete chars
		 */
		S_IC,			/* insert character			*/
		S_DC,			/* delete character			*/

		/*----------------------------------------------------------------
		 * bells
		 */
		S_BL,			/* bell						*/
		S_VB,			/* visible bell				*/

		/*----------------------------------------------------------------
		 * clear cmds
		 */
		S_CD,			/* clear to EOS				*/
		S_CE,			/* clear to EOL				*/
		S_CL,			/* clear screen				*/

		/*----------------------------------------------------------------
		 * cursor display
		 */
		S_VE,			/* cursor normal			*/
		S_VI,			/* cursor invisible			*/
		S_VS,			/* cursor very visible		*/

		/*----------------------------------------------------------------
		 * session init/end
		 */
		S_TI,			/* terminal init			*/
		S_TE,			/* terminal end				*/

		/*----------------------------------------------------------------
		 * window title
		 */
		S_TTS,			/* title begin				*/
		S_TTE,			/* title end				*/

		/*----------------------------------------------------------------
		 * iconification on/off
		 */
		S_ICC,			/* icon close				*/
		S_ICO,			/* icon open				*/

		S_ICBS,			/* beg icon bits			*/
		S_ICBE,			/* end icon bits			*/

		/*----------------------------------------------------------------
		 * alternate chars
		 */
		S_ACE,			/* alt-chars enable			*/
		S_ACD,			/* alt-chars disable		*/

		/*----------------------------------------------------------------
		 * keyboard mapping on/off
		 */
		S_KBO,			/* keyboard map on			*/
		S_KBF,			/* keyboard map off			*/

		/*----------------------------------------------------------------
		 * printer on/off
		 */
		S_PRO,			/* printer on				*/
		S_PRF,			/* printer off				*/

		/*----------------------------------------------------------------
		 * attributes on/off
		 */
		S_STS,			/* beg standout				*/
		S_STE,			/* end standout				*/

		S_ULS,			/* beg underline			*/
		S_ULE,			/* end underline			*/

		S_BDS,			/* beg bold					*/
		S_BDE,			/* end bold					*/

		S_BLS,			/* beg blink				*/
		S_BLE,			/* end blink				*/

		S_DMS,			/* beg dim					*/
		S_DME,			/* end dim					*/

		S_IVS,			/* beg invisible			*/
		S_IVE,			/* end invisible			*/

		S_PRS,			/* beg protected			*/
		S_PRE,			/* end protected			*/

		/*----------------------------------------------------------------
		 * reg fonts on/off
		 */
		S_RS1,			/* beg reg char set 1		*/
		S_RE1,			/* end reg char set 1		*/

		S_RS2,			/* beg reg char set 2		*/
		S_RE2,			/* end reg char set 2		*/

		S_RS3,			/* beg reg char set 3		*/
		S_RE3,			/* end reg char set 3		*/

		S_RS4,			/* beg reg char set 4		*/
		S_RE4,			/* end reg char set 4		*/

		/*----------------------------------------------------------------
		 * alt fonts on/off
		 */
		S_AS1,			/* beg alt char set 1		*/
		S_AE1,			/* end alt char set 1		*/

		S_AS2,			/* beg alt char set 2		*/
		S_AE2,			/* end alt char set 2		*/

		S_AS3,			/* beg alt char set 3		*/
		S_AE3,			/* end alt char set 3		*/

		S_AS4,			/* beg alt char set 4		*/
		S_AE4,			/* end alt char set 4		*/

		/*----------------------------------------------------------------
		 * mouse on/off
		 */
		S_MBEG,			/* mouse on					*/
		S_MEND,			/* mouse off				*/
		S_MSTR,			/* mouse input string		*/

		/*----------------------------------------------------------------
		 * miscellaneous
		 */
		S_SNAP,			/* screen snap taken		*/
		S_DESC,			/* description				*/
		S_ACS,			/* acs char string			*/

	S_END_STRINGS
};
typedef enum screen_cmd	SCRN_CMD;

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCSCRNCMDS_H */
