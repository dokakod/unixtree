/*------------------------------------------------------------------------
 * color routines
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * RGB table for defined colors
 *
 * NOTE: make sure this table is in the same order as COLOR_* (tcapattr.h)
 */
struct rgb
{
	short red;
	short green;
	short blue;
};
typedef struct rgb RGB;

static const RGB tcap_rgb[] =
{
/*	  red  green    blue						*/
	{   0,	   0,	   0 },		/* black		*/
	{   0,	   0,	 785 },		/* blue			*/
	{   0,	 785,	   0 },		/* green		*/
	{   0,	 785,	 785 },		/* cyan			*/
	{ 785,	   0,	   0 },		/* red			*/
	{ 785,	   0,	 785 },		/* magenta		*/
	{ 785,	 785,	   0 },		/* yellow		*/
	{ 785,	 785,	 785 },		/* white		*/

	{ 470,	 470,	 470 },		/* grey			*/
	{ 470,	 470,	1000 },		/* lt-blue		*/
	{ 470,	1000,	 470 },		/* lt-green		*/
	{ 470,	1000,	1000 },		/* lt-cyan		*/
	{1000,	 470,	 470 },		/* lt-red		*/
	{1000,	 470,	1000 },		/* lt-magenta	*/
	{1000,	1000,	 470 },		/* lt-yellow	*/
	{1000,	1000,	1000 }		/* lt-white		*/
};

/*------------------------------------------------------------------------
 * tcap_has_colors() - check if colors are defined for a screen
 */
int tcap_has_colors (const TERMINAL *tp)
{
	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0)
		return (FALSE);

	return (tp->scrn->color_flag);
}

/*------------------------------------------------------------------------
 * tcap_start_color() - initialize color processing for a screen
 */
int tcap_start_color (TERMINAL *tp)
{
	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * bail if no color capability
	 */
	if (! tcap_has_colors(tp))
		return (-1);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_init_pair() - define a color pair value
 */
int tcap_init_pair (TERMINAL *tp, short pair, short f, short b)
{
	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * bail if invalid pair number
	 */
	if (pair < 0 || pair >= COLOR_PAIRS)
		return (-1);

	/*--------------------------------------------------------------------
	 * bail if invalid fg value or invalid bg value
	 */
	if (f < 0 || f >= NUM_COLORS)
		return (-1);

	if (b < 0 || b >= NUM_COLORS)
		return (-1);

	/*--------------------------------------------------------------------
	 * store the value
	 */
	TERMINAL_LOCK(tp);
	{
		tp->tcap->clr_pairs[pair] = A_CLR(f, b);
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_pair_content() - get the fg/bg values for a color pair entry
 */
int tcap_pair_content (const TERMINAL *tp, short pair, short *f, short *b)
{
	attr_t a;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * bail if invalid pair value
	 */
	if (pair < 0 || pair >= COLOR_PAIRS)
		return (-1);

	/*--------------------------------------------------------------------
	 * bail if pair is unset
	 */
	a = tp->tcap->clr_pairs[pair];
	if (a == A_UNSET)
		return (-1);

	/*--------------------------------------------------------------------
	 * retrieve pair values
	 */
	*f = (short)A_FG_CLRNUM(a);
	*b = (short)A_BG_CLRNUM(a);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_init_all_pairs() - initialize all color pairs
 */
int tcap_init_all_pairs (TERMINAL *tp)
{
	int f;
	int b;
	int pair;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * set all pairs to default colors
	 */
	TERMINAL_LOCK(tp);
	{
		for (f=0; f<NUM_COLORS; f++)
		{
			for (b=0; b<NUM_COLORS; b++)
			{
				pair = (f << 4) | b;
				tp->tcap->clr_pairs[pair] = A_CLR(f, b);
			}
		}
	}
	TERMINAL_UNLOCK(tp);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_pair_number() - get the color-pair entry number for a given value
 */
int tcap_pair_number (const TERMINAL *tp, attr_t a)
{
	int i;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * look for given attr & return pair number if found
	 */
	a &= A_COLOR;
	for (i=0; i<COLOR_PAIRS; i++)
	{
		if (a == tp->tcap->clr_pairs[i])
			return (i);
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * tcap_can_change_color() - check if display can change colors
 *
 * NOTE: we don't support this functionality, so we just return FALSE always.
 */
int tcap_can_change_color (const TERMINAL *tp)
{
	return (FALSE);
}

/*------------------------------------------------------------------------
 * tcap_init_color() - change the RGB values for a color
 *
 * NOTE: we don't support this functionality, so we just return -1 always.
 */
int tcap_init_color (TERMINAL *tp, short color, short r, short g, short b)
{
	return (-1);
}

/*------------------------------------------------------------------------
 * tcap_color_content() - get the RGB values for a given color number
 */
int tcap_color_content (const TERMINAL *tp, short color,
	short *r, short *g, short *b)
{
	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * bail if invalid color value
	 */
	if (color < 0 || color >= NUM_COLORS)
		return (-1);

	/*--------------------------------------------------------------------
	 * return RGB values
	 */
	*r = tcap_rgb[color].red;
	*g = tcap_rgb[color].green;
	*b = tcap_rgb[color].blue;

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_color_pair() - get the attr_t value for a given color-pair.
 */
attr_t tcap_color_pair (const TERMINAL *tp, int n)
{
	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0)
		return (A_UNSET);

	/*--------------------------------------------------------------------
	 * bail if invalid pair value
	 */
	if (n < 0 || n >= COLOR_PAIRS)
		return (A_UNSET);

	/*--------------------------------------------------------------------
	 * return attr set for given pair
	 */
	return (tp->tcap->clr_pairs[n]);
}
