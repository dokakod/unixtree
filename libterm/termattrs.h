/*------------------------------------------------------------------------
 * Description of the attr_t (attribute)
 */
#ifndef WATTRS_H
#define WATTRS_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*@ +++API+++ */
/*------------------------------------------------------------------------
 * An "attr_t" contains attribute data as follows:
 *
 * +--------+--------+--------+--------+
 * |ffffbbbb|Ammmmmmm|        |        |
 * +--------+--------+--------+--------+
 *
 * where:
 *
 *	ffff		foreground color (0-15)	(NOT a bit mask!)
 *	bbbb		background color (0-15)	(NOT a bit mask!)
 *	A			alternate char set bit
 *	mmmmmmm		mono-attribute bits		(bit mask)
 *
 * This representation supports an attribute of both mono & color
 * attributes at the same time, although we know of no terminal
 * (or emulator) which supports this.
 *
 * Note that with this representation, it is not possible to distinguish
 * between a attr_t with mono attribute A_NORMAL and one with color
 * attribute of black/black.  This *should* not be a problem, since
 * users normally do not display characters as black/black.
 *
 * This type is designed to be able to be "or'd" with character data
 * to present a combined word of attribute & character.
 */
typedef unsigned int	attr_t;

/*------------------------------------------------------------------------
 * mono attributes
 */
#define A_MONO			(attr_t)0x007f0000		/* mono attributes mask */

#define A_NORMAL		(attr_t)0x00000000
#define A_REVERSE		(attr_t)0x00010000
#define A_STANDOUT		A_REVERSE
#define A_UNDERLINE		(attr_t)0x00020000
#define A_BOLD			(attr_t)0x00040000
#define A_BLINK			(attr_t)0x00080000
#define A_DIM			(attr_t)0x00100000
#define A_INVIS			(attr_t)0x00200000
#define A_PROTECT		(attr_t)0x00400000

#define NUM_MONO_ATTRS	8						/* number of mono attrs */

/*------------------------------------------------------------------------
 * attributes common to mono & color
 */
#define A_COMMON		(attr_t)0x00800000		/* common attributes mask */

#define A_ALTCHARSET	(attr_t)0x00800000

#define NUM_COMM_ATTRS	1						/* number of common attrs */

/*------------------------------------------------------------------------
 * color attributes
 */
#define A_COLOR			(attr_t)0xff000000		/* fg & bg    bit mask	*/

#define A_FG_BITS		(attr_t)0xf0000000		/* foreground bit mask	*/
#define A_BG_BITS		(attr_t)0x0f000000		/* background bit mask	*/

#define NUM_COLORS		16						/* number of colors	*/

/*------------------------------------------------------------------------
 * no attribute value (unset)
 */
#define A_UNSET			(A_MONO | A_COMMON | A_COLOR)

/*------------------------------------------------------------------------
 * convert color attribute to color number (0 - NUM_COLORS)
 */
#define A_FG_CLRNUM(a)	((int)(((a) & A_FG_BITS) >> 28))
#define A_BG_CLRNUM(a)	((int)(((a) & A_BG_BITS) >> 24))

/*------------------------------------------------------------------------
 * convert color number (0 - NUM_COLORS) to color attribute
 */
#define A_FG_CLRVAL(f)	(((attr_t)(f) & 0x0f) << 28)
#define A_BG_CLRVAL(b)	(((attr_t)(b) & 0x0f) << 24)

#define A_CLR(f,b)		(A_FG_CLRVAL(f) | A_BG_CLRVAL(b))

/*------------------------------------------------------------------------
 * color codes
 */
#define COLOR_BLACK			0x00
#define COLOR_RED			0x01
#define COLOR_GREEN			0x02
#define COLOR_YELLOW		0x03
#define COLOR_BLUE			0x04
#define COLOR_MAGENTA		0x05
#define COLOR_CYAN			0x06
#define COLOR_WHITE			0x07

#define COLOR_BRIGHT		0x08

#define COLOR_GREY			(COLOR_BLACK   | COLOR_BRIGHT)
#define COLOR_LTRED			(COLOR_RED     | COLOR_BRIGHT)
#define COLOR_LTGREEN		(COLOR_GREEN   | COLOR_BRIGHT)
#define COLOR_LTYELLOW		(COLOR_YELLOW  | COLOR_BRIGHT)
#define COLOR_LTBLUE		(COLOR_BLUE    | COLOR_BRIGHT)
#define COLOR_LTMAGENTA		(COLOR_MAGENTA | COLOR_BRIGHT)
#define COLOR_LTCYAN		(COLOR_CYAN    | COLOR_BRIGHT)
#define COLOR_LTWHITE		(COLOR_WHITE   | COLOR_BRIGHT)

#define COLOR_FG_BRIGHT		A_FG_CLRVAL(COLOR_BRIGHT)
#define COLOR_BG_BRIGHT		A_BG_CLRVAL(COLOR_BRIGHT)

/*------------------------------------------------------------------------
 * defines for curses compatability
 */
#define COLORS				NUM_COLORS
#define COLOR_PAIRS			(NUM_COLORS * NUM_COLORS)

/*@ ---API--- */
/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* WATTRS_H */
