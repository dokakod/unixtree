/*------------------------------------------------------------------------
 * tag info struct
 */
#ifndef TAGINFO_H
#define TAGINFO_H

struct tag_info
{
	int		tag_choice;
	time_t	tag_date;
	int		tag_size;
	int		tag_perm_mask;
	int		tag_perm_tag;
	int		tag_order;
	int		tag_uid;
	int		tag_gid;
	int		tag_inode;
	int		tag_link_count;
	int		tag_type;
	int		tag_brk;
	BLIST *	tag_node;
};
typedef struct tag_info TAG_INFO;

#endif /* TAGINFO_H */
