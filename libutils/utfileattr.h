/*------------------------------------------------------------------------
 * fileattr struct
 */
#ifndef FILEATTR_H
#define FILEATTR_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * special color codes
 */
#define FA_RAINBOW	((attr_t)0x01)			/* display rainbow			*/
#define FA_RANDOM	((attr_t)0x02)			/* display random colors	*/

/*------------------------------------------------------------------------
 * numeric spec
 */
#define FA_COMP_LE	1
#define FA_COMP_LT	2
#define FA_COMP_GE	3
#define FA_COMP_GT	4
#define FA_COMP_EQ	5
#define FA_COMP_NE	6

struct num_spec
{
	int		comp;
	int		data;
};
typedef struct num_spec NUM_SPEC;

/*------------------------------------------------------------------------
 * permission spec
 */
struct perm_spec
{
	int		and_mask;
	int		or_mask;
};
typedef struct perm_spec PERM_SPEC;

/*------------------------------------------------------------------------
 * type spec
 */
#define FA_TYPE_REG		1
#define FA_TYPE_CHR		2
#define FA_TYPE_BLK		3
#define FA_TYPE_LINK	4
#define FA_TYPE_FIFO	5
#define FA_TYPE_PIPE	6
#define FA_TYPE_SOCK	7
#define FA_TYPE_DOOR	8
#define FA_TYPE_UNK		9

struct type_spec
{
	int		allb;
	int		type;
	int		mode;
};
typedef struct type_spec TYPE_SPEC;

/*------------------------------------------------------------------------
 * name spec
 */
struct name_spec
{
	int		allb;
	char	spec[MAX_PATHLEN];
};
typedef struct name_spec NAME_SPEC;

/*------------------------------------------------------------------------
 * fileattr struct
 */
#define FA_ATTR_ATIME	1
#define FA_ATTR_CTIME	2
#define FA_ATTR_MTIME	3
#define FA_ATTR_INODE	4
#define FA_ATTR_LINKS	5
#define FA_ATTR_SIZE	6
#define FA_ATTR_PERMS	7
#define FA_ATTR_OWNER	8
#define FA_ATTR_GROUP	9
#define FA_ATTR_TYPE	10
#define FA_ATTR_NAME	11
#define FA_ATTR_DIR		12

struct fileattr
{
	int			type;
	int			more;
	attr_t		attr;
	union
	{
		NUM_SPEC	nums;
		PERM_SPEC	perms;
		TYPE_SPEC	types;
		NAME_SPEC	names;
	}			data;
};
typedef struct fileattr FILEATTR;

/*------------------------------------------------------------------------
 * function prototypes
 */
extern BLIST *	fileattr_load (const char *path, int *errs);
extern int		fileattr_save (const char *path, BLIST *flist);
extern BLIST *	fileattr_free (BLIST *flist);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* FILEATTR_H */
