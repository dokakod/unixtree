/*------------------------------------------------------------------------
 * MOUNT_INFO info
 */
#ifndef OSMNT_H
#define OSMNT_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * MOUNT_INFO struct
 */
struct mount_entry
{
	char	mt_dev[MAX_PATHLEN];	/* name of device		*/
	char	mt_dir[MAX_PATHLEN];	/* directory mounted on	*/
	int		mt_flags;				/* flags - see below	*/
	dev_t	st_dev;					/* from stat struct		*/
};
typedef struct mount_entry MOUNT_INFO;

/*------------------------------------------------------------------------
 * flags
 */
#define MT_READONLY		0x01		/* read only file system		*/
#define MT_REMOTE		0x02		/* remote file system			*/
#define MT_NOCASE		0x04		/* case-insensitive file system	*/

/*------------------------------------------------------------------------
 * open a mount table
 *
 *	This routine opens the mount-table and returns an opaque pointer
 *	or NULL if it cannot be opened. The returned value points to a
 *	platform-dependent structure.
 */
extern void *	os_mnt_open		(void);

/*------------------------------------------------------------------------
 * close a mount table
 *
 *	This routine closes the mount-table.
 *	If the struct pointer was allocated on the heap by os_mnt_open(),
 *	then this routine should free it back.
 *
 *	It is passed the opaque pointer from os_mnt_open().
 */
extern void		os_mnt_close	(void *md);

/*------------------------------------------------------------------------
 * read a mount table entry
 *
 *	This routine reads the next entry from the mount-table and fills
 *	in the MOUNT_INFO entry.
 *
 *	It should return 0 if an entry was read and -1
 *	if all entries have been read or if an error occurred.
 *
 *	It is passed the opaque pointer from os_mnt_open().
 */
extern int		os_mnt_read		(void *md, MOUNT_INFO *m);

/*------------------------------------------------------------------------
 * read a mount table entry
 *
 *	This routine returns the amount of free-space in the file-system
 *	containing the specified path.  The return value is
 *	the number of 1K blocks or -1 if it cannot be determined.
 */
extern long		os_mnt_space	(const char *path);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* OSMNT_H */
