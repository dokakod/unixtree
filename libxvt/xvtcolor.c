/*------------------------------------------------------------------------
 * color processing
 */
#include "xvtcommon.h"

/*------------------------------------------------------------------------
 * color table
 *
 *	This table uses the "standard" mapping of colors to our color table.
 */
const XVT_COLOR	xvt_color_tbl[] =
{
	{ XVT_CLR_BLACK,		"black"			},
	{ XVT_CLR_RED,			"red"			},
	{ XVT_CLR_GREEN,		"green"			},
	{ XVT_CLR_YELLOW,		"yellow"		},
	{ XVT_CLR_BLUE,			"blue"			},
	{ XVT_CLR_MAGENTA,		"magenta"		},
	{ XVT_CLR_CYAN,			"cyan"			},
	{ XVT_CLR_WHITE,		"white"			},

	{ XVT_CLR_GREY,			"grey"			},
	{ XVT_CLR_LTRED,		"lt-red"		},
	{ XVT_CLR_LTGREEN,		"lt-green"		},
	{ XVT_CLR_LTYELLOW,		"lt-yellow"		},
	{ XVT_CLR_LTBLUE,		"lt-blue"		},
	{ XVT_CLR_LTMAGENTA,	"lt-magenta"	},
	{ XVT_CLR_LTCYAN,		"lt-cyan"		},
	{ XVT_CLR_LTWHITE,		"lt-white"		},

	{ XVT_CLR_FG,			"foreground"	},
	{ XVT_CLR_BG,			"background"	}
};

/*------------------------------------------------------------------------
 * get color value for a name
 */
int xvt_color_value (const char *name)
{
	int	i;

	if (name == 0 || *name == 0)
		return (-1);

	for (i=0; i<XVT_CLR_NUM; i++)
	{
		if (xvt_strccmp(name, xvt_color_tbl[i].name) == 0)
			return (xvt_color_tbl[i].code);
	}

	return (-1);
}
