/*------------------------------------------------------------------------
 * This is the mapping table for the ACS characters
 *
 * This table is designed to make this curses implementation compatible
 * with "standard" curses.
 */
#include "wincommon.h"

const chtype acs_map[] =
{
	/* 0x00 - 0x1f */

	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,

	/* 0x20 - 0x2f */

	0, 0, 0, 0, 0, 0, 0, 0,
	0,
	0,
	0,
	'>',				/* +	ACS_RARROW	t5410	*/
	'<',				/* ,	ACS_LARROW	t5410	*/
	'^',				/* -	ACS_UARROW	t5410	*/
	'v',				/* .	ACS_DARROW	t5410	*/
	0,

	/* 0x30 - 0x5f */

	0xdb | A_ALTCHARSET,		/* 0	ACS_BLOCK	t5410	*/
	   0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,

	/* 0x60 - 0x7f */

	0xb2 | A_ALTCHARSET,		/* `	ACS_DIAMOND	vt100	*/
	0xb0 | A_ALTCHARSET,		/* a	ACS_CKBOARD	vt100	*/
	0,				/* b	ACS_HTAB	vt100	*/
	0,				/* c	ACS_FF		vt100	*/
	0,				/* d	ACS_CR		vt100	*/
	0,				/* e	ACS_LF		vt100	*/
	0xf8 | A_ALTCHARSET,		/* f	ACS_DEGREE	vt100	*/
	0xf1 | A_ALTCHARSET,		/* g	ACS_PLMINUS	vt100	*/

	0xb2 | A_ALTCHARSET,		/* h	ACS_BOARD	t5410	*/
	0xe8 | A_ALTCHARSET,		/* i	ACS_LANTERN	t5410	*/
	0xd9 | A_ALTCHARSET,		/* j	ACS_LRCORNER	vt100	*/
	0xbf | A_ALTCHARSET,		/* k	ACS_URCORNER	vt100	*/
	0xda | A_ALTCHARSET,		/* l	ACS_ULCORNER	vt100	*/
	0xc0 | A_ALTCHARSET,		/* m	ACS_LLCORNER	vt100	*/
	0xc5 | A_ALTCHARSET,		/* n	ACS_PLUS	vt100	*/
	'-',				/* o	ACS_S1		vt100	*/

	'-',				/* p	ACS_S3		vt100	*/
	0xc4 | A_ALTCHARSET,		/* q	ACS_HLINE	vt100	*/
	'-',				/* r	ACS_S7		vt100	*/
	'-',				/* s	ACS_S9		vt100	*/
	0xc3 | A_ALTCHARSET,		/* t	ACS_LTEE	vt100	*/
	0xb9 | A_ALTCHARSET,		/* u	ACS_RTEE	vt100	*/
	0xc1 | A_ALTCHARSET,		/* v	ACS_BTEE	vt100	*/
	0xc2 | A_ALTCHARSET,		/* w	ACS_TTEE	vt100	*/

	0xb3 | A_ALTCHARSET,		/* x	ACS_VLINE	vt100	*/
	0xf3 | A_ALTCHARSET,		/* y	ACS_LE		vt100	*/
	0xf2 | A_ALTCHARSET,		/* z	ACS_GE		vt100	*/
	0xe3 | A_ALTCHARSET,		/* {	ACS_PI		vt100	*/
	0,				/* |	ACS_NE		vt100	*/
	0,				/* }	ACS_POUND	vt100	*/
	0xf9 | A_ALTCHARSET,		/* ~	ACS_BULLET	vt100	*/
	0
};
