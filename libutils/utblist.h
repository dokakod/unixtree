/*------------------------------------------------------------------------
 * blist definition header file
 */
#ifndef UTBLIST_H
#define	UTBLIST_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * blist struct
 */
typedef struct blist BLIST;
struct blist
{
	BLIST *	next;				/* pointer to next entry in list	*/
	BLIST *	prev;				/* pointer to prev entry in list	*/
	void *	id;					/* pointer to data					*/
};

/*------------------------------------------------------------------------
 * debugging interface
 *
 * These macros exist to get better debugging info from our malloc
 * interface.  Without these macros, all our alloc/free entries are
 * reported as coming from bnew() & bfree().  With these macros, these
 * entries are reported as coming from the files which called these
 * routines, thus we can actually find where an errant alloc/free was
 * actually done.
 */
#ifdef V_MALLOC_FILEINFO
#  define BNEW(a)			bnew	(a,		__FILE__, __LINE__)
#  define BFREE(l,d)		bfree	(l,d,	__FILE__, __LINE__)
#  define BSCRAP(l,d)		bscrap	(l,d,	__FILE__, __LINE__)
#  define BDELETE(l,e,d)	bdelete	(l,e,d,	__FILE__, __LINE__)
#  define BDUP(l)			bup		(l,		__FILE__, __LINE__)
#else
#  define BNEW(a)			bnew	(a,		0, 0)
#  define BFREE(l,d)		bfree	(l,d,	0, 0)
#  define BSCRAP(l,d)		bscrap	(l,d,	0, 0)
#  define BDELETE(l,e,d)	bdelete	(l,e,d,	0, 0)
#  define BDUP(l)			bdup	(l,		0, 0)
#endif

/*------------------------------------------------------------------------
 * blist macros
 */
#define bnext(b)			( (b)->next )
#define bprev(b)			( (b)->prev )
#define bid(b)				( (b)->id )

/*------------------------------------------------------------------------
 * blist functions
 */
typedef void	BLIST_FUNC	(BLIST *l, void *a);
typedef int		BLIST_COMP	(const void *p1, const void *p2);

extern BLIST *	bnew		(void *a,
								const char *file, int line);
extern BLIST *	bfree		(BLIST *l, int delid,
								const char *file, int line);
extern BLIST *	bscrap		(BLIST *l, int delid,
								const char *file, int line);
extern BLIST *	bdelete		(BLIST *l, BLIST *e, int delid,
								const char *file, int line);
extern BLIST *	bdup		(BLIST *l,
								const char *file, int line);

extern BLIST *	bnextc		(BLIST *l);
extern BLIST *	bprevc		(BLIST *l);
extern BLIST *	bappend		(BLIST *l, BLIST *e);
extern void		bapplyf		(BLIST *l, BLIST_FUNC *f, void *a);
extern int		bcount		(BLIST *l);
extern BLIST *	bfind		(BLIST *l, void *a);
extern int		bindex		(BLIST *l, BLIST *e);
extern BLIST *	binsert		(BLIST *l, BLIST *e);
extern BLIST *	binsertf	(BLIST *l, BLIST *e, BLIST_COMP *f);
extern BLIST *	blast		(BLIST *l);
extern BLIST *	bnth		(BLIST *l, int n);
extern BLIST *	bremove		(BLIST *l, BLIST *e);
extern BLIST *	bsort		(BLIST *l, BLIST_COMP *f);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif	/* UTBLIST_H */
