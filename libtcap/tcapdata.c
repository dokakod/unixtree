/*------------------------------------------------------------------------
 * data & routines for initializing tcap_data
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * default empty tcap_data struct
 */
const TCAP_DATA tcap_def_tcap_data =
{
	/*--------------------------------------------------------------------
	 * misc info
	 */
	0,						/* ERRS		error tbl pointer	*/
	0,						/* DEFS		def list			*/

	0,						/* TTY_TYPE	name of term used	*/
	TCAP_DB_NONE,			/* DB_TYPE	database type		*/

	/*--------------------------------------------------------------------
	 * TERM_INTS
	 */
	{
		0,					/* MAXLINES	number of lines		*/
		0,					/* MAXCOLS	number of columns	*/
		8,					/* TABSIZE  width of a tab		*/

		3,					/* NB		num mouse buttons	*/
		500,				/* DBLCLK_INTERVAL				*/

		500,				/* PREFIX_INTERVAL				*/

		0,					/* MC		max num of colors	*/
		0,					/* MAX_FG						*/
		0,					/* MAX_BG						*/

		{					/* COLOR_FG_MAP table			*/
			COLOR_BLACK,
			COLOR_RED,
			COLOR_GREEN,
			COLOR_YELLOW,
			COLOR_BLUE,
			COLOR_MAGENTA,
			COLOR_CYAN,
			COLOR_WHITE,

			COLOR_GREY,
			COLOR_LTRED,
			COLOR_LTGREEN,
			COLOR_LTYELLOW,
			COLOR_LTBLUE,
			COLOR_LTMAGENTA,
			COLOR_LTCYAN,
			COLOR_LTWHITE
		},

		{					/* COLOR_BG_MAP table			*/
			COLOR_BLACK,
			COLOR_RED,
			COLOR_GREEN,
			COLOR_YELLOW,
			COLOR_BLUE,
			COLOR_MAGENTA,
			COLOR_CYAN,
			COLOR_WHITE,

			COLOR_GREY,
			COLOR_LTRED,
			COLOR_LTGREEN,
			COLOR_LTYELLOW,
			COLOR_LTBLUE,
			COLOR_LTMAGENTA,
			COLOR_LTCYAN,
			COLOR_LTWHITE
		}
	},

	/*--------------------------------------------------------------------
	 * TERM_BOOLS
	 */
	{
		FALSE,				/* AM		automatic margins	*/
		TRUE,				/* ALC		allow color			*/
		TRUE,				/* MD		display mouse 		*/
		TRUE,				/* IFC		insert final char	*/
		TRUE,				/* ALA		allow alt chars		*/
		FALSE				/* SCN		use scancodes		*/
	},

	/*--------------------------------------------------------------------
	 * TERM_PARMS
	 */
	{
		NULL,				/* CM		cursor motion		*/
		NULL,				/* CM_TC	termcap  CM			*/
		NULL,				/* CM_TI	terminfo CM			*/

		NULL,				/* CSR		reg CS				*/
		NULL,				/* CSR_TC	reg termcap  CS		*/
		NULL,				/* CSR_TI	reg terminfo CS		*/

		NULL,				/* CSM		map CS				*/
		NULL,				/* CSM_TC	map termcap  CS		*/
		NULL,				/* CSM_TI	map terminfo CS		*/

		NULL,				/* FGR		reg FG				*/
		NULL,				/* FGR_TC	reg termcap  FG		*/
		NULL,				/* FGR_TI	reg terminfo FG		*/

		NULL,				/* FGM		map FG				*/
		NULL,				/* FGM_TC	map termcap  FG		*/
		NULL,				/* FGM_TI	map terminfo FG		*/

		NULL,				/* BGR		reg BG				*/
		NULL,				/* BGR_TC	reg termcap  BG		*/
		NULL,				/* BGR_TI	reg terminfo BG		*/

		NULL,				/* BGM		map BG				*/
		NULL,				/* BGM_TC	map termcap  BG		*/
		NULL,				/* BGM_TI	map terminfo BG		*/
	},

	/*--------------------------------------------------------------------
	 * TERM_STRS
	 */
	{
		NULL,				/* AL		add line			*/
		NULL,				/* DL		delete line			*/

		NULL,				/* IC		insert char			*/
		NULL,				/* DC		delete char			*/

		NULL,				/* BL		bell				*/
		NULL,				/* VB		visible bell		*/

		NULL,				/* CL		clear screen		*/
		NULL,				/* CE		clear to EOL		*/
		NULL,				/* CD		clear to EOS		*/

		NULL,				/* VI		cursor invisible	*/
		NULL,				/* VE		cursor visible		*/
		NULL,				/* VS		cursor very visible	*/

		NULL,				/* TI		terminal init		*/
		NULL,				/* TE		terminal end		*/

		NULL,				/* TTS		title begin			*/
		NULL,				/* TTE		title end			*/

		NULL,				/* ICC		icon close			*/
		NULL,				/* ICO		icon open			*/

		NULL,				/* ICBS		beg icon bits		*/
		NULL,				/* ICBE		beg icon bits		*/

		NULL,				/* ACE		alt-chars enable	*/
		NULL,				/* ACD		alt-chars disable	*/

		NULL,				/* KBO		keyboard map on		*/
		NULL,				/* KBF		keyboard map off	*/

		NULL,				/* PRO		printer on			*/
		NULL,				/* PRF		printer off			*/

		/* Attributes */

		NULL,				/* STS		beg standout		*/
		NULL,				/* STE		end standout		*/

		NULL,				/* ULS		beg underline		*/
		NULL,				/* ULE		end underline		*/

		NULL,				/* BDS		beg bold			*/
		NULL,				/* BDE		end bold			*/

		NULL,				/* BLS		beg blink			*/
		NULL,				/* BLE		end blink			*/

		NULL,				/* DMS		beg dim				*/
		NULL,				/* DME		end dim				*/

		NULL,				/* IVS		beg invisible		*/
		NULL,				/* IVE		end invisible		*/

		NULL,				/* PRS		beg protected		*/
		NULL,				/* PRE		end protected		*/

		/* Reg char set entries */

		{
			NULL,			/* R1S		beg reg char set 1	*/
			NULL,			/* R2S		beg reg char set 2	*/
			NULL,			/* R3S		beg reg char set 3	*/
			NULL			/* R4S		beg reg char set 4	*/
		},

		{
			NULL,			/* R1E		end reg char set 1	*/
			NULL,			/* R2E		end reg char set 2	*/
			NULL,			/* R3E		end reg char set 3	*/
			NULL			/* R4E		end reg char set 4	*/
		},

		/* Alt char set entries */

		{
			NULL,			/* A1S		beg alt char set 1	*/
			NULL,			/* A2S		beg alt char set 2	*/
			NULL,			/* A3S		beg alt char set 3	*/
			NULL			/* A4S		beg alt char set 4	*/
		},

		{
			NULL,			/* A1E		end alt char set 1	*/
			NULL,			/* A2E		end alt char set 2	*/
			NULL,			/* A3E		end alt char set 3	*/
			NULL			/* A4E		end alt char set 4	*/
		},

		/* mouse entries */

		NULL,				/* MBEG		mouse on 			*/
		NULL,				/* MEND		mouse off			*/
		NULL,				/* MSTR		mouse input string	*/

		/* miscellaneous */

		NULL,				/* SNAP		snap picture taken	*/
		NULL,				/* DESC		term description	*/
		NULL,				/* ACS		acs chars			*/

		/* fg color table */

		{
			NULL,			/* black			*/
			NULL,			/* blue				*/
			NULL,			/* green			*/
			NULL,			/* cyan				*/
			NULL,			/* red				*/
			NULL,			/* magenta			*/
			NULL,			/* yellow			*/
			NULL,			/* white			*/

			NULL,			/* grey				*/
			NULL,			/* lt-blue			*/
			NULL,			/* lt-green			*/
			NULL,			/* lt-cyan			*/
			NULL,			/* lt-red			*/
			NULL,			/* lt-magenta		*/
			NULL,			/* lt-yellow		*/
			NULL			/* lt-white			*/
		},

		/* bg color table */

		{
			NULL,			/* black      bkgnd	*/
			NULL,			/* blue       bkgnd	*/
			NULL,			/* green      bkgnd	*/
			NULL,			/* cyan       bkgnd	*/
			NULL,			/* red        bkgnd	*/
			NULL,			/* magenta    bkgnd	*/
			NULL,			/* yellow     bkgnd	*/
			NULL,			/* white      bkgnd	*/

			NULL,			/* grey       bkgnd	*/
			NULL,			/* lt-blue    bkgnd	*/
			NULL,			/* lt-green   bkgnd	*/
			NULL,			/* lt-cyan    bkgnd	*/
			NULL,			/* lt-red     bkgnd	*/
			NULL,			/* lt-magenta bkgnd	*/
			NULL,			/* lt-yellow  bkgnd	*/
			NULL			/* lt-white   bkgnd	*/
		}
	},

	/*--------------------------------------------------------------------
	 * Parm data entries
	 */
	{
		{ S_ZERO, S_ZERO, 0, 0, NULL },	/* cm entry			*/
		{ S_ZERO, S_ZERO, 0, 0, NULL },	/* cs entry			*/
		{ S_ZERO, S_ZERO, 0, 0, NULL },	/* fg entry			*/
		{ S_ZERO, S_ZERO, 0, 0, NULL }	/* bg entry			*/
	},

	/*--------------------------------------------------------------------
	 * Translation table entries 
	 */
	{					/* INP_TBL	input translation table */
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
		0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
		0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
		0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
		0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
		0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
		0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
		0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
		0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
		0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
		0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
		0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
		0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
		0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
		0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
		0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
		0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
		0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
		0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
		0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
		0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
		0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
		0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
		0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
		0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
		0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
		0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
	},

	{					/* REG_TBL	reg output translation table */
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
		0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
		0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
		0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
		0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
		0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
		0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
		0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
		0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
		0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
		0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
		0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
		0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
		0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
		0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
		0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
		0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
		0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
		0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
		0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
		0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
		0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
		0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
		0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
		0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
		0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
		0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
	},

	{					/* ALT_TBL	alt output translation table */
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
		0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
		0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
		0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
		0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
		0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
		0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
		0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
		0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
		0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
		0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
		0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
		0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
		0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
		0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
		0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
		0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
		0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
		0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
		0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
		0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
		0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
		0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
		0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
		0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
		0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
		0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
	},

	/*--------------------------------------------------------------------
	 * font tables
	 */
	{					/* REG_FONT_TBL		*/
		0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	/* 00 - 0f */
		0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	/* 10 - 1f */
		0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	/* 20 - 2f */
		0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	/* 30 - 3f */
		0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	/* 40 - 4f */
		0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	/* 50 - 5f */
		0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	/* 60 - 6f */
		0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	/* 70 - 7f */

		0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	/* 80 - 8f */
		0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	/* 90 - 9f */
		0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	/* a0 - af */
		0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	/* b0 - bf */
		0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	/* c0 - cf */
		0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	/* d0 - df */
		0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	/* e0 - ef */
		0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0	/* f0 - ff */
	},

	{					/* ALT_FONT_TBL		*/
		2, 2, 2, 2,  2, 2, 2, 2,  2, 2, 2, 2,  2, 2, 2, 2,	/* 00 - 0f */
		2, 2, 2, 2,  2, 2, 2, 2,  2, 2, 2, 2,  2, 2, 2, 2,	/* 10 - 1f */

		0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	/* 20 - 2f */
		0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	/* 30 - 3f */
		0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	/* 40 - 4f */
		0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	/* 50 - 5f */
		0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	/* 60 - 6f */
		0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	/* 70 - 7f */

		1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,	/* 80 - 8f */
		1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,	/* 90 - 9f */
		1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,	/* a0 - af */
		1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,	/* b0 - bf */
		1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,	/* c0 - cf */
		1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,	/* d0 - df */
		1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,	/* e0 - ef */
		1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1	/* f0 - ff */
	},

	/*--------------------------------------------------------------------
	 * color pair table
	 */
	{					/* CLR_PAIRS		*/
#		define U	A_UNSET
		U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,	/* 00 - 0f */
		U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,	/* 10 - 1f */
		U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,	/* 20 - 2f */
		U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,	/* 30 - 3f */
		U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,	/* 40 - 4f */
		U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,	/* 50 - 5f */
		U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,	/* 60 - 6f */
		U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,	/* 70 - 7f */

		U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,	/* 80 - 8f */
		U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,	/* 90 - 9f */
		U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,	/* a0 - af */
		U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,	/* b0 - bf */
		U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,	/* c0 - cf */
		U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,	/* d0 - df */
		U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,	/* e0 - ef */
		U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U	/* f0 - ff */
#		undef U
	},

	/*--------------------------------------------------------------------
	 * function key info
	 */
	0,					/* FKS_START		*/
	0,					/* FKS_LAST			*/

	0					/* KFUNCS			*/
};

/*------------------------------------------------------------------------
 * tcap_clr_free() - free all color entries in a screen
 */
static void tcap_clr_free (TCAP_DATA *td)
{
	if (td != 0)
	{
		int i;

		for (i=0; i<NUM_COLORS; i++)
		{
			if (is_cmd_pres(td->strs.clr_fg[i]))
				FREE(td->strs.clr_fg[i]);
			td->strs.clr_fg[i] = 0;
		}

		for (i=0; i<NUM_COLORS; i++)
		{
			if (is_cmd_pres(td->strs.clr_bg[i]))
				FREE(td->strs.clr_bg[i]);
			td->strs.clr_bg[i] = 0;
		}
	}
}

/*------------------------------------------------------------------------
 * tcap_init_tcap_data() - initialize a tcap struct
 */
TCAP_DATA * tcap_init_tcap_data (void)
{
	TCAP_DATA *t;

	t = (TCAP_DATA *)MALLOC(sizeof(*t));
	if (t != 0)
	{
		memcpy(t, &tcap_def_tcap_data, sizeof(*t));
	}

	return (t);
}

/*------------------------------------------------------------------------
 * tcap_free_tcap_data() - free all data in a tcap struct
 */
void tcap_free_tcap_data (TCAP_DATA *td)
{
	if (td != 0)
	{
		if (td->tty_type != 0)
			FREE(td->tty_type);

		tcap_tc_free (td);
		tcap_clr_free(td);
		tcap_kf_free (td);
		tcap_fk_free (td);

		FREE(td);
	}
}
