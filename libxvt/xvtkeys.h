/*------------------------------------------------------------------------
 * key strings
 */
#ifndef XVTKEYS_H
#define XVTKEYS_H

/*------------------------------------------------------------------------
 * key defines
 */
#define XVT_KEY_FN			0x0100			/* function keys		*/
#define XVT_KEY_KP			0x0200			/* keypad   keys		*/
#define XVT_KEY_CS			0x0200			/* cursor   keys		*/
#define XVT_KEY_NL			0x0200			/* numlock  keys		*/
#define XVT_KEY_MS			0x0400			/* misc     keys		*/
#define XVT_KEY_PF			0x0800			/* prefix   keys		*/
#define XVT_KEY_OT			0x1000			/* other    keys		*/
#define XVT_KEY_MASK		0x7f00

#define XVT_KEY_SHFT		0
#define XVT_KEY_CTRL		1
#define XVT_KEY_META		2
#define XVT_KEY_NUML		3

#define XVT_KEY_FN_01		( XVT_KEY_FN | 0x00 )
#define XVT_KEY_FN_02		( XVT_KEY_FN | 0x01 )
#define XVT_KEY_FN_03		( XVT_KEY_FN | 0x02 )
#define XVT_KEY_FN_04		( XVT_KEY_FN | 0x03 )
#define XVT_KEY_FN_05		( XVT_KEY_FN | 0x04 )
#define XVT_KEY_FN_06		( XVT_KEY_FN | 0x05 )
#define XVT_KEY_FN_07		( XVT_KEY_FN | 0x06 )
#define XVT_KEY_FN_08		( XVT_KEY_FN | 0x07 )
#define XVT_KEY_FN_09		( XVT_KEY_FN | 0x08 )
#define XVT_KEY_FN_10		( XVT_KEY_FN | 0x09 )
#define XVT_KEY_FN_11		( XVT_KEY_FN | 0x0a )
#define XVT_KEY_FN_12		( XVT_KEY_FN | 0x0b )
#define XVT_KEY_FN_13		( XVT_KEY_FN | 0x0c )
#define XVT_KEY_FN_14		( XVT_KEY_FN | 0x0d )
#define XVT_KEY_FN_15		( XVT_KEY_FN | 0x0e )
#define XVT_KEY_FN_16		( XVT_KEY_FN | 0x0f )

#define XVT_KEY_KP_0		( XVT_KEY_KP | 0x00 )
#define XVT_KEY_KP_1		( XVT_KEY_KP | 0x01 )
#define XVT_KEY_KP_2		( XVT_KEY_KP | 0x02 )
#define XVT_KEY_KP_3		( XVT_KEY_KP | 0x03 )
#define XVT_KEY_KP_4		( XVT_KEY_KP | 0x04 )
#define XVT_KEY_KP_5		( XVT_KEY_KP | 0x05 )
#define XVT_KEY_KP_6		( XVT_KEY_KP | 0x06 )
#define XVT_KEY_KP_7		( XVT_KEY_KP | 0x07 )
#define XVT_KEY_KP_8		( XVT_KEY_KP | 0x08 )
#define XVT_KEY_KP_9		( XVT_KEY_KP | 0x09 )
#define XVT_KEY_KP_DELETE	( XVT_KEY_KP | 0x0a )
#define XVT_KEY_KP_SUB		( XVT_KEY_KP | 0x0b )
#define XVT_KEY_KP_ADD		( XVT_KEY_KP | 0x0c )
#define XVT_KEY_KP_DIV		( XVT_KEY_KP | 0x0d )
#define XVT_KEY_KP_MUL		( XVT_KEY_KP | 0x0e )
#define XVT_KEY_KP_ENTER	( XVT_KEY_KP | 0x0f )

#define XVT_KEY_CS_0		( XVT_KEY_CS | 0x00 )
#define XVT_KEY_CS_1		( XVT_KEY_CS | 0x01 )
#define XVT_KEY_CS_2		( XVT_KEY_CS | 0x02 )
#define XVT_KEY_CS_3		( XVT_KEY_CS | 0x03 )
#define XVT_KEY_CS_4		( XVT_KEY_CS | 0x04 )
#define XVT_KEY_CS_5		( XVT_KEY_CS | 0x05 )
#define XVT_KEY_CS_6		( XVT_KEY_CS | 0x06 )
#define XVT_KEY_CS_7		( XVT_KEY_CS | 0x07 )
#define XVT_KEY_CS_8		( XVT_KEY_CS | 0x08 )
#define XVT_KEY_CS_9		( XVT_KEY_CS | 0x09 )
#define XVT_KEY_CS_DELETE	( XVT_KEY_CS | 0x0a )
#define XVT_KEY_CS_SUB		( XVT_KEY_CS | 0x0b )
#define XVT_KEY_CS_ADD		( XVT_KEY_CS | 0x0c )
#define XVT_KEY_CS_DIV		( XVT_KEY_CS | 0x0d )
#define XVT_KEY_CS_MUL		( XVT_KEY_CS | 0x0e )
#define XVT_KEY_CS_ENTER	( XVT_KEY_CS | 0x0f )

#define XVT_KEY_NL_0		( XVT_KEY_NL | 0x00 )
#define XVT_KEY_NL_1		( XVT_KEY_NL | 0x01 )
#define XVT_KEY_NL_2		( XVT_KEY_NL | 0x02 )
#define XVT_KEY_NL_3		( XVT_KEY_NL | 0x03 )
#define XVT_KEY_NL_4		( XVT_KEY_NL | 0x04 )
#define XVT_KEY_NL_5		( XVT_KEY_NL | 0x05 )
#define XVT_KEY_NL_6		( XVT_KEY_NL | 0x06 )
#define XVT_KEY_NL_7		( XVT_KEY_NL | 0x07 )
#define XVT_KEY_NL_8		( XVT_KEY_NL | 0x08 )
#define XVT_KEY_NL_9		( XVT_KEY_NL | 0x09 )
#define XVT_KEY_NL_DECIMAL	( XVT_KEY_NL | 0x0a )
#define XVT_KEY_NL_SUB		( XVT_KEY_NL | 0x0b )
#define XVT_KEY_NL_ADD		( XVT_KEY_NL | 0x0c )
#define XVT_KEY_NL_DIV		( XVT_KEY_NL | 0x0d )
#define XVT_KEY_NL_MUL		( XVT_KEY_NL | 0x0e )
#define XVT_KEY_NL_ENTER	( XVT_KEY_NL | 0x0f )

#define XVT_KEY_MS_PAUSE	( XVT_KEY_MS | 0x00 )
#define XVT_KEY_MS_PRTSCR	( XVT_KEY_MS | 0x01 )
#define XVT_KEY_MS_SCRLCK	( XVT_KEY_MS | 0x02 )
#define XVT_KEY_MS_SYSREQ	( XVT_KEY_MS | 0x03 )
#define XVT_KEY_MS_BREAK	( XVT_KEY_MS | 0x04 )
#define XVT_KEY_MS_RESIZE	( XVT_KEY_MS | 0x05 )

#define XVT_KEY_PF_SHFT		( XVT_KEY_PF | XVT_KEY_SHFT )
#define XVT_KEY_PF_CTRL		( XVT_KEY_PF | XVT_KEY_CTRL )
#define XVT_KEY_PF_META		( XVT_KEY_PF | XVT_KEY_META )
#define XVT_KEY_PF_NUML		( XVT_KEY_PF | XVT_KEY_NUML )

#define XVT_KEY_OT_UNKNOWN	( XVT_KEY_OT | 0x00 )

/*------------------------------------------------------------------------
 * key mask values & friends
 */
#define KEY_MASK_NONE		0x00
#define KEY_MASK_SHFT		0x01
#define KEY_MASK_CTRL		0x02
#define KEY_MASK_META		0x04
#define KEY_MASK_NUML		0x08

/*------------------------------------------------------------------------
 * key data structs
 */
struct key_data
{
	const char *	send;				/* string to send	*/
	const char *	name;				/* printable name	*/
};
typedef struct key_data KEY_DATA;

#endif /* XVTKEYS_H */
