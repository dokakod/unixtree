/*------------------------------------------------------------------------
 *	thread & mutex routines
 *
 *	There are three versions here:
 *
 *		1.	Threads not supported
 *		2.	Threads in Windows
 *		3.	Threads in Unix
 *
 *	These routines manipulate three different data types:
 *
 *		1.	THREAD_ID		sub-thread IDs
 *		2.	THREAD_MUTEX	critical sections
 *		3.	THREAD_COUNTER	thread-protected counters
 *
 *	Notes:
 *
 *	1.	If the compile-time variable V_USE_THREADS is NOT set,
 *		these routines always act as if threads are not supported.
 *
 *	2.	If the run-time environment variable V_NO_THREADS is set,
 *		these routines always act as if threads are not supported.
 */
#ifndef SYSTHREAD_H
#define SYSTHREAD_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * thread routine prototype
 *
 * This mess comes from the following:
 *
 * 1.   Windows:	thread routines are 'void   (*)(void *)' & are __cdecl
 * 2.   Unix:		thread routines are 'void * (*)(void *)' & must be "C"
 */
#if defined(_MSC_VER_) || defined(_WIN32)
#  define THREAD_RTN_DECL	void __cdecl
#  define THREAD_RTN_TYPE	void
#  define THREAD_RTN_ARGS	void *
#  define THREAD_RTN_VAL	/* nothing */
#else
#  ifdef __cplusplus
#    define THREAD_RTN_DECL	extern "C" void *
#  else
#    define THREAD_RTN_DECL	void *
#  endif
#  define THREAD_RTN_TYPE	void *
#  define THREAD_RTN_ARGS	void *
#  define THREAD_RTN_VAL	0
#endif

typedef THREAD_RTN_TYPE THREAD_RTN (THREAD_RTN_ARGS);

/*------------------------------------------------------------------------
 * thread-id data type
 */
typedef unsigned long		THREAD_ID;

/*------------------------------------------------------------------------
 * mutex data type
 *
 * This struct is designed to be the same size regardless of the O/S
 * or whether threads are used or not.
 */
typedef struct thread_mutex	THREAD_MUTEX;
struct thread_mutex
{
	int				initialized;	/* TRUE if initialized	*/
	int				depth;			/* recurse count		*/

	union
	{
		struct
		{
			int		filler[16];
		}			m_filler;
	}				cs;
};

#define THREAD_MUTEX_INITIALIZER	{ 0 }	/* initialization value	*/

/*------------------------------------------------------------------------
 * counter data type
 */
typedef long				THREAD_COUNTER;

/*------------------------------------------------------------------------
 * thread functions
 */
extern int				systhread_avail		(void);
extern THREAD_ID		systhread_begin		(THREAD_RTN *rtn, void *data);
extern void				systhread_end		(void);
extern void				systhread_wait		(THREAD_ID tid);
extern int				systhread_cancel	(THREAD_ID tid, int wait);
extern THREAD_ID		systhread_id		(void);
extern int				systhread_fork		(void);

/*------------------------------------------------------------------------
 * mutex functions
 */
extern THREAD_MUTEX *	sysmutex_alloc		(void);
extern void				sysmutex_free		(THREAD_MUTEX *mp);

extern void				sysmutex_init		(THREAD_MUTEX *mp);
extern void				sysmutex_delete		(THREAD_MUTEX *mp);

extern int				sysmutex_try		(THREAD_MUTEX *mp);
extern void				sysmutex_enter		(THREAD_MUTEX *mp);
extern void				sysmutex_leave		(THREAD_MUTEX *mp);

/*------------------------------------------------------------------------
 * counter functions
 */
extern THREAD_COUNTER	syscount_incr		(THREAD_COUNTER *cp);
extern THREAD_COUNTER	syscount_decr		(THREAD_COUNTER *cp);
extern THREAD_COUNTER	syscount_exch		(THREAD_COUNTER *cp,
												THREAD_COUNTER c);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* SYSTHREAD_H */
