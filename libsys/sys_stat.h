/*------------------------------------------------------------------------
 *	This is our front-end interface to the <sys/stat.h> file.
 *
 *	We use this because there is a host of macros defined in many versions
 *	of this file but are not universally supported or defined.
 *	If they are not supported or defined, then we define them here.
 *
 *	These macros are easy ways to deal with file type and permission
 *	mode bits without hardcoding.  The following are examples of these:
 *
 *	Mask bits for the file type:
 *
 *		S_IFMT			mask for all file types
 *
 *		S_IFREG			... is a regular file
 *		S_IFDIR			... is a directory
 *		S_IFCHR			... is a character special file
 *		S_IFBLK			... is a block special file
 *		S_IFIFO			... is a FIFO
 *		S_IFNAM			... is a named pipe
 *		S_IFSOCK		... is a socket
 *		S_IFLNK			... is a symbolic link
 *		S_IFDOOR		... is a door
 *
 *	Parameterized macros for the file types returning TRUE if..
 *
 *		S_ISREG(m)		... is a regular file
 *		S_ISDIR(m)		... is a directory
 *		S_ISCHR(m)		... is a character special file
 *		S_ISBLK(m)		... is a block special file
 *		S_ISFIFO(m)		... is a FIFO
 *		S_ISNAM(m)		... is a named pipe
 *		S_ISSOCK(m)		... is a socket
 *		S_ISLNK(m)		... is a symbolic link
 *		S_ISDOOR(m)		... is a door
 *
 *	Mask bits for permissions:
 *
 *		S_ISUID			exec:	set owner id
 *		S_ISGID			exec:	set group id
 *		S_ISVTX			exec:	save text (sticky bit)
 *
 *		S_IRUSR			owner:	read  permission
 *		S_IWUSR			owner:	write permission
 *		S_IXUSR			owner:	exec  permission
 *
 *		S_IRGRP			group:	read  permission
 *		S_IWGRP			group:	write permission
 *		S_IXGRP			group:	exec  permission
 *
 *		S_IROTH			other:	read  permission
 *		S_IWOTH			other:	write permission
 *		S_IXOTH			other:	exec  permission
 *
 *	Other defines set (if applicable):
 *
 *		X_IFLNK			system supports symbolic links
 */
#ifndef SYS_STAT_H
#define SYS_STAT_H

#include <sys/types.h>
#include <sys/stat.h>

/*------------------------------------------------------------------------
 * Define ANSI standard types for Windows (not very ANSI std, are they?)
 */
#if V_WINDOWS
	typedef unsigned short	mode_t;
	typedef short			uid_t;
	typedef short			gid_t;
	typedef short			nlink_t;
#endif

/*------------------------------------------------------------------------
 * Note whether this system defines symbolic links
 */
#ifdef S_IFLNK
#define X_IFLNK		TRUE			/* system has symlinks */
#endif

/*------------------------------------------------------------------------
 * Define any missing file types
 *
 * Note: these are NOT bit values, but numeric values within the mask
 */
#ifndef   S_IFMT
#  define S_IFMT		0xf000			/* file-type mask		*/
#endif

#ifndef   S_IFDIR
#  define S_IFDIR		0x4000			/* directory			*/
#endif

#ifndef   S_IFCHR
#  define S_IFCHR		0x2000			/* character special	*/
#endif

#ifndef   S_IFBLK
#  define S_IFBLK		0x6000			/* block special		*/
#endif

#ifndef   S_IFREG
#  define S_IFREG		0x8000			/* regular				*/
#endif

#ifndef   S_IFIFO
#  define S_IFIFO		0x1000			/* FIFO					*/
#endif

#ifndef   S_IFLNK
#  define S_IFLNK		0xa000			/* symbolic link		*/
#endif

#ifndef   S_IFSOCK
#  define S_IFSOCK		0xc000			/* socket				*/
#endif

#ifndef   S_IFNAM
#  define S_IFNAM		0x5000			/* named pipe			*/
#endif

#ifndef   S_IFDOOR
#  define S_IFDOOR		0xd000			/* door					*/
#endif

/*------------------------------------------------------------------------
 * Define any missing file-type macros
 *
 * Note: Windows uses a 0 for reg files.
 */
#ifndef   S_ISREG
#  define S_ISREG(m)	( (((m) & S_IFMT) == S_IFREG) || \
						  (((m) & S_IFMT) == 0) )
#endif

#ifndef   S_ISBLK
#  define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
#endif

#ifndef   S_ISCHR
#  define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
#endif

#ifndef   S_ISDIR
#  define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#endif

#ifndef   S_ISFIFO
#  define S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
#endif

#ifndef   S_IFNAM
#  define S_ISNAM(m)	(((m) & S_IFMT) == S_IFNAM)
#endif

#ifndef   S_ISLNK
#  define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
#endif

#ifndef   S_ISSOCK
#  define S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)
#endif

#ifndef   S_ISDOOR
#  define S_ISDOOR(m)	(((m) & S_IFMT) == S_IFDOOR)
#endif

/*------------------------------------------------------------------------
 * Define any missing permission bits
 */
#ifndef   S_ISUID
#  define S_ISUID		0x0800			/* exec:	set owner id			*/
#endif

#ifndef   S_ISGID
#  define S_ISGID		0x0400			/* exec:	set group id			*/
#endif

#ifndef   S_ISVTX
#  define S_ISVTX		0x0200			/* exec:	save text (sticky bit)	*/
#endif

#ifndef   S_IRUSR
#  define S_IRUSR		0x0100			/* owner:	read  permission		*/
#endif

#ifndef   S_IWUSR
#  define S_IWUSR		0x0080			/* owner:	write permission		*/
#endif

#ifndef   S_IXUSR
#  define S_IXUSR		0x0040			/* owner:	exec  permission		*/
#endif

#ifndef   S_IRGRP
#  define S_IRGRP		0x0020			/* group:	read  permission		*/
#endif

#ifndef   S_IWGRP
#  define S_IWGRP		0x0010			/* group:	write permission		*/
#endif

#ifndef   S_IXGRP
#  define S_IXGRP		0x0008			/* group:	exec  permission		*/
#endif

#ifndef   S_IROTH
#  define S_IROTH		0x0004			/* other:	read  permission		*/
#endif

#ifndef   S_IWOTH
#  define S_IWOTH		0x0002			/* other:	write permission		*/
#endif

#ifndef   S_IXOTH
#  define S_IXOTH		0x0001			/* other:	exec  permission		*/
#endif

/*------------------------------------------------------------------------
 * Define combo permissions (for anyone)
 */
#ifndef   S_IRWXU
#  define S_IRWXU		(S_IRUSR | S_IWUSR | S_IXUSR)
#endif

#ifndef   S_IRWXG
#  define S_IRWXG		(S_IRGRP | S_IWGRP | S_IXGRP)
#endif

#ifndef   S_IRWXO
#  define S_IRWXO		(S_IROTH | S_IWOTH | S_IXOTH)
#endif

#ifndef   S_PERMS
#  define S_PERMS		(S_ISUID | S_ISGID | S_ISVTX | \
						 S_IRWXU | S_IRWXG | S_IRWXO)
#endif

#endif /* SYS_STAT_H */
