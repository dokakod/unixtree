/*------------------------------------------------------------------------
 * X toolkit options table
 */
#include "xvtcommon.h"

/*------------------------------------------------------------------------
 * extra help lines
 */
static const char * xvt_tk_debug_lines[] =
{
	"\"" XVT_D_OPT_EVENTS "\" (txt: all events)",
	"\"" XVT_D_OPT_ACTION "\" (txt: X11 events)",
	"\"" XVT_D_OPT_OUTPUT "\" (txt: child output)",
	"\"" XVT_D_OPT_SCREEN "\" (bin: child output)",
	"\"" XVT_D_OPT_INPKBD "\" (txt: child input)",
	"\"" XVT_D_OPT_KEYBRD "\" (bin: child input)",
	"\"" XVT_D_OPT_RESFIL "\" (resource entries)",
	"\"" XVT_D_OPT_TKOPTS "\" (options dump)",
	"<type>:<name> for file",
	"<type>:-      for stderr",

	0
};

/*------------------------------------------------------------------------
 * toolkit option table
 */
#define	TK_OFFSET_OF(t,f)	( (int)&(((t *)0)->f) )
#define O(field)			TK_OFFSET_OF(XVT_DATA, field)

const TK_OPTIONS	xvt_tk_options[] =
{
	/*--------------------------------------------------------------------
	 * string options
	 */
	{	XVT_TK_OPT_STR,		O(display),			TK_FLAG_ENVNAME,
		"display",			"-display string",	"X server to contact",
		0,					XVT_S_OPT_DISPLAY },

	{	XVT_TK_OPT_STR,		O(termname),		0,
		"tn",				"-tn name",			"value for TERM env var",
		"termName",			XVT_S_OPT_TERMNAME },

	{	XVT_TK_OPT_STR,		O(resname),			TK_FLAG_APPOVR,
		"name",				"-name string",		"client name",
		0,					XVT_S_OPT_RESNAME },

	{	XVT_TK_OPT_STR,		O(geometry),		0,
		"geometry",			"-geometry geom",	"size (in chars) & position",
		"geometry",			XVT_S_OPT_GEOMETRY },

	{	XVT_TK_OPT_STR,		O(icongeom),		0,
		"icongeom",			"-icongeom geom",	"icon geometry (pos only)",
		"iconGeometry",		XVT_S_OPT_ICONGEOM },

	{	XVT_TK_OPT_STR,		O(termmode),		0,
		"tm",				"-tm opts",			"stty settings for window",
		"ttyModes",			XVT_S_OPT_TERMMODE },

	{	XVT_TK_OPT_STR,		O(ico_file),		0,
		"iconfile",			"-iconfile path",	"icon file to use",
		"iconFile",			XVT_S_OPT_ICONFILE },

	{	XVT_TK_OPT_STR,		O(pointer),			0,
		"pointer",			"-pointer name",	"pointer type",
		"pointerShape",		XVT_S_OPT_POINTER },

	{	XVT_TK_OPT_STR,		O(envvar),			0,
		"envvar",			"-envvar name",		"env var name for term-type",
		"envVar",			XVT_S_OPT_ENVVAR },

	{	XVT_TK_OPT_STR,		O(win_label),		TK_FLAG_APPOVR |
												TK_FLAG_PHONYDFLT,
		"title",			"-title string",	"window label",
		"title",			XVT_S_OPT_TITLE },

	{	XVT_TK_OPT_STR,		O(ico_label),		TK_FLAG_APPOVR |
												TK_FLAG_PHONYDFLT,
		"ititle",			"-ititle string",	"icon label",
		"iconName",			XVT_S_OPT_ITITLE },

	{	XVT_TK_OPT_STR,		O(printer),			TK_FLAG_PHONYDFLT,
		"printer",			"-printer spec",	"name, |cmd, >file, >>file",
		"printer",			XVT_S_OPT_PRINTER },

	{	XVT_TK_OPT_STR,		O(resstr),			0,
		"xrm",				"-xrm name",		"specify resource entry",
		0,					XVT_S_OPT_RESSTR },

	{	XVT_TK_OPT_STR,		O(resfile),			0,
		"xrf",				"-xrf path",		"alternate resource file",
		0,					XVT_S_OPT_RESFILE },

	{	XVT_TK_OPT_STR,		O(ptyspec),			TK_FLAG_NOTHERE |
												TK_FLAG_NOHELP_0,
		"pty",				"-pty xxd|c.d",		"pty specification",
		0,					XVT_S_OPT_PTYSPEC },

	{	XVT_TK_OPT_STR,		O(terminal),		TK_FLAG_NOHELP_0,
		"terminal",			"-terminal type",	"terminal type",
		"terminalType",		XVT_S_OPT_TERMINAL },

	/*--------------------------------------------------------------------
	 * font options
	 */
	{	XVT_TK_OPT_FNT,		O(reg_fname[0]),	0,
		"fn",				"-fn fontname",		"normal text font",
		"font",				XVT_F_OPT_REGFONT0 },

	{	XVT_TK_OPT_FNT,		O(reg_fname[1]),	TK_FLAG_NOHELP_0,
		"fn1",				"-fn1 fontname",	"normal text font 1",
		"font1",			XVT_F_OPT_REGFONT1 },

	{	XVT_TK_OPT_FNT,		O(reg_fname[2]),	TK_FLAG_NOHELP_0,
		"fn2",				"-fn2 fontname",	"normal text font 2",
		"font2",			XVT_F_OPT_REGFONT2 },

	{	XVT_TK_OPT_FNT,		O(reg_fname[3]),	TK_FLAG_NOHELP_0,
		"fn3",				"-fn3 fontname",	"normal text font 3",
		"font3",			XVT_F_OPT_REGFONT3 },

	{	XVT_TK_OPT_FNT,		O(reg_fname[4]),	TK_FLAG_NOHELP_0,
		"fn4",				"-fn4 fontname",	"normal text font 4",
		"font4",			XVT_F_OPT_REGFONT4 },

	{	XVT_TK_OPT_FNT,		O(reg_fname[5]),	TK_FLAG_NOHELP_0,
		"fn5",				"-fn5 fontname",	"normal text font 5",
		"font5",			XVT_F_OPT_REGFONT5 },

	{	XVT_TK_OPT_FNT,		O(reg_fname[6]),	TK_FLAG_NOHELP_0,
		"fn6",				"-fn6 fontname",	"normal text font 6",
		"font6",			XVT_F_OPT_REGFONT6 },

	{	XVT_TK_OPT_FNT,		O(reg_fname[7]),	TK_FLAG_NOHELP_0,
		"fn7",				"-fn7 fontname",	"normal text font 7",
		"font7",			XVT_F_OPT_REGFONT7 },

	{	XVT_TK_OPT_FNT,		O(bld_fname[0]),	0,
		"fb",				"-fb fontname",		"bold text font",
		"boldFont",			XVT_F_OPT_BLDFONT0 },

	{	XVT_TK_OPT_FNT,		O(bld_fname[1]),	TK_FLAG_NOHELP_0,
		"fb1",				"-fb1 fontname",	"bold text font 1",
		"boldFont1",		XVT_F_OPT_BLDFONT1 },

	{	XVT_TK_OPT_FNT,		O(bld_fname[2]),	TK_FLAG_NOHELP_0,
		"fb2",				"-fb2 fontname",	"bold text font 2",
		"boldFont2",		XVT_F_OPT_BLDFONT2 },

	{	XVT_TK_OPT_FNT,		O(bld_fname[3]),	TK_FLAG_NOHELP_0,
		"fb3",				"-fb3 fontname",	"bold text font 3",
		"boldFont3",		XVT_F_OPT_BLDFONT3 },

	{	XVT_TK_OPT_FNT,		O(bld_fname[4]),	TK_FLAG_NOHELP_0,
		"fb4",				"-fb4 fontname",	"bold text font 4",
		"boldFont4",		XVT_F_OPT_BLDFONT4 },

	{	XVT_TK_OPT_FNT,		O(bld_fname[5]),	TK_FLAG_NOHELP_0,
		"fb5",				"-fb5 fontname",	"bold text font 5",
		"boldFont5",		XVT_F_OPT_BLDFONT5 },

	{	XVT_TK_OPT_FNT,		O(bld_fname[6]),	TK_FLAG_NOHELP_0,
		"fb6",				"-fb6 fontname",	"bold text font 6",
		"boldFont6",		XVT_F_OPT_BLDFONT6 },

	{	XVT_TK_OPT_FNT,		O(bld_fname[7]),	TK_FLAG_NOHELP_0,
		"fb7",				"-fb7 fontname",	"bold text font 7",
		"boldFont7",		XVT_F_OPT_BLDFONT7 },

	/*--------------------------------------------------------------------
	 * color options
	 */
	{	XVT_TK_OPT_CLR,		O(scr_color[XVT_CLR_BLACK]),	TK_FLAG_NOHELP_0,
		"black",			"-black color",		"color for  0 (black)",
		"color0",			XVT_C_OPT_BLACK },

	{	XVT_TK_OPT_CLR,		O(scr_color[XVT_CLR_RED]),		TK_FLAG_NOHELP_0,
		"red",				"-red color",		"color for  1 (red)",
		"color1",			XVT_C_OPT_RED },

	{	XVT_TK_OPT_CLR,		O(scr_color[XVT_CLR_GREEN]),	TK_FLAG_NOHELP_0,
		"green",			"-green color",		"color for  2 (green)",
		"color2",			XVT_C_OPT_GREEN },

	{	XVT_TK_OPT_CLR,		O(scr_color[XVT_CLR_YELLOW]),	TK_FLAG_NOHELP_0,
		"yellow",			"-yellow color",	"color for  3 (yellow)",
		"color3",			XVT_C_OPT_YELLOW },

	{	XVT_TK_OPT_CLR,		O(scr_color[XVT_CLR_BLUE]),		TK_FLAG_NOHELP_0,
		"blue",				"-blue color",		"color for  4 (blue)",
		"color4",			XVT_C_OPT_BLUE },

	{	XVT_TK_OPT_CLR,		O(scr_color[XVT_CLR_MAGENTA]),	TK_FLAG_NOHELP_0,
		"magenta",			"-magenta color",	"color for  5 (magenta)",
		"color5",			XVT_C_OPT_MAGENTA },

	{	XVT_TK_OPT_CLR,		O(scr_color[XVT_CLR_CYAN]),		TK_FLAG_NOHELP_0,
		"cyan",				"-cyan color",		"color for  6 (cyan)",
		"color6",			XVT_C_OPT_CYAN },

	{	XVT_TK_OPT_CLR,		O(scr_color[XVT_CLR_WHITE]),	TK_FLAG_NOHELP_0,
		"white",			"-white color",		"color for  7 (white)",
		"color7",			XVT_C_OPT_WHITE },

	{	XVT_TK_OPT_CLR,		O(scr_color[XVT_CLR_GREY]),		TK_FLAG_NOHELP_0,
		"grey",				"-grey color",		"color for  8 (grey)",
		"color8",			XVT_C_OPT_GREY },

	{	XVT_TK_OPT_CLR,		O(scr_color[XVT_CLR_LTRED]),	TK_FLAG_NOHELP_0,
		"ltred",			"-ltred color",		"color for  9 (lt-red)",
		"color9",			XVT_C_OPT_LTRED },

	{	XVT_TK_OPT_CLR,		O(scr_color[XVT_CLR_LTGREEN]),	TK_FLAG_NOHELP_0,
		"ltgreen",			"-ltgreen color",	"color for 10 (lt-green)",
		"color10",			XVT_C_OPT_LTGREEN },

	{	XVT_TK_OPT_CLR,		O(scr_color[XVT_CLR_LTYELLOW]),	TK_FLAG_NOHELP_0,
		"ltyellow",			"-ltyellow color",	"color for 11 (lt-yellow)",
		"color11",			XVT_C_OPT_LTYELLOW },

	{	XVT_TK_OPT_CLR,		O(scr_color[XVT_CLR_LTBLUE]),	TK_FLAG_NOHELP_0,
		"ltblue",			"-ltblue color",	"color for 12 (lt-blue)",
		"color12",			XVT_C_OPT_LTBLUE },

	{	XVT_TK_OPT_CLR,		O(scr_color[XVT_CLR_LTMAGENTA]),TK_FLAG_NOHELP_0,
		"ltmagenta",		"-ltmagenta color",	"color for 13 (lt-magenta)",
		"color13",			XVT_C_OPT_LTMAGENTA },

	{	XVT_TK_OPT_CLR,		O(scr_color[XVT_CLR_LTCYAN]),	TK_FLAG_NOHELP_0,
		"ltcyan",			"-ltcyan color",	"color for 14 (lt-cyan)",
		"color14",			XVT_C_OPT_LTCYAN },

	{	XVT_TK_OPT_CLR,		O(scr_color[XVT_CLR_LTWHITE]),	TK_FLAG_NOHELP_0,
		"ltwhite",			"-ltwhite color",	"color for 15 (lt-white)",
		"color15",			XVT_C_OPT_LTWHITE },

	{	XVT_TK_OPT_CLR,		O(fg_color),		0,
		"fg",				"-fg color",		"color for foreground",
		"foreground",		XVT_C_OPT_FG },

	{	XVT_TK_OPT_CLR,		O(bg_color),		0,
		"bg",				"-bg color",		"color for background",
		"background",		XVT_C_OPT_BG },

	{	XVT_TK_OPT_CLR,		O(pr_color),		0,
		"pr",				"-pr color",		"color for pointer",
		"pointerColor",		XVT_C_OPT_POINTER },

	{	XVT_TK_OPT_CLR,		O(bd_color),		0,
		"bd",				"-bd color",		"color for border",
		"borderColor",		XVT_C_OPT_BORDER },

	{	XVT_TK_OPT_CLR,		O(cr_color),		0,
		"cr",				"-cr color",		"color for text cursor",
		"cursorColor",		XVT_C_OPT_CURSOR },

	{	XVT_TK_OPT_CLR,		O(sb_color),		TK_FLAG_NOTINHELP,
		"sbc",				"-sbc color",		"color for scrollbar",
		"scrollbarColor",	XVT_C_OPT_SB },

	{	XVT_TK_OPT_CLR,		O(st_color),		TK_FLAG_NOTINHELP,
		"stc",				"-stc color",		"color for scrollbar trough",
		"troughColor",		XVT_C_OPT_ST },

	/*--------------------------------------------------------------------
	 * numeric options
	 */
	{	XVT_TK_OPT_NUM,		O(scr_cols),		0,
		"width",			"-width num",		"number of columns",
		"width",			(char *)XVT_N_OPT_COLS },

	{	XVT_TK_OPT_NUM,		O(scr_rows),		0,
		"height",			"-height num",		"number of rows",
		"height",			(char *)XVT_N_OPT_ROWS },

	{	XVT_TK_OPT_NUM,		O(scr_tabs),		0,
		"tw",				"-tw num",			"tab width",
		"tabWidth",			(char *)XVT_N_OPT_TABWIDTH },

	{	XVT_TK_OPT_NUM,		O(border),			0,
		"bw",				"-bw num",			"border width",
		"borderWidth",		(char *)XVT_N_OPT_BORDER },

	{	XVT_TK_OPT_NUM,		O(margin),			0,
		"mw",				"-mw num",			"margin width",
		"marginWidth",		(char *)XVT_N_OPT_MARGIN },

	{	XVT_TK_OPT_NUM,		O(mbcols),			0,
		"nb",				"-nb num",			"margin bell cols from right",
		"nMarginBell",		(char *)XVT_N_OPT_MBCOLS },

	{	XVT_TK_OPT_NUM,		O(fontno),			0,
		"fontno",			"-fontno num",		"font number",
		"fontNum",			(char *)XVT_N_OPT_FONTNO },

	{	XVT_TK_OPT_NUM,		O(sblines),			TK_FLAG_NOTINHELP,
		"sl",				"-sl num",			"number of lines to save",
		"saveLines",		(char *)XVT_N_OPT_SBLINES },

	/*--------------------------------------------------------------------
	 * boolean options
	 */
	{	XVT_TK_OPT_BLN,		O(iconic),			0,
		"iconic",			"-/+iconic",		"start iconic",
		"iconStartup",		(char *)XVT_B_OPT_ICONIC },

	{	XVT_TK_OPT_BLN,		O(monodisp),		0,
		"mono",				"-/+mono",			"mono display",
		"monoDisplay",		(char *)XVT_B_OPT_MONO },

	{	XVT_TK_OPT_BLN,		O(login),			TK_FLAG_NOTHERE,
		"ls",				"-/+ls",			"login shell",
		"loginShell",		(char *)XVT_B_OPT_LOGIN },

	{	XVT_TK_OPT_BLN,		O(ahcursor),		0,
		"ah",				"-/+ah",			"always highlight cursor",
		"alwaysHighlight",	(char *)XVT_B_OPT_HILIGHT },

	{	XVT_TK_OPT_BLN,		O(reverse),			0,
		"rv",				"-/+rv",			"reverse video",
		"reverseVideo",		(char *)XVT_B_OPT_REVVID },

	{	XVT_TK_OPT_BLN,		O(visual),			0,
		"vb",				"-/+vb",			"visual bell",
		"visualBell",		(char *)XVT_B_OPT_VISBELL },

	{	XVT_TK_OPT_BLN,		O(autowrap),		0,
		"aw",				"-/+aw",			"auto wraparound",
		"autoWrap",			(char *)XVT_B_OPT_AUTOWRAP },

	{	XVT_TK_OPT_BLN,		O(revwrap),			0,
		"rw",				"-/+rw",			"reverse wraparound",
		"reverseWrap",		(char *)XVT_B_OPT_REVWRAP },

	{	XVT_TK_OPT_BLN,		O(mbell),			0,
		"mb",				"-/+mb",			"margin bell",
		"marginBell",		(char *)XVT_B_OPT_MBELL },

	{	XVT_TK_OPT_BLN,		O(quiet),			0,
		"quiet",			"-/+quiet",			"quiet (^G ignored)",
		"quiet",			(char *)XVT_B_OPT_QUIET },

	{	XVT_TK_OPT_BLN,		O(ignptr),			0,
		"px",				"-/+px",			"ignore pointer",
		"ignorePointer",	(char *)XVT_B_OPT_IGNPTR },

	{	XVT_TK_OPT_BLN,		O(console),			TK_FLAG_NOTHERE,
		"console",			"-/+console",		"intercept console msgs",
		"consoleMode",		(char *)XVT_B_OPT_CONSOLE },

	{	XVT_TK_OPT_BLN,		O(lblinfo),			TK_FLAG_NOHELP_0,
		"li",				"-/+li",			"show debug info in label",
		"labelInfo",		(char *)XVT_B_OPT_LBLINFO },

	{	XVT_TK_OPT_BLN,		O(kpfont),			0,
		"kpfont",			"-/+kpfont",		"KP+/- to change font size",
		"kpFont",			(char *)XVT_B_OPT_KPFONT },

	{	XVT_TK_OPT_BLN,		O(mapoutput),		TK_FLAG_NOHELP_0,
		"mo",				"-/+mo",			"map on output if iconic",
		"mapOnOutput",		(char *)XVT_B_OPT_MAPOUTPUT },

	{	XVT_TK_OPT_BLN,		O(mapalert),		TK_FLAG_NOHELP_0,
		"ma",				"-/+ma",			"map on ^G output if iconic",
		"mapAlert",			(char *)XVT_B_OPT_MAPALERT },

	{	XVT_TK_OPT_BLN,		O(a132),			TK_FLAG_NOHELP_0,
		"a132",				"-/+a132",			"allow 80-132 column switch",
		"allow132",			(char *)XVT_B_OPT_A132 },

	{	XVT_TK_OPT_BLN,		O(c132),			TK_FLAG_NOHELP_0,
		"c132",				"-/+c132",			"switch to 132 column mode",
		"column132",			(char *)XVT_B_OPT_C132 },

	{	XVT_TK_OPT_BLN,		O(xerrors),			0,
		"xer",				"-/+xer",			"display X error msgs",
		"showXerrors",		(char *)XVT_B_OPT_XERRORS },

	{	XVT_TK_OPT_BLN,		O(prtkeep),			0,
		"po",				"-/+po",			"keep printer open",
		"keepPrinter",		(char *)XVT_B_OPT_PRTKEEP },

	{	XVT_TK_OPT_BLN,		O(cufix),			0,
		"cu",				"-/+cu",			"enable curses (more) fix",
		"curses",			(char *)XVT_B_OPT_CUFIX },

	{	XVT_TK_OPT_BLN,		O(acticon),			TK_FLAG_NOTINHELP,
		"ai",				"-/+ai",			"enable active icon",
		"activeIcon",		(char *)XVT_B_OPT_ACTICON },

	{	XVT_TK_OPT_BLN,		O(sb_enable),		TK_FLAG_NOTINHELP,
		"sb",				"-/+sb",			"enable scrollbar",
		"scrollBar",		(char *)XVT_B_OPT_SBENABLE },

	{	XVT_TK_OPT_BLN,		O(sb_right),		TK_FLAG_NOTINHELP,
		"sr",				"-/+sr",			"scrollbar on right",
		"scrollBarRight",	(char *)XVT_B_OPT_SBRIGHT },

	{	XVT_TK_OPT_BLN,		O(sb_shadow),		TK_FLAG_NOTINHELP,
		"st",				"-/+st",			"enable scrollbar shadow",
		"scrollBarShadow",	(char *)XVT_B_OPT_SBSHADOW },

	{	XVT_TK_OPT_BLN,		O(sb_scrout),		TK_FLAG_NOTINHELP,
		"si",				"-/+si",			"scroll on tty output",
		"scrollTtyOutput",	(char *)XVT_B_OPT_SBSCROUT },

	{	XVT_TK_OPT_BLN,		O(sb_scrkey),		TK_FLAG_NOTINHELP,
		"sk",				"-/+sk",			"scroll on keypress",
		"scrollTtyKeypress",	(char *)XVT_B_OPT_SBSCRKEY },

	/*--------------------------------------------------------------------
	 * debug options
	 */
	{	XVT_TK_OPT_DBG,		-1,					0,
		"debug",			"-debug type",		"debug <type> to <type>.log",
		0,					0,	xvt_tk_debug_lines },

	/*--------------------------------------------------------------------
	 * command options
	 */
	{	XVT_TK_OPT_CMD,		-1,					TK_FLAG_NOTHERE,
		"exec",				"-exec cmd ...",	"cmd to run (must be last)",
		0,					0 },

	/*--------------------------------------------------------------------
	 * end of list
	 */
	{ 0 }
};
