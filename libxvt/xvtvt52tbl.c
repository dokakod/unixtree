/*------------------------------------------------------------------------
 * vt52 command table
 */
#include "xvtcommon.h"
#include "xvtvt52.h"

/*------------------------------------------------------------------------
 * TOP esc-seq cmds
 */
static const ESC_DATA	xvt_vt52_esctbl_top[] =
{
/*	  code	mode		type		rtn						desc			*/

	{ 'Y',	VT52_PRM,	0,			0,						0				},

	{ 'A',	-1,			-1,			xvt_vt52_TOP_CUU,		0				},
	{ 'B',	-1,			-1,			xvt_vt52_TOP_CUD,		0				},
	{ 'C',	-1,			-1,			xvt_vt52_TOP_CUF,		0				},
	{ 'D',	-1,			-1,			xvt_vt52_TOP_CUB,		0				},
	{ 'F',	-1,			-1,			xvt_vt52_TOP_SGM,		0				},
	{ 'G',	-1,			-1,			xvt_vt52_TOP_EGM,		0				},
	{ 'H',	-1,			-1,			xvt_vt52_TOP_CUH,		0				},
	{ 'I',	-1,			-1,			xvt_vt52_TOP_RI,		0				},
	{ 'J',	-1,			-1,			xvt_vt52_TOP_ED,		0				},
	{ 'K',	-1,			-1,			xvt_vt52_TOP_EL,		0				},
	{ 'V',	-1,			-1,			xvt_vt52_TOP_DECPCL,	0				},
	{ 'W',	-1,			-1,			xvt_vt52_TOP_DECSPC,	0				},
	{ 'X',	-1,			-1,			xvt_vt52_TOP_DECEPC,	0				},
	{ 'Z',	-1,			-1,			xvt_vt52_TOP_DA,		0				},
	{ '=',	-1,			-1,			xvt_vt52_TOP_DECKPAM,	0				},
	{ '>',	-1,			-1,			xvt_vt52_TOP_DECKPNM,	0				},
	{ '<',	-1,			-1,			xvt_vt52_TOP_DECANM,	0				},
	{ '^',	-1,			-1,			xvt_vt52_TOP_DECSAP,	0				},
	{ '_',	-1,			-1,			xvt_vt52_TOP_DECEAP,	0				},
	{ ']',	-1,			-1,			xvt_vt52_TOP_DECPSC,	0				},

	{ -1, 0, 0, 0, 0 }
};

/*------------------------------------------------------------------------
 * PRM esc-seq cmds
 */
static const ESC_DATA	xvt_vt52_esctbl_prm[] =
{
/*	  code	mode		type		rtn						desc			*/

	{ 0,	-1,			-1,			xvt_vt52_PRM_CUP,		0				},

	{ -1, 0, 0, 0, 0 }
};

/*------------------------------------------------------------------------
 * escape cmd table
 */
static const ESC_CMD	xvt_vt52_esctbl[] =
{
/*	  name		cmd-tbl					codes	data			params	*/

	{ "TOP",	xvt_vt52_esctbl_top,	0,		DATA_NONE,		0		},
	{ "PRM",	xvt_vt52_esctbl_prm,	0,		DATA_NONE,		-2		},
};

/*------------------------------------------------------------------------
 * control-char cmd table
 */
static const CTL_CMD	xvt_vt52_ctltbl[] =
{
	{ 0x07,	xvt_vt52_CTL_BEL	},
	{ 0x08,	xvt_vt52_CTL_BS		},
	{ 0x09,	xvt_vt52_CTL_HT		},
	{ 0x0a,	xvt_vt52_CTL_LF		},
	{ 0x0b,	xvt_vt52_CTL_LF		},
	{ 0x0c,	xvt_vt52_CTL_LF		},
	{ 0x0d,	xvt_vt52_CTL_CR		},

	{ -1, 0 }
};

/*------------------------------------------------------------------------
 * vt52 cmd table
 */
const CMD_TBL	xvt_vt52_cmdtbl =
{
	"vt52",						/* terminal name			*/
	"vt52",						/* default term type		*/
	xvt_vt52_init,				/* initialization routine	*/
	xvt_vt52_char_output,		/* char output routine		*/
	xvt_vt52_char_print,		/* char print routine		*/
	xvt_vt52_ctltbl,			/* control char table		*/
	xvt_vt52_esctbl,			/* escape sequence table	*/
	CONF_VT52					/* ANSI conformance level	*/
};
