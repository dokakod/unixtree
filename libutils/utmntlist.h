/*------------------------------------------------------------------------
 * header file for mount list routines
 */
#ifndef MNTLIST_H
#define MNTLIST_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * function prototypes
 */
extern BLIST *	ml_get_mounts			(BLIST *ml);
extern BLIST *	ml_free_mounts			(BLIST *ml);
extern int		ml_get_num_mounts		(BLIST *ml);

extern MOUNT_INFO *	ml_get_mount_ent_by_num	(BLIST *ml, int n);
extern MOUNT_INFO *	ml_get_mount_ent_by_dev	(BLIST *ml, int dev);

extern long		ml_get_free_by_dev		(BLIST *ml, int dev);

extern int		ml_is_remote_filesys	(BLIST *ml, int dev);
extern int		ml_is_readonly_filesys	(BLIST *ml, int dev);
extern int		ml_is_nocase_filesys	(BLIST *ml, int dev);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* MNTLIST_H */
