/*------------------------------------------------------------------------
 * rgb color list
 */
#ifndef XVTRGB_H
#define XVTRGB_H

/*------------------------------------------------------------------------
 * rgb struct entry
 */
struct rgb_colors
{
	int		red;						/* red   component: 0-255		*/
	int		green;						/* green component: 0-255		*/
	int		blue;						/* blue  component: 0-255		*/
};
typedef struct rgb_colors	RGB_COLORS;

#define RGB_NAME_LEN		24
struct rgb_entry
{
	char		name[RGB_NAME_LEN];		/* color name (w/o white-space)	*/
	RGB_COLORS	colors;					/* RGB colors					*/
};
typedef struct rgb_entry	RGB_ENTRY;

/*------------------------------------------------------------------------
 * rgb functions
 */
extern RGB_ENTRY *	xvt_rgb_list		(void);
extern void			xvt_rgb_free		(RGB_ENTRY *list);
extern void			xvt_rgb_dump		(FILE *fp, int verbose);

extern int			xvt_rgb_check		(XVT_DATA *xd);

#endif /* XVTRGB_H */
