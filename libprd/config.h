/*------------------------------------------------------------------------
 * Header for config routines
 */
#ifndef CONFIG_H
#define CONFIG_H

/*------------------------------------------------------------------------
 * menu entry types
 */
#define CFG_HDG				1		/* menu heading */
#define CFG_MAIN			2		/* main entry */
#define CFG_ITEM			3		/* item entry */
#define CFG_BTN				4		/* navigation button */

/*------------------------------------------------------------------------
 * menu types
 */
#define CFG_TYPE_MAIN		1		/* main       menu */
#define CFG_TYPE_SETTINGS	2		/* settings   menu */
#define CFG_TYPE_DISPLAY	3		/* attributes menu */

/*------------------------------------------------------------------------
 * config menu struct
 */
struct cfg_menu
{
	int		type;			/* config entry type							*/

	int		selection;		/* select char or 0 for heading					*/

	int		varptr;			/* offset of option entry in options table		*/
							/* or msgno for heading or main menu			*/

	int		line;			/* line to display on (rel to sub-sub window)	*/
							/* or col of last row for BUTTONS				*/
};
typedef struct cfg_menu CFG_MENU;

/*------------------------------------------------------------------------
 * structure for configuration data
 */
struct cfg_data
{
	const CFG_MENU *	main_menu;		/* main        menu */
	const CFG_MENU **	settings;		/* settings    menu */
	const CFG_MENU **	colors;			/* color attrs menu */
	const CFG_MENU **	monos;			/* mono  attrs menu */
};
typedef struct cfg_data CFG_DATA;

/*------------------------------------------------------------------------
 * config data (found in cfgdata.c)
 */
extern const CFG_DATA *	cfg_data;			/* pointer to config data */

/*------------------------------------------------------------------------
 * config info
 *
 * This data is static because config routines may be called
 * asyncronously via a window resize or attribute changes.
 */
struct config_info
{
	int					main_indent;
	int					main_width;

	int					setting_changes_made;
	int					display_changes_made;
	int					item_changed;

	int					setting_color;
	int					screen_changed;
	int					acs_changed;

	int					menu_type;
	int					main_ent;
	int					page_ent;
	int					page_no;

	const CFG_MENU *	menu;
	char				cfg_fn[MAX_PATHLEN];
};
typedef struct config_info CONFIG_INFO;

/*------------------------------------------------------------------------
 * defaults table (found in dflttbl.c)
 */
extern const DEFT *		dflt_tbl;			/* pointer to defaults table */

#endif /* CONFIG_H */
