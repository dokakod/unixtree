/*------------------------------------------------------------------------
 * internal data
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * monochrome attribute table
 */
const ATTR_TBL tcap_mono_attrs[] =
{
	{ "normal",				A_NORMAL		},
	{ "reverse",			A_REVERSE		},
	{ "underline",			A_UNDERLINE		},
	{ "bold",				A_BOLD			},
	{ "blink",				A_BLINK			},
	{ "dim",				A_DIM			},
	{ "invisible",			A_INVIS			},
	{ "protected",			A_PROTECT		}
};

/*------------------------------------------------------------------------
 * common attribute table
 */
const ATTR_TBL tcap_common_attrs[] =
{
	{ "alt-char",			A_ALTCHARSET	}
};

/*------------------------------------------------------------------------
 * foreground color table
 */
const ATTR_TBL tcap_foregrnd_attrs[] =
{
	{ "black",				COLOR_BLACK		},
	{ "red",				COLOR_RED		},
	{ "green",				COLOR_GREEN		},
	{ "yellow",				COLOR_YELLOW	},
	{ "blue",				COLOR_BLUE		},
	{ "magenta",			COLOR_MAGENTA	},
	{ "cyan",				COLOR_CYAN		},
	{ "white",				COLOR_WHITE		},

	{ "grey",				COLOR_GREY		},
	{ "lt-red",				COLOR_LTRED		},
	{ "lt-green",			COLOR_LTGREEN	},
	{ "lt-yellow",			COLOR_LTYELLOW	},
	{ "lt-blue",			COLOR_LTBLUE	},
	{ "lt-magenta",			COLOR_LTMAGENTA	},
	{ "lt-cyan",			COLOR_LTCYAN	},
	{ "lt-white",			COLOR_LTWHITE	}
};

/*------------------------------------------------------------------------
 * background color table
 */
const ATTR_TBL tcap_backgrnd_attrs[] =
{
	{ "black-bgnd",		 	COLOR_BLACK		},
	{ "red-bgnd",			COLOR_RED		},
	{ "green-bgnd",			COLOR_GREEN		},
	{ "yellow-bgnd",		COLOR_YELLOW	},
	{ "blue-bgnd",			COLOR_BLUE		},
	{ "magenta-bgnd",		COLOR_MAGENTA	},
	{ "cyan-bgnd",			COLOR_CYAN		},
	{ "white-bgnd",			COLOR_WHITE		},

	{ "grey-bgnd",			COLOR_GREY		},
	{ "lt-red-bgnd",		COLOR_LTRED		},
	{ "lt-green-bgnd",		COLOR_LTGREEN	},
	{ "lt-yellow-bgnd",		COLOR_LTYELLOW	},
	{ "lt-blue-bgnd",		COLOR_LTBLUE	},
	{ "lt-magenta-bgnd",	COLOR_LTMAGENTA	},
	{ "lt-cyan-bgnd",		COLOR_LTCYAN	},
	{ "lt-white-bgnd",		COLOR_LTWHITE	}
};
