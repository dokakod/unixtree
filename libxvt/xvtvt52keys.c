/*------------------------------------------------------------------------
 * vt52 key tables
 */
#include "xvtcommon.h"
#include "xvtvt52.h"

/*------------------------------------------------------------------------
 * prefix strings
 */
#define CSI				"\033["
#define SS3				"\033O"
#define APL				"\033?"

/*------------------------------------------------------------------------
 * strings to send for function keys
 */
const KEY_DATA xvt_vt52_keys_fn_reg[] =
{
	{ SS3 "P",			"f1"	},		/* old xterms used CSI "11~"	*/
	{ SS3 "Q",			"f2"	},		/* old xterms used CSI "12~"	*/
	{ SS3 "R",			"f3"	},		/* old xterms used CSI "13~"	*/
	{ SS3 "S",			"f4"	},		/* old xterms used CSI "14~"	*/
	{ CSI "15~",		"f5"	},
	{ CSI "17~",		"f6"	},
	{ CSI "18~",		"f7"	},
	{ CSI "19~",		"f8"	},
	{ CSI "20~",		"f9"	},
	{ CSI "21~",		"f10"	},
	{ CSI "23~",		"f11"	},
	{ CSI "24~",		"f12"	},
	{ CSI "25~",		"f13"	},
	{ CSI "26~",		"f14"	},
	{ CSI "28~",		"f15"	},
	{ CSI "29~",		"f16"	}
};

/*------------------------------------------------------------------------
 * strings to send for control keys
 */
const KEY_DATA xvt_vt52_keys_ct_reg[] =
{
	{ "\000",			"^@"	},
	{ "\001",			"^A"	},
	{ "\002",			"^B"	},
	{ "\003",			"^C"	},
	{ "\004",			"^D"	},
	{ "\005",			"^E"	},
	{ "\006",			"^F"	},
	{ "\007",			"^G"	},
	{ "\010",			"^H"	},
	{ "\011",			"^I"	},
	{ "\012",			"^J"	},
	{ "\013",			"^K"	},
	{ "\014",			"^L"	},
	{ "\015",			"^M"	},
	{ "\016",			"^N"	},
	{ "\017",			"^O"	},
	{ "\020",			"^P"	},
	{ "\021",			"^Q"	},
	{ "\022",			"^R"	},
	{ "\023",			"^S"	},
	{ "\024",			"^T"	},
	{ "\025",			"^U"	},
	{ "\026",			"^V"	},
	{ "\027",			"^W"	},
	{ "\030",			"^X"	},
	{ "\031",			"^Y"	},
	{ "\032",			"^Z"	},
	{ "\033",			"^["	},
	{ "\034",			"^\\"	},
	{ "\035",			"^]"	},
	{ "\036",			"^^"	},
	{ "\037",			"^_"	}
};

const KEY_DATA xvt_vt52_keys_ct_map[] =
{
	{ "\000",			"^@"	},
	{ "\001",			"^A"	},
	{ "\002",			"^B"	},
	{ "\003",			"^C"	},
	{ "\004",			"^D"	},
	{ "\005",			"^E"	},
	{ "\006",			"^F"	},
	{ "\007",			"^G"	},
	{ "\010",			"^H"	},
	{ "\011",			"^I"	},
	{ "\012",			"^J"	},
	{ "\013",			"^K"	},
	{ "\014",			"^L"	},
	{ "\015",			"^M"	},
	{ "\016",			"^N"	},
	{ "\017",			"^O"	},
	{ "\020",			"^P"	},
	{ "\021",			"^Q"	},
	{ "\022",			"^R"	},
	{ "\023",			"^S"	},
	{ "\024",			"^T"	},
	{ "\025",			"^U"	},
	{ "\026",			"^V"	},
	{ "\027",			"^W"	},
	{ "\030",			"^X"	},
	{ "\031",			"^Y"	},
	{ "\032",			"^Z"	},
	{ "\033\033",		"^["	},
	{ "\034",			"^\\"	},
	{ "\035",			"^]"	},
	{ "\036",			"^^"	},
	{ "\037",			"^_"	}
};

/*------------------------------------------------------------------------
 * strings to send for misc keys
 */
const KEY_DATA xvt_vt52_keys_ms_reg[] =
{
    { "",				"Pause"		},
    { "",				"PrSc"		},
    { "",				"ScrLck"	},
	{ "",				"SysReq"	},
	{ "",				"Break"		},
	{ "",				"Resize"	}
};

const KEY_DATA xvt_vt52_keys_ms_map[] =
{
    { "",				"Pause"		},
    { "",				"PrSc"		},
    { "",				"ScrLck"	},
	{ "",				"SysReq"	},
	{ "",				"Break"		},
	{ "\035~",			"Resize"	}
};

/*------------------------------------------------------------------------
 * strings to send for keypad keys
 */
const KEY_DATA xvt_vt52_keys_kp_reg[] =
{
	{ CSI "2~",			"Insert"	},
	{ CSI "F",			"End"		},
	{ CSI "B",			"Down"		},
	{ CSI "6~",			"Pgdn"		},
	{ CSI "D",			"Left"		},
	{ CSI "E",			"Middle"	},
	{ CSI "C",			"Right"		},
	{ CSI "H",			"Home"		},
	{ CSI "A",			"Up"		},
	{ CSI "5~",			"PgUp"		},
	{ CSI "3~",			"Delete"	},
	{ "-",				"KP -"		},
	{ "+",				"KP +"		},
	{ "/",				"KP /"		},
	{ "*",				"KP *"		},
	{ "\r",				"KP enter"	}
};

const KEY_DATA xvt_vt52_keys_kp_cur[] =
{
	{ CSI "2~",			"Insert"	},
	{ CSI "F",			"End"		},
	{ SS3 "B",			"Down"		},
	{ CSI "6~",			"Pgdn"		},
	{ SS3 "D",			"Left"		},
	{ CSI "E",			"Middle"	},
	{ SS3 "C",			"Right"		},
	{ CSI "H",			"Home"		},
	{ SS3 "A",			"Up"		},
	{ CSI "5~",			"PgUp"		},
	{ CSI "3~",			"Delete"	},
	{ "-",				"KP -"		},
	{ "+",				"KP +"		},
	{ "/",				"KP /"		},
	{ "*",				"KP *"		},
	{ "\r",				"KP enter"	}
};

const KEY_DATA xvt_vt52_keys_kp_apl[] =
{
	{ APL "p",			"APL KP 0"	},
	{ APL "q",			"APL KP 1"	},
	{ APL "r",			"APL KP 2"	},
	{ APL "s",			"APL KP 3"	},
	{ APL "t",			"APL KP 4"	},
	{ APL "u",			"APL KP 5"	},
	{ APL "v",			"APL KP 6"	},
	{ APL "w",			"APL KP 7"	},
	{ APL "x",			"APL KP 8"	},
	{ APL "y",			"APL KP 9"	},
	{ CSI "3~",			"APL KP Delete"	},
	{ APL "m",			"APL KP -"	},
	{ APL "k",			"APL KP +"	},
	{ APL "o",			"APL KP /"	},
	{ APL "j",			"APL KP *"	},
	{ APL "M",			"APL KP enter"	}
};

const KEY_DATA xvt_vt52_keys_kp_map[] =
{
	{ CSI "2~",			"Insert"	},
	{ CSI "F",			"End"		},
	{ CSI "B",			"Down"		},
	{ CSI "6~",			"Pgdn"		},
	{ CSI "D",			"Left"		},
	{ CSI "E",			"Middle"	},
	{ CSI "C",			"Right"		},
	{ CSI "H",			"Home"		},
	{ CSI "A",			"Up"		},
	{ CSI "5~",			"PgUp"		},
	{ CSI "3~",			"Delete"	},
	{ "-",				"KP -"		},
	{ "+",				"KP +"		},
	{ "/",				"KP /"		},
	{ "*",				"KP *"		},
	{ "\r",				"KP enter"	}
};

/*------------------------------------------------------------------------
 * prefix keys
 */
const KEY_DATA xvt_vt52_keys_pf_reg[] =
{
	{ "",				"shft-" },
	{ "",				"ctrl-" },
	{ "",				"meta-" }
};

const KEY_DATA xvt_vt52_keys_pf_map[] =
{
	{ "\035s",			"shft-" },
	{ "\035c",			"ctrl-" },
	{ "\035a",			"meta-" }
};

/*------------------------------------------------------------------------
 * state keys (shift, control, alt)
 */
const KEY_DATA	xvt_vt52_keys_st_up[] =
{
	{ "\033[sr!",		"shft"		},
	{ "\033[cr!",		"ctrl"		},
	{ "\033[ar!",		"meta"		},
	{ "",				"num-lock"	}
};

const KEY_DATA	xvt_vt52_keys_st_dn[] =
{
	{ "\033[sp!",		"shft"		},
	{ "\033[cp!",		"ctrl"		},
	{ "\033[ap!",		"meta"		},
	{ "",				"num-lock"	}
};
