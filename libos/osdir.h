/*------------------------------------------------------------------------
 * directory reading routines
 *
 *	These are the interface structures used for the directory
 *	processing routines.  Since the pointed-to structure is
 *	so system-dependent, we have decided to make it opaque and
 *	tell the user that it's void* to prevent shenanigans. This
 *	also means we don't have to include all the <dir.h> stuff
 *	unless we're actually building the real library code.
 *
 */
#ifndef OSDIR_H
#define OSDIR_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * opaque dir struct
 */
typedef void	UTDIR;

/*------------------------------------------------------------------------
 * function prototypes
 */
extern UTDIR *	os_dir_open		(const char *path);
extern int		os_dir_close	(UTDIR *utdir);
extern int		os_dir_read		(UTDIR *utdir, char *filename);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* OSDIR_H */
