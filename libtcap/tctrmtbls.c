/*------------------------------------------------------------------------
 *	This module contains all tables and strings used by
 *	the various routines in this library.
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * internal global variables
 */
const char *	tcap_keyword_term	= "term";
const char *	tcap_keyword_map_fg	= "map-fg-color";
const char *	tcap_keyword_map_bg	= "map-bg-color";
const char *	tcap_keyword_map	= "map-color";
const char *	tcap_keyword_dbg	= "debug";
const char *	tcap_keyword_key	= "define-key";
const char *	tcap_keyword_dbtype	= "db-type";

/*------------------------------------------------------------------------
 *	This table is the list of keys we will look for
 *	in the termcap/terminfo database.
 */
const KF_TABLE tcap_tc_keys[] =
{
/*	  termcap-name	terminfo-name	s	key-value		*/

	{ "k1",			"kf1",			0,	KEY_F1			},
	{ "k2",			"kf2",			0,	KEY_F2			},
	{ "k3",			"kf3",			0,	KEY_F3			},
	{ "k4",			"kf4",			0,	KEY_F4			},
	{ "k5",			"kf5",			0,	KEY_F5			},
	{ "k6",			"kf6",			0,	KEY_F6			},
	{ "k7",			"kf7",			0,	KEY_F7			},
	{ "k8",			"kf8",			0,	KEY_F8			},
	{ "k9",			"kf9",			0,	KEY_F9			},
	{ "k0",			"kf0",			0,	KEY_F10			},
	{ "k;",			"kf10",			0,	KEY_F10			},
	{ "F1",			"kf11",			0,	KEY_F11			},
	{ "F2",			"kf12",			0,	KEY_F12			},

	{ "ku",			"kcuu1",		0,	KEY_UP			},
	{ "kd",			"kcud1",		0,	KEY_DOWN		},
	{ "kl",			"kcub1",		0,	KEY_LEFT		},
	{ "kr",			"kcuf1",		0,	KEY_RIGHT		},
	{ "kh",			"khome",		0,	KEY_HOME		},
	{ "ke",			"kend",			0,	KEY_END			},
	{ "kP",			"kpp",			0,	KEY_PGUP		},
	{ "kN",			"knp",			0,	KEY_PGDN		},

	{ "ki",			"ki",			0,	KEY_INSERT		},
	{ "kD",			"kdch1",		0,	KEY_DELETE		},

	/* curses compatability entries */

#if 0
	{ "K1",		"ka1",				0, KEY_A1			},
	{ "K3",		"ka3",				0, KEY_A3			},
	{ "K2",		"kb2",				0, KEY_B2			},
	{ "kb",		"kbs",				0, KEY_BACKSPACE	},
	{ "@1",		"kbeg",				0, KEY_BEG			},
	{ "kB",		"kcbt",				0, KEY_BTAB			},
	{ "K4",		"kc1",				0, KEY_C1			},
	{ "K5",		"kc3",				0, KEY_C3			},
	{ "@2",		"kcan",				0, KEY_CANCEL		},
	{ "ka",		"ktbc",				0, KEY_CATAB		},
	{ "kC",		"kclr",				0, KEY_CLEAR		},
	{ "@3",		"kclo",				0, KEY_CLOSE		},
	{ "@4",		"kcmd",				0, KEY_COMMAND		},
	{ "@5",		"kcpy",				0, KEY_COPY			},
	{ "@6",		"kcrt",				0, KEY_CREATE		},
	{ "kt",		"kctab",			0, KEY_CTAB			},
	{ "kD",		"kdch1",			0, KEY_DC			},
	{ "kL",		"kdl1",				0, KEY_DL			},
	{ "kM",		"krmir",			0, KEY_EIC			},
	{ "@7",		"kend",				0, KEY_END			},
	{ "@8",		"kent",				0, KEY_ENTER		},
	{ "kE",		"kel",				0, KEY_EOL			},
	{ "kS",		"ked",				0, KEY_EOS			},
	{ "@9",		"kext",				0, KEY_EXIT			},
	{ "k0",		"kf0",				0, KEY_F(0)			},
	{ "k1",		"kf1",				0, KEY_F(1)			},
	{ "k2",		"kf2",				0, KEY_F(2)			},
	{ "k3",		"kf3",				0, KEY_F(3)			},
	{ "k4",		"kf4",				0, KEY_F(4)			},
	{ "k5",		"kf5",				0, KEY_F(5)			},
	{ "k6",		"kf6",				0, KEY_F(6)			},
	{ "k7",		"kf7",				0, KEY_F(7)			},
	{ "k8",		"kf8",				0, KEY_F(8)			},
	{ "k9",		"kf9",				0, KEY_F(9)			},
	{ "k;",		"kf10",				0, KEY_F(10)		},
	{ "F1",		"kf11",				0, KEY_F(11)		},
	{ "F2",		"kf12",				0, KEY_F(12)		},
	{ "F3",		"kf13",				0, KEY_F(13)		},
	{ "F4",		"kf14",				0, KEY_F(14)		},
	{ "F5",		"kf15",				0, KEY_F(15)		},
	{ "F6",		"kf16",				0, KEY_F(16)		},
	{ "F7",		"kf17",				0, KEY_F(17)		},
	{ "F8",		"kf18",				0, KEY_F(18)		},
	{ "F9",		"kf19",				0, KEY_F(19)		},
	{ "FA",		"kf20",				0, KEY_F(20)		},
	{ "FB",		"kf21",				0, KEY_F(21)		},
	{ "FC",		"kf22",				0, KEY_F(22)		},
	{ "FD",		"kf23",				0, KEY_F(23)		},
	{ "FE",		"kf24",				0, KEY_F(24)		},
	{ "FF",		"kf25",				0, KEY_F(25)		},
	{ "FG",		"kf26",				0, KEY_F(26)		},
	{ "FH",		"kf27",				0, KEY_F(27)		},
	{ "FI",		"kf28",				0, KEY_F(28)		},
	{ "FJ",		"kf29",				0, KEY_F(29)		},
	{ "FK",		"kf30",				0, KEY_F(30)		},
	{ "FL",		"kf31",				0, KEY_F(31)		},
	{ "FM",		"kf32",				0, KEY_F(32)		},
	{ "FN",		"kf33",				0, KEY_F(13)		},
	{ "FO",		"kf34",				0, KEY_F(34)		},
	{ "FP",		"kf35",				0, KEY_F(35)		},
	{ "FQ",		"kf36",				0, KEY_F(36)		},
	{ "FR",		"kf37",				0, KEY_F(37)		},
	{ "FS",		"kf38",				0, KEY_F(38)		},
	{ "FT",		"kf39",				0, KEY_F(39)		},
	{ "FU",		"kf40",				0, KEY_F(40)		},
	{ "FV",		"kf41",				0, KEY_F(41)		},
	{ "FW",		"kf42",				0, KEY_F(42)		},
	{ "FX",		"kf43",				0, KEY_F(43)		},
	{ "FY",		"kf44",				0, KEY_F(44)		},
	{ "FZ",		"kf45",				0, KEY_F(45)		},
	{ "Fa",		"kf46",				0, KEY_F(46)		},
	{ "Fb",		"kf47",				0, KEY_F(47)		},
	{ "Fc",		"kf48",				0, KEY_F(48)		},
	{ "Fd",		"kf49",				0, KEY_F(49)		},
	{ "Fe",		"kf50",				0, KEY_F(50)		},
	{ "Ff",		"kf51",				0, KEY_F(51)		},
	{ "Fg",		"kf52",				0, KEY_F(52)		},
	{ "Fh",		"kf53",				0, KEY_F(53)		},
	{ "Fi",		"kf54",				0, KEY_F(54)		},
	{ "Fj",		"kf55",				0, KEY_F(55)		},
	{ "Fk",		"kf56",				0, KEY_F(56)		},
	{ "Fl",		"kf57",				0, KEY_F(57)		},
	{ "Fm",		"kf58",				0, KEY_F(58)		},
	{ "Fn",		"kf59",				0, KEY_F(59)		},
	{ "Fo",		"kf60",				0, KEY_F(60)		},
	{ "Fp",		"kf61",				0, KEY_F(61)		},
	{ "Fq",		"kf62",				0, KEY_F(62)		},
	{ "Fr",		"kf63",				0, KEY_F(63)		},
	{ "@0",		"kfnd",				0, KEY_FIND			},
	{ "%1",		"khlp",				0, KEY_HELP			},
	{ "kI",		"kich1",			0, KEY_IC			},
	{ "kA",		"kil1",				0, KEY_IL			},
	{ "kH",		"kll",				0, KEY_LL			},
	{ "%2",		"kmrk",				0, KEY_MARK			},
	{ "%3",		"kmsg",				0, KEY_MESSAGE		},
	{ "Km",		"kmous",			0, KEY_MOUSE		},
	{ "%4",		"kmov",				0, KEY_MOVE			},
	{ "%5",		"knxt",				0, KEY_NEXT			},
	{ "%6",		"kopn",				0, KEY_OPEN			},
	{ "%7",		"kopt",				0, KEY_OPTIONS		},
	{ "%8",		"kprv",				0, KEY_PREVIOUS		},
	{ "%9",		"kprt",				0, KEY_PRINT		},
	{ "%0",		"krdo",				0, KEY_REDO			},
	{ "&1",		"kref",				0, KEY_REFERENCE	},
	{ "&2",		"krfr",				0, KEY_REFRESH		},
	{ "&3",		"krpl",				0, KEY_REPLACE		},
	{ "&4",		"krst",				0, KEY_RESTART		},
	{ "&5",		"kres",				0, KEY_RESUME		},
	{ "&6",		"ksav",				0, KEY_SAVE			},
	{ "&9",		"kBEG",				0, KEY_SBEG			},
	{ "&0",		"kCAN",				0, KEY_SCANCEL		},
	{ "k1",		"kCMD",				0, KEY_SCOMMAND		},
	{ "k2",		"kCPY",				0, KEY_SCOPY		},
	{ "k3",		"kCRT",				0, KEY_SCREATE		},
	{ "*4",		"kDC",				0, KEY_SDC			},
	{ "*5",		"kDL",				0, KEY_SDL			},
	{ "*6",		"kslt",				0, KEY_SELECT		},
	{ "*7",		"kEND",				0, KEY_SEND			},
	{ "*8",		"kEOL",				0, KEY_SEOL			},
	{ "*9",		"kEXT",				0, KEY_SEXIT		},
	{ "kF",		"kind",				0, KEY_SF			},
	{ "*0",		"kFND",				0, KEY_SFIND		},
	{ "#1",		"kHLP",				0, KEY_SHELP		},
	{ "#2",		"kHOM",				0, KEY_SHOME		},
	{ "#3",		"kIC",				0, KEY_SIC			},
	{ "#4",		"kLFT",				0, KEY_SLEFT		},
	{ "%a",		"kMSG",				0, KEY_SMESSAGE		},
	{ "%b",		"kMOV",				0, KEY_SMOVE		},
	{ "%c",		"kNXT",				0, KEY_SNEXT		},
	{ "%d",		"kOPT",				0, KEY_SOPTIONS		},
	{ "%e",		"kPRV",				0, KEY_SPREVIOUS	},
	{ "%f",		"kPRT",				0, KEY_SPRINT		},
	{ "kR",		"kri",				0, KEY_SR			},
	{ "%g",		"kRDO",				0, KEY_SREDO		},
	{ "%h",		"kRPL",				0, KEY_SREPLACE		},
	{ "%i",		"kRIT",				0, KEY_SRIGHT		},
	{ "%j",		"kRES",				0, KEY_SRSUME		},
	{ "!1",		"kSAV",				0, KEY_SSAVE		},
	{ "!2",		"kSPD",				0, KEY_SSUSPEND		},
	{ "kT",		"khts",				0, KEY_STAB			},
	{ "!3",		"kUND",				0, KEY_SUNDO		},
	{ "&7",		"kspd",				0, KEY_SUSPEND		},
	{ "&8",		"kund",				0, KEY_UNDO			},
#endif

	{ 0, 0, 0, 0 }
};

/*------------------------------------------------------------------------
 *	This is a list of default definitions for keys
 *	for very dumb terminals which don't have them.
 */
const KF_TABLE tcap_dumb_keys[] =
{
	{ 0, 0, "\035",		KEY_KFUNC	},	/* ^] as prefix key		*/

#if V_UNIX
	{ 0, 0, "\033\033",	KEY_ESCAPE	},	/* <ESC><ESC> as escape	*/
#endif

	{ 0, 0, 0, 0 }
};

/*------------------------------------------------------------------------
 *	Termcap data table
 *
 *	These entries are the termcap-type strings.
 *	Those which have corrresponding termcap/terminfo entries are so noted.
 */
#define O(field)		X_OFFSET_OF(TCAP_DATA,field)

const TC_DATA tcap_termcap_data[] =
{
	/*--------------------------------------------------------------------
	 * numeric entries
	 */
	{ "cols",				TC_NUM,	"co", "cols" ,	O(ints.maxcols), S_COLS	},
	{ "lines",				TC_NUM, "li", "lines",	O(ints.maxrows), S_ROWS	},
	{ "tabsize",			TC_NUM,	"it", "it",		O(ints.tabsize), S_TABS	},

	{ "mouse-buttons",		TC_NUM, NULL, NULL,		O(ints.nb),		 S_NB	},
	{ "mouse-dblclk",		TC_NUM, NULL, NULL,		O(ints.dblclk_intvl),
																	 S_MDBL	},

	{ "prefix-interval",	TC_NUM, NULL, NULL,		O(ints.prefix_intvl),
																	 S_PRFX	},

	{ "max-colors",			TC_NUM, "Co", "colors",	O(ints.mc),		 S_MC	},


	/*--------------------------------------------------------------------
	 * boolean entries
	 */
	{ "auto-margin",		TC_BLN,	"am", "am",		O(bools.am),	S_AM	},
	{ "allow-color",		TC_BLN,	NULL, NULL,		O(bools.alc),	S_ALC	},
	{ "mouse-display",		TC_BLN,	NULL, NULL,		O(bools.md),	S_MD	},
	{ "ins-final-char",		TC_BLN,	NULL, NULL,		O(bools.ifc),	S_IFC	},
	{ "allow-alt-chars",	TC_BLN,	NULL, NULL,		O(bools.ala),	S_ALA	},
	{ "use-scancodes",		TC_BLN,	NULL, NULL,		O(bools.scn),	S_SCN	},

	/*--------------------------------------------------------------------
	 * parameter entries
	 */
	{ "move-cursor",		TC_PRM, NULL, NULL,		O(parms.cm),	S_CM	},
	{ "move-cursor-tc",		TC_PRM, "cm", NULL,		O(parms.cm_tc),	S_CM_TC	},
	{ "move-cursor-ti",		TC_PRM, NULL, "cup",	O(parms.cm_ti),	S_CM_TI	},

	{ "set-color-str",		TC_PRM, NULL, NULL,		O(parms.csr),	S_CSR	 },
	{ "set-color-str-tc",	TC_PRM, NULL, NULL,		O(parms.csr_tc),S_CSR_TC },
	{ "set-color-str-ti",	TC_PRM, NULL, NULL,		O(parms.csr_ti),S_CSR_TI },

	{ "map-color-str",		TC_PRM, NULL, NULL,		O(parms.csm),	S_CSM	 },
	{ "map-color-str-tc",	TC_PRM, NULL, NULL,		O(parms.csm_tc),S_CSM_TC },
	{ "map-color-str-ti",	TC_PRM, NULL, NULL,		O(parms.csm_ti),S_CSM_TI },

	{ "set-fg",				TC_PRM,	NULL, NULL,		O(parms.fgr),	S_FGR	 },
	{ "set-fg-tc",			TC_PRM,	"Sf", NULL,		O(parms.fgr_tc),S_FGR_TC },
	{ "set-fg-ti",			TC_PRM,	NULL, "setf",	O(parms.fgr_ti),S_FGR_TI },

	{ "map-fg",				TC_PRM,	NULL, NULL,		O(parms.fgm),	S_FGM	 },
	{ "map-fg-tc",			TC_PRM,	"AF", NULL,		O(parms.fgm_tc),S_FGM_TC },
	{ "map-fg-ti",			TC_PRM,	NULL, "setaf",	O(parms.fgm_ti),S_FGM_TI },

	{ "set-bg",				TC_PRM,	NULL, NULL,		O(parms.bgr),	S_BGR	 },
	{ "set-bg-tc",			TC_PRM,	"Sb", NULL,		O(parms.bgr_tc),S_BGR_TC },
	{ "set-bg-ti",			TC_PRM,	NULL, "setb",	O(parms.bgr_ti),S_BGR_TI },

	{ "map-bg",				TC_PRM,	NULL, NULL,		O(parms.bgm),	S_BGM	 },
	{ "map-bg-tc",			TC_PRM,	"AB", NULL,		O(parms.bgm_tc),S_BGM_TC },
	{ "map-bg-ti",			TC_PRM,	NULL, "setab",	O(parms.bgm_ti),S_BGM_TI },

	/*--------------------------------------------------------------------
	 * string entries
	 */
	{ "add-line",			TC_STR, "al", "il1",	O(strs.al),		S_AL	},
	{ "delete-line",		TC_STR, "dl", "dl1",	O(strs.dl),		S_DL	},

	{ "insert-char",		TC_STR, "ic", "ich1",	O(strs.ic),		S_IC	},
	{ "delete-char",		TC_STR, "dc", "dch1",	O(strs.dc),		S_DC	},

	{ "bell",				TC_STR, "bl", "bel",	O(strs.bl),		S_BL	},
	{ "visible-bell",		TC_STR, "vb", "flash",	O(strs.vb),		S_VB	},

	{ "clear-screen",		TC_STR, "cl", "clear",	O(strs.cl),		S_CL	},
	{ "clear-to-eol",		TC_STR, "ce", "el",		O(strs.ce),		S_CE	},
	{ "clear-to-eos",		TC_STR, "cd", "ed",		O(strs.cd),		S_CD	},

	{ "cursor-on",			TC_STR, "ve", "cnorm",	O(strs.ve),		S_VE	},
	{ "cursor-off",			TC_STR, "vi", "civis",	O(strs.vi),		S_VI	},
	{ "cursor-visible",		TC_STR,	"vs", "cvvis",	O(strs.vs),		S_VS	},

	{ "session-beg",		TC_STR, "ti", "is1",	O(strs.ti),		S_TI	},
	{ "session-end",		TC_STR, "te", "rs1",	O(strs.te),		S_TE	},

	{ "title-beg",			TC_STR, "ts", "tsl",	O(strs.tts),	S_TTS	},
	{ "title-end",			TC_STR, "fs", "fsl",	O(strs.tte),	S_TTE	},

	{ "icon-close",			TC_STR, NULL, NULL,		O(strs.icc),	S_ICC	},
	{ "icon-open",			TC_STR, NULL, NULL,		O(strs.ico),	S_ICO	},

	{ "icon-bits-beg",		TC_STR, NULL, NULL,		O(strs.icbs),	S_ICBS	},
	{ "icon-bits-end",		TC_STR, NULL, NULL,		O(strs.icbe),	S_ICBE	},

	{ "alt-chars-ena",		TC_STR, "eA", "enacs",	O(strs.ace),	S_ACE	},
	{ "alt-chars-dis",		TC_STR, "ea", "rmacs",	O(strs.acd),	S_ACD	},

	{ "keymap-on",			TC_STR, NULL, NULL,		O(strs.kbo),	S_KBO	},
	{ "keymap-off",			TC_STR, NULL, NULL,		O(strs.kbf),	S_KBF	},

	{ "printer-on",			TC_STR, "po", "mc5",	O(strs.pro),	S_PRO	},
	{ "printer-off",		TC_STR, "pf", "mc4",	O(strs.prf),	S_PRF	},

	{ "reverse-on",			TC_STR, "so", "smso",	O(strs.sts),	S_STS	},
	{ "reverse-off",		TC_STR, "se", "rmso",	O(strs.ste),	S_STE	},
	{ "underline-on",		TC_STR, "us", "smul",	O(strs.uls),	S_ULS	},
	{ "underline-off",		TC_STR, "ue", "rmul",	O(strs.ule),	S_ULE	},
	{ "bold-on",			TC_STR, "md", "bold",	O(strs.bds),	S_BDS	},
	{ "bold-off",			TC_STR, "me", "sgr0",	O(strs.bde),	S_BDE	},
	{ "blink-on",			TC_STR, "mb", "blink",	O(strs.bls),	S_BLS	},
	{ "blink-off",			TC_STR, "me", "sgr0",	O(strs.ble),	S_BLE	},
	{ "dim-on",				TC_STR, "mh", "dim",	O(strs.dms),	S_DMS	},
	{ "dim-off",			TC_STR, "me", "sgr0",	O(strs.dme),	S_DME	},
	{ "invisible-on",		TC_STR, "mk", "invis",	O(strs.ivs),	S_IVS	},
	{ "invisible-off",		TC_STR, "me", "sgr0",	O(strs.ive),	S_IVE	},
	{ "protected-on",		TC_STR, "mp", "prot",	O(strs.prs),	S_PRS	},
	{ "protected-off",		TC_STR, "me", "sgr0",	O(strs.pre),	S_PRE	},

	{ "reg-char-1-on",		TC_STR, NULL, NULL,		O(strs.rns[0]),	S_RS1	},
	{ "reg-char-2-on",		TC_STR, NULL, NULL,		O(strs.rns[1]),	S_RS2	},
	{ "reg-char-3-on",		TC_STR, NULL, NULL,		O(strs.rns[2]),	S_RS2	},
	{ "reg-char-4-on",		TC_STR, NULL, NULL,		O(strs.rns[3]),	S_RS4	},

	{ "reg-char-1-off",		TC_STR, NULL, NULL,		O(strs.rne[0]),	S_RE1	},
	{ "reg-char-2-off",		TC_STR, NULL, NULL,		O(strs.rne[1]),	S_RE2	},
	{ "reg-char-3-off",		TC_STR, NULL, NULL,		O(strs.rne[2]),	S_RE3	},
	{ "reg-char-4-off",		TC_STR, NULL, NULL,		O(strs.rne[3]),	S_RE4	},

	{ "alt-char-1-on",		TC_STR, "as", "smacs",	O(strs.ans[0]),	S_AS1	},
	{ "alt-char-2-on",		TC_STR, NULL, NULL,		O(strs.ans[1]),	S_AS2	},
	{ "alt-char-3-on",		TC_STR, NULL, NULL,		O(strs.ans[2]),	S_AS3	},
	{ "alt-char-4-on",		TC_STR, NULL, NULL,		O(strs.ans[3]),	S_AS4	},

	{ "alt-char-1-off",		TC_STR, "ae", "rmacs",	O(strs.ane[0]),	S_AE1	},
	{ "alt-char-2-off",		TC_STR, NULL, NULL,		O(strs.ane[1]),	S_AE2	},
	{ "alt-char-3-off",		TC_STR, NULL, NULL,		O(strs.ane[2]),	S_AE3	},
	{ "alt-char-4-off",		TC_STR, NULL, NULL,		O(strs.ane[3]),	S_AE4	},

	{ "mouse-on",			TC_STR,	NULL, NULL,		O(strs.mbeg),	S_MBEG	},
	{ "mouse-off",			TC_STR,	NULL, NULL,		O(strs.mend),	S_MEND	},
	{ "mouse-input",		TC_STR,	NULL, NULL,		O(strs.mstr),	S_MSTR	},

	{ "screen-snap",		TC_STR,	NULL, NULL,		O(strs.snap),	S_SNAP	},
	{ "description",		TC_STR,	"DS", "desc",	O(strs.desc),	S_DESC	},
	{ "vt100-char-set",		TC_STR,	"ac", "acsc",	O(strs.acs),	S_ACS	},

	{ 0, 0, 0, 0, 0, S_ZERO }
};

#undef O

/*------------------------------------------------------------------------
 *	This table maps whatever VT100 chars possible
 *	to the PC character set.
 *
 *	Note we only do those chars which correspond to to the IBM PC charset.
 */
const ACS_CHRS tcap_acs_chars[] =
{
	{ VT100_ULCORNER,	0xda },					/* SVSH upper left corner	*/
	{ VT100_LLCORNER,	0xc0 },					/* SVSH lower left corner	*/
	{ VT100_URCORNER,	0xbf },					/* SVSH upper right corner	*/
	{ VT100_LRCORNER,	0xd9 },					/* SVSH lower right corner	*/
	{ VT100_RTEE,		0xb4 },					/* SVSH right tee			*/
	{ VT100_LTEE,		0xc3 },					/* SVSH left tee			*/
	{ VT100_BTEE,		0xc1 },					/* SVSH bottom tee			*/
	{ VT100_TTEE,		0xc2 },					/* SVSH top tee				*/
	{ VT100_PLUS,		0xc5 },					/* SVSH plus - center		*/

	{ VT100_ULCORNER,	0xc9 },					/* DVDH upper left corner	*/
	{ VT100_LLCORNER,	0xc8 },					/* DVDH lower left corner	*/
	{ VT100_URCORNER,	0xbb },					/* DVDH upper right corner	*/
	{ VT100_LRCORNER,	0xbc },					/* DVDH lower right corner	*/
	{ VT100_RTEE,		0xb9 },					/* DVDH right tee			*/
	{ VT100_LTEE,		0xcc },					/* DVDH left tee			*/
	{ VT100_BTEE,		0xca },					/* DVDH bottom tee			*/
	{ VT100_TTEE,		0xcb },					/* DVDH top tee				*/
	{ VT100_PLUS,		0xce },					/* DVDH plus - center		*/

	{ VT100_ULCORNER,	0xd5 },					/* SVDH upper left corner	*/
	{ VT100_LLCORNER,	0xd4 },					/* SVDH lower left corner	*/
	{ VT100_URCORNER,	0xb8 },					/* SVDH upper right corner	*/
	{ VT100_LRCORNER,	0xbe },					/* SVDH lower right corner	*/
	{ VT100_RTEE,		0xb5 },					/* SVDH right tee			*/
	{ VT100_LTEE,		0xc6 },					/* SVDH left tee			*/
	{ VT100_BTEE,		0xcf },					/* SVDH bottom tee			*/
	{ VT100_TTEE,		0xd1 },					/* SVDH top tee				*/
	{ VT100_PLUS,		0xd8 },					/* SVDH plus - center		*/

	{ VT100_ULCORNER,	0xd6 },					/* DVSH upper left corner	*/
	{ VT100_LLCORNER,	0xd3 },					/* DVSH lower left corner	*/
	{ VT100_URCORNER,	0xb7 },					/* DVSH upper right corner	*/
	{ VT100_LRCORNER,	0xbd },					/* DVSH lower right corner	*/
	{ VT100_RTEE,		0xb6 },					/* DVSH right tee			*/
	{ VT100_LTEE,		0xc7 },					/* DVSH left tee			*/
	{ VT100_BTEE,		0xd0 },					/* DVSH bottom tee			*/
	{ VT100_TTEE,		0xd2 },					/* DVSH top tee				*/
	{ VT100_PLUS,		0xd7 },					/* DVSH plus - center		*/

	{ VT100_HLINE,		0xc4 },					/* SH horizontal line		*/
	{ VT100_VLINE,		0xb3 },					/* SV vertical line			*/

	{ VT100_HLINE,		0xcd },					/* DH horizontal line		*/
	{ VT100_VLINE,		0xba },					/* DV vertical line			*/

	{ VT100_DIAMOND,	0xb2 },					/* solid square block		*/
	{ VT100_CKBOARD,	0xb0 },					/* checker board			*/
	{ VT100_BLOCK,		0xdb },					/* solid square block		*/
	{ VT100_PI,			0xe3 },					/* PI						*/
	{ VT100_PLMINUS,	0xf1 },					/* plus/minus				*/
	{ VT100_GE,			0xf2 },					/* greater than or equal	*/
	{ VT100_LE,			0xf3 },					/* less than or equal		*/
	{ VT100_DEGREE,		0xf8 },					/* degree symbol			*/
	{ VT100_BULLET,		0xf9 },					/* bullet					*/

	{ 0, 0 }
};

/*------------------------------------------------------------------------
 * translate table list
 */
const XLATE_LIST tcap_xlate_list[] =
{
	{ "input-char-set",	FONT_TBL_INP,	0 },

	{ "reg-char-0-set",	FONT_TBL_REG,	0 },
	{ "reg-char-1-set",	FONT_TBL_REG,	1 },
	{ "reg-char-2-set",	FONT_TBL_REG,	2 },
	{ "reg-char-3-set",	FONT_TBL_REG,	3 },
	{ "reg-char-4-set",	FONT_TBL_REG,	4 },

	{ "alt-char-0-set",	FONT_TBL_ALT,	0 },
	{ "alt-char-1-set",	FONT_TBL_ALT,	1 },
	{ "alt-char-2-set",	FONT_TBL_ALT,	2 },
	{ "alt-char-3-set",	FONT_TBL_ALT,	3 },
	{ "alt-char-4-set",	FONT_TBL_ALT,	4 },

	{ 0, 0, 0 }
};

/*------------------------------------------------------------------------
 * font screen cmd tables
 */
const FONT_CMD tcap_reg_font_cmds[] =
{
	{ S_ZERO,	S_ZERO	},
	{ S_RS1,	S_RE1	},
	{ S_RS2,	S_RE2	},
	{ S_RS3,	S_RE3	},
	{ S_RS4,	S_RE4	}
};

const FONT_CMD tcap_alt_font_cmds[] =
{
	{ S_ZERO,	S_ZERO	},
	{ S_AS1,	S_AE1	},
	{ S_AS2,	S_AE2	},
	{ S_AS3,	S_AE3	},
	{ S_AS4,	S_AE4	}
};

/*------------------------------------------------------------------------
 * param info
 */
#define O(field)		X_OFFSET_OF(PARM_DATA,field)

const PARAM_INFO tcap_param_info[] =
{
	/*--------------------------------------------------------------------
	 * move cursor
	 */
	{ S_CM,		S_CM,	0,		TRUE,	O(cm),	0					},
	{ S_CM_TC,	S_CM,	0,		FALSE,	O(cm),	TCAP_DB_TERMCAP		},
	{ S_CM_TI,	S_CM,	0,		FALSE,	O(cm),	TCAP_DB_TERMINFO	},

	/*--------------------------------------------------------------------
	 * color string
	 */
	{ S_CSR,	S_CS,	FALSE,	TRUE,	O(cs),	0					},
	{ S_CSR_TC,	S_CS,	FALSE,	FALSE,	O(cs),	TCAP_DB_TERMCAP		},
	{ S_CSR_TI,	S_CS,	FALSE,	FALSE,	O(cs),	TCAP_DB_TERMINFO	},

	{ S_CSM,	S_CS,	TRUE,	TRUE,	O(cs),	0					},
	{ S_CSM_TC,	S_CS,	TRUE,	FALSE,	O(cs),	TCAP_DB_TERMCAP		},
	{ S_CSM_TI,	S_CS,	TRUE,	FALSE,	O(cs),	TCAP_DB_TERMINFO	},

	/*--------------------------------------------------------------------
	 * foreground color
	 */
	{ S_FGR,	S_FG,	FALSE,	TRUE,	O(fg),	0					},
	{ S_FGR_TC,	S_FG,	FALSE,	FALSE,	O(fg),	TCAP_DB_TERMCAP		},
	{ S_FGR_TI,	S_FG,	FALSE,	FALSE,	O(fg),	TCAP_DB_TERMINFO	},

	{ S_FGM,	S_FG,	TRUE,	TRUE,	O(fg),	0					},
	{ S_FGM_TC,	S_FG,	TRUE,	FALSE,	O(fg),	TCAP_DB_TERMCAP		},
	{ S_FGM_TI,	S_FG,	TRUE,	FALSE,	O(fg),	TCAP_DB_TERMINFO	},

	/*--------------------------------------------------------------------
	 * background color
	 */
	{ S_BGR,	S_BG,	FALSE,	TRUE,	O(bg),	0					},
	{ S_BGR_TC,	S_BG,	FALSE,	FALSE,	O(bg),	TCAP_DB_TERMCAP		},
	{ S_BGR_TI,	S_BG,	FALSE,	FALSE,	O(bg),	TCAP_DB_TERMINFO	},

	{ S_BGM,	S_BG,	TRUE,	TRUE,	O(bg),	0					},
	{ S_BGM_TC,	S_BG,	TRUE,	FALSE,	O(bg),	TCAP_DB_TERMCAP		},
	{ S_BGM_TI,	S_BG,	TRUE,	FALSE,	O(bg),	TCAP_DB_TERMINFO	},

	{ S_ZERO, S_ZERO, 0, 0, 0 }
};

#undef O
