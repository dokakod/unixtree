/*------------------------------------------------------------------------
 * archive stuff
 */
#ifndef ARCH_H
#define ARCH_H

/*------------------------------------------------------------------------
 * misc defines
 */
#define ARCH_REL_DIR_NAME	"..."	/* name for relative directory */
#define ARCH_MIN_BLKS		10		/* min blocks to split */

/*------------------------------------------------------------------------
 * tar header definitions
 */
#define TBLOCK		512		/* length of tar header and data blocks */
#define TNAMLEN		100		/* maximum length for tar file names */
#define TMODLEN		8		/* length of mode field */
#define TUIDLEN		8		/* length of uid field */
#define TGIDLEN		8		/* length of gid field */
#define TSIZLEN		12		/* length of size field */
#define TTIMLEN		12		/* length of modification time field */
#define TCRCLEN		8		/* length of header checksum field */

#define T_TYPE_ARG	'\0'
#define T_TYPE_REG	'0'
#define T_TYPE_LNK	'1'
#define T_TYPE_SYM	'2'
#define T_TYPE_CHR	'3'
#define T_TYPE_BLK	'4'
#define T_TYPE_DIR	'5'
#define T_TYPE_FFO	'6'

#define TAR_MAGIC	"ustar"		/* magic field in new tar hdr */

struct old_tar_header
{
	char name[TNAMLEN];			/* 100 zero padded */
	char mode[TMODLEN];			/*   8 format "oooooo 0" */
	char uid[TUIDLEN];			/*   8 format "oooooo 0" */
	char gid[TGIDLEN];			/*   8 format "oooooo 0" */
	char size[TSIZLEN];			/*  12 format "ooooooooooo " */
	char mtime[TTIMLEN];		/*  12 format "ooooooooooo " */
	char chksum[TCRCLEN];		/*   8 format "oooooo0 " */
	char linkflag;				/*   1 zero or ascii digit */
	char linkname[TNAMLEN];		/* 100 zero padded */
	char extno[4];				/*   4 format "ooo0" left-adjusted */
	char extotal[4];			/*   4 format "ooo0" left-adjusted */
	char efsize[TSIZLEN];		/*  12 format "ooooooooooo " */
};

struct new_tar_header
{
	char name[TNAMLEN];			/* 100 zero padded */
	char mode[TMODLEN];			/*   8 format "oooooo 0" */
	char uid[TUIDLEN];			/*   8 format "oooooo 0" */
	char gid[TGIDLEN];			/*   8 format "oooooo 0" */
	char size[TSIZLEN];			/*  12 format "ooooooooooo " */
	char mtime[TTIMLEN];		/*  12 format "ooooooooooo " */
	char chksum[TCRCLEN];		/*   8 format "oooooo0 " */
	char linkflag;				/*   1 zero or ascii digit */
	char linkname[TNAMLEN];		/* 100 zero padded */
	char magic[6];				/*   6  format "ustar0" */
	char version[2];			/*   2  format "00" */
	char uname[32];				/*  32  format "nnnn0" */
	char gname[32];				/*  32  format "nnnn0" */
	char devmajor[8];			/*   8  format */
	char devminor[8];			/*   8  format */
	char prefix[155];
	char extno[1];
	char extotal[1];
	char totsize[10];
};

union tblock
{
	char					dummy[TBLOCK];
	struct old_tar_header	obuf;
	struct new_tar_header	nbuf;
};

/*------------------------------------------------------------------------
 * bar header definitions
 */
#define BBLOCK		512		/* length of bar header and data blocks */
#define BNAMLEN		100		/* maximum length for bar file names */
#define BMODLEN		8		/* length of mode field */
#define BUIDLEN		8		/* length of uid field */
#define BGIDLEN		8		/* length of gid field */
#define BSIZLEN		12		/* length of size field */
#define BTIMLEN		12		/* length of modification time field */
#define BCRCLEN		8		/* length of header checksum field */
#define BDEVLEN		8		/* length of header rdev field */

#define B_TYPE_DIR	0
#define B_TYPE_REG	'0'
#define B_TYPE_LNK	'1'
#define B_TYPE_SYM	'2'
#define B_TYPE_SPC	'3'

#define BAR_MAGIC	"V"

struct bar_header
{
	char mode[BMODLEN];			/*   8 format "ooooooo0" */
	char uid[BUIDLEN];			/*   8 format "oooooo 0" */
	char gid[BGIDLEN];			/*   8 format "oooooo 0" */
	char size[BSIZLEN];			/*  12 format "ooooooooooo " */
	char mtime[BTIMLEN];		/*  12 format "ooooooooooo " */
	char chksum[BCRCLEN];		/*   8 format "dddddd0 " */
	char rdev[BDEVLEN];			/*   8 format "ooooooo " */
	char linkflag;				/*   1 zero or ascii digit */
	char bar_magic[2];			/*   2 "26" */
	char volume_num[4];			/*   4 binary */
	char compressed;			/*   1 binary */
	char date[BTIMLEN];			/*  12 format "oooooooooo " */
	char start_of_name[1];
};

union bblock
{
	char				dummy[BBLOCK];
	struct bar_header	dbuf;
};

/*------------------------------------------------------------------------
 * cpio header definitions
 */

/* Magic numbers */

#define CMN_BIN	(unsigned short)0x71c7	/* Magic Num for Binary hdr */
#define CMN_BBS	(unsigned short)0xc771	/* Magic Num for Byte-Swap hdr */
#define CMS_CHR	"070707"				/* Magic Str for CHR (old -c) hdr */
#define CMS_ASC	"070701"				/* Magic Str for ASC (new -c) hdr */
#define CMS_CRC	"070702"				/* Magic Str for CRC (-h crc) hdr */
#define CMS_LEN	6						/* Magic Str LENgth */

/* Various header and field lengths */

#define HNAMLEN	256			/* max filename len for BIN and CHR headers */
#define ENAMLEN	1024		/* max filename len for ASC and CRC headers */

/* cpio trailer definition */

#define CPIO_TRAILER		"TRAILER!!!"

/* cpio binary header definition */

struct cpio_bin_hdr
{
	unsigned short	h_magic;			/* magic number field */
	short			h_dev;				/* file system of file */
	unsigned short	h_ino;				/* inode of file */
	unsigned short	h_mode;				/* modes of file */
	unsigned short	h_uid;				/* uid of file */
	unsigned short	h_gid;				/* gid of file */
	short			h_nlink;			/* number of links to file */
	short			h_rdev;				/* maj/min numbers for special files */
	short			h_mtime[2];			/* modification time of file */
	short			h_namesz;			/* length of filename */
	short			h_filesz[2];		/* size of file */
	char			h_name[HNAMLEN];	/* filename */
};

#define BINSZ	X_OFFSET_OF(struct cpio_bin_hdr, h_name[0])	/* size - name */

/* cpio CHR header format */

struct cpio_chr_hdr
{
	char			c_magic[CMS_LEN];
	char			c_dev[6];
	char			c_ino[6];
	char			c_mode[6];
	char			c_uid[6];
	char			c_gid[6];
	char			c_nlink[6];
	char			c_rdev[6];
	char			c_mtime[11];
	char			c_namesz[6];
	char			c_filesz[11];
	char			c_name[HNAMLEN];
};

#define CHRSZ	X_OFFSET_OF(struct cpio_chr_hdr, c_name[0])	/* size - name */

/* cpio ASC and CRC header format */

struct cpio_asc_hdr
{
	char			e_magic[CMS_LEN];
	char			e_ino[8];
	char			e_mode[8];
	char			e_uid[8];
	char			e_gid[8];
	char			e_nlink[8];
	char			e_mtime[8];
	char			e_filesz[8];
	char			e_maj[8];
	char			e_min[8];
	char			e_rmaj[8];
	char			e_rmin[8];
	char			e_namesz[8];
	char			e_chksum[8];
	char			e_name[ENAMLEN];
};

#define ASCSZ	X_OFFSET_OF(struct cpio_asc_hdr, e_name[0])	/* size - name */

union cpio_hdr
{
	struct cpio_bin_hdr bh;
	struct cpio_chr_hdr ch;
	struct cpio_asc_hdr ah;
};

#define cpio_hdr_type_bin		1
#define cpio_hdr_type_chr		2
#define cpio_hdr_type_asc		3
#define cpio_hdr_type_crc		4

/*------------------------------------------------------------------------
 * ARCH_DEV struct
 */
struct arch_dev
{
	char	d_name[MAX_FILELEN];			/* archive[n]		*/
	char	d_desc[MAX_FILELEN];			/* description		*/
	char	d_device[MAX_PATHLEN];			/* device			*/
	int		d_blkfactor;					/* blk factor		*/
	int		d_numblks;						/* num blks on dev	*/
	int		d_tape;							/* TRUE if tape		*/
};
typedef struct arch_dev ARCH_DEV;

/*------------------------------------------------------------------------
 * ARCH_INFO struct
 */
struct arch_info
{
	int		a_blksize;				/* fixed size of archive record */
	int		a_blkfactor;			/* recs per block */
	int		a_bufsize;				/* size of block */
	int		a_volsize;				/* in blks (not k) */
	int		a_dev_type;				/* archive device type */
	int		a_type;					/* archive type */

	char	a_name[ARCH_NAME_LEN];			/* user name of archive */
	char	a_device[MAX_PATHLEN];			/* filename or device name */
	char	a_pattern[MAX_PATHLEN];			/* name pattern */
	char	a_rel_name[MAX_PATHLEN];		/* relative name */
	char	a_temp_name[MAX_PATHLEN];		/* temp name buffer */
	char	a_file_name[MAX_FILELEN];		/* name of current file */
	char	a_ext_rel_path[MAX_PATHLEN];	/* rel dir to extract into */

	char	a_save_comp[MAX_PATHLEN];		/* save compress name */
	char	a_temp_comp[MAX_PATHLEN];		/* temp compress name */
	int		a_comp_dev;						/* temp name is a device */

	char *	a_buffer;				/* buffer - malloced elsewhere */
	char *	a_recp;					/* ptr to cur rec in a_buffer */
	BLIST *	a_links;				/* list of links */

	int		a_open_flags;			/* open flags for open() */
	int		a_fd;					/* file descriptor for i/o */
	int		a_blk_count;			/* current block number */
	int		a_rec_count;			/* record number in block */
	int		a_recs_read;			/* # recs read in a block */
	int		a_volno;				/* volume number */
	int		a_do_auto;				/* auto replace existing vols */
	int		a_name_only;			/* use filename only */
	int		a_compressed;			/* archive contains compressed ents */
	int		a_hdr_type;				/* archive header type */
	int		a_init;					/* TRUE if initialized */
	int		a_ioerr;				/* TRUE if ioerr */
	time_t	a_time;					/* time hdr written */

	char *			cpio_buffer;
	int				cpio_bufsize;
	long			cpio_buf_addr;
	int				cpio_buf_pos;
	int				cpio_hdr_len;
	union cpio_hdr	cpio_hdr;
	long			cpio_volsize;
	int				cpio_buf_len;
	int				cpio_hdr_is_bbs;		/* hdr is byte-swappped */
};
typedef struct arch_info ARCH_INFO;

#endif /* ARCH_H */
