/*------------------------------------------------------------------------
 *	defines for the resource file
 */
#ifndef RES_H
#define RES_H

#include <stdio.h>

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * resource file name
 */
#define RES_FILENAME	"resfile"

/*------------------------------------------------------------------------
 * "magic" number for identifying a resource file
 */
#define RES_MAGIC			0x01171957

/*------------------------------------------------------------------------
 * res entry types
 */
#define R_TYPE_STR			0		/* char string data	*/
#define R_TYPE_NUM			1		/* numeric data		*/
#define R_TYPE_KEY			2		/* key values		*/

/*------------------------------------------------------------------------
 * resource type struct
 */
struct res_type
{
	int				type_code;		/* resource type					*/
	const char *	type_name;		/* name of type						*/
	const char *	type_decl;		/* type of declaration				*/
};
typedef struct res_type RES_TYPE;

/*------------------------------------------------------------------------
 * resource list
 */
struct res_list_entry
{
	char			name[32];			/* entry name						*/
	char			caps[32];			/* entry name as all caps			*/
	char			type[32];			/* entry type name					*/
	const RES_TYPE *rt;					/* pointer to RES_TYPE entry		*/
};
typedef struct res_list_entry RES_LIST_ENTRY;

struct res_list
{
	char			lang_name[32];	/* language name					*/
	int				num_ents;		/* number of entries				*/
	RES_LIST_ENTRY	entries[20];	/* room for 20 entries				*/
};
typedef struct res_list RES_LIST;

/*------------------------------------------------------------------------
 * resource file header
 */
struct res_hdr
{
	unsigned int	magic;			/* magic number for resource file	*/
	int				num_ents;		/* number of entries after res data	*/
	char			name[32];		/* name of this file				*/
	char			file[32];		/* filename							*/
};
typedef struct res_hdr RES_HDR;

/*------------------------------------------------------------------------
 * resource file entry header (stored in file)
 */
struct res_entry_hdr
{
	int				num;			/* number of entries in array		*/
	int				size;			/* size of strings (0 for no array)	*/
	int				type;			/* type of entry					*/
	char			name[8];		/* name of entry					*/
};
typedef struct res_entry_hdr RES_ENTRY_HDR;

/*------------------------------------------------------------------------
 * resource file entry (stored in memory)
 */
struct res_entry_data
{
	void *			array;			/* data array						*/
	int				num;			/* number of entries in array		*/
	int				type;			/* type of entry					*/
	char			name[8];		/* name of entry					*/
};
typedef struct res_entry_data RES_ENTRY_DATA;

/*------------------------------------------------------------------------
 * resource file item
 */
struct res_item
{
	char			name[32];		/* name of data item				*/
	void *			data;			/* numeric value or ptr to string	*/
};
typedef struct res_item RES_ITEM;

/*------------------------------------------------------------------------
 * struct holding entire resource file
 */
struct res_file
{
	RES_HDR				hdr;			/* header						*/
	int					loaded;			/* TRUE if loaded from a file	*/
	int					has_names;		/* TRUE if names in entries		*/
	RES_ENTRY_DATA *	entries;		/* array of entries				*/
};
typedef struct res_file RES_FILE;

/*------------------------------------------------------------------------
 * restool functions
 */
extern const RES_TYPE *	res_find_by_type	(int type);
extern const RES_TYPE *	res_find_by_name	(const char *name);

extern int				res_list_load		(FILE *fp,
												RES_LIST *rl,
												char *msgbuf);

extern int				res_text_to_ch		(RES_LIST_ENTRY *rle,
												char *msgbuf);

/*------------------------------------------------------------------------
 * run-time functions
 */
extern int				res_hdr_read		(FILE *fp,
												RES_HDR *res_hdr);

extern int				res_entry_read		(FILE *fp,
												RES_ENTRY_DATA *res_data,
												int load_names);

extern int				res_hdr_write		(FILE *fp,
												const RES_HDR *res_hdr);

extern int				res_entry_write		(FILE *fp,
												const RES_ENTRY_DATA *res_data);

extern RES_FILE *		res_file_load		(const char *res_path,
												int load_names);

extern int				res_file_save		(const char *res_path,
												const RES_FILE *rf);

extern void				res_file_free		(RES_FILE *rf);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* RES_H */
