/*------------------------------------------------------------------------
 * definitions for box & line-drawing characters
 */
#ifndef	WBOX_H
#define	WBOX_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*@ +++API+++ */
/*------------------------------------------------------------------------
 * block letter size
 */
#define BLK_WIDTH	8		/* width  of a block letter */
#define BLK_HEIGHT	5		/* height of a block letter */

/*------------------------------------------------------------------------
 *	box types
 */
#define B_ASCII		0		/* ascii char boxes			*/
#define	B_SHSV		1		/* single horiz single vert	*/
#define	B_DHDV		2		/* double horiz double vert	*/
#define	B_DHSV		3		/* double horiz single vert	*/
#define	B_SHDV		4		/* single horiz double vert	*/
#define B_BLOCK		5		/* block chars				*/

/* I can never remember the order (HV / VH), so here's the other way */

#define	B_SVSH		B_SHSV	/* single vert single horiz	*/
#define	B_DVDH		B_DHDV	/* double vert double horiz	*/
#define	B_SVDH		B_DHSV	/* single vert double horiz	*/
#define	B_DVSH		B_SHDV	/* double vert single horiz	*/

/*------------------------------------------------------------------------
 * box char definitions
 *
 * A 3x3 box looks like this:
 *
 *	TL	TC	TR
 *	ML	MC	MR
 *	BL	BC	BR
 *
 * A 5x5 box looks like this:
 *
 *	TL	HO	TC	HO	TR
 *	VE		VE		VE
 *	ML	HO	MC	HO	MR
 *	VE		VE		VE
 *	BL	HO	BC	HO	BR
 */
#define	B_BL		0		/* bottom left		*/
#define B_BC		1		/* bottom center	*/
#define B_BR		2		/* bottom right		*/

#define B_ML		3		/* middle left		*/
#define B_MC		4		/* middle center	*/
#define B_MR		5		/* middle right		*/

#define B_TL		6		/* top    left		*/
#define B_TC		7		/* top    center	*/
#define B_TR		8		/* top    right		*/

#define B_VE		9		/* vert   line		*/
#define B_HO		10		/* horiz  line		*/

/*------------------------------------------------------------------------
 * box chars table
 */
extern const unsigned char box_chrs[6][11];

/*------------------------------------------------------------------------
 * ACS compatability definitions
 *
 * These definitions follow the VT100 naming convention
 */
extern const chtype	acs_map[];

/* box characters */

#define ACS_ULCORNER	(acs_map['l'])
#define ACS_LLCORNER	(acs_map['m'])
#define ACS_URCORNER	(acs_map['k'])
#define ACS_LRCORNER	(acs_map['j'])
#define ACS_RTEE		(acs_map['u'])
#define ACS_LTEE		(acs_map['t'])
#define ACS_BTEE		(acs_map['v'])
#define ACS_TTEE		(acs_map['w'])
#define ACS_PLUS		(acs_map['n'])

/* line characters */

#define ACS_HLINE		(acs_map['q'])
#define ACS_VLINE		(acs_map['x'])

/* special characters */

#define ACS_BLOCK		(acs_map['0'])
#define ACS_DIAMOND		(acs_map['`'])
#define ACS_CKBOARD		(acs_map['a'])
#define ACS_DEGREE		(acs_map['f'])
#define ACS_PLMINUS		(acs_map['g'])
#define ACS_BULLET		(acs_map['~'])
#define ACS_S1			(acs_map['o'])
#define ACS_S3			(acs_map['p'])
#define ACS_S5			ACS_HLINE
#define ACS_S7			(acs_map['r'])
#define ACS_S9			(acs_map['s'])
#define ACS_LEQUAL		(acs_map['y'])
#define ACS_GEQUAL		(acs_map['z'])
#define ACS_PI			(acs_map['{'])
#define ACS_NEQUAL		(acs_map['|'])
#define ACS_STERLING	(acs_map['}'])

/* extended (non-VT100) characters */

#define ACS_LARROW		(acs_map[','])
#define ACS_RARROW		(acs_map['+'])
#define ACS_UARROW		(acs_map['-'])
#define ACS_DARROW		(acs_map['.'])
#define ACS_LANTERN		(acs_map['i'])
#define ACS_BOARD		(acs_map['h'])

/*@ ---API--- */
/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif	/* WBOX_H */
