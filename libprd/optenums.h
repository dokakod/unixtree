/*------------------------------------------------------------------------
 * These are the command options for the various option vars
 *
 * Note that for all entries, the default value is 0.
 */
#ifndef OPTENUMS_H
#define OPTENUMS_H

/*------------------------------------------------------------------------
 * sort type (opt_sort_type)
 */
#define sort_name		0			/* sort by name			*/
#define sort_ext		1			/* sort by extension	*/
#define sort_date		2			/* sort by date			*/
#define sort_inode		3			/* sort by inode number	*/
#define sort_links		4			/* sort by # links		*/
#define sort_size		5			/* sort by size			*/
#define sort_unsorted	6			/* unsorted				*/

/*------------------------------------------------------------------------
 * sort order (opt_sort_order)
 */
#define sort_asc		0			/* sort ascending		*/
#define sort_desc		1			/* sort descending		*/

/*------------------------------------------------------------------------
 * screen type (opt_screen_type)
 */
#define screen_auto		0			/* auto-detect mode		*/
#define screen_color	1			/* force to color		*/
#define screen_mono		2			/* force to mono		*/

/*------------------------------------------------------------------------
 * file display format (opt_file_fmt)
 */
#define fmt_fname1		0			/* 1 col name only		*/
#define fmt_fname3		1			/* 3 col name only		*/
#define fmt_fsize		2			/* name & size			*/
#define fmt_fdate		3			/* name & date			*/
#define fmt_fperms		4			/* name & perms			*/

/*------------------------------------------------------------------------
 * directory display format (opt_dir_format)
 */
#define fmt_dname		0			/* name in tree fmt		*/
#define fmt_dsize		1			/* name & size			*/
#define fmt_ddate		2			/* name & date			*/
#define fmt_dperms		3			/* name & perms			*/

/*------------------------------------------------------------------------
 * logging method (opt_logging_method)
 */
#define log_manual		0			/* log dir only			*/
#define log_auto		1			/* auto-log sub-dirs	*/

/*------------------------------------------------------------------------
 * time format (opt_time_fmt)
 */
#define time_12			0			/* 12-hour format		*/
#define time_24			1			/* 24-hour format		*/

/*------------------------------------------------------------------------
 * date format (opt_date_fmt)
 */
#define date_mdy		0			/* mo/dy/yr				*/
#define date_dmy		1			/* dy/mo/yr				*/
#define date_ymd		2			/* yr/mo/dy				*/

/*------------------------------------------------------------------------
 * cpio hdr format (opt_cpio_hdr_fmt)
 */
#define cpio_hdr_chr	0			/* chr hdr (old -c)		*/
#define cpio_hdr_bin	1			/* bin hdr				*/
#define cpio_hdr_bbs	2			/* byte-swapped bin hdr	*/
#define cpio_hdr_asc	3			/* ASC hdr (new -c)		*/
#define cpio_hdr_crc	4			/* crc hdr (-h crc)		*/

/*------------------------------------------------------------------------
 * tar hdr format (opt_tar_hdr_fmt)
 */
#define tar_hdr_old		0			/* old format			*/
#define tar_hdr_ustar	1			/* ustar (posix) format	*/

/*------------------------------------------------------------------------
 * viewer format (opt_view_fmt)
 */
#define view_fmt_auto	0			/* auto-detect mode		*/
#define view_fmt_ascii	1			/* ascii mode			*/
#define view_fmt_wrap	2			/* ascii - wrap lines	*/
#define view_fmt_hex	3			/* hex format			*/
#define view_fmt_dump	4			/* dump format			*/

/*------------------------------------------------------------------------
 * compression type (opt_comp_type)
 */
#define comp_lzw		0			/* ala "compress"		*/
#define comp_gzip		1			/* ala "gzip"			*/
#define comp_pack		2			/* ala "pack"			*/

/*------------------------------------------------------------------------
 * file/dir date display (opt_date_type)
 */
#define date_modified	0			/* use m_time			*/
#define date_accessed	1			/* use a_time			*/
#define date_created	2			/* use c_time			*/

/*------------------------------------------------------------------------
 * autolog option (opt_autolog)
 */
#define auto_disabled	0			/* disabled				*/
#define auto_check		1			/* mark  if changed		*/
#define auto_log		2			/* relog if changed		*/

#endif /* OPTENUMS_H */
