/*------------------------------------------------------------------------
 * X pointer routines
 */
#include "xvtcommon.h"
#include "xvtx.h"

/*------------------------------------------------------------------------
 * cursor types table
 */
struct pointer_types
{
	int				ptr_code;		/* type code	*/
	const char *	ptr_name;		/* pointer name	*/
};
typedef struct pointer_types POINTER_TYPES;

static const POINTER_TYPES xvt_w_ptr_tbl[] =
{
	{ XC_X_cursor,				"x-cursor"				},
	{ XC_arrow,					"arrow"					},
	{ XC_based_arrow_down,		"based-arrow-down"		},
	{ XC_based_arrow_up,		"based-arrow-up"		},
	{ XC_boat,					"boat"					},
	{ XC_bogosity,				"bogosity"				},
	{ XC_bottom_left_corner,	"bottom-left-corner"	},
	{ XC_bottom_right_corner,	"bottom-right-corner"	},
	{ XC_bottom_side,			"bottom-side"			},
	{ XC_bottom_tee,			"bottom-tee"			},
	{ XC_box_spiral,			"box-spiral"			},
	{ XC_center_ptr,			"center-ptr"			},
	{ XC_circle,				"circle"				},
	{ XC_clock,					"clock"					},
	{ XC_coffee_mug,			"coffee-mug"			},
	{ XC_cross,					"cross"					},
	{ XC_cross_reverse,			"cross-reverse"			},
	{ XC_crosshair,				"crosshair"				},
	{ XC_diamond_cross,			"diamond-cross"			},
	{ XC_dot,					"dot"					},
	{ XC_dotbox,				"dotbox"				},
	{ XC_double_arrow,			"double-arrow"			},
	{ XC_draft_large,			"draft-large"			},
	{ XC_draft_small,			"draft-small"			},
	{ XC_draped_box,			"draped-box"			},
	{ XC_exchange,				"exchange"				},
	{ XC_fleur,					"fleur"					},
	{ XC_gobbler,				"gobbler"				},
	{ XC_gumby,					"gumby"					},
	{ XC_hand1,					"hand1"					},
	{ XC_hand2,					"hand2"					},
	{ XC_heart,					"heart"					},
	{ XC_icon,					"icon"					},
	{ XC_iron_cross,			"iron-cross"			},
	{ XC_left_ptr,				"left-ptr"				},
	{ XC_left_side,				"left-side"				},
	{ XC_left_tee,				"left-tee"				},
	{ XC_leftbutton,			"left-button"			},
	{ XC_ll_angle,				"ll-angle"				},
	{ XC_lr_angle,				"lr-angle"				},
	{ XC_man,					"man"					},
	{ XC_middlebutton,			"middle-button"			},
	{ XC_mouse,					"mouse"					},
	{ XC_pencil,				"pencil"				},
	{ XC_pirate,				"pirate"				},
	{ XC_plus,					"plus"					},
	{ XC_question_arrow,		"question-arrow"		},
	{ XC_right_ptr,				"right-ptr"				},
	{ XC_right_side,			"right-side"			},
	{ XC_right_tee,				"right-tee"				},
	{ XC_rightbutton,			"right-button"			},
	{ XC_rtl_logo,				"rtl-logo"				},
	{ XC_sailboat,				"sailboat"				},
	{ XC_sb_down_arrow,			"sb-down-arrow"			},
	{ XC_sb_h_double_arrow,		"sb-h-double-arrow"		},
	{ XC_sb_left_arrow,			"sb-left-arrow"			},
	{ XC_sb_right_arrow,		"sb-right-arrow"		},
	{ XC_sb_up_arrow,			"sb-up-arrow"			},
	{ XC_sb_v_double_arrow,		"sb-v-double-arrow"		},
	{ XC_shuttle,				"shuttle"				},
	{ XC_sizing,				"sizing"				},
	{ XC_spider,				"spider"				},
	{ XC_spraycan,				"spraycan"				},
	{ XC_star,					"star"					},
	{ XC_target,				"target"				},
	{ XC_tcross,				"tcross"				},
	{ XC_top_left_arrow,		"top-left-arrow"		},
	{ XC_top_left_corner,		"top-left-corner"		},
	{ XC_top_right_corner,		"top-right-corner"		},
	{ XC_top_side,				"top-side"				},
	{ XC_top_tee,				"top-tee"				},
	{ XC_trek,					"trek"					},
	{ XC_ul_angle,				"ul-angle"				},
	{ XC_umbrella,				"umbrella"				},
	{ XC_ur_angle,				"ur-angle"				},
	{ XC_watch,					"watch"					},
	{ XC_xterm,					"xterm"					},

	{ -1, 0 }
};

#define NUM_POINTERS	((sizeof(xvt_w_ptr_tbl) / sizeof(*xvt_w_ptr_tbl)) - 1)

/*------------------------------------------------------------------------
 * convert a pointer name to a number
 */
int xvt_w_ptr_value (const char *name)
{
	const POINTER_TYPES *	cp;

	/*--------------------------------------------------------------------
	 * check for null name
	 */
	if (name == 0 || *name == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * if string is numeric, just return numeric value
	 */
	if (isdigit(*name))
	{
		int code = atoi(name) & ~1;

		if (code < 0 || code > XC_num_glyphs)
			return (-1);

		return (code);
	}

	/*--------------------------------------------------------------------
	 * lookup name in cursor-name table
	 */
	for (cp = xvt_w_ptr_tbl; cp->ptr_name; cp++)
	{
		if (xvt_strccmpc(cp->ptr_name, name) == 0)
			return (cp->ptr_code);
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * get a pointer name/code from a number
 */
const char * xvt_w_ptr_name (int n)
{
	if (n < 0 || n >= NUM_POINTERS)
		return (0);

	return (xvt_w_ptr_tbl[n].ptr_name);
}

int xvt_w_ptr_code (int n)
{
	if (n < 0 || n >= NUM_POINTERS)
		return (-1);

	return (xvt_w_ptr_tbl[n].ptr_code);
}

