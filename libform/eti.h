/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifndef _ETI_H
#define	_ETI_H

#ifdef __cplusplus
extern "C" {
#endif

#define	MAX_COMMAND	(KEY_MAX + 512)

typedef int		OPTIONS;

typedef char *		(*	PTF_charP) ();
typedef void		(*	PTF_void) ();
typedef int		(*	PTF_int) ();

#define	E_OK			  0
#define	E_SYSTEM_ERROR		 -1
#define	E_BAD_ARGUMENT		 -2
#define	E_POSTED		 -3
#define	E_CONNECTED		 -4
#define	E_BAD_STATE		 -5
#define	E_NO_ROOM		 -6
#define	E_NOT_POSTED		 -7
#define	E_UNKNOWN_COMMAND	 -8
#define	E_NO_MATCH		 -9
#define	E_NOT_SELECTABLE	-10
#define	E_NOT_CONNECTED		-11
#define	E_REQUEST_DENIED	-12
#define	E_INVALID_FIELD		-13
#define	E_CURRENT		-14

#ifdef __cplusplus
}
#endif

#endif	/* _ETI_H */
