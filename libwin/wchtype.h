/*------------------------------------------------------------------------
 * this header defines everything about chtypes
 */
#ifndef WCHTYPE_H
#define WCHTYPE_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*@ +++API+++ */
/*------------------------------------------------------------------------
 * A "chtype" is the type of data stored in the window lines.
 * It contains the character, chararacter-code, and attributes as follows:
 *
 * +--------+--------+--------+--------+
 * |ffffbbbb|Ammmmmmm|kkkkkkkk|cccccccc|
 * +--------+--------+--------+--------+
 *
 * where:
 *
 *	attr_t
 *		ffff		foreground color
 *		bbbb		background color
 *		A			alternate char set bit
 *		mmmmmmm		mono-attribute bits
 *	code_t
 *		kkkkkkkk	character-code
 *	char_t
 *		ccccc		character
 *
 * The "character-code" is used by the mouse routines for display and input.
 */

/*------------------------------------------------------------------------
 * basic types
 */
typedef unsigned int	chtype;			/* chtype definition	*/
typedef unsigned char	code_t;			/* character-code part	*/
typedef unsigned char	char_t;			/* character part		*/

/*------------------------------------------------------------------------
 * masks for the different parts of a chtype
 */
#define	A_ATTRIBUTES	( (chtype)0xffff0000 )		/* attribute part */
#define A_CHARCODE		( (chtype)0x0000ff00 )		/* char code part */
#define A_CHARTEXT		( (chtype)0x000000ff )		/* character part */

/*------------------------------------------------------------------------
 * macros to get chtype values
 */
#define A_GETATTR(ch)	( (attr_t)(((ch) & A_ATTRIBUTES)   ) )
#define A_GETCODE(ch)	( (code_t)(((ch) & A_CHARCODE) >> 8) )
#define A_GETTEXT(ch)	( (char_t)(((ch) & A_CHARTEXT)     ) )

/*------------------------------------------------------------------------
 * macros to set chtype values
 */
#define A_SETATTR(c)	( (chtype)(((c)     ) & A_ATTRIBUTES) )
#define A_SETCODE(k)	( (chtype)(((k) << 8) & A_CHARCODE)   )
#define A_SETTEXT(c)	( (chtype)(((c)     ) & A_CHARTEXT)   )

/*@ ---API--- */
/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* WCHTYPE_H */
