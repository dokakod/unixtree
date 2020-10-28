/*------------------------------------------------------------------------
 *	Misc system defines
 */
#ifndef SYS_DEFS_H
#define SYS_DEFS_H

/*------------------------------------------------------------------------
 * Misc defines used everywhere
 */
#ifndef TRUE
#  define	TRUE				(1)
#endif

#ifndef FALSE
#  define	FALSE				(0)
#endif

#ifndef	EXIT_SUCCESS
#  define	EXIT_SUCCESS		0
#endif

#ifndef EXIT_FAILURE
#  define	EXIT_FAILURE		1
#endif

/*------------------------------------------------------------------------
 * Windows or UNIX ?
 */
#if defined(_MSC_VER) || defined(_WIN32)

	/*--------------------------------------------------------------------
	 * Windows
	 */
#  ifndef V_WINDOWS
#    define V_WINDOWS			TRUE
#  endif

#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN	TRUE
#  endif

#  ifndef STRICT
#    define STRICT				TRUE
#  endif

#else

	/*--------------------------------------------------------------------
	 * Unix
	 */
#  ifndef V_UNIX
#    define V_UNIX				TRUE
#  endif

#endif

/*------------------------------------------------------------------------
 * Max lengths for pathnames & filenames
 */
#ifndef MAX_PATHLEN
#define MAX_PATHLEN				1024
#endif

#ifndef MAX_FILELEN
#define MAX_FILELEN				1024
#endif

/*------------------------------------------------------------------------
 * macro to get offset of a field in a struct
 */
#ifndef X_OFFSET_OF
#define	X_OFFSET_OF(t,f)		( (int)&(((t *)0)->f) )
#endif

/*------------------------------------------------------------------------
 * macro to get size of a table
 */
#ifndef X_TBLSIZE
#define X_TBLSIZE(t)			( sizeof(t) / sizeof(*t) )
#endif

/*------------------------------------------------------------------------
 * Macros to swap 2-byte & 4-byte words, and they DO NOT care about any kind
 * of current byte ordering -- they just swap the bytes unconditionally.
 *
 * Note that these macros expect a pointer to the word being swapped.
 */
#ifndef X_SWAP2
#define X_SWAP2(foo)	*(unsigned short *) (foo) = \
						( \
							(( *(unsigned short *)(foo) ) >> 8) | \
							(( *(unsigned short *)(foo) ) << 8)   \
						)
#endif

#ifndef X_SWAP4
#define X_SWAP4(foo)	*(unsigned int *) (foo) = \
						( \
							(( *(unsigned int *)(foo)              ) >> 24) | \
							(( *(unsigned int *)(foo) & 0x00ff0000 ) >>  8) | \
							(( *(unsigned int *)(foo) & 0x0000ff00 ) <<  8) | \
							(( *(unsigned int *)(foo)              ) << 24)   \
						)
#endif

/*------------------------------------------------------------------------
 * The following macros will conditionally swap 2-byte & 4-byte integers,
 * depending on the machine architecture.
 *
 * These macros assume a "standard" representation of little-endian.
 */
#ifndef X_SWAP2_CONDITIONAL
#define	X_SWAP2_CONDITIONAL(x)	{ \
									int _one_ = 1; \
									if (*(unsigned char *)(&_one_) != 1) \
										X_SWAP2(x); \
								}
#endif

#ifndef X_SWAP4_CONDITIONAL
#define	X_SWAP4_CONDITIONAL(x)	{ \
									int _one_ = 1; \
									if (*(unsigned char *)(&_one_) != 1) \
										X_SWAP4(x); \
								}
#endif

/*------------------------------------------------------------------------
 * macros for "local" and "standard" integer representations.
 */
#ifndef X_STD2
#define	X_STD2(x)		X_SWAP2_CONDITIONAL(x)
#endif

#ifndef X_LCL2
#define	X_LCL2(x)		X_SWAP2_CONDITIONAL(x)
#endif

#ifndef X_STD4
#define	X_STD4(x)		X_SWAP4_CONDITIONAL(x)
#endif

#ifndef X_LCL4
#define	X_LCL4(x)		X_SWAP4_CONDITIONAL(x)
#endif

/*------------------------------------------------------------------------
 * UNUSED PARAMETERS / VARIABLE
 *
 * These allow us to mark a parameter or variable as not being used and
 * suppress the error/warning messages about them. This seems to work
 * for both PC-Lint and the Microsoft compiler in maximum-error mode. We
 * have separate macros (that currently do the same thing) because we
 * may need to tailor this behavior differently.
 */
#ifndef X_UNUSED_PARAMETER
#define X_UNUSED_PARAMETER(p)	(void)(p)
#endif

#ifndef X_UNUSED_VARIABLE
#define X_UNUSED_VARIABLE(v)	(void)(v)
#endif

#endif /* SYS_DEFS_H */
