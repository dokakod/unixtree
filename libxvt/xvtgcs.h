/*------------------------------------------------------------------------
 * char-set tables
 */
#ifndef XVTGCS_H
#define XVTGCS_H

/*------------------------------------------------------------------------
 * vt char-set codes
 */
#define VT_ALT			'0'					/* line-drawing				*/
#define VT_UK			'A'					/* United Kingdom			*/
#define VT_US			'B'					/* US (USASCII)				*/
#define VT_ALTROM_S		'1'					/* alt ROM standard			*/
#define VT_ALTROM_G		'2'					/* alt ROM graphic			*/

/* these are not implemented */

#define VT_DUTCH		'4'					/* Multinational			*/
#define VT_FINNISH1		'5'					/* Finnish					*/
#define VT_FINNISH2		'C'					/* Finnish					*/
#define VT_FRENCH		'R'					/* French					*/
#define VT_FRENCHCAN	'Q'					/* French Canadian			*/
#define VT_GERMAN		'K'					/* German					*/
#define VT_ITALIAN		'Y'					/* Italian					*/
#define VT_NORWEGIAN1	'6'					/* Norwegian/Danish			*/
#define VT_NORWEGIAN2	'E'					/* Norwegian/Danish			*/
#define VT_SPANISH		'Z'					/* Spanish					*/
#define VT_SWEDISH1		'7'					/* Swedish					*/
#define VT_SPANISH2		'H'					/* Swedish					*/
#define VT_SWISS		'='					/* Swiss					*/

/*------------------------------------------------------------------------
 * gcs char-set codes (actually indexes into xvt_gcs_tbl[])
 */
#define GCS_US			0					/* 'B'	USASCII				*/
#define GCS_UK			1					/* 'A'	UK					*/
#define GCS_ALT			2					/* '0'	DEC line-drawing	*/

/*------------------------------------------------------------------------
 * functions
 */
extern int			xvt_gcs_get_gcs_code	(int vt_code);

extern const char *	xvt_gcs_get_name		(int gcs_code);
extern int			xvt_gcs_get_char		(int gcs_code, int c);

#endif /* XVTGCS_H */
