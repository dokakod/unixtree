/*------------------------------------------------------------------------
 * vt100 command table
 */
#include "xvtcommon.h"
#include "xvtvt100.h"

/*------------------------------------------------------------------------
 * TOP esc-seq cmds
 */
static const ESC_DATA	xvt_vt100_esctbl_top[] =
{
/*	  code	mode		type		rtn							desc		*/

	{ ' ',	VT100_CTL,	0,			0,							0			},
	{ '%',	VT100_CHS,	0,			0,							0			},

	{ '(',	VT100_GCS,	0,			0,							0			},
	{ ')',	VT100_GCS,	1,			0,							0			},
	{ '*',	VT100_GCS,	2,			0,							0			},
	{ '+',	VT100_GCS,	3,			0,							0			},

	{ '[',	VT100_CSI,	-1,			0,							0			},
	{ ']',	VT100_OSC,	-1,			0,							0			},
	{ '#',	VT100_DEC,	-1,			0,							0			},

	{ 'P',	VT100_DEV,	DEV_DCS,	0,							0			},
	{ '^',	VT100_DEV,	DEV_PM,		0,							0			},
	{ '_',	VT100_DEV,	DEV_APC,	0,							0			},
	{ 'X',	VT100_DEV,	DEV_SOS,	0,							0			},

	{ '>',	-1,			-1,			xvt_vt100_TOP_DECKPAM,		0			},
	{ '=',	-1,			-1,			xvt_vt100_TOP_DECKPNM,		0			},
	{ '6',	-1,			-1,			xvt_vt100_TOP_DECBI,		0			},
	{ '7',	-1,			-1,			xvt_vt100_TOP_DECSC,		0			},
	{ '8',	-1,			-1,			xvt_vt100_TOP_DECRC,		0			},
	{ '9',	-1,			-1,			xvt_vt100_TOP_DECFI,		0			},
	{ 'D',	-1,			-1,			xvt_vt100_TOP_IND,			0			},
	{ 'E',	-1,			-1,			xvt_vt100_TOP_NEL,			0			},
	{ 'F',	-1,			-1,			xvt_vt100_TOP_CLL,			0			},
	{ 'H',	-1,			-1,			xvt_vt100_TOP_HTS,			0			},
	{ 'M',	-1,			-1,			xvt_vt100_TOP_RI,			0			},
	{ 'N',	-1,			-1,			xvt_vt100_TOP_SS2,			0			},
	{ 'O',	-1,			-1,			xvt_vt100_TOP_SS3,			0			},
	{ 'V',	-1,			-1,			xvt_vt100_TOP_SPA,			0			},
	{ 'W',	-1,			-1,			xvt_vt100_TOP_EPA,			0			},
	{ 'Z',	-1,			-1,			xvt_vt100_TOP_DA,			0			},
	{ '\\',	-1,			-1,			xvt_vt100_TOP_DEVEND,		0			},
	{ 'c',	-1,			-1,			xvt_vt100_TOP_RIS,			0			},
	{ 'l',	-1,			-1,			xvt_vt100_TOP_MEML,			0			},
	{ 'm',	-1,			-1,			xvt_vt100_TOP_MEMU,			0			},
	{ 'n',	-1,			-1,			xvt_vt100_TOP_LS2,			0			},
	{ 'o',	-1,			-1,			xvt_vt100_TOP_LS3,			0			},
	{ '|',	-1,			-1,			xvt_vt100_TOP_LS3R,			0			},
	{ '}',	-1,			-1,			xvt_vt100_TOP_LS2R,			0			},
	{ '~',	-1,			-1,			xvt_vt100_TOP_LS1R,			0			},

	{ -1, 0, 0, 0, 0 }
};

/*------------------------------------------------------------------------
 * CTL esc-seq cmds
 */
static const ESC_DATA	xvt_vt100_esctbl_ctl[] =
{
/*	  code	mode		type		rtn						desc			*/

	{ 'F',	-1,			-1,			0,		"set 7-bit controls"			},
	{ 'G',	-1,			-1,			0,		"set 8-bit controls"			},
	{ 'L',	-1,			-1,			0,		"set ANSI conformance level 1"	},
	{ 'M',	-1,			-1,			0,		"set ANSI conformance level 2"	},
	{ 'N',	-1,			-1,			0,		"set ANSI conformance level 3"	},

	{ -1, 0, 0, 0, 0 }
};

/*------------------------------------------------------------------------
 * CHS esc-seq cmds
 */
static const ESC_DATA	xvt_vt100_esctbl_chs[] =
{
/*	  code	mode		type		rtn						desc			*/

	{ '@',	-1,			-1,			0,		"select default char set"		},
	{ 'G',	-1,			-1,			0,		"select UTF-8   char set"		},

	{ -1, 0, 0, 0, 0 }
};

/*------------------------------------------------------------------------
 * DEC esc-seq cmds
 */
static const ESC_DATA	xvt_vt100_esctbl_dec[] =
{
/*	  code	mode		type		rtn						desc			*/

	{ '8',	-1,			-1,			xvt_vt100_DEC_DECALN,	0				},

	{ '3',	-1,			-1,			0,					"dbl-height, top"	},
	{ '4',	-1,			-1,			0,					"dbl-height, bot"	},
	{ '5',	-1,			-1,			0,					"sng-width"			},
	{ '6',	-1,			-1,			0,					"dbl-width"			},

	{ -1, 0, 0, 0, 0 }
};

/*------------------------------------------------------------------------
 * GCS esc-seq cmds
 */
static const ESC_DATA	xvt_vt100_esctbl_gcs[] =
{
/*	  code	mode		type		rtn						desc			*/

	{ 0,	-1,			-1,			xvt_vt100_GCS_CODE,		0				},

	{ -1, 0, 0, 0, 0 }
};

/*------------------------------------------------------------------------
 * CSI esc-seq cmds
 */
static const ESC_DATA	xvt_vt100_esctbl_csi[] =
{
/*	  code	mode		type		rtn						desc			*/

	{ '?',	VT100_PRI,	-1,			0,						0				},

	{ '@',	-1,			-1,			xvt_vt100_CSI_ICH,		0				},
	{ 'A',	-1,			-1,			xvt_vt100_CSI_CUU,		0				},
	{ 'B',	-1,			-1,			xvt_vt100_CSI_CUD,		0				},
	{ 'C',	-1,			-1,			xvt_vt100_CSI_CUF,		0				},
	{ 'D',	-1,			-1,			xvt_vt100_CSI_CUB,		0				},
	{ 'E',	-1,			-1,			xvt_vt100_CSI_CNL,		0				},
	{ 'F',	-1,			-1,			xvt_vt100_CSI_CPL,		0				},
	{ 'G',	-1,			-1,			xvt_vt100_CSI_HPA,		0				},
	{ 'H',	-1,			-1,			xvt_vt100_CSI_CUP,		0				},
	{ 'I',	-1,			-1,			xvt_vt100_CSI_CHT,		0				},
	{ 'J',	-1,			-1,			xvt_vt100_CSI_ED,		0				},
	{ 'K',	-1,			-1,			xvt_vt100_CSI_EL,		0				},
	{ 'L',	-1,			-1,			xvt_vt100_CSI_AL,		0				},
	{ 'M',	-1,			-1,			xvt_vt100_CSI_DL,		0				},
	{ 'P',	-1,			-1,			xvt_vt100_CSI_DCH,		0				},
	{ 'S',	-1,			-1,			xvt_vt100_CSI_SD,		0				},
	{ 'T',	-1,			-1,			xvt_vt100_CSI_SU,		0				},
	{ 'W',	-1,			-1,			xvt_vt100_CSI_TBS,		0				},
	{ 'X',	-1,			-1,			xvt_vt100_CSI_ECH,		0				},
	{ 'Z',	-1,			-1,			xvt_vt100_CSI_CBT,		0				},
	{ '^',	-1,			-1,			xvt_vt100_CSI_SU,		0				},
	{ '`',	-1,			-1,			xvt_vt100_CSI_HPA,		0				},
	{ 'a',	-1,			-1,			xvt_vt100_CSI_CUF,		0				},
	{ 'b',	-1,			-1,			xvt_vt100_CSI_REP,		0				},
	{ 'c',	-1,			-1,			xvt_vt100_CSI_DA,		0				},
	{ 'd',	-1,			-1,			xvt_vt100_CSI_VPA,		0				},
	{ 'e',	-1,			-1,			xvt_vt100_CSI_CUU,		0				},
	{ 'f',	-1,			-1,			xvt_vt100_CSI_CUP,		0				},
	{ 'g',	-1,			-1,			xvt_vt100_CSI_TBC,		0				},
	{ 'h',	-1,			-1,			xvt_vt100_CSI_SM,		0				},
	{ 'i',	-1,			-1,			xvt_vt100_CSI_MC,		0				},
	{ 'l',	-1,			-1,			xvt_vt100_CSI_RM,		0				},
	{ 'm',	-1,			-1,			xvt_vt100_CSI_SGR,		0				},
	{ 'n',	-1,			-1,			xvt_vt100_CSI_DSR,		0				},
	{ 'p',	-1,			-1,			xvt_vt100_CSI_DECSCL,	0				},
	{ 'q',	-1,			-1,			xvt_vt100_CSI_DECSCA,	0				},
	{ 'r',	-1,			-1,			xvt_vt100_CSI_DECSTBM,	0				},
	{ 's',	-1,			-1,			xvt_vt100_CSI_DECSLRM,	0				},
	{ 't',	-1,			-1,			xvt_vt100_CSI_TERM,		0				},
	{ 'x',	-1,			-1,			xvt_vt100_CSI_DECREQT,	0				},
	{ 'y',	-1,			-1,			xvt_vt100_CSI_DECTST,	0				},
	{ '~',	-1,			-1,			xvt_vt100_CSI_DECDC,	0				},
	{ '}',	-1,			-1,			xvt_vt100_CSI_DECIC,	0				},

	{ -1, 0, 0, 0, 0 }
};

/*------------------------------------------------------------------------
 * PRI esc-seq cmds
 */
static const ESC_DATA	xvt_vt100_esctbl_pri[] =
{
/*	  code	mode		type		rtn						desc			*/

	{ 'i',	-1,			-1,			xvt_vt100_PRI_DECMC,	0				},
	{ 'J',	-1,			-1,			xvt_vt100_PRI_DECSED,	0				},
	{ 'K',	-1,			-1,			xvt_vt100_PRI_DECSEL,	0				},
	{ 'n',	-1,			-1,			xvt_vt100_PRI_DECDSR,	0				},
	{ 'h',	-1,			-1,			xvt_vt100_PRI_DECSET,	0				},
	{ 'l',	-1,			-1,			xvt_vt100_PRI_DECRST,	0				},
	{ 'r',	-1,			-1,			xvt_vt100_PRI_DECRES,	0				},
	{ 's',	-1,			-1,			xvt_vt100_PRI_DECSAV,	0				},
	{ 't',	-1,			-1,			xvt_vt100_PRI_DECTGL,	0				},

	{ -1, 0, 0, 0, 0 }
};

/*------------------------------------------------------------------------
 * OSC esc-seq cmds
 */
static const ESC_DATA	xvt_vt100_esctbl_osc[] =
{
/*	  code	mode		type		rtn						desc			*/

	{ 0x07,	-1,			-1,			xvt_vt100_OSC_LABEL,	0				},
	{ 0,	-1,			-1,			0,						0				},

	{ -1, 0, 0, 0, 0 }
};

/*------------------------------------------------------------------------
 * DEV esc-seq cmds
 */
static const ESC_DATA	xvt_vt100_esctbl_dev[] =
{
/*	  code	mode		type		rtn						desc			*/

	{ 0,	-1,			-1,			0,						0				},

	{ -1, 0, 0, 0, 0 }
};

/*------------------------------------------------------------------------
 * escape cmd table
 */
static const ESC_CMD	xvt_vt100_esctbl[] =
{
/*	  name		cmd-tbl					codes		data			params	*/

	{ "TOP",	xvt_vt100_esctbl_top,	0,			DATA_NONE,		0		},
	{ "CTL",	xvt_vt100_esctbl_ctl,	0,			DATA_NONE,		0		},
	{ "CHS",	xvt_vt100_esctbl_chs,	0,			DATA_NONE,		0		},
	{ "DEC",	xvt_vt100_esctbl_dec,	0,			DATA_NONE,		0		},
	{ "GCS",	xvt_vt100_esctbl_gcs,	0,			DATA_NONE,		0		},
	{ "CSI",	xvt_vt100_esctbl_csi,	" !\">'=",	DATA_NONE,		6		},
	{ "PRI",	xvt_vt100_esctbl_pri,	0,			DATA_NONE,		6		},
	{ "OSC",	xvt_vt100_esctbl_osc,	0,			DATA_CLEAN,		1		},
	{ "DEV",	xvt_vt100_esctbl_dev,	0,			DATA_ESCAPE,	0		}
};

/*------------------------------------------------------------------------
 * control-char cmd table
 */
static const CTL_CMD	xvt_vt100_ctltbl[] =
{
	{ 0x05,	xvt_vt100_CTL_ENQ	},
	{ 0x07,	xvt_vt100_CTL_BEL	},
	{ 0x08,	xvt_vt100_CTL_BS	},
	{ 0x09,	xvt_vt100_CTL_HT	},
	{ 0x0a,	xvt_vt100_CTL_LF	},
	{ 0x0b,	xvt_vt100_CTL_LF	},
	{ 0x0c,	xvt_vt100_CTL_LF	},
	{ 0x0d,	xvt_vt100_CTL_CR	},
	{ 0x0e,	xvt_vt100_CTL_SO	},
	{ 0x0f,	xvt_vt100_CTL_SI	},

	{ -1, 0 }
};

/*------------------------------------------------------------------------
 * vt100 cmd table
 */
const CMD_TBL	xvt_vt100_cmdtbl =
{
	"vt100",					/* terminal name			*/
	"xterm",					/* default term type		*/
	xvt_vt100_init,				/* initialization routine	*/
	xvt_vt100_char_output,		/* char output routine		*/
	xvt_vt100_char_print,		/* char print routine		*/
	xvt_vt100_ctltbl,			/* control char table		*/
	xvt_vt100_esctbl,			/* escape sequence table	*/
	CONF_VT100					/* ANSI conformance level	*/
};

/*------------------------------------------------------------------------
 * vt200 cmd table
 */
const CMD_TBL	xvt_vt200_cmdtbl =
{
	"vt200",					/* terminal name			*/
	"xterm",					/* default term type		*/
	xvt_vt100_init,				/* initialization routine	*/
	xvt_vt100_char_output,		/* char output routine		*/
	xvt_vt100_char_print,		/* char print routine		*/
	xvt_vt100_ctltbl,			/* control char table		*/
	xvt_vt100_esctbl,			/* escape sequence table	*/
	CONF_VT200					/* ANSI conformance level	*/
};

/*------------------------------------------------------------------------
 * vt300 cmd table
 */
const CMD_TBL	xvt_vt300_cmdtbl =
{
	"vt300",					/* terminal name			*/
	"xterm",					/* default term type		*/
	xvt_vt100_init,				/* initialization routine	*/
	xvt_vt100_char_output,		/* char output routine		*/
	xvt_vt100_char_print,		/* char print routine		*/
	xvt_vt100_ctltbl,			/* control char table		*/
	xvt_vt100_esctbl,			/* escape sequence table	*/
	CONF_VT300					/* ANSI conformance level	*/
};

/*------------------------------------------------------------------------
 * xterm cmd table
 */
const CMD_TBL	xvt_xterm_cmdtbl =
{
	"xterm",					/* terminal name			*/
	"xterm",					/* default term type		*/
	xvt_vt100_init,				/* initialization routine	*/
	xvt_vt100_char_output,		/* char output routine		*/
	xvt_vt100_char_print,		/* char print routine		*/
	xvt_vt100_ctltbl,			/* control char table		*/
	xvt_vt100_esctbl,			/* escape sequence table	*/
	CONF_VT300					/* ANSI conformance level	*/
};
