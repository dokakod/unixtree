/*****************************************************************************
** FILE NAME:		main.h
**
** DESCRIPTION:
**
******************************************************************************
** REVISION HISTORY:
**	Wed May 31, 1989 - initiated by Ken Broomfield
**
*****************************************************************************/

#include "libsys.h"

extern char Source_Name [MAX_PATHLEN];
extern char Help_Name [MAX_PATHLEN];
extern char Include_Name [MAX_PATHLEN];
extern FILE *Source_File;
extern FILE *Help_File;
extern FILE *Include_File;
extern int	Topic_Count;
extern int	Line_Number;
extern int	Last_Eol;
extern char Source_File_Error[];
extern char Help_File_Error[];
extern char Inc_File_Error[];
extern int verbose;
extern char *Pgm_Name;

extern	void Rewind_Source (void);
extern	void Pass1 (void);

/*****************************************************************************
**								Constants
*****************************************************************************/

/*
**	Limits
*/

#define MAX_SOURCE_LINE		128		/* max length of a source text file line */
#define MAX_SYMBOL			32		/* maximum symbolic topic name length,
										including terminator */
#define SYM_PTR_ARRAY_SIZE	1024	/* elements in topic symbol table pointer
										array */


#define COMMENT_CHAR		'#'		/* indicates a comment line */


/*****************************************************************************
**								Data Types
*****************************************************************************/

/*
**	Topic symbol table entry
**	Size should be a power of two
*/

typedef struct topsym
{
	struct topsym	*next;			/* link to next entry or NULL */
	int				index;			/* topic index */
	unsigned short	flags;			/* topic flags (GLOS, HCENTER, etc.) */
	char			name [1];		/* name string */
} TOPSYM;

#define TOPSYM_SIZE	( \
					sizeof(struct topsym *) + \
					sizeof(int) + \
					sizeof(unsigned short) + \
					sizeof(char) \
					)

/*****************************************************************************
**									Macros
*****************************************************************************/

/*.
******************************************************************************
**
**	macro -	 WHITESPACE:
**
**	desc -	  See if a character is whitespace (space, tab, newline)
**
**	entry -	 c				   char to check
**
**	exit -	  returns int	 TRUE if whitespace
**
**	notes -	 Beware of macro side effects.
**
******************************************************************************
*/

#define WHITESPACE(c)	((c) == ' '  ||  (c) == '\t'  ||  (c) == '\n')


/*.
******************************************************************************
**
**	macro -	 SEPARATOR:
**
**	desc -	  See if a character is a separator (i.e. between tokens on
**				the symbol/flags line of a topic)
**
**	entry -	 c				   char to check
**
**	exit -	  returns int	 TRUE if separator
**
**	notes -
**
******************************************************************************
*/

#define SEPARATOR(c)	((c) == ' '	 || \
						 (c) == ','  || \
						 (c) == '\t' || \
						 (c) == '\n')


/*.
******************************************************************************
**
**	macro -	 HELP_FILE_CHECK:
**
**	desc -	  Check for an error on the help file - display a message
**				and terminate if any
**
**	notes -
**
******************************************************************************
*/

#define HELP_FILE_CHECK if (ferror (Help_File))			\
						{								\
							fprintf (stderr, Help_File_Error);	\
							exit (-1);					\
						}


/*.
******************************************************************************
**
**	macro -	 INC_FILE_CHECK:
**
**	desc -	  Check for an error on the include file - display a message
**				and terminate if any
**
**	notes -
**
******************************************************************************
*/

#define INC_FILE_CHECK	if (ferror (Include_File))	\
						{								\
							fprintf (stderr, Inc_File_Error);		\
							exit (-1);					\
						}
