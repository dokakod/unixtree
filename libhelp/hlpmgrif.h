/*------------------------------------------------------------------------
 * help manager definitions
 */
#ifndef HLPMGRIF_H
#define HLPMGRIF_H

/*------------------------------------------------------------------------
 * constants
 */
#ifndef TRUE
#  define TRUE				(1)
#endif

#ifndef FALSE
#  define FALSE				(0)
#endif

/*------------------------------------------------------------------------
 * debugging flags
 */
#define DEBUGGING			FALSE		/* enables/disables debugging code */
#define TRACING				FALSE		/* enables/disables tracing */

/*------------------------------------------------------------------------
 * limits & defaults
 */
#define WIN_MAX_SCREEN_DIMY	128		/* max lines on a screen */

#define WIN_DEF_HSB_DIMY	1		/* default horz scroll bar height */
#define WIN_DEF_VSB_DIMX	1		/* default vertical scroll bar width */

#define WIN_EVENT_QUEUE_MAX 32		/* maximum elements in event queue */

/*------------------------------------------------------------------------
 * help definitions
 */
#define WIN_HELP_MAX_TITLE  60		/* maximum size of a help topic title */
#define WIN_HELP_MAX_LINE   60		/* maximum displayed size of a help text
										line (must be >= MAX_TITLE) */
#define WIN_HELP_MAX_LINE_BYTES 128 /* maximum size of a help text line: text,
										escape sequences, and terminator */
#define WIN_HELP_TITLE_LINES 1		/* number of lines of title text */
#define WIN_HELP_MAX_TOPICS 32767   /* maximum number of help topics */
#define WIN_HELP_INDEX_TOPIC 0		/* topic number of the main help index */
#define WIN_HELP_NO_TOPIC   -1		/* topic number indicating no help topic
										(brings up the help index topic) */
#define WIN_HELP_NO_HELP	-2		/* topic number indicating don't bring up
										help at all */
#define WIN_HELP_SIGNATURE  0xbeeff00d  /* signature value */

#define WIN_HELP_GLOS_BIT   0x8000  /* glossary topic */

/*------------------------------------------------------------------------
 * help file header
 */
typedef struct
{
	unsigned int	signature;			/* signature word */
	unsigned short 	topics;				/* number of topics in help file */
	unsigned char	reserved[10];		/* pad to 16 bytes */
} HELP_HDR;

/*------------------------------------------------------------------------
 * coordinate point
 */
typedef struct point
{
	int				x;
	int				y;
} HELP_POINT;

/*------------------------------------------------------------------------
 * help file topic array entry
 */
typedef struct
{
	int				text_offset;	/* offset into file for this topic */
	unsigned int	text_lines;		/* # lines of text, excluding title line */
	unsigned int	flags;			/* control flags */
} HELP_TOPIC;

/*------------------------------------------------------------------------
 * Escape string constants
 */
#define WIN_ESC_SIZE		2			/* size of escape sequences */
#define WIN_ESC_CHAR		0xff		/* first byte of escape sequence */

/* Second byte - opcodes - must never equal zero */
#define WIN_ESC_EAC_LO		0x10		/* set attribute for the next */
#define WIN_ESC_EAC_HI		0x1f		/* ..character only */
/* (low 4 bits are an attr index) */

#define WIN_ESC_EAS_LO		0x20		/* set attr for all subsequent */
#define WIN_ESC_EAS_HI		0x2f		/* ..characters */
/* (low 4 bits are an attr index) */

#define WIN_ESC_EA_INDEX_MASK 0x0f		/* mask to extract attribute index */
/* from attribute ESC sequence */

#define WIN_ESC_ECR			0x30		/* move to the strt of the next line */

#define WIN_ESC_ETAB		0x31		/* tab - used only in menus */
/* to right justify text */

#define WIN_ESC_LINK		0x32		/* topic link - used only in help */
/* text to link to another topic */

#define WIN_ESC_ESUB_LO		0x40		/* substitute string */
#define WIN_ESC_ESUB_HI		0x4f

/*------------------------------------------------------------------------
 * Window flag bits (used by hlpbld only)
 */
#define WIN_WF_CREATED_BIT			0x0001	/* window is created */
#define WIN_WF_TABSTOP_BIT			0x0002	/* tabstop in a dialog */
#define WIN_WF_GROUP_BIT			0x0004	/* first window in a dialog grp */
#define WIN_WF_TABGROUP_MASK (WIN_WF_TABSTOP_BIT | WIN_WF_GROUP_BIT)
#define WIN_WF_DEFBUTTON_BIT		0x0008	/* default enter pushbutton */
#define WIN_WF_ESCBUTTON_BIT		0x0010	/* escape pushbutton */
#define WIN_WF_HCENTER_BIT			0x0020	/* center window horizontally */
#define WIN_WF_VCENTER_BIT			0x0040	/* center window vertically */
#define WIN_WF_HVCENTER_MASK (WIN_WF_HCENTER_BIT | WIN_WF_VCENTER_BIT)
#define WIN_WF_ED_ALLOC_BIT			0x0080	/* text buf alloc'd in edit win */
#define WIN_WF_ED_UPCASE_BIT		0x0100	/* edit window cvt to upper case */
#define WIN_WF_ED_NUMERIC_BIT		0x8000	/* edit field accepts nums only */
#define WIN_WF_LEFT_BIT				0x0200	/* left justify window */
#define WIN_WF_RIGHT_BIT			0x0400	/* right justify window */
#define WIN_WF_TOP_BIT				0x0800	/* top justify window */
#define WIN_WF_BOTTOM_BIT			0x1000	/* bottom justify window */
#define WIN_WF_NOADJUST_BIT			0x2000	/* don't adjust window position */
/* if partially off screen */
#define WIN_WF_DISABLED_BIT			0x4000	/* window disabled */
#define WIN_WF_LIST_NOTAGMOVE_BIT	0x0080	/* don't move mark after tagging */
/* list items with the space bar */

/*------------------------------------------------------------------------
 * Escape string sequences
 */

/* Set the attribute for the next character only */

#define EAC0	"\377\020"			/* attribute index 0 */
#define EAC1	"\377\021"			/* attribute index 1 */
#define EAC2	"\377\022"			/* attribute index 2 */
#define EAC3	"\377\023"			/* attribute index 3 */

/* Set the attribute for all subsequent characters */

#define EAS0	"\377\040"			/* attribute index 0 */
#define EAS1	"\377\041"			/* attribute index 1 */
#define EAS2	"\377\042"			/* attribute index 2 */
#define EAS3	"\377\043"			/* attribute index 3 */

/*------------------------------------------------------------------------
 *  macro -		WIN_HELP_LINK_ENCODE:
 *
 *  desc -		Encode a help topic link index so that neither byte is
 *				0x00 (line terminator) or 0xff (escape character)
 *
 *  entry -		short index		   topic index: 0..SHRT_MAX
 *
 *  exit -		returns unsigned short	encoded topic index
 *
 *  notes -		For a thorough discussion the use of this macro,
 *				see the notes for the HLPBLD program in file MAIN.C.
 */
#define WIN_HELP_LINK_ENCODE(index) \
	((unsigned short) ((short)index) + \
	((unsigned short) ((unsigned int)index) / 0xfe) * 2 + 0x101)


/*------------------------------------------------------------------------
 *  macro -		WIN_HELP_LINK_DECODE:
 *
 *  desc -		Decode a help topic link index
 *
 *  entry -		unsigned short index	  encoded topic index
 *
 *  exit -		returns int		 decoded index: 0..INT_MAX
 *
 *  notes -		For a thorough discussion the use of this macro,
 *				see the notes for the HLPBLD program in file MAIN.C.
 */
#define WIN_HELP_LINK_DECODE(index) \
	((short) (((unsigned short)index) - \
	((((unsigned int)index) - 0x101) / 0x100) * 2 - 0x101))

#endif /* HLPMGRIF_H */
