/*------------------------------------------------------------------------
 * thead & mutex routines
 */

#include <stdlib.h>
#include <string.h>
#include "sys_defs.h"
#include "sys_osdep.h"
#include "systhread.h"
#include "sysmem.h"

#if V_WINDOWS
#  ifndef V_USE_THREADS
#    define V_USE_THREADS	1
#  endif
#endif

#if V_USE_THREADS
#  if V_WINDOWS
#    include <windows.h>
#  else
#    include <pthread.h>
#  endif
#endif

/*------------------------------------------------------------------------
 * internal mutex data type
 *
 * This struct is designed to be the same size regardless of the O/S
 * or whether threads are used or not.
 */
typedef struct thread_mutex_t	THREAD_MUTEX_T;
struct thread_mutex_t
{
	int							initialized;	/* TRUE if initialized	*/
	int							depth;			/* recurse count		*/

	union
	{
#if V_USE_THREADS
#  if V_WINDOWS
		/*----------------------------------------------------------------
		 * WIN32 mutex
		 */
		struct
		{
			CRITICAL_SECTION	crit;
		}						m_win32;
#  else
		/*----------------------------------------------------------------
		 * UNIX mutex
		 */
		struct
		{
			pthread_mutex_t		crit;
			pthread_mutexattr_t	attr;
		}						m_unix;
#  endif
#endif
		/*----------------------------------------------------------------
		 * DEFAULT mutex (for no threads available)
		 */
		struct
		{
			int					crit;
		}						m_dflt;

		/*----------------------------------------------------------------
		 * FILLER mutex (to allocate proper amount of space)
		 */
		struct
		{
			int					filler[16];
		}						m_filler;
	}							cs;
};

/*========================================================================
 * thread functions
 */
#if V_USE_THREADS
static int				systhread_use	= -1;
#  if ! V_WINDOWS
static pthread_mutex_t	systhread_cs	= PTHREAD_MUTEX_INITIALIZER;
#  endif
#else
static int				systhread_use	= FALSE;
#endif

/*------------------------------------------------------------------------
 * systhread_avail() - return whether threads are available
 *
 * returns: TRUE if threads available, FALSE if not
 */
int systhread_avail (void)
{
	if (systhread_use < 0)
	{
		const char *env = getenv("V_NO_THREADS");

		systhread_use = (env == 0);
	}

	return (systhread_use);
}

/*------------------------------------------------------------------------
 * systhread_begin() - run a procedure in a sub-thread
 *
 * returns: thread-id of sub-thread or 0
 */
THREAD_ID systhread_begin (THREAD_RTN *rtn, void *data)
{
#if V_USE_THREADS
	if (systhread_avail())
	{
#  if V_WINDOWS
		/*----------------------------------------------------------------
		 * windows version
		 */
		{
			THREAD_ID	tid;

			tid = _beginthread(rtn, 0, data);
			return (tid);
		}
#  else
		/*----------------------------------------------------------------
		 * unix version
		 */
		{
			pthread_t	tid;
			int			rc;

			rc = pthread_create(&tid, 0, rtn, data);
			if (rc < 0)
				tid = 0;

			return ((THREAD_ID)tid);
		}
#  endif
	}
#endif

	/*--------------------------------------------------------------------
	 * no threads available
	 */
	return (0);
}

/*------------------------------------------------------------------------
 * systhread_end() - called by a thread to end itself
 */
void systhread_end (void)
{
#if V_USE_THREADS
	if (systhread_avail())
	{
#  if V_WINDOWS
		/*----------------------------------------------------------------
		 * windows version
		 */
		{
			_endthread();
		}
#  else
		/*----------------------------------------------------------------
		 * unix version
		 */
		{
			pthread_exit(0);
		}
#  endif
	}
#endif
}

/*------------------------------------------------------------------------
 * systhread_wait() - wait for a sub-thread to terminate
 */
void systhread_wait (THREAD_ID tid)
{
#if V_USE_THREADS
	if (systhread_avail())
	{
#  if V_WINDOWS
		/*----------------------------------------------------------------
		 * windows version
		 */
		{
			return;
		}
#  else
		/*----------------------------------------------------------------
		 * unix version
		 */
		{
			pthread_join((pthread_t)tid, 0);
		}
#  endif
	}
#endif
}

/*------------------------------------------------------------------------
 * systhread_cancel() - cancel a sub-thread
 */
int systhread_cancel (THREAD_ID tid, int wait)
{
#if V_USE_THREADS
	if (systhread_avail())
	{
#  if V_WINDOWS
		/*----------------------------------------------------------------
		 * windows version
		 */
		{
			return (-1);
		}
#  else
		/*----------------------------------------------------------------
		 * unix version
		 */
		{
			int	rc;

			rc = pthread_cancel((pthread_t)tid);
			if (rc == 0 && wait)
				systhread_wait(tid);

			return (rc);
		}
#  endif
	}
#endif

	/*--------------------------------------------------------------------
	 * no threads available
	 */
	return (-1);
}

/*------------------------------------------------------------------------
 * systhread_id() - called by a thread to get its ID
 */
THREAD_ID systhread_id (void)
{
#if V_USE_THREADS
	if (systhread_avail())
	{
#  if V_WINDOWS
		/*----------------------------------------------------------------
		 * windows version
		 */
		{
			return GetCurrentThreadId();
		}
#  else
		/*----------------------------------------------------------------
		 * unix version
		 */
		{
			return (THREAD_ID)pthread_self();
		}
#  endif
	}
#endif

	/*--------------------------------------------------------------------
	 * no threads available
	 */
	return (0);
}

/*------------------------------------------------------------------------
 * systhread_fork() - called to fork() only the primary thread
 *
 * returns:	<0	error
 *			 0	child  return
 *			>0	parent return (value is pid of child)
 */
int systhread_fork (void)
{
#if V_WINDOWS
	/*--------------------------------------------------------------------
	 * windows version (no fork() in windows)
	 */
	{
		return (-1);
	}
#else
	/*--------------------------------------------------------------------
	 * unix version
	 */
	{
		int pid;

		if (systhread_avail())
		{
#  if defined(__sun)
			pid = fork1();
#  elif defined(THREAD_FORK)
			pid = THREAD_FORK();
#  else
			pid = fork();
#endif
		}
		else
		{
			pid = fork();
		}

		return (pid);
	}
#endif
}

/*========================================================================
 * mutex routines
 */

/*------------------------------------------------------------------------
 * sysmutex_alloc() - allocate & initialize a critical section
 */
THREAD_MUTEX * sysmutex_alloc (void)
{
	THREAD_MUTEX *	mp;

	mp = (THREAD_MUTEX *)MALLOC(sizeof(*mp));
	if (mp != 0)
	{
		memset(mp, 0, sizeof(*mp));
		sysmutex_init(mp);
	}

	return (mp);
}

/*------------------------------------------------------------------------
 * sysmutex_free() - delete & free a critical section
 */
void sysmutex_free (THREAD_MUTEX *mp)
{
	if (mp != 0)
	{
		sysmutex_delete(mp);
		FREE(mp);
	}
}

/*------------------------------------------------------------------------
 * sysmutex_init() - initialize a critical section
 */
void sysmutex_init (THREAD_MUTEX *mp)
{
	THREAD_MUTEX_T *	tm	= (THREAD_MUTEX_T *)mp;

#if V_USE_THREADS
	if (systhread_avail())
	{
#  if V_WINDOWS
		/*----------------------------------------------------------------
		 * windows version
		 */
		{
			if (tm != 0)
			{
				tm->initialized		= TRUE;
				tm->depth			= 0;
				InitializeCriticalSection(&tm->cs.m_win32.crit);
			}
		}
#  else
		/*----------------------------------------------------------------
		 * unix version
		 */
		{
			if (tm != 0)
			{
				tm->initialized		= TRUE;
				tm->depth			= 0;
				pthread_mutexattr_init(&tm->cs.m_unix.attr);
#if defined(linux)
				pthread_mutexattr_settype(&tm->cs.m_unix.attr,
					PTHREAD_MUTEX_RECURSIVE_NP);
#else
				pthread_mutexattr_settype(&tm->cs.m_unix.attr,
					PTHREAD_MUTEX_RECURSIVE);
#endif
				pthread_mutex_init(&tm->cs.m_unix.crit, &tm->cs.m_unix.attr);
			}
		}
#  endif
	}
	else
#endif

	/*--------------------------------------------------------------------
	 * no threads available
	 */
	{
		if (tm != 0)
		{
			tm->initialized		= TRUE;
			tm->depth			= 0;
			tm->cs.m_dflt.crit	= 0;
		}
	}
}

/*------------------------------------------------------------------------
 * sysmutex_delete() - delete a critical section
 */
void sysmutex_delete (THREAD_MUTEX *mp)
{
	THREAD_MUTEX_T *	tm	= (THREAD_MUTEX_T *)mp;

#if V_USE_THREADS
	if (systhread_avail())
	{
#  if V_WINDOWS
		/*----------------------------------------------------------------
		 * windows version
		 */
		{
			if (tm != 0)
			{
				tm->initialized		= FALSE;
				tm->depth			= 0;
				DeleteCriticalSection(&tm->cs.m_win32.crit);
			}
		}
#  else
		/*----------------------------------------------------------------
		 * unix version
		 */
		{
			if (tm != 0)
			{
				tm->initialized		= FALSE;
				tm->depth			= 0;
				pthread_mutex_destroy(&tm->cs.m_unix.crit);
				pthread_mutexattr_destroy(&tm->cs.m_unix.attr);
			}
		}
#  endif
	}
	else
#endif

	/*--------------------------------------------------------------------
	 * no threads available
	 */
	{
		if (tm != 0)
		{
			tm->initialized		= FALSE;
			tm->depth			= 0;
			tm->cs.m_dflt.crit	= -1;
		}
	}
}

/*------------------------------------------------------------------------
 * sysmutex_try() - try to lock a critical section
 *
 * returns:	0 if lock suceeded, -1 if not
 */
int sysmutex_try (THREAD_MUTEX *mp)
{
	THREAD_MUTEX_T *	tm	= (THREAD_MUTEX_T *)mp;
	int					rc;

#if V_USE_THREADS
	if (systhread_avail())
	{
#  if V_WINDOWS
		/*----------------------------------------------------------------
		 * windows version
		 */
		{
			if (tm != 0)
			{
				BOOL bRC;

#    if (_WIN32_WINNT >= 0x400)
				bRC = TryEnterCriticalSection(&tm->cs.m_win32.crit);
				if (bRC)
					tm->depth++;
#    else
				bRC = FALSE;
#    endif

				rc = (bRC ? 0 : -1);
			}
			else
			{
				rc = -1;
			}
		}
#  else
		/*----------------------------------------------------------------
		 * unix version
		 */
		{
			if (tm != 0)
			{
				rc = pthread_mutex_trylock(&tm->cs.m_unix.crit);
				if (rc == 0)
					tm->depth++;
			}
			else
			{
				rc = -1;
			}
		}
#  endif
	}
	else
#endif

	/*--------------------------------------------------------------------
	 * no threads available
	 */
	{
		if (tm != 0)
		{
			tm->cs.m_dflt.crit++;
			tm->depth++;
			rc = 0;
		}
		else
		{
			rc = -1;
		}
	}
	return (rc);
}

/*------------------------------------------------------------------------
 * sysmutex_enter() - unconditionally lock a critical section (may wait)
 */
void sysmutex_enter (THREAD_MUTEX *mp)
{
	THREAD_MUTEX_T *	tm	= (THREAD_MUTEX_T *)mp;

#if V_USE_THREADS
	if (systhread_avail())
	{
#  if V_WINDOWS
		/*----------------------------------------------------------------
		 * windows version
		 */
		{
			if (tm != 0)
			{
				EnterCriticalSection(&tm->cs.m_win32.crit);
				tm->depth++;
			}
		}
#  else
		/*----------------------------------------------------------------
		 * unix version
		 */
		{
			if (tm != 0)
			{
				pthread_mutex_lock(&tm->cs.m_unix.crit);
				tm->depth++;
			}
		}
#  endif
	}
	else
#endif

	/*--------------------------------------------------------------------
	 * no threads available
	 */
	{
		if (tm != 0)
		{
			tm->cs.m_dflt.crit++;
			tm->depth++;
		}
	}
}

/*------------------------------------------------------------------------
 * sysmutex_leave() - unlock a critical section
 */
void sysmutex_leave (THREAD_MUTEX *mp)
{
	THREAD_MUTEX_T *	tm	= (THREAD_MUTEX_T *)mp;

#if ! V_USE_THREADS
	/*--------------------------------------------------------------------
	 * no threads available
	 */
	{
		if (tm != 0)
		{
			tm->depth--;
			tm->cs.m_dflt.crit--;
		}
	}
#else
#  if V_WINDOWS
	/*--------------------------------------------------------------------
	 * windows version
	 */
	{
		if (tm != 0)
		{
			tm->depth--;
			LeaveCriticalSection(&tm->cs.m_win32.crit);
		}
	}
#  else
	/*--------------------------------------------------------------------
	 * unix version
	 */
	{
		if (tm != 0)
		{
			tm->depth--;
			pthread_mutex_unlock(&tm->cs.m_unix.crit);
		}
	}
#  endif
#endif
}

/*========================================================================
 * counter functions
 */

/*------------------------------------------------------------------------
 * syscount_incr() - increment a counter
 */
THREAD_COUNTER syscount_incr (THREAD_COUNTER *cp)
{
#if V_USE_THREADS
	if (systhread_avail())
	{
#  if V_WINDOWS
		/*----------------------------------------------------------------
		 * windows version
		 */
		{
			return InterlockedIncrement(cp);
		}
#  else
		/*----------------------------------------------------------------
		 * unix version
		 */
		{
			THREAD_COUNTER	c;

			pthread_mutex_lock(&systhread_cs);
			{
				c = ++*cp;
			}
			pthread_mutex_unlock(&systhread_cs);

			return (c);
		}
#  endif
	}
#endif

	/*--------------------------------------------------------------------
	 * no threads available
	 */
	{
		return (++*cp);
	}
}

/*------------------------------------------------------------------------
 * syscount_decr() - decrement a counter
 */
THREAD_COUNTER syscount_decr (THREAD_COUNTER *cp)
{
#if V_USE_THREADS
	if (systhread_avail())
	{
#  if V_WINDOWS
		/*----------------------------------------------------------------
		 * windows version
		 */
		{
			return InterlockedDecrement(cp);
		}
#  else
		/*----------------------------------------------------------------
		 * unix version
		 */
		{
			THREAD_COUNTER	c;

			pthread_mutex_lock(&systhread_cs);
			{
				c = --*cp;
			}
			pthread_mutex_unlock(&systhread_cs);

			return (c);
		}
#  endif
	}
#endif

	/*--------------------------------------------------------------------
	 * no threads available
	 */
	{
		return (--*cp);
	}
}

/*------------------------------------------------------------------------
 * syscount_exch() - exchange a counter for another
 */
THREAD_COUNTER syscount_exch (THREAD_COUNTER *cp, THREAD_COUNTER c)
{
#if V_USE_THREADS
	if (systhread_avail())
	{
#  if V_WINDOWS
		/*----------------------------------------------------------------
		 * windows version
		 */
		{
			return InterlockedExchange(cp, c);
		}
#  else
		/*----------------------------------------------------------------
		 * unix version
		 */
		{
			THREAD_COUNTER	old_c;

			pthread_mutex_lock(&systhread_cs);
			{
				old_c = *cp;
				*cp   = c;
			}
			pthread_mutex_unlock(&systhread_cs);

			return (old_c);
		}
#  endif
	}
#endif

	/*--------------------------------------------------------------------
	 * no threads available
	 */
	{
		THREAD_COUNTER	old_c;

		old_c = *cp;
		*cp   = c;

		return (old_c);
	}
}
