/*------------------------------------------------------------------------
 * list definition header file
 */
#ifndef LIST_H
#define	LIST_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * list struct
 */
typedef struct list LIST;
struct list
{
	LIST *	next;				/* pointer to next entry in list	*/
	void *	id;					/* pointer to data					*/
};

/*------------------------------------------------------------------------
 * debugging interface
 *
 * These macros exist to get better debugging info from our malloc
 * interface.  Without these macros, all our alloc/free entries are
 * reported as coming from lnew() & lfree().  With these macros, these
 * entries are reported as coming from the files which called these
 * routines, thus we can actually find where an errant alloc/free was
 * actually done.
 */
#ifdef V_MALLOC_FILEINFO
#  define LNEW(a)			lnew	(a,		__FILE__, __LINE__)
#  define LFREE(l,d)		lfree	(l,d,	__FILE__, __LINE__)
#  define LSCRAP(l,d)		lscrap	(l,d,	__FILE__, __LINE__)
#  define LDELETE(l,e,d)	ldelete	(l,e,d,	__FILE__, __LINE__)
#  define LDUP(l)			ldup	(l,		__FILE__, __LINE__)
#else
#  define LNEW(a)			lnew	(a,		0, 0)
#  define LFREE(l,d)		lfree	(l,d,	0, 0)
#  define LSCRAP(l,d)		lscrap	(l,d,	0, 0)
#  define LDELETE(l,e,d)	ldelete	(l,e,d,	0, 0)
#  define LDUP(l)			ldup	(l,		0, 0)
#endif

/*------------------------------------------------------------------------
 * list macros
 */
#define lnext(l)			( (l)->next )
#define lid(l)				( (l)->id )

/*------------------------------------------------------------------------
 * list functions
 */
typedef void	LIST_FUNC	(LIST *l, void *a);
typedef int		LIST_COMP	(const void *p1, const void *p2);

extern LIST *	lnew		(void *a,
								const char *file, int line);
extern LIST *	lfree		(LIST *l, int delid,
								const char *file, int line);
extern LIST *	lscrap		(LIST *l, int delid,
								const char *file, int line);
extern LIST *	ldelete		(LIST *l, LIST *e, int delid,
								const char *file, int line);
extern LIST *	ldup		(LIST *l,
								const char *file, int line);

extern LIST *	lappend		(LIST *l, LIST *e);
extern void		lapplyf		(LIST *l, LIST_FUNC *f, void *a);
extern int		lcount		(LIST *l);
extern LIST *	lfind		(LIST *l, void *a);
extern int		lindex		(LIST *l, LIST *e);
extern LIST *	linsert		(LIST *l, LIST *e);
extern LIST *	llast		(LIST *l);
extern LIST *	lnth		(LIST *l, int n);
extern LIST *	lremove		(LIST *l, LIST *e);
extern LIST *	lsort		(LIST *l, LIST_COMP *f);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif	/* LIST_H */
