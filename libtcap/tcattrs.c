/*------------------------------------------------------------------------
 * attribute name/value routines
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * key names for "regular" keys
 */
static const char * tcap_keynames[] =
{
	"^@",   "^A",   "^B",   "^C",   "^D",   "^E",   "^F",   "^G",
	"^H",   "^I",   "^J",   "^K",   "^L",   "^M",   "^N",   "^O",

	"^P",   "^Q",   "^R",   "^S",   "^T",   "^U",   "^V",   "^W",
	"^X",   "^Y",   "^Z",   "^[",   "^\\",  "^]",   "^^",   "^_",

	" ",    "!",    "\"",   "#",    "$",    "%",    "&",    "'",
	"(",    ")",    "*",    "+",    ",",    "-",    ".",    "/",

	"0",    "1",    "2",    "3",    "4",    "5",    "6",    "7",
	"8",    "9",    ":",    ";",    "<",    "=",    ">",    "?",

	"@",    "A",    "B",    "C",    "D",    "E",    "F",    "G",
	"H",    "I",    "J",    "K",    "L",    "M",    "N",    "O",

	"P",    "Q",    "R",    "S",    "T",    "U",    "V",    "W",
	"X",    "Y",    "Z",    "[",    "\\",   "]",    "^",    "_",

	"`",    "a",    "b",    "c",    "d",    "e",    "f",    "g",
	"h",    "i",    "j",    "k",    "l",    "m",    "n",    "o",

	"p",    "q",    "r",    "s",    "t",    "u",    "v",    "w",
	"x",    "y",    "z",    "{",    "|",    "}",    "~",    "^?",

	"M-^@", "M-^A", "M-^B", "M-^C", "M-^D", "M-^E", "M-^F", "M-^G",
	"M-^H", "M-^I", "M-^J", "M-^K", "M-^L", "M-^M", "M-^N", "M-^O",

	"M-^P", "M-^Q", "M-^R", "M-^S", "M-^T", "M-^U", "M-^V", "M-^W",
	"M-^X", "M-^Y", "M-^Z", "M-^[", "M-^\\","M-^]", "M-^^", "M-^_",

	"M- ",  "M-!",  "M-\"", "M-#",  "M-$",  "M-%",  "M-&",  "M-'",
	"M-(",  "M-)",  "M-*",  "M-+",  "M-,",  "M--",  "M-.",  "M-/",

	"M-0",  "M-1",  "M-2",  "M-3",  "M-4",  "M-5",  "M-6",  "M-7",
	"M-8",  "M-9",  "M-:",  "M-;",  "M-<",  "M-=",  "M->",  "M-?",

	"M-@",  "M-A",  "M-B",  "M-C",  "M-D",  "M-E",  "M-F",  "M-G",
	"M-H",  "M-I",  "M-J",  "M-K",  "M-L",  "M-M",  "M-N",  "M-O",

	"M-P",  "M-Q",  "M-R",  "M-S",  "M-T",  "M-U",  "M-V",  "M-W",
	"M-X",  "M-Y",  "M-Z",  "M-[",  "M-\\", "M-]",  "M-^",  "M-_",

	"M-`",  "M-a",  "M-b",  "M-c",  "M-d",  "M-e",  "M-f",  "M-g",
	"M-h",  "M-i",  "M-j",  "M-k",  "M-l",  "M-m",  "M-n",  "M-o",

	"M-p",  "M-q",  "M-r",  "M-s",  "M-t",  "M-u",  "M-v",  "M-w",
	"M-x",  "M-y",  "M-z",  "M-{",  "M-|",  "M-}",  "M-~",  "M-^?"
};

/*------------------------------------------------------------------------
 * tcap_get_key_value() - get key value for a given key name
 */
int tcap_get_key_value (const char *name)
{
	const KEY_DEF *	kp;

	if (tcap_trmncmp("key", name) == 0)
		name += 3;

	for (kp=tcap_key_tbl; kp->key_name; kp++)
	{
		if (tcap_trmcmp(name, kp->key_name) == 0)
		{
			return (kp->key_code);
		}
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * tcap_get_key_name() - get name for a key value
 */
const char * tcap_get_key_name (int code, char *buf)
{
	const KEY_DEF *	kp;

	/*--------------------------------------------------------------------
	 * bail if negative value
	 */
	if (code < 0)
	{
		return ("UNKNOWN KEY");
	}

	/*--------------------------------------------------------------------
	 * process all "regular" keys
	 */
	if (code < 256)
	{
		if (buf != 0)
		{
			strcpy(buf, tcap_keynames[code]);
			return (buf);
		}
		else
		{
			return (tcap_keynames[code]);
		}
	}

	/*--------------------------------------------------------------------
	 * process all "special" keys
	 */
	for (kp=tcap_key_tbl; kp->key_name; kp++)
	{
		if (code == kp->key_code)
		{
			if (buf != 0)
			{
				strcpy(buf, kp->key_name);
				return (buf);
			}
			else
			{
				return (kp->key_name);
			}
		}
	}

	/*--------------------------------------------------------------------
	 * something else ...
	 */
	if (buf != 0)
		return tcap_get_char_name(code, buf);
	else
		return ("UNKNOWN KEY");
}

/*------------------------------------------------------------------------
 * tcap_get_char_name() - get char value as a printable string
 */
const char * tcap_get_char_name (int code, char *buf)
{
	const char *hex = "0123456789abcdef";

	code &= 0xffff;

	/*--------------------------------------------------------------------
	 * check if control char
	 */
	if (code < ' ')
	{
		buf[0] = '^';
		buf[1] = code + '@';
		buf[2] = 0;

		return (buf);
	}

	/*--------------------------------------------------------------------
	 * check if regular char
	 */
	if (code < 0x7f)
	{
		buf[0] = code;
		buf[1] = 0;

		return (buf);
	}

	/*--------------------------------------------------------------------
	 * check if delete char
	 */
	if (code == 0x7f)
	{
		buf[0] = '^';
		buf[1] = '?';
		buf[2] = 0;

		return (buf);
	}

	/*--------------------------------------------------------------------
	 * high-bit char
	 */
	if (code <= 0xff)
	{
		buf[0] = '0';
		buf[1] = 'x';
		buf[2] = hex[(code & 0x00f0) >>  4];
		buf[3] = hex[(code & 0x000f)      ];
		buf[4] = 0;

		return (buf);
	}

	/*--------------------------------------------------------------------
	 * special char
	 */
	/* if (code > 0xff) */
	{
		buf[0] = '0';
		buf[1] = 'x';
		buf[2] = hex[(code & 0xf000) >> 12];
		buf[3] = hex[(code & 0x0f00) >>  8];
		buf[4] = hex[(code & 0x00f0) >>  4];
		buf[5] = hex[(code & 0x000f)      ];
		buf[6] = 0;

		return (buf);
	}
}

/*------------------------------------------------------------------------
 * tcap_get_attr_name_by_code() - get names associated with a mono attr value
 */
const char * tcap_get_attr_name_by_code (attr_t code, char *buf)
{
	int i;

	*buf = 0;

	/*--------------------------------------------------------------------
	 * process mono attributes
	 */
	if ((code & A_MONO) == A_NORMAL)
	{
		strcat(buf, tcap_mono_attrs[0].attr_name);
	}
	else
	{
		for (i=1; i<NUM_MONO_ATTRS; i++)
		{
			if (code & tcap_mono_attrs[i].attr_code)
			{
				if (*buf != 0)
					strcat(buf, "|");
				strcat(buf, tcap_mono_attrs[i].attr_name);
			}
		}
	}

	return (buf);
}

/*------------------------------------------------------------------------
 * tcap_get_color_name_by_code() - get "fg/bg" name for a color attribute
 */
const char * tcap_get_color_name_by_code (attr_t code, char *buf)
{
	strcpy(buf, tcap_get_fg_name_by_code(code));
	strcat(buf, "/");
	strcat(buf, tcap_get_bg_name_by_code(code));

	return (buf);
}

/*------------------------------------------------------------------------
 * tcap_get_fg_name_by_code() - get name associated with a fg attr value
 */
const char * tcap_get_fg_name_by_code (attr_t code)
{
	return (tcap_foregrnd_attrs[A_FG_CLRNUM(code)].attr_name);
}

/*------------------------------------------------------------------------
 * tcap_get_bg_name_by_code() - get name associated with a bg attr value
 */
const char * tcap_get_bg_name_by_code (attr_t code)
{
	return (tcap_backgrnd_attrs[A_BG_CLRNUM(code)].attr_name);
}

/*------------------------------------------------------------------------
 * tcap_get_attr_name_by_num() - get mono name attribute by index
 */
const char * tcap_get_attr_name_by_num (int n)
{
	if (n < 0 || n >= NUM_MONO_ATTRS)
		return ("?");

	return (tcap_mono_attrs[n].attr_name);
}

/*------------------------------------------------------------------------
 * tcap_get_fg_name_by_num() - get fg color name by index
 */
const char * tcap_get_fg_name_by_num (int n)
{
	if (n < 0 || n >= NUM_COLORS)
		return ("?");

	return (tcap_foregrnd_attrs[n].attr_name);
}

/*------------------------------------------------------------------------
 * tcap_get_bg_name_by_num() - get bg color name by index
 */
const char * tcap_get_bg_name_by_num (int n)
{
	if (n < 0 || n >= NUM_COLORS)
		return ("?");

	return (tcap_backgrnd_attrs[n].attr_name);
}

/*------------------------------------------------------------------------
 * tcap_get_attr_value() - get mono attr value by index
 */
attr_t tcap_get_attr_value_by_num (int n)
{
	if (n < 0 || n >= NUM_MONO_ATTRS)
		return (0);

	return (tcap_mono_attrs[n].attr_code);
}

/*------------------------------------------------------------------------
 * tcap_get_fg_value() - get fg attr value by index
 */
attr_t tcap_get_fg_value_by_num (int n)
{
	if (n < 0 || n >= NUM_COLORS)
		return (0);

	return (tcap_foregrnd_attrs[n].attr_code);
}

/*------------------------------------------------------------------------
 * tcap_get_bg_value() - get bg attr value by index
 */
attr_t tcap_get_bg_value_by_num (int n)
{
	if (n < 0 || n >= NUM_COLORS)
		return (0);

	return (tcap_backgrnd_attrs[n].attr_code);
}

/*------------------------------------------------------------------------
 * tcap_get_attr_num_by_name() - get index of a attr name
 */
int tcap_get_attr_num_by_name (const char *name)
{
	int n;

	for (n=0; n<NUM_MONO_ATTRS; n++)
	{
		if (tcap_trmcmp(name, tcap_mono_attrs[n].attr_name) == 0)
			return (n);
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * tcap_get_fg_num_by_name() - get index of a fg name
 */
int tcap_get_fg_num_by_name (const char *name)
{
	int n;

	for (n=0; n<NUM_COLORS; n++)
	{
		if (tcap_trmcmp(name, tcap_foregrnd_attrs[n].attr_name) == 0)
			return (n);
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * tcap_get_bg_num_by_name() - get index of a bg name
 */
int tcap_get_bg_num_by_name (const char *name)
{
	int n;

	for (n=0; n<NUM_COLORS; n++)
	{
		if (tcap_trmcmp(name, tcap_backgrnd_attrs[n].attr_name) == 0)
			return (n);
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * tcap_get_attr_num_by_code() - get index of a attr code
 */
int tcap_get_attr_num_by_code (attr_t code)
{
	int n;

	code &= A_MONO;
	for (n=0; n<NUM_MONO_ATTRS; n++)
	{
		if (tcap_mono_attrs[n].attr_code == code)
			return (n);
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * tcap_get_fg_num_by_code() - get index of a fg code
 */
int tcap_get_fg_num_by_code (attr_t code)
{
	int n = A_FG_CLRNUM(code);

	return (n);
}

/*------------------------------------------------------------------------
 * tcap_get_bg_num_by_code() - get index of a bg code
 */
int tcap_get_bg_num_by_code (attr_t code)
{
	int n = A_BG_CLRNUM(code);

	return (n);
}
