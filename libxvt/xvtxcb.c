/*------------------------------------------------------------------------
 * X clipboard routines
 */
#include "xvtcommon.h"
#include "xvtx.h"

/*------------------------------------------------------------------------
 * copy data to/from the clipboard
 */
#define PROP_DFLT	"CUT_BUFFER0"
#ifndef PROP_NAME
#define PROP_NAME	PROP_DFLT
#endif

static Atom xvt_w_cb_property (TERMWIN *tw, const char *name)
{
	Atom	property;

	property = XInternAtom(tw->X->display, name, TRUE);
	if (property == None)
		property = XInternAtom(tw->X->display, PROP_DFLT, TRUE);

	return (property);
}

int xvt_w_cb_copy_to (TERMWIN *tw)
{
	XSetSelectionOwner(tw->X->display, XA_PRIMARY, tw->X->win_vt, CurrentTime);
	if (XGetSelectionOwner(tw->X->display, XA_PRIMARY) != tw->X->win_vt)
		return (-1);

	XChangeProperty(tw->X->display, tw->X->win_root, tw->X->cb_text,
		XA_STRING, 8, PropModeReplace,
		tw->selection_ptr, tw->selection_len);

	return (0);
}

static unsigned char * xvt_w_cb_copy_from_atom (TERMWIN *tw,
	Atom property, int *plen)
{
	Window			win;
	long			nread;
	unsigned long	bytes_after;
	unsigned long	nitems;
	Atom			type;
	int				format;
	unsigned char *	prop;
	unsigned char *	data	= 0;
	int				len		= 0;
	int				old_len	= 0;

	win = XGetSelectionOwner(tw->X->display, XA_PRIMARY);
	if (win == None)
		win = tw->X->win_root;

	for (nread=0, bytes_after=1; bytes_after > 0; nread += nitems)
	{
		nitems = 0;
		XGetWindowProperty(tw->X->display, win, property,
			(nread / 4), 4096, FALSE, AnyPropertyType,
			&type, &format, &nitems, &bytes_after, &prop);

		if (nitems == 0)
			break;

		len += (int)nitems;
		data = (unsigned char *)REALLOC(data, len);
		memcpy(data + old_len, prop, nitems);
		old_len = len;

		XFree(prop);
	}

	if (plen != 0)
		*plen = len;

	return (data);
}

static unsigned char * xvt_w_cb_copy_from_name (TERMWIN *tw,
	const char *prop_name, int *plen)
{
	Atom	property = xvt_w_cb_property(tw, prop_name);

	return xvt_w_cb_copy_from_atom(tw, property, plen);
}

unsigned char * xvt_w_cb_copy_from (TERMWIN *tw, int *plen)
{
	return xvt_w_cb_copy_from_name(tw, PROP_NAME, plen);
}
