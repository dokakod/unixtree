/*------------------------------------------------------------------------
 * color processing
 */
#ifndef XVTCOLOR_H
#define XVTCOLOR_H

/*------------------------------------------------------------------------
 * color table
 *
 *	This table uses the "standard" mapping of colors to our color table.
 */
struct xvt_color
{
	int				code;
	const char *	name;
};
typedef struct xvt_color	XVT_COLOR;

extern const XVT_COLOR	xvt_color_tbl[];

#define color_code(n)	xvt_color_tbl[(n)].code
#define color_name(n)	xvt_color_tbl[(n)].name

extern int	xvt_color_value	(const char *name);

#endif /* XVTCOLOR_H */
