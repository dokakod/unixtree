/*------------------------------------------------------------------------
 * Defines for the structures used
 */
#ifndef STRUCTS_H
#define STRUCTS_H

/*------------------------------------------------------------------------
 * misc defines
 */
#define NUM_SPLIT_WINS		2		/* number of split windows */

#define OUR_MAXFN			15		/* min length of filename display */

#define FULLNAME(p)			((p)->fullname)

/*------------------------------------------------------------------------
 * command mode values
 */
#define m_dir				1
#define m_file				2
#define m_showall			3
#define m_level_showall		4
#define m_global			5
#define m_tag_showall		6
#define m_tag_global		7

/*------------------------------------------------------------------------
 * command sub-mode values
 */
#define m_reg				1
#define m_alt				2
#define m_tag				3

/*------------------------------------------------------------------------
 * NODE block
 */
#define N_NODENAME_LEN	64
#define N_MAGIC			0x01234567

struct nblk
{
	char 			root_name[MAX_PATHLEN];
	char			node_name[N_NODENAME_LEN];
	int				node_type;
	int				node_flags;
	void *			node_sub_blk;

	TREE *			root;
	TREE *			cur_dir_tree;

	BLIST *			dir_list;
	BLIST *			dir_disp_list[NUM_SPLIT_WINS];
	BLIST *			showall_flist;
	BLIST *			showall_mlist[NUM_SPLIT_WINS];
	BLIST *			link_list;

	int				node_total_count;
	int				node_total_bytes;
	int				node_match_count;
	int				node_match_bytes;
	int				node_tagged_count;
	int				node_tagged_bytes;
	int				node_tagged_blocks;
};
typedef struct nblk NBLK;

/*------------------------------------------------------------------------
 * node types (node_type)
 */
#define N_FS		0		/* mounted file system */
#define N_ARCH		1		/* archive file system */
#define N_FTP		2		/* FTP     file system */

/*------------------------------------------------------------------------
 * flag bits in node structure (node_flags)
 */
#define N_READONLY	1

/*------------------------------------------------------------------------
 * node-sub-info blocks
 */
#define ARCH_NAME_LEN	12

struct ablk
{
	char			arch_name[ARCH_NAME_LEN];
	char			arch_devname[MAX_PATHLEN];
	char			arch_tmpname[MAX_PATHLEN];
	char			arch_typename[ARCH_NAME_LEN];
	int				arch_type;
	int				arch_dev_type;
	int				arch_blkfactor;
	int				arch_volsize;
	int				arch_size;
	int				arch_numvols;
};
typedef struct ablk ABLK;

/*------------------------------------------------------------------------
 * archive types (arch_type)
 */
#define A_TAR			1		/* tar archive */
#define A_BAR			2		/* bar archive */
#define A_CPIO			3		/* cpio archive */
#define A_ZTAR			4		/* GNU ztar archive */

/*------------------------------------------------------------------------
 * values for arch_dev_type
 */
#define A_DEV_FILE		0		/* device is a file */
#define A_DEV_SEEK		1		/* device is seekable */
#define A_DEV_NOSEEK	2		/* device is not seekable */

/*------------------------------------------------------------------------
 * DIRECTORY block
 */
struct dblk
{
	char			name[OUR_MAXFN];
	char *			fullname;
	struct stat		stbuf;
	time_t			log_time;
	off_t			dir_size;
	int				flags;
	int				hidden;

	TREE *			dir_tree;
	BLIST *			flist;
	BLIST *			mlist[NUM_SPLIT_WINS];
};
typedef	struct dblk DBLK;

/*------------------------------------------------------------------------
 * flag bits in dir structure
 */
#define D_NOT_LOGGED	0x01
#define D_CHANGED		0x02
#define D_CANT_LOG		0x04
#define D_PHONY			0x10

/*------------------------------------------------------------------------
 * FILE block
 */
struct fblk
{
	char			name[OUR_MAXFN];
	char *			fullname;
	struct stat		stbuf;
	int				archive_vol;
	off_t			archive_loc;
	int				tagged;
	DBLK *			dir;
	char *			sym_name;
	int				sym_mode;
};
typedef struct fblk FBLK;

/*------------------------------------------------------------------------
 * UID/GID info block
 */
#define UG_LEN		9

struct usr_grp
{
	int				ug_id;
	char			ug_name[UG_LEN];
};
typedef struct usr_grp USR_GRP;

/*------------------------------------------------------------------------
 * window display block
 */
struct dstat
{
	WINDOW *		cur_path_line;
	WINDOW *		cur_dir_win;
	WINDOW *		cur_dest_dir_win;
	WINDOW *		large_file_win;
	WINDOW *		small_file_win;
	WINDOW *		cur_file_win;

	WINDOW *		cur_av_path_line;
	WINDOW *		cur_av_mode_win;
	WINDOW *		cur_av_file_win;
	WINDOW *		cur_av_view_win;
	WINDOW *		cur_av_dest_dir_win;

	WINDOW *		save_file_win;
	WINDOW *		save_path_line;
	WINDOW *		save_dest_win;

	NBLK *			cur_root;
	TREE *			cur_dir_tree;
	DBLK *			cur_dir;
	FBLK *			cur_file;
	FBLK *			av_file_displayed;
	VFCB *			vfcb;

	char			path_name[MAX_PATHLEN];

	BLIST *			global_flist;
	BLIST *			global_mlist;
	BLIST *			cur_node;

	BLIST *			first_dir;
	BLIST *			top_dir;
	BLIST *			dir_cursor;

	BLIST *			first_file;
	BLIST *			base_file;
	BLIST *			top_file;
	BLIST *			file_cursor;

	int				dir_scroll_bar_displayed;
	int				file_scroll_bar_displayed;
	int				dir_scroll_bar;
	int				file_scroll_bar;

	int				command_mode;
	int				cmd_sub_mode;
	int				save_cmd_mode;
	int				save_file_fmt;

	int				dir_total_count;
	int				dir_total_bytes;
	int				dir_match_count;
	int				dir_match_bytes;
	int				dir_tagged_count;
	int				dir_tagged_bytes;
	int				dir_tagged_blocks;

	char			file_spec[MAX_PATHLEN];
	int				file_spec_pres;
	int				invert_file_spec;

	int				in_dest_dir;
	int				in_small_window;

	int				dir_scroll;
	int				dir_fmt;
	int				dir_disp_len;

	int				file_fmt;
	int				file_line_width;
	int				file_disp_len;

	int				numdirs;
	int				max_dir_line;
	int				max_dir_level;
	int				cur_dir_no;
	int				cur_dir_line;

	int				numfiles;
	int				max_file_line;
	int				max_file_col;
	int				cur_file_line;
	int				cur_file_col;
	int				cur_file_no;

	int				tag_mask;
};
typedef struct dstat DSTAT;

#endif /* STRUCTS_H */
