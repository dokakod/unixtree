/*------------------------------------------------------------------------
 * view control block
 */
#ifndef VFCB_H
#define VFCB_H

/*------------------------------------------------------------------------
 * buffer block
 */
#define V_MAX_LINEWIDTH	256
#define	V_BLKSIZE		1024

struct buffer
{
	long			b_addr;
	int				b_len;
	unsigned int	b_mode;
	unsigned char *	b_data;
};
typedef struct buffer BUFFER;

/*------------------------------------------------------------------------
 * values for b_mode
 */
#define B_SHORT		0x0001
#define B_DIRTY		0x0002

/*------------------------------------------------------------------------
 * view control block
 */
struct vfcb
{
	WINDOW *		v_win;
	char			v_pathname[MAX_PATHLEN];
	char			v_dispname[MAX_PATHLEN];
	char			v_linkname[MAX_PATHLEN];
	int				v_fd;
	unsigned int	v_mode;
	int				v_blksize;
	long			v_filesize;
	long			v_top_pos;
	long			v_bot_pos;
	long			v_cur_pos;
	long			v_srch_pos;

	REGEXP *		v_re;
	const char *	v_fnd_ptr;
	const char *	v_end_ptr;

	unsigned char *	v_cur_ptr;

	int				v_tab_width;
	int				v_ascii_width;
	int				v_hex_width;
	int				v_dump_width;
	int				v_num_buffers;
	int				v_empty_no;

	BUFFER *		v_curbuf;
	BUFFER *		v_buffers;
};
typedef struct vfcb VFCB;

/*------------------------------------------------------------------------
 * values for v_mode
 */
#define V_MODE				0x00ff			/* mask to get mode bits */
#define V_ASCII				0x0001			/* display in ASCII format */
#define V_DUMP				0x0002			/* display in dump  format */
#define V_HEX				0x0003			/* display in hex   format */
#define V_WRAP				0x0004			/* display in wrap  format */

#define V_MASKED			0x0100			/* mask the bits in the display */
#define V_AUTO				0x0200			/* auto select display type */
#define V_HSCROLLBAR		0x0400			/* display scroll bar at bottom */
#define V_VSCROLLBAR		0x0800			/* display scroll bar at left */
#define V_SYM				0x1000			/* treat symlink as symlink */
#define V_EXPANDED			0x2000			/* file expanded prior to view */

#define V_RDWR				0x4000			/* file opened rd/wr */

/*------------------------------------------------------------------------
 * defines for commands
 */
#define V_CMD_UP			KEY_UP
#define V_CMD_DOWN			KEY_DOWN
#define V_CMD_LEFT			KEY_LEFT
#define V_CMD_RIGHT			KEY_RIGHT
#define V_CMD_HOME			KEY_HOME
#define V_CMD_END			KEY_END
#define V_CMD_PGUP			KEY_PGUP
#define V_CMD_PGDN			KEY_PGDN

#define V_CMD_ASCII			'a'
#define V_CMD_DUMP			'd'
#define V_CMD_HEX			'h'
#define V_CMD_WRAP			'w'
#define V_KEEP_MODE			'k'
#define V_CMD_TOGGLE_MASK	'm'

#define V_CMD_REDISPLAY		'r'
#define V_CMD_REDISP_NR		'R'
#define V_CMD_OPEN_RO		'o'
#define V_CMD_OPEN_RW		'O'
#define V_CMD_CLOSE			'c'

#define V_SRCH_NEXT			'n'
#define V_SRCH_PREV			'p'

#endif /* VFCB_H */
