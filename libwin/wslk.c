/*------------------------------------------------------------------------
 * soft-label routines
 */
#include "wincommon.h"

/*------------------------------------------------------------------------
 * slk_display() - display label data in window
 */
static void slk_display (SLK_DATA *sd)
{
	WINDOW *	win			= sd->win;
	attr_t		save_attr	= wattrget(win);
	int			i;
	int			y;
	int			x;

	/*--------------------------------------------------------------------
	 * clear window to space attribute
	 */
	wattrset(win, sd->sp_attrs);
	werase(win);

	/*--------------------------------------------------------------------
	 * display each label
	 */
	wattrset(win, sd->lb_attrs);

	y = 0;
	x = (sd->num_labels == 12 ? 2 : 1);
	for (i=0; i<sd->num_labels; i++)
	{
		SLK_LABEL *	sl = sd->labels + i;

		/*----------------------------------------------------------------
		 * update line position if 2-line display
		 */
		if (i == sd->lbls_per_line)
		{
			y++;
			x = (sd->num_labels == 12 ? 2 : 1);
		}

		/*----------------------------------------------------------------
		 * display label (note: they are already padded out with spaces)
		 */
		wmove(win, y, x);
		wcaddstr(win, sl->label_disp, sl->label_code);

		/*----------------------------------------------------------------
		 * update label position on line
		 */
		x += (SLK_LABEL_LEN + 1);

		switch (sd->layout_fmt)
		{
		case SLK_LAYOUT_FMT_1_323:
		case SLK_LAYOUT_FMT_2_323:
			/*------------------------------------------------------------
			 * labels are: " xxx xxx xxx  xxx xxx  xxx xxx xxx "
			 */
			switch (i)
			{
			case 2:
			case 4:
			case 10:
			case 12:
				x += 2;
				break;
			}
			break;

		case SLK_LAYOUT_FMT_1_44:
		case SLK_LAYOUT_FMT_2_44:
			/*------------------------------------------------------------
			 * labels are: " xxx xxx xxx xxx  xxx xxx xxx xxx "
			 */
			switch (i)
			{
			case 3:
			case 11:
				x += 2;
				break;
			}
			break;

		case SLK_LAYOUT_FMT_2_33:
			/*------------------------------------------------------------
			 * labels are: "  xxx  xxx  xxx   xxx  xxx  xxx "
			 */
			x += 2;
			switch (i)
			{
			case 2:
			case 8:
				x += 2;
				break;
			}
			break;

		case SLK_LAYOUT_FMT_2_222:
			/*------------------------------------------------------------
			 * labels are: "  xxx  xxx   xxx  xxx   xxx  xxx "
			 */
			x += 2;
			switch (i)
			{
			case 1:
			case 3:
			case 7:
			case 9:
				x += 2;
				break;
			}
			break;
		}
	}

	/*--------------------------------------------------------------------
	 * done
	 */
	wattrset(win, save_attr);
}

/*------------------------------------------------------------------------
 * scrn_slk_setup() - initialize the slk window
 */
int scrn_slk_setup (SCREEN *s)
{
	SLK_DATA *	sd;

	/*--------------------------------------------------------------------
	 * get slk data pointer
	 */
	sd	= SCR_SLKDATA(s);

	/*--------------------------------------------------------------------
	 * check if attributes have been set
	 */
	if (sd->lb_attrs == A_UNSET)
		sd->lb_attrs = wstandget(sd->win);

	if (sd->sp_attrs == A_UNSET)
		sd->sp_attrs = wattrget(sd->win);

	/*--------------------------------------------------------------------
	 * setup initial display
	 */
	scrn_slk_noutrefresh(s);

	return (OK);
}

/*------------------------------------------------------------------------
 * slk_init() - cache slk layout format
 */
int slk_init (int layout_fmt)
{
	/*--------------------------------------------------------------------
	 * check layout_fmt value
	 */
	switch (layout_fmt)
	{
	case SLK_LAYOUT_FMT_NONE:
	case SLK_LAYOUT_FMT_1_323:
	case SLK_LAYOUT_FMT_1_44:
	case SLK_LAYOUT_FMT_2_323:
	case SLK_LAYOUT_FMT_2_44:
	case SLK_LAYOUT_FMT_2_33:
	case SLK_LAYOUT_FMT_2_222:
		win_slk_fmt = layout_fmt;
		break;

	default:
		return (ERR);
	}

	return (OK);
}

/*------------------------------------------------------------------------
 * scrn_slk_init() - initialize soft-labels
 */
int scrn_slk_init (SCREEN *s, int layout_fmt)
{
	SLK_DATA *	sd;
	int			i;
	int			n;
	int			p;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (s == 0)
		return (ERR);
	sd = SCR_SLKDATA(s);

	/*--------------------------------------------------------------------
	 * check layout_fmt value
	 */
	switch (layout_fmt)
	{
	case SLK_LAYOUT_FMT_NONE:
		sd->layout_fmt = layout_fmt;
		return (OK);

	case SLK_LAYOUT_FMT_1_323:
	case SLK_LAYOUT_FMT_1_44:
		n = 8;
		p = 8;
		break;

	case SLK_LAYOUT_FMT_2_323:
	case SLK_LAYOUT_FMT_2_44:
		n = 16;
		p = 8;
		break;

	case SLK_LAYOUT_FMT_2_33:
	case SLK_LAYOUT_FMT_2_222:
		n = 12;
		p = 6;
		break;

	default:
		return (ERR);
	}

	/*--------------------------------------------------------------------
	 * initialize table
	 */
	sd->layout_fmt		= layout_fmt;
	sd->num_labels		= n;
	sd->lbls_per_line	= p;
	sd->lb_attrs		= A_UNSET;
	sd->sp_attrs		= A_UNSET;

	for (i = 0;
		i < sizeof(sd->labels) / sizeof(*sd->labels);
		i++)
	{
		SLK_LABEL *	sl = sd->labels + i;
		int			j;

		sl->label_fmt		= SLK_LABEL_FMT_LEFT;
		sl->label_code		= 0;
		sl->label_text[0]	= 0;

		for (j=0; j<SLK_LABEL_LEN; j++)
			sl->label_disp[j] = ' ';
		sl->label_disp[j] = 0;
	}

	return (OK);
}

/*------------------------------------------------------------------------
 * slk_set() - set label text for an entry
 */
int scrn_slk_set (SCREEN *s, int labnum, const char *label, int label_fmt)
{
	return scrn_slk_set_code(s, labnum, label, label_fmt, 0);
}

/*------------------------------------------------------------------------
 * slk_set_code() - set label text & code for an entry
 */
int scrn_slk_set_code (SCREEN *s, int labnum, const char *label,
	int label_fmt, int code)
{
	SLK_DATA *	sd;
	SLK_LABEL *	sl;
	int			label_len;
	int			label_start;
	int			i;
	int			j;

	/*--------------------------------------------------------------------
	 * validate args
	 */
	if (s == 0)
		return (ERR);
	sd = SCR_SLKDATA(s);

	if (sd->win == 0)
		return (ERR);

	if (labnum <= 0 || labnum > sd->num_labels)
		return (ERR);

	if (label == 0)
		label = "";

	label_len = strlen(label);
	if (label_len > SLK_LABEL_LEN)
		return (ERR);

	switch (label_fmt)
	{
	case SLK_LABEL_FMT_LEFT:
		label_start = 0;
		break;

	case SLK_LABEL_FMT_CENTER:
		label_start = (SLK_LABEL_LEN - label_len) / 2;
		break;

	case SLK_LABEL_FMT_RIGHT:
		label_start = (SLK_LABEL_LEN - label_len);
		break;

	default:
		return (ERR);
	}

	sl = sd->labels + (labnum - 1);

	/*--------------------------------------------------------------------
	 * store label data
	 */
	sl->label_fmt	= label_fmt;
	sl->label_code	= code;

	strcpy(sl->label_text, label);

	for (i=0; i<label_start; i++)
		sl->label_disp[i] = ' ';
	for (j=0; label[j]; j++)
		sl->label_disp[i++] = label[j];
	for (; i<SLK_LABEL_LEN; i++)
		sl->label_disp[i] = ' ';
	sl->label_disp[i] = 0;

	return (OK);
}

/*------------------------------------------------------------------------
 * slk_label() - get the label for an entry
 */
char * scrn_slk_label (SCREEN *s, int labnum)
{
	SLK_DATA *	sd;

	if (s == 0)
		return (0);
	sd = SCR_SLKDATA(s);

	if (sd->win == 0)
		return (0);

	if (labnum <= 0 || labnum > sd->num_labels)
		return (0);

	return (sd->labels[labnum-1].label_text);
}

/*------------------------------------------------------------------------
 * slk_attron() - specify attribute to add
 */
int scrn_slk_attron (SCREEN *s, attr_t attrs)
{
	SLK_DATA *	sd;

	if (s == 0)
		return (ERR);
	sd = SCR_SLKDATA(s);

	if (sd->win == 0)
		return (ERR);

	attrs = A_GETATTR(attrs);
	sd->lb_attrs |= attrs;

	return (OK);
}

/*------------------------------------------------------------------------
 * slk_attroff() - specify attribute to remove
 */
int scrn_slk_attroff (SCREEN *s, attr_t attrs)
{
	SLK_DATA *	sd;

	if (s == 0)
		return (ERR);
	sd = SCR_SLKDATA(s);

	if (sd->win == 0)
		return (ERR);

	attrs = A_GETATTR(attrs);
	sd->lb_attrs &= ~attrs;

	return (OK);
}

/*------------------------------------------------------------------------
 * slk_attrset() - specify attribute for label
 */
int scrn_slk_attrset (SCREEN *s, attr_t attrs)
{
	SLK_DATA *	sd;

	if (s == 0)
		return (ERR);
	sd = SCR_SLKDATA(s);

	if (sd->win == 0)
		return (ERR);

	attrs = A_GETATTR(attrs);
	sd->lb_attrs = attrs;

	return (OK);
}

/*------------------------------------------------------------------------
 * slk_attrbkgd() - specify attribute for background on label line
 */
int scrn_slk_attrbkgd (SCREEN *s, attr_t attrs)
{
	SLK_DATA *	sd;

	if (s == 0)
		return (ERR);
	sd = SCR_SLKDATA(s);

	if (sd->win == 0)
		return (ERR);

	attrs = A_GETATTR(attrs);
	sd->sp_attrs = attrs;

	return (OK);
}

/*------------------------------------------------------------------------
 * slk_refresh() - refresh the label window
 */
int scrn_slk_refresh (SCREEN *s)
{
	SLK_DATA *	sd;
	int			rc;

	if (s == 0)
		return (ERR);
	sd = SCR_SLKDATA(s);

	if (sd->win == 0)
		return (ERR);

	rc = scrn_slk_noutrefresh(s);
	if (rc == OK)
		scrn_doupdate(s);

	return (rc);
}

/*------------------------------------------------------------------------
 * slk_noutrefresh() - refresh the label window with no output
 */
int scrn_slk_noutrefresh (SCREEN *s)
{
	SLK_DATA *	sd;

	if (s == 0)
		return (ERR);
	sd = SCR_SLKDATA(s);

	if (sd->win == 0)
		return (ERR);

	slk_display(sd);
	wnoutrefresh(sd->win);

	return (OK);
}

/*------------------------------------------------------------------------
 * slk_clear() - clear labels from the screen
 */
int scrn_slk_clear (SCREEN *s)
{
	SLK_DATA *	sd;

	if (s == 0)
		return (ERR);
	sd = SCR_SLKDATA(s);

	if (sd->win == 0)
		return (ERR);

	werase(sd->win);
	wrefresh(sd->win);

	return (OK);
}

/*------------------------------------------------------------------------
 * slk_restore() - restore label display on the screen
 */
int scrn_slk_restore (SCREEN *s)
{
	SLK_DATA *	sd;

	if (s == 0)
		return (ERR);
	sd = SCR_SLKDATA(s);

	if (sd->win == 0)
		return (ERR);

	slk_display(sd);
	wrefresh(sd->win);

	return (OK);
}

/*------------------------------------------------------------------------
 * slk_touch() - force a redraw of all labels
 */
int scrn_slk_touch (SCREEN *s)
{
	SLK_DATA *	sd;

	if (s == 0)
		return (ERR);
	sd = SCR_SLKDATA(s);

	if (sd->win == 0)
		return (ERR);

	touchwin(sd->win);

	return (OK);
}
