/*------------------------------------------------------------------------
 *	malloc interface package
 *
 *	This is the interface file for the malloc package, which provides
 *	increased debugging capabilities.
 *
 *	This package keeps its own chain of allocated/freed memory requests,
 *	and aids in tracking memory leaks, etc.
 *
 *	A key part of this mechanism is that we can capture the filename
 *	and line number of the source file that performed the allocation
 *	and free.
 *
 *------------------------------------------------------------------------
 *
 *	The following environment variables are queried at run time the first
 *	time a call is made to MALLOC() or when sysmem_init() is called:
 *
 *	V_MALLOC_NODEBUG	If set, do minimal debugging (not recommended).
 *						[value is ignored]
 *						(default is to do debugging)
 *
 *	V_MALLOC_NOFREE		If set, don't actually free any data.
 *						This enables catching a free of already freed data.
 *						[value is ignored]
 *						(default is to free data)
 *
 *	V_MALLOC_CHAIN		If set, output the malloc chain on error.
 *						[value is ignored]
 *						(default is to not output the chain)
 *
 *	V_MALLOC_DUMP=nnn	If set, dump data along with the chain entries.
 *						[value is max chars per entry, 0 == all]
 *						(default is to not dump data in chain dump)
 *
 *	V_MALLOC_ABORT		If set, abort if error occurs (after all msgs).
 *						[value is ignored]
 *						(default is to continue if error detected)
 *
 *	V_MALLOC_CHKNULL	If set, check for frees of a NULL pointer.
 *						[value is ignored]
 *						(default is to ignore frees of NULL)
 *
 *	V_MALLOC_CHKNOMEM	If set, output message if a memory allocation fails.
 *						[value is ignored]
 *						(default is to just return a NULL pointer)
 *
 *	V_MALLOC_CHKALLOC	If set, check chain at each allocate.
 *						[value is ignored]
 *						(default is to not check the chain each time)
 *
 *	V_MALLOC_CHKFREE	If set, check chain at each free.
 *						[value is ignored]
 *						(default is to not check the chain each time)
 *
 *	V_MALLOC_DMPALLOC	If set, dump alloc info for each MALLOC call.
 *						[value is ignored]
 *						(default is to not dump the chain each time)
 *
 *	V_MALLOC_DMPFREE	If set, dump free info for each FREE call.
 *						[value is ignored]
 *						(default is to not dump the chain each time)
 *
 *	V_MALLOC_DBGFILE=p	If set, names a debug file for msgs.
 *						[value is pathname to use]
 *						(default is stderr)
 *
 *	V_MALLOC_NOSTDERR	If set, do not use stderr as default output stream.
 *						In Windows, this will direct any output to the
 *						debugger via OutputDebugString().
 *						[value is ignored]
 *						(default is to use stderr as default stream)
 *
 *	V_MALLOC_ATEXIT		If set, automatically display a usage message at exit.
 *						[value is ignored]
 *						(default is to not display msg at exit)
 *
 *------------------------------------------------------------------------
 *
 *	There are 2 compile-time modes and 2 run-time modes available:
 *
 *	compile-time modes:
 *
 *		If V_MALLOC_FILEINFO is defined and is non-zero, then all calls
 *		have the file-name and line-number added to the call.  This
 *		produces the maximum debugging capability at the expense of
 *		executable-size.  Note that this can be turned on for
 *		individual source files or libraries, in order to add extra
 *		debugging only in suspect code.  It is recommended that this
 *		option only be used in debug mode.
 *
 *		If V_MALLOC_FILEINFO is NOT defined or is zero, then all calls
 *		do not have this info added.  All the other debugging features
 *		(such as creating malloc chains, etc.) are still used.
 *
 *		Note that this compile-time option is used in routines that
 *		call these memory routines.
 *
 *	run-time modes:
 *
 *		If the env variable V_MALLOC_NODEBUG is defined (the value
 *		is ignored), then only minimal debugging is done, i.e.,
 *		just a count of MALLOCs and FREEs are done.
 *
 *		If the env variable V_MALLOC_NODEBUG is NOT defined, then
 *		malloc debugging is enabled, and all the above-mentioned env
 *		variables are queried.
 *
 *------------------------------------------------------------------------
 *
 * Tracking memory-leaks
 *
 * A memory-leak is caused by a program "forgetting" about a particular
 * mamory allocation.  The program may function properly, but its
 * memory usage will grow as it runs.  The easiest way to check for this
 * is to do the following:
 *
 *	1.	At program termination, free all memory that you know that you
 *		allocated.  If using the extended curses package, be sure to call
 *		end_curses() to free all curses-related memory.
 *
 *	2.	Call sysmem_count() and check if the return value is zero.  If so,
 *		then you have accounted for all memory allocations.
 *
 *	3.	If there are outstanding memory allocations from step #2, then
 *		dump them out using the sysmem_mlist().  This call will be most
 *		useful if V_MALLOC_FILEINFO was defined at compile time.
 *
 * For example, the following code could be used:
 *
 *	void pgm_exit (int exit_code)
 *	{
 *		int count;
 *
 *		// free all data from the program
 *		...
 *
 *		// free all curses data structs
 *		end_curses();
 *
 *		// check malloc info
 *		count = SYSMEM_COUNT();
 *		if (count > 0)
 *		{
 *			char msgbuf[64];
 *
 *			SYSMEM_OUTSTR(0, SYSMEM_STATS(msgbuf));
 *			SYSMEM_MLIST(0, 0, 0);
 *		}
 *
 *		exit(exit_code);
 *	}
 *
 * or, use the following code:
 *
 *	void pgm_exit (int exit_code)
 *	{
 *		// free all data from the program
 *		...
 *
 *		// free all curses data structs
 *		end_curses();
 *
 *		// check malloc info
 *		SYSMEM_MEMCHK(0, TRUE);
 *
 *		exit(exit_code);
 *	}
 *
 *------------------------------------------------------------------------
 */
#ifndef SYSMEM_H
#define SYSMEM_H

#include <stdio.h>
#include <stdarg.h>

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * memory node struct
 */
typedef struct mem_node MEM_NODE;
struct mem_node
{
	int				hd_magic;	/* reg magic or free magic		*/
	int				size;		/* size of data (user) area		*/

	int				mcount;		/* malloc count number			*/
	int				fcount;		/* free   count number			*/

	int				mline;		/* line num where gotten or 0	*/
	int				fline;		/* line num where freed	 or 0	*/

	const char *	mrtn;		/* routine  where gotten or 0	*/
	const char *	frtn;		/* routine  where freed  or 0	*/

	const char *	mfile;		/* filename where gotten or 0	*/
	const char *	ffile;		/* filename where freed	 or 0	*/

	MEM_NODE *		next;		/* pointer to next entry or 0	*/
	MEM_NODE *		prev;		/* pointer to prev entry or 0	*/

	char *			data;		/* pointer to data area			*/
	int	*			ptlr;		/* pointer to trailer			*/

#if 0							/* desc of rest of entry		*/

	char			data[size];	/* actual user data				*/

								/* Note: since this follows		*/
								/* a pointer, it will always	*/
								/* be suitably aligned.			*/

								/* Also note: size will always	*/
								/* be rounded up to a multiple	*/
								/* of 4 (sizeof(int)).			*/

	int				tl_magic;	/* trailer magic number			*/
#endif
};

/*------------------------------------------------------------------------
 * magic numbers used in MEM_NODE
 */
#define MEM_HD_MAGIC	11111111	/* hd_magic value when allocated	*/
#define MEM_TL_MAGIC	22222222	/* tl_magic value when allocated	*/
#define MEM_FR_MAGIC	33333333	/* hd_magic value when freed		*/

/*------------------------------------------------------------------------
 * macro to access node info for an allocated data item
 */
#define MEM_NODE_HDR_PTR(p)	( (MEM_NODE *)((char *)(p) - sizeof(MEM_NODE)) )
#define MEM_NODE_TLR_PTR(p)	( MEM_NODE_HDR_PTR(p)->ptlr )

/*------------------------------------------------------------------------
 * interface calls for passing file/line info
 */
#define SYSMEM_MALLOC(s,r,f,l)		sysmem_malloc	(s,		r, f, l)
#define SYSMEM_CALLOC(n,s,r,f,l)	sysmem_calloc	(n, s,	r, f, l)
#define SYSMEM_REALLOC(p,s,r,f,l)	sysmem_realloc	(p, s,	r, f, l)
#define SYSMEM_FREE(p,r,f,l)		sysmem_free		(p,		r, f, l)
#define SYSMEM_STRDUP(s,r,f,l)		sysmem_strdup	(s,		r, f, l)

	/*--------------------------------------------------------------------
	 * debug interface
	 */
#ifdef V_MALLOC_FILEINFO

#  define MALLOC(s)				SYSMEM_MALLOC	(s,		0, __FILE__, __LINE__)
#  define CALLOC(n,s)			SYSMEM_CALLOC	(n, s,	0, __FILE__, __LINE__)
#  define REALLOC(p,s)			SYSMEM_REALLOC	(p, s,	0, __FILE__, __LINE__)
#  define FREE(p)				SYSMEM_FREE		(p,		0, __FILE__, __LINE__)
#  define STRDUP(s)				SYSMEM_STRDUP	(s,		0, __FILE__, __LINE__)

	/*--------------------------------------------------------------------
	 * non-debug interface
	 */
#else /* V_MALLOC_FILEINFO */

#  define MALLOC(s)				SYSMEM_MALLOC	(s,		0, 0, 0)
#  define CALLOC(n,s)			SYSMEM_CALLOC	(n, s,	0, 0, 0)
#  define REALLOC(p,s)			SYSMEM_REALLOC	(p, s,	0, 0, 0)
#  define FREE(p)				SYSMEM_FREE		(p,		0, 0, 0)
#  define STRDUP(s)				SYSMEM_STRDUP	(s,		0, 0, 0)

#endif /* V_MALLOC_FILEINFO */

/*------------------------------------------------------------------------
 * debugging calls
 */
#define SYSMEM_STATS(s)				sysmem_stats	(s)
#define SYSMEM_OUTSTR(fp, str)		sysmem_outstr	(fp, str)
#define SYSMEM_COUNT()				sysmem_count	()
#define SYSMEM_MLIST(fp,dump,len)	sysmem_mlist	(fp, dump, len)
#define SYSMEM_MEMCHK(fp,dump_chn)	sysmem_memchk	(fp, dump_chn)

/*========================================================================
 * malloc interface calls
 */
extern void *	sysmem_malloc	(int size,
								const char *rtn, const char* file, int line);

extern void *	sysmem_calloc	(int num, int size,
								const char *rtn, const char *file, int line);

extern void *	sysmem_realloc	(void *pointer, int size,
								const char *rtn, const char *file, int line);

extern void		sysmem_free		(void *pointer,
								const char *rtn, const char *file, int line);

extern char *	sysmem_strdup	(const char *str,
								const char *rtn, const char *file, int line);

/*========================================================================
 * malloc interface utility calls
 */

/*------------------------------------------------------------------------
 * initialize malloc-interface
 *
 * This routine is normally called automatically the first time
 * a call to MALLOC is done, but the user may call it himself.
 * This routine does all the env variable querying.
 *
 * This call results in a no-op if the interface is already initialized.
 */
extern void		sysmem_init		(void);

/*------------------------------------------------------------------------
 * output a message line via the sysmem logging facility
 *
 * Note that these calls will always append a new-line to the end
 * of the line being output.
 */
extern void		sysmem_outstr	(FILE *fp, const char *str);
extern void		sysmem_outfmt	(FILE *fp, const char *fmt, ...);
extern void		sysmem_outvar	(FILE *fp, const char *fmt, va_list args);

/*------------------------------------------------------------------------
 * get count of outstanding mallocs
 *
 * returns: the count of mallocs which have not been freed
 */
extern int		sysmem_count	(void);

/*------------------------------------------------------------------------
 * check integrity of malloc chain
 *
 * returns:
 *		-1	general error in chain
 *		 0	chain OK
 *		>0	entry number with error (first encountered)
 */
extern int		sysmem_check	(char *msgbuf);

/*------------------------------------------------------------------------
 * get a one-line printable summary of malloc statistics
 *
 * returns: pointer to msgbuf (which will contain the summary line)
 */
extern char *	sysmem_stats	(char *msgbuf);

/*------------------------------------------------------------------------
 * check if any mallocs are outstanding & dump chain if so
 *
 * This outputs a malloc count msg &
 * optionally dumps the chain if enything is left.
 */
extern void		sysmem_memchk	(FILE *fp, int dump_chn);

/*------------------------------------------------------------------------
 * dump the malloc chain
 *
 * Each entry will be displayed on a line with the following:
 *
 *		entry number	(0 -> not in table yet or removed from table)
 *		malloc number	(the nth call to malloc)
 *		size			(in bytes)
 *		address of data	(in hex)
 *		file-name		(if present)
 *		line-number		(if present)
 *
 * If "fp" is NULL, output will go to stderr (unless sysmem_dumprtn()
 * has been called to specify an alternate output).
 *
 * If "dump_data" is TRUE, each line will be followed by subsequent lines
 * with the actual data in that entry.
 *
 * If "maxlen" is zero, all data will be dumped.  If "maxlen" > 0,
 * then only "maxlen" bytes will be dumped.
 *
 * returns: number of entries in the chain
 */
extern int		sysmem_mlist	(FILE *fp, int dump_data, int maxlen);

/*------------------------------------------------------------------------
 * dump the free chain (only valid if V_MALLOC_NOFREE is set)
 *
 * See sysmem_mlist() for details.  Each entry will also have the
 * file-name and line-number where the entry was freed, if present.
 *
 * returns: number of entries in the chain
 */
extern int		sysmem_flist	(FILE *fp, int dump_data, int maxlen);

/*------------------------------------------------------------------------
 * traverse the malloc chain
 *
 *	MEM_TRAVERSE_RTN routine is called with pointer to each node & pointer to
 *	user data.  It returns 0 (keep going), != 0 (stop traverse).
 */
typedef	int		MEM_TRAVERSE_RTN	(void *data, MEM_NODE *p);

extern	void	sysmem_traverse	(MEM_TRAVERSE_RTN *rtn, void *data);

/*------------------------------------------------------------------------
 * find the entry previous to a data pointer.
 *
 * This routine will try to find the MEM_NODE entry for the entry previous
 * to the entry containing the data specified.  It will search the malloc
 * chain for the entry, and if not found, will search the free chain if
 * present.
 *
 * This routine is intended to be used from within a debugger.
 * It is assumed that if a data entry is "trashed", it is due to the program
 * "over-running"  the data in the previous entry, thus trashing the data
 * (and all node info for that entry).  This call enables the user to find
 * that entry, and, hopefully, discover where the over-run occurred.
 *
 * Assume the following scenario:
 *
 *		char *p = MALLOC(n);	// then p later contains bogus data
 *
 * Then, from the debugger (we are using DBX here) the user can give
 * a command such as:
 *
 *		print * sysmem_prev(p)
 *
 * This command will print out the node info (if found) for the previous
 * entry.
 *
 * Note that the results from this call may be unreliable, since the
 * previous data may have been already freed and the memory re-allocated.
 * To insure maximum usefulness of this call, specify V_MALLOC_NOFREE in
 * the environment when running the program exhibiting this problem.
 * Also, if V_MALLOC_FILEINFO is specified at compile time, the filename &
 * line-number where the data was allocated will be available.
 *
 * returns: pointer to previous node if found or NULL
 */
extern MEM_NODE *	sysmem_prev	(void *p);

/*------------------------------------------------------------------------
 * specify an alternate routine to call to do all output
 *
 * If this is not called, all output will be written to stderr
 * (unless V_MALLOC_DBGFILE was specified with a filename for output).
 */
typedef  void   MEM_DUMP_RTN	(void *data, const char *line);

extern void		sysmem_dumprtn	(MEM_DUMP_RTN *rtn, void *data);

/*------------------------------------------------------------------------
 * abort (to debugger if possible)
 */
extern void		sysmem_abort	(void);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* SYSMEM_H */
