/*------------------------------------------------------------------------
 * tree definition header file
 */
#ifndef TREE_H
#define	TREE_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * tree struct
 */
typedef struct tree TREE;
struct tree
{
	TREE *	parent;				/* pointer to parent tree	*/
	TREE *	left;				/* pointer to left   tree	*/
	TREE *	right;				/* pointer to right  tree	*/
	void *	id;					/* pointer to data			*/
};

/*------------------------------------------------------------------------
 * debugging interface
 *
 * These macros exist to get better debugging info from our malloc
 * interface.  Without these macros, all our alloc/free entries are
 * reported as coming from tnew() & tfree().  With these macros, these
 * entries are reported as coming from the files which called these
 * routines, thus we can actually find where an errant alloc/free was
 * actually done.
 */
#ifdef V_MALLOC_FILEINFO
#  define TNEW(a)			tnew		(a,		__FILE__, __LINE__)
#  define TFREE(l,d)		tfree		(l,d,	__FILE__, __LINE__)
#  define TSCRAP(l,d)		tscrap		(l,d,	__FILE__, __LINE__)
#  define TDELETEL(l,e,d)	tdeletel	(l,e,d,	__FILE__, __LINE__)
#  define TDELETER(l,e,d)	tdeleter	(l,e,d,	__FILE__, __LINE__)
#else
#  define TNEW(a)			tnew		(a,		0, 0)
#  define TFREE(l,d)		tfree		(l,d,	0, 0)
#  define TSCRAP(l,d)		tscrap		(l,d,	0, 0)
#  define TDELETEL(l,e,d)	tdeletel	(l,e,d,	0, 0)
#  define TDELETER(l,e,d)	tdeleter	(l,e,d,	0, 0)
#endif

/*------------------------------------------------------------------------
 * tree macros
 */
#define tparent(t)			( (t)->parent )
#define tleft(t)			( (t)->left )
#define tright(t)			( (t)->right )
#define tid(t)				( (t)->id )

/*------------------------------------------------------------------------
 * tree functions
 */
typedef void	TREE_FUNC	(TREE *t, void *a);
typedef int		TREE_COMP	(const void *p1, const void *p2);

extern TREE *	tnew		(void *a,
								const char *file, int line);
extern TREE *	tfree		(TREE *t, int delid,
								const char *file, int line);
extern TREE *	tscrap		(TREE *t, int delid,
								const char *file, int line);

extern TREE *	tdeletel	(TREE *t, TREE *e, int delid,
								const char *file, int line);
extern TREE *	tdeleter	(TREE *t, TREE *e, int delid,
								const char *file, int line);

extern TREE *	troot		(TREE *t);
extern int		tdepth		(TREE *t);

extern TREE *	tappendl	(TREE *t, TREE *e);
extern TREE *	tappendr	(TREE *t, TREE *e);

extern void		tapplyfl	(TREE *t, TREE_FUNC *f, void *a);
extern void		tapplyfr	(TREE *t, TREE_FUNC *f, void *a);

extern int		tcountl		(TREE *t);
extern int		tcountr		(TREE *t);

extern TREE *	tfindl		(TREE *t, void *a);
extern TREE *	tfindr		(TREE *t, void *a);

extern TREE *	tnthl		(TREE *t, int n);
extern TREE *	tnthr		(TREE *t, int n);

extern TREE *	tinsertl	(TREE *t, TREE *e);
extern TREE *	tinsertr	(TREE *t, TREE *e);

extern TREE *	tinsertlf	(TREE *t, TREE *e, TREE_COMP *f);
extern TREE *	tinsertrf	(TREE *t, TREE *e, TREE_COMP *f);

extern TREE *	tlastl		(TREE *t);
extern TREE *	tlastr		(TREE *t);

extern int		tindexl		(TREE *t, TREE *e);
extern int		tindexr		(TREE *t, TREE *e);

extern TREE *	tremovel	(TREE *t, TREE *e);
extern TREE *	tremover	(TREE *t, TREE *e);

extern void		tpreorder	(TREE *t, TREE_FUNC *f, void *a);
extern void		tendorder	(TREE *t, TREE_FUNC *f, void *a);
extern void		tpostorder	(TREE *t, TREE_FUNC *f, void *a);

extern TREE *	tsortr		(TREE *t, TREE_COMP *f);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif	/* TREE_H */
