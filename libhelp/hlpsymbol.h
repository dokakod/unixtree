/*****************************************************************************
** FILE NAME:   symbol.h
**
** DESCRIPTION:
**
******************************************************************************
** REVISION HISTORY:
**   Thu Jul 27, 1989 - initiated by "Ken Broomfield"
**
*****************************************************************************/

extern	void Sym_Init (void);
extern	int Add_Symbol (unsigned char *sym, int index, unsigned short flags);
extern	TOPSYM *Lookup_Symbol (unsigned char *sym);
