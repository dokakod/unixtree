/*------------------------------------------------------------------------
 *	malloc interface routines
 *
 *	See sysmem.h for details.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sysmem.h"

/*------------------------------------------------------------------------
 * common defines
 */
#ifndef TRUE
#  define TRUE		1
#endif

#ifndef FALSE
#  define FALSE		0
#endif

/*------------------------------------------------------------------------
 * OS-specific defines
 */
#if defined(_MSC_VER) || defined(_WIN32)
	/*--------------------------------------------------------------------
	 * In Windows
	 */
#  include <windows.h>

	/*--------------------------------------------------------------------
	 * debugger output
	 */
#  define DEBUG_OUTPUT(s)	OutputDebugString(s)

	/*--------------------------------------------------------------------
	 * Invoke the debugger to abort
	 * (this will abort if no debugger present).
	 */
#  define OS_ABORT()		DebugBreak()

#else
	/*--------------------------------------------------------------------
	 * In UNIX
	 */

	/*--------------------------------------------------------------------
	 * debugger output
	 *
	 * This is currently disabled, since in SCO OpenServer the
	 * log functions are in the socket library, which we don't
	 * want to force the user to always reference.
	 *
	 * What we *really* need is an O/S-independent mechanism.
	 */
#  if 0
#    include <syslog.h>

static int	sysmem_log_open	= FALSE;

#    define DEBUG_OUTPUT(s)	if (! sysmem_log_open) \
							{ \
								openlog("sysmem", LOG_PID, LOG_USER); \
								sysmem_log_open = TRUE; \
							} \
							syslog(LOG_DEBUG, (char *)s)
#  else
#    define DEBUG_OUTPUT(s)	/* nada */
#  endif

	/*--------------------------------------------------------------------
	 * Use abort() to abort.
	 */
#  define OS_ABORT()		abort()

#endif /* O/S specific stuff */

/*------------------------------------------------------------------------
 * thread stuff
 *
 * Note: we only include the thread header if requested, so that
 * sysmem.[ch] can be extracted & used separately.
 */
#if V_USE_THREADS
	/*--------------------------------------------------------------------
	 * use critical sections
	 */
#  include "systhread.h"

static THREAD_MUTEX			sysmem_cs	= THREAD_MUTEX_INITIALIZER;

#  define mcs_init()		sysmutex_init	(&sysmem_cs)
#  define mcs_enter()		sysmutex_enter	(&sysmem_cs)
#  define mcs_leave()		sysmutex_leave	(&sysmem_cs)

#else
	/*--------------------------------------------------------------------
	 * no critical sections
	 */
#  define mcs_init()		/* nada */
#  define mcs_enter()		/* nada */
#  define mcs_leave()		/* nada */
#endif

/*------------------------------------------------------------------------
 * static data struct
 */
struct sysmem_data
{
	/*--------------------------------------------------------------------
	 * counters
	 */
	int			m_count;			/* count of mallocs				*/
	int			f_count;			/* count of frees				*/

	int			cur_allocated;		/* current allocated			*/
	int			max_allocated;		/* maximum allocated			*/

	/*--------------------------------------------------------------------
	 * data pointers
	 */
	MEM_NODE *	m_head;				/* first malloc entry			*/
	MEM_NODE *	m_tail;				/* last  malloc entry			*/

	MEM_NODE *	f_head;				/* first free   entry			*/
	MEM_NODE *	f_tail;				/* last  free   entry			*/

	/*--------------------------------------------------------------------
	 * debug flags (set by sysmem_init())
	 */
	int			malloc_init;		/* TRUE if initialized			*/

	int			do_debug;			/* TRUE if debugging wanted		*/
									/* from V_MALLOC_NODEBUG		*/

	int			do_free;			/* TRUE if frees really free	*/
									/* from V_MALLOC_NOFREE			*/

	int			do_chkalloc;		/* TRUE to check each malloc	*/
									/* from V_MALLOC_CHKALLOC		*/

	int			do_chkfree;			/* TRUE to check each free		*/
									/* from V_MALLOC_CHKFREE		*/

	int			do_chknull;			/* TRUE to check for free(0)	*/
									/* from V_MALLOC_CHKNULL		*/

	int			do_chknomem;		/* TRUE to check for NULL alloc	*/
									/* from V_MALLOC_CHKNOMEM		*/

	int			do_dmpalloc;		/* TRUE to dump each malloc		*/
									/* from V_MALLOC_DMPALLOC		*/

	int			do_dmpfree;			/* TRUE to dump each free		*/
									/* from V_MALLOC_DMPFREE		*/

	int			do_chain;			/* TRUE to dump chain in msgs	*/
									/* from V_MALLOC_CHAIN			*/

	int			do_abort;			/* TRUE to abort if error		*/
									/* from V_MALLOC_ABORT			*/

	int			do_dump;			/* TRUE to dump data with chain	*/
	int			dump_max;			/* max bytes of data to dump	*/
									/* from V_MALLOC_DUMP=nnn		*/

	int			do_stderr;			/* TRUE to default to stderr	*/
									/* from V_MALLOC_NOSTDERR		*/

	int			do_atexit;			/* TRUE to output msg atexit	*/
									/* from V_MALLOC_ATEXIT			*/

	FILE *		abort_fp;			/* stream to write msgs to		*/
									/* from V_MALLOC_DBGFILE=file	*/
									/* defaults to stderr			*/

	/*--------------------------------------------------------------------
	 * alternate outout info (set by sysmem_dumprtn())
	 */
	MEM_DUMP_RTN *	dump_rtn;		/* alt routine to call for msgs	*/
	void *			dump_data;		/* user data to pass to rtn()	*/
};

static struct sysmem_data	sysmem_data	= { 0 };
#define SD					sysmem_data

/*========================================================================
 * static routines
 */

/*------------------------------------------------------------------------
 * Function:	sysmem_atexit
 *
 * Description:	atexit processing
 */
static void
sysmem_atexit (void)
{
	char	malloc_msg[128];

	/*--------------------------------------------------------------------
	 * output the message
	 */
	sysmem_outstr(0, sysmem_stats(malloc_msg));

	/*--------------------------------------------------------------------
	 * now check if any mallocs left & dump chain if so
	 */
	if (SD.do_chain)
	{
		int count = sysmem_count();

		if (count > 0)
		{
			sysmem_mlist(0, SD.do_dump, SD.dump_max);
		}
	}
}

/*------------------------------------------------------------------------
 * Function:	sysmem_dump_data
 *
 * Description:	dump data in "hd" format:
 *
 *		nnnnnn 00112233 44556677 8899aabb ccddeeff ................
 *		...
 *
 * Returns:		none
 */
static void
sysmem_dump_data (FILE *fp, const char *data, int len)
{
	const unsigned char *	udata = (const unsigned char *)data;
	int		offset			= 0;
	int		chrs_per_line	= 16;

	/*----------------------------------------------------------------
	 * sanity check
	 */
	if (data == 0 || len <= 0)
		return;

	/*----------------------------------------------------------------
	 * run through data one line at a time
	 */
	while (offset < len)
	{
		int num = chrs_per_line;
		int i;
		char linebuf[128];
		char *l;

		/*----------------------------------------------------------------
		 * check if less than "chrs_per_line" chars left to display
		 */
		if ((len - offset) < num)
			num = (len - offset);

		/*----------------------------------------------------------------
		 * address part of line
		 */
		sprintf(linebuf, "      %06x ", offset);
		l = linebuf + strlen(linebuf);

		/*----------------------------------------------------------------
		 * hex part of line
		 */
		for (i=0; i<chrs_per_line; i++)
		{
			if ((i % 4) == 0)
				*l++ = ' ';

			if (i < num)
			{
				*l++ = "0123456789abcdef"[udata[i] / 16];
				*l++ = "0123456789abcdef"[udata[i] % 16];
			}
			else
			{
				*l++ = ' ';
				*l++ = ' ';
			}
		}

		/*----------------------------------------------------------------
		 * ascii part of line
		 */
		*l++ = ' ';
		for (i=0; i<chrs_per_line; i++)
		{
			int c;

			if (i < num)
			{
				c = udata[i];
				if (c < 0x20 || c >= 0x7f)
					c = '.';
			}
			else
			{
				c = ' ';
			}

			*l++ = c;
		}
		*l = 0;

		/*----------------------------------------------------------------
		 * output it
		 */
		sysmem_outstr(fp, linebuf);

		/*----------------------------------------------------------------
		 * bump counters
		 */
		offset += chrs_per_line;
		udata  += chrs_per_line;
	}
}

/*------------------------------------------------------------------------
 * Function:	sysmem_dump_entry
 *
 * Description:	dump a MEM_NODE entry
 *
 * Returns:		0 if entry OK, -1 if corrupted
 */
static int
sysmem_dump_entry (FILE *fp, MEM_NODE *n, int entry, int dump_data, int maxlen)
{
	int *	tail_magic_ptr;
	int		head_magic_number;
	int		hd_corrupt = FALSE;
	int		tl_corrupt = FALSE;
	char	linebuf[256];
	int		l;

	/*--------------------------------------------------------------------
	 * get header & trailer info
	 */
	head_magic_number = n->hd_magic;
	tail_magic_ptr 	  = (int *)((char *)n + sizeof(*n) + (n->size));

	/*--------------------------------------------------------------------
	 * create info line
	 */
	l = sprintf(linebuf, "%6d: %6d  %6d  %08lx",
		entry,
		n->mcount,
		n->size,
		(long)n->data);

	if (n->mfile != 0)
	{
		l += sprintf(linebuf + l, "  alloc at %5d  %s",
			n->mline, n->mfile);

		if (n->mrtn != 0)
			l += sprintf(linebuf + l, " (%s)", n->mrtn);
	}

	/*--------------------------------------------------------------------
	 * add corrupted info if needed
	 */
	if (head_magic_number != MEM_HD_MAGIC &&
	    head_magic_number != MEM_FR_MAGIC )
	{
		strcat(linebuf, "  *** HEAD CORRUPTED");
		hd_corrupt = TRUE;
	}

	if (tail_magic_ptr != n->ptlr)
	{
		strcat(linebuf, "  *** TAIL PTR CORRUPTED");
		hd_corrupt = TRUE;
	}

	if (! hd_corrupt &&
	    *tail_magic_ptr   != MEM_TL_MAGIC)
	{
		strcat(linebuf, "  *** TAIL CORRUPTED");
		tl_corrupt = TRUE;
	}

	/*--------------------------------------------------------------------
	 * output this line
	 */
	sysmem_outstr(fp, linebuf);

	/*--------------------------------------------------------------------
	 * If entry was freed, output free info also
	 */
	if (n->ffile)
	{
		int l;

		l = sprintf(linebuf, "        %6d                    freed at %5d  %s",
			n->fcount,
			n->fline,
			n->ffile);
		if (n->frtn != 0)
			sprintf(linebuf + l, " (%s)", n->frtn);
		sysmem_outstr(fp, linebuf);
	}

	/*--------------------------------------------------------------------
	 * output data if requested and entry not corrupt
	 */
	if (dump_data && ! hd_corrupt)
	{
		int len = n->size;

		if (maxlen > 0 && maxlen < len)
			len = maxlen;

		sysmem_dump_data(fp, n->data, len);
	}

	/*--------------------------------------------------------------------
	 * return whether data is valid or corrupt
	 */
	return (hd_corrupt || tl_corrupt ? -1 : 0);
}

/*------------------------------------------------------------------------
 * Function:	sysmem_errmsg
 *
 * Description:	abort with error msg
 *
 * Returns:		none
 *
 * Note:		this routine does not return if do_abort is in effect
 */
static void
sysmem_errmsg (void *p, MEM_NODE *m, const char *msg,
	const char *rtn, const char *file, int line)
{
	char	linebuf[128];
	int		l = 0;

	/*----------------------------------------------------------------
	 * output address if present
	 */
	if (p != 0)
	{
		l += sprintf(linebuf + l, "address %08lx: ", (long)p);
	}

	/*----------------------------------------------------------------
	 * output error msg
	 */
	l += sprintf(linebuf + l, "%s", msg);

	/*----------------------------------------------------------------
	 * output file info if present
	 */
	if (file != 0)
	{
		l += sprintf(linebuf + l, ": line %d: %s (%s)", line, file, rtn);
	}

	sysmem_outstr(SD.abort_fp, linebuf);

	/*----------------------------------------------------------------
	 * dump current node if we have one
	 */
	if (m != 0)
	{
		sysmem_dump_entry(SD.abort_fp, m, 0, SD.do_dump, SD.dump_max);
	}

	/*----------------------------------------------------------------
	 * dump malloc queue if requested
	 */
	if (SD.do_chain)
		sysmem_mlist(SD.abort_fp, SD.do_dump, SD.dump_max);

	/*----------------------------------------------------------------
	 * now abort if requested
	 */
	sysmem_abort();
}

/*========================================================================
 * global routines
 */

/*------------------------------------------------------------------------
 * Function:	sysmem_init
 *
 * Description:	initialize the malloc system
 */
void
sysmem_init (void)
{
	char *env;

	/*----------------------------------------------------------------
	 * do this only once
	 */
	if (SD.malloc_init)
		return;

	/*----------------------------------------------------------------
	 * first time initializations
	 */
	mcs_init();

	SD.m_count			= 0;
	SD.f_count			= 0;
	SD.cur_allocated	= 0;
	SD.max_allocated	= 0;

	SD.m_head			= 0;
	SD.m_tail			= 0;
	SD.f_head			= 0;
	SD.f_tail			= 0;

	SD.malloc_init		= TRUE;
	SD.do_debug			= TRUE;
	SD.do_free			= TRUE;
	SD.do_chkalloc		= FALSE;
	SD.do_chkfree		= FALSE;
	SD.do_chknull		= FALSE;
	SD.do_chknomem		= FALSE;
	SD.do_dmpalloc		= FALSE;
	SD.do_dmpfree		= FALSE;
	SD.do_chain			= FALSE;
	SD.do_abort			= FALSE;
	SD.do_dump			= FALSE;
	SD.dump_max			= FALSE;
	SD.do_stderr		= TRUE;
	SD.do_atexit		= FALSE;

	SD.abort_fp			= 0;

	SD.dump_rtn			= 0;
	SD.dump_data		= 0;

	/*----------------------------------------------------------------
	 * check env variables
	 */
	env = getenv("V_MALLOC_NODEBUG");
	if (env != 0)
	{
		SD.do_debug = FALSE;
	}

	env = getenv("V_MALLOC_NOFREE");
	if (env != 0)
	{
		SD.do_free = FALSE;
	}

	env = getenv("V_MALLOC_CHKALLOC");
	if (env != 0)
	{
		SD.do_chkalloc = TRUE;
	}

	env = getenv("V_MALLOC_CHKFREE");
	if (env != 0)
	{
		SD.do_chkfree = TRUE;
	}

	env = getenv("V_MALLOC_CHKNULL");
	if (env != 0)
	{
		SD.do_chknull = TRUE;
	}

	env = getenv("V_MALLOC_CHKNOMEM");
	if (env != 0)
	{
		SD.do_chknomem = TRUE;
	}

	env = getenv("V_MALLOC_DMPALLOC");
	if (env != 0)
	{
		SD.do_dmpalloc = TRUE;
	}

	env = getenv("V_MALLOC_DMPFREE");
	if (env != 0)
	{
		SD.do_dmpfree = TRUE;
	}

	env = getenv("V_MALLOC_CHAIN");
	if (env != 0)
	{
		SD.do_chain = TRUE;
	}

	env = getenv("V_MALLOC_DUMP");
	if (env != 0)
	{
		SD.do_dump  = TRUE;
		SD.dump_max = atoi(env);
	}

	env = getenv("V_MALLOC_ABORT");
	if (env != 0)
	{
		SD.do_abort = TRUE;
	}

	env = getenv("V_MALLOC_NOSTDERR");
	if (env != 0)
	{
		SD.do_stderr = FALSE;
	}

	env = getenv("V_MALLOC_ATEXIT");
	if (env != 0)
	{
		SD.do_atexit = TRUE;
	}

	env = getenv("V_MALLOC_DBGFILE");
	if (env != 0)
	{
		SD.abort_fp = fopen(env, "w");
	}

	/*----------------------------------------------------------------
	 * set default stream for messages
	 */
	if (SD.abort_fp == 0 && SD.do_stderr)
		SD.abort_fp = stderr;

	/*----------------------------------------------------------------
	 * check for atexit processsing
	 */
	if (SD.do_atexit)
		atexit(sysmem_atexit);
}

/*------------------------------------------------------------------------
 * Function:	sysmem_malloc
 *
 * Description:	malloc interface
 *
 * Returns:		pointer to area or NULL
 */
void *
sysmem_malloc (int size,
	const char *rtn, const char *file, int line)
{
	char *		p;
	MEM_NODE *	m;
	int			get_size;

	/*----------------------------------------------------------------
	 * check if initialized
	 */
	if (! SD.malloc_init)
		sysmem_init();

	/*----------------------------------------------------------------
	 * if not debug mode, just do minimum stuff
	 */
	if (! SD.do_debug)
	{
		p = (char *)malloc(size);
		if (p != 0)
			SD.m_count++;

		return ((void *)p);
	}

	/*----------------------------------------------------------------
	 * check if chain-check requested
	 */
	if (SD.do_chkalloc)
	{
		char msgbuf[128];
		int entry_number = sysmem_check(msgbuf);

		if (entry_number)
		{
			char msg[128];

			sprintf(msg, "check at malloc failed at entry %d: %s",
				entry_number, msgbuf);
			sysmem_errmsg(0, 0, msg, rtn, file, line);
		}
	}

	/*----------------------------------------------------------------
	 * check that size is > 0
	 */
	if (size <= 0)
	{
		char msg[128];

		sprintf(msg, "malloc called with size %d", size);
		sysmem_errmsg(0, 0, msg, rtn, file, line);

		return (0);
	}

	/*----------------------------------------------------------------
	 * round up size to a multiple of "sizeof(int)"
	 * (for alignment purposes)
	 */
	size = (size + sizeof(int) - 1) & ~(sizeof(int)-1);

	/*----------------------------------------------------------------
	 * calculate size to get
	 */
	get_size = size + sizeof(MEM_NODE) + sizeof(int);

	/*----------------------------------------------------------------
	 * get the memory
	 */
	p = (char *)malloc(get_size);
	if (p == 0)
	{
		char msg[128];

		sprintf(msg, "could not allocate %d bytes", size);
		sysmem_errmsg(0, 0, msg, rtn, file, line);

		return (0);
	}

	/*----------------------------------------------------------------
	 * get pointer to node & data
	 */
	m = (MEM_NODE *)p;
	p += sizeof(MEM_NODE);

	/*----------------------------------------------------------------
	 * perform critical processing
	 */
	mcs_enter();
	{
		/*------------------------------------------------------------
		 * adjust counters
		 */
		SD.m_count++;
		SD.cur_allocated += size;
		if (SD.cur_allocated > SD.max_allocated)
			SD.max_allocated = SD.cur_allocated;

		/*------------------------------------------------------------
		 * set header info
		 */
		m->hd_magic = MEM_HD_MAGIC;
		m->mcount   = SD.m_count;
		m->fcount   = 0;
		m->size     = size;
		m->mrtn		= rtn;
		m->mline    = line;
		m->fline    = 0;
		m->mfile    = file;
		m->ffile    = 0;
		m->frtn		= 0;
		m->prev     = SD.m_tail;
		m->next     = 0;
		m->data     = p;
		m->ptlr		= (int *)(p + size);

		/*------------------------------------------------------------
		 * set trailer info
		 */
		*m->ptlr	= MEM_TL_MAGIC;

		/*------------------------------------------------------------
		 * add this node to the list
		 */
		if (SD.m_head == 0)
			SD.m_head = m;
		else
			SD.m_tail->next = m;

		SD.m_tail = m;
	}
	mcs_leave();

	/*----------------------------------------------------------------
	 * check if alloc dump requested
	 */
	if (SD.do_dmpalloc)
	{
		char	linebuf[256];
		int		l;

		l = sprintf(linebuf, "alloc:\t%6d\t%6d\t%08lx",
			m->mcount,
			m->size,
			(long)m->data);

		if (m->mfile != 0)
		{
			sprintf(linebuf + l, "  at %5d  %s", m->mline, m->mfile);
		}

		sysmem_outstr(SD.abort_fp, linebuf);
	}

	/*----------------------------------------------------------------
	 * return data pointer to caller
	 */
	return ((void *)p);
}

/*------------------------------------------------------------------------
 * Function:	sysmem_calloc
 *
 * Description:	calloc interface
 *
 * Returns:		pointer to area or NULL
 *
 * Notes:		zeroes out the area
 */
void *
sysmem_calloc (int num, int size,
	const char *rtn, const char *file, int line)
{
	void *	ptr;
	int		len = num * size;

	ptr = sysmem_malloc(len, rtn, file, line);
	if (ptr != 0)
		memset(ptr, 0, len);

	return (ptr);
}

/*------------------------------------------------------------------------
 * Function:	sysmem_realloc
 *
 * Description:	Reallocate a buffer to a different size
 *
 * Returns:		new pointer
 */
void *
sysmem_realloc (void *pointer, int size,
	const char *rtn, const char *file, int line)
{
	void *		p;
	MEM_NODE *	m = 0;

	/*----------------------------------------------------------------
	 * if not debugging, just do the minimum
	 */
	if (! SD.do_debug)
	{
		SD.m_count++;
		SD.f_count++;
		p = (void *)realloc(pointer, size);

		return (p);
	}

	/*----------------------------------------------------------------
	 * check new size
	 */
	if (size < 0)
	{
		char msg[128];

		sprintf(msg, "realloc called with size %d", size);
		sysmem_errmsg(pointer, 0, msg, rtn, file, line);

		return (0);
	}

	/*----------------------------------------------------------------
	 * if size is zero & pointer is not NULL, just do a free
	 */
	if (size == 0 && pointer != 0)
	{
		sysmem_free(pointer, rtn, file, line);

		return (0);
	}

	/*----------------------------------------------------------------
	 * if old pointer specified, verify it
	 */
	if (pointer != 0)
	{
		int *	t;

		m = (MEM_NODE *)((char *)pointer - sizeof(MEM_NODE));
		t = (int      *)((char *)pointer + m->size);

		if (m->hd_magic != MEM_HD_MAGIC ||
		    t           != m->ptlr      ||
		    *t          != MEM_TL_MAGIC)
		{
			const char *msg;

			/*------------------------------------------------
			 * data is invalid - check why
			 */
			if (m->hd_magic == MEM_FR_MAGIC)
			{
				msg = "attempt to realloc already freed data";
			}
			else if (m->hd_magic != MEM_HD_MAGIC)
			{
				msg = "corrupted data header";
			}
			else if (t != m->ptlr)
			{
				msg = "corrupted trailer pointer";
			}
			else /* if (*t != MEM_TL_MAGIC) */
			{
				msg = "corrupted data trailer";
			}

			sysmem_errmsg(pointer, m, msg, rtn, file, line);
		}
	}

	/*----------------------------------------------------------------
	 * allocate new data area
	 */
	p = sysmem_malloc(size, rtn, file, line);

	/*----------------------------------------------------------------
	 * if old data exists, copy it & free it
	 */
	if (pointer != 0)
	{
		int s;

		/*--------------------------------------------------------
		 * get size of old data
		 */
		s = m->size;

		/*--------------------------------------------------------
		 * now copy the data
		 */
		if (size < s)
			s = size;
		memcpy(p, pointer, s);

		/*--------------------------------------------------------
		 * now free the old pointer
		 */
		sysmem_free(pointer, rtn, file, line);
	}

	return (p);
}

/*------------------------------------------------------------------------
 * Function:	sysmem_free
 *
 * Description:	free interface
 *
 * Returns:		none
 */
void
sysmem_free (void *pointer,
	const char *rtn, const char *file, int line)
{
	char *		p;
	MEM_NODE *	m;
	MEM_NODE *	mn;
	MEM_NODE *	mp;
	int *		t;
	int			s;

	/*----------------------------------------------------------------
	 * check if NULL pointer
	 */
	if (pointer == 0)
	{
		if (SD.do_chknull)
		{
			sysmem_errmsg(pointer, 0, "free called with NULL pointer",
				rtn, file, line);
		}

		return;
	}

	/*----------------------------------------------------------------
	 * if not debug mode, just do minimum
	 */
	if (! SD.do_debug)
	{
		SD.f_count++;
		free(pointer);
		return;
	}

	/*----------------------------------------------------------------
	 * check if anything to free
	 */
	if (SD.m_head == 0)
	{
		sysmem_errmsg(pointer, 0, "free called with no mallocs pending",
			rtn, file, line);
		return;
	}

	/*----------------------------------------------------------------
	 * check if chain-check requested
	 */
	if (SD.do_chkfree)
	{
		char msgbuf[128];
		int entry_number = sysmem_check(msgbuf);

		if (entry_number)
		{
			char msg[128];

			sprintf(msg, "check at free failed at entry %d: %s",
				entry_number, msgbuf);
			sysmem_errmsg(0, 0, msg, rtn, file, line);
		}
	}

	/*----------------------------------------------------------------
	 * get pointer to our header and trailer
	 */
	p = (char *)pointer;
	p -= sizeof(MEM_NODE);
	m = (MEM_NODE *)p;
	s = m->size;

	t = (int *)(p + sizeof(MEM_NODE) + s);

	/*----------------------------------------------------------------
	 * check if data is still intact
	 */
	if (m->hd_magic != MEM_HD_MAGIC ||
	    t           != m->ptlr      ||
	    *t          != MEM_TL_MAGIC)
	{
		const char *msg;

		/*--------------------------------------------------------
		 * data is invalid - check why
		 */
		if (m->hd_magic == MEM_FR_MAGIC)
		{
			msg = "attempt to free already freed data";
		}
		else if (m->hd_magic != MEM_HD_MAGIC)
		{
			msg = "corrupted data header";
		}
		else if (t != m->ptlr)
		{
			msg = "corrupted trailer pointer";
		}
		else /* if (*t != MEM_TL_MAGIC) */
		{
			msg = "corrupted data trailer";
		}

		sysmem_errmsg(pointer, m, msg, rtn, file, line);
	}

	/*----------------------------------------------------------------
	 * perform critical processing
	 */
	mcs_enter();
	{
		/*------------------------------------------------------------
		 * check if free dump requested
		 */
		if (SD.do_dmpfree)
		{
			char	linebuf[256];
			int		l;

			l = sprintf(linebuf, "free:\t%6d\t%6d\t%08lx",
				m->mcount,
				m->size,
				(long)m->data);

			if (file != 0)
			{
				sprintf(linebuf + l, "  at %5d  %s", line, file);
			}

			sysmem_outstr(SD.abort_fp, linebuf);
		}

		/*------------------------------------------------------------
		 * store free info in entry
		 */
		m->hd_magic = MEM_FR_MAGIC;
		m->fcount   = ++SD.f_count;
		m->fline    = line;
		m->ffile    = file;
		m->frtn		= rtn;

		/*------------------------------------------------------------
		 * adjust our pointers
		 */
		mn = m->next;
		mp = m->prev;

		if (mn == 0)
			SD.m_tail = mp;
		else
			mn->prev = mp;

		if (mp == 0)
			SD.m_head = mn;
		else
			mp->next = mn;

		/*------------------------------------------------------------
		 * adjust counters
		 */
		SD.cur_allocated -= s;
	}
	mcs_leave();

	/*----------------------------------------------------------------
	 * check if we are actually freeing the data
	 */
	if (SD.do_free)
	{
		/*--------------------------------------------------------
		 * yes - free it
		 */
		free(p);
	}
	else
	{
		/*--------------------------------------------------------
		 * no - add this node to the free list
		 */
		mcs_enter();
		{
			m->next = 0;
			m->prev = SD.f_tail;

			if (SD.f_head == 0)
				SD.f_head = m;
			else
				SD.f_tail->next = m;

			SD.f_tail = m;
		}
		mcs_leave();
	}
}

/*------------------------------------------------------------------------
 * Function:	sysmem_strdup
 *
 * Description:	strdup interface
 *
 * Returns:		pointer to new string or NULL
 */
char *
sysmem_strdup (const char *str,
	const char *rtn, const char *file, int line)
{
	int		l;
	char *	p;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (str == 0)
		return 0;

	/*--------------------------------------------------------------------
	 * allocate buffer for copy
	 */
	l = strlen(str);
	p = (char *)sysmem_malloc(l+1, rtn, file, line);

	/*--------------------------------------------------------------------
	 * copy data if allocate succeeded
	 */
	if (p != 0)
		strcpy(p, str);

	return (p);
}

/*------------------------------------------------------------------------
 * Function:	sysmem_count
 *
 * Description:	return number of outstanding entries
 *
 * Returns:		number of outstanding malloc counts
 */
int
sysmem_count (void)
{
	int count;

	/*----------------------------------------------------------------
	 * check if initialized
	 */
	if (! SD.malloc_init)
		sysmem_init();

	/*----------------------------------------------------------------
	 * get count of outstanding mallocs
	 */
	mcs_enter();
	{
		count = (SD.m_count - SD.f_count);
	}
	mcs_leave();

	return (count);
}

/*------------------------------------------------------------------------
 * Function:	sysmem_check
 *
 * Description:	check the malloc chain
 *
 * Returns:		-1  general error in chain
 *				 0  chain OK
 *				>0  entry number with error
 */
int
sysmem_check (char *msgbuf)
{
	MEM_NODE *	n;
	int			count = 0;
	int			num_outstanding;
	int			rc;

	/*----------------------------------------------------------------
	 * check if initialized
	 */
	if (! SD.malloc_init)
		sysmem_init();

	/*----------------------------------------------------------------
	 * if not debugging, this call is worthless
	 */
	if (! SD.do_debug)
		return (0);

	/*----------------------------------------------------------------
	 * perform critical processing
	 */
	mcs_enter();
	{
		num_outstanding = (SD.m_count - SD.f_count);

		/*------------------------------------------------------------
		 * check top-level counters
		 */
		if (num_outstanding < 0)
		{
			sprintf(msgbuf, "malloc count (%d) < free count (%d)",
				SD.m_count, SD.f_count);
			rc = -1;
			goto done;
		}

		if (SD.m_head == 0)
		{
			if (num_outstanding != 0)
			{
				sprintf(msgbuf, "empty chain with malloc count = %d",
					num_outstanding);
				rc = -1;
				goto done;
			}
		}

		/*------------------------------------------------------------
		 * rip through the chain & validate all entries
		 */
		for (n=SD.m_head; n; n=n->next)
		{
			int *tail_magic_ptr;
			int  head_magic_number;

			count++;

			head_magic_number = n->hd_magic;
			tail_magic_ptr 	  = (int *)((char *)n + sizeof(*n) + (n->size));

			if (head_magic_number != MEM_HD_MAGIC &&
			    head_magic_number != MEM_FR_MAGIC )
			{
				sprintf(msgbuf, "invalid header");
				rc = count;
				goto done;
			}

			if (tail_magic_ptr != n->ptlr)
			{
				sprintf(msgbuf, "invalid trailer pointer");
				rc = count;
				goto done;
			}

			if (*tail_magic_ptr != MEM_TL_MAGIC)
			{
				sprintf(msgbuf, "invalid trailer");
				rc = count;
				goto done;
			}

			if (n->mcount <= 0 || n->mcount > SD.m_count)
			{
				sprintf(msgbuf, "invalid entry count %d", n->mcount);
				rc = count;
				goto done;
			}
		}

		/*------------------------------------------------------------
		 * final checks
		 */
		if (num_outstanding != count)
		{
			sprintf(msgbuf,
			"counters: mallocs (%d) frees (%d) outstanding (%d) chain (%d)",
				SD.m_count, SD.f_count, num_outstanding, count);
			rc = -1;
			goto done;
		}

		rc = 0;

done:	;
	}
	mcs_leave();

	return (rc);
}

/*------------------------------------------------------------------------
 * Function:	sysmem_stats
 *
 * Description:	return a string summarizing malloc usage
 *
 * Returns:		pointer to msgbuf
 */
char *
sysmem_stats (char *msgbuf)
{
	/*----------------------------------------------------------------
	 * check if initialized
	 */
	if (! SD.malloc_init)
		sysmem_init();

	/*----------------------------------------------------------------
	 * get statistics message
	 */
	mcs_enter();
	{
		int	count	= ( SD.m_count - SD.f_count);

		if (SD.do_debug)
		{
			sprintf(msgbuf,
				"mallocs = %d  frees = %d  cnt = %d  cur = %d  max = %d",
				SD.m_count, SD.f_count, count,
				SD.cur_allocated, SD.max_allocated);
		}
		else
		{
			sprintf(msgbuf,
				"mallocs = %d  frees = %d  cnt = %d",
				SD.m_count, SD.f_count, count);
		}
	}
	mcs_leave();

	return (msgbuf);
}

/*------------------------------------------------------------------------
 * Function:	sysmem_outstr, sysmem_outfmt, sysmem_outvar
 *
 * Description:	output a debug line
 *
 * Returns:		none
 */
void
sysmem_outstr (FILE *fp, const char *line)
{
	/*--------------------------------------------------------------------
	 * alt output routine always takes precedence
	 */
	if (SD.dump_rtn != 0)
	{
		/*----------------------------------------------------------------
		 * call user output routine with line
		 */
		(*SD.dump_rtn)(SD.dump_data, line);
	}
	else
	{
		/*----------------------------------------------------------------
		 * if fp is NULL, use default stream
		 */
		if (fp == 0)
			fp = SD.abort_fp;

		/*----------------------------------------------------------------
		 * output to stream or debugger
		 */
		if (fp != 0)
		{
			fputs(line, fp);
			fputc('\n', fp);
			fflush(fp);
		}
		else
		{
			DEBUG_OUTPUT(line);
		}
	}
}

void
sysmem_outfmt (FILE *fp, const char *fmt, ...)
{
	va_list	args;

	va_start(args, fmt);
	sysmem_outvar(fp, fmt, args);
	va_end(args);
}

void
sysmem_outvar (FILE *fp, const char *fmt, va_list args)
{
	char	buffer[BUFSIZ];

	vsprintf(buffer, fmt, args);
	sysmem_outstr(fp, buffer);
}

/*------------------------------------------------------------------------
 * check if any mallocs are outstanding & dump chain if so
 *
 * This outputs a malloc count msg &
 * optionally dumps the chain if enything is left.
 */
void
sysmem_memchk (FILE *fp, int dump_chn)
{
	char	malloc_msg[128];

	/*--------------------------------------------------------------------
	 * output the message
	 */
	sysmem_outstr(0, sysmem_stats(malloc_msg));

	/*--------------------------------------------------------------------
	 * now check if any mallocs left & dump chain if so
	 */
	if (dump_chn)
	{
		int count = sysmem_count();

		if (count > 0)
		{
			sysmem_mlist(fp, FALSE, 0);
		}
	}
}

/*------------------------------------------------------------------------
 * Function:	sysmem_mlist
 *
 * Description:	dump malloc chain to a stream
 *
 * Returns:		number of entries
 */
int
sysmem_mlist (FILE *fp, int dump_data, int maxlen)
{
	int count = 0;

	/*----------------------------------------------------------------
	 * check if initialized
	 */
	if (! SD.malloc_init)
		sysmem_init();

	/*----------------------------------------------------------------
	 * only do this if debugging is enabled
	 * (otherwise there is no chain)
	 */
	if (SD.do_debug)
	{
		const char *	dashes = "================================";

		sysmem_outstr(fp, dashes);

		/*--------------------------------------------------------
		 * perform critical processing
		 */
		mcs_enter();
		{
			/*----------------------------------------------------
			 * dump out list
			 */
			if (SD.m_head == 0)
			{
				sysmem_outstr(fp, "no blocks allocated");
			}
			else
			{
				MEM_NODE *	n;
				char		msg[128];
				int			entry = 0;

				/*--------------------------------------------
				 * output count of entries
				 */
				sprintf(msg, "number of entries: %d",
					SD.m_count - SD.f_count);
				sysmem_outstr(fp, msg);

				/*--------------------------------------------
				 * output each entry
				 */
				for (n=SD.m_head; n; n=n->next)
				{
					int rc;

					rc = sysmem_dump_entry(fp, n, ++entry, dump_data, maxlen);
					if (rc)
					{
						sysmem_outstr(fp, "*** break");
						break;
					}

					count++;
				}
			}
		}
		mcs_leave();

		sysmem_outstr(fp, dashes);
	}

	return (count);
}

/*------------------------------------------------------------------------
 * Function:	sysmem_flist
 *
 * Description:	dump free chain to a stream
 *
 * Returns:		number of entries
 */
int
sysmem_flist (FILE *fp, int dump_data, int maxlen)
{
	int count = 0;

	/*----------------------------------------------------------------
	 * check if initialized
	 */
	if (! SD.malloc_init)
		sysmem_init();

	/*----------------------------------------------------------------
	 * only do this if debugging is enabled & no-free is in effect
	 * (otherwise there is no chain)
	 */
	if (SD.do_debug && ! SD.do_free)
	{
		sysmem_outstr(fp, "=====================");

		/*--------------------------------------------------------
		 * perform critical processing
		 */
		mcs_enter();
		{
			/*----------------------------------------------------
			 * dump out list
			 */
			if (SD.f_head == 0)
			{
				sysmem_outstr(fp, "no blocks freed");
			}
			else
			{
				MEM_NODE *	n;
				char		msg[128];
				int			entry = 0;

				/*--------------------------------------------
				 * output count of entries
				 */
				sprintf(msg, "number of entries: %d", SD.f_count);
				sysmem_outstr(fp, msg);

				/*--------------------------------------------
				 * output each entry
				 */
				for (n=SD.f_head; n; n=n->next)
				{
					int rc;

					rc = sysmem_dump_entry(fp, n, ++entry, dump_data, maxlen);
					if (rc)
					{
						sysmem_outstr(fp, "*** break");
						break;
					}

					count++;
				}
			}
		}
		mcs_leave();

		sysmem_outstr(fp, "=====================");
	}

	return (count);
}

/*------------------------------------------------------------------------
 * Function:	sysmem_traverse
 *
 * Description:	traversee the malloc chain
 *
 * Returns:		none
 */
void
sysmem_traverse (MEM_TRAVERSE_RTN *rtn, void *data)
{
	/*----------------------------------------------------------------
	 * check if initialized
	 */
	if (! SD.malloc_init)
		sysmem_init();

	/*------------------------------------------------------------
	 * perform critical processing
	 */
	mcs_enter();
	{
		MEM_NODE *n;

		/*--------------------------------------------------------
		 * traverse the malloc list
		 */
		for (n=SD.m_head; n; n=n->next)
		{
			int rc;

			/*------------------------------------------------------------
			 * call user routine
			 */
			rc = (*rtn)(data, n);
			if (rc)
			{
				/*--------------------------------------------------------
				 * user-requested break
				 */
				break;
			}
		}
	}
	mcs_leave();
}

/*------------------------------------------------------------------------
 * Function:	sysmem_prev
 *
 * Description:	find the entry previous to the entry for specified data
 *
 * Returns:		pointer to previous entry or NULL
 */
MEM_NODE *
sysmem_prev (void *p)
{
	MEM_NODE *	m_data	= MEM_NODE_HDR_PTR(p);
	MEM_NODE *	m_curr;
	MEM_NODE *	m_prev;

	/*----------------------------------------------------------------
	 * check if initialized
	 */
	if (! SD.malloc_init)
		return (0);

	/*------------------------------------------------------------
	 * do search within the critical section
	 */
	mcs_enter();
	{
		/*--------------------------------------------------------
		 * check the malloc chain
		 */
		m_prev = 0;
		for (m_curr = SD.m_head; m_curr; m_curr = m_curr->next)
		{
			if (m_curr == m_data)
				break;

			m_prev = m_curr;
		}

		/*----------------------------------------------------------------
		 * if entry not found, check the free chain
		 */
		if (m_curr == 0)
		{
			m_prev = 0;
			for (m_curr = SD.f_head; m_curr; m_curr = m_curr->next)
			{
				if (m_curr == m_data)
					break;

				m_prev = m_curr;
			}
		}
	}
	mcs_leave();

	/*--------------------------------------------------------------------
	 * check if we have an entry
	 */
	if (m_curr == 0)
		m_prev = 0;

	return (m_prev);
}

/*------------------------------------------------------------------------
 * Function:	sysmem_dumprtn
 *
 * Description:	specify a dump routine to call to output msgs
 *
 * Returns:		none
 */
void
sysmem_dumprtn (MEM_DUMP_RTN *rtn, void *data)
{
	/*----------------------------------------------------------------
	 * check if initialized
	 */
	if (! SD.malloc_init)
		sysmem_init();

	/*----------------------------------------------------------------
	 * store data
	 */
	mcs_enter();
	{
		SD.dump_rtn  = rtn;
		SD.dump_data = data;
	}
	mcs_leave();
}

/*------------------------------------------------------------------------
 * Function:	sysmem_abort
 *
 * Description:	abort if requested
 *
 * Returns:		none
 *
 * Notes:		if do_abort is set, this does not return
 */
void
sysmem_abort (void)
{
	/*----------------------------------------------------------------
	 * abort if requested
	 */
	if (SD.do_abort)
	{
		/*--------------------------------------------------------
		 * abort the process
		 */
		OS_ABORT();

		/*--------------------------------------------------------
		 * just in case we come back (guaranteed to abort!)
		 */
		++*(char *)0;
		/*NOTREACHED*/
	}
}
