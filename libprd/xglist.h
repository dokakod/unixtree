/*------------------------------------------------------------------------
 * menu lists
 */
#ifndef XGLIST_H
#define XGLIST_H

/*------------------------------------------------------------------------
 * menu list indexes
 */
#define XGL_NONE				0
#define XGL_CFG_CHR				1
#define XGL_CFG_FILE			2
#define XGL_CFG_NUM				3
#define XGL_CFG_STR				4
#define XGL_ARCH_NAME			5
#define XGL_EXEC				6
#define XGL_EXT_REL_PATH		7
#define XGL_GOTO_FILE			8
#define XGL_GOTO_DIR			9
#define XGL_NODE_PATH			10
#define XGL_REL_BASE			11
#define XGL_BATCH_FILENAME		12
#define XGL_EDIT_FILE			13
#define XGL_PRT_FILE			14
#define XGL_SRCH_STR			15
#define XGL_TEMPLATE			16
#define XGL_FILE_TO_COPY		17
#define XGL_CMD					18
#define XGL_DATE				19
#define XGL_DEST				20
#define XGL_DEVICE				21
#define XGL_FILESPEC			22
#define XGL_GROUP				23
#define XGL_HOST				24
#define XGL_INODE				25
#define XGL_NEWNAME				26
#define XGL_NLINK				27
#define XGL_NODE				28
#define XGL_OWNER				29
#define XGL_PATTERN				30
#define XGL_RDIR				31
#define XGL_RELNODE				32
#define XGL_RENAME				33
#define XGL_SIZE				34
#define XGL_USER				35
#define XGL_PRINT_FILE_NAME		36
#define XGL_SYM_NAMES			37
#define XGL_TREESPEC			38
#define XGL_DIFFNAMES			39
#define XGL_DIFFRPT				40
#define XGL_GET_PERMS			41
#define XGL_SET_PERMS			42

#define XGL_NUM_ENTRIES			42

/*------------------------------------------------------------------------
 * menu list entry
 */
struct xg_entry
{
	int		keep;			/* TRUE to keep this entry		*/
	char *	line;			/* pointer to text				*/
};
typedef struct xg_entry XG_ENTRY;

/*------------------------------------------------------------------------
 * menu list
 */
struct xg_list
{
	int		num;			/* number of entries in list	*/
	BLIST *	lines;			/* list of XG_ENTRY entries		*/
};
typedef struct xg_list XG_LIST;

#endif /* XG_LIST_H */
