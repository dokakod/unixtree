/*------------------------------------------------------------------------
 * routines for accessing the XVT_DATA struct
 */
#include "xvtcommon.h"

/*------------------------------------------------------------------------
 * allocate an xvt-data struct
 */
XVT_DATA * xvt_data_alloc (void)
{
	XVT_DATA *	xd;

	xd = (XVT_DATA *)MALLOC(sizeof(*xd));
	if (xd != 0)
	{
		xvt_data_init(xd, FALSE);
	}

	return (xd);
}

/*------------------------------------------------------------------------
 * free an xvt-data struct
 */
XVT_DATA * xvt_data_free (XVT_DATA *xd)
{
	if (xd != 0)
	{
		if (xd->argv != 0)
			FREE(xd->argv);

		FREE(xd);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * initialize an xvt-data struct
 */
void xvt_data_init (XVT_DATA *xd, int set_dflt)
{
	if (xd != 0)
		xvt_tk_data_init(xd, set_dflt);
}

/*------------------------------------------------------------------------
 * allocate a new xvt-data struct & copy data
 */
XVT_DATA * xvt_data_copy (XVT_DATA *xd)
{
	XVT_DATA *	nd	= xvt_data_alloc();
	char *		old_buf;
	char *		buf_end;
	char *		new_buf;

	if (nd != 0 && xd != 0)
	{
		const TK_OPTIONS *	op;

		/*----------------------------------------------------------------
		 * copy over all data
		 */
		memcpy(nd, xd, sizeof(*nd));

		/*----------------------------------------------------------------
		 * clear argc & argv
		 */
		nd->argc = 0;
		nd->argv = 0;

		/*----------------------------------------------------------------
		 * check if any strings point to buffer area & adjust if necessary
		 */
		old_buf	= xd->tkbuf;
		buf_end	= xd->tkbuf + sizeof(xd->tkbuf);
		new_buf	= nd->tkbuf;

		for (op=xvt_tk_options; op->opt_type; op++)
		{
			const char **	pstr;

			switch (op->opt_type)
			{
			case XVT_TK_OPT_STR:
			case XVT_TK_OPT_CLR:
			case XVT_TK_OPT_FNT:
				pstr = (const char **)((char *)nd + op->opt_offset);
				if (*pstr >= old_buf && *pstr < buf_end)
					*pstr = new_buf + (*pstr - old_buf);
				break;
			}
		}
	}

	return (nd);
}

/*------------------------------------------------------------------------
 * routine defines
 */
#define TBLSIZE(tbl)	(sizeof(tbl) / sizeof(*tbl))

#define XVT_OPT_SET_RTN_INT(var_name) \
void xvt_data_set_##var_name (XVT_DATA *xd, int n) \
{ \
	if (xd != 0)	xd->var_name = n; \
}

#define XVT_OPT_SET_RTN_BOOL(var_name) \
void xvt_data_set_##var_name (XVT_DATA *xd, int b) \
{ \
	if (xd != 0)	xd->var_name = b; \
}

#define XVT_OPT_SET_RTN_CHARP(var_name) \
void xvt_data_set_##var_name (XVT_DATA *xd, const char *s) \
{ \
	if (xd != 0)	xd->var_name = s; \
}

#define XVT_OPT_SET_RTN_CHARPP(var_name) \
void xvt_data_set_##var_name (XVT_DATA *xd, const char **s) \
{ \
	if (xd != 0)	xd->var_name = s; \
}

#define XVT_OPT_SET_RTN_CHARP_N(var_name,n) \
void xvt_data_set_##var_name (XVT_DATA *xd, const char *s, int n) \
{ \
	if (xd != 0 && n >= 0 && n < TBLSIZE(xd->var_name)) \
		xd->var_name[n] = s; \
}

#define XVT_OPT_SET_RTN_FILEP(var_name) \
void xvt_data_set_##var_name (XVT_DATA *xd, FILE *fp) \
{ \
	if (xd != 0)	xd->var_name = fp; \
}

#define XVT_OPT_SET_RTN_PATH(var_name) \
void xvt_data_set_##var_name (XVT_DATA *xd, const char *p) \
{ \
	if (xd != 0 && p != 0 && strlen(p) < XVT_MAX_PATHLEN)	\
		strcpy(xd->var_name, p); \
}

#define XVT_OPT_GET_RTN_INT(var_name) \
int xvt_data_get_##var_name (XVT_DATA *xd) \
{ \
	return (xd == 0 ? -1 : xd->var_name); \
}

#define XVT_OPT_GET_RTN_BOOL(var_name) \
int xvt_data_get_##var_name (XVT_DATA *xd) \
{ \
	return (xd == 0 ? -1 : xd->var_name); \
}

#define XVT_OPT_GET_RTN_CHARP(var_name) \
const char * xvt_data_get_##var_name (XVT_DATA *xd) \
{ \
	return (xd == 0 ? 0 : xd->var_name); \
}

#define XVT_OPT_GET_RTN_CHARP_N(var_name,n) \
const char * xvt_data_get_##var_name (XVT_DATA *xd, int n) \
{ \
	return (xd == 0 || n < 0 || n >= TBLSIZE(xd->var_name) ? \
		0 : xd->var_name[n]); \
}

#define XVT_OPT_GET_RTN_CHARPP(var_name) \
char ** xvt_data_get_##var_name (XVT_DATA *xd) \
{ \
	return (xd == 0 ? 0 : xd->var_name); \
}

#define XVT_OPT_GET_RTN_FILEP(var_name) \
FILE * xvt_data_get_##var_name (XVT_DATA *xd) \
{ \
	return (xd == 0 ? 0 : xd->var_name); \
}

#define XVT_OPT_GET_RTN_PATH(var_name) \
const char * xvt_data_get_##var_name (XVT_DATA *xd) \
{ \
	return (xd == 0 ? 0 : xd->var_name); \
}

#define XVT_OPT_GET_RTN_GENERIC(var_name) \
xvt_data_get_##var_name (XVT_DATA *xd) \
{ \
	return (xd == 0 ? 0 : xd->var_name); \
}

/*------------------------------------------------------------------------
 * set routines (user-supplied data)
 */
XVT_OPT_SET_RTN_CHARP	(appname)

XVT_OPT_SET_RTN_CHARP_N	(reg_fname,n)
XVT_OPT_SET_RTN_CHARP_N	(bld_fname,n)

XVT_OPT_SET_RTN_CHARP	(display)
XVT_OPT_SET_RTN_CHARP	(termname)
XVT_OPT_SET_RTN_CHARP	(resname)
XVT_OPT_SET_RTN_CHARP	(geometry)
XVT_OPT_SET_RTN_CHARP	(icongeom)
XVT_OPT_SET_RTN_CHARP	(termmode)
XVT_OPT_SET_RTN_CHARP	(ico_file)
XVT_OPT_SET_RTN_CHARP	(pointer)
XVT_OPT_SET_RTN_CHARP	(envvar)
XVT_OPT_SET_RTN_CHARP	(win_label)
XVT_OPT_SET_RTN_CHARP	(ico_label)
XVT_OPT_SET_RTN_CHARP	(printer)
XVT_OPT_SET_RTN_CHARP	(resstr)
XVT_OPT_SET_RTN_CHARP	(resfile)
XVT_OPT_SET_RTN_CHARP	(ptyspec)
XVT_OPT_SET_RTN_CHARP	(terminal)

XVT_OPT_SET_RTN_CHARP_N	(scr_color,n)
XVT_OPT_SET_RTN_CHARP	(fg_color)
XVT_OPT_SET_RTN_CHARP	(bg_color)
XVT_OPT_SET_RTN_CHARP	(cr_color)
XVT_OPT_SET_RTN_CHARP	(pr_color)
XVT_OPT_SET_RTN_CHARP	(bd_color)
XVT_OPT_SET_RTN_CHARP	(sb_color)
XVT_OPT_SET_RTN_CHARP	(st_color)

XVT_OPT_SET_RTN_INT		(scr_cols)
XVT_OPT_SET_RTN_INT		(scr_rows)
XVT_OPT_SET_RTN_INT		(scr_tabs)
XVT_OPT_SET_RTN_INT		(border)
XVT_OPT_SET_RTN_INT		(margin)
XVT_OPT_SET_RTN_INT		(mbcols)
XVT_OPT_SET_RTN_INT		(fontno)
XVT_OPT_SET_RTN_INT		(sblines)

XVT_OPT_SET_RTN_BOOL	(iconic)
XVT_OPT_SET_RTN_BOOL	(monodisp)
XVT_OPT_SET_RTN_BOOL	(login)
XVT_OPT_SET_RTN_BOOL	(ahcursor)
XVT_OPT_SET_RTN_BOOL	(reverse)
XVT_OPT_SET_RTN_BOOL	(visual)
XVT_OPT_SET_RTN_BOOL	(autowrap)
XVT_OPT_SET_RTN_BOOL	(revwrap)
XVT_OPT_SET_RTN_BOOL	(mbell)
XVT_OPT_SET_RTN_BOOL	(quiet)
XVT_OPT_SET_RTN_BOOL	(ignptr)
XVT_OPT_SET_RTN_BOOL	(console)
XVT_OPT_SET_RTN_BOOL	(lblinfo)
XVT_OPT_SET_RTN_BOOL	(kpfont)
XVT_OPT_SET_RTN_BOOL	(mapoutput)
XVT_OPT_SET_RTN_BOOL	(mapalert)
XVT_OPT_SET_RTN_BOOL	(a132)
XVT_OPT_SET_RTN_BOOL	(c132)
XVT_OPT_SET_RTN_BOOL	(xerrors)
XVT_OPT_SET_RTN_BOOL	(prtkeep)
XVT_OPT_SET_RTN_BOOL	(cufix)
XVT_OPT_SET_RTN_BOOL	(acticon)
XVT_OPT_SET_RTN_BOOL	(sb_enable)
XVT_OPT_SET_RTN_BOOL	(sb_right)
XVT_OPT_SET_RTN_BOOL	(sb_shadow)
XVT_OPT_SET_RTN_BOOL	(sb_scrout)
XVT_OPT_SET_RTN_BOOL	(sb_scrkey)

XVT_OPT_SET_RTN_FILEP	(events_fp)
XVT_OPT_SET_RTN_FILEP	(action_fp)
XVT_OPT_SET_RTN_FILEP	(output_fp)
XVT_OPT_SET_RTN_FILEP	(screen_fp)
XVT_OPT_SET_RTN_FILEP	(inpkbd_fp)
XVT_OPT_SET_RTN_FILEP	(keybrd_fp)
XVT_OPT_SET_RTN_FILEP	(resfil_fp)
XVT_OPT_SET_RTN_FILEP	(tkopts_fp)

XVT_OPT_SET_RTN_PATH	(events_path)
XVT_OPT_SET_RTN_PATH	(action_path)
XVT_OPT_SET_RTN_PATH	(output_path)
XVT_OPT_SET_RTN_PATH	(screen_path)
XVT_OPT_SET_RTN_PATH	(inpkbd_path)
XVT_OPT_SET_RTN_PATH	(keybrd_path)
XVT_OPT_SET_RTN_PATH	(resfil_path)
XVT_OPT_SET_RTN_PATH	(tkopts_path)

XVT_OPT_SET_RTN_CHARPP	(resfiles)
XVT_OPT_SET_RTN_CHARPP	(resdirs)
XVT_OPT_SET_RTN_CHARPP	(respaths)

/*------------------------------------------------------------------------
 * get routines (user-supplied data)
 */
XVT_OPT_GET_RTN_CHARP	(appname)

XVT_OPT_GET_RTN_CHARP_N	(reg_fname,n)
XVT_OPT_GET_RTN_CHARP_N	(bld_fname,n)

XVT_OPT_GET_RTN_CHARP	(display)
XVT_OPT_GET_RTN_CHARP	(termname)
XVT_OPT_GET_RTN_CHARP	(resname)
XVT_OPT_GET_RTN_CHARP	(geometry)
XVT_OPT_GET_RTN_CHARP	(icongeom)
XVT_OPT_GET_RTN_CHARP	(termmode)
XVT_OPT_GET_RTN_CHARP	(ico_file)
XVT_OPT_GET_RTN_CHARP	(pointer)
XVT_OPT_GET_RTN_CHARP	(envvar)
XVT_OPT_GET_RTN_CHARP	(win_label)
XVT_OPT_GET_RTN_CHARP	(ico_label)
XVT_OPT_GET_RTN_CHARP	(printer)
XVT_OPT_GET_RTN_CHARP	(resstr)
XVT_OPT_GET_RTN_CHARP	(resfile)
XVT_OPT_GET_RTN_CHARP	(ptyspec)
XVT_OPT_GET_RTN_CHARP	(terminal)

XVT_OPT_GET_RTN_CHARP_N	(scr_color,n)
XVT_OPT_GET_RTN_CHARP	(fg_color)
XVT_OPT_GET_RTN_CHARP	(bg_color)
XVT_OPT_GET_RTN_CHARP	(cr_color)
XVT_OPT_GET_RTN_CHARP	(pr_color)
XVT_OPT_GET_RTN_CHARP	(bd_color)
XVT_OPT_GET_RTN_CHARP	(sb_color)
XVT_OPT_GET_RTN_CHARP	(st_color)

XVT_OPT_GET_RTN_INT		(scr_cols)
XVT_OPT_GET_RTN_INT		(scr_rows)
XVT_OPT_GET_RTN_INT		(scr_tabs)
XVT_OPT_GET_RTN_INT		(border)
XVT_OPT_GET_RTN_INT		(margin)
XVT_OPT_GET_RTN_INT		(mbcols)
XVT_OPT_GET_RTN_INT		(fontno)
XVT_OPT_GET_RTN_INT		(sblines)

XVT_OPT_GET_RTN_BOOL	(iconic)
XVT_OPT_GET_RTN_BOOL	(monodisp)
XVT_OPT_GET_RTN_BOOL	(login)
XVT_OPT_GET_RTN_BOOL	(ahcursor)
XVT_OPT_GET_RTN_BOOL	(reverse)
XVT_OPT_GET_RTN_BOOL	(visual)
XVT_OPT_GET_RTN_BOOL	(autowrap)
XVT_OPT_GET_RTN_BOOL	(revwrap)
XVT_OPT_GET_RTN_BOOL	(mbell)
XVT_OPT_GET_RTN_BOOL	(quiet)
XVT_OPT_GET_RTN_BOOL	(ignptr)
XVT_OPT_GET_RTN_BOOL	(console)
XVT_OPT_GET_RTN_BOOL	(lblinfo)
XVT_OPT_GET_RTN_BOOL	(kpfont)
XVT_OPT_GET_RTN_BOOL	(mapoutput)
XVT_OPT_GET_RTN_BOOL	(mapalert)
XVT_OPT_GET_RTN_BOOL	(a132)
XVT_OPT_GET_RTN_BOOL	(c132)
XVT_OPT_GET_RTN_BOOL	(xerrors)
XVT_OPT_GET_RTN_BOOL	(prtkeep)
XVT_OPT_GET_RTN_BOOL	(cufix)
XVT_OPT_GET_RTN_BOOL	(acticon)
XVT_OPT_GET_RTN_BOOL	(sb_enable)
XVT_OPT_GET_RTN_BOOL	(sb_right)
XVT_OPT_GET_RTN_BOOL	(sb_shadow)
XVT_OPT_GET_RTN_BOOL	(sb_scrout)
XVT_OPT_GET_RTN_BOOL	(sb_scrkey)

XVT_OPT_GET_RTN_FILEP	(events_fp)
XVT_OPT_GET_RTN_FILEP	(action_fp)
XVT_OPT_GET_RTN_FILEP	(output_fp)
XVT_OPT_GET_RTN_FILEP	(screen_fp)
XVT_OPT_GET_RTN_FILEP	(inpkbd_fp)
XVT_OPT_GET_RTN_FILEP	(keybrd_fp)
XVT_OPT_GET_RTN_FILEP	(resfil_fp)
XVT_OPT_GET_RTN_FILEP	(tkopts_fp)

XVT_OPT_GET_RTN_PATH	(events_path)
XVT_OPT_GET_RTN_PATH	(action_path)
XVT_OPT_GET_RTN_PATH	(output_path)
XVT_OPT_GET_RTN_PATH	(screen_path)
XVT_OPT_GET_RTN_PATH	(inpkbd_path)
XVT_OPT_GET_RTN_PATH	(keybrd_path)
XVT_OPT_GET_RTN_PATH	(resfil_path)
XVT_OPT_GET_RTN_PATH	(tkopts_path)

const
XVT_OPT_GET_RTN_CHARPP	(resfiles)
const
XVT_OPT_GET_RTN_CHARPP	(resdirs)
const
XVT_OPT_GET_RTN_CHARPP	(respaths)

/*------------------------------------------------------------------------
 * get routines (generated data)
 */
XVT_OPT_GET_RTN_INT		(argc)
XVT_OPT_GET_RTN_CHARPP	(argv)
XVT_OPT_GET_RTN_CHARP	(term_type)
const
XVT_OPT_GET_RTN_CHARPP	(term_defs)
XVT_OPT_GET_RTN_INT		(fd_inp)
XVT_OPT_GET_RTN_INT		(fd_out)

XVT_EVENT_RTN *
XVT_OPT_GET_RTN_GENERIC	(event_rtn)

void *
XVT_OPT_GET_RTN_GENERIC	(event_data)

XVT_OPT_GET_RTN_INT		(cur_cols)
XVT_OPT_GET_RTN_INT		(cur_rows)
XVT_OPT_GET_RTN_INT		(exit_code)
XVT_OPT_GET_RTN_INT		(error_num)
XVT_OPT_GET_RTN_INT		(error_quit)
XVT_OPT_GET_RTN_CHARP	(error_msg)
